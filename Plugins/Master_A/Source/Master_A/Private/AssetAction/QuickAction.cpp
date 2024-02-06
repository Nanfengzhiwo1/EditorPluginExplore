// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetAction/QuickAction.h"
#include "Debug/MessageAction.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include <Editor/UnrealEd/Public/ObjectTools.h>

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

void UQuickAction::AddPrefixes()
{
	TArray<UObject*>SelectedObjects = UEditorUtilityLibrary::GetSelectedAssets();

	// track of how many prefixes have been added
	uint32 Counter = 0;

	for (UObject* SelectedObject : SelectedObjects) 
	{
		if (!SelectedObject)
		{
			continue;
		}
		// use the prefix map find the key
		FString* PrefixFound= PrefixMap.Find(SelectedObject->GetClass());

		//check the pointer before using
		if (!PrefixFound || PrefixFound->IsEmpty())
		{
			ShowMsgDialog(EAppMsgType::Ok, TEXT("Failed to find prefix for class")+SelectedObject->GetClass()->GetName());
			continue;
		}
		FString OldName = SelectedObject->GetName();

		// check if the name already starts with the prefix
		if (OldName.StartsWith(*PrefixFound))
		{
			ShowMsgDialog(EAppMsgType::Ok, OldName + TEXT("already has prefix added"));
			continue;
		}

		// check UMaterialInstanceConstant type 
		if (SelectedObject->IsA<UMaterialInstanceConstant>())
		{
			OldName.RemoveFromStart(TEXT("M_"));
			OldName.RemoveFromEnd(TEXT("_Inst"));
		}
		const FString NewNameWithPrefix = *PrefixFound + OldName;
		
		UEditorUtilityLibrary::RenameAsset(SelectedObject,NewNameWithPrefix);
		++Counter;
	}

	if(Counter>0)
	{
		ShowNotifyInfo(TEXT("Successfully renamed " + FString::FromInt(Counter) + " assets"));
	}
	
}

void UQuickAction::RemoveUnusedAssets()
{
	TArray<FAssetData> SelectedAssetsData= UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<FAssetData> UnusedAssetsData;

	for (const FAssetData SelectedAssetData : SelectedAssetsData)
	{
		TArray<FString> AssetReferences=UEditorAssetLibrary::FindPackageReferencersForAsset(SelectedAssetData.ObjectPath.ToString());

		if(AssetReferences.Num()==0)
		{
			UnusedAssetsData.Emplace(SelectedAssetData);
		}
	}

	if (UnusedAssetsData.Num() == 0)
	{
		ShowMsgDialog(EAppMsgType::Ok,TEXT("No unused assets found among selected assets."), false);
		return;
	}

	const int32 NumOfAssetsDeleted = ObjectTools::DeleteAssets(UnusedAssetsData);

	if (NumOfAssetsDeleted==0)
	{
		return;
	}
	ShowNotifyInfo(TEXT("Successfully deleted "+FString::FromInt(NumOfAssetsDeleted) +TEXT("unused assets.")));
}
