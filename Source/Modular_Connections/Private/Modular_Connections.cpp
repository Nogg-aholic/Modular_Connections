// Copyright Epic Games, Inc. All Rights Reserved.

#include "Modular_Connections.h"



#include "ModularConnectionBuildable.h"
#include "ModularConnectionsSubsystem.h"
#include "Buildables/FGBuildableResourceExtractor.h"
#include "Hologram/FGBuildableHologram.h"
#include "Hologram/FGPipelineHologram.h"
#include "Hologram/FGConveyorBeltHologram.h"
#include "Hologram/FGConveyorPoleHologram.h"
#include "Patching/NativeHookManager.h"

#define LOCTEXT_NAMESPACE "FModular_ConnectionsModule"

class UModularConnectionsSubsystem;

void FModular_ConnectionsModule::StartupModule()
{

    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module


#if WITH_EDITOR
#else

    AFGBuildableHologram* CDOHologramBuildable = GetMutableDefault<AFGBuildableHologram>();
    AFGHologram* CDOHologram = GetMutableDefault<AFGHologram>();
    AFGBuildableHologram* CDOBelt = GetMutableDefault<AFGConveyorBeltHologram>();
    AFGPipelineHologram* CDOPipe = GetMutableDefault<AFGPipelineHologram>();
    
    
    SUBSCRIBE_METHOD_VIRTUAL(AFGBuildableHologram::IsValidHitResult, CDOHologramBuildable, [](auto& Scope, const AFGBuildableHologram* Obj, const FHitResult& hitResult) {
        if (Obj->mBuildClass && Obj->mBuildClass->IsChildOf(AModularConnectionBuildable::StaticClass()))
        {

            if (!Obj->mValidHitClasses.Contains(AFGBuildable::StaticClass()))
            {
                auto* Objs = const_cast<AFGBuildableHologram*>(Obj);
                Objs->mValidHitClasses.Empty();
                TSubclassOf<class AActor> SubClass = AFGBuildable::StaticClass();
                Objs->mValidHitClasses.Add(SubClass);
            }
            
            if(Obj->mMaxPlacementFloorAngle != 90)
            {
                auto* Objs = const_cast<AFGBuildableHologram*>(Obj);
                Objs->mMaxPlacementFloorAngle = 90;
            }
            if (Scope(Obj, hitResult))
            {
                if (hitResult.Actor.Get())
                {
                    AActor* i = hitResult.Actor.Get();
                    if (Cast<APawn>(i))
                        Scope.Override(false);
                    auto* comp = i->GetComponentByClass(Obj->HasAuthority() ? UFGInventoryComponent::StaticClass() : UFGReplicationDetailInventoryComponent::StaticClass());
                    if (comp)
                    {
                        AFGBuildableHologram* Objs = const_cast<AFGBuildableHologram*>(Obj);
                        Obj->GetWorld()->GetSubsystem<UModularConnectionsSubsystem>()->LastHitResults.Add(Objs, hitResult);
                        Objs->SetHologramLocationAndRotation(hitResult);
                        Scope.Override(true);
                        return true;

                    }
                }
            }
            Scope.Override(false);
            return false;
        }
        return Scope(Obj, hitResult);
        });


    SUBSCRIBE_METHOD_VIRTUAL(AFGBuildableHologram::CheckValidFloor, CDOHologramBuildable, [](auto& Scope, AFGBuildableHologram* Obj) {
        if (Obj->mBuildClass && Obj->mBuildClass->IsChildOf(AModularConnectionBuildable::StaticClass()))
        {
            Scope.Cancel();
        }
    });

    SUBSCRIBE_METHOD_VIRTUAL(AFGBuildableHologram::SetHologramLocationAndRotation, CDOHologramBuildable, [](auto& Scope, AFGBuildableHologram* Obj, const FHitResult& hitResult) {
        if (Obj->mBuildClass && Obj->mBuildClass->IsChildOf(AModularConnectionBuildable::StaticClass()))
        {
            if (hitResult.bBlockingHit)
            {
                if (hitResult.Actor.Get())
                {
                    auto* SubSystem = Obj->GetWorld()->GetSubsystem<UModularConnectionsSubsystem>();
                    FVector FinalLocation = hitResult.ImpactPoint;
                    const FPlane Plane = FPlane(hitResult.Location, hitResult.ImpactNormal);
                    APlayerController* Cont = Cast < APlayerController>(Obj->GetConstructionInstigator()->GetController());
                    if (Cont->IsInputKeyDown(FKey(EKeys::LeftControl)))
                    {
                        FRotator RotX;
                        AActor* Ac = hitResult.Actor.Get();
                        AFGBuildable* Build = Cast<AFGBuildable>(Ac);
                        FinalLocation = hitResult.ImpactPoint;
                        FinalLocation.Z = ((static_cast<int32>(FinalLocation.Z/110)-1)*110)-25;
                        FinalLocation = FVector::PointPlaneProject(FinalLocation,Plane);
                        if (Build)
                        {
                            Obj->SnapToFloor(Build, FinalLocation, RotX);
                        }
                    }
                    FRotator Rot = hitResult.ImpactNormal.Rotation() + FRotator(10 * SubSystem->RotationalStep, 10 * SubSystem->RotationalStepYaw, 0);
                    Rot.Yaw = static_cast<int32>(Rot.Yaw);
                    Rot.Pitch = static_cast<int32>(Rot.Pitch);
                    const FVector Location = FinalLocation + (Rot.Vector() * (10 * SubSystem->OffsetStep));
                    Obj->SetActorLocation(Location);
                    Obj->SetActorRotation(Rot, ETeleportType::TeleportPhysics);
                    Scope.Cancel();
                }
            }
        }
    });

    SUBSCRIBE_METHOD_VIRTUAL(AFGBuildableHologram::ConfigureComponents, CDOHologramBuildable, [](auto& Scope, const AFGBuildableHologram* Obj, class AFGBuildable* inBuildable) {
        Scope(Obj, inBuildable);
        if (Cast<AModularConnectionBuildable>(inBuildable))
        {
            auto * subsystem = Obj->GetWorld()->GetSubsystem<UModularConnectionsSubsystem>();
            if (subsystem->LastHitResults.Contains(Obj))
            {
                Cast<AModularConnectionBuildable>(inBuildable)->AttachedBuilding = subsystem->LastHitResults.Find(Obj)->Actor.Get();
                subsystem->LastHitResults.Remove(Obj);
            }
        }
    });


    SUBSCRIBE_METHOD_VIRTUAL(AFGBuildableHologram::ScrollRotate, CDOHologramBuildable, [](auto& Scope, AFGBuildableHologram* Obj, int32 delta, int32 step) {
        if (Obj->mBuildClass && Obj->mBuildClass->IsChildOf(AModularConnectionBuildable::StaticClass()))
        {
            APlayerController* Cont = Cast < APlayerController>(Obj->GetConstructionInstigator()->GetController());

            if (Cont->IsInputKeyDown(FKey(EKeys::LeftControl)))
            {
                Obj->GetWorld()->GetSubsystem<UModularConnectionsSubsystem>()->OffsetStep += delta;
                Scope.Cancel();
            }
            else if (Cont->IsInputKeyDown(FKey(EKeys::LeftShift)))
            {
                Obj->GetWorld()->GetSubsystem<UModularConnectionsSubsystem>()->RotationalStep += delta;
                Scope.Cancel();
            }
            else
            {
                Obj->GetWorld()->GetSubsystem<UModularConnectionsSubsystem>()->RotationalStepYaw += delta;
                Scope.Cancel();
            }
        }
    });
  

    SUBSCRIBE_METHOD_VIRTUAL(AFGConveyorBeltHologram::CheckClearance, CDOBelt, [](auto& Scope, AFGConveyorBeltHologram* Obj) {
        
        bool Found = false;
        if(Obj && !Obj->IsPendingKill())
        {
            for (auto* i : Obj->mSnappedConnectionComponents)
            {
                if (!i || i->IsPendingKill())
                    continue;
                if (i->mConnectorClearance ==  99.93421f)
                {
                    Obj->mMaxPlacementFloorAngle = 90;
                    Found = true;
                }
            }
        }
        if (!Found)
            Scope(Obj);
        else
        {
            Scope.Cancel();
        }
    });



    SUBSCRIBE_METHOD_VIRTUAL(AFGPipelineHologram::CheckClearance, CDOPipe, [](auto& Scope, AFGPipelineHologram* Obj) {

        bool Found = false;
        for (auto* i : Obj->mSnappedConnectionComponents)
        {
            if (!i)
                continue;
            if (i->GetConnectorClearance() == 0.93421f)
            {
                Obj->mMaxPlacementFloorAngle = 90;
                Found = true;
            }
        }
        if (!Found)
            Scope(Obj);
        else
        {
            Scope.Cancel();
        }
    });
#endif
}


void FModular_ConnectionsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FModular_ConnectionsModule, Modular_Connections)