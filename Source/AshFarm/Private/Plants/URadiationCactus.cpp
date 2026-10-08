// Fill out your copyright notice in the Description page of Project Settings.


#include "Plants/URadiationCactus.h"


//构造函数
URadiationCactus::URadiationCactus()
{
	PlantName = TEXT("辐射仙人掌");
	WaterConsumption = 0.005f;//耗水
}
//重写评估土壤类型
float URadiationCactus::EvaluateSoilType(ESoilType SoilType) const
{
	if (SoilType == ESoilType::Sand)
	{
		return 1.5f;
	}
	return 1.0f;
}

float URadiationCactus::EvaluateRadiation(int32 RadiationLevel) const
{
	if (RadiationLevel == 1)
	{
		return 1.0f;
	}
	return 0.0f;
}









