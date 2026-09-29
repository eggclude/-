#include "Plants/PlantBase.h"
#include "AshFarm.h"

//析构函数
UPlantBase::~UPlantBase() = default;

//获取植物描述
FString UPlantBase::GetDescription() const { 
	return TEXT("一种灰烬时代的作物"); 
}

//生长
void UPlantBase::Grow(
	  float DeltaTime,
	  float Fertility,
	  float Moisture,
	  float Temperature,
	  int32 RadiationLevel,
	  float ToxicityLevel)
{
	//没有水或者已成熟就不生长
	if (Moisture <= 0.0f || bIsMature)
	{
		return;
	}

	//按时间和生长速度推进进度
	GrowthProgress += GrowthSpeed * DeltaTime;

	//检查是否成熟
	if (GrowthProgress >= MatureProgress)
	{
		GrowthProgress = MatureProgress;  //卡在成熟值上，别让进度条溢出
		bIsMature = true;
		OnMature();
	}
}

//当成熟时调用
void UPlantBase::OnMature()
{
	UE_LOG(A_LogAshFarm, Warning, TEXT("植物名称：%s，植物成熟"), *GetPlantName());
}
