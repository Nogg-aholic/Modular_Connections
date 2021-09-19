

#pragma once

#include "CoreMinimal.h"

#include "Hologram/FGHologram.h"
#include "Subsystems/WorldSubsystem.h"
#include "ModularConnectionsSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MODULAR_CONNECTIONS_API UModularConnectionsSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	int32 RotationalStep;
	int32 RotationalStepYaw;
	int32 OffsetStep;

	UPROPERTY()
	TMap<AFGHologram * ,FHitResult>  LastHitResults;
};
