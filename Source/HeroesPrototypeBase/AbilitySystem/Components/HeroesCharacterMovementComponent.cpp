// Copyright Samuel Reitich 2023.


#include "AbilitySystem/Components/HeroesCharacterMovementComponent.h"

#include "AbilitySystem/AttributeSets/MovementAttributeSet.h"
#include "AbilitySystem/HeroesNativeGameplayTags.h"
#include "GameFramework/Character.h"
#include "GameFramework/PhysicsVolume.h"
#include "GameplayEffectExtension.h"
#include "HeroesAbilitySystemComponent.h"
#include "HeroesGameFramework/HeroesGameData.h"
#include "HeroesLogChannels.h"
#include "HeroesGameFramework/HeroesAssetManager.h"
#include "Kismet/KismetMathLibrary.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Identifier_Action_Generic_Jump, "Ability.Identifier.Action.Generic.Jump", "The default jump ability.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_SetByCaller_Movement, "SetByCaller.Movement", "Data tags used to set the magnitude of movement-related modifiers and executions in gameplay effects.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_SetByCaller_Movement_Acceleration, "SetByCaller.Movement.Acceleration", "Data tag to set the magnitude of a Set by Caller acceleration modifier.");

UHeroesCharacterMovementComponent::UHeroesCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Jumping/falling variable values.
	bHardLandingEnabled = true;
	MinimumHardLandingSpeed = 500.0f;
	HardLandingSlowScale = 30.0f;
	JumpZVelocity = 650.0f;
	BrakingDecelerationFalling = 0.5f;
	AirControl = 1.0f;
	AirControlBoostVelocityThreshold = 0.0f;
	FallingLateralFriction = 0.5f;

	// General variable values.
	GravityScale = 2.0f;
	MaxAcceleration = 2560.0f;
	BrakingFrictionFactor = 2.0f;
	SetCrouchedHalfHeight(52.5f);

	// Walking variable values.
	MaxStepHeight = 25.0f;
	SetWalkableFloorAngle(46.0f);
	MaxWalkSpeed = 700.0f;
	CrouchWalkSpeedDecreaseNormalized = 0.6;
	SlowWalkSpeedDecreaseNormalized = 0.4;
	BrakingDecelerationWalking = 2048.0f;
	bMaintainHorizontalGroundVelocity = false;
	PerchRadiusThreshold = 15.0f;
	PerchAdditionalHeight = 25.0f;
	bUseFlatBaseForFloorChecks = true;

	// Save the original values of movement variables that will be directly modified by attributes with multipliers.
	BaseAirControl = AirControl;
	BaseBrakingFrictionFactor = BrakingFrictionFactor;
	BaseBrakingDecelerationWalking = BrakingDecelerationWalking;
}

void UHeroesCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Bind the OnLanded callback to when this component's owning character lands.
	if (ACharacter* OwningCharacter = GetCharacterOwner())
	{
		OwningCharacter->LandedDelegate.AddDynamic(this, &UHeroesCharacterMovementComponent::OnLanded);
	}
}

bool UHeroesCharacterMovementComponent::CanEditChange(const FProperty* InProperty) const
{
	const FName PropertyName = InProperty->GetFName();

	/* Hide variables related to crouching speed. The crouching speed is calculated using attributes, not the movement
	 * component. */
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UHeroesCharacterMovementComponent, MaxWalkSpeedCrouched))
	{
		return false;
	}

	return Super::CanEditChange(InProperty);
}

