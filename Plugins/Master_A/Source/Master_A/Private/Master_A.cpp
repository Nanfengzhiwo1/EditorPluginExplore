// Copyright Epic Games, Inc. All Rights Reserved.

#include "Master_A.h"

#include "ContentBrowserModule.h"
#include "Debug/MessageAction.h"
#include "EditorAssetLibrary.h"
#include <Editor/UnrealEd/Public/ObjectTools.h>
#include "AssetToolsModule.h"
#include "AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "FMaster_AModule"

void FMaster_AModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	InitContentBrowserMenuExtention();
}

void FMaster_AModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}


#pragma region ContentBrowserMenuExtention
void FMaster_AModule::InitContentBrowserMenuExtention()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FContentBrowserMenuExtender_SelectedPaths>& ContentBrowserModleMenuExtenders = ContentBrowserModule.GetAllPathViewContextMenuExtenders();
	/*
	// This is a delegate,return type is TSharedRef<FExtender>
	FContentBrowserMenuExtender_SelectedPaths CustomContentBrowserMenuDelegate;
	// bind this delegate to that function 
	CustomContentBrowserMenuDelegate.BindRaw(this, &FMaster_AModule::CustomContentBrowserMenuExtender);
	ContentBrowserModleMenuExtenders.Add(CustomContentBrowserMenuDelegate);
	*/

	ContentBrowserModleMenuExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this, &FMaster_AModule::CustomContentBrowserMenuExtender));
}
TSharedRef<FExtender> FMaster_AModule::CustomContentBrowserMenuExtender(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> MenuExtender(new FExtender());

	if (SelectedPaths.Num()>0)
	{
		MenuExtender->AddMenuExtension(
			FName("Delete"),
			EExtensionHook::After,
			TSharedPtr<FUICommandList>(),
			// This is a delegate,no return type,one parameter
			FMenuExtensionDelegate::CreateRaw(this,&FMaster_AModule::AddContentBrowersMenuEntry));
			
		FolderPathsSelected = SelectedPaths;
	}

	return MenuExtender;
}

void FMaster_AModule::AddContentBrowersMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry
	(
		// title
		FText::FromString(TEXT("Delete Unused Assets")),
		// tips
		FText::FromString(TEXT("Safely delete all unused assets under folder")),
		// icon
		FSlateIcon(),
		// UI actions,this is a delegate,no return type,no parameter
		FExecuteAction::CreateRaw(this, &FMaster_AModule::OnDeleteUnusedAssetButtonClicked)
	);
}

void FMaster_AModule::OnDeleteUnusedAssetButtonClicked()
{
	if (FolderPathsSelected.Num()>1)
	{
		MessageAction::ShowMsgDialog(EAppMsgType::Ok,TEXT("You can only do this to one folder"));
		return;
	}
	TArray<FString>AssetPathNames= UEditorAssetLibrary::ListAssets(FolderPathsSelected[0]);

	if (AssetPathNames.Num()==0)
	{
		MessageAction::ShowMsgDialog(EAppMsgType::Ok, TEXT("No asset found under selected folder")); 
		return;
	}

	EAppReturnType::Type ConfirmResult= MessageAction::ShowMsgDialog(EAppMsgType::YesNo, TEXT("A total of ")+FString::FromInt(AssetPathNames.Num())+TEXT(" found. \nWoudle you like to procceed?"));

	if (ConfirmResult==EAppReturnType::No)
	{
		return;
	}

	FixUpRedirectors();

	TArray<FAssetData> UnusedAssetsData;

	for (const FString& AssetPathName : AssetPathNames)
	{
		// Don't touch Developers folder and Collections in Content folder
		if (AssetPathName.Contains(TEXT("Developers"))|| AssetPathName.Contains(TEXT("Collections")))
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName))
		{
			continue;
		}

		TArray<FString>AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPathName);

		if (AssetReferencers.Num()==0)
		{
			const FAssetData& UnusedAssetData = UEditorAssetLibrary::FindAssetData(AssetPathName);
			UnusedAssetsData.Add(UnusedAssetData);
		}
	}
	if (UnusedAssetsData.Num()>0)
	{
		ObjectTools::DeleteAssets(UnusedAssetsData);
	}
	else
	{
		MessageAction::ShowMsgDialog(EAppMsgType::Ok, TEXT("No unused asset found under selected folder"));
	}
}
void FMaster_AModule::FixUpRedirectors()
{
	TArray<UObjectRedirector* >RedirectorsToFixArray;

	/*get redirectors*/
	// access a  AssetRegistryModule,and store it in a local variable,only one instansce use "&",
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FARFilter Filter;
	// recursive pass to be true,it can go inside of all the subfolders
	Filter.bRecursivePaths = true;
	// which folder can go into
	Filter.PackagePaths.Emplace("/Game");
	// what's the name of the class that filter  !!!Class names are now represented by path names. Please use ClassPaths
	Filter.ClassPaths.Emplace("/Game/ObjectRedirector");

	TArray<FAssetData> OutRedirectors;

	AssetRegistryModule.Get().GetAssets(Filter, OutRedirectors);

	for (const FAssetData& RedirectorData : OutRedirectors)
	{
		if (UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(RedirectorData.GetAsset()))
		{
			RedirectorsToFixArray.Add(RedirectorToFix);
		}
	}

	/*fix redirectors*/
	FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

	AssetToolsModule.Get().FixupReferencers(RedirectorsToFixArray);
}
#pragma endregion

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMaster_AModule, Master_A)
