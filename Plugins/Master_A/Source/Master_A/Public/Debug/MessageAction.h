#pragma once

// Print message on screen
void ScreenPrint(const FString& Message,const FColor& Color) 
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.f, Color, Message);
	}
}

// Print message on log
void LogPrint(const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
}