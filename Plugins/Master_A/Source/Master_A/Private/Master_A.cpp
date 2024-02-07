// Copyright Epic Games, Inc. All Rights Reserved.

#include "Master_A.h"

#include "ContentBrowserModule.h"

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
	FContentBrowserModule& ContentBrowserModule= FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FContentBrowserMenuExtender_SelectedPaths>& ContentBrowserModleMenuExtenders= ContentBrowserModule.GetAllPathViewContextMenuExtenders();
	FContentBrowserMenuExtender_SelectedPaths CustomContentBrowserMenuDelegate;
	ContentBrowserModleMenuExtenders.Emplace(CustomContentBrowserMenuDelegate);
}
#pragma endregion

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMaster_AModule, Master_A)
