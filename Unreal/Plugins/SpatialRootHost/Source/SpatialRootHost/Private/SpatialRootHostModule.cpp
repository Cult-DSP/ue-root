#include "SpatialRootHostModule.h"

#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogSpatialRootHost);
DEFINE_LOG_CATEGORY(LogSpatialRootAudio);
DEFINE_LOG_CATEGORY(LogSpatialRootDiagnostics);

void FSpatialRootHostModule::StartupModule()
{
    UE_LOG(LogSpatialRootHost, Log, TEXT("SpatialRootHost module started."));
}

void FSpatialRootHostModule::ShutdownModule()
{
    UE_LOG(LogSpatialRootHost, Log, TEXT("SpatialRootHost module shut down."));
}

IMPLEMENT_MODULE(FSpatialRootHostModule, SpatialRootHost)

