// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Plants/PlantBase.h"
#include "UAshPotato.generated.h"

/**
 * 
 */
UCLASS()
class ASHFARM_API UAshPotato : public UPlantBase
{
	GENERATED_BODY()

public:
	//先敲构造函数

	UAshPotato();//默认构造函数

	//重写植物描述
	virtual FString GetDescription() const override;
	//重写肥力等级
	virtual float EvaluateFertility(float Fertility) const override;
	//重写湿度等级
	virtual float EvaluateMoisture(float Moisture) const override;
	//成熟命名重写
	virtual void OnMature() override;
};
