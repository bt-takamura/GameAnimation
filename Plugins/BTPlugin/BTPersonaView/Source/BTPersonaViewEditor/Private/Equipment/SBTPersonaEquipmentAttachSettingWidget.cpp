//--------------------------------------------------
//! @file   SBTPersonaEquipmentAttachSettingWidget.cpp
//! @brief  BTペルソナ機器アタッチ設定ウィジェット
//! @auther BT.Masaki Okuda
//--------------------------------------------------
#include "SBTPersonaEquipmentAttachSettingWidget.h"

#include "BTPersonaEquipmentAttachComponent.h"
#include "BTPersonaEquipmentComponentSettingCustomization.h"
#include "IPersonaPreviewScene.h"
#include "IPersonaToolkit.h"
#include "SlateOptMacros.h"

#include "Equipment/BTPersonaEquipmentProviderAssetInterface.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

namespace BTPersonaEquipmentPrivate
	{
		/** 生成済みのコンポーネントリストアイテム情報 */
		struct FLiveComponentItemData {
			/** リストアイテムとして表示するテキスト */
			FText DisplayText;
			/** ToolTipテキスト表示 */
			FText ToolTipText;
			/** FBTPersonaEquipmentAttachComponentSpawnInfo::ComponentTemplateInstanceのUniqueID */
			int32 SpawnComponentInfoID;
		};

		enum class EPresetTreeDataNode {
			PresetNode,	//!< プリセット表示用ノード
			AssetPathNode, //!< アセットパス表示用ノード:PresetNodeを子ノードとして持つ
		};

		/** プリセットウィジェット表示用ノード */
		struct FPresetTreeDataNode {
			explicit FPresetTreeDataNode(EPresetTreeDataNode InNodeType)
				: NodeType(InNodeType) { ; }

			/** ノードタイプ */
			const EPresetTreeDataNode NodeType;
			/** ノードに関連したアセット(IBTPersonaEquipmentProviderAssetInterface継承) */
			TStrongObjectPtr<UObject> RelatedProviderAsset;

			/** (アセットパスノードのみ)プリセットノード配列 */
			TArray<TSharedPtr<FPresetTreeDataNode>> PresetNodes;
			/** (プリセット表示用ノードのみ)プリセット機器データ */
			FBTPersonaEquipmentData EquipmentPresetData;
		};

		/** 参考：SGraphActionMenu.cpp::SCategoryHeaderTableRow */
		template<typename ItemType>
		class SProviderPathHeaderTableRow : public STableRow<ItemType> {
		public:
			SLATE_BEGIN_ARGS(SProviderPathHeaderTableRow)
				{
				}

				SLATE_DEFAULT_SLOT(typename SProviderPathHeaderTableRow::FArguments, Content)
			SLATE_END_ARGS()

			void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
			{
				STableRow<ItemType>::ChildSlot
						.Padding(0.0f, 2.0f, .0f, 0.0f)
						[
							SAssignNew(ContentBorder, SBorder)
					.BorderImage(this, &SProviderPathHeaderTableRow::GetBackgroundImage)
					.Padding(FMargin(3.0f, 5.0f))
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.VAlign(VAlign_Center)
								.Padding(5.0f)
								.AutoWidth()
								[
									SNew(SExpanderArrow, STableRow< ItemType >::SharedThis(this))
								]
								+ SHorizontalBox::Slot()
								.VAlign(VAlign_Center)
								.AutoWidth()
								[
									InArgs._Content.Widget
								]
							]
						];

				STableRow<ItemType>::ConstructInternal(
					typename STableRow<ItemType>::FArguments()
					.Style(FAppStyle::Get(), "DetailsView.TreeView.TableRow")
					.ShowSelection(false),
					InOwnerTableView
					);
			}

			const FSlateBrush* GetBackgroundImage() const
			{
				if( STableRow<ItemType>::IsHovered() )
				{
					return FAppStyle::Get().GetBrush("Brushes.Secondary");
				}
				else
				{
					return FAppStyle::Get().GetBrush("Brushes.Header");
				}
			}

			virtual void SetContent(TSharedRef<SWidget> InContent) override
			{
				ContentBorder->SetContent(InContent);
			}

			virtual void SetRowContent(TSharedRef<SWidget> InContent) override
			{
				ContentBorder->SetContent(InContent);
			}

			virtual const FSlateBrush* GetBorder() const override
			{
				return nullptr;
			}

			virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
			{
				if( MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton )
				{
					STableRow<ItemType>::ToggleExpansion();
					return FReply::Handled();
				}
				else
				{
					return FReply::Unhandled();
				}
			}

		private:
			TSharedPtr<SBorder> ContentBorder;
		};
	}// namespace BTPersonaEquipmentPrivate


