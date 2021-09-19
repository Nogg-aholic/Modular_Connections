#include "ModularConnectionBPLib.h"


#include "FGFactoryConnectionComponent.h"
#include "Buildables/FGBuildableGeneratorNuclear.h"
#include "Buildables/FGBuildableManufacturer.h"
#include "Buildables/FGBuildableResourceExtractor.h"
#include "Buildables/FGBuildableResourceSink.h"
#include "Buildables/FGBuildableTrainPlatformCargo.h"


void UModularConnectionBPLib::AddToInputConnection(AFGBuildableStorage* Buildable, UFGFactoryConnectionComponent* Con)
{
	if (Buildable && Con)
		Buildable->mCachedInputConnections.Add(Con);
}

void UModularConnectionBPLib::RemoveInputConnection(AFGBuildableStorage* Buildable, UFGFactoryConnectionComponent* Con)
{
	if (Buildable && Buildable->mCachedInputConnections.Contains(Con))
		Buildable->mCachedInputConnections.Remove(Con);
}

int32 UModularConnectionBPLib::AddToDockingInputConnection(AFGBuildableDockingStation* Buildable,
                                                           UFGFactoryConnectionComponent* Con, bool Storage)
{
	if (Buildable && Con)
	{
		if (Storage)
		{
			if (Buildable->mFuelConnections.Contains(Con))
			{
				Buildable->mFuelConnections.Remove(Con);
			}
			Buildable->mStorageInputConnections.Add(Con);
			return Buildable->mStorageInputConnections.Find(Con);
		}
		else
		{
			if (Buildable->mStorageInputConnections.Contains(Con))
			{
				Buildable->mStorageInputConnections.Remove(Con);
			}
			Buildable->mFuelConnections.Add(Con);
			return Buildable->mFuelConnections.Find(Con);
		}
	}
	return -1;
}

void UModularConnectionBPLib::RemoveDockingInputConnection(AFGBuildableDockingStation* Buildable,
                                                           UFGFactoryConnectionComponent* Con)
{
	if (!Buildable)
		return;

	if (Buildable->mStorageInputConnections.Contains(Con))
	{
		Buildable->mStorageInputConnections.Remove(Con);
	}
	if (Buildable->mFuelConnections.Contains(Con))
	{
		Buildable->mFuelConnections.Remove(Con);
	}
}

int32 UModularConnectionBPLib::AddToDroneInputConnection(AFGBuildableDroneStation* Buildable,
                                                         UFGFactoryConnectionComponent* Con)
{
	if (Buildable && Con)
	{
		Buildable->mStorageInputConnections.Add(Con);
		return Buildable->mStorageInputConnections.Find(Con);
	}
	return -1;
}

void UModularConnectionBPLib::RemoveDroneInputConnection(AFGBuildableDroneStation* Buildable,
                                                         UFGFactoryConnectionComponent* Con)
{
	if (Buildable)
		if (Buildable->mStorageInputConnections.Contains(Con))
		{
			Buildable->mStorageInputConnections.Remove(Con);
		}
}


