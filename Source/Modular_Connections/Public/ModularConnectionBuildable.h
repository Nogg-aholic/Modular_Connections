

#pragma once

#include "CoreMinimal.h"
#include "Buildables/FGBuildable.h"
#include "FGPipeConnectionComponent.h"
#include "FGFactoryConnectionComponent.h"
#include "Buildables/FGBuildablePole.h"

#include "ModularConnectionBuildable.generated.h"

/**
 * 
 */
UCLASS()
class MODULAR_CONNECTIONS_API AModularConnectionBuildable : public AFGBuildable
{
	GENERATED_BODY()

	virtual void BeginPlay()override;

	virtual void Destroyed() override;

	virtual void PostLoadGame_Implementation(int32 saveVersion, int32 gameVersion) override;

	virtual bool ShouldSave_Implementation() const override;

public:
	
	UFUNCTION(BlueprintCallable)
	void UnRegister();
	UFUNCTION(BlueprintCallable)
    void ConnectToMachine();
	UFUNCTION(BlueprintCallable)
    void SetupPipe(UFGPipeConnectionComponent* Con, UFGInventoryComponent* Inventory, int32 Index) const;
	UFUNCTION(BlueprintCallable)
    void SetupConveyor(UFGFactoryConnectionComponent* Con, UFGInventoryComponent* Inventory, int32 Index) const;

	UPROPERTY(SaveGame,BlueprintReadOnly)
	UFGPipeConnectionComponent * CachedPipe;
	UPROPERTY(SaveGame, BlueprintReadOnly)
	UFGFactoryConnectionComponent* CachedBelt;
	UPROPERTY(SaveGame)
	UStaticMeshComponent * MeshProxy;
	UPROPERTY(BlueprintReadWrite,SaveGame)
	AActor * AttachedBuilding;
	
	UPROPERTY(BlueprintReadWrite,SaveGame)
	int32 IndexOverride = -1;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	int32 InventoryIndex = -1;
};
