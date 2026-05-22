#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SpatialRootDiagnostics.h"
#include "SpatialRootBridge.generated.h"

UCLASS(BlueprintType)
class SPATIALROOTHOST_API USpatialRootBridge : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Spatial Root")
    bool Initialize();

    UFUNCTION(BlueprintCallable, Category = "Spatial Root")
    bool LoadAdmBwf(const FString& AdmPath);

    UFUNCTION(BlueprintCallable, Category = "Spatial Root")
    bool LoadLayout(const FString& LayoutPath);

    UFUNCTION(BlueprintCallable, Category = "Spatial Root")
    bool Start();

    UFUNCTION(BlueprintCallable, Category = "Spatial Root")
    void Pause();

    UFUNCTION(BlueprintCallable, Category = "Spatial Root")
    void Resume();

    UFUNCTION(BlueprintCallable, Category = "Spatial Root")
    void Stop();

    UFUNCTION(BlueprintCallable, Category = "Spatial Root")
    void Shutdown();

    UFUNCTION(BlueprintCallable, Category = "Spatial Root")
    void SetMasterGainDb(float Db);

    UFUNCTION(BlueprintCallable, Category = "Spatial Root")
    void SetDbapFocus(float Focus);

    UFUNCTION(BlueprintCallable, Category = "Spatial Root")
    void SetSpeakerMix(float Mix);

    UFUNCTION(BlueprintCallable, Category = "Spatial Root")
    void SetSubMix(float Mix);

    UFUNCTION(BlueprintPure, Category = "Spatial Root")
    FString GetLastError() const;

    UFUNCTION(BlueprintPure, Category = "Spatial Root")
    FSpatialRootDiagnostics GetDiagnostics() const;

private:
    void SetLastOperation(const FString& Operation);
    bool Fail(const FString& Operation, const FString& Error);

    FSpatialRootDiagnostics Diagnostics;
    float MasterGainDb = 0.0f;
    float DbapFocus = 1.5f;
    float SpeakerMix = 0.0f;
    float SubMix = 0.0f;
};