void SBTPersonaEquipmentAttachSettingWidget::Construct(const FArguments& InArgs)
{
	WeakPersonaToolkit = InArgs._InPersonaToolkit;
	checkf(WeakPersonaToolkit.IsValid(), TEXT("Invalid PersonaToolkit"));

	const TSharedPtr<IPersonaToolkit> PersonaKit = WeakPersonaToolkit.Pin();
	const AActor* PreviewActor = PersonaKit->GetPreviewScene()->GetActor();
	EquipmentAttachComponent = (PreviewActor != nullptr) ? PreviewActor->FindComponentByClass<UBTPersonaEquipmentAttachComponent>() : nullptr;
	check(EquipmentAttachComponent);

	if( EquipmentAttachComponent != nullptr )
	{
		// 5番リスト関連初期化
		OnEquipmentAttachComponentChanged();
		EquipmentAttachComponent->OnEquipmentAttachComponentChanged().AddSP(this, &SBTPersonaEquipmentAttachSettingWidget::OnEquipmentAttachComponentChanged);
	}

	// プリセット提供表示用ノードをさくせい
	CreatePresetTreeDataList();

	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bHideSelectionTip = true;
	DetailsViewArgs.bShowSectionSelector = true;
	DetailsViewArgs.NotifyHook = this; // プロパティ全体の変更を検知する(変更後コンポ―ネント再生成を行う)
	DetailsViewArgs.ViewIdentifier = TEXT("SBTPersonaEquipmentAttachSettingWidget");;

	SelectedComponentDetailsViewWidget = PropertyModule.CreateDetailView(DetailsViewArgs);
	check(SelectedComponentDetailsViewWidget.IsValid());

	if( SelectedComponentDetailsViewWidget.IsValid() )
	{// コンポーネント詳細パネルのカスタマイズ
		SelectedComponentDetailsViewWidget->RegisterInstancedCustomPropertyLayout(UActorComponent::StaticClass()
			, FOnGetDetailCustomizationInstance::CreateStatic(&FBTPersonaEquipmentComponentSettingCustomization::MakeInstance
				, MakeWeakObjectPtr(EquipmentAttachComponent), WeakPersonaToolkit.Pin()->GetPreviewScene().ToWeakPtr(), StaticCastSharedRef<SWidget>(SharedThis(this))));
	}

	// ウィジェット配置イメージはクラス宣言のコメントを参照すること
	ChildSlot[
		SNew(SSplitter)
		.Orientation(Orient_Horizontal)
		.PhysicalSplitterHandleSize(4.0f)
		+ SSplitter::Slot()
		[// 1、２、３、４，５ウィジェット
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[// 1ウィジェット説明
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("アタッチプリセットリスト")))
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.FillHeight(1)
			.Padding(10, 1)
			[// 1ウィジェット
				SAssignNew(PresetTreeViewWidget, STreeView<TSharedPtr<BTPersonaEquipmentPrivate::FPresetTreeDataNode>>)
				.ItemHeight(24)
				.TreeItemsSource(&PresetTreeDataTreeList)
				.SelectionMode(ESelectionMode::Multi)
				.IsEnabled_UObject(EquipmentAttachComponent.Get(), &UBTPersonaEquipmentAttachComponent::CanSetEquipmentComponentExternal)
				.OnGetChildren_Static(&SBTPersonaEquipmentAttachSettingWidget::GetPresetChildNode)
				.OnGenerateRow_Static(&SBTPersonaEquipmentAttachSettingWidget::GeneratePresetTreeViewWidget)
				.HighlightParentNodesForSelection(true)
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoHeight()
			.Padding(10, 5)
			[// 2ウィジェット
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.AutoWidth()
				[
					SNew(STextBlock)
					.ColorAndOpacity(this, &SBTPersonaEquipmentAttachSettingWidget::GetPersonaEquipmentErrorStateColor)
					.Text(this, &SBTPersonaEquipmentAttachSettingWidget::GetPersonaEquipmentErrorStateText)
					.ToolTipText(this, &SBTPersonaEquipmentAttachSettingWidget::GetPersonaEquipmentErrorStateToolTipText)
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Right)
				.AutoWidth()
				[
					SNew(SButton)
					.OnClicked(this, &SBTPersonaEquipmentAttachSettingWidget::ApplySelectedPresets)
					.ForegroundColor(FAppStyle::GetSlateColor(TEXT("DefaultForeground")))
					.ButtonStyle(FAppStyle::Get(), "PrimaryButton")
					.ContentPadding(FMargin(10, 4))
					.IsEnabled_UObject(EquipmentAttachComponent.Get(), &UBTPersonaEquipmentAttachComponent::CanSetEquipmentComponentExternal)
					.ToolTipText(FText::FromString(TEXT("選択したプリセットを反映します。")))
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
							.TextStyle(FAppStyle::Get(), "ContentBrowser.TopBar.Font")
							.Font(FAppStyle::Get().GetFontStyle("Bold"))
							.Text(FText::FromString(FString(TEXT("反映"))))
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
							.TextStyle(FAppStyle::Get(), "ContentBrowser.TopBar.Font")
							.Font(FAppStyle::Get().GetFontStyle("FontAwesome.12"))
							.Text(FText::FromString(FString(TEXT("\xf061"))))
							// \xf061 => fa-arrow-right
						]
					]
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[// 5ウィジェット説明
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("生成済みのコンポーネントリスト")))
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.FillHeight(1)
			[// 5ウィジェット
				SAssignNew(LiveComponentsListViewWidget, SListView< TSharedPtr<BTPersonaEquipmentPrivate::FLiveComponentItemData> >)
				.ListItemsSource(&LiveComponentsList)
				.OnGenerateRow_Static(&SBTPersonaEquipmentAttachSettingWidget::GenerateEquipmentComponentItemRow)
				.ItemHeight(28)
				.SelectionMode(ESelectionMode::SingleToggle)
				.OnSelectionChanged(this, &SBTPersonaEquipmentAttachSettingWidget::OnEquipmentComponentSelectChanged)
				.OnContextMenuOpening(this, &SBTPersonaEquipmentAttachSettingWidget::GenerateEquipmentComponentListContextMenu)
				.ClearSelectionOnClick(true)
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoHeight()
			.Padding(10, 5)
			[// 4ウィジェット
				SNew(SButton)
				.OnClicked(this, &SBTPersonaEquipmentAttachSettingWidget::ApplyRemoveAllComponents)
				.ForegroundColor(FAppStyle::GetSlateColor(TEXT("DefaultForeground")))
				.ButtonStyle(FAppStyle::Get(), "DetailsView.NameAreaButton")
				.ContentPadding(FMargin(2, 2))
				.IsEnabled(this, &SBTPersonaEquipmentAttachSettingWidget::CanRemoveAnyComponents)
				.ToolTipText(FText::FromString(TEXT("生成コンポーネントを全て削除します。")))
				[
					SNew(STextBlock)
					.TextStyle(FAppStyle::Get(), "ContentBrowser.TopBar.Font")
					.Font(FAppStyle::Get().GetFontStyle("Bold"))
					.Text(FText::FromString(FString(TEXT("全て削除"))))
				]
			]
		]
		+ SSplitter::Slot()
		[// 6ウィジェット
			SelectedComponentDetailsViewWidget.ToSharedRef()
		]
	];
}

