// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Plants/PlantBase.h"
#include "AshCorn.generated.h"

/**
 * 
 * corn 玉米类 可实例化
 * BlueprintType 可在蓝图中使用
 * Blueprintable 可在蓝图中实例化
 */
UCLASS(BlueprintType,Blueprintable, meta=(DisplayName="灰烬玉米基类"))
class ASHFARM_API UAshCorn : public UPlantBase
{
	GENERATED_BODY()

	
	
public:
	//构造函数
	UAshCorn();
	
	//生长 override重写
	virtual void Grow(float DeltaTime, EsoilQuality SoilQuality,ESoilType SoilType,float Fertility, float Moisture, float Temperature, int32 RadiationLevel, float ToxicityLevel) override;
	//成熟 override重写
	virtual void OnMature() override;
	//重写植物描述
	virtual FString GetDescription() const override;
	//重写评估辐射等级
	virtual float EvaluateRadiation(int32 RadiationLevel) const override;
	//重写土壤湿度等级
	virtual float EvaluateMoisture(float Moisture) const override;
	//重写环境温度等级
	virtual float EvaluateTemperature(float Temperature) const override;
	//重写土壤肥力等级
	virtual float EvaluateFertility(float Fertility) const override;
};