void UHeroesCharacterMovementComponent::InitializeWithAbilitySystem(UHeroesAbilitySystemComponent* InASC)
{
	const AActor* Owner = GetOwner();
	check(Owner);

	// Do not initialize this component if we've already initialized it with an ASC.
	if (HeroesASC)
	{
		UE_LOG(LogHeroes, Warning, TEXT("UHeroesCharacterMovementComponent: Tried to initialize movement component for owner [%s], but component has already been initialized with an ASC."), *GetNameSafe(Owner));
		return;
	}

	// Cache the given ASC and ensure it isn't null.
	HeroesASC = InASC;
	if (!HeroesASC)
	{
		UE_LOG(LogHeroes, Error, TEXT("UHeroesCharacterMovementComponent: Failed to initialize movement component for owner [%s]. Given ASC is null."), *GetNameSafe(Owner));
		return;
	}

	// Cache the given ASC's combat attribute set and ensure it isn't null.
	MovementAttributeSet = HeroesASC->GetSet<UMovementAttributeSet>();
	if (!MovementAttributeSet)
	{
		UE_LOG(LogHeroes, Error, TEXT("UHeroesCharacterMovementComponent: Initialized movement component for owner [%s], but given ASC's movement attribute set is null."), *GetNameSafe(Owner));
		return;
	}


	// Bind delegates to the new movement attribute set's attribute changes.
	HeroesASC->GetGameplayAttributeValueChangeDelegate(UMovementAttributeSet::GetMovementSpeedAttribute()).AddUObject(this, &UHeroesCharacterMovementComponent::OnMovementSpeedChangedChanged);
	HeroesASC->GetGameplayAttributeValueChangeDelegate(UMovementAttributeSet::GetMovementAccelerationAttribute()).AddUObject(this, &UHeroesCharacterMovementComponent::OnMovementAccelerationChanged);
	HeroesASC->GetGameplayAttributeValueChangeDelegate(UMovementAttributeSet::GetDirectionalControlAttribute()).AddUObject(this, &UHeroesCharacterMovementComponent::OnDirectionalControlChanged);
	HeroesASC->GetGameplayAttributeValueChangeDelegate(UMovementAttributeSet::GetGravityScaleAttribute()).AddUObject(this, &UHeroesCharacterMovementComponent::OnGravityScaleChanged);
	HeroesASC->GetGameplayAttributeValueChangeDelegate(UMovementAttributeSet::GetJumpStrengthAttribute()).AddUObject(this, &UHeroesCharacterMovementComponent::OnJumpStrengthChanged);

	// Reset attributes to their default values, set in the character movement component.
	HeroesASC->SetNumericAttributeBase(UMovementAttributeSet::GetMovementSpeedAttribute(), MaxWalkSpeed);
	HeroesASC->SetNumericAttributeBase(UMovementAttributeSet::GetMovementAccelerationAttribute(), MaxAcceleration);
	HeroesASC->SetNumericAttributeBase(UMovementAttributeSet::GetDirectionalControlAttribute(), 1.0f);
	HeroesASC->SetNumericAttributeBase(UMovementAttributeSet::GetGravityScaleAttribute(), GravityScale);
	HeroesASC->SetNumericAttributeBase(UMovementAttributeSet::GetJumpStrengthAttribute(), JumpZVelocity);

	// Broadcast initial movement value changes.
	MovementSpeedChangedDelegate.Broadcast(this, MovementAttributeSet->GetMovementSpeed(), MovementAttributeSet->GetMovementSpeed(), nullptr);
	MovementAccelerationChangedDelegate.Broadcast(this, MovementAttributeSet->GetMovementAcceleration(), MovementAttributeSet->GetMovementAcceleration(), nullptr);
	DirectionalControlChangedDelegate.Broadcast(this, MovementAttributeSet->GetDirectionalControl(), MovementAttributeSet->GetDirectionalControl(), nullptr);
	GravityScaleChangedDelegate.Broadcast(this, MovementAttributeSet->GetGravityScale(), MovementAttributeSet->GetGravityScale(), nullptr);
	JumpStrengthChangedDelegate.Broadcast(this, MovementAttributeSet->GetJumpStrength(), MovementAttributeSet->GetJumpStrength(), nullptr);

}

void UHeroesCharacterMovementComponent::UninitializeFromAbilitySystem()
{
	// Reset our cached variables.
	MovementAttributeSet = nullptr;
	HeroesASC = nullptr;
}

void UHeroesCharacterMovementComponent::OnUnregister()
{
	// Uninitialize this component from its owner's ASC before unregistering it.
	UninitializeFromAbilitySystem();

	Super::OnUnregister();
}

AActor* UHeroesCharacterMovementComponent::GetInstigatorFromAttributeChangeData(const FOnAttributeChangeData& ChangeData)
{
	// Try to get the attribute change's original instigator from its gameplay effect spec.
	if (ChangeData.GEModData != nullptr)
	{
		const FGameplayEffectContextHandle& EffectContext = ChangeData.GEModData->EffectSpec.GetEffectContext();
		return EffectContext.GetOriginalInstigator();
	}

	return nullptr;
}

float UHeroesCharacterMovementComponent::GetMovementSpeed() const
{
	// Try to retrieve the current value of the MovementSpeed attribute from the attribute set.
	if (MovementAttributeSet)
	{
		return MovementAttributeSet->GetMovementSpeed();
	}

	UE_LOG(LogHeroes, Warning, TEXT("UHeroesCharacterMovementComponent: Attempted to retrieve MovementSpeed for owner [%s], but a movement attribute set was not found."), *GetNameSafe(GetOwner()));
	return 0.0f;
}