void SBTPersonaEquipmentAttachSettingWidget::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EquipmentAttachComponent);
}

FString SBTPersonaEquipmentAttachSettingWidget::GetReferencerName() const
{
	return TEXT("SBTPersonaEquipmentAttachSettingWidgetReferencer");
}

void SBTPersonaEquipmentAttachSettingWidget::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	FNotifyHook::NotifyPostChange(PropertyChangedEvent, PropertyThatChanged);

	// コンポ―ネント詳細パネルからテンプレートインスタンスの変更を検知したため、表示用コンポ―ネントを再生成する
	if( SelectedComponentDetailsViewWidget.IsValid() && EquipmentAttachComponent != nullptr )
	{
		for( TWeakObjectPtr<UObject> SelectedWeakObject : SelectedComponentDetailsViewWidget->GetSelectedObjects() )
		{
			const UObject* SelectedObject = SelectedWeakObject.Get();
			if( SelectedObject == nullptr || SelectedObject->IsA<UActorComponent>() == false ) { continue; }
			EquipmentAttachComponent->RecreateSpawnedComponentFromTemplateInstanceUniqueID(SelectedObject->GetUniqueID());
		}
		EquipmentAttachComponent->MarkComponentChangedExternal();
	}
}

/**
 * @brief 生成コンポーネント変更時呼び出しイベント
 */
