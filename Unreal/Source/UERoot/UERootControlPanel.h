#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UERootControlPanel.generated.h"

class ASpatialRootHostTestActor;
class UButton;
class UCheckBox;
class UEditableTextBox;
class USlider;
class UTextBlock;

UCLASS()
class UEROOT_API UUERootControlPanel : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Spatial Root")
    void SetHostActor(ASpatialRootHostTestActor* InHostActor);

protected:
    void NativeConstruct() override;
    void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    UFUNCTION()
    void OnStartTestToneClicked();

    UFUNCTION()
    void OnStopTestToneClicked();

    UFUNCTION()
    void OnStartHostBusClicked();

    UFUNCTION()
    void OnPauseClicked();

    UFUNCTION()
    void OnResumeClicked();

    UFUNCTION()
    void OnStopClicked();

    UFUNCTION()
    void OnApplyParamsClicked();

    UFUNCTION()
    void OnLiveUpdateChanged(bool bIsChecked);

    UFUNCTION()
    void OnMasterGainChanged(float Value);

    UFUNCTION()
    void OnDbapFocusChanged(float Value);

    UFUNCTION()
    void OnSpeakerMixChanged(float Value);

    UFUNCTION()
    void OnSubMixChanged(float Value);

    void BuildWidgetTree();
    void SyncFieldsFromActor();
    void ApplyPathFields();
    void ApplyRuntimeParams();
    void UpdateDiagnosticsText();
    float SliderToDb(float Value) const;
    float SliderToFocus(float Value) const;

    UPROPERTY()
    TObjectPtr<ASpatialRootHostTestActor> HostActor;

    UPROPERTY()
    TObjectPtr<UEditableTextBox> AdmPathTextBox;

    UPROPERTY()
    TObjectPtr<UEditableTextBox> LusidScenePathTextBox;

    UPROPERTY()
    TObjectPtr<UEditableTextBox> LayoutPathTextBox;

    UPROPERTY()
    TObjectPtr<USlider> MasterGainSlider;

    UPROPERTY()
    TObjectPtr<USlider> DbapFocusSlider;

    UPROPERTY()
    TObjectPtr<USlider> SpeakerMixSlider;

    UPROPERTY()
    TObjectPtr<USlider> SubMixSlider;

    UPROPERTY()
    TObjectPtr<UCheckBox> LiveUpdateCheckBox;

    UPROPERTY()
    TObjectPtr<UTextBlock> DiagnosticsText;

    bool bLiveUpdate = false;
    float MasterGainDb = 0.0f;
    float DbapFocus = 1.5f;
    float SpeakerMixDb = 0.0f;
    float SubMixDb = 0.0f;
};
