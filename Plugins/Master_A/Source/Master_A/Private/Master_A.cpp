// Copyright Epic Games, Inc. All Rights Reserved.

#include "Master_A.h"

#include "ContentBrowserModule.h"
#include "Debug/MessageAction.h"
#include "EditorAssetLibrary.h"
#include <Editor/UnrealEd/Public/ObjectTools.h>
#include "AssetToolsModule.h"
#include "AssetRegistryModule.h"
#include "SlateWidgets/AdvanceDeletionWidget.h"

#define LOCTEXT_NAMESPACE "FMaster_AModule"

void FMaster_AModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	InitContentBrowserMenuExtention();
	RegisterAdvanceDeletionTab();
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
	//get hold of all the menu extenders
	TArray<FContentBrowserMenuExtender_SelectedPaths>& ContentBrowserModleMenuExtenders = ContentBrowserModule.GetAllPathViewContextMenuExtenders();
	/*
	// This is a delegate,return type is TSharedRef<FExtender>
	FContentBrowserMenuExtender_SelectedPaths CustomContentBrowserMenuDelegate;
	// bind this delegate to that function
	CustomContentBrowserMenuDelegate.BindRaw(this, &FMaster_AModule::CustomContentBrowserMenuExtender);
	ContentBrowserModleMenuExtenders.Add(CustomContentBrowserMenuDelegate);
	*/

	// add custom delegate to all the existing delegates
	ContentBrowserModleMenuExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this, &FMaster_AModule::CustomContentBrowserMenuExtender));
}
// To define the position for inserting menu entry
TSharedRef<FExtender> FMaster_AModule::CustomContentBrowserMenuExtender(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> MenuExtender(new FExtender());

	if (SelectedPaths.Num() > 0)
	{
		// add extention hook
		MenuExtender->AddMenuExtension(
			FName("Delete"),
			EExtensionHook::After,
			// add custom hot keys
			TSharedPtr<FUICommandList>(),
			// This is a delegate,no return type,one parameter
			FMenuExtensionDelegate::CreateRaw(this, &FMaster_AModule::AddContentBrowersMenuEntry));

		FolderPathsSelected = SelectedPaths;
	}

	return MenuExtender;
}

// Define deatils for the custom menu entry
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

	MenuBuilder.AddMenuEntry
	(
		// title
		FText::FromString(TEXT("Delete Empty Folders")),
		// tips
		FText::FromString(TEXT("Safely delete all empty folders")),
		// icon
		FSlateIcon(),
		// UI actions,this is a delegate,no return type,no parameter
		FExecuteAction::CreateRaw(this, &FMaster_AModule::OnDeleteEmptyFoldersButtonClicked)
	);

	MenuBuilder.AddMenuEntry
	(
		// title
		FText::FromString(TEXT("Advance Deletion")),
		// tips
		FText::FromString(TEXT("List assets by specific condition in a tab for deleting")),
		// icon
		FSlateIcon(),
		// UI actions,this is a delegate,no return type,no parameter
		FExecuteAction::CreateRaw(this, &FMaster_AModule::OnAdvanceDeletionButtonClicked)
	);
}