UFGInventoryComponent* UModularConnectionBPLib::GetDefaultInventoryForDirection(
	AActor* AttachedBuilding, const bool IsOut)
{
	if(!AttachedBuilding)
		return nullptr;
	const bool Auth = AttachedBuilding->HasAuthority();

	if (auto* Manufacturer = Cast<AFGBuildableManufacturer>(AttachedBuilding))
	{
		if (IsOut)
			return Auth ? Manufacturer->mOutputInventory : Manufacturer->GetOutputInventory();

		return Manufacturer->mInputInventory;
	}
	if (auto* Storage = Cast<AFGBuildableStorage>(AttachedBuilding))
	{
		return Auth ?  Storage->mStorageInventory :Storage->GetStorageInventory();
	}
	if (auto* TrainStation = Cast<AFGBuildableTrainPlatformCargo>(AttachedBuilding))
	{
		return  Auth ? TrainStation->mInventory : TrainStation->GetInventory();
	}
	if (auto* DockingStation = Cast<AFGBuildableDockingStation>(AttachedBuilding))
	{
		return Auth ? DockingStation->mInventory :  DockingStation->GetInventory();
	}
	if (auto* DroneStation = Cast<AFGBuildableDroneStation>(AttachedBuilding))
	{
		if (IsOut)
		{
			return Auth ? DroneStation->mOutputInventory : DroneStation->GetOutputInventory();
		}
		else
		{
			return Auth ? DroneStation->mInputInventory : DroneStation->GetInputInventory();
		}
	}
	if (auto* NuclearGenerator = Cast<AFGBuildableGeneratorNuclear>(AttachedBuilding))
	{
		if (IsOut)
		{
			return Auth ? NuclearGenerator->mOutputInventory : NuclearGenerator->mOutputInventoryHandler->GetActiveInventoryComponent();
		}
		else
			return Auth ?  NuclearGenerator->mFuelInventory : NuclearGenerator->GetFuelInventory();
	}
	if (auto* FuelGenerator = Cast<AFGBuildableGeneratorFuel>(AttachedBuilding))
	{
		return Auth ?  FuelGenerator->mFuelInventory : FuelGenerator->GetFuelInventory();
	}
	if (auto* Sink = Cast<AFGBuildableResourceSink>(AttachedBuilding))
	{
		// no inventory
	}
	else if (auto* Extractor = Cast<AFGBuildableResourceExtractor>(AttachedBuilding))
	{
		return Auth ? Extractor->mOutputInventory : Extractor->GetOutputInventory();
	}
	else if (auto* BuildableFactory = Cast<AFGBuildableFactory>(AttachedBuilding))
	{
		if (Auth)
		{
			auto* Ignore = BuildableFactory->mInventoryPotential;
			TArray<UFGInventoryComponent*> Arr;
			BuildableFactory->GetComponents<UFGInventoryComponent>(Arr);
			for (auto i : Arr)
			{
				if (i == Ignore)
					continue;
				return i;
			}
		}
		else
		{
			auto* Ignore = BuildableFactory->GetPotentialInventory();
			TArray<UFGReplicationDetailInventoryComponent*> Arr;
			BuildableFactory->GetComponents<UFGReplicationDetailInventoryComponent>(Arr);
			for (auto i : Arr)
			{
				if (!i)
					continue;

				if (i->GetActiveInventoryComponent() == Ignore)
					continue;
				return i->GetActiveInventoryComponent();
			}
		}
	}
	else if (AttachedBuilding)
	{
		if (Auth)
		{
			TArray<UFGInventoryComponent*> Arr;
			AttachedBuilding->GetComponents<UFGInventoryComponent>(Arr);
			for (auto i : Arr)
			{
				if (!i)
					continue;
				return i;
			}
		}
		else
		{
			TArray<UFGReplicationDetailInventoryComponent*> Arr;
			BuildableFactory->GetComponents<UFGReplicationDetailInventoryComponent>(Arr);
			for (auto i : Arr)
			{
				if (!i)
					continue;
				return i->GetActiveInventoryComponent();
			}
		}
	}
	return nullptr;
}


