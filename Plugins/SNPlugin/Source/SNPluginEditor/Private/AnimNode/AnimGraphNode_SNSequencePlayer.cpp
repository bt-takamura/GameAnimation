// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNode/AnimGraphNode_SNSequencePlayer.h"

#include "Modules/ModuleManager.h"
#include "ToolMenus.h"

#include "Kismet2/CompilerResultsLog.h"
#include "AnimGraphCommands.h"
#include "AssetRegistry/ARFilter.h"
#include "BlueprintActionFilter.h"
#include "DetailLayoutBuilder.h"
#include "Animation/AnimComposite.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimPoseSearchProvider.h"
#include "Animation/AnimRootMotionProvider.h"
#include "UObject/UE5MainStreamObjectVersion.h"
#include "IAnimBlueprintNodeOverrideAssetsContext.h"
#include "SNDef.h"
#include "Animation/SNAnimInstanceBase.h"

#define LOCTEXT_NAMESPACE "UAnimGraphNode_SNSequencePlayer"

/////////////////////////////////////////////////////
// UAnimGraphNode_SNSequencePlayer

//----------------------------------------------------------------------//
//
//! @brief デフォルトコンストラクタ
//
//! @param ObjectInitializer イニシャライザ
//
//----------------------------------------------------------------------//
UAnimGraphNode_SNSequencePlayer::UAnimGraphNode_SNSequencePlayer(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	
}

//----------------------------------------------------------------------//
//
//! @brief シリアライザ
//
//! @param Ar 
//
//----------------------------------------------------------------------//
void UAnimGraphNode_SNSequencePlayer::Serialize(FArchive& Ar){
	
	Super::Serialize(Ar);
	
	Ar.UsingCustomVersion(FUE5MainStreamObjectVersion::GUID);
	
	if(Ar.IsLoading() && Ar.CustomVer(FUE5MainStreamObjectVersion::GUID) < FUE5MainStreamObjectVersion::AnimNodeConstantDataRefactorPhase0){
		Node.PlayRateScaleBiasClampConstants.CopyFromLegacy(Node.PlayRateScaleBiasClamp_DEPRECATED);
	}
}

void UAnimGraphNode_SNSequencePlayer::PreloadRequiredAssets(){
	
	PreloadRequiredAssetsHelper(Node.GetSequence(), FindPin(GET_MEMBER_NAME_STRING_CHECKED(FAnimNode_SNSequencePlayer, Sequence)));
	
	Super::PreloadRequiredAssets();
}

FLinearColor UAnimGraphNode_SNSequencePlayer::GetNodeTitleColor() const {
	
	UAnimSequenceBase* Sequence = Node.GetSequence();
	
	if((Sequence != nullptr) && Sequence->IsValidAdditive()){
		return FLinearColor(0.10f, 0.60f, 0.12f);
	} else {
		return FColor(200, 100, 100);
	}
}

FSlateIcon UAnimGraphNode_SNSequencePlayer::GetIconAndTint(FLinearColor& OutColor) const
{
	return FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.AnimSequence");
}

FText UAnimGraphNode_SNSequencePlayer::GetNodeTitle(ENodeTitleType::Type TitleType) const {
	
	UEdGraphPin* SequencePin = FindPin(GET_MEMBER_NAME_STRING_CHECKED(FAnimNode_SNSequencePlayer, Sequence));
	// Nodeを表示する時の設定(Node名に「SN Sequence Player」が表示されます。)
	return GetNodeTitleHelper(TitleType, SequencePin, LOCTEXT("PlayerDesc", "SN Sequence Player"), [](UAnimationAsset* InAsset){
			// シーケンスアセットデータを取得
			UAnimSequenceBase* SequenceBase = CastChecked<UAnimSequenceBase>(InAsset);
			
			const bool bAdditive = SequenceBase->IsValidAdditive();
			
			return bAdditive ? LOCTEXT("AdditivePostFix", "(additive)") : FText::GetEmpty();
		}
	);
}