void FMaster_AModule::OnDeleteUnusedAssetButtonClicked()
{
	if (FolderPathsSelected.Num() > 1)
	{
		MessageAction::ShowMsgDialog(EAppMsgType::Ok, TEXT("You can only do this to one folder"));
		return;
	}
	TArray<FString>AssetPathNames = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0]);

	// Whether there are assets under the folder
	if (AssetPathNames.Num() == 0)
	{
		MessageAction::ShowMsgDialog(EAppMsgType::Ok, TEXT("No asset found under selected folder"), false);
		return;
	}

	EAppReturnType::Type ConfirmResult = MessageAction::ShowMsgDialog(EAppMsgType::YesNo, TEXT("A total of ") + FString::FromInt(AssetPathNames.Num()) + TEXT(" assets need to be checked. \nWoulde you like to procceed?"), false);

	if (ConfirmResult == EAppReturnType::No)
	{
		return;
	}

	FixUpRedirectors();

	TArray<FAssetData> UnusedAssetsData;

	for (const FString& AssetPathName : AssetPathNames)
	{
		// Don't touch these folders in Content folder
		if (AssetPathName.Contains(TEXT("Developers")) || AssetPathName.Contains(TEXT("Collections")) || AssetPathName.Contains(TEXT("__ExternalActors__")) || AssetPathName.Contains(TEXT("__ExternalObjects__")))
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName))
		{
			continue;
		}

		TArray<FString>AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPathName);

		if (AssetReferencers.Num() == 0)
		{
			const FAssetData& UnusedAssetData = UEditorAssetLibrary::FindAssetData(AssetPathName);
			UnusedAssetsData.Add(UnusedAssetData);
		}
	}
	if (UnusedAssetsData.Num() > 0)
	{
		ObjectTools::DeleteAssets(UnusedAssetsData);
	}
	else
	{
		MessageAction::ShowMsgDialog(EAppMsgType::Ok, TEXT("No unused asset found under selected folder"), false);
	}
}
void FMaster_AModule::OnDeleteEmptyFoldersButtonClicked()
{
	FixUpRedirectors();
	TArray<FString>FolderPathsArray = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0], true, true);
	uint32 Counter = 0;

	FString EmptyFolderPathsNames;
	TArray<FString> EmptyFoldersPathsArray;

	for (const FString& FolderPath : FolderPathsArray)
	{
		if (FolderPath.Contains(TEXT("Developers")) || FolderPath.Contains(TEXT("Collections")) || FolderPath.Contains(TEXT("__ExternalActors__")) || FolderPath.Contains(TEXT("__ExternalObjects__")))
		{
			continue;
		}
		if (!UEditorAssetLibrary::DoesDirectoryExist(FolderPath))
		{
			continue;
		}
		if (!UEditorAssetLibrary::DoesDirectoryHaveAssets(FolderPath))
		{
			EmptyFolderPathsNames.Append(FolderPath);
			EmptyFolderPathsNames.Append(TEXT("\n"));
			EmptyFoldersPathsArray.Add(FolderPath);
		}
	}

	if (EmptyFoldersPathsArray.Num() == 0)
	{
		MessageAction::ShowMsgDialog(EAppMsgType::Ok, TEXT("No empty folder found under selected folder"), false);
		return;
	}

	EAppReturnType::Type ConfirmResult = MessageAction::ShowMsgDialog(EAppMsgType::OkCancel, TEXT("Empty folders found in :\n") + EmptyFolderPathsNames + TEXT("\nWould you like to delete all?"), false);

	if (ConfirmResult == EAppReturnType::Cancel)
	{
		return;
	}

	for (const FString& EmptyFolderPath : EmptyFoldersPathsArray)
	{
		UEditorAssetLibrary::DeleteDirectory(EmptyFolderPath) ? ++Counter : MessageAction::ScreenPrint(TEXT("Failed to delete" + EmptyFolderPath), FColor::Red);
	}

	if (Counter > 0)
	{
		MessageAction::ShowNotifyInfo(TEXT("Successfully deleted ") + FString::FromInt(Counter) + TEXT(" folders"));
	}
}
void FMaster_AModule::OnAdvanceDeletionButtonClicked()
{
	FixUpRedirectors();
	FGlobalTabmanager::Get()->TryInvokeTab(FName("Advance Deletion"));
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
	Filter.ClassPaths.Emplace("/Script/ObjectRedirector");

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

#pragma region CustomEditorTab
void FMaster_AModule::RegisterAdvanceDeletionTab()
{
	//FOnSpawnTab:one return value (TSharedRef<SDockTab>),one Parameter(const FSpawnTabArgs& )
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FName("AdvanceDeletion"), FOnSpawnTab::CreateRaw(this, &FMaster_AModule::OnSpawnAdvanceDeltionTab)).SetDisplayName(FText::FromString(TEXT("Advance Deletion")));
}