void SBTPersonaEquipmentAttachSettingWidget::OnEquipmentAttachComponentChanged()
{
	TArray<TSharedPtr<BTPersonaEquipmentPrivate::FLiveComponentItemData>, TInlineAllocator<16>> NewLiveComponentsList;
	TSharedPtr<BTPersonaEquipmentPrivate::FLiveComponentItemData> LastCreatedItemData; // 最後に生成したコンポーネント情報(ここを選択するようにする)

	ON_SCOPE_EXIT
		{// 関数終了時に必ず新規コンポ―ネント配列を適用する
			LiveComponentsList = NewLiveComponentsList;

			if( LiveComponentsListViewWidget.IsValid() )
			{
				// リスト表示更新
				LiveComponentsListViewWidget->RebuildList();

				if( LastCreatedItemData.IsValid() )
				{// 最後に生成したコンポーネント情報を選択する
					LiveComponentsListViewWidget->SetSelection(LastCreatedItemData);
				}
			}
		};

	if( EquipmentAttachComponent == nullptr ) { return; }

	// 生成コンポーネント情報配列からアイテム配列に変換する
	// アイテム配列に設定済みの要素があればそれを使用する
	for( const FBTPersonaEquipmentAttachComponentSpawnInfo& SpawnedComponentElm : EquipmentAttachComponent->GetSpawnedComponentInfos() )
	{
		const int32 UniqueID = SpawnedComponentElm.GetTemplateInstanceUniqueID();
		if( UniqueID == INDEX_NONE ) { continue; }

		const TSharedPtr<BTPersonaEquipmentPrivate::FLiveComponentItemData>* FindItemData = LiveComponentsList.FindByPredicate([UniqueID](const TSharedPtr<BTPersonaEquipmentPrivate::FLiveComponentItemData>& InPtr)
			{
				return InPtr->SpawnComponentInfoID == UniqueID;
			});

		if( FindItemData != nullptr && FindItemData->IsValid() )
		{// すでに設定済み
			NewLiveComponentsList.Add(*FindItemData);
		}
		else if( SpawnedComponentElm.GetTemplateInstanceUniqueID() != INDEX_NONE )
		{// 新しく設定する
			const FBTPersonaEquipmentComponentData& ComponentData = SpawnedComponentElm.EquipmentComponentData;
			TSharedPtr<BTPersonaEquipmentPrivate::FLiveComponentItemData> NewItemData = MakeShareable<BTPersonaEquipmentPrivate::FLiveComponentItemData>(new BTPersonaEquipmentPrivate::FLiveComponentItemData());
			NewItemData->SpawnComponentInfoID = SpawnedComponentElm.GetTemplateInstanceUniqueID();
			NewItemData->DisplayText = FText::FromString(GetNameSafe(SpawnedComponentElm.ComponentTemplateInstance));
			NewItemData->ToolTipText = FText::FromString(ComponentData.ToString());
			NewLiveComponentsList.Add(NewItemData);
			LastCreatedItemData = NewItemData;;
		}
	}
}

