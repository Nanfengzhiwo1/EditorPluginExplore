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

	TArray<FString>FolderPathsSelected;

	// Function: Define the position for inserting menu entry
	TSharedRef<FExtender> CustomContentBrowserMenuExtender(const TArray<FString>& SelectedPaths);

	// Function: Define all the details for menu entry
	void AddContentBrowersMenuEntry(class FMenuBuilder& MenuBuilder);

	// Function: Excute the Action
	void OnDeleteUnusedAssetButtonClicked();

	void OnDeleteEmptyFoldersButtonClicked();

	void OnAdvanceDeletionButtonClicked();
	void FixUpRedirectors();
#pragma endregion 

#pragma region CustomEditorTab
	void RegisterAdvanceDeletionTab();

	TSharedRef<SDockTab> OnSpawnAdvanceDeltionTab(const FSpawnTabArgs& SpawnTabArgs);

	TArray<TSharedPtr<FAssetData>> GetAllAssetDataUnderSelectedFolder();
	
#pragma endregion

public:
#pragma region ProccessDataForAdvanceDeletionTab
	bool DeleteSingleAssetForAssetList(const FAssetData&AssetDataToDelete);
	bool DeleteMultipleAssetsForAssetList(const TArray<FAssetData>&AssetsToDelete);
	void ListUnusedAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>&AssetDataToFilter,TArray<TSharedPtr<FAssetData>>& OutUnusedAssetsData);
	void ListSameNameAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>&AssetDataToFilter,TArray<TSharedPtr<FAssetData>>& OutSameNameAssetsData);
	void SyncContentBrowerToClickedAssetForAssetList(const FString& AssetPathToSync);
#pragma endregion 
};
