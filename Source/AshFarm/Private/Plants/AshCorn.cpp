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
	return TEXT("最基础的作物，哪里都可以种"); 
}
//生长
void UAshCorn::Grow(
	  float DeltaTime,
	  float Fertility,
	  float Moisture,
	  float Temperature,
	  int32 RadiationLevel,
	  float ToxicityLevel)
{
	//玉米喜欢湿度50%以上（生长速度翻倍），湿度低于20%则减半
	float MoistureMultiplier = 1.0f;
	if (Moisture > 0.5f)
	{
		MoistureMultiplier = 2.0f;
	}
	else if (Moisture < 0.2f)
	{
		MoistureMultiplier = 0.5f;
	}

	//按湿度倍率临时调整速度，再交给基类推进生长进度
	const float OriginalSpeed = GrowthSpeed;
	GrowthSpeed = OriginalSpeed * MoistureMultiplier;
	Super::Grow(DeltaTime, Fertility, Moisture, Temperature, RadiationLevel, ToxicityLevel);
	GrowthSpeed = OriginalSpeed;
}

//成熟
void UAshCorn::OnMature()
{
	UE_LOG(LogTemp, Warning, TEXT("灰烬玉米成熟可采集"));
}
