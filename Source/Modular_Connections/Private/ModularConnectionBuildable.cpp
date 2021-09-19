


#include "ModularConnectionBuildable.h"



#include "FGColoredInstanceMeshProxy.h"
#include "ModularConnectionBPLib.h"
#include "Modular_Connections.h"
#include "Buildables/FGBuildableTrainPlatformCargo.h"
#include "Hologram/FGBuildableDroneHologram.h"


void AModularConnectionBuildable::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle Handle;
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "ConnectToMachine");
	FTimerManager & Manager = GetWorld()->GetTimerManager();
	Manager.SetTimer(Handle,Delegate, 0.1f, false);

	
}

void AModularConnectionBuildable::Destroyed()
{
	UnRegister();
	Super::Destroyed();
}

void AModularConnectionBuildable::PostLoadGame_Implementation(int32 saveVersion, int32 gameVersion)
{
	ConnectToMachine();
}

bool AModularConnectionBuildable::ShouldSave_Implementation() const
{
	return true;
}


void AModularConnectionBuildable::UnRegister()
{
	TInlineComponentArray<UFGPipeConnectionComponent*> PrimComponents(this);
	GetComponents<UFGPipeConnectionComponent>(PrimComponents);
	TInlineComponentArray<UFGFactoryConnectionComponent*> ConveyorCons(this);
	GetComponents<UFGFactoryConnectionComponent>(ConveyorCons);
	if(CachedBelt && !ConveyorCons.Contains(CachedBelt))
	{
		ConveyorCons.Add(CachedBelt);
	}
	if(CachedPipe && !PrimComponents.Contains(CachedPipe))
	{
		PrimComponents.Add(CachedPipe);
	}
	for(auto i : PrimComponents)
	{
		UModularConnectionBPLib::RemovePipeConnectionOnBuilding(AttachedBuilding,i);
		i->AttachToComponent(GetRootComponent(),FAttachmentTransformRules::KeepWorldTransform);
		//i->*get(stealC_ownerPrivate()) = this;
		if(MeshProxy)
		{
			MeshProxy->DestroyComponent();
		}
		i->DestroyComponent();
	}

	for(auto i: ConveyorCons)
	{
		UModularConnectionBPLib::RemoveConnectionOnBuilding(AttachedBuilding,i);
		i->AttachToComponent(GetRootComponent(),FAttachmentTransformRules::KeepWorldTransform);
		//i->*get(stealC_ownerPrivate()) = this;
		if(MeshProxy)
		{
			MeshProxy->DestroyComponent();
		}
		i->DestroyComponent();
	}
}

