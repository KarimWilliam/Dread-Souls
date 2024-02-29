/*******************************************************************************
The content of this file includes portions of the proprietary AUDIOKINETIC Wwise
Technology released in source code form as part of the game integration package.
The content of this file may not be used without valid licenses to the
AUDIOKINETIC Wwise Technology.
Note that the use of the game engine is subject to the Unreal(R) Engine End User
License Agreement at https://www.unrealengine.com/en-US/eula/unreal
 
License Usage
 
Licensees holding valid licenses to the AUDIOKINETIC Wwise Technology may use
this file in accordance with the end user license agreement provided with the
software or, alternatively, in accordance with the terms contained
in a written agreement between you and Audiokinetic Inc.
Copyright (c) 2023 Audiokinetic Inc.
*******************************************************************************/

#include "Wwise/WwiseReconcileCommandlet.h"

#include "AkAudioEvent.h"
#include "AkAudioType.h"
#include "AkAuxBus.h"
#include "AkEffectShareSet.h"
#include "AkInitBank.h"
#include "AkSettings.h"
#include "AkStateValue.h"
#include "AkSwitchValue.h"
#include "AkTrigger.h"
#include "AkUnrealAssetDataHelper.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "FileHelpers.h"
#include "ObjectTools.h"
#include "AkAssetFactories.h"
#include "PackageTools.h"
#include "Wwise/Stats/Reconcile.h"
#include "Wwise/WwiseReconcile.h"
#include "Wwise/WwiseProjectDatabase.h"

static constexpr auto ModesParam = TEXT("modes");
static constexpr auto CreateOption = TEXT("create");
static constexpr auto UpdateOption = TEXT("update");
static constexpr auto DeleteOption = TEXT("delete");
static constexpr auto AllOption = TEXT("all");
static constexpr auto HelpOption = TEXT("help");

UWwiseReconcileCommandlet::UWwiseReconcileCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;

	HelpDescription = TEXT("Commandlet to generate Wwise SoundBanks.");

	HelpParamNames.Add(ModesParam);
	HelpParamDescriptions.Add(FString::Format(TEXT("Comma separated list of operations to perform on assets.\n"
		"{0}: Create Unreal assets from the Generated SoundBanks\n"
		"{1}: Update existing Unreal assets. This updates the asset name as well as its metadata.\n"
		"{2}: Delete Unreal assets that no longer exist in the Generated SoundBanks\n"
		"{3}: Fully reconcile Unreal assets"),
		{CreateOption, UpdateOption, DeleteOption, AllOption}));

	HelpParamNames.Add("?, help");
	HelpParamDescriptions.Add(TEXT("Display help"));

	HelpUsage = FString::Format(TEXT("<Editor.exe> <path_to_uproject> -run=WwiseReconcileCommandlet -modes={0},{1},{2},{3}"), {CreateOption, DeleteOption, UpdateOption, AllOption});
}