TArray<UFGInventoryComponent*> UModularConnectionBPLib::GetInventoriesForBuilding(AActor* AttachedBuilding)
{
	TArray<UFGInventoryComponent*> Inventories;
	if (!AttachedBuilding)
		return Inventories;

	const bool Auth = AttachedBuilding->HasAuthority();

	if (auto* Manufacturer = Cast<AFGBuildableManufacturer>(AttachedBuilding))
	{
		Inventories.Add(Auth ? Manufacturer->mInputInventory: Manufacturer->GetInputInventory());
		Inventories.Add(Auth ? Manufacturer->mOutputInventory :Manufacturer->GetOutputInventory());
	}
	else if (auto* Storage = Cast<AFGBuildableStorage>(AttachedBuilding))
	{
		Inventories.Add(Auth ? Storage->mStorageInventory : Storage->GetStorageInventory());
	}
	else if (auto* TrainStation = Cast<AFGBuildableTrainPlatformCargo>(AttachedBuilding))
	{
		Inventories.Add(Auth ? TrainStation->mInventory: TrainStation->GetInventory());
	}
	else if (auto* DockingStation = Cast<AFGBuildableDockingStation>(AttachedBuilding))
	{
		Inventories.Add(Auth ? DockingStation->mInventory: DockingStation->GetInventory());
		Inventories.Add(Auth ? DockingStation->mFuelInventory: DockingStation->GetFuelInventory());
	}
	else if (auto* DroneStation = Cast<AFGBuildableDroneStation>(AttachedBuilding))
	{
		Inventories.Add(Auth ? DroneStation->mInputInventory: DroneStation->GetInputInventory());
		Inventories.Add(Auth ? DroneStation->mOutputInventory : DroneStation->GetOutputInventory());
		Inventories.Add(Auth ? DroneStation->mBatteryInventory : DroneStation->GetBatteryInventory());
	}
	else if (auto* NuclearGenerator = Cast<AFGBuildableGeneratorNuclear>(AttachedBuilding))
	{
		Inventories.Add(Auth ? NuclearGenerator->mFuelInventory : NuclearGenerator->GetFuelInventory());
		Inventories.Add(Auth ? NuclearGenerator->mOutputInventory: NuclearGenerator->mOutputInventoryHandler->GetActiveInventoryComponent());
	}
	else if (auto* FuelGenerator = Cast<AFGBuildableGeneratorFuel>(AttachedBuilding))
	{
		Inventories.Add(Auth ? FuelGenerator->mFuelInventory : FuelGenerator->GetFuelInventory());
	}
	else if (auto* Sink = Cast<AFGBuildableResourceSink>(AttachedBuilding))
	{
		// no inventory
	}
	else if (auto* Extractor = Cast<AFGBuildableResourceExtractor>(AttachedBuilding))
	{
		Inventories.Add(Auth? Extractor->mOutputInventory : Extractor->GetOutputInventory());
	}
	else if (auto* BuildableFactory = Cast<AFGBuildableFactory>(AttachedBuilding))
	{
		
		if (Auth)
		{
			auto* Ignore = BuildableFactory->mInventoryPotential;
			TArray<UFGInventoryComponent*> Arr;
			BuildableFactory->GetComponents<UFGInventoryComponent>(Arr);
			for (auto i : Arr)
			{
				if (i == Ignore)
					continue;
				Inventories.Add(i);
			}
		}
		else
		{
			auto* Ignore = BuildableFactory->GetPotentialInventory();
			TArray<UFGReplicationDetailInventoryComponent*> Arr;
			BuildableFactory->GetComponents<UFGReplicationDetailInventoryComponent>(Arr);
			for (auto i : Arr)
			{
				if (!i)
					continue;

				if (i->GetActiveInventoryComponent() == Ignore)
					continue;
				Inventories.Add(i->GetActiveInventoryComponent());
			}
		}
		
	}
	else if (AttachedBuilding)
	{
		if (Auth)
		{
			TArray<UFGInventoryComponent*> Arr;
			AttachedBuilding->GetComponents<UFGInventoryComponent>(Arr);
			for (auto i : Arr)
			{
				if (!i)
					continue;
				Inventories.Add(i);
			}
		}
		else
		{
			TArray<UFGReplicationDetailInventoryComponent*> Arr;
			BuildableFactory->GetComponents<UFGReplicationDetailInventoryComponent>(Arr);
			for (auto i : Arr)
			{
				if (!i)
					continue;
				Inventories.Add(i->GetActiveInventoryComponent());
			}
		}
	}
	return Inventories;
}


