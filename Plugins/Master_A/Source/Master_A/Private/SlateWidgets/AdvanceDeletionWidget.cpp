// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/AdvanceDeletionWidget.h"

#include "Master_A.h"
#include "SlateBasics.h"
#include "Debug/MessageAction.h"

#define ListAll TEXT("List All Available Assets")
#define ListUnused TEXT("List Unused Assets")
#define ListSameName TEXT("List Assets With Same Name")

void SAdvanceDeletionTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	StoredAssetsData = InArgs._AssetDataToStore;
	DisplayedAssetsData=StoredAssetsData;
	CheckBoxesArray.Empty();
	AssetsDataToDeletArray.Empty();
	ComboBoxSourceItems.Empty();
	
	ComboBoxSourceItems.Add(MakeShared<FString>(ListAll));
	ComboBoxSourceItems.Add(MakeShared<FString>(ListUnused));
	ComboBoxSourceItems.Add(MakeShared<FString>(ListSameName));
	
	FSlateFontInfo TitleTextFont = GetEmboseedTextFont();
	TitleTextFont.Size = 30;

	ChildSlot
	[
		// main vertical box
		SNew(SVerticalBox)

		// title text
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Advance Deletion")))
			.Font(TitleTextFont)
			.Justification(ETextJustify::Center)
			.ColorAndOpacity(FColor::White)
		]
		// select condition
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			// Combo Box
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				ConstructComboBox()
			]
		]
		// asset list
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				ConstructAssetListView()
			]
		]
		// buttons
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)
			[
				ConstructDeleteAllButton()
			]
			+SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)
			[
				ConstructSelectAllButton()
			]
			+SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)
			[
				ConstructDeselectAllButton()
			]
		]
	];
}

TSharedRef<SListView<TSharedPtr<FAssetData>>> SAdvanceDeletionTab::ConstructAssetListView()
{
	ConstructedAssetListView= SNew(SListView<TSharedPtr<FAssetData>>)
				.ItemHeight(24.f)
				.ListItemsSource(&DisplayedAssetsData)
				.OnGenerateRow(this, &SAdvanceDeletionTab::OnGenerateRowForList);
	return ConstructedAssetListView.ToSharedRef();
}

void SAdvanceDeletionTab::RefreshAssetListView()
{
	CheckBoxesArray.Empty();
	AssetsDataToDeletArray.Empty();
	
	if (ConstructedAssetListView.IsValid())
	{
		ConstructedAssetListView->RebuildList();
	}
}
#pragma region ComboBoxForListingCondition
TSharedRef<SComboBox<TSharedPtr<FString>>> SAdvanceDeletionTab::ConstructComboBox()
{
	TSharedRef<SComboBox<TSharedPtr<FString>>>ConstructedComboBox=SNew(SComboBox<TSharedPtr<FString>>)
	.OptionsSource(&ComboBoxSourceItems)
	.OnGenerateWidget(this,&SAdvanceDeletionTab::OnGenerateComboContent)
	.OnSelectionChanged(this,&SAdvanceDeletionTab::OnComboSelectionChanged)
	[
		SAssignNew(ComboDisplayTextBlock,STextBlock)
		.Text(FText::FromString(TEXT("List Assets option")))
	];

	return ConstructedComboBox;
}

TSharedRef<SWidget> SAdvanceDeletionTab::OnGenerateComboContent(TSharedPtr<FString> SourceItem)
{
	TSharedRef<STextBlock>ConstructedComboText= SNew(STextBlock).Text(FText::FromString(*SourceItem.Get()));
	return ConstructedComboText;
}

void SAdvanceDeletionTab::OnComboSelectionChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo)
{
	MessageAction::LogPrint(TEXT("1"));

	FMaster_AModule&Master_AModule= FModuleManager::LoadModuleChecked<FMaster_AModule>(TEXT("Master_A"));
	
	ComboDisplayTextBlock->SetText(FText::FromString(*SelectedOption.Get()));
	
	//Pass data for our module to filter based on the selected options
	if (*SelectedOption.Get()==ListAll)
	{
		//List all stored asset data
		DisplayedAssetsData=StoredAssetsData;
		RefreshAssetListView();
	}
	else if(*SelectedOption.Get()==ListUnused)
	{
		//List all unused asssets
		Master_AModule.ListUnusedAssetsForAssetList(StoredAssetsData,DisplayedAssetsData);
		RefreshAssetListView();
	}
	else if (*SelectedOption.Get()==ListSameName)
	{
		//List assets with same name
		Master_AModule.ListSameNameAssetsForAssetList(StoredAssetsData,DisplayedAssetsData);
		RefreshAssetListView();
	}
}

