// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_GatherTargetInfo.h"
#include "AI/EnemyAIController.h"
#include "Animation/WidgetAnimation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/EnemyCharacter.h"
#include "Characters/MainCharacter.h"
#include "Components/ActionComponent.h"
#include "Components/AttributesComponent.h"
#include "HUD/CombatOverlay.h"
#include "HUD/MainHUD.h"


UBTService_GatherTargetInfo::UBTService_GatherTargetInfo()
{
	MaxAttackRange = 200.f;
	AttackRangeKey.SelectedKeyName = "WithinAttackRange";
}

void UBTService_GatherTargetInfo::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Check distance between ai pawn and target actor
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackBoardComp))
	{
		if(DoOnce)
		{
			BlackBoardComp->SetValueAsBool("PerformStandupOnce",false);
			DoOnce=false;
		}

		AActor* TargetActor = Cast<AActor>(BlackBoardComp->GetValueAsObject("TargetActor"));
		if (TargetActor)
		{
			BlackBoardComp->SetValueAsBool("AIActive", true);
			AMainCharacter* MC = Cast<AMainCharacter>(TargetActor);
			if(DoOnce2)
			{
				MC->GetCombatOverlay()->AddToViewport();
				MC->GetCombatOverlay()->PlayAnimation(MC->GetCombatOverlay()->FadeOut,0,1,EUMGSequencePlayMode::Reverse);
				MC->GetCombatOverlay()->SetHealProgressBar(MC->GetAttributesComp()->GetHealChargeProgress());
				MC->GetCombatOverlay()->SetHealthChargeAmount(MC->GetAttributesComp()->GetHeals());
				DoOnce2=false;
			}
			AAIController* MyController = OwnerComp.GetAIOwner();
			APawn* AIPawn = MyController->GetPawn();

			if (ensure(AIPawn))
			{
				AEnemyCharacter* EC = Cast<AEnemyCharacter>(AIPawn);
				//If AI just got activated then do the stand up action
				if (!EC->GetAIActive())
				{
					EC->SetAiActive(true);
					//EC->GetActionComp()->StartActionByName(EC, "StandUpAction");
				}

				//GETTING SINGED ANGLE BETWEEN A FORWARD VECTOR AND A LOCATION IN SPACE
				//Drop the Z axis cause we dont care about it in the calculation. we want everything to be in the XY axis
				FVector DropZ = FVector(1, 1, 0);
				FVector ForwardVector = (AIPawn->GetActorForwardVector()).GetSafeNormal();
				FVector TargetLocation = TargetActor->GetActorLocation() * DropZ;
				FVector ActorLocation = AIPawn->GetActorLocation() * DropZ;
				//The vector going from me to my target
				FVector DirectionVec = (TargetLocation - ActorLocation).GetSafeNormal();
				//Angle between Forward vector and vector going to my target in radians and unsigned.
				float Angle = FMath::Acos(FVector::DotProduct(ForwardVector, DirectionVec));
				float AngleInDegree = FMath::RadiansToDegrees(Angle);
				//Cross product helps us distinguish between negative and positive angle. 
				FVector CrossVector = FVector::CrossProduct(ForwardVector, DirectionVec);
				if (CrossVector.Z < 0) AngleInDegree = AngleInDegree * -1;
				float DistanceTo = FVector::Distance(TargetLocation, ActorLocation);
				bool bWithinRange = DistanceTo < MaxAttackRange;
				if (MC)
				{
					bool TargetIsHealing = MC->GetActionComp()->ActiveGamePlayTags.HasTag(FGameplayTag::RequestGameplayTag("Healing"));
					BlackBoardComp->SetValueAsBool("TargetIsHealing", TargetIsHealing);
					float TargetStamina = MC->GetAttributesComp()->Stamina;
					float Health=MC->GetAttributesComp()->GetHealth();
					if(Health<=0)
					{
						BlackBoardComp->SetValueAsBool("AIActive", false);
					}
					BlackBoardComp->SetValueAsFloat("TargetStamina", TargetStamina);
				}

				BlackBoardComp->SetValueAsBool("WithinAttackRange", bWithinRange);
				BlackBoardComp->SetValueAsFloat("DistanceToTarget", DistanceTo);
				BlackBoardComp->SetValueAsFloat("TargetSpeed", TargetActor->GetVelocity().Size());
				BlackBoardComp->SetValueAsFloat("AngleToTarget", AngleInDegree);
				BlackBoardComp->SetValueAsVector("TargetVector", TargetLocation);
			}
		}
	}
}