/**
 * @brief コンポ―ネントリストアイテム毎のウィジェット生成(5番)
 */
// ReSharper disable once CppPassValueParameterByConstReference
TSharedRef<ITableRow> SBTPersonaEquipmentAttachSettingWidget::GenerateEquipmentComponentItemRow(TSharedPtr<BTPersonaEquipmentPrivate::FLiveComponentItemData> InComponentItemData, const TSharedRef<STableViewBase>& InOwnerTable)
{
	check(InComponentItemData.IsValid());

	const TSharedPtr<ITableRow> NewRow = SNew(STableRow< TSharedPtr<FString> >, InOwnerTable)
	[
		SNew(STextBlock)
			.Text(InComponentItemData->DisplayText)
			.ToolTipText(InComponentItemData->ToolTipText)
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
	];
	return NewRow.ToSharedRef();
}

/**
 * @brief 生成コンポーネント選択変更時呼び出しイベント(5番)
 */
// ReSharper disable once CppPassValueParameterByConstReference
// ReSharper disable once CppMemberFunctionMayBeConst
void SBTPersonaEquipmentAttachSettingWidget::OnEquipmentComponentSelectChanged(TSharedPtr<BTPersonaEquipmentPrivate::FLiveComponentItemData> InComponentItemData, ESelectInfo::Type InType)
{
	UActorComponent* SelectedComponent = nullptr;

	// 選択情報からコンポ―ネントのテンプレートインスタンスを取得
	if( InComponentItemData.IsValid() && EquipmentAttachComponent != nullptr )
	{
		if( const FBTPersonaEquipmentAttachComponentSpawnInfo* FindSpawnInfo = EquipmentAttachComponent->FindSpawnedComponentInfoFromTemplateInstanceUniqueID(InComponentItemData->SpawnComponentInfoID) )
		{
			SelectedComponent = FindSpawnInfo->ComponentTemplateInstance;
		}
	}

	// 詳細表示するオブジェクトをウィジェット側に設定
	if( SelectedComponentDetailsViewWidget.IsValid() )
	{
		SelectedComponentDetailsViewWidget->SetObject(SelectedComponent);
	}
}

/**
 * @brief コンポ―ネントリスト右クリックで表示するメニュー作成(5番)
 */
TSharedPtr<SWidget> SBTPersonaEquipmentAttachSettingWidget::GenerateEquipmentComponentListContextMenu()
{
	constexpr bool bCloseAfterSelection = true;
	FMenuBuilder MenuBuilder(bCloseAfterSelection, TSharedPtr<FUICommandList>());

	if( LiveComponentsListViewWidget.IsValid() && LiveComponentsListViewWidget->GetNumItemsSelected() > 0 )
	{
		MenuBuilder.BeginSection(NAME_None);
		MenuBuilder.AddMenuEntry(
			FText::FromString(TEXT("コンポ―ネントを削除")),
			FText::FromString(TEXT("選択したコンポ―ネントを削除します。")),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.AssetActions.Delete"),
			FUIAction(FExecuteAction::CreateSP(this, &SBTPersonaEquipmentAttachSettingWidget::RemoveSelectedEquipmentComponent)));
		MenuBuilder.EndSection();
	}
	return MenuBuilder.MakeWidget();
}

/**
 * @brief 選択コンポ―ネントを削除する(5番)
 */
