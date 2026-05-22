#include "SpatialRootSubsystem.h"

void USpatialRootSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    Bridge = NewObject<USpatialRootBridge>(this);
    Bridge->Initialize();
}

void USpatialRootSubsystem::Deinitialize()
{
    if (Bridge)
    {
        Bridge->Shutdown();
    }
    Bridge = nullptr;
    Super::Deinitialize();
}

USpatialRootBridge* USpatialRootSubsystem::GetBridge()
{
    if (!Bridge)
    {
        Bridge = NewObject<USpatialRootBridge>(this);
        Bridge->Initialize();
    }
    return Bridge;
}

FSpatialRootDiagnostics USpatialRootSubsystem::GetDiagnostics() const
{
    return Bridge ? Bridge->GetDiagnostics() : FSpatialRootDiagnostics();
}

