// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/Action/GAJumpAction.h"

#include "SNDef.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameAnimationSample/Character/Player/Component/MMLocomotionComponent.h"
#include "GameAnimationSample/Character/Player/Component/ClimbActionComponent.h"

void UGAJumpAction::ExecAction(const FInputActionValue& InputActionValue)
{
	Super::ExecAction(InputActionValue);

	ACharacter* Character = GetOwner<ACharacter>();
	if (Character == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Character is nullptr."));
		return;
	}
	UMMLocomotionComponent* MMLocompotionComponent = Character->GetComponentByClass<UMMLocomotionComponent>();
	if (MMLocompotionComponent == nullptr)
	{
		SNPLUGIN_LOG(TEXT("MMLocomotionComponent is nullptr."));
		return;
	}

	UClimbActionComponent* ClimbActionComponent = Character->GetComponentByClass<UClimbActionComponent>();
	if (ClimbActionComponent != nullptr)
	{
		//! クライムアクションを止めた場合、処理終了
		if (ClimbActionComponent->CanselAction() == true)
		{
			return;
		}
	}
	
	if (MMLocompotionComponent->IsEnableTraversalAction() == true)
	{
		return;
	}

	if(Character->GetCharacterMovement()->IsMovingOnGround() == true)
	{

		float FowardTraceDistance = MMLocompotionComponent->GetTraversalForwardTraceDistance();

		bool TraversalCheckFailed = false;
		bool MontageSelectionFailed = false;
		
		MMLocompotionComponent->ExecTraversalAction(FowardTraceDistance, TraversalCheckFailed, MontageSelectionFailed);

		//! トラバーサルに失敗
		if(TraversalCheckFailed == true)
		{
			//! クライムアクションコンポーネントがある場合、クライムを試みる
			if (ClimbActionComponent != nullptr)
			{
				if (ClimbActionComponent->TryAction() == true)
				{
					return;
				}
			}
				
			Character->Jump();	
		}

		if(MontageSelectionFailed == true)
		{
			SNPLUGIN_LOG(TEXT("Traversal Montagen can't find."));
		}
	} 
	else
	{
		// @@Satoshi Nishimura テスト的にジャンプ・落下中にしがみつけるようにしてみる。
		// つながりやめり込みが気になるものの、SplineComponentが仕込まれているメッシュであれば掴める。
		float FowardTraceDistance = MMLocompotionComponent->GetTraversalForwardTraceDistance();

		bool TraversalCheckFailed = false;
		bool MontageSelectionFailed = false;
			
		MMLocompotionComponent->ExecTraversalAction(FowardTraceDistance * 1.5f, TraversalCheckFailed, MontageSelectionFailed);

	}
}