float UHeroesCharacterMovementComponent::GetMovementAcceleration() const
{
	// Try to retrieve the current value of the MovementAcceleration attribute from the attribute set.
	if (MovementAttributeSet)
	{
		return MovementAttributeSet->GetMovementAcceleration();
	}

	UE_LOG(LogHeroes, Warning, TEXT("UHeroesCharacterMovementComponent: Attempted to retrieve MovementAcceleration for owner [%s], but a movement attribute set was not found."), *GetNameSafe(GetOwner()));
	return 0.0f;
}

float UHeroesCharacterMovementComponent::GetDirectionalControl() const
{
	// Try to retrieve the current value of the DirectionalControl attribute from the attribute set.
	if (MovementAttributeSet)
	{
		return MovementAttributeSet->GetDirectionalControl();
	}

	UE_LOG(LogHeroes, Warning, TEXT("UHeroesCharacterMovementComponent: Attempted to retrieve DirectionalControl for owner [%s], but a movement attribute set was not found."), *GetNameSafe(GetOwner()));
	return 0.0f;
}

float UHeroesCharacterMovementComponent::GetGravityScale() const
{
	// Try to retrieve the current value of the GravityScale attribute from the attribute set.
	if (MovementAttributeSet)
	{
		return MovementAttributeSet->GetGravityScale();
	}

	UE_LOG(LogHeroes, Warning, TEXT("UHeroesCharacterMovementComponent: Attempted to retrieve GravityScale for owner [%s], but a movement attribute set was not found."), *GetNameSafe(GetOwner()));
	return 0.0f;
}

float UHeroesCharacterMovementComponent::GetJumpStrength() const
{
	// Try to retrieve the current value of the JumpStrength attribute from the attribute set.
	if (MovementAttributeSet)
	{
		return MovementAttributeSet->GetJumpStrength();
	}

	UE_LOG(LogHeroes, Warning, TEXT("UHeroesCharacterMovementComponent: Attempted to retrieve JumpStrength for owner [%s], but a movement attribute set was not found."), *GetNameSafe(GetOwner()));
	return 0.0f;
}

void UHeroesCharacterMovementComponent::OnMovementSpeedChangedChanged(const FOnAttributeChangeData& ChangeData)
{
	// Update the maximum walking speed and the maximum crouch-walking speed using the new movement speed attribute value.
	MaxWalkSpeed = GetMovementSpeed();
	MaxWalkSpeedCrouched = GetMovementSpeed() * CrouchWalkSpeedDecreaseNormalized;
	
	// Fire the delegate for when the MovementSpeed attribute is changed.
	MovementSpeedChangedDelegate.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttributeChangeData(ChangeData));
}

void UHeroesCharacterMovementComponent::OnMovementAccelerationChanged(const FOnAttributeChangeData& ChangeData)
{
	// Update the maximum acceleration using the new movement acceleration attribute value.
	MaxAcceleration = GetMovementAcceleration();

	// Fire the delegate for when the MovementAcceleration attribute is changed.
	MovementAccelerationChangedDelegate.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttributeChangeData(ChangeData));
}

void UHeroesCharacterMovementComponent::OnDirectionalControlChanged(const FOnAttributeChangeData& ChangeData)
{
	// Update directional control-related values using their cached base values and the new directional control attribute's value.
	const float NewDirectionalControl = GetDirectionalControl();
	AirControl = BaseAirControl * NewDirectionalControl;
	BrakingFrictionFactor = BaseBrakingFrictionFactor * NewDirectionalControl;
	BrakingDecelerationWalking = BaseBrakingDecelerationWalking * NewDirectionalControl;

	// Fire the delegate for when the DirectionalControl attribute is changed.
	DirectionalControlChangedDelegate.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttributeChangeData(ChangeData));
}

void UHeroesCharacterMovementComponent::OnGravityScaleChanged(const FOnAttributeChangeData& ChangeData)
{
	// Update the gravity scale using the new gravity scale attribute value.
	GravityScale = GetGravityScale();

	// Fire the delegate for when the GravityScale attribute is changed.
	GravityScaleChangedDelegate.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttributeChangeData(ChangeData));
}

void UHeroesCharacterMovementComponent::OnJumpStrengthChanged(const FOnAttributeChangeData& ChangeData)
{
	// Update the vertical jump velocity using the new jump strength attribute value.
	JumpZVelocity = GetJumpStrength();

	// Fire the delegate for when the JumpStrength attribute is changed.
	JumpStrengthChangedDelegate.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttributeChangeData(ChangeData));
}

