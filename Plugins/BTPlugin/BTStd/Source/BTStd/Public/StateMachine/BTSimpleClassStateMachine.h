#pragma once

/*
 * @brief 任意クラスの関数を登録して使うステートマシン
 * @tparam FClass ステートマシンを利用するクラス
 * @tparam FId Enum情報を基本的にこちらで指定する
 * @note SetState()系処理で各Stateの関数を登録した後、Procを呼び出して更新を行います
 *       ChangeState()系処理で他ステートに切り替えを行います
 */
template<class FClass, class FId>
class TBTSimpleClassStateMachine
{
public:
	typedef void (FClass::* FEnterFunc)(FId);
	typedef bool (FClass::* FProcFunc)(float);
	typedef void (FClass::* FExitFunc)(FId);

	TBTSimpleClassStateMachine(FClass* InClass) : Class(InClass)
	{
		check(Class);
	}

	//! @brief ステートの動作を一括で設定する
	//! @param[in] StateId ステート識別子
	//! @param[in] EnterFunc ステート開始関数
	//! @param[in] ProcFunc ステート更新関数
	//! @param[in] ExitFunc ステート終了関数
	void SetState(const FId StateId, FEnterFunc EnterFunc = nullptr, FProcFunc ProcFunc = nullptr, FExitFunc ExitFunc = nullptr);

	//! @brief ステートの開始関数を設定する
	//! @param[in] StateId ステート識別子
	//! @param[in] EnterFunc ステート開始関数
	void SetStateEnterFunc(const FId StateId, FEnterFunc EnterFunc);

	//! @brief ステートの更新関数を設定する
	//! @param[in] StateId ステート識別子
	//! @param[in] ProcFunc ステート更新関数
	void SetStateProcFunc(const FId StateId, FProcFunc ProcFunc);

	//! @brief ステートの更新関数を設定する
	//! @param[in] StateId ステート識別子
	//! @param[in] ExitFunc ステート終了関数
	void SetStateExitFunc(const FId StateId, FExitFunc ExitFunc);

	//! @brief ステートマシンを実行する
	//! @param[in] NewStateId 開始ステート識別子
	//! @param[in] bFailedIfNotFound 指定したステートが見つからなければ失敗を返すかどうか
	//! @retval true：成功, false：失敗
	bool Activate(const FId NewStateId, const bool bFailedIfNotFound = true){ return ChangeState(NewStateId, bFailedIfNotFound); }

	//! @brief ステートマシンを終了する
	//! @param[in] bCallExitFunc 現在のステートの終了処理を呼び出す
	void Deactivate(const bool bCallExitFunc = true);

	//! @brief ステートマシンを更新する
	//! @param[in] DeltaTime 1Fの経過時間
	bool Proc(float DeltaTime);

	//! @brief ステートの変更をリクエストする
	//! @note 次のProcが呼び出されたときに変更を実行します
	//! @param[in] NewStateId 変更の対象となるステート情報
	//! @param[in] bFailedIfNotFound 指定したステートが見つからなければ失敗を返すかどうか
	//! @retval true：成功, false：失敗
	bool ChangeStateRequest(const FId NewStateId, const bool bFailedIfNotFound = true);

	//! @brief ステートを変更する
	//! @param[in] NewStateId 変更の対象となるステート情報
	//! @param[in] bFailedIfNotFound 指定したステートが見つからなければ失敗を返すかどうか
	//! @retval true：成功, false：失敗
	bool ChangeState(const FId NewStateId, const bool bFailedIfNotFound = true);

	//! @brief ステートマシンが実行中かどうかを確認する
	//! @retval true：実行中, false：非実行
	bool IsActive() const { return CurrentState.IsSet(); }

	//! @brief ステートが動作してからの経過時間を返す
	//! @retval 経過時間（秒）
	float GetStateElapsedTime() const { return CurrentState->ElapsedTime; }

	//! @brief 現在のステートの識別子を返す
	//! @retval ステート識別子
	FId GetStateID() const { return CurrentState->Id; };

protected:

	//! @brief 各ステート関数のポインタの所持
	struct FState
	{
		//! @brief ステート起動時の関数
		FEnterFunc Enter = nullptr;

		//! @brief ステート更新時の関数
		FProcFunc Proc = nullptr;

		//! @brief ステート終了時の関数
		FExitFunc Exit = nullptr;
	};

	//! @brief 現在のステート情報
	struct FCurrentState : FState
	{
		~FCurrentState()
		{
			static_cast<FId*>(&Id)->FId::~FId();
		}

		//! @brief ステート識別子
		FId Id = {};

		//! @brief 経過時間
		float ElapsedTime = 0.0f;
	};

protected:

