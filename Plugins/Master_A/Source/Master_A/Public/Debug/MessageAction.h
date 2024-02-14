#pragma once
#include "Misc/MessageDialog.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

namespace MessageAction {
	// Print message on screen
	static void ScreenPrint(const FString& Message, const FColor& Color)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.f, Color, Message);
		}
	}

	// Print message on log
	static void LogPrint(const FString& Message)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
	}

	// Print message on Dialog
	static EAppReturnType::Type ShowMsgDialog(EAppMsgType::Type MessageType, const FString& Message, bool bShowMsgAsWarning = true)
	{
		if (bShowMsgAsWarning)
		{
			FText MsgTitle = FText::FromString(TEXT("Warning"));
			return FMessageDialog::Open(MessageType, FText::FromString(Message), &MsgTitle);
		}
		else
		{
			return FMessageDialog::Open(MessageType, FText::FromString(Message));
		}
	}

	// Pring message by notify
	static void ShowNotifyInfo(const FString& Message)
	{
		FNotificationInfo NotifyInfo(FText::FromString(Message));
		NotifyInfo.bUseLargeFont = true;
		NotifyInfo.FadeOutDuration = 7.f;

		// Add Notify to editor
		FSlateNotificationManager::Get().AddNotification(NotifyInfo);
	}
}
