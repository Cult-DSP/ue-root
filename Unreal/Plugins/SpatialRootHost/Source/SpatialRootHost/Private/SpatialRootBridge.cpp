#include "SpatialRootBridge.h"

#include "SpatialRootHostModule.h"
#include "Dom/JsonObject.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#if SPATIALROOT_WITH_ENGINESESSION
THIRD_PARTY_INCLUDES_START
#include "EngineSession.hpp"
THIRD_PARTY_INCLUDES_END
#endif

namespace
{
constexpr int32 TranslabOutputChannels = 18;

FString SpatialRootCheckoutRoot()
{
    return FPaths::ConvertRelativePathToFull(FPaths::Combine(
        FPaths::ProjectDir(),
        TEXT("Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot")));
}
}

bool USpatialRootBridge::Initialize()
{
    SetLastOperation(TEXT("Initialize"));
    Diagnostics.AudioPathMode = ESpatialRootAudioPathMode::UEProceduralSource;
    Diagnostics.UnrealRenderBusChannelCount = TranslabOutputChannels;

    const FString DefaultLayout = GetDefaultTranslabLayoutPath();
    if (FPaths::FileExists(DefaultLayout))
    {
        Diagnostics.LayoutPath = DefaultLayout;
        UpdateLayoutDiagnostics(DefaultLayout);
    }

    if (!EnsureSession())
    {
        return false;
    }

    Diagnostics.bEngineInitialized = true;
    Diagnostics.LastError.Reset();
    return true;
}

bool USpatialRootBridge::LoadAdmBwf(const FString& AdmPath)
{
    SetLastOperation(TEXT("LoadAdmBwf"));
    Diagnostics.AdmPath = AdmPath;
    Diagnostics.bAdmLoaded = false;

    if (!FPaths::FileExists(AdmPath))
    {
        return Fail(TEXT("LoadAdmBwf"), FString::Printf(TEXT("ADM/BW64 file does not exist: %s"), *AdmPath));
    }

    Diagnostics.LastError.Reset();
    return true;
}

bool USpatialRootBridge::LoadLusidScene(const FString& ScenePath)
{
    SetLastOperation(TEXT("LoadLusidScene"));
    Diagnostics.LusidScenePath = ScenePath;
    Diagnostics.bLusidSceneLoaded = false;

    if (!FPaths::FileExists(ScenePath))
    {
        return Fail(TEXT("LoadLusidScene"), FString::Printf(TEXT("LUSID scene file does not exist: %s"), *ScenePath));
    }

    Diagnostics.LastError.Reset();
    return true;
}

bool USpatialRootBridge::LoadLayout(const FString& LayoutPath)
{
    SetLastOperation(TEXT("LoadLayout"));
    Diagnostics.LayoutPath = LayoutPath;
    Diagnostics.bLayoutLoaded = false;
    return UpdateLayoutDiagnostics(LayoutPath);
}

bool USpatialRootBridge::LoadTranslabLayout()
{
    SetLastOperation(TEXT("LoadTranslabLayout"));
    return LoadLayout(GetDefaultTranslabLayoutPath());
}

