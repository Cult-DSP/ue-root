#include "UERootControlPanel.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ScrollBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "SpatialRootBridge.h"
#include "SpatialRootHostTestActor.h"

namespace
{
UTextBlock* MakeText(UWidgetTree* Tree, const FString& Text, float Size = 14.0f)
{
    UTextBlock* TextBlock = Tree->ConstructWidget<UTextBlock>();
    TextBlock->SetText(FText::FromString(Text));
    TextBlock->SetAutoWrapText(true);
    FSlateFontInfo Font = TextBlock->GetFont();
    Font.Size = Size;
    TextBlock->SetFont(Font);
    return TextBlock;
}

UEditableTextBox* MakeTextBox(UWidgetTree* Tree, const FString& Text)
{
    UEditableTextBox* TextBox = Tree->ConstructWidget<UEditableTextBox>();
    TextBox->SetText(FText::FromString(Text));
    TextBox->SetSelectAllTextWhenFocused(true);
    return TextBox;
}

UButton* MakeButton(UWidgetTree* Tree, const FString& Label)
{
    UButton* Button = Tree->ConstructWidget<UButton>();
    UTextBlock* ButtonText = MakeText(Tree, Label, 13.0f);
    ButtonText->SetJustification(ETextJustify::Center);
    Button->AddChild(ButtonText);
    return Button;
}

void AddPaddedChild(UVerticalBox* Box, UWidget* Widget, const FMargin& Padding = FMargin(8.0f, 3.0f))
{
    UVerticalBoxSlot* Slot = Box->AddChildToVerticalBox(Widget);
    Slot->SetPadding(Padding);
}

void AddRow(UWidgetTree* Tree, UVerticalBox* Box, const FString& Label, UWidget* Field)
{
    UHorizontalBox* Row = Tree->ConstructWidget<UHorizontalBox>();
    UTextBlock* LabelText = MakeText(Tree, Label, 13.0f);
    UHorizontalBoxSlot* LabelSlot = Row->AddChildToHorizontalBox(LabelText);
    LabelSlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
    LabelSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
    LabelSlot->SetHorizontalAlignment(HAlign_Fill);

    UHorizontalBoxSlot* FieldSlot = Row->AddChildToHorizontalBox(Field);
    FieldSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
    FieldSlot->SetHorizontalAlignment(HAlign_Fill);

    AddPaddedChild(Box, Row);
}
}

void UUERootControlPanel::SetHostActor(ASpatialRootHostTestActor* InHostActor)
{
    HostActor = InHostActor;
    SyncFieldsFromActor();
    UpdateDiagnosticsText();
}

void UUERootControlPanel::NativeConstruct()
{
    Super::NativeConstruct();
    BuildWidgetTree();
    SyncFieldsFromActor();
    UpdateDiagnosticsText();
}

void UUERootControlPanel::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    UpdateDiagnosticsText();
}

