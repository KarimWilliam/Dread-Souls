// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QueueComponent.generated.h"

class AMainCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STYLIZEDTEST_API UQueueComponent : public UActorComponent
{
	GENERATED_BODY()


public:
	UQueueComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	bool GetIsActionQueued();
	UFUNCTION(BlueprintCallable)
	FName GetQueuedActionName();
	UFUNCTION(BlueprintCallable)
	bool GetIsActionLocked();
	UFUNCTION(BlueprintCallable)
	void ActivateAction();

	bool UpdateQueuedAction(class UAction* Action);
	bool ClearQueuedAction();

protected:
	virtual void BeginPlay() override;

private:
	UAction* PreviousAction;

	UPROPERTY(VisibleAnywhere)
	bool IsActionQueued = false;
	UPROPERTY(VisibleAnywhere)
	UAction* QueuedAction;
	UPROPERTY(VisibleAnywhere)
	bool IsActionLocked = true;
	UPROPERTY(VisibleAnywhere)
	bool IsFastActionLocked = false;
	UPROPERTY(VisibleAnywhere)
	bool IsRegularActionLocked = false;
	UPROPERTY(VisibleAnywhere)
	bool IsSlowActionLocked = false;
	UPROPERTY(VisibleAnywhere)
	bool IsPreviousActionLocked = true;
	AMainCharacter* MC;


public:
	//ACTION LOCK HELPERS
	UFUNCTION(BlueprintCallable)
	void LockQueuedAction();
	UFUNCTION(BlueprintCallable)
	void UnlockQueuedAction();

	UFUNCTION(BlueprintCallable)
	bool GetIsPreviousActionLocked();
	UFUNCTION(BlueprintCallable)
	void LockPreviousQueuedAction();
	UFUNCTION(BlueprintCallable)
	void UnlockPreviousQueuedAction();

	UFUNCTION()
	void SetPreviousAction(UAction* Action)
	{
		PreviousAction=Action;
	}
	 
	UAction* GetPreviousAction()
	{
		return PreviousAction;
	}
	UFUNCTION(BlueprintCallable)
	void LockAllQueuedAction();
};