bool USpatialRootBridge::Start()
{
    SetLastOperation(TEXT("Start"));
    Diagnostics.TransportState = ESpatialRootTransportState::Stopped;

    if (!EnsureSession())
    {
        return false;
    }

    if (Diagnostics.LusidScenePath.IsEmpty())
    {
        return Fail(TEXT("Start"), TEXT("EngineSession requires a LUSID scene path before ADM direct streaming can start."));
    }
    if (Diagnostics.AdmPath.IsEmpty())
    {
        return Fail(TEXT("Start"), TEXT("ADM/BW64 path is empty."));
    }
    if (Diagnostics.LayoutPath.IsEmpty())
    {
        return Fail(TEXT("Start"), TEXT("Layout path is empty."));
    }

#if SPATIALROOT_WITH_ENGINESESSION
    ResetSession();
    if (!EnsureSession())
    {
        return false;
    }

    EngineOptions Options;
    Options.sampleRate = 48000;
    Options.bufferSize = 512;
    Options.oscPort = 0;
    if (!Session->configureEngine(Options))
    {
        return Fail(TEXT("configureEngine"), UTF8_TO_TCHAR(Session->getLastError().c_str()));
    }

    SceneInput Scene;
    Scene.scenePath = TCHAR_TO_UTF8(*Diagnostics.LusidScenePath);
    Scene.admFile = TCHAR_TO_UTF8(*Diagnostics.AdmPath);
    if (!Session->loadScene(Scene))
    {
        Diagnostics.bAdmLoaded = false;
        Diagnostics.bLusidSceneLoaded = false;
        return Fail(TEXT("loadScene"), UTF8_TO_TCHAR(Session->getLastError().c_str()));
    }
    Diagnostics.bAdmLoaded = true;
    Diagnostics.bLusidSceneLoaded = true;

    LayoutInput Layout;
    Layout.layoutPath = TCHAR_TO_UTF8(*Diagnostics.LayoutPath);
    if (!Session->applyLayout(Layout))
    {
        Diagnostics.bLayoutLoaded = false;
        return Fail(TEXT("applyLayout"), UTF8_TO_TCHAR(Session->getLastError().c_str()));
    }
    Diagnostics.bLayoutLoaded = true;
    UpdateLayoutDiagnostics(Diagnostics.LayoutPath);

    RuntimeParams Params;
    Params.masterGain = FMath::Pow(10.0f, MasterGainDb / 20.0f);
    Params.dbapFocus = DbapFocus;
    Params.speakerMixDb = SpeakerMix;
    Params.subMixDb = SubMix;
    if (!Session->configureRuntime(Params))
    {
        return Fail(TEXT("configureRuntime"), UTF8_TO_TCHAR(Session->getLastError().c_str()));
    }

    if (!Session->start())
    {
        Diagnostics.AudioPathMode = ESpatialRootAudioPathMode::SpatialRootOwnsDevice;
        return Fail(TEXT("start"), UTF8_TO_TCHAR(Session->getLastError().c_str()));
    }

    Diagnostics.TransportState = ESpatialRootTransportState::Running;
    Diagnostics.AudioPathMode = ESpatialRootAudioPathMode::SpatialRootOwnsDevice;
    Diagnostics.LastError = TEXT("EngineSession started. Current Spatial Root API owns its AlloLib device; Unreal render bus is not receiving Spatial Root PCM yet.");
    RefreshEngineStatus();
    return true;
#else
    return Fail(TEXT("Start"), TEXT("EngineSessionCore static library has not been built or linked. Run Spatial Root build.sh --engine-only inside the submodule."));
#endif
}

void USpatialRootBridge::Pause()
{
    SetLastOperation(TEXT("Pause"));
#if SPATIALROOT_WITH_ENGINESESSION
    if (Session)
    {
        Session->setPaused(true);
    }
#endif
    Diagnostics.TransportState = ESpatialRootTransportState::Paused;
    RefreshEngineStatus();
}

void USpatialRootBridge::Resume()
{
    SetLastOperation(TEXT("Resume"));
#if SPATIALROOT_WITH_ENGINESESSION
    if (Session)
    {
        Session->setPaused(false);
    }
#endif
    Diagnostics.TransportState = ESpatialRootTransportState::Running;
    RefreshEngineStatus();
}

void USpatialRootBridge::Stop()
{
    SetLastOperation(TEXT("Stop"));
#if SPATIALROOT_WITH_ENGINESESSION
    if (Session)
    {
        Session->shutdown();
    }
#endif
    Diagnostics.TransportState = ESpatialRootTransportState::Stopped;
    RefreshEngineStatus();
}

void USpatialRootBridge::Shutdown()
{
    SetLastOperation(TEXT("Shutdown"));
#if SPATIALROOT_WITH_ENGINESESSION
    if (Session)
    {
        Session->shutdown();
    }
#endif
    delete Session;
    Session = nullptr;
    Diagnostics.bEngineInitialized = false;
    Diagnostics.TransportState = ESpatialRootTransportState::Stopped;
}

FString USpatialRootBridge::GetDefaultTranslabLayoutPath() const
{
    return FPaths::Combine(
        SpatialRootCheckoutRoot(),
        TEXT("spatial_engine/speaker_layouts/translab-sono-layout.json"));
}

void USpatialRootBridge::SetMasterGainDb(float Db)
{
    SetLastOperation(TEXT("SetMasterGainDb"));
    MasterGainDb = Db;
#if SPATIALROOT_WITH_ENGINESESSION
    if (Session)
    {
        Session->setMasterGain(FMath::Pow(10.0f, MasterGainDb / 20.0f));
    }
#endif
}

void USpatialRootBridge::SetDbapFocus(float Focus)
{
    SetLastOperation(TEXT("SetDbapFocus"));
    DbapFocus = FMath::Max(0.1f, Focus);
#if SPATIALROOT_WITH_ENGINESESSION
    if (Session)
    {
        Session->setDbapFocus(DbapFocus);
    }
#endif
}

void USpatialRootBridge::SetSpeakerMix(float Mix)
{
    SetLastOperation(TEXT("SetSpeakerMix"));
    SpeakerMix = Mix;
#if SPATIALROOT_WITH_ENGINESESSION
    if (Session)
    {
        Session->setSpeakerMixDb(SpeakerMix);
    }
#endif
}