void UModularConnectionBPLib::InsertConnectionOnBuilding(AActor* AttachedBuilding,
                                                         UFGFactoryConnectionComponent* Con, int32 Index)
{
	bool Out = Con->GetDirection() == EFactoryConnectionDirection::FCD_OUTPUT;
	if (auto* Manu = Cast<AFGBuildableManufacturer>(AttachedBuilding))
	{
		if (Out)
		{
			if (!Manu->mFactoryOutputConnections.Contains(Con))
				Manu->mFactoryOutputConnections.Add(Con);

			if (Manu->mFactoryInputConnections.Contains(Con))
			{
				Manu->mFactoryInputConnections.Remove(Con);
			}
		}
		else
		{
			if (!Manu->mFactoryInputConnections.Contains(Con))
				Manu->mFactoryInputConnections.Add(Con);
			if (Manu->mFactoryOutputConnections.Contains(Con))
			{
				Manu->mFactoryOutputConnections.Remove(Con);
			}
		}
	}
	else if (auto* Storage = Cast<AFGBuildableStorage>(AttachedBuilding))
	{
		if (Out)
		{
			if (Storage->mCachedInputConnections.Contains(Con))
				Storage->mCachedInputConnections.Remove(Con);
		
		}
		else
		{
			if (!Storage->mCachedInputConnections.Contains(Con))
				Storage->mCachedInputConnections.Add(Con);
		}
	}
	else if (auto* TrainStation = Cast<AFGBuildableTrainPlatformCargo>(AttachedBuilding))
	{
		if (Out)
		{
			if (TrainStation->mStorageInputConnections.Contains(Con))
			{
				TrainStation->mStorageInputConnections.Remove(Con);
			}
		}
		else
		{
			if (!TrainStation->mStorageInputConnections.Contains(Con))
				TrainStation->mStorageInputConnections.Add(Con);
		}
	}
	else if (auto* DockingStation = Cast<AFGBuildableDockingStation>(AttachedBuilding))
	{
		if (Out)
		{
			if (DockingStation->mStorageInputConnections.Contains(Con))
			{
				DockingStation->mStorageInputConnections.Remove(Con);
			}
			if (DockingStation->mFuelConnections.Contains(Con))
			{
				DockingStation->mFuelConnections.Remove(Con);
			}
		}
		else
		{
			auto& Arr = Index == -1 ? DockingStation->mStorageInputConnections : DockingStation->mFuelConnections;
			auto& ArrOther = Index == -1 ? DockingStation->mFuelConnections : DockingStation->mStorageInputConnections;

			if (!Arr.Contains(Con))
				Arr.Add(Con);
			if (ArrOther.Contains(Con))
			{
				ArrOther.Remove(Con);
			}
		}
	}
	else if (auto* DroneStation = Cast<AFGBuildableDroneStation>(AttachedBuilding))
	{
		if (Out)
		{
			if (DroneStation->mStorageInputConnections.Contains(Con))
			{
				DroneStation->mStorageInputConnections.Remove(Con);
			}
		}
		else
		{
			if (!DroneStation->mStorageInputConnections.Contains(Con))
				DroneStation->mStorageInputConnections.Add(Con);
		}
	}
	else if (auto* FuelGenerator = Cast<AFGBuildableGeneratorFuel>(AttachedBuilding))
	{
		if (Out)
		{
			if (FuelGenerator->mCachedInputConnections.Contains(Con))
			{
				FuelGenerator->mCachedInputConnections.Remove(Con);
			}
		}
		else
		{
			if (!FuelGenerator->mCachedInputConnections.Contains(Con))
				FuelGenerator->mCachedInputConnections.Add(Con);
		}
	}
	else if (auto* Sink = Cast<AFGBuildableResourceSink>(AttachedBuilding))
	{
		if (Out)
		{
			if (Sink->mFactoryInputConnections.Contains(Con))
			{
				Sink->mFactoryInputConnections.Remove(Con);
			}
		}
		else
		{
			if (!Sink->mFactoryInputConnections.Contains(Con))
				Sink->mFactoryInputConnections.Add(Con);
		}
		// no inventory
	}
	else if (auto* Extractor = Cast<AFGBuildableResourceExtractor>(AttachedBuilding))
	{
		//
	}
	else if (auto* BuildableFactory = Cast<AFGBuildableFactory>(AttachedBuilding))
	{
	}
}

