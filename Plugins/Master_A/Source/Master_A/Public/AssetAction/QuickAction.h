// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "QuickAction.generated.h"

/**
 * 
 */
UCLASS()
class MASTER_A_API UQuickAction : public UAssetActionUtility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(CallInEditor)
	void DuplicateAssets(int32 NumOfDuplicates);

};