void USpatialRootBridge::SetSubMix(float Mix)
{
    SetLastOperation(TEXT("SetSubMix"));
    SubMix = Mix;
#if SPATIALROOT_WITH_ENGINESESSION
    if (Session)
    {
        Session->setSubMixDb(SubMix);
    }
#endif
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
    Diagnostics.LastError = Error.IsEmpty() ? TEXT("Unknown Spatial Root error.") : Error;
    Diagnostics.AudioPathMode = ESpatialRootAudioPathMode::FailedNotConnected;
    UE_LOG(LogSpatialRootHost, Warning, TEXT("%s failed: %s"), *Operation, *Diagnostics.LastError);
    return false;
}

void USpatialRootBridge::BeginDestroy()
{
    Shutdown();
    Super::BeginDestroy();
}

bool USpatialRootBridge::EnsureSession()
{
#if SPATIALROOT_WITH_ENGINESESSION
    if (!Session)
    {
        Session = new EngineSession();
    }
    return true;
#else
    return Fail(TEXT("EnsureSession"), TEXT("SPATIALROOT_WITH_ENGINESESSION is disabled because EngineSessionCore was not found at build time."));
#endif
}

bool USpatialRootBridge::UpdateLayoutDiagnostics(const FString& LayoutPath)
{
    if (!FPaths::FileExists(LayoutPath))
    {
        return Fail(TEXT("LoadLayout"), FString::Printf(TEXT("Layout file does not exist: %s"), *LayoutPath));
    }

    FString JsonText;
    if (!FFileHelper::LoadFileToString(JsonText, *LayoutPath))
    {
        return Fail(TEXT("LoadLayout"), FString::Printf(TEXT("Could not read layout file: %s"), *LayoutPath));
    }

    TSharedPtr<FJsonObject> Root;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
    if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
    {
        return Fail(TEXT("LoadLayout"), FString::Printf(TEXT("Could not parse layout JSON: %s"), *LayoutPath));
    }

    int32 MaxChannel = INDEX_NONE;
    int32 InternalChannels = 0;
    auto VisitChannelArray = [&MaxChannel, &InternalChannels](const TArray<TSharedPtr<FJsonValue>>* Values)
    {
        if (!Values)
        {
            return;
        }
        InternalChannels += Values->Num();
        for (const TSharedPtr<FJsonValue>& Value : *Values)
        {
            const TSharedPtr<FJsonObject> Object = Value.IsValid() ? Value->AsObject() : nullptr;
            if (!Object.IsValid())
            {
                continue;
            }
            int32 Channel = 0;
            if (Object->TryGetNumberField(TEXT("deviceChannel"), Channel) ||
                Object->TryGetNumberField(TEXT("channel"), Channel))
            {
                MaxChannel = FMath::Max(MaxChannel, Channel);
            }
        }
    };

    const TArray<TSharedPtr<FJsonValue>>* Speakers = nullptr;
    const TArray<TSharedPtr<FJsonValue>>* Subwoofers = nullptr;
    Root->TryGetArrayField(TEXT("speakers"), Speakers);
    Root->TryGetArrayField(TEXT("subwoofers"), Subwoofers);
    VisitChannelArray(Speakers);
    VisitChannelArray(Subwoofers);

    Diagnostics.SpatialRootInternalRenderBusChannelCount = InternalChannels;
    Diagnostics.SpatialRootRequiredOutputChannelCount = MaxChannel >= 0 ? MaxChannel + 1 : 0;
    Diagnostics.RenderedChannelCount = Diagnostics.SpatialRootRequiredOutputChannelCount;
    Diagnostics.UnrealRenderBusChannelCount = Diagnostics.SpatialRootRequiredOutputChannelCount;
    Diagnostics.bLayoutLoaded = false;
    Diagnostics.LastError.Reset();

    if (LayoutPath.EndsWith(TEXT("translab-sono-layout.json")) &&
        Diagnostics.SpatialRootRequiredOutputChannelCount != TranslabOutputChannels)
    {
        return Fail(TEXT("LoadLayout"), FString::Printf(
            TEXT("TransLab layout expected %d output channels, parsed %d."),
            TranslabOutputChannels,
            Diagnostics.SpatialRootRequiredOutputChannelCount));
    }

    return true;
}

void USpatialRootBridge::RefreshEngineStatus()
{
#if SPATIALROOT_WITH_ENGINESESSION
    if (!Session)
    {
        return;
    }
    const EngineStatus Status = Session->queryStatus();
    Diagnostics.UnderrunCount = static_cast<int32>(Status.xruns);
    Diagnostics.TransportState = Status.paused
        ? ESpatialRootTransportState::Paused
        : Diagnostics.TransportState;
#endif
}

void USpatialRootBridge::ResetSession()
{
#if SPATIALROOT_WITH_ENGINESESSION
    if (Session)
    {
        Session->shutdown();
        delete Session;
        Session = nullptr;
    }
#endif
}