	//! @brief クラスのポインタ
	FClass* Class = nullptr;

	//! @brief 現在のステート
	TOptional<FCurrentState> CurrentState;

	//! @brief ステートのマップ
	TMap<FId, FState> StateMap;

	//! @brief 次に起動しようとしているステート
	TOptional<FId> RequestedNextStateId;
};

template<class FClass, class FId>
bool TBTSimpleClassStateMachine<FClass, FId>::Proc(float DeltaTime)
{
	bool bResult = false;

	if (ensureMsgf(CurrentState.IsSet(), TEXT("ステートが曖昧です。Activate()もしくはChangeState()の呼び出しが必要です")))
	{
		if (RequestedNextStateId.IsSet())
		{
			ChangeState(*RequestedNextStateId);
			RequestedNextStateId.Reset();
		}

		if (CurrentState->Proc)
		{
			check(Class);
			bResult = (Class->*CurrentState->Proc)(DeltaTime);
		}

		if(CurrentState.IsSet())
		{
			CurrentState->ElapsedTime += DeltaTime;
		}
	}

	return bResult;
}

template<class FClass, class FId>
bool TBTSimpleClassStateMachine<FClass, FId>::ChangeStateRequest(const FId NewStateId, const bool bFailedIfNotFound)
{
	check(Class);
	if (CurrentState.IsSet() && NewStateId == CurrentState->Id)
	{
		return false;
	}

	const FState* NewState = bFailedIfNotFound ? StateMap.Find(NewStateId) : &StateMap.FindOrAdd(NewStateId);
	if (NewState)
	{
		RequestedNextStateId = NewStateId;
		return true;
	}

	return false;
}

template<class FClass, class FId>
bool TBTSimpleClassStateMachine<FClass, FId>::ChangeState(const FId NewStateId, const bool bFailedIfNotFound)
{
	check(Class);
	if (CurrentState.IsSet())
	{
		if (NewStateId != CurrentState->Id)
		{
			const FState* NewState = bFailedIfNotFound ? StateMap.Find(NewStateId) : &StateMap.FindOrAdd(NewStateId);
			if (NewState)
			{
				if (CurrentState->Exit)
				{
					(Class->*CurrentState->Exit)(NewStateId);
				}

				const FId OldStateId = CurrentState->Id;

				CurrentState = {*NewState, NewStateId, 0.0f};

				if (CurrentState->Enter)
				{
					(Class->*CurrentState->Enter)(OldStateId);
				}

				return true;
			}
		}
	}
	else
	{
		const FState* NewState = bFailedIfNotFound ? StateMap.Find(NewStateId) : &StateMap.FindOrAdd(NewStateId);
		if (NewState)
		{
			CurrentState = {*NewState, NewStateId, 0.0f};

			if (CurrentState->Enter)
			{
				const FId OldStateId = NewStateId;
				(Class->*CurrentState->Enter)(OldStateId);
			}

			return true;
		}
	}

	return false;
}

template<class FClass, class FId>
void TBTSimpleClassStateMachine<FClass, FId>::SetState(const FId StateId, FEnterFunc EnterFunc, FProcFunc ProcFunc, FExitFunc ExitFunc)
{
	check(!CurrentState.IsSet());
	FState& State = StateMap.FindOrAdd(StateId);
	State.Enter = EnterFunc;
	State.Proc = ProcFunc;
	State.Exit = ExitFunc;
}

template<class FClass, class FId>
void TBTSimpleClassStateMachine<FClass, FId>::SetStateEnterFunc(const FId StateId, FEnterFunc EnterFunc)
{
	check(!CurrentState.IsSet());
	FState& State = StateMap.FindOrAdd(StateId);
	State.Enter = EnterFunc;
}

template<class FClass, class FId>
void TBTSimpleClassStateMachine<FClass, FId>::SetStateProcFunc(const FId StateId, FProcFunc ProcFunc)
{
	check(!CurrentState.IsSet());
	FState& State = StateMap.FindOrAdd(StateId);
	State.Proc = ProcFunc;
}

template<class FClass, class FId>
void TBTSimpleClassStateMachine<FClass, FId>::SetStateExitFunc(const FId StateId, FExitFunc ExitFunc)
{
	check(!CurrentState.IsSet());
	FState& State = StateMap.FindOrAdd(StateId);
	State.Exit = ExitFunc;
}

template<class FClass, class FId>
void TBTSimpleClassStateMachine<FClass, FId>::Deactivate(const bool bCallExitFunc)
{
	if (CurrentState.IsSet())
	{
		if (bCallExitFunc)
		{
			if (CurrentState->Exit)
			{
				(Class->*CurrentState->Exit)(CurrentState->Id);
			}
		}

		CurrentState.Reset();
	}
}
