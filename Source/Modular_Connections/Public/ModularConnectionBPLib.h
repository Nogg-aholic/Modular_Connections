

#pragma once

#include "CoreMinimal.h"


#include "FGPipeConnectionComponent.h"
#include "Buildables/FGBuildableStorage.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Buildables/FGBuildableDockingStation.h"
#include "Buildables/FGBuildableDroneStation.h"
#include "ModularConnectionBPLib.generated.h"

/**
 * 
 */
UCLASS()
class MODULAR_CONNECTIONS_API UModularConnectionBPLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	

public:

	UFUNCTION(BlueprintCallable)
	static void AddToInputConnection(AFGBuildableStorage* Buildable, UFGFactoryConnectionComponent* Con);


	UFUNCTION(BlueprintCallable)
	static void RemoveInputConnection(AFGBuildableStorage* Buildable, UFGFactoryConnectionComponent* Con);
	
	UFUNCTION(BlueprintCallable)
	static int32 AddToDockingInputConnection(AFGBuildableDockingStation* Buildable, UFGFactoryConnectionComponent* Con, bool Storage);
	
	UFUNCTION(BlueprintCallable)
	static void RemoveDockingInputConnection(AFGBuildableDockingStation* Buildable, UFGFactoryConnectionComponent* Con);
	
	UFUNCTION(BlueprintCallable)
	static int32 AddToDroneInputConnection(AFGBuildableDroneStation* Buildable, UFGFactoryConnectionComponent* Con);
	
	UFUNCTION(BlueprintCallable)
	static void RemoveDroneInputConnection(AFGBuildableDroneStation* Buildable, UFGFactoryConnectionComponent* Con);

	UFUNCTION(BlueprintCallable)
	static UFGInventoryComponent* GetDefaultInventoryForDirection(AActor* AttachedBuilding, bool IsOut);
	
	UFUNCTION(BlueprintCallable)
	static TArray<UFGInventoryComponent * >  GetInventoriesForBuilding(AActor * AttachedBuilding);
	UFUNCTION(BlueprintCallable)
	static void InsertConnectionOnBuilding(AActor* AttachedBuilding, UFGFactoryConnectionComponent* Con, int32 Index);

	UFUNCTION(BlueprintCallable)
	static void InsertPipeConnectionOnBuilding(AActor* AttachedBuilding, UFGPipeConnectionComponent* Con);

	UFUNCTION(BlueprintCallable)
	static void RemoveConnectionOnBuilding(AActor* AttachedBuilding, UFGFactoryConnectionComponent* Con);

	UFUNCTION(BlueprintCallable)
	static void RemovePipeConnectionOnBuilding(AActor* AttachedBuilding, UFGPipeConnectionComponent* Con);
};
