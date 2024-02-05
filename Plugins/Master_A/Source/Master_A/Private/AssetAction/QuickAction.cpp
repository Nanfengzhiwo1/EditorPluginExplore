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
		ShowMsgDialog(EAppMsgType::Ok,TEXT("Please enter a valid number"));
		return;
	}

	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();

	// track how many assets have been duplicated
	uint32 Counter = 0;

	for (const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		for (int32 i=0;i<NumOfDuplicates;i++)
		{
			const FString SourceAssetPath = SelectedAssetData.ObjectPath.ToString();
			const FString NewDuplicatedAssetName = SelectedAssetData.AssetName.ToString() + TEXT("_") + FString::FromInt(i+1);
			const FString NewPathName =FPaths::Combine(SelectedAssetData.PackagePath.ToString(),NewDuplicatedAssetName);

			// when pointer is valid ,duplicated an asset successfully
			if (UEditorAssetLibrary::DuplicateAsset(SourceAssetPath, NewPathName))
			{
				// OnlyIfIsDirty:false,means save assets no matter what 
				UEditorAssetLibrary::SaveAsset(NewPathName,false);
				++Counter;
			}
		}
	}

	if (Counter > 0)
	{
		ShowNotifyInfo(TEXT("Successfully duplicated " + FString::FromInt(Counter) + "Assets"));
	}
	
}