TSharedRef<SDockTab> FMaster_AModule::OnSpawnAdvanceDeltionTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[
			SNew(SAdvanceDeletionTab).AssetDataToStore(GetAllAssetDataUnderSelectedFolder())
		];
}

TArray<TSharedPtr<FAssetData>> FMaster_AModule::GetAllAssetDataUnderSelectedFolder()
{
	TArray<TSharedPtr<FAssetData>>AvaiableAssetsData;
	TArray<FString>AssetPathNames=UEditorAssetLibrary::ListAssets(FolderPathsSelected[0]);
	
	for (const FString& AssetPathName : AssetPathNames)
	{
		// Don't touch these folders in Content folder
		if (AssetPathName.Contains(TEXT("Developers")) || AssetPathName.Contains(TEXT("Collections")) || AssetPathName.Contains(TEXT("__ExternalActors__")) || AssetPathName.Contains(TEXT("__ExternalObjects__")))
		{
			continue;
		}

		const FAssetData Data=UEditorAssetLibrary::FindAssetData(AssetPathName);
		AvaiableAssetsData.Add(MakeShared<FAssetData>(Data));
	}
	return AvaiableAssetsData;
}
#pragma endregion 

#pragma region ProccessDataForAdvanceDeletionTab

bool FMaster_AModule::DeleteSingleAssetForAssetList(const FAssetData& AssetDataToDelete)
{
	TArray<FAssetData>AssetDataForDeletion;
	AssetDataForDeletion.Add(AssetDataToDelete);
	if (ObjectTools::DeleteAssets(AssetDataForDeletion)>0)
	{
		return  true;
	}
	return  false;
}

bool FMaster_AModule::DeleteMultipleAssetsForAssetList(const TArray<FAssetData>& AssetsToDelete)
{
	if (ObjectTools::DeleteAssets(AssetsToDelete)>0)
	{
		return true;
	}
	return false;
}

void FMaster_AModule::ListUnusedAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter,
	TArray<TSharedPtr<FAssetData>>& OutUnusedAssetsData)
{
	OutUnusedAssetsData.Empty();
	for (const TSharedPtr<FAssetData>&DataSharedPtr:AssetDataToFilter)
	{
		
		TArray<FString>AssetReferencers= UEditorAssetLibrary::FindPackageReferencersForAsset(DataSharedPtr->ObjectPath.ToString());
		if (AssetReferencers.Num()==0)
		{
			OutUnusedAssetsData.Add(DataSharedPtr);
		}
	}
}

void FMaster_AModule::ListSameNameAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter,
	TArray<TSharedPtr<FAssetData>>& OutSameNameAssetsData)
{
	OutSameNameAssetsData.Empty();
	
	//Multimap for supporting finding assets with same name
	TMultiMap<FString,TSharedPtr<FAssetData>>AssetsInfoMultiMap;
	for (const TSharedPtr<FAssetData>& DataSharedPtr:AssetDataToFilter)
	{
		AssetsInfoMultiMap.Emplace(DataSharedPtr->AssetName.ToString(),DataSharedPtr);
	}
	for (const TSharedPtr<FAssetData>& DataSharedPtr:AssetDataToFilter)
	{
		TArray<TSharedPtr<FAssetData>>OutAssetsData;
		AssetsInfoMultiMap.MultiFind(DataSharedPtr->AssetName.ToString(),OutAssetsData);

		if (OutAssetsData.Num()<=1)
		{
			continue;
		}
		for(const TSharedPtr<FAssetData>&SameNameData:OutAssetsData)
		{
			if (SameNameData.IsValid())
			{
				OutSameNameAssetsData.AddUnique(SameNameData);
			}
		}
	}
}

void FMaster_AModule::SyncContentBrowerToClickedAssetForAssetList(const FString& AssetPathToSync)
{
	TArray<FString>AssetsPathToSync;
	AssetsPathToSync.Add(AssetPathToSync);
	UEditorAssetLibrary::SyncBrowserToObjects(AssetsPathToSync);
}

#pragma endregion

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMaster_AModule, Master_A)
