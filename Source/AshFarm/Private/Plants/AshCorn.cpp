// Fill out your copyright notice in the Description page of Project Settings.


#include "Plants/AshCorn.h"

#include "AshFarm.h"
//构造函数
UAshCorn::UAshCorn()
{
	PlantName = TEXT("灰烬玉米");
	GrowthSpeed = 2.0f;
	//水损耗0.5f
	WaterConsumption = 0.02f;
	FertilityConsumption = 0.02f;
	MatureProgress = 80.0f;
}


//重写植物描述
FString UAshCorn::GetDescription() const { 
	return TEXT("灰烬玉米，最基础的作物，哪里都可以种"); 
}
//生长
void UAshCorn::Grow(
	  float DeltaTime,
	  EsoilQuality SoilQuality,
	  float Fertility,
	  float Moisture,
	  float Temperature,
	  int32 RadiationLevel,
	  float ToxicityLevel)
{
	//调用基类的生长函数
	Super::Grow(DeltaTime, SoilQuality, Fertility, Moisture, Temperature, RadiationLevel, ToxicityLevel);
}

//成熟
void UAshCorn::OnMature()
{
	UE_LOG(LogTemp, Warning, TEXT("灰烬玉米成熟可采集"));
}

//评估辐射等级
float UAshCorn::EvaluateRadiation(int32 RadiationLevel) const
{
	//无辐射时,玉米非常喜欢
	if (RadiationLevel  > 0)
	{
		return 0.0f;
	}
	return 1.0f;
}
//评估湿度等级
float UAshCorn::EvaluateMoisture(float Moisture) const
{
	//湿度为0.9时,玉米生长速度减半
	if (Moisture > 0.9f)
	{
		return 0.5f;
	}
	return 1.0f;
}
//评估环境温度等级s
float UAshCorn::EvaluateTemperature(float Temperature) const
{
	//温度为5时,玉米停止生长
	if (Temperature < 5.0f)
	{
		return 0.0f;
	}
	return 1.0f;
}
float UAshCorn::EvaluateFertility(float Fertility) const
{
	//肥力为30时,玉米速度为0.8倍
	if (Fertility < 30.0f)
	{
		return 0.8f;
	}
	return 1.0f;
}
//评估土壤肥力等级
