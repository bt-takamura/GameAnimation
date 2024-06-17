//--------------------------------------------------
//! @file   SBTPersonaEquipmentAttachSettingWidget.h
//! @brief  BTペルソナ機器アタッチ設定ウィジェット
//! @auther BT.Masaki Okuda
//--------------------------------------------------
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
class UBTPersonaEquipmentAttachComponent;
class IPersonaToolkit;

namespace BTPersonaEquipmentPrivate
	{
		struct FLiveComponentItemData;
		struct FPresetTreeDataNode;
	}

/**
 * BTペルソナ機器アタッチ設定ウィジェット
 * @note:
 * ウィンドウ作成イメージ
 * ---------------------
 * |  1        |    |  6               |
 * |            |    |                   |
 * ------  |    |                   |
 *      |  2   |    |                   |
 * ------  |    |                   |
 * |  5        |    |                   |
 * |            |    |                   |
 * ------  |    |                   |
 *      |  4   |    |                   |
 * ---------------------
 * 1:機器プリセット一覧表示
 * 2:機器プリセット追加ボタン
 * 4:生成コンポーネント全削除ボタン
 * 5:プレビューコンポーネント一覧表示
 * 6:5番で選択しているコンポ―ネントの詳細表示
 */
class BTPERSONAVIEWEDITOR_API SBTPersonaEquipmentAttachSettingWidget : public SCompoundWidget, public FGCObject, public FNotifyHook {
	typedef SCompoundWidget Super;

private:
	enum class EPersonaEquipmentErrorState {
		NoError,
		AnyUnitsInvalid, //!< 必要なオブジェクトが存在しない
		AnyAssetLoading, //!< 必要アセットをロード中
		ProviderAssetNone,//!< 機器提供アセットがない
	};

public:
	SLATE_BEGIN_ARGS(SBTPersonaEquipmentAttachSettingWidget)
		{
		}

		SLATE_ARGUMENT(TWeakPtr<IPersonaToolkit>, InPersonaToolkit)
	SLATE_END_ARGS()


	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	// Begin FGCObject interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override final;
	virtual FString GetReferencerName() const override final;
	// End of FGCObject interface

	// Begin FNotifyHook interface
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;
	// End of FNotifyHook interface
private:
	/** 生成コンポーネント変更時呼び出しイベント */
	void OnEquipmentAttachComponentChanged();
	/** コンポ―ネントリストアイテム毎のウィジェット生成(5番) */
	static TSharedRef<class ITableRow> GenerateEquipmentComponentItemRow(TSharedPtr<BTPersonaEquipmentPrivate::FLiveComponentItemData> InComponentItemData, const TSharedRef<STableViewBase>& InOwnerTable);
	/** 生成コンポーネント選択変更時呼び出しイベント(5番) */
	void OnEquipmentComponentSelectChanged(TSharedPtr<BTPersonaEquipmentPrivate::FLiveComponentItemData> InComponentItemData, ESelectInfo::Type InType);
	/** コンポ―ネントリスト右クリックで表示するメニュー作成(5番) */
	TSharedPtr<SWidget> GenerateEquipmentComponentListContextMenu();
	/** 選択コンポ―ネントを削除する(5番) */
	void RemoveSelectedEquipmentComponent() const;
	/** 2番の適用ボタンからプリセットを反映 */
	FReply ApplySelectedPresets() const;

	/** 1番で表示する機器プリセットリスト作成(1番) */
	void CreatePresetTreeDataList();
	/** プリセットノードの子ノードを取得(1番) */
	static void GetPresetChildNode(TSharedPtr<BTPersonaEquipmentPrivate::FPresetTreeDataNode> InItem, TArray<TSharedPtr<BTPersonaEquipmentPrivate::FPresetTreeDataNode>>& OutChildren);
	/** 機器プリセットリストアイテム毎のウィジェット生成(1番) */
	static TSharedRef<class ITableRow> GeneratePresetTreeViewWidget(TSharedPtr<BTPersonaEquipmentPrivate::FPresetTreeDataNode> InNode, const TSharedRef<STableViewBase>& InOwnerTable);

	/** 生成コンポ―ネント削除はかのうか(4番) */
	bool CanRemoveAnyComponents() const;
	/** コンポ―ネント削除ボタンの入力を反映(4番) */
	FReply ApplyRemoveAllComponents() const;

	/** エラー状況取得(2番左)*/
	EPersonaEquipmentErrorState GetPersonaEquipmentErrorState() const;
	/** ウィジェットerror状況テキスト取得(2番左)*/
	FText GetPersonaEquipmentErrorStateText() const;
	/** ウィジェットerror状況tooltipテキスト取得(2番左)*/
	FText GetPersonaEquipmentErrorStateToolTipText() const;
	/** ウィジェットerror状況カラー取得(2番左)*/
	FSlateColor GetPersonaEquipmentErrorStateColor() const;

private:
	/** ペルソナツールキット(Constructで取得) */
	TWeakPtr<IPersonaToolkit> WeakPersonaToolkit;
	/** アタッチしているコンポ―ネント */
	TObjectPtr<UBTPersonaEquipmentAttachComponent> EquipmentAttachComponent;

	/** コンポ―ネント詳細パラメータ表示ウィジェット(6番) */
	TSharedPtr<IDetailsView> SelectedComponentDetailsViewWidget;
	/** 5番で表示する生成済みのコンポーネントリスト */
	TArray<TSharedPtr<BTPersonaEquipmentPrivate::FLiveComponentItemData>> LiveComponentsList;
	/**プレビューコンポーネント一覧表示(5番) */
	TSharedPtr<SListView<TSharedPtr<BTPersonaEquipmentPrivate::FLiveComponentItemData>>> LiveComponentsListViewWidget;


	/** 機器プリセット一覧表示(1番)*/
	TSharedPtr<STreeView<TSharedPtr<BTPersonaEquipmentPrivate::FPresetTreeDataNode>>> PresetTreeViewWidget;
	/** 1番で表示する機器プリセットリスト */
	TArray<TSharedPtr<BTPersonaEquipmentPrivate::FPresetTreeDataNode>> PresetTreeDataTreeList;
};
