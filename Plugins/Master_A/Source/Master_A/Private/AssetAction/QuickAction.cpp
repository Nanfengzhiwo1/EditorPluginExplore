// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetAction/QuickAction.h"
#include "Debug/MessageAction.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include <Editor/UnrealEd/Public/ObjectTools.h>
#include "AssetToolsModule.h"
#include "AssetRegistryModule.h"

void UQuickAction::DuplicateAssets(int32 NumOfDuplicates)
{
	// when number is invalid ,print screen message
	if (NumOfDuplicates<=0)
	{
		MessageAction::ShowMsgDialog(EAppMsgType::Ok,TEXT("Please enter a valid number"));
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
		MessageAction::ShowNotifyInfo(TEXT("Successfully duplicated " + FString::FromInt(Counter) + "Assets"));
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
			MessageAction::ShowMsgDialog(EAppMsgType::Ok, TEXT("Failed to find prefix for class")+SelectedObject->GetClass()->GetName());
			continue;
		}
		FString OldName = SelectedObject->GetName();

		// check if the name already starts with the prefix
		if (OldName.StartsWith(*PrefixFound))
		{
			MessageAction::ShowMsgDialog(EAppMsgType::Ok, OldName + TEXT("already has prefix added"));
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
		MessageAction::ShowNotifyInfo(TEXT("Successfully renamed " + FString::FromInt(Counter) + " assets"));
	}
	
}

void UQuickAction::RemoveUnusedAssets()
{
	TArray<FAssetData> SelectedAssetsData= UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<FAssetData> UnusedAssetsData;

	FixUpRedirectors();
	
	for (const FAssetData SelectedAssetData : SelectedAssetsData)
	{
		TArray<FString> AssetReferences=UEditorAssetLibrary::FindPackageReferencersForAsset(SelectedAssetData.ObjectPath.ToString());

		if(AssetReferences.Num()==0)
		{
			UnusedAssetsData.Add(SelectedAssetData);
		}
	}

	if (UnusedAssetsData.Num() == 0)
	{
		MessageAction::ShowMsgDialog(EAppMsgType::Ok,TEXT("No unused assets found among selected assets."), false);
		return;
	}

	const int32 NumOfAssetsDeleted = ObjectTools::DeleteAssets(UnusedAssetsData);

	if (NumOfAssetsDeleted==0)
	{
		return;
	}
	MessageAction::ShowNotifyInfo(TEXT("Successfully deleted "+FString::FromInt(NumOfAssetsDeleted) +TEXT("unused assets.")));
}

void UQuickAction::FixUpRedirectors()
{
	TArray<UObjectRedirector* >RedirectorsToFixArray;

	/*get redirectors*/
	// access a  AssetRegistryModule,and store it in a local variable,only one instansce use "&",
	FAssetRegistryModule& AssetRegistryModule= FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace("/Game");
	Filter.ClassPaths.Emplace("/Script/ObjectRedirector");

	TArray<FAssetData> OutRedirectors;
	
	AssetRegistryModule.Get().GetAssets(Filter,OutRedirectors);

	for (const FAssetData& OutRedirector:OutRedirectors)
	{
		if (UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(OutRedirector.GetAsset()))
		{
			RedirectorsToFixArray.Add(RedirectorToFix);
		}
	}

	/*fix redirectors*/
	FAssetToolsModule& AssetToolsModule=FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

	AssetToolsModule.Get().FixupReferencers(RedirectorsToFixArray);
}
