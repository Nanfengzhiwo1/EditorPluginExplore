// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"


#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundWave.h"
#include "Engine/Texture.h"
#include "Blueprint/UserWidget.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraEmitter.h"
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

	UFUNCTION(CallInEditor)
	void AddPrefixes();

	UFUNCTION(CallInEditor)
	void RemoveUnusedAssets();
private:
	TMap<UClass*, FString>PrefixMap =
	{
		{UBlueprint::StaticClass(),TEXT("BP_")},
		{UStaticMesh::StaticClass(),TEXT("SM_")},
		{UMaterial::StaticClass(),TEXT("M_")},
		{UMaterialInstanceConstant::StaticClass(),TEXT("MI_")},
		{UMaterialFunctionInterface::StaticClass(),TEXT("MF_")},
		{USoundCue::StaticClass(),TEXT("SC_")},
		{USoundWave::StaticClass(),TEXT("SW_")},
		{UTexture::StaticClass(),TEXT("T_")},
		{UTexture2D::StaticClass(),TEXT("T2D_")},
		{UUserWidget::StaticClass(),TEXT("WBP_")},
		{USkeletalMeshComponent::StaticClass(),TEXT("SK_")},
		{UNiagaraSystem::StaticClass(),TEXT("NS_")},
		{UNiagaraEmitter::StaticClass(),TEXT("NE_")},
	};

	void FixUpRedirectors();
};
