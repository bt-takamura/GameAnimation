// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Curves/CurveLinearColor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstanceController.h"

#include "Materials/BTStdMaterialTypes.h"

struct FBTStdMaterialParamChangerBase;

class FBTStdMaterialParamChangerHandle
{
public:
	static const FBTStdMaterialParamChangerHandle NullHandle;
public:
	BTSTD_API bool IsValid() const;
	BTSTD_API void Reset();

	BTSTD_API FBTStdMaterialParamChangerBase* Get() const;
	BTSTD_API FBTStdMaterialParamChangerBase* operator->() const;

	explicit operator bool() const
	{
		return IsValid();
	}
	bool operator!() const noexcept
	{
		return !static_cast<bool>(*this);
	}

	explicit FBTStdMaterialParamChangerHandle()
	{
		m_UniqueId = 0;
		m_pChanger = nullptr;
	}

protected:
	explicit FBTStdMaterialParamChangerHandle(FBTStdMaterialParamChangerBase* InChanger, uint64 InId);

private:
	uint64 m_UniqueId;
	FBTStdMaterialParamChangerBase* m_pChanger;

	friend class FBTStdMaterialManager;
};

struct FBTStdMaterialParamChangerBase
{
	virtual ~FBTStdMaterialParamChangerBase() {}
	virtual void Restore() = 0;
	virtual bool IsRestored() const = 0;
	virtual bool Update(float DeltaTime) = 0;
	virtual void Abort() = 0;
	virtual bool IsAborted() const = 0;

	void SetActorCustomTimeDilationEnabled(bool bEnable)
	{
		m_UseActorCustomTimeDilation = bEnable;
	}

protected:
	bool m_UseActorCustomTimeDilation = true;

protected:
	static float ToProgressTime(UCurveBase* InCurve, float InProgressRate)
	{
		if (!InCurve)
		{
			return 0.0f;
		}

		float MinTime = 0.0f, MaxTime = 0.0f;
		InCurve->GetTimeRange(MinTime, MaxTime);

		return FMath::Lerp(MinTime, MaxTime, InProgressRate);
	}

	static void GetParam(float& OutParamValue, UMaterialInstanceDynamic* InTarget, const FName& InParameterName)
	{
		FMaterialParameterInfo ParameterInfo(InParameterName);
		InTarget->GetScalarParameterValue(ParameterInfo, OutParamValue);
	}

	static void GetParam(FLinearColor& OutParamValue, UMaterialInstanceDynamic* InTarget, const FName& InParameterName)
	{
		FMaterialParameterInfo ParameterInfo(InParameterName);
		InTarget->GetVectorParameterValue(ParameterInfo, OutParamValue);
	}

	static void GetParameter(UTexture*& OutParameter, UMaterialInstanceDynamic* InTarget, const FName& InParameterName)
	{
		FMaterialParameterInfo ParameterInfo(InParameterName);
		InTarget->GetTextureParameterValue(ParameterInfo, OutParameter);
	}

	static void GetParameter(float& OutParameter, UMaterialParameterCollectionInstance* InTarget, const FName& InParameterName)
	{
		InTarget->GetScalarParameterValue(InParameterName, OutParameter);
	}

	static void GetParameter(FLinearColor& OutParameter, UMaterialParameterCollectionInstance* InTarget, const FName& InParameterName)
	{
		InTarget->GetVectorParameterValue(InParameterName, OutParameter);
	}

	static void GetParam(float& OutParamValue, UNiagaraComponent* InTarget, const FName& InParameterName)
	{
		FNiagaraSystemInstanceControllerConstPtr controller = InTarget->GetSystemInstanceController();
		const UNiagaraSystem* System = controller ? controller->GetSystem() : nullptr;
		if (System != nullptr)
		{
			TArray<FNiagaraVariable> Variables;
			System->GetExposedParameters().GetParameters(Variables);
			for (FNiagaraVariable& Variable : Variables)
			{
				if (Variable.GetName() == InParameterName)
				{
					OutParamValue = System->GetExposedParameters().GetParameterValue<float>(Variable);
					return;
				}
			}
		}
		OutParamValue = 0.f;
	}

	static void GetParam(FLinearColor& OutParamValue, UNiagaraComponent* InTarget, const FName& InParameterName)
	{
		FNiagaraSystemInstanceControllerConstPtr controller = InTarget->GetSystemInstanceController();
		const UNiagaraSystem* System = controller ? controller->GetSystem() : nullptr;
		if (System != nullptr)
		{
			TArray<FNiagaraVariable> Variables;
			System->GetExposedParameters().GetParameters(Variables);
			for (FNiagaraVariable& Variable : Variables)
			{
				if (Variable.GetName() == InParameterName)
				{
					OutParamValue = System->GetExposedParameters().GetParameterValue<FLinearColor>(Variable);
					return;
				}
			}
		}
		OutParamValue = FLinearColor::Black;
	}
protected:
	static float CalcDuration(float InDuration, float InSource)
	{
		return InDuration;
	}

	static float CalcDuration(float InDuration, const FLinearColor& InSource)
	{
		return InDuration;
	}

	static float CalcDuration(float InDuration, UCurveBase* InSource)
	{
		if (InDuration > 0.0f)
		{
			return InDuration;
		}

		if (!InSource)
		{
			return InDuration;
		}

		float MinTime = 0.0f, MaxTime = 0.0f;
		InSource->GetTimeRange(MinTime, MaxTime);
		const float CurveDuration = MaxTime - MinTime;
		return CurveDuration;
	}

	static float CalcDuration(float InDuration, UTexture* InSource)
	{
		return InDuration;
	}
};

// TODO:FBTStdMaterialParamChangerBaseに吸収してもよいかも
struct FBTStdMaterialParamChanger : public FBTStdMaterialParamChangerBase
{
protected:
public:
	virtual ~FBTStdMaterialParamChanger()
	{
	}

	virtual void Restore() override
	{
		m_IsRestore = true;
	}

	virtual bool IsRestored() const override
	{
		return m_IsRestore;
	}

	virtual void Abort() override
	{
		m_IsAbort = true;
	}

	virtual bool IsAborted() const override
	{
		return m_IsAbort;
	}
protected:
	explicit FBTStdMaterialParamChanger(UWorld* InWorld, const FName& InParamName, float InDuration, const FBTStdMaterialRGBAMask& InRGBAMask)
		: m_World(InWorld)
		, m_IsRestore(false)
		, m_ParamName(InParamName)
		, m_Duration(InDuration)
		, m_ElapsedTime(0.0f)
		, m_ProgressRate(0.0f)
		, m_IsAbort(false)
		, m_rgbaMask(InRGBAMask)
	{
	}

	bool AdvanceTime(float DeltaTime)
	{
		m_ElapsedTime = FMath::Min(m_ElapsedTime + DeltaTime, m_Duration);
		m_ProgressRate = (m_Duration > 0.0f) ? (m_ElapsedTime / m_Duration) : 1.0f;
		return m_ProgressRate < 1.0f;
	}

protected:
	const TWeakObjectPtr<UWorld> m_World;
	bool m_IsRestore;

	const FName m_ParamName;
	float m_Duration;
	float m_ElapsedTime;
	float m_ProgressRate;
	bool m_IsAbort;
	FBTStdMaterialRGBAMask m_rgbaMask;
};