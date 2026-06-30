#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UERootGameMode.generated.h"

class ASpatialRootHostTestActor;
class UUERootControlPanel;

UCLASS()
class UEROOT_API AUERootGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AUERootGameMode();

    UPROPERTY(EditDefaultsOnly, Category = "Spatial Root")
    TSubclassOf<ASpatialRootHostTestActor> TestActorClass;

    UPROPERTY(EditDefaultsOnly, Category = "Spatial Root")
    TSubclassOf<UUERootControlPanel> ControlPanelClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spatial Root")
    FString DefaultAdmBwfPath = TEXT("/Users/lucian/projects/spatialroot/sourceData/CANYON-ATMOS-LFE.wav");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spatial Root")
    FString DefaultLusidScenePath = TEXT("/Users/lucian/projects/spatialroot/sourceData/lusid_package/scene.lusid.json");

protected:
    void BeginPlay() override;

private:
    UPROPERTY()
    TObjectPtr<ASpatialRootHostTestActor> TestActor;

    UPROPERTY()
    TObjectPtr<UUERootControlPanel> ControlPanel;
};