void UUERootControlPanel::BuildWidgetTree()
{
    if (!WidgetTree)
    {
        return;
    }

    UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("SpatialRootControlPanelCanvas"));
    WidgetTree->RootWidget = Canvas;

    UBorder* PanelBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("SpatialRootControlPanelBorder"));
    PanelBorder->SetPadding(FMargin(10.0f));
    PanelBorder->SetBrushColor(FLinearColor(0.02f, 0.025f, 0.03f, 0.86f));

    UCanvasPanelSlot* PanelSlot = Canvas->AddChildToCanvas(PanelBorder);
    PanelSlot->SetAnchors(FAnchors(0.0f, 0.0f));
    PanelSlot->SetAlignment(FVector2D(0.0f, 0.0f));
    PanelSlot->SetPosition(FVector2D(24.0f, 24.0f));
    PanelSlot->SetSize(FVector2D(720.0f, 820.0f));

    UScrollBox* Scroll = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("SpatialRootControlPanel"));
    PanelBorder->SetContent(Scroll);

    UVerticalBox* RootBox = WidgetTree->ConstructWidget<UVerticalBox>();
    Scroll->AddChild(RootBox);

    AddPaddedChild(RootBox, MakeText(WidgetTree, TEXT("Spatial Root Host"), 20.0f), FMargin(8.0f, 8.0f, 8.0f, 4.0f));

    AdmPathTextBox = MakeTextBox(WidgetTree, TEXT(""));
    LusidScenePathTextBox = MakeTextBox(WidgetTree, TEXT(""));
    LayoutPathTextBox = MakeTextBox(WidgetTree, TEXT(""));
    AddRow(WidgetTree, RootBox, TEXT("ADM/BW64"), AdmPathTextBox);
    AddRow(WidgetTree, RootBox, TEXT("LUSID scene"), LusidScenePathTextBox);
    AddRow(WidgetTree, RootBox, TEXT("Layout JSON"), LayoutPathTextBox);

    UHorizontalBox* TransportRow = WidgetTree->ConstructWidget<UHorizontalBox>();
    UButton* StartToneButton = MakeButton(WidgetTree, TEXT("Start Tone"));
    UButton* StopToneButton = MakeButton(WidgetTree, TEXT("Stop Tone"));
    UButton* StartHostBusButton = MakeButton(WidgetTree, TEXT("Start"));
    UButton* PauseButton = MakeButton(WidgetTree, TEXT("Pause"));
    UButton* ResumeButton = MakeButton(WidgetTree, TEXT("Resume"));
    UButton* StopButton = MakeButton(WidgetTree, TEXT("Stop"));
    StartToneButton->OnClicked.AddDynamic(this, &UUERootControlPanel::OnStartTestToneClicked);
    StopToneButton->OnClicked.AddDynamic(this, &UUERootControlPanel::OnStopTestToneClicked);
    StartHostBusButton->OnClicked.AddDynamic(this, &UUERootControlPanel::OnStartHostBusClicked);
    PauseButton->OnClicked.AddDynamic(this, &UUERootControlPanel::OnPauseClicked);
    ResumeButton->OnClicked.AddDynamic(this, &UUERootControlPanel::OnResumeClicked);
    StopButton->OnClicked.AddDynamic(this, &UUERootControlPanel::OnStopClicked);

    for (UButton* Button : {StartToneButton, StopToneButton, StartHostBusButton, PauseButton, ResumeButton, StopButton})
    {
        UHorizontalBoxSlot* Slot = TransportRow->AddChildToHorizontalBox(Button);
        Slot->SetPadding(FMargin(2.0f));
        Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
    }
    AddPaddedChild(RootBox, TransportRow);

    MasterGainSlider = WidgetTree->ConstructWidget<USlider>();
    DbapFocusSlider = WidgetTree->ConstructWidget<USlider>();
    SpeakerMixSlider = WidgetTree->ConstructWidget<USlider>();
    SubMixSlider = WidgetTree->ConstructWidget<USlider>();
    MasterGainSlider->SetValue(60.0f / 72.0f);
    DbapFocusSlider->SetValue((1.5f - 0.1f) / 4.9f);
    SpeakerMixSlider->SetValue(60.0f / 72.0f);
    SubMixSlider->SetValue(60.0f / 72.0f);
    MasterGainSlider->OnValueChanged.AddDynamic(this, &UUERootControlPanel::OnMasterGainChanged);
    DbapFocusSlider->OnValueChanged.AddDynamic(this, &UUERootControlPanel::OnDbapFocusChanged);
    SpeakerMixSlider->OnValueChanged.AddDynamic(this, &UUERootControlPanel::OnSpeakerMixChanged);
    SubMixSlider->OnValueChanged.AddDynamic(this, &UUERootControlPanel::OnSubMixChanged);
    AddRow(WidgetTree, RootBox, TEXT("Master gain dB"), MasterGainSlider);
    AddRow(WidgetTree, RootBox, TEXT("DBAP focus"), DbapFocusSlider);
    AddRow(WidgetTree, RootBox, TEXT("Speaker mix dB"), SpeakerMixSlider);
    AddRow(WidgetTree, RootBox, TEXT("Sub mix dB"), SubMixSlider);

    UHorizontalBox* ApplyRow = WidgetTree->ConstructWidget<UHorizontalBox>();
    LiveUpdateCheckBox = WidgetTree->ConstructWidget<UCheckBox>();
    LiveUpdateCheckBox->OnCheckStateChanged.AddDynamic(this, &UUERootControlPanel::OnLiveUpdateChanged);
    UButton* ApplyParamsButton = MakeButton(WidgetTree, TEXT("Apply Params"));
    ApplyParamsButton->OnClicked.AddDynamic(this, &UUERootControlPanel::OnApplyParamsClicked);
    ApplyRow->AddChildToHorizontalBox(MakeText(WidgetTree, TEXT("Live update"), 13.0f));
    ApplyRow->AddChildToHorizontalBox(LiveUpdateCheckBox);
    UHorizontalBoxSlot* ApplySlot = ApplyRow->AddChildToHorizontalBox(ApplyParamsButton);
    ApplySlot->SetPadding(FMargin(12.0f, 0.0f, 0.0f, 0.0f));
    ApplySlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
    AddPaddedChild(RootBox, ApplyRow);

    DiagnosticsText = MakeText(WidgetTree, TEXT("Diagnostics pending."), 12.0f);
    AddPaddedChild(RootBox, MakeText(WidgetTree, TEXT("Diagnostics"), 16.0f), FMargin(8.0f, 12.0f, 8.0f, 2.0f));
    AddPaddedChild(RootBox, DiagnosticsText);

    UE_LOG(LogTemp, Log, TEXT("UERootControlPanel built native widget tree."));
}

