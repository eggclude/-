#include "Plants/PlantBase.h"
#include "AshFarm.h"

//析构函数
/*UPlantBase::~UPlantBase() = default;*/


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
	if (Moisture <= PlantDefaults::GROWTH_PROGRESS_THRES || bIsMature)
	{
		return;
	}
	

	//按时间和生长速度倍数推进进度
	GrowthProgress += GrowthSpeed * GrowthSpeedMulti * DeltaTime;
	GrowthProgress = FMath::Clamp(GrowthProgress,0.0f,MatureProgress);//确保生长进度在0到成熟值之间
	//检查是否成熟
	if (GrowthProgress >= MatureProgress)
	{
		GrowthProgress = MatureProgress;  //卡在成熟值上，别让进度条溢出
		bIsMature = true;
		OnMature();
	}
}

//根据生长进度更新网格体
UStaticMesh* UPlantBase::GetStageMesh() const
{
	float GrowthProgressRation = GrowthProgress/MatureProgress;//GrowthProgressRation=成长进度除以成熟值 
	//小于生长期返回种苗子网格体
	if (GrowthProgressRation <= PlantDefaults::GROWTH_PROGRESS_THRES)
	{
		return SeedlingMesh;//生长进度小于等于阈值，返回种苗子网格体
	}
	//小于花期长期返回增长网格体
	else if (GrowthProgressRation < PlantDefaults::FERTILITY_PROGRESS_THRES)
	{
		return GrowthMesh;//生长进度小于花期长期阈值，返回增长网格体
	}
	//小于成熟期返回开花网格体
	else if (GrowthProgressRation < 1.0F)
	{
		return FlowerMesh;//生长进度小于成熟期阈值，返回开花网格体
	}
	//大于等于成熟阈值返回成熟网格体
	else
	{
		return MatureMesh;//生长进度大于等于成熟阈值，返回成熟网格体
	}
}


//当成熟时调用
void UPlantBase::OnMature()
{
	UE_LOG(A_LogAshFarm, Warning, TEXT("植物名称：%s，植物成熟"), *GetPlantName());
}