int32 UWwiseReconcileCommandlet::Main(const FString& Params)
{
	int32 Result = 0;

	TMap<FString, FString> ParsedParams;
	ParseCommandLine(*Params, CmdTokens, CmdSwitches, ParsedParams);

	if( Params.Contains(TEXT("?")) || Params.Contains(HelpOption) )
	{
		PrintHelp();
		return Result;
	}

	EWwiseReconcileOperationFlags ReconcileOperationFlags = EWwiseReconcileOperationFlags::None;

	if (ParsedParams.Contains(ModesParam))
	{
		FString ModeStr = ParsedParams.FindRef("modes");

		TArray<FString> Modes;
		if (ModeStr.Contains(TEXT(",")))
		{
			ModeStr.ParseIntoArray(Modes, TEXT(","), true);
		}
		else
		{
			Modes.Add(ModeStr);
		}

		if (Modes.Num() == 0)
		{
			PrintHelp();
			Result = -1;
			return Result;
		}

		if (Modes.Contains(CreateOption))
		{
			ReconcileOperationFlags |= EWwiseReconcileOperationFlags::Create;
		}
		if (Modes.Contains(UpdateOption))
		{
			ReconcileOperationFlags |= EWwiseReconcileOperationFlags::UpdateExisting;
		}
		if (Modes.Contains(DeleteOption))
		{
			ReconcileOperationFlags |= EWwiseReconcileOperationFlags::Delete;
		}
		if (Modes.Contains(AllOption))
		{
			ReconcileOperationFlags |= EWwiseReconcileOperationFlags::All;
		}
	}

	if (ReconcileOperationFlags == EWwiseReconcileOperationFlags::None)
	{
		UE_LOG(LogWwiseReconcile, Error, TEXT("No Reconcile mode specified"))
		PrintHelp();
		Result = -1;
		return Result;
	}

	GetAllAssets();

	GetAssetChanges(ReconcileOperationFlags);

	for (const auto& Asset : AssetsToCreate)
	{
		UE_LOG(LogWwiseReconcile, Verbose, TEXT("New Asset %s will be created."), *Asset.WwiseAnyRef.GetName().ToString());
	}

	for (const auto& Asset: AssetsToUpdate)
	{
		UE_LOG(LogWwiseReconcile, Verbose, TEXT("Asset %s will be updated."), *Asset.GetFullName());
	}

	for (const auto& Asset: AssetsToDelete)
	{
		UE_LOG(LogWwiseReconcile, Verbose, TEXT("Asset %s of type %s will be deleted."), *Asset.GetFullName(), *AkUnrealAssetDataHelper::GetAssetClassName(Asset).ToString());
	}

	int NumAssetsToReconcile = AssetsToCreate.Num() + AssetsToUpdate.Num() + AssetsToDelete.Num();

	if (NumAssetsToReconcile > 0)
	{
		UE_LOG(LogWwiseReconcile, Display, TEXT("Reconciling %d Wwise Asset(s)..."), NumAssetsToReconcile)

			if (!ReconcileAssets())
			{
				Result = -1;
				UE_LOG(LogWwiseReconcile, Error, TEXT("Failed to reconcile assets. Check the log for details"));
			}
	}

	else
	{
		UE_LOG(LogWwiseReconcile, Display, TEXT("No Wwise Assets to Reconcile..."))
	}

	UE_LOG(LogWwiseReconcile, Display, TEXT("Finished reconciling Wwise Assets..."))
	

	return Result;
}

void UWwiseReconcileCommandlet::PrintHelp()
{
	UE_LOG(LogWwiseReconcile, Display, TEXT("%s"), *HelpDescription);
	UE_LOG(LogWwiseReconcile, Display, TEXT("Usage: %s"), *HelpUsage);
	UE_LOG(LogWwiseReconcile, Display, TEXT("Parameters:"));
	for (int32 i = 0; i < HelpParamNames.Num(); ++i)
	{
		UE_LOG(LogWwiseReconcile, Display, TEXT("\t- %s: %s"), *HelpParamNames[i], *HelpParamDescriptions[i]);
	}
	UE_LOG(LogWwiseReconcile, Display, TEXT("For more information, see %s"), *HelpWebLink);
}