void UUERootControlPanel::SyncFieldsFromActor()
{
    if (!HostActor)
    {
        return;
    }

    if (AdmPathTextBox)
    {
        AdmPathTextBox->SetText(FText::FromString(HostActor->AdmBwfPath));
    }
    if (LusidScenePathTextBox)
    {
        LusidScenePathTextBox->SetText(FText::FromString(HostActor->LusidScenePath));
    }
    if (LayoutPathTextBox)
    {
        LayoutPathTextBox->SetText(FText::FromString(HostActor->LayoutPath));
    }
}

void UUERootControlPanel::ApplyPathFields()
{
    if (!HostActor)
    {
        return;
    }

    HostActor->AdmBwfPath = AdmPathTextBox ? AdmPathTextBox->GetText().ToString() : HostActor->AdmBwfPath;
    HostActor->LusidScenePath = LusidScenePathTextBox ? LusidScenePathTextBox->GetText().ToString() : HostActor->LusidScenePath;
    HostActor->LayoutPath = LayoutPathTextBox ? LayoutPathTextBox->GetText().ToString() : HostActor->LayoutPath;
}

void UUERootControlPanel::ApplyRuntimeParams()
{
    if (HostActor && HostActor->GetBridge())
    {
        HostActor->GetBridge()->SetMasterGainDb(MasterGainDb);
        HostActor->GetBridge()->SetDbapFocus(DbapFocus);
        HostActor->GetBridge()->SetSpeakerMix(SpeakerMixDb);
        HostActor->GetBridge()->SetSubMix(SubMixDb);
    }
}

