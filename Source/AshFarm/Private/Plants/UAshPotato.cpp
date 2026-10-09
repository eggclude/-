// Fill out your copyright notice in the Description page of Project Settings.


#include "Plants/UAshPotato.h"


UAshPotato::UAshPotato()
{
	//初始化属性
	PlantConfig.PlantName = TEXT("灰烬土豆");
	PlantConfig.WaterConsumption = 0.025f;
	PlantConfig.GrowthSpeed = 0.8f;
}

//评估肥力等级
float UAshPotato::EvaluateFertility(float Fertility) const
{
	//课件要求：肥力 > 50 正常生长；肥力 > 20 停止生长；其余(20~50) 按 0.8 倍
	if (Fertility >= 50.0f)
	{
		return 1.0f;
	}
	else if (Fertility >= 20.0f)
	{
		return 0.8f; 
	}
	else
	{
		return 0.0f;
	}
}
//评估湿度等级
float UAshPotato::EvaluateMoisture(float Moisture) const
{
	//课件要求：湿度 > 0.8 时生长速度减半；其余情况正常
	if (Moisture > 0.8f)
	{
		return 0.5f;
	}
	else
	{
		return 1.0f;	//兜底：其余情况正常生长
	}
}
//成熟
void UAshPotato::OnMature()
{
	Super::OnMature(); 
	UE_LOG(LogTemp, Warning, TEXT("土豆鼓起来一个包"));
}