//----------------------------------------------------------------------//
//
//! @brief 右クリックで開くメニューストリップに表示するカテゴリを取得
//
//! @retval 右クリックで開くメニューストリップに表示する
//
//----------------------------------------------------------------------//
FText UAnimGraphNode_SNSequencePlayer::GetMenuCategory() const {
	return LOCTEXT("MenuCategory", "SN Animation|Sequences|Custom Node");
}

void UAnimGraphNode_SNSequencePlayer::GetMenuActions(FBlueprintActionDatabaseRegistrar& InActionRegistrar) const
{
	GetMenuActionsHelper(
		InActionRegistrar,
		GetClass(),
		{ UAnimSequence::StaticClass(), UAnimComposite::StaticClass() },
		{ },
		[](const FAssetData& InAssetData, UClass* InClass){
			
			if(InAssetData.IsValid()){
				
				const FString TagValue = InAssetData.GetTagValueRef<FString>(GET_MEMBER_NAME_CHECKED(UAnimSequence, AdditiveAnimType));
				
				if(const bool bKnownToBeAdditive = (!TagValue.IsEmpty() && !TagValue.Equals(TEXT("AAT_None")))){
					return FText::Format(LOCTEXT("MenuDescFormat_PlayAdditive", "Play '{0}' (additive)"), FText::FromName(InAssetData.AssetName));
				} else {
					return FText::Format(LOCTEXT("MenuDescFormat_Play1", "Play '{0}'"), FText::FromName(InAssetData.AssetName));
				}
			} else {
				return LOCTEXT("PlayerDesc", "SN Sequence Player");
			}
		},
		[](const FAssetData& InAssetData, UClass* InClass)
		{
			if(InAssetData.IsValid())
			{
				const FString TagValue = InAssetData.GetTagValueRef<FString>(GET_MEMBER_NAME_CHECKED(UAnimSequence, AdditiveAnimType));
				if(const bool bKnownToBeAdditive = (!TagValue.IsEmpty() && !TagValue.Equals(TEXT("AAT_None"))))
				{
					return FText::Format(LOCTEXT("MenuDescTooltipFormat_PlayAdded", "Play (additive)\n'{0}'"), FText::FromString(InAssetData.GetObjectPathString()));
				}
				else
				{
					return FText::Format(LOCTEXT("MenuDescTooltipFormat_Play", "Play\n'{0}'"), FText::FromString(InAssetData.GetObjectPathString()));
				}
			}
			else
			{
				return LOCTEXT("PlayerDescTooltip", "SN Sequence Player");
			}
		},
		[](UEdGraphNode* InNewNode, bool bInIsTemplateNode, const FAssetData InAssetData)
		{
			UAnimGraphNode_AssetPlayerBase::SetupNewNode(InNewNode, bInIsTemplateNode, InAssetData);
		});	
}

EAnimAssetHandlerType UAnimGraphNode_SNSequencePlayer::SupportsAssetClass(const UClass* AssetClass) const
{
	if (AssetClass->IsChildOf(UAnimSequence::StaticClass()) || AssetClass->IsChildOf(UAnimComposite::StaticClass()))
	{
		return EAnimAssetHandlerType::PrimaryHandler;
	}
	else
	{
		return EAnimAssetHandlerType::NotSupported;
	}
}

void UAnimGraphNode_SNSequencePlayer::GetOutputLinkAttributes(FNodeAttributeArray& OutAttributes) const
{
	Super::GetOutputLinkAttributes(OutAttributes);

	if (UE::Anim::IAnimRootMotionProvider::Get())
	{
		OutAttributes.Add(UE::Anim::IAnimRootMotionProvider::AttributeName);
	}
}

