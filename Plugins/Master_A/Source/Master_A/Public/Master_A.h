// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMaster_AModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


private:
	// In order to have better readability
#pragma region ContentBrowserMenuExtention
	void InitContentBrowserMenuExtention();
	// Function: Define the position for inserting menu entry
	TSharedRef<FExtender> CustomContentBrowserMenuExtender(const TArray<FString>& SelectedPaths);

	// Function: Define all the details for menu entry
	void AddContentBrowersMenuEntry(class FMenuBuilder& MenuBuilder);

	// Function: Excute the Action
	void OnDeleteUnusedAssetButtonClicked();
#pragma endregion 
};
