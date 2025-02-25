// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

#include "SGameModeBase.h"
#include "Net/UnrealNetwork.h"


static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), true, TEXT("Global Damage Modifier for Attribute Component."), ECVF_Cheat);


USAttributeComponent::USAttributeComponent()
{
	SetIsReplicatedByDefault(true);
}

bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetHealthMax());
}

USAttributeComponent* USAttributeComponent::GetAttributes(const AActor* FromActor)
{
	if (FromActor)
	{
		return Cast<USAttributeComponent>(FromActor->GetComponentByClass(StaticClass()));
	}

	return nullptr;
}

bool USAttributeComponent::IsActorAlive(const AActor* Actor)
{
	const USAttributeComponent* AttributeComp = GetAttributes(Actor);
	return AttributeComp != nullptr && AttributeComp->IsAlive();
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if (!GetOwner()->CanBeDamaged())
	{
		return false;
	}

	if (Delta < 0.f)
	{
		const float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();
		Delta *= DamageMultiplier;
	}

	const float OldHealth = Health;

	const float NewHealth = FMath::Clamp(Health + Delta, 0.f, HealthMax);
	const float ActualDelta = NewHealth - OldHealth;

	if (GetOwner()->HasAuthority())
	{
		Health = NewHealth;

		if (!FMath::IsNearlyZero(ActualDelta))
		{
			MulticastHealthChanged(InstigatorActor, Health, Delta);
		}

		if (ActualDelta < 0.f && FMath::IsNearlyZero(Health))
		{
			if (ASGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASGameModeBase>())
			{
				GameMode->OnActorKilled(GetOwner(), InstigatorActor);
			}
		}
	}

	return !FMath::IsNearlyZero(ActualDelta);
}

bool USAttributeComponent::ApplyRageChange(AActor* InstigatorActor, float Delta)
{
	const float OldRage = Rage;
	const float NewRage = FMath::Clamp(Rage + Delta, 0.f, RageMax);
	const float ActualDelta = NewRage - OldRage;

	if (GetOwner()->HasAuthority())
	{
		Rage = NewRage;

		if (!FMath::IsNearlyZero(ActualDelta))
		{
			MulticastRageChanged(InstigatorActor, Rage, Delta);
		}
	}

	return !FMath::IsNearlyZero(ActualDelta);
}

bool USAttributeComponent::IsMaxHealth() const
{
	return Health == HealthMax;
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.f;
}

bool USAttributeComponent::IsLowHealth() const
{
	return Health <= (HealthMax * 0.25f);
}

float USAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}

void USAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, const float NewHealth, const float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, this, NewHealth, Delta);
}

void USAttributeComponent::MulticastRageChanged_Implementation(AActor* InstigatorActor, const float NewRage, const float Delta)
{
	OnRageChanged.Broadcast(InstigatorActor, this, NewRage, Delta);
}

void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAttributeComponent, Health);
	DOREPLIFETIME(USAttributeComponent, HealthMax);
	DOREPLIFETIME(USAttributeComponent, Rage);
	DOREPLIFETIME(USAttributeComponent, RageMax);
}