void SBTPersonaEquipmentAttachSettingWidget::RemoveSelectedEquipmentComponent() const
{
	if( LiveComponentsListViewWidget.IsValid() && EquipmentAttachComponent != nullptr )
	{
		// 選択されたコンポ―ネントをすべて削除
		TArray<TSharedPtr<BTPersonaEquipmentPrivate::FLiveComponentItemData>> SelectedItems = LiveComponentsListViewWidget->GetSelectedItems();
		for( TSharedPtr<BTPersonaEquipmentPrivate::FLiveComponentItemData> SelectItemElm : SelectedItems )
		{
			if( SelectItemElm.IsValid() )
			{
				EquipmentAttachComponent->RemoveSpawnedComponentFromTemplateInstanceUniqueID(SelectItemElm->SpawnComponentInfoID);
				EquipmentAttachComponent->MarkComponentChangedExternal();
			}
		}
	}
}

/**
 * @brief 2番の適用ボタンからプリセットを反映
 */
FReply SBTPersonaEquipmentAttachSettingWidget::ApplySelectedPresets() const
{
	if( PresetTreeViewWidget.IsValid() && EquipmentAttachComponent != nullptr && EquipmentAttachComponent->CanSetEquipmentComponentExternal() )
	{
		// 選択されたプリセットをすべて反映
		TArray<TSharedPtr<BTPersonaEquipmentPrivate::FPresetTreeDataNode>> SelectedItems = PresetTreeViewWidget->GetSelectedItems();

		TArray<FBTPersonaEquipmentComponentData, TInlineAllocator<64>> EquipmentComponentDataList;
		TArray<UObject*, TInlineAllocator<64>> ProviderAssets;

		// 選択されているプリセットからコンポ―ネントデータを取得
		for( TSharedPtr<BTPersonaEquipmentPrivate::FPresetTreeDataNode> InSelectedNodeItem : SelectedItems )
		{
			if( InSelectedNodeItem.IsValid() == false || InSelectedNodeItem->NodeType != BTPersonaEquipmentPrivate::EPresetTreeDataNode::PresetNode ) { continue; }

			for( const FBTPersonaEquipmentComponentData& ComponentDataElm : InSelectedNodeItem->EquipmentPresetData.ComponentData )
			{
				if( ComponentDataElm.IsValidData() == false ) { continue; }
				ProviderAssets.Add(InSelectedNodeItem->RelatedProviderAsset.Get());
				EquipmentComponentDataList.Add(ComponentDataElm);
			}
		}

		// コンポ―ネントを設定
		EquipmentAttachComponent->RequestEquipmentComponentExternal(EquipmentComponentDataList, ProviderAssets);
	}

	return FReply::Unhandled();
}

/**
 * @brief 1番で表示する機器プリセットリスト作成(1番)
 */
void SBTPersonaEquipmentAttachSettingWidget::CreatePresetTreeDataList()
{
	if( EquipmentAttachComponent == nullptr ) { return; }

	if( EquipmentAttachComponent->IsLoadedEquipmentProviderAssets() == false )
	{// 機器情報提供アセットがロードされていないのでロードするよう要求
		EquipmentAttachComponent->OnAssetProviderLoadCompleted().AddSP(this, &SBTPersonaEquipmentAttachSettingWidget::CreatePresetTreeDataList);
		EquipmentAttachComponent->RequestLoadEquipmentProviderAssets();
		return;
	}

	EquipmentAttachComponent->OnAssetProviderLoadCompleted().RemoveAll(this);
	PresetTreeDataTreeList.Empty();

	// プリセット提供アセットリストからツリーノードを生成
	for( UObject* LoadedProviderAsset : EquipmentAttachComponent->GetLoadedEquipmentProviderAssets() )
	{
		const IBTPersonaEquipmentProviderAssetInterface* ProviderAssetInterface = Cast<IBTPersonaEquipmentProviderAssetInterface>(LoadedProviderAsset);
		if( ProviderAssetInterface == nullptr ) { continue; }

		TArray<FBTPersonaEquipmentData> EquipmentDataList;
		ProviderAssetInterface->GetPersonaEquipmentData(EquipmentDataList);
		if( EquipmentDataList.Num() == 0 ) { continue; }

		// アセットパス表示ノードを作成
		const TSharedPtr<BTPersonaEquipmentPrivate::FPresetTreeDataNode>& AssetPathNodeRef = PresetTreeDataTreeList.Add_GetRef(MakeShareable(new BTPersonaEquipmentPrivate::FPresetTreeDataNode(BTPersonaEquipmentPrivate::EPresetTreeDataNode::AssetPathNode)));
		AssetPathNodeRef->RelatedProviderAsset = TStrongObjectPtr<UObject>(LoadedProviderAsset);

		AssetPathNodeRef->PresetNodes.Reserve(EquipmentDataList.Num());

		// 子にプリセット表示ノードを作成+ 追加
		for( const FBTPersonaEquipmentData& EquipmentPresetDataElm : EquipmentDataList )
		{
			const TSharedPtr<BTPersonaEquipmentPrivate::FPresetTreeDataNode>& PresetNodeRef = AssetPathNodeRef->PresetNodes.Add_GetRef(MakeShareable(new BTPersonaEquipmentPrivate::FPresetTreeDataNode(BTPersonaEquipmentPrivate::EPresetTreeDataNode::PresetNode)));
			PresetNodeRef->RelatedProviderAsset = TStrongObjectPtr<UObject>(LoadedProviderAsset);
			PresetNodeRef->EquipmentPresetData = EquipmentPresetDataElm;
		}
	}

	// リストを表示しなおす
	if( PresetTreeViewWidget.IsValid() )
	{
		PresetTreeViewWidget->RebuildList();
	}
}