void UWwiseReconcileCommandlet::GetAllAssets()
{
	AssetRegistryModule = &FModuleManager::LoadModuleChecked<FAssetRegistryModule>(
		"AssetRegistry");

	AssetToolsModule = &FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");

	if (!AssetToolsModule)
	{
		UE_LOG(LogWwiseReconcile, Error, TEXT("Could not load the AssetTools Module"));
		return;
	}

	if (!AssetRegistryModule)
	{
		UE_LOG(LogWwiseReconcile, Error, TEXT("Could not load the AssetRegistry Module"));
		return;
	}

#if UE_5_1_OR_LATER
	AssetRegistryModule->Get().GetAssetsByClass(UAkAudioType::StaticClass()->GetClassPathName(), Assets, true);
#else
	AssetRegistryModule->Get().GetAssetsByClass(UAkAudioType::StaticClass()->GetFName(), Assets, true);
#endif

	GuidAssetMap.Empty();
	for (const FAssetData& AssetData : Assets)
	{
		if (UAkAudioType* AkAudioAsset = Cast<UAkAudioType>(AssetData.GetAsset()))
		{
			auto AssetGuid = AkAudioAsset->GetWwiseGuid();
			// Exclude the Init bank, and invalid assets
			if (AssetGuid.IsValid())
			{
				GuidAssetMap.Add(AssetGuid, AssetData);
			}

			else if (!AkUnrealAssetDataHelper::AssetOfType<UAkInitBank>(AssetData))
			{
				InvalidAssets.Add(AssetData);
			}
		}
	}


	ProjectDatabase = FWwiseProjectDatabase::Get();
	if (UNLIKELY(!ProjectDatabase))
	{
		UE_LOG(LogWwiseReconcile, Error, TEXT("Could not load project database"));
	}

	ProjectDatabase->UpdateDataStructure();

	GuidWwiseMetadataMap.Empty();

}

void UWwiseReconcileCommandlet::GetAssetChanges(EWwiseReconcileOperationFlags OperationFlags)
{
	ProjectDatabase = FWwiseProjectDatabase::Get();

	ProjectDatabase->UpdateDataStructure();

	if (UNLIKELY(!ProjectDatabase || !ProjectDatabase->IsProjectDatabaseParsed()))
	{
		UE_LOG(LogWwiseReconcile, Error, TEXT("No data loaded from Wwise project database"));
		return;
	}

	FWwiseDataStructureScopeLock DataStructure(*ProjectDatabase);

	TSet<FGuid> FoundAssets;

	// Check to make sure there are no issues getting data for the CurrentPlatform
	if (DataStructure.GetSoundBanks().Num() == 0)
	{
		FName PlatformName = DataStructure.GetCurrentPlatform().GetPlatformName();
		UE_LOG(LogWwiseReconcile, Error, TEXT("No data loaded from Wwise project database for the curent platform %s"), *PlatformName.ToString());
		return;
	}

	if (DataStructure.GetCurrentPlatformData()->Guids.Num() == 0)
	{
		FName PlatformName = DataStructure.GetCurrentPlatform().GetPlatformName();
		UE_LOG(LogWwiseReconcile, Error, TEXT("No data loaded from Wwise project database for the curent platform %s"), *PlatformName.ToString());
		return;
	}

	for (const auto& WwiseRef : DataStructure.GetCurrentPlatformData()->Guids)
	{
		const FWwiseAnyRef WwiseRefValue = WwiseRef.Value;
		EWwiseRefType RefType = WwiseRefValue.GetType();
		UClass* RefClass = GetUClassFromWwiseRefType(RefType);
		FGuid RefGuid = WwiseRefValue.GetGuid();

		if (!RefClass || FoundAssets.Contains(RefGuid))
		{
			continue;
		}

		FAssetData Asset = GuidAssetMap.FindRef(WwiseRefValue.GetGuid());

		if (!Asset.IsValid())
		{
			if (RefClass && EnumHasAnyFlags(OperationFlags, EWwiseReconcileOperationFlags::Create))
			{
				AssetsToCreate.Add({ WwiseRefValue }); 
			}
		}

		else if (EnumHasAnyFlags(OperationFlags, EWwiseReconcileOperationFlags::UpdateExisting))
		{
			UAkAudioType* AkAudioAsset = Cast<UAkAudioType>(Asset.GetAsset());
			if (RefClass && AkAudioAsset)
			{
				// Ignore SoundBanks
				if (RefType != EWwiseRefType::SoundBanksInfo)
				{

					FName AssetName = AkUnrealAssetDataHelper::GetAssetDefaultName(&WwiseRefValue);

					if (AkAudioAsset->IsAssetOutOfDate(WwiseRefValue))
					{
						AssetsToUpdate.Add(Asset);
					}

					if (AkAudioAsset->GetName() != AssetName.ToString())
					{
						AssetsToRename.Add(Asset);
					}
				}
			}
		}

		FoundAssets.Add(RefGuid);
		GuidAssetMap.Remove(RefGuid);
	}

	if (EnumHasAnyFlags(OperationFlags, EWwiseReconcileOperationFlags::Delete))
	{
		for (const auto& GuidAsset : GuidAssetMap)
		{
			if(!FoundAssets.Contains(GuidAsset.Key))
			{
				AssetsToDelete.Add(GuidAsset.Value);
			}
		}

		for (const auto& Asset : InvalidAssets)
		{
			UAkAudioType* AkAudioAsset = Cast<UAkAudioType>(Asset.GetAsset());

			if (!AkAudioAsset->ObjectIsInSoundBanks())
			{
				AssetsToDelete.Add(Asset);
			}
		}

		GuidAssetMap.Empty();
		InvalidAssets.Empty();
	}

	if (EnumHasAnyFlags(OperationFlags, EWwiseReconcileOperationFlags::UpdateExisting))
	{
		for (const auto& Asset : InvalidAssets)
		{
			UAkAudioType* AkAudioAsset = Cast<UAkAudioType>(Asset.GetAsset());
			if (AkAudioAsset->ObjectIsInSoundBanks())
			{
				AssetsToUpdate.Add(Asset);
				AssetsToRename.Add(Asset);
			}
		}
	}
}


