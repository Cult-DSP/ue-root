#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SpatialRootBridge.h"
#include "SpatialRootSubsystem.generated.h"

UCLASS(BlueprintType)
class SPATIALROOTHOST_API USpatialRootSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    void Initialize(FSubsystemCollectionBase& Collection) override;
    void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "Spatial Root")
    USpatialRootBridge* GetBridge();

    UFUNCTION(BlueprintPure, Category = "Spatial Root")
    FSpatialRootDiagnostics GetDiagnostics() const;

private:
    UPROPERTY()
    TObjectPtr<USpatialRootBridge> Bridge;
};

