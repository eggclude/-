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
UCLASS(BlueprintType,Blueprintable, meta=(DisplayName="灰烬玉米"))
class ASHFARM_API UAshCorn : public UPlantBase
{
	GENERATED_BODY()

public:
	//构造函数
	UAshCorn();
	
	
	
	
	//生长 override重写
	virtual void Grow(float DeltaTime, float Fertility, float Moisture, float Temperature, int32 RadiationLevel, float ToxicityLevel) override;
	//成熟 override重写
	virtual void OnMature() override;
	//重写植物描述
	virtual FString GetDescription() const override ;
		

};
