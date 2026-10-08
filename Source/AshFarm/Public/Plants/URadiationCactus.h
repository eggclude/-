// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Plants/PlantBase.h"
#include "URadiationCactus.generated.h"

/**
 * 
 */
UCLASS()
class ASHFARM_API URadiationCactus : public UPlantBase
{
	GENERATED_BODY()
	
	
	public:
	//构造函数
	URadiationCactus();
	//重写评估土壤类型
	virtual float EvaluateSoilType(ESoilType SoilType)  const override;
	//重写辐射等级
	virtual float EvaluateRadiation(int32 RadiationLevel) const override;//评估辐射等级
};

