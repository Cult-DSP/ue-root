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

    UWorld* World = GetWorld();
    if (!World)
    {
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
        }
    }

    APlayerController* PlayerController = World->GetFirstPlayerController();
    if (!ControlPanel && PlayerController && ControlPanelClass)
    {
        ControlPanel = CreateWidget<UUERootControlPanel>(PlayerController, ControlPanelClass);
        if (ControlPanel)
        {
            ControlPanel->SetHostActor(TestActor);
            ControlPanel->AddToViewport();
            PlayerController->SetShowMouseCursor(true);
            PlayerController->SetInputMode(FInputModeGameAndUI());
        }
    }
}
