#include "SpatialRootHostTestActor.h"

#include "Components/SceneComponent.h"
#include "SpatialRootBridge.h"
#include "SpatialRootRenderBusComponent.h"
#include "SpatialRootSubsystem.h"
#include "SpatialRootTestToneComponent.h"

ASpatialRootHostTestActor::ASpatialRootHostTestActor()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    TestTone = CreateDefaultSubobject<USpatialRootTestToneComponent>(TEXT("SpatialRootTestTone"));
    TestTone->SetupAttachment(SceneRoot);
    TestTone->bAutoActivate = false;

    RenderBus = CreateDefaultSubobject<USpatialRootRenderBusComponent>(TEXT("SpatialRootRenderBus"));
    RenderBus->SetupAttachment(SceneRoot);
    RenderBus->bAutoActivate = false;
    RenderBus->RequestedChannelCount = RequestedChannelCount;
    RenderBus->bUseSpatialRootHostBus = true;
}

void ASpatialRootHostTestActor::BeginPlay()
{
    Super::BeginPlay();
    InitializeBridge();
}

bool ASpatialRootHostTestActor::InitializeBridge()
{
    UGameInstance* GameInstance = GetGameInstance();
    if (!GameInstance)
    {
        return false;
    }

    USpatialRootSubsystem* Subsystem = GameInstance->GetSubsystem<USpatialRootSubsystem>();
    if (!Subsystem)
    {
        return false;
    }

    Bridge = Subsystem->GetBridge();
    if (!Bridge)
    {
        return false;
    }

    if (LayoutPath.IsEmpty())
    {
        LayoutPath = Bridge->GetDefaultTranslabLayoutPath();
    }

    RenderBus->Bridge = Bridge;
    RenderBus->ConfigureRenderBus(RequestedChannelCount);
    return Bridge->Initialize();
}

bool ASpatialRootHostTestActor::StartTestTone()
{
    if (!TestTone)
    {
        return false;
    }

    TestTone->Start();
    return true;
}

void ASpatialRootHostTestActor::StopTestTone()
{
    if (TestTone)
    {
        TestTone->Stop();
    }
}

bool ASpatialRootHostTestActor::StartSpatialRootHostBus()
{
    if (!InitializeBridge() || !ApplyInputPaths())
    {
        return false;
    }

    RenderBus->Bridge = Bridge;
    RenderBus->bUseSpatialRootHostBus = true;
    RenderBus->ConfigureRenderBus(RequestedChannelCount);
    RenderBus->ClearQueuedAudio();

    if (!Bridge->StartHostBus(HostBusBlockSize, RequestedChannelCount, HostBusSampleRate))
    {
        return false;
    }

    RenderBus->Start();
    return true;
}

void ASpatialRootHostTestActor::StopSpatialRootHostBus()
{
    if (RenderBus)
    {
        RenderBus->Stop();
        RenderBus->ClearQueuedAudio();
    }

    if (Bridge)
    {
        Bridge->StopHostBus();
    }
}

void ASpatialRootHostTestActor::PauseSpatialRoot()
{
    if (Bridge)
    {
        Bridge->Pause();
    }
}

void ASpatialRootHostTestActor::ResumeSpatialRoot()
{
    if (Bridge)
    {
        Bridge->Resume();
    }
}

USpatialRootBridge* ASpatialRootHostTestActor::GetBridge() const
{
    return Bridge;
}

FSpatialRootDiagnostics ASpatialRootHostTestActor::GetDiagnostics() const
{
    return Bridge ? Bridge->GetDiagnostics() : FSpatialRootDiagnostics();
}

bool ASpatialRootHostTestActor::ApplyInputPaths()
{
    if (!Bridge)
    {
        return false;
    }

    if (LayoutPath.IsEmpty())
    {
        LayoutPath = Bridge->GetDefaultTranslabLayoutPath();
    }

    return Bridge->LoadLusidScene(LusidScenePath)
        && Bridge->LoadAdmBwf(AdmBwfPath)
        && Bridge->LoadLayout(LayoutPath);
}