float UHeroesCharacterMovementComponent::CalculateLandingHardness(float LandingSpeed, const UCurveFloat* HardLandingEffectCurve) const
{
	const ACharacter* OwningCharacter = GetCharacterOwner();
	
	if (!IsValid(OwningCharacter))
	{
		return 0.0f;
	}

	// The maximum speed at which this character can make a landing.
	const float MaximumFallingSpeed = OwningCharacter->GetCharacterMovement()->GetPhysicsVolume()->TerminalVelocity;
	
	// Don't make a hard landing if hard landings are disabled or if the character's falling speed did not meet the hard landing threshold.
	if (!bHardLandingEnabled || FMath::Abs(LandingSpeed) < MinimumHardLandingSpeed)
	{
		return 0.0f;
	}
	
	// Calculate how hard the character hit the ground, linearly normalized between the minimum "hard landing" speed and the character's terminal velocity.
	const float LinearLandingHardness = UKismetMathLibrary::NormalizeToRange(FMath::Min(FMath::Abs(LandingSpeed), MaximumFallingSpeed), MinimumHardLandingSpeed, MaximumFallingSpeed);
	
	// Return the landing hardness
	if (!HardLandingEffectCurve)
	{
		return LinearLandingHardness;
	}
	
	// Get the linearly normalized landing hardness's corresponding value on the custom landing effect curve.
	return HardLandingEffectCurve->GetFloatValue(LinearLandingHardness);
}

void UHeroesCharacterMovementComponent::OnLanded(const FHitResult& Hit)
{
	// Cancel any "jump" ability when we land.
	const FGameplayTagContainer JumpTag = FGameplayTagContainer(TAG_Ability_Identifier_Action_Generic_Jump);
	HeroesASC->CancelAbilities(&JumpTag);

	// Don't do anything when landing if hard landing is disabled.
	if (!bHardLandingEnabled)
	{
		return;
	}

	// Get the velocity at which the character landed.
	const FVector LandingVelocity = GetLastUpdateVelocity();

	// Don't do anything if the character's landing speed did not meet the hard landing threshold.
	if (FMath::Abs(LandingVelocity.Z) < MinimumHardLandingSpeed)
	{
		return;
	}

	// TODO: Use the currently equipped item's curve here.
	const float LandingHardness = CalculateLandingHardness(LandingVelocity.Z);


	// Scale back the player's velocity depending on how hard they fell.
	Velocity = LandingVelocity / (LandingHardness * HardLandingSlowScale);


	// Apply a temporary gameplay effect that slows the player's maximum acceleration depending on how hard they fell.
	const TSubclassOf<UGameplayEffect> AccelerationGE = UHeroesAssetManager::GetSubclass(UHeroesGameData::Get().AccelerationGameplayEffect_Duration);
	if (!AccelerationGE)
	{
		UE_LOG(LogHeroes, Error, TEXT("UHeroesCharacterMovementComponent: OnLanded failed for owner [%s]. Unable to find gameplay effect [%s]."), *GetNameSafe(GetOwner()), *UHeroesGameData::Get().AccelerationGameplayEffect_Duration.GetAssetName());
		return;
	}

	const FGameplayEffectSpecHandle SpecHandle = HeroesASC->MakeOutgoingSpec(AccelerationGE, 1.0f, HeroesASC->MakeEffectContext());
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (!Spec)
	{
		UE_LOG(LogHeroes, Error, TEXT("UHeroesCharacterMovementComponent: OnLanded failed for owner [%s]. Unable to make outgoing spec for [%s]."), *GetNameSafe(GetOwner()), *GetNameSafe(AccelerationGE));
		return;
	}

	// Set the magnitude of the effect's duration.
	constexpr float AccelerationDuration =  0.5f;
	Spec->SetSetByCallerMagnitude(FHeroesNativeGameplayTags::Get().SetByCaller_Duration, AccelerationDuration);

	// Set the magnitude of the effect's acceleration multiplier.
	constexpr float MinAccelerationMultiplier = 0.5f;
	const float AccelerationMultiplier = FMath::FInterpConstantTo(1.0f, MinAccelerationMultiplier, LandingHardness, 5.0f);
	Spec->SetSetByCallerMagnitude(TAG_SetByCaller_Movement_Acceleration, AccelerationMultiplier);

	// Apply the gameplay effect to the character.
	HeroesASC->ApplyGameplayEffectSpecToSelf(*Spec);
}