void UModularConnectionBPLib::InsertPipeConnectionOnBuilding(AActor* AttachedBuilding,
                                                             UFGPipeConnectionComponent* Con)
{
	bool Out = Con->GetPipeConnectionType() == EPipeConnectionType::PCT_PRODUCER;
	if (auto* Manu = Cast<AFGBuildableManufacturer>(AttachedBuilding))
	{
		if (Out)
		{
			if (!Manu->mPipeOutputConnections.Contains(Con))
				Manu->mPipeOutputConnections.Add(Con);

			if (Manu->mPipeInputConnections.Contains(Con))
			{
				Manu->mPipeInputConnections.Remove(Con);
			}
		}
		else
		{
			if (!Manu->mPipeInputConnections.Contains(Con))
				Manu->mPipeInputConnections.Add(Con);
			if (Manu->mPipeOutputConnections.Contains(Con))
			{
				Manu->mPipeOutputConnections.Remove(Con);
			}
		}
	}
	else if (auto* Storage = Cast<AFGBuildableStorage>(AttachedBuilding))
	{
	}
	else if (auto* TrainStation = Cast<AFGBuildableTrainPlatformCargo>(AttachedBuilding))
	{
		if (Out)
		{
			if (!TrainStation->mPipeOutputConnections.Contains(Con))
				TrainStation->mPipeOutputConnections.Add(Con);

			if (TrainStation->mPipeInputConnections.Contains(Con))
			{
				TrainStation->mPipeInputConnections.Remove(Con);
			}
		}
		else
		{
			if (!TrainStation->mPipeInputConnections.Contains(Con))
				TrainStation->mPipeInputConnections.Add(Con);
			if (TrainStation->mPipeOutputConnections.Contains(Con))
			{
				TrainStation->mPipeOutputConnections.Remove(Con);
			}
		}
	}
	else if (auto* DockingStation = Cast<AFGBuildableDockingStation>(AttachedBuilding))
	{
	}
	else if (auto* DroneStation = Cast<AFGBuildableDroneStation>(AttachedBuilding))
	{
	}
	else if (auto* FuelGenerator = Cast<AFGBuildableGeneratorFuel>(AttachedBuilding))
	{
		if (Out)
		{
			if (FuelGenerator->mCachedPipeInputConnections.Contains(Con))
			{
				FuelGenerator->mCachedPipeInputConnections.Remove(Con);
			}
		}
		else
		{
			if (!FuelGenerator->mCachedPipeInputConnections.Contains(Con))
				FuelGenerator->mCachedPipeInputConnections.Add(Con);
		}
	}
	else if (auto* Sink = Cast<AFGBuildableResourceSink>(AttachedBuilding))
	{
	}
	else if (auto* Extractor = Cast<AFGBuildableResourceExtractor>(AttachedBuilding))
	{
		//
		if (Out)
		{
			if (!Extractor->mPipeOutputConnections.Contains(Con))
			{
				Extractor->mPipeOutputConnections.Add(Con);
			}
		}
		else
		{
			if (Extractor->mPipeOutputConnections.Contains(Con))
				Extractor->mPipeOutputConnections.Remove(Con);
		}
	}
	else if (auto* BuildableFactory = Cast<AFGBuildableFactory>(AttachedBuilding))
	{
	}
}

