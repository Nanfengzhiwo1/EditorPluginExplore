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
#pragma endregion 
};