void UAnimGraphNode_SNSequencePlayer::ValidateAnimNodeDuringCompilation(class USkeleton* ForSkeleton, class FCompilerResultsLog& MessageLog)
{
	Super::ValidateAnimNodeDuringCompilation(ForSkeleton, MessageLog);

	ValidateAnimNodeDuringCompilationHelper(ForSkeleton, MessageLog, Node.GetSequence(), UAnimSequenceBase::StaticClass(), FindPin(GET_MEMBER_NAME_STRING_CHECKED(FAnimNode_SNSequencePlayer, Sequence)), GET_MEMBER_NAME_CHECKED(FAnimNode_SNSequencePlayer, Sequence));
}

void UAnimGraphNode_SNSequencePlayer::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	if (!Context->bIsDebugging)
	{
		// add an option to convert to single frame
		{
			FToolMenuSection& Section = Menu->AddSection("AnimGraphNodeSequencePlayer", LOCTEXT("SequencePlayerHeading", "SN Sequence Player"));
			Section.AddMenuEntry(FAnimGraphCommands::Get().OpenRelatedAsset);
			Section.AddMenuEntry(FAnimGraphCommands::Get().ConvertToSeqEvaluator);
		}
	}
}

void UAnimGraphNode_SNSequencePlayer::SetAnimationAsset(UAnimationAsset* Asset)
{
	if (UAnimSequenceBase* Seq = Cast<UAnimSequenceBase>(Asset))
	{
		Node.SetSequence(Seq);
	}
}

void UAnimGraphNode_SNSequencePlayer::CopySettingsFromAnimationAsset(UAnimationAsset* Asset)
{
	if (UAnimSequenceBase* Seq = Cast<UAnimSequenceBase>(Asset))
	{
		Node.SetLoopAnimation(Seq->bLoop);
	}
}

void UAnimGraphNode_SNSequencePlayer::OnOverrideAssets(IAnimBlueprintNodeOverrideAssetsContext& InContext) const
{
	if(InContext.GetAssets().Num() > 0)
	{
		if (UAnimSequenceBase* Sequence = Cast<UAnimSequenceBase>(InContext.GetAssets()[0]))
		{
			FAnimNode_SNSequencePlayer& AnimNode = InContext.GetAnimNode<FAnimNode_SNSequencePlayer>();
			AnimNode.SetSequence(Sequence);
		}
	}
}

void UAnimGraphNode_SNSequencePlayer::BakeDataDuringCompilation(class FCompilerResultsLog& MessageLog)
{
	UAnimBlueprint* AnimBlueprint = GetAnimBlueprint();
	AnimBlueprint->FindOrAddGroup(Node.GetGroupName());
}

void UAnimGraphNode_SNSequencePlayer::GetAllAnimationSequencesReferred(TArray<UAnimationAsset*>& AnimationAssets) const
{
	if(Node.GetSequence())
	{
		HandleAnimReferenceCollection(Node.GetSequence(), AnimationAssets);
	}
}

void UAnimGraphNode_SNSequencePlayer::ReplaceReferredAnimations(const TMap<UAnimationAsset*, UAnimationAsset*>& AnimAssetReplacementMap)
{
	HandleAnimReferenceReplacement(Node.Sequence, AnimAssetReplacementMap);
}

bool UAnimGraphNode_SNSequencePlayer::DoesSupportTimeForTransitionGetter() const
{
	return true;
}

UAnimationAsset* UAnimGraphNode_SNSequencePlayer::GetAnimationAsset() const 
{
	UAnimBlueprint* AnimBlueprint(GetAnimBlueprint());

	if(AnimBlueprint != nullptr)
	{
		//UAnimInstance* AnimInstance = NewObject<UAnimInstance>(GetWorld(), AnimBlueprint->GetAnimBlueprintGeneratedClass());

		//UAnimInstance* LinkedAnimInstance(AnimInstance->GetLinkedAnimLayerInstanceByClass(USNAnimInstanceBase::StaticClass()));

		//FString AAA(LinkedAnimInstance->GetName());
		
		FString Name(AnimBlueprint->GetName());

		SNPLUGIN_LOG(TEXT("%s"), *Name);
	}
	UAnimSequenceBase* Sequence = Node.GetSequence();
	UEdGraphPin* SequencePin = FindPin(GET_MEMBER_NAME_STRING_CHECKED(FAnimNode_SNSequencePlayer, Sequence));
	if (SequencePin != nullptr && Sequence == nullptr)
	{
		Sequence = Cast<UAnimSequenceBase>(SequencePin->DefaultObject);
	}

	return Sequence;
}

