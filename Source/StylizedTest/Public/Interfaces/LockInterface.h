// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LockInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULockInterface : public UInterface
{
	GENERATED_BODY()
};


class STYLIZEDTEST_API ILockInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void GetLocked(AActor* Locker, bool Release = false);

	UFUNCTION(BlueprintNativeEvent)
	bool IsLockable();
};