/**
 * @brief プリセットノードの子ノードを取得(1番)
 */
// ReSharper disable once CppPassValueParameterByConstReference
void SBTPersonaEquipmentAttachSettingWidget::GetPresetChildNode(TSharedPtr<BTPersonaEquipmentPrivate::FPresetTreeDataNode> InItem, TArray<TSharedPtr<BTPersonaEquipmentPrivate::FPresetTreeDataNode>>& OutChildren)
{
	if( InItem.IsValid() ) { OutChildren = InItem->PresetNodes; }
}

/**
 * @brief 機器プリセットリストアイテム毎のウィジェット生成(1番) 
 */
// ReSharper disable once CppPassValueParameterByConstReference
TSharedRef<ITableRow> SBTPersonaEquipmentAttachSettingWidget::GeneratePresetTreeViewWidget(TSharedPtr<BTPersonaEquipmentPrivate::FPresetTreeDataNode> InNode, const TSharedRef<STableViewBase>& InOwnerTable)
{
	using namespace BTPersonaEquipmentPrivate;

	const FMargin RowPadding = FMargin(0, 2);

	TSharedPtr<STableRow<TSharedPtr<FPresetTreeDataNode>>> TableRow;
	TSharedPtr<SHorizontalBox> RowContainer;
	TSharedPtr<SWidget> RowContent;

	// Row
	if( InNode->NodeType == EPresetTreeDataNode::AssetPathNode )
	{
		TableRow = SNew(SProviderPathHeaderTableRow< TSharedPtr<FPresetTreeDataNode> >, InOwnerTable);
	}
	else
	{
		TableRow = SNew(STableRow< TSharedPtr<FPresetTreeDataNode> >, InOwnerTable);
	}

	// RowContent
	if( InNode->NodeType == EPresetTreeDataNode::AssetPathNode )
	{
		RowContent = SNew(STextBlock)
			.Text(FText::FromString(InNode->RelatedProviderAsset->GetName()))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10));
	}
	else
	{
		RowContent = SNew(STextBlock)
			.Text(InNode->EquipmentPresetData.DisplayName)
			.ToolTipText((InNode->EquipmentPresetData.ToolTip.IsEmpty()) ? InNode->EquipmentPresetData.DisplayName : InNode->EquipmentPresetData.ToolTip)
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10));
	}

	TableRow->SetRowContent
		(
		SAssignNew(RowContainer, SHorizontalBox)
		);

	RowContainer->AddSlot()
				.AutoWidth()
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Right)
	[
		SNew(SExpanderArrow, TableRow)
		.BaseIndentLevel(1)
	];

	RowContainer->AddSlot()
				.FillWidth(1.0)
				.Padding(RowPadding)
	[
		RowContent.ToSharedRef()
	];

	return TableRow.ToSharedRef();
}