const TCHAR* UAnimGraphNode_SNSequencePlayer::GetTimePropertyName() const 
{
	return TEXT("InternalTimeAccumulator");
}

UScriptStruct* UAnimGraphNode_SNSequencePlayer::GetTimePropertyStruct() const 
{
	return FAnimNode_SNSequencePlayer::StaticStruct();
}

void UAnimGraphNode_SNSequencePlayer::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	Super::CustomizeDetails(DetailBuilder);

	if (!UE::Anim::IPoseSearchProvider::IsAvailable())
	{
		DetailBuilder.HideCategory(TEXT("PoseMatching"));

		DetailBuilder.HideProperty(TEXT("Sequence"));
	}
}

void UAnimGraphNode_SNSequencePlayer::CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 ArrayIndex) const
{
	Super::CustomizePinData(Pin, SourcePropertyName, ArrayIndex);

	if (Pin->PinName == GET_MEMBER_NAME_STRING_CHECKED(FAnimNode_SNSequencePlayer, PlayRate))
	{
		if (!Pin->bHidden)
		{
			// Draw value for PlayRateBasis if the pin is not exposed
			UEdGraphPin* PlayRateBasisPin = FindPin(GET_MEMBER_NAME_STRING_CHECKED(FAnimNode_SNSequencePlayer, PlayRateBasis));
			if (!PlayRateBasisPin || PlayRateBasisPin->bHidden)
			{
				if (Node.GetPlayRateBasis() != 1.f)
				{
					FFormatNamedArguments Args;
					Args.Add(TEXT("PinFriendlyName"), Pin->PinFriendlyName);
					Args.Add(TEXT("PlayRateBasis"), FText::AsNumber(Node.GetPlayRateBasis()));
					Pin->PinFriendlyName = FText::Format(LOCTEXT("FAnimNode_SNSequencePlayer_PlayRateBasis_Value", "({PinFriendlyName} / {PlayRateBasis})"), Args);
				}
			}
			else // PlayRateBasisPin is visible
			{
				FFormatNamedArguments Args;
				Args.Add(TEXT("PinFriendlyName"), Pin->PinFriendlyName);
				Pin->PinFriendlyName = FText::Format(LOCTEXT("FAnimNode_SNSequencePlayer_PlayRateBasis_Name", "({PinFriendlyName} / PlayRateBasis)"), Args);
			}

			Pin->PinFriendlyName = Node.GetPlayRateScaleBiasClampConstants().GetFriendlyName(Pin->PinFriendlyName);
		}
	}
}

void UAnimGraphNode_SNSequencePlayer::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = (PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None);

	// Reconstruct node to show updates to PinFriendlyNames.
	if ((PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FAnimNode_SNSequencePlayer, PlayRateBasis))
		|| (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FInputScaleBiasClampConstants, bMapRange))
		|| (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FInputRange, Min))
		|| (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FInputRange, Max))
		|| (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FInputScaleBiasClampConstants, Scale))
		|| (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FInputScaleBiasClampConstants, Bias))
		|| (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FInputScaleBiasClampConstants, bClampResult))
		|| (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FInputScaleBiasClampConstants, ClampMin))
		|| (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FInputScaleBiasClampConstants, ClampMax))
		|| (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FInputScaleBiasClampConstants, bInterpResult))
		|| (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FInputScaleBiasClampConstants, InterpSpeedIncreasing))
		|| (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FInputScaleBiasClampConstants, InterpSpeedDecreasing)))
	{
		ReconstructNode();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}


#undef LOCTEXT_NAMESPACE
