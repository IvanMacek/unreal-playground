// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SItemChest.generated.h"

UCLASS()
class UNREALPLAYGROUND_API ASItemChest : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()

public:	
	ASItemChest();

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	virtual void OnActorLoaded_Implementation() override;

	UPROPERTY(EditAnywhere)
	float TargetPitch;

protected:

	UPROPERTY(ReplicatedUsing="OnRep_LidOpened", SaveGame, BlueprintReadOnly)
	bool bLidOpened = false;

	UFUNCTION()
	void OnRep_LidOpened();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* LidMesh;
};
