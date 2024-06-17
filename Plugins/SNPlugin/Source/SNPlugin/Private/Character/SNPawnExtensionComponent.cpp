// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SNPawnExtensionComponent.h"

#include "GeometryTypes.h"
#include "SNDef.h"
#include "GameplayTags/SNGameplayTags.h"

#include "Components/GameFrameworkComponentDelegates.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

const FName USNPawnExtensionComponent::Name_ActorFeatureName = FName(TEXT("PawnExtension"));

USNPawnExtensionComponent::USNPawnExtensionComponent(const FObjectInitializer& Initializer)
:Super(Initializer)
{
	// Tickは走らないように
	PrimaryComponentTick.bStartWithTickEnabled = false;

	PrimaryComponentTick.bCanEverTick = false;
	// レプリケーションは行う
	SetIsReplicatedByDefault(true);
}

void USNPawnExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void USNPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();
	
#if WITH_EDITOR

	const APawn* Pawn = GetPawn<APawn>();

	ensureAlwaysMsgf(Pawn != nullptr, TEXT("PawnExtensionComponent on [%s] can only be added to Pawn Actors"), *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> PawnExtensionComponents;

	Pawn->GetComponents(USNPawnExtensionComponent::StaticClass(), PawnExtensionComponents);

	ensureAlwaysMsgf(PawnExtensionComponents.Num() == 1, TEXT("Only one PawnExtensionComponent should exist on [%s]"), *GetNameSafe(GetOwner()));
#endif
	
	RegisterInitStateFeature();
}

void USNPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	ensure(TryToChangeInitState(SNGameplayTags::InitState_Spawned));

	CheckDefaultInitialization();
}

void USNPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	
	Super::EndPlay(EndPlayReason);
}

void USNPawnExtensionComponent::HandleControllerChanged()
{
	CheckDefaultInitialization();
}

void USNPawnExtensionComponent::HandlePlayerStateReplicated()
{
	CheckDefaultInitialization();
}

void USNPawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}


bool USNPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	SNPLUGIN_ASSERT(Manager != nullptr, TEXT("Manager is nullptr."));

	APawn* Pawn(GetPawn<APawn>());

	if((CurrentState.IsValid() == false) && (DesiredState == SNGameplayTags::InitState_Spawned))
	{
		if(Pawn != nullptr)
		{
			return true;
		}
	}

	if((CurrentState == SNGameplayTags::InitState_Spawned) && (DesiredState == SNGameplayTags::InitState_DataAvailable))
	{
		const bool bHasAuthority = Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

		if((bHasAuthority == true) || (bIsLocallyControlled == true))
		{
			if(GetController<AController>() == nullptr)
			{
				return false;
			}
		}

		return true;
	} else if((CurrentState == SNGameplayTags::InitState_DataInitialized) && (DesiredState == SNGameplayTags::InitState_DataInitialized))
	{
		return Manager->HaveAllFeaturesReachedInitState(Pawn, SNGameplayTags::InitState_DataAvailable);
	} else if((CurrentState == SNGameplayTags::InitState_DataInitialized) && (DesiredState == SNGameplayTags::InitState_GameplayReady))
	{
		return true;
	}

	return false;
}

void USNPawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if(DesiredState == SNGameplayTags::InitState_DataInitialized)
	{
		
	}
}

void USNPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if(Params.FeatureName != Name_ActorFeatureName)
	{
		if(Params.FeatureState == SNGameplayTags::InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

void USNPawnExtensionComponent::CheckDefaultInitialization()
{
	CheckDefaultInitializationForImplementers();

	static const TArray<FGameplayTag> StateChain = {SNGameplayTags::InitState_Spawned, SNGameplayTags::InitState_DataAvailable, SNGameplayTags::InitState_DataInitialized, SNGameplayTags::InitState_GameplayReady};

	ContinueInitStateChain(StateChain);
}



