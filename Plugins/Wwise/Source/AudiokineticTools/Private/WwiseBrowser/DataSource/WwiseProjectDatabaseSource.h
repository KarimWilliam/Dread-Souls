#pragma once

#include "IWwiseBrowserDataSource.h"
#include "Misc/TextFilter.h"
#include "Wwise/Ref/WwiseRefCollections.h"

struct FWwiseMetadataBasicReference;

struct WwiseItemTreePath
{
	FString HierarchyName;
	FString RootFolder;
	TArray<FString> IntermediateFolders;
	FString ItemName;
};

class FWwiseProjectDatabaseDataSource : IWwiseBrowserDataSource
{
public:
	DECLARE_DELEGATE(FOnWwiseProjectDatasbaseDataSourceRefreshed)

	virtual ~FWwiseProjectDatabaseDataSource() override;

	FText GetProjectName();

	// IWwiseBrowserDataSource

	virtual bool Init() override;
	virtual void ConstructTree(bool bShouldRefresh) override;
	virtual FWwiseTreeItemPtr ConstructTreeRoot(EWwiseItemType::Type Type) override;
	virtual int32 LoadChildren(const FGuid& InParentId, const FString& InParentPath, TArray<FWwiseTreeItemPtr>& OutChildren) override;
	virtual int32 LoadChildren(FWwiseTreeItemPtr InParentItem) override;
	virtual int32 GetChildItemCount(const FWwiseTreeItemPtr& InParentItem) override;
	virtual FWwiseTreeItemPtr GetRootItem(EWwiseItemType::Type RootType) override;
	virtual FWwiseTreeItemPtr LoadFilteredRootItem(EWwiseItemType::Type ItemType, TSharedPtr<StringFilter> CurrentFilter) override;

	FWwiseTreeItemPtr FindItemFromPath(const FString& InCurrentItemPath);
	FWwiseTreeItemPtr FindItem(const FWwiseTreeItemPtr InItem);

	FOnWwiseProjectDatasbaseDataSourceRefreshed ProjectDatabaseDataSourceRefreshed;

private:


	void BuildEvents(const WwiseEventGlobalIdsMap& Events);
	void BuildBusses(const WwiseBusGlobalIdsMap& Busses);
	void BuildAuxBusses(const WwiseAuxBusGlobalIdsMap& AuxBusses);
	void BuildAcousticTextures(const WwiseAcousticTextureGlobalIdsMap& AcousticTextures);
	void BuildStateGroups(const WwiseStateGroupGlobalIdsMap& StateGroups);
	void BuildStates(const WwiseStateGlobalIdsMap& States);
	void BuildSwitchGroups(const WwiseSwitchGroupGlobalIdsMap& SwitchGroups);
	void BuildSwitches(const WwiseSwitchGlobalIdsMap& Switches);
	void BuildGameParameters(const WwiseGameParameterGlobalIdsMap& GameParameters);
	void BuildTriggers(const WwiseTriggerGlobalIdsMap& Triggers);
	void BuildEffectShareSets(const WwisePluginShareSetGlobalIdsMap& EffectShareSets);
	bool ParseTreePath(const FString& ObjectPath, WwiseItemTreePath& OutItemPath);

	bool BuildFolderHierarchy(const FWwiseMetadataBasicReference& WwiseItem, EWwiseItemType::Type ItemType,
		const FWwiseTreeItemPtr
		CurrentRootFolder);

	void CopyTree(FWwiseTreeItemPtr SourceTreeItem, FWwiseTreeItemPtr DestTreeItem);

	void FilterTree(FWwiseTreeItemPtr TreeItem, TSharedPtr<StringFilter> SearchFilter);

	bool IsContainer(EWwiseItemType::Type ItemType) const;
	
	/** Root items, one for each type of Wwise object */
	FCriticalSection RootItemsLock;
	TArray< FWwiseTreeItemPtr > RootItems;

	// Map of all tree items
	TMap<FGuid, FWwiseTreeItemPtr> AllValidTreeItemsByGuid;

	// Container paths along the Browser Tree
	TMap<FString, FWwiseTreeItemPtr> NodesByPath;

	// Allows some optimization if we have already applied a search
	FString OldFilterText;

	FDelegateHandle OnDatabaseUpdateCompleteHandle;
};
