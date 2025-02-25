// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackboardComp))
	{
		if (const AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor")))
		{
			const AAIController* MyController = OwnerComp.GetAIOwner();
			if (ensure(MyController))
			{
				const APawn* AIPawn = MyController->GetPawn();
				if (ensure(AIPawn))
				{
					const float DistanceTo = FVector::Distance(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());

					const bool bWithinRange = DistanceTo < 1500.f;

					bool bHasLOS = false;
					if (bWithinRange)
					{
						bHasLOS	= MyController->LineOfSightTo(TargetActor);
					}

					BlackboardComp->SetValueAsBool(AttackRangeKey.SelectedKeyName, bWithinRange && bHasLOS);

				}
			}
		}
	}
}
