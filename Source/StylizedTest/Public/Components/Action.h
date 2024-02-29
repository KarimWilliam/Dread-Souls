// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "Action.generated.h"

UENUM(BlueprintType)
enum class EQueueActionSpeed : uint8
{
	EAS_Fast UMETA(DisplayName = "Fast"),
	EAS_Regular UMETA(DisplayName = "Regular"),
	EAS_Slow UMETA(DisplayName = "Slow")
};

class UActionComponent;
class UWorld;

UCLASS(Blueprintable)
class STYLIZEDTEST_API UAction : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Action")
	EQueueActionSpeed QueueActionSpeed = EQueueActionSpeed::EAS_Regular;

	UPROPERTY(EditDefaultsOnly, Category="Action")
	bool bAutoStart;

	UPROPERTY(VisibleDefaultsOnly, Category="Action")
	FName ActionName;

	UPROPERTY(EditAnywhere, Category="Action")
	float ActionPauseDuration =0;

	UPROPERTY(EditAnywhere, Category="Action")
	float ActionDamageMultiplier =1;

	UPROPERTY(VisibleDefaultsOnly, Category="Action")
	bool bActionCanLockDirection = false;

	UFUNCTION(BlueprintNativeEvent, Category="Action")
	bool CanStart(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category="Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Action")
	void StopAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category="Action")
	virtual void InterruptAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category="Action")
	bool IsRunning() const;



	UWorld* GetWorld() const override;
	//must override this in order to have functionality visible in bluepirnts. like spawning actors.

	//HELPERS
	UFUNCTION(BlueprintCallable)
	FName GetActionName();

	UFUNCTION(BlueprintCallable)
	bool GetIsActionQueueable();

	bool GetAUXAction()
	{
		return bAUXAction;
	}

	FGameplayTagContainer GetGrantsTags()
	{
		return GrantsTags;
	}


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSoftObjectPtr<UTexture2D> Icon; //soft object reference

	UPROPERTY(EditDefaultsOnly, Category="Tags")
	FGameplayTagContainer GrantsTags;
	UPROPERTY(EditDefaultsOnly, Category="Tags")
	FGameplayTagContainer BlockedTags;

	UFUNCTION(BlueprintCallable, Category="Action")
	UActionComponent* GetOwningComponent() const;

	bool bIsRunning;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Action")
	bool bQueueableAction = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Action")
	bool bAUXAction = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Action")
	bool bBypassBusy = false;
	float TimeStarted;

	UPROPERTY(EditDefaultsOnly, Category="Action", meta=(ToolTip="Front cost of starting the ability"))
	float StaminaCost = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Action", meta=(ToolTip="Cost per second of having the ability active"))
	float StaminaConsumption = 0.f;

	UFUNCTION(BlueprintCallable, Category="Action")
	bool HaveEnoughStamina();

	UFUNCTION(BlueprintCallable, Category="Action")
	void ConsumeStamina();

private:
	UPROPERTY()
	class UQueueComponent* QueueComponent;
};
