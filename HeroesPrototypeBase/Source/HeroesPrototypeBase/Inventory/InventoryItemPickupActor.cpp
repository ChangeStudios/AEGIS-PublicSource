// Copyright Samuel Reitich 2023.

#include "Inventory/InventoryItemPickupActor.h"

#include "Components/CapsuleComponent.h"
#include "Engine/ActorChannel.h"
#include "HeroesLogChannels.h"
#include "InventoryComponent.h"
#include "InventoryItemDefinition.h"
#include "InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "Player/PlayerStates/Game/HeroesGamePlayerStateBase.h"

AInventoryItemPickupActor::AInventoryItemPickupActor()
{
	// Networking.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	AActor::SetReplicateMovement(true);

	// Create the root skeletal mesh component and set up its physics simulation.
	RootMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Root Mesh"));
	SetRootComponent(RootMesh);
	RootMesh->SetSimulatePhysics(true);
	RootMesh->CanCharacterStepUpOn = ECB_No;
	RootMesh->SetCollisionProfileName(TEXT("Custom"));
	RootMesh->UpdateCollisionProfile();
	RootMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	RootMesh->SetCollisionObjectType(ECC_PhysicsBody);
	RootMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	RootMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	RootMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	// Create the collision component for detecting overlaps.
	OverlapCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Overlap Collision"));
	OverlapCollision->SetupAttachment(RootMesh);
	OverlapCollision->ShapeColor = FColor(0, 200, 0);
	OverlapCollision->InitCapsuleSize(40.0f, 40.0f);
	OverlapCollision->OnComponentBeginOverlap.AddDynamic(this, &AInventoryItemPickupActor::OnPickUpOverlap);

	// Create the collision component for detecting players looking at this actor.
	LookAtCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Look-At Collision"));
	LookAtCollision->SetupAttachment(RootMesh);
	LookAtCollision->ShapeColor = FColor(200, 100, 100);
	LookAtCollision->InitCapsuleSize(50.0f, 50.0f);
}

void AInventoryItemPickupActor::Init(UInventoryItemInstance* InItemInstance)
{
	if (IsValid(InItemInstance))
	{
		RepresentedItemInstance = InItemInstance;
	}
}

void AInventoryItemPickupActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		/* If this actor has not been initialized with an item instance, create a new item instance to represent using
		 * this actor's item class. */
		if (!IsValid(RepresentedItemInstance))
		{
			check(RepresentedItemDefinitionClass);

			RepresentedItemInstance = NewObject<UInventoryItemInstance>(this);
			RepresentedItemInstance->Init(RepresentedItemDefinitionClass);
		}

		// Do not utilize a pick-up cooldown if this item actor was not spawned with an instigator.
		if (!GetInstigator())
		{
			bInstigatorCooldownExpired = true;
		}
		// Wait 0.25 seconds (arbitrary) before allowing the player who threw this item to pick it up again.
		else
		{
			FTimerHandle MemberTimerHandle;
			GetWorldTimerManager().SetTimer(MemberTimerHandle, FTimerDelegate::CreateLambda([&]
			{
				bInstigatorCooldownExpired = true;
				GetWorldTimerManager().ClearTimer(MemberTimerHandle);
			}), 0.25f, false);
		}
	}
}

bool AInventoryItemPickupActor::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	bWroteSomething |= Channel->ReplicateSubobject(RepresentedItemInstance, *Bunch, *RepFlags);

	return bWroteSomething;
}

void AInventoryItemPickupActor::OnPickUpOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Do not let the player who dropped this item pick it up again until a short cooldown has expired.
	if (OtherActor == GetInstigator() && !bInstigatorCooldownExpired)
	{
		return;
	}

	if (const APawn* Pawn = Cast<APawn>(OtherActor))
	{
		if (const AHeroesGamePlayerStateBase* HeroesPS = Cast<AHeroesGamePlayerStateBase>(Pawn->GetPlayerState()))
		{
			// Try to automatically pick up this item when a player overlaps it.
			const EInventoryActionResult PickUpResult = HeroesPS->GetInventoryComponent()->AddItemSoft(RepresentedItemInstance);

			// If the player successfully picked up this item, destroy the item actor.
			if (PickUpResult == EInventoryActionResult::Success)
			{
				Destroy();
			}
		}
	}
}

void AInventoryItemPickupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInventoryItemPickupActor, RepresentedItemInstance);
}