bool UWwiseReconcileCommandlet::ReconcileAssets()
{

	bool Succeeded = true;

	if (AssetsToCreate.Num() != 0 && CreateAssets().Num() == 0)
	{
		UE_LOG(LogWwiseReconcile, Warning, TEXT("No New AkAudioType assets created"));
		Succeeded = false;
	}

	if (AssetsToUpdate.Num() != 0 && UpdateExistingAssets().Num() == 0)
	{
		UE_LOG(LogWwiseReconcile, Warning, TEXT("Failed to consolidate existing AkAudioType assets"));
		Succeeded = false;
	}

	if (AssetsToDelete.Num() != 0 && DeleteAssets() <= 0)
	{
		UE_LOG(LogWwiseReconcile, Warning, TEXT("Failed to delete outdated AkAudioType assets"))
		Succeeded = false;
	}

	return Succeeded;
}

TArray<FAssetData> UWwiseReconcileCommandlet::CreateAssets()
{
	check(IsInGameThread());

	TArray<UPackage*> PackagesToSave;
	TArray<FAssetData> NewAssets;

	for (const auto& Asset : AssetsToCreate)
	{
		const FWwiseAnyRef WwiseRef = Asset.WwiseAnyRef;

		FName AssetName = AkUnrealAssetDataHelper::GetAssetDefaultName(&WwiseRef);
		FString AssetPackagePath = AkUnrealAssetDataHelper::GetAssetDefaultPackagePath(&WwiseRef);
		AssetPackagePath = UPackageTools::SanitizePackageName(AssetPackagePath);

		UClass* NewAssetClass = GetUClassFromWwiseRefType(WwiseRef.GetType());

		if(!NewAssetClass)
		{
			UE_LOG(LogWwiseReconcile, Error, TEXT("Could not determine which type of asset to create for '%s' in '%s'."), *AssetName.ToString(), *AssetPackagePath);
			continue;
		}

		UE_LOG(LogWwiseReconcile, Verbose, TEXT("Creating new asset '%s' in '%s'."), *AssetName.ToString(), *AssetPackagePath);

		UAkAudioType* NewAkAudioObject = Cast<UAkAudioType>(
			AssetToolsModule->Get().CreateAsset(AssetName.ToString(), AssetPackagePath, NewAssetClass, nullptr));

		if (!NewAkAudioObject)
		{
			UE_LOG(LogWwiseReconcile, Error, TEXT("Could not save asset %s"), *AssetName.ToString());
			continue;
		}

		NewAkAudioObject->FillInfo(WwiseRef);

		NewAssets.Add(FAssetData(NewAkAudioObject));

		UE_LOG(LogWwiseReconcile, Verbose, TEXT("Created asset %s"), *AssetName.ToString());

		PackagesToSave.Add(NewAkAudioObject->GetPackage());
	}

	AssetsToCreate.Empty();

	if (!UEditorLoadingAndSavingUtils::SavePackages(PackagesToSave, false))
	{
		UE_LOG(LogWwiseReconcile, Error, TEXT("Could not save packages"));
		return {};
	}

	return NewAssets;
}