/**
 * @brief 生成コンポ―ネント削除はかのうか(4番)
 */
bool SBTPersonaEquipmentAttachSettingWidget::CanRemoveAnyComponents() const
{
	if( EquipmentAttachComponent == nullptr ) { return false; }
	return LiveComponentsList.Num() > 0;
}

/**
 * @brief コンポ―ネント削除ボタンの入力を反映(4番)
 */
FReply SBTPersonaEquipmentAttachSettingWidget::ApplyRemoveAllComponents() const
{
	if( EquipmentAttachComponent != nullptr )
	{
		EquipmentAttachComponent->RequestRemoveAllSpawnedComponentsExternal();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

/**
 * @brief エラー状況取得(2番左)
 */
SBTPersonaEquipmentAttachSettingWidget::EPersonaEquipmentErrorState SBTPersonaEquipmentAttachSettingWidget::GetPersonaEquipmentErrorState() const
{
	if( WeakPersonaToolkit.IsValid() == false || EquipmentAttachComponent == nullptr )
	{
		return EPersonaEquipmentErrorState::AnyUnitsInvalid;
	}

	if( EquipmentAttachComponent->IsLoadingAnyAssets() )
	{
		return EPersonaEquipmentErrorState::AnyAssetLoading;
	}

	if( EquipmentAttachComponent->IsLoadedEquipmentProviderAssets() && EquipmentAttachComponent->GetLoadedEquipmentProviderAssets().Num() == 0 )
	{
		return EPersonaEquipmentErrorState::ProviderAssetNone;
	}

	return EPersonaEquipmentErrorState::NoError;
}

/**
 * @brief ウィジェットerror状況テキスト取得(2番左)
 */
FText SBTPersonaEquipmentAttachSettingWidget::GetPersonaEquipmentErrorStateText() const
{
	switch( GetPersonaEquipmentErrorState() )
	{
	case EPersonaEquipmentErrorState::AnyAssetLoading :
		return FText::FromString(TEXT("アセットロード中..."));
	case EPersonaEquipmentErrorState::ProviderAssetNone :
		return FText::FromString(TEXT("機器情報提供アセットが存在しません"));
	case EPersonaEquipmentErrorState::AnyUnitsInvalid :
		return FText::FromString(TEXT("オブジェクトエラー"));
	case EPersonaEquipmentErrorState::NoError :
	default :
		return FText();
	}
}

/**
 * @brief ウィジェットerror状況tooltipテキスト取得(2番左)
 */
FText SBTPersonaEquipmentAttachSettingWidget::GetPersonaEquipmentErrorStateToolTipText() const
{
	switch( GetPersonaEquipmentErrorState() )
	{
	case EPersonaEquipmentErrorState::AnyAssetLoading :
		return FText::FromString(TEXT("裏でアセットをロード中です。しばらくお待ちください。"));
	case EPersonaEquipmentErrorState::ProviderAssetNone :
		return FText::FromString(TEXT("機器情報提供アセットが設定されていません。エディタ設定から機器情報提供アセットを設定してください。"));
	case EPersonaEquipmentErrorState::AnyUnitsInvalid :
		return FText::FromString(TEXT("本機能で必要なインスタンスが存在しません。"));
	case EPersonaEquipmentErrorState::NoError :
	default :
		return FText();
	}
}

/**
 * @brief ウィジェットerror状況カラー取得(2番左)
 */
FSlateColor SBTPersonaEquipmentAttachSettingWidget::GetPersonaEquipmentErrorStateColor() const
{
	switch( GetPersonaEquipmentErrorState() )
	{
	case EPersonaEquipmentErrorState::AnyAssetLoading :
	case EPersonaEquipmentErrorState::AnyUnitsInvalid :
		return FSlateColor(FColor::Red);
	case EPersonaEquipmentErrorState::NoError :
	case EPersonaEquipmentErrorState::ProviderAssetNone :
	default :
		return FSlateColor(FColor::White);
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
