// Copyright Epic Games, Inc. All Rights Reserved.

#include "Master_A.h"

#include "ContentBrowserModule.h"
#include "Debug/MessageAction.h"

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
	ContentBrowserModleMenuExtenders.Emplace(CustomContentBrowserMenuDelegate);
	*/

	ContentBrowserModleMenuExtenders.Emplace(FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this, &FMaster_AModule::CustomContentBrowserMenuExtender));
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
}
#pragma endregion

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMaster_AModule, Master_A)