void UModularConnectionBPLib::RemoveConnectionOnBuilding(AActor* AttachedBuilding,
                                                         UFGFactoryConnectionComponent* Con)
{
	if (auto* Manu = Cast<AFGBuildableManufacturer>(AttachedBuilding))
	{
			if (Manu->mFactoryInputConnections.Contains(Con))
			{
				Manu->mFactoryInputConnections.Remove(Con);
			}
	
			if (Manu->mFactoryOutputConnections.Contains(Con))
			{
				Manu->mFactoryOutputConnections.Remove(Con);
			}
		
	}
	else if (auto* Storage = Cast<AFGBuildableStorage>(AttachedBuilding))
	{
		
			if (Storage->mCachedInputConnections.Contains(Con))
				Storage->mCachedInputConnections.Remove(Con);
		
	}
	else if (auto* TrainStation = Cast<AFGBuildableTrainPlatformCargo>(AttachedBuilding))
	{
		
			if (TrainStation->mStorageInputConnections.Contains(Con))
			{
				TrainStation->mStorageInputConnections.Remove(Con);
			}
	
	}
	else if (auto* DockingStation = Cast<AFGBuildableDockingStation>(AttachedBuilding))
	{
	
			if (DockingStation->mStorageInputConnections.Contains(Con))
			{
				DockingStation->mStorageInputConnections.Remove(Con);
			}
			if (DockingStation->mFuelConnections.Contains(Con))
			{
				DockingStation->mFuelConnections.Remove(Con);
			}
	}
	else if (auto* DroneStation = Cast<AFGBuildableDroneStation>(AttachedBuilding))
	{
		
			if (DroneStation->mStorageInputConnections.Contains(Con))
			{
				DroneStation->mStorageInputConnections.Remove(Con);
			}
		
	}
	else if (auto* FuelGenerator = Cast<AFGBuildableGeneratorFuel>(AttachedBuilding))
	{
		
			if (FuelGenerator->mCachedInputConnections.Contains(Con))
			{
				FuelGenerator->mCachedInputConnections.Remove(Con);
			}
		
		
	}
	else if (auto* Sink = Cast<AFGBuildableResourceSink>(AttachedBuilding))
	{
		
			if (Sink->mFactoryInputConnections.Contains(Con))
			{
				Sink->mFactoryInputConnections.Remove(Con);
			}
		
		// no inventory
	}
}

void UModularConnectionBPLib::RemovePipeConnectionOnBuilding(AActor* AttachedBuilding,
                                                             UFGPipeConnectionComponent* Con)
{
	if (auto* Manu = Cast<AFGBuildableManufacturer>(AttachedBuilding))
	{
		if (Manu->mPipeInputConnections.Contains(Con))
		{
			Manu->mPipeInputConnections.Remove(Con);
		}

		if (Manu->mPipeOutputConnections.Contains(Con))
		{
			Manu->mPipeOutputConnections.Remove(Con);
		}
	}
	else if (auto* Storage = Cast<AFGBuildableStorage>(AttachedBuilding))
	{
	
	}
	else if (auto* TrainStation = Cast<AFGBuildableTrainPlatformCargo>(AttachedBuilding))
	{

		if (TrainStation->mPipeInputConnections.Contains(Con))
		{
			TrainStation->mPipeInputConnections.Remove(Con);
		}
	
		if (TrainStation->mPipeOutputConnections.Contains(Con))
		{
			TrainStation->mPipeOutputConnections.Remove(Con);
		}
		
	}
	else if (auto* DockingStation = Cast<AFGBuildableDockingStation>(AttachedBuilding))
	{
	}
	else if (auto* DroneStation = Cast<AFGBuildableDroneStation>(AttachedBuilding))
	{
	}
	else if (auto* FuelGenerator = Cast<AFGBuildableGeneratorFuel>(AttachedBuilding))
	{
		if (FuelGenerator->mCachedPipeInputConnections.Contains(Con))
		{
			FuelGenerator->mCachedPipeInputConnections.Remove(Con);
		}
	}
	else if (auto* Sink = Cast<AFGBuildableResourceSink>(AttachedBuilding))
	{
	}
	else if (auto* Extractor = Cast<AFGBuildableResourceExtractor>(AttachedBuilding))
	{
		//
		if (Extractor->mPipeOutputConnections.Contains(Con))
			Extractor->mPipeOutputConnections.Remove(Con);
		
	}
	else if (auto* BuildableFactory = Cast<AFGBuildableFactory>(AttachedBuilding))
	{
	}
	
}