void AModularConnectionBuildable::ConnectToMachine()
{
	if(!CachedPipe && !CachedBelt)
	{
		if(AttachedBuilding)
		{
			TArray<UFGInventoryComponent * > Arr = UModularConnectionBPLib::GetInventoriesForBuilding(AttachedBuilding);
			TInlineComponentArray<UFGPipeConnectionComponent*> PrimComponents(this);
			GetComponents<UFGPipeConnectionComponent>(PrimComponents);
			TInlineComponentArray<UFGFactoryConnectionComponent*> ConveyorCons(this);
			TInlineComponentArray<UStaticMeshComponent*> Meshs(this);

			GetComponents<UFGFactoryConnectionComponent>(ConveyorCons);
			if(CachedBelt && !ConveyorCons.Contains(CachedBelt))
			{
				ConveyorCons.Add(CachedBelt);
			}
			if(CachedPipe && !PrimComponents.Contains(CachedPipe))
			{
				PrimComponents.Add(CachedPipe);
			}
		
			for(auto i : PrimComponents)
			{
				if(!CachedPipe)
					CachedPipe = i;
				auto e =i->GetPipeConnectionType() ;
				auto Loc = i->GetComponentTransform();
				i->DestroyComponent();
				CachedPipe = NewObject<UFGPipeConnectionComponent>(AttachedBuilding, UFGPipeConnectionComponent::StaticClass(),*FString("AttachedModularConnection_").Append(*GetName()));
				CachedPipe->SetPipeConnectionType(e);
				CachedPipe->SetWorldTransform(Loc);
				CachedPipe->AttachToComponent(AttachedBuilding->GetRootComponent(),FAttachmentTransformRules::KeepWorldTransform);
				CachedPipe->SetConnectorClearance(0.93421f);
				for(auto x: Meshs)
				{
					if(Cast<UFGColoredInstanceMeshProxy>(x))
						continue;
					MeshProxy = NewObject<UStaticMeshComponent>(AttachedBuilding, UStaticMeshComponent::StaticClass(),*FString("AttachedModularConnectionMesh_").Append(*GetName()));
					MeshProxy->SetStaticMesh(x->GetStaticMesh());
					MeshProxy->AttachToComponent(CachedPipe,FAttachmentTransformRules::KeepWorldTransform);
				}
			}

			for(auto i: ConveyorCons)
			{
				if(!CachedBelt)
					CachedBelt = i;
				auto e = i->GetDirection();
				auto Loc = i->GetComponentTransform();
				i->DestroyComponent();
				CachedBelt = NewObject<UFGFactoryConnectionComponent>(AttachedBuilding, UFGFactoryConnectionComponent::StaticClass(),*FString("AttachedModularConnection_").Append(*GetName()));
				CachedBelt->SetDirection(e);
				CachedBelt->SetWorldTransform(GetTransform());
				CachedBelt->AttachToComponent(AttachedBuilding->GetRootComponent(),FAttachmentTransformRules::KeepWorldTransform);
				CachedBelt->mConnectorClearance = 99.93421f;
				//CachedBelt->mOuterBuildable = Cast<AFGBuildable>(AttachedBuilding);
				for(auto x: Meshs)
				{
					if(Cast<UFGColoredInstanceMeshProxy>(x))
						continue;
					MeshProxy = NewObject<UStaticMeshComponent>(AttachedBuilding, UStaticMeshComponent::StaticClass(),*FString("AttachedModularConnectionMesh_").Append(*GetName()));
					MeshProxy->SetStaticMesh(x->GetStaticMesh());
					MeshProxy->AttachToComponent(CachedBelt,FAttachmentTransformRules::KeepWorldTransform);
				}
			}
		}
	}
	else
	{
		TInlineComponentArray<UFGPipeConnectionComponent*> PrimComponents(this);
		GetComponents<UFGPipeConnectionComponent>(PrimComponents);
		TInlineComponentArray<UFGFactoryConnectionComponent*> ConveyorCons(this);
		GetComponents<UFGFactoryConnectionComponent>(ConveyorCons);

		for(auto i : PrimComponents)
		{
			i->DestroyComponent();
			if (CachedPipe)
			{
				if (CachedPipe->GetConnectorClearance() != 0.93421f)
				{
					CachedPipe->SetConnectorClearance(0.93421f);
				}
				CachedPipe->SetWorldTransform(GetTransform());
				CachedPipe->AttachToComponent(AttachedBuilding->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

			}
			if (!MeshProxy)
			{
				TInlineComponentArray<UStaticMeshComponent*> Meshs(this);
				for (auto x : Meshs)
				{
					if (Cast<UFGColoredInstanceMeshProxy>(x))
						continue;
					MeshProxy = NewObject<UStaticMeshComponent>(AttachedBuilding, UStaticMeshComponent::StaticClass(), *FString("AttachedModularConnectionMesh_").Append(*GetName()));
					MeshProxy->SetStaticMesh(x->GetStaticMesh());
					MeshProxy->AttachToComponent(CachedBelt, FAttachmentTransformRules::KeepWorldTransform);
				}
			}
		}
		for(auto i : ConveyorCons)
		{
			i->DestroyComponent();
			if (CachedBelt)
			{
				if (CachedBelt->mConnectorClearance != 99.93421f)
				{
					CachedBelt->mConnectorClearance = 99.93421f;
				}
				CachedBelt->SetWorldTransform(GetTransform());
				CachedBelt->AttachToComponent(AttachedBuilding->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
				CachedBelt->mConnectorClearance = 99.93421f;
			}

			if (!MeshProxy)
			{
				TInlineComponentArray<UStaticMeshComponent*> Meshs(this);
				for (auto x : Meshs)
				{
					if (Cast<UFGColoredInstanceMeshProxy>(x))
						continue;
					MeshProxy = NewObject<UStaticMeshComponent>(AttachedBuilding, UStaticMeshComponent::StaticClass(), *FString("AttachedModularConnectionMesh_").Append(*GetName()));
					MeshProxy->SetStaticMesh(x->GetStaticMesh());
					MeshProxy->AttachToComponent(CachedPipe, FAttachmentTransformRules::KeepWorldTransform);
				}
			}
		}

		
	}
	if(AttachedBuilding)
	{
		TArray<UFGInventoryComponent * > Arr = UModularConnectionBPLib::GetInventoriesForBuilding(AttachedBuilding);
		TInlineComponentArray<UFGPipeConnectionComponent*> PrimComponents(this);
		GetComponents<UFGPipeConnectionComponent>(PrimComponents);
		TInlineComponentArray<UFGFactoryConnectionComponent*> ConveyorCons(this);
		GetComponents<UFGFactoryConnectionComponent>(ConveyorCons);
		if(CachedBelt && !ConveyorCons.Contains(CachedBelt))
		{
			ConveyorCons.Add(CachedBelt);
		}
		if(CachedPipe && !PrimComponents.Contains(CachedPipe))
		{
			PrimComponents.Add(CachedPipe);
		}
		
		if(Arr.IsValidIndex(InventoryIndex))
		{
			if(CachedPipe)
			{
				SetupPipe(CachedPipe, Arr[InventoryIndex], IndexOverride);
			}
			if(CachedBelt)
			{
				SetupConveyor(CachedBelt, Arr[InventoryIndex], IndexOverride);
			}
		}
		else // Index is not specified -> Default Logic for Inventories
		{
			if(CachedPipe)
			{
				const bool bIsOut = CachedPipe->GetPipeConnectionType() == EPipeConnectionType::PCT_PRODUCER;
				SetupPipe(CachedPipe, UModularConnectionBPLib::GetDefaultInventoryForDirection(AttachedBuilding,bIsOut), IndexOverride);
			}

			if(CachedBelt)
			{
				const bool bIsOut = CachedBelt->GetDirection() == EFactoryConnectionDirection::FCD_OUTPUT;
				SetupConveyor(CachedBelt, UModularConnectionBPLib::GetDefaultInventoryForDirection(AttachedBuilding,bIsOut), IndexOverride);
			}
		}
		
	}
	
}


void AModularConnectionBuildable::SetupPipe(UFGPipeConnectionComponent * Con,UFGInventoryComponent * Inventory, const int32 Index) const
{
	Con->SetInventory(Inventory);
	Con->mInventoryAccessIndex = IndexOverride;
	UE_LOG(LogTemp,Error,TEXT("SetInventory to : %s . Index %i"),*Con->GetName(), Con->mInventoryAccessIndex);
	UModularConnectionBPLib::InsertPipeConnectionOnBuilding(AttachedBuilding,Con);
}

void AModularConnectionBuildable::SetupConveyor(UFGFactoryConnectionComponent * Con,UFGInventoryComponent * Inventory, const int32 Index) const
{
	Con->SetInventory(Inventory);
	Con->mInventoryAccessIndex = IndexOverride;
	UE_LOG(LogTemp,Error,TEXT("SetInventory to : %s . Index %i"),*Con->GetName(), Con->mInventoryAccessIndex);
	UModularConnectionBPLib::InsertConnectionOnBuilding(AttachedBuilding,Con,InventoryIndex);
}