TArray<FAssetData> UWwiseReconcileCommandlet::UpdateExistingAssets()
{
	check(IsInGameThread());

	TArray<FAssetRenameData> AssetsToRenameData;
	TArray<UPackage*> PackagesToSave;
	TArray<FAssetData> UpdatedAssets;

	for (const auto& AssetData: AssetsToUpdate)
	{
		if (auto AkAudioAsset = Cast<UAkAudioType>(AssetData.GetAsset()))
		{
			AkAudioAsset->FillInfo();
			FAssetData NewAssetData = FAssetData(AkAudioAsset);
			PackagesToSave.Add(NewAssetData.GetPackage());
			UpdatedAssets.Add(NewAssetData);
			AkAudioAsset->MarkPackageDirty();
		}
	}

	for (const auto& AssetData: AssetsToRename)
	{
		if (auto AkAudioAsset = Cast<UAkAudioType>(AssetData.GetAsset()))
		{
			FName NewAssetName = AkAudioAsset->GetAssetDefaultName();
			FAssetRenameData AssetRenameData(AssetData.GetAsset(), AssetData.PackagePath.ToString(), NewAssetName.ToString());
			AssetsToRenameData.Add(AssetRenameData);
		}
	}

	AssetsToUpdate.Empty();
	AssetsToRename.Empty();

	if (!UEditorLoadingAndSavingUtils::SavePackages(PackagesToSave, false))
	{
		UE_LOG(LogWwiseReconcile, Error, TEXT("Failed to save updated Wwise assets."))
		return {};
	}

	if (AssetsToRenameData.Num() > 0 && !AssetToolsModule->Get().RenameAssets(AssetsToRenameData))
	{
		UE_LOG(LogWwiseReconcile, Error, TEXT("Failed to rename updated Wwise assets."))
	}

	return UpdatedAssets;
}

int32 UWwiseReconcileCommandlet::DeleteAssets()
{
	check(IsInGameThread());

	if (AssetsToDelete.Num() == 0)
	{
		return 0;
	}

	TArray<UObject*> ObjectsToDelete;
	for (const auto& Asset : AssetsToDelete)
	{
		ObjectsToDelete.Add(Asset.GetAsset());
	}

	int32 NumDeletedObjects = ObjectTools::ForceDeleteObjects(ObjectsToDelete, false);

	if (NumDeletedObjects != ObjectsToDelete.Num())
	{
		UE_LOG(LogWwiseReconcile, Error, TEXT("Could not delete assets. Verify that none of the assets are still being referenced."))
	}

	AssetsToDelete.Empty();

	return NumDeletedObjects;

}

UClass* UWwiseReconcileCommandlet::GetUClassFromWwiseRefType(EWwiseRefType RefType)
{
	switch (RefType)
	{
	case EWwiseRefType::Event:
		return UAkAudioEvent::StaticClass();
	case EWwiseRefType::AuxBus:
		return UAkAuxBus::StaticClass();
	case EWwiseRefType::AcousticTexture:
		return UAkAcousticTexture::StaticClass();
	case EWwiseRefType::State:
		return UAkStateValue::StaticClass();
	case EWwiseRefType::Switch:
		return UAkSwitchValue::StaticClass();
	case EWwiseRefType::GameParameter:
		return UAkRtpc::StaticClass();
	case EWwiseRefType::Trigger:
		return UAkTrigger::StaticClass();
	case EWwiseRefType::PluginShareSet:
		return UAkEffectShareSet::StaticClass();
	case EWwiseRefType::None:
		return nullptr;
	default:
			return nullptr;
	}
}