#pragma endregion 


#pragma region RowWidgetForAssetListView	
TSharedRef<ITableRow> SAdvanceDeletionTab::OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay,
                                                                const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!AssetDataToDisplay.IsValid())
	{
		return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable);
	}

	const FString DisplayAssetClassName = AssetDataToDisplay->GetClass()->GetName();
	// AssetClassPath repalce AssetClass

	const FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString();

	FSlateFontInfo AssetClassNameFont = GetEmboseedTextFont();
	AssetClassNameFont.Size = 10;

	FSlateFontInfo AssetNameFont = GetEmboseedTextFont();
	AssetNameFont.Size = 15;

	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListViewRowWidget =
		SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable).Padding(FMargin(5.f))
		[
			SNew(SHorizontalBox)
			// display checkboxs
			+ SHorizontalBox::Slot()
			  .HAlign(HAlign_Left)
			  .VAlign(VAlign_Center)
			  .FillWidth(.05f)
			[
				ConstructCheckBox(AssetDataToDisplay)
			]

			// display asset class names
			+ SHorizontalBox::Slot()
			  .HAlign(HAlign_Center)
			  .VAlign(VAlign_Fill)
			  .FillWidth(.5f)
			[
				ConstructTextForRowWidget(DisplayAssetClassName, AssetClassNameFont)
			]
			// display asset names
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Fill)
			[
				ConstructTextForRowWidget(DisplayAssetName, AssetNameFont)
			]
			// display delete buttons
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Fill)
			[
				ConstructButtonForRowWidget(AssetDataToDisplay)
			]
		];
	return ListViewRowWidget;
}

TSharedRef<SCheckBox> SAdvanceDeletionTab::ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SCheckBox> ConstructedCheckBox = SNew(SCheckBox)
	.Type(ESlateCheckBoxType::CheckBox)
	.OnCheckStateChanged(this, &SAdvanceDeletionTab::OnCheckBoxStateChanged, AssetDataToDisplay)
	.Visibility(EVisibility::Visible);

	CheckBoxesArray.Add(ConstructedCheckBox);
	return ConstructedCheckBox;
}

void SAdvanceDeletionTab::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
		if (AssetsDataToDeletArray.Contains(AssetData))
		{
			AssetsDataToDeletArray.Remove(AssetData);
		}
		break;
	case ECheckBoxState::Checked:
		AssetsDataToDeletArray.AddUnique(AssetData);
		break;
	case ECheckBoxState::Undetermined:
		break;
	default:
		break;
	}
	MessageAction::LogPrint(AssetData->AssetName.ToString());
}

TSharedRef<STextBlock> SAdvanceDeletionTab::ConstructTextForRowWidget(const FString& TextContent, const FSlateFontInfo& FontToUse)
{
	TSharedRef<STextBlock>ConstructedTextBlock=SNew(STextBlock)
		.Text(FText::FromString(TextContent))
		.Font(FontToUse)
		.ColorAndOpacity(FColor::White);
	return ConstructedTextBlock;
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructButtonForRowWidget(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> ConstructedButton=SNew(SButton)
	.Text(FText::FromString(TEXT("Delete")))
	.OnClicked(this,&SAdvanceDeletionTab::OnDeleteButtonClicked,AssetDataToDisplay);
	return ConstructedButton;
}

FReply SAdvanceDeletionTab::OnDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData)
{
	FMaster_AModule&Master_AModule= FModuleManager::LoadModuleChecked<FMaster_AModule>(TEXT("Master_A"));
	const bool bAssetDeleted=Master_AModule.DeleteSingleAssetForAssetList(*ClickedAssetData.Get());
	if (bAssetDeleted)
	{
		//Update the list source items
		if (StoredAssetsData.Contains(ClickedAssetData))
		{
			StoredAssetsData.Remove(ClickedAssetData);
		}
		if (DisplayedAssetsData.Contains(ClickedAssetData))
		{
			DisplayedAssetsData.Remove(ClickedAssetData);
		}
		//Refresh the list
		RefreshAssetListView();
	}	
	return  FReply::Handled();
}

