#include "UERootGameMode.h"

#include "Blueprint/UserWidget.h"
#include "SpatialRootHostTestActor.h"
#include "UERootControlPanel.h"

AUERootGameMode::AUERootGameMode()
{
    TestActorClass = ASpatialRootHostTestActor::StaticClass();
    ControlPanelClass = UUERootControlPanel::StaticClass();
}

void AUERootGameMode::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Log, TEXT("UERootGameMode BeginPlay: spawning Spatial Root runtime harness."));

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("UERootGameMode BeginPlay failed: no world."));
        return;
    }

    if (!TestActor && TestActorClass)
    {
        FActorSpawnParameters SpawnParameters;
        SpawnParameters.Name = TEXT("SpatialRootHostTestActor");
        TestActor = World->SpawnActor<ASpatialRootHostTestActor>(
            TestActorClass,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            SpawnParameters);

        if (TestActor)
        {
            TestActor->AdmBwfPath = DefaultAdmBwfPath;
            TestActor->LusidScenePath = DefaultLusidScenePath;
            TestActor->InitializeBridge();
            UE_LOG(LogTemp, Log, TEXT("UERootGameMode spawned ASpatialRootHostTestActor with ADM '%s' and LUSID '%s'."),
                *DefaultAdmBwfPath,
                *DefaultLusidScenePath);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("UERootGameMode failed to spawn ASpatialRootHostTestActor."));
        }
    }

    APlayerController* PlayerController = World->GetFirstPlayerController();
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("UERootGameMode could not find a PlayerController for the control panel."));
    }
    if (!ControlPanel && PlayerController && ControlPanelClass)
    {
        ControlPanel = CreateWidget<UUERootControlPanel>(PlayerController, ControlPanelClass);
        if (ControlPanel)
        {
            ControlPanel->SetHostActor(TestActor);
            ControlPanel->AddToViewport(100);
            PlayerController->SetShowMouseCursor(true);
            PlayerController->SetInputMode(FInputModeGameAndUI());
            UE_LOG(LogTemp, Log, TEXT("UERootGameMode added UUERootControlPanel to viewport."));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("UERootGameMode failed to create UUERootControlPanel."));
        }
    }
}
