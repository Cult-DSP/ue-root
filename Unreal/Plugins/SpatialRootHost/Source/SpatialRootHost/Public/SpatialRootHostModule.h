#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSpatialRootHost, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSpatialRootAudio, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSpatialRootDiagnostics, Log, All);

class FSpatialRootHostModule final : public IModuleInterface
{
public:
    void StartupModule() override;
    void ShutdownModule() override;
};

