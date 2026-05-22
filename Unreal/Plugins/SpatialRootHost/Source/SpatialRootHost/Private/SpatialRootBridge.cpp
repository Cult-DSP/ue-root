#include "SpatialRootBridge.h"

#include "SpatialRootHostModule.h"

bool USpatialRootBridge::Initialize()
{
    SetLastOperation(TEXT("Initialize"));
    Diagnostics.bEngineInitialized = true;
    Diagnostics.AudioPathMode = ESpatialRootAudioPathMode::StubTestGeneratorOnly;
    Diagnostics.LastError = TEXT("Spatial Root EngineSession is not linked yet; bridge is running as a documented stub.");
    UE_LOG(LogSpatialRootHost, Warning, TEXT("%s"), *Diagnostics.LastError);
    return true;
}

bool USpatialRootBridge::LoadAdmBwf(const FString& AdmPath)
{
    SetLastOperation(TEXT("LoadAdmBwf"));
    Diagnostics.AdmPath = AdmPath;
    Diagnostics.bAdmLoaded = false;
    return Fail(TEXT("LoadAdmBwf"), TEXT("ADM/BW64 loading is pending EngineSessionCore linking."));
}

bool USpatialRootBridge::LoadLayout(const FString& LayoutPath)
{
    SetLastOperation(TEXT("LoadLayout"));
    Diagnostics.LayoutPath = LayoutPath;
    Diagnostics.bLayoutLoaded = false;
    return Fail(TEXT("LoadLayout"), TEXT("Layout loading is pending EngineSessionCore linking."));
}

bool USpatialRootBridge::Start()
{
    SetLastOperation(TEXT("Start"));
    Diagnostics.TransportState = ESpatialRootTransportState::Stopped;
    return Fail(TEXT("Start"), TEXT("Spatial Root audio is not connected to Unreal yet; use the test-tone component for Unreal audio-path validation."));
}

void USpatialRootBridge::Pause()
{
    SetLastOperation(TEXT("Pause"));
    Diagnostics.TransportState = ESpatialRootTransportState::Paused;
}

void USpatialRootBridge::Resume()
{
    SetLastOperation(TEXT("Resume"));
    Diagnostics.TransportState = ESpatialRootTransportState::Running;
}

void USpatialRootBridge::Stop()
{
    SetLastOperation(TEXT("Stop"));
    Diagnostics.TransportState = ESpatialRootTransportState::Stopped;
}

void USpatialRootBridge::Shutdown()
{
    SetLastOperation(TEXT("Shutdown"));
    Diagnostics.bEngineInitialized = false;
    Diagnostics.TransportState = ESpatialRootTransportState::Stopped;
}

void USpatialRootBridge::SetMasterGainDb(float Db)
{
    SetLastOperation(TEXT("SetMasterGainDb"));
    MasterGainDb = Db;
}

void USpatialRootBridge::SetDbapFocus(float Focus)
{
    SetLastOperation(TEXT("SetDbapFocus"));
    DbapFocus = FMath::Max(0.1f, Focus);
}

void USpatialRootBridge::SetSpeakerMix(float Mix)
{
    SetLastOperation(TEXT("SetSpeakerMix"));
    SpeakerMix = Mix;
}

void USpatialRootBridge::SetSubMix(float Mix)
{
    SetLastOperation(TEXT("SetSubMix"));
    SubMix = Mix;
}

FString USpatialRootBridge::GetLastError() const
{
    return Diagnostics.LastError;
}

FSpatialRootDiagnostics USpatialRootBridge::GetDiagnostics() const
{
    return Diagnostics;
}

void USpatialRootBridge::SetLastOperation(const FString& Operation)
{
    Diagnostics.LastOperation = Operation;
}

bool USpatialRootBridge::Fail(const FString& Operation, const FString& Error)
{
    Diagnostics.LastOperation = Operation;
    Diagnostics.LastError = Error;
    Diagnostics.AudioPathMode = ESpatialRootAudioPathMode::FailedNotConnected;
    UE_LOG(LogSpatialRootHost, Warning, TEXT("%s failed: %s"), *Operation, *Error);
    return false;
}