#pragma endregion 

#pragma region TabButtons
TSharedRef<SButton> SAdvanceDeletionTab::ConstructDeleteAllButton()
{
	TSharedRef<SButton>DeleteAllButton=SNew(SButton).ContentPadding(FMargin(5.f)).OnClicked(this,&SAdvanceDeletionTab::OnDeleteAllButtonClicked);
	DeleteAllButton->SetContent(ConstructTextForTabButtons(TEXT("Delete All")));
	return DeleteAllButton;
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructSelectAllButton()
{
	TSharedRef<SButton>SelectAllButton=SNew(SButton).ContentPadding(FMargin(5.f)).OnClicked(this,&SAdvanceDeletionTab::OnSelectAllButtonClicked);
	SelectAllButton->SetContent(ConstructTextForTabButtons(TEXT("Select All")));
	return SelectAllButton;
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructDeselectAllButton()
{
	TSharedRef<SButton>DeselectAllButton=SNew(SButton).ContentPadding(FMargin(5.f)).OnClicked(this,&SAdvanceDeletionTab::OnDeselectAllButtonClicked);
	DeselectAllButton->SetContent(ConstructTextForTabButtons(TEXT("Deselect All")));
	return DeselectAllButton;
}

FReply SAdvanceDeletionTab::OnDeleteAllButtonClicked()
{
	if (AssetsDataToDeletArray.Num()==0)
	{
		MessageAction::ShowMsgDialog(EAppMsgType::Ok,TEXT("No asset currently selected!"));
		return FReply::Handled();
	}

	TArray<FAssetData>AssetDataToDelete;
	for (const TSharedPtr<FAssetData>& Data:AssetsDataToDeletArray)
	{
		AssetDataToDelete.Add(*Data.Get());
	}
	FMaster_AModule&Master_AModule= FModuleManager::LoadModuleChecked<FMaster_AModule>(TEXT("Master_A"));
	const bool bAssetsDeleted=Master_AModule.DeleteMultipleAssetsForAssetList(AssetDataToDelete);
	if (bAssetsDeleted)
	{
		for (const TSharedPtr<FAssetData>& DeletedData:AssetsDataToDeletArray)
		{
			//Updating the stored assets data
			if (StoredAssetsData.Contains(DeletedData))
			{
				StoredAssetsData.Remove(DeletedData);
			}
			if (DisplayedAssetsData.Contains(DeletedData))
			{
				DisplayedAssetsData.Remove(DeletedData);
			}
		}
		RefreshAssetListView();
	}
	//pass data to our module for deletion
	return FReply::Handled();
}

FReply SAdvanceDeletionTab::OnSelectAllButtonClicked()
{
	if (CheckBoxesArray.Num()==0)
	{
		return FReply::Handled();
	}
	for (const TSharedRef<SCheckBox>& CheckBox:CheckBoxesArray)
	{
		if (!CheckBox->IsChecked())
		{
			CheckBox->ToggleCheckedState();
		}
		
	}
	return FReply::Handled();
}

FReply SAdvanceDeletionTab::OnDeselectAllButtonClicked()
{
	if (CheckBoxesArray.Num()==0)
	{
		return FReply::Handled();
	}
	for (const TSharedRef<SCheckBox>& CheckBox:CheckBoxesArray)
	{
		if (CheckBox->IsChecked())
		{
			CheckBox->ToggleCheckedState();
		}
		
	}
	return FReply::Handled();
}
#pragma endregion 
TSharedRef<STextBlock> SAdvanceDeletionTab::ConstructTextForTabButtons(const FString& TextContent)
{
	FSlateFontInfo ButtonTextFont = GetEmboseedTextFont();
	ButtonTextFont.Size=15;

	TSharedRef<STextBlock>ConstructedTextBlock=SNew(STextBlock).Text(FText::FromString(TextContent)).Font(ButtonTextFont).Justification(ETextJustify::Center);
	return  ConstructedTextBlock;
}




