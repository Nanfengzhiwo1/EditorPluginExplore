// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetAction/QuickAction.h"
#include "Debug/MessageAction.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"

void UQuickAction::DuplicateAssets(int32 NumOfDuplicates)
{
	// when number is invalid ,print screen message
	if (NumOfDuplicates<=0)
	{
		ScreenPrint(TEXT("Please enter a valid number!"),FColor::Red);
		return;
	}

	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	uint32 Counter = 0;

	for (const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		for (int32 i=0;i<NumOfDuplicates;i++)
		{
		
		}
	}
}