void UUERootControlPanel::UpdateDiagnosticsText()
{
    if (!DiagnosticsText)
    {
        return;
    }

    if (!HostActor)
    {
        DiagnosticsText->SetText(FText::FromString(TEXT("No Spatial Root host actor is available.")));
        return;
    }

    const FSpatialRootDiagnostics Diagnostics = HostActor->GetDiagnostics();
    const FString Message = FString::Printf(
        TEXT("Engine initialized: %s\nADM: %s\nLUSID: %s\nLayout: %s\nADM loaded: %s\nLUSID loaded: %s\nLayout loaded: %s\nTransport: %d\nUE sample rate: %d\nUE output channels: %d\nSpatial Root required channels: %d\nRendered channels: %d\nUnderruns/xruns: %d\nAudio path mode: %d\nLast operation: %s\nLast error: %s\nLast warning: %s"),
        Diagnostics.bEngineInitialized ? TEXT("true") : TEXT("false"),
        *Diagnostics.AdmPath,
        *Diagnostics.LusidScenePath,
        *Diagnostics.LayoutPath,
        Diagnostics.bAdmLoaded ? TEXT("true") : TEXT("false"),
        Diagnostics.bLusidSceneLoaded ? TEXT("true") : TEXT("false"),
        Diagnostics.bLayoutLoaded ? TEXT("true") : TEXT("false"),
        static_cast<int32>(Diagnostics.TransportState),
        Diagnostics.UnrealSampleRate,
        Diagnostics.UnrealOutputChannelCount,
        Diagnostics.SpatialRootRequiredOutputChannelCount,
        Diagnostics.RenderedChannelCount,
        Diagnostics.UnderrunCount,
        static_cast<int32>(Diagnostics.AudioPathMode),
        *Diagnostics.LastOperation,
        *Diagnostics.LastError,
        *Diagnostics.LastWarning);
    DiagnosticsText->SetText(FText::FromString(Message));
}

float UUERootControlPanel::SliderToDb(float Value) const
{
    return FMath::Lerp(-60.0f, 12.0f, Value);
}

float UUERootControlPanel::SliderToFocus(float Value) const
{
    return FMath::Lerp(0.1f, 5.0f, Value);
}

void UUERootControlPanel::OnStartTestToneClicked()
{
    if (HostActor)
    {
        HostActor->StartTestTone();
    }
}

void UUERootControlPanel::OnStopTestToneClicked()
{
    if (HostActor)
    {
        HostActor->StopTestTone();
    }
}

void UUERootControlPanel::OnStartHostBusClicked()
{
    if (HostActor)
    {
        ApplyPathFields();
        ApplyRuntimeParams();
        HostActor->StartSpatialRootHostBus();
    }
}

void UUERootControlPanel::OnPauseClicked()
{
    if (HostActor)
    {
        HostActor->PauseSpatialRoot();
    }
}

void UUERootControlPanel::OnResumeClicked()
{
    if (HostActor)
    {
        HostActor->ResumeSpatialRoot();
    }
}

void UUERootControlPanel::OnStopClicked()
{
    if (HostActor)
    {
        HostActor->StopSpatialRootHostBus();
        HostActor->StopTestTone();
    }
}

void UUERootControlPanel::OnApplyParamsClicked()
{
    ApplyPathFields();
    ApplyRuntimeParams();
}

void UUERootControlPanel::OnLiveUpdateChanged(bool bIsChecked)
{
    bLiveUpdate = bIsChecked;
}

void UUERootControlPanel::OnMasterGainChanged(float Value)
{
    MasterGainDb = SliderToDb(Value);
    if (bLiveUpdate)
    {
        ApplyRuntimeParams();
    }
}

void UUERootControlPanel::OnDbapFocusChanged(float Value)
{
    DbapFocus = SliderToFocus(Value);
    if (bLiveUpdate)
    {
        ApplyRuntimeParams();
    }
}

void UUERootControlPanel::OnSpeakerMixChanged(float Value)
{
    SpeakerMixDb = SliderToDb(Value);
    if (bLiveUpdate)
    {
        ApplyRuntimeParams();
    }
}

void UUERootControlPanel::OnSubMixChanged(float Value)
{
    SubMixDb = SliderToDb(Value);
    if (bLiveUpdate)
    {
        ApplyRuntimeParams();
    }
}
