// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryItemPickupActor.generated.h"

class UInventoryItemDefinition;
class UInventoryItemInstance;

class UBoxComponent;
class UCapsuleComponent;
class USkeletalMeshComponent;

UCLASS()
class HEROESPROTOTYPEBASE_API AInventoryItemPickupActor : public AActor
{
	GENERATED_BODY()

	// Construction.

public:

	/** Default constructor. */
	AInventoryItemPickupActor();



	// Initialization.

public:

	/** Creates a new item instance for this item to represent if this item actor was not initialized with one. */
	virtual void BeginPlay() override;

	/** Initializes this item actor's data. If this actor is spawned during runtime, this must be called before the
	 * actor finishes spawning. This does not have to be called if this actor is created in the editor. */
	virtual void Init(UInventoryItemInstance* InItemInstance);

	/** When an item pick-up actor is spawned, there is a short cooldown before it can be picked up again by the player
	 * who threw it. The player will be ignored until this cooldown expires. **/
	bool bInstigatorCooldownExpired = false;



	// Item data.

public:

	/** Enable sub-object replication to replicate this actor's item data. */
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

protected:

	/** The item instance that this actor represents. If this actor is initialized without an item instance, one will
	 * be created for it. */
	UPROPERTY(Replicated, BlueprintReadOnly, meta = (HideInDetailPanel))
	TObjectPtr<UInventoryItemInstance> RepresentedItemInstance = nullptr;

	/** The definition class of the item that this actor represents. This is used to create an item instance for this
	 * actor when spawned, if necessary. This only needs to be set if this actor was created in the editor instead of
	 * being spawned. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Info")
	const TSubclassOf<UInventoryItemDefinition> RepresentedItemDefinitionClass;



	// Overlap detection.

protected:

	/** Called when another actor overlaps this actor's overlap collision component. This is used to make actors
	 * automatically pick up this item when walking over it. */
	UFUNCTION()
	void OnPickUpOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Components.

// Accessors.
public:

	/** Getter for the root mesh component. */
	USkeletalMeshComponent* GetRootMesh() const { return RootMesh; }

	/** Getter for the overlap-detection collision component. */
	UCapsuleComponent* GetOverlapCollision() const { return OverlapCollision; }

	/** Getter for the look-at-detection collision component. */
	UCapsuleComponent* GetLookAtCollision() const { return LookAtCollision; }

// Components.
protected:

	/** The mesh that visually represents this item. This mesh has to be the root component for the actor to simulate
	 * physics using the mesh's physics body. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMeshComponent> RootMesh;

	/** The collision component used to detect when another actor overlaps with this actor. This is used to detect
	 * actors walking over this item to automatically pick it up. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCapsuleComponent> OverlapCollision;

	/** The collision component used to detect when a player looks at this actor. This is used to detect players
	 * looking at this item to manually pick it up. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCapsuleComponent> LookAtCollision;

};
