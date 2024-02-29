// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "ActionComponent.generated.h"

class UAction;
class UActionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActionStateChanged, UActionComponent*, OwningComp, UAction*, Action);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STYLIZEDTEST_API UActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UActionComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tags")
	FGameplayTagContainer ActiveGamePlayTags; //A Tag Container holds a collection of FGameplayTags. All the active tags on the player character

	UFUNCTION(BlueprintCallable, Category="Action")
	void AddAction(AActor* Instigator, TSubclassOf<UAction> ActionClass);

	UFUNCTION(BlueprintCallable, Category="Action")
	void RemoveAction(UAction* ActionToRemove);

	UFUNCTION(BlueprintCallable, Category="Action")
	virtual bool StartActionByName(AActor* Instigator, FName ActionName);

	UFUNCTION(BlueprintCallable, Category="Action")
	virtual bool StopActionByName(AActor* Instigator, FName ActionName);

	UFUNCTION(BlueprintCallable, Category="Action")
	virtual void InterruptRunningActions(AActor* Instigator);

	UPROPERTY(BlueprintAssignable)
	FOnActionStateChanged OnActionStarted;

	UPROPERTY(BlueprintAssignable)
	FOnActionStateChanged OnActionStopped;

	//default abilities that characters start with
	UPROPERTY(EditAnywhere, Category="Actions")
	TArray<TSubclassOf<UAction>> DefaultActions;

	UAction* GetLastPerformedAction();
	UFUNCTION(BlueprintCallable)
	UAction* GetRunningAction();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	TArray<UAction*> Actions;

	UPROPERTY()
	UAction* LastPerformedAction;
	
	UPROPERTY()
	UAction* CurrentRunningAction;

	UFUNCTION(BlueprintCallable, Category="Action")
	void OnStaminaDepleted();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; //end all running actions
	

	void InterruptAUXActions(AActor* Instigator);
	
};

