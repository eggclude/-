#include "Plants/PlantBase.h"

#include <rapidjson/internal/meta.h>

#include "AshFarm.h"
//析构函数
/*UPlantBase::~UPlantBase() = default;*/


void UPlantBase::SetGrowthStage()
{
	float GrowthProgressRation = GrowthProgress / PlantConfig.MatureProgress;
	//小于生长期返回种苗子网格体
	if (GrowthProgressRation <= PlantDefaults::GROWTH_PROGRESS_THRES)
	{
		GrowthStage = EGrowthStage::Seeding;//生长进度小于等于阈值，返回种苗子网格体
	}
	//小于花期长期返回花期网格体
	else if (GrowthProgressRation < PlantDefaults::FERTILITY_PROGRESS_THRES)
	{
		GrowthStage = EGrowthStage::Growing;//生长进度小于花期长期阈值，返回花期网格体
	}
	//小于成熟期返回开花网格体
	else if (GrowthProgressRation < 1.0F)
	{
		GrowthStage = EGrowthStage::Flowering;//生长进度小于成熟期阈值，返回开花网格体
	}
	//大于等于成熟阈值返回成熟网格体
	else
	{
		GrowthStage = EGrowthStage::Mature;//生长进度大于等于成熟阈值，返回成熟网格体
	}
}

//获取生长阶段文本
FString UPlantBase::GetGrowthStageText() const
{
	switch (GrowthStage)
	{
		case EGrowthStage::Seeding:
		return TEXT("幼苗期");
		case EGrowthStage::Growing:
		return TEXT("生长期");
		case EGrowthStage::Flowering:
		return TEXT("开花期");
		case EGrowthStage::Mature:
		return TEXT("成熟期");
		default:
		return TEXT("未知阶段");
	}
}

//生长
void UPlantBase::Grow(float DeltaTime,const FPlantGrowthContext& Context)
{
	//没有水或者已成熟就不生长
	if (Context.Moisture <= 0.0F || bIsMature)
	{
		return;
	}
	//综合评估倍率 
	float EvaluatedMulti = EvaluateSoilQuality(Context.SoilQuality)
	*	EvaluateSoilType(Context.SoilType)				//评估土壤类型
	*	EvaluateSoilQuality(Context.SoilQuality) 		//评估土壤品质
	*	EvaluateFertility(Context.Fertility) 			//评估土壤肥力
	*	EvaluateMoisture(Context.Moisture) 				//评估土壤湿度
	*	EvaluateTemperature(Context.Temperature) 		//评估环境温度
	*	EvaluateRadiation(Context.RadiationLevel) 		//评估辐射等级
	*	EvaluateToxicity(Context.Toxicity) ;		//评估毒性等级

	//逆境累计
if (EvaluatedMulti < 1.0f)
{
	Stress += PlantConfig.Sensitivity *(1.0f - EvaluatedMulti) * DeltaTime;//逆境值 = 敏感度 *（1.0f基础值-综合评估倍率）*时间
	Stress = FMath::Clamp(Stress,0.0f,100.0f);//确保逆境值在0到100之间
	SetPlantQuality();//设置品质
}
	
	//按时间和生长速度倍数推进进度
	GrowthProgress += PlantConfig.GrowthSpeed * EvaluatedMulti * DeltaTime;
	GrowthProgress = FMath::Clamp(GrowthProgress,0.0f,PlantConfig.MatureProgress);//确保生长进度在0到成熟值之间
	//检查是否成熟
	if (GrowthProgress >= PlantConfig.MatureProgress)
	{
		GrowthProgress = PlantConfig.MatureProgress;  //卡在成熟值上，别让进度条溢出
		bIsMature = true;
		OnMature();
	}
}
//设置品质
void UPlantBase::SetPlantQuality()
{
	;
	if (Stress < PlantDefaults::QUALITY_PREMIUM_THRESHOLD)
	{
		CurrentQuality = EPlantQuality::Premium;//优质
	}
	else if (Stress < PlantDefaults::QUALITY_NORMAL_THRESHOLD)
	{
		CurrentQuality = EPlantQuality::Normal;//普通
	}
	else
	{
		CurrentQuality = EPlantQuality::Withered;//干扁
	}
}
FString UPlantBase::GetQualityText() const
{
	switch (CurrentQuality)
	{
		case EPlantQuality::Withered:
		return TEXT("干瘪");
		case EPlantQuality::Normal:
		return TEXT("普通");
		case EPlantQuality::Premium:
		return TEXT("优质");
	default:
		return TEXT("未知品质");
	}
}

//根据生长进度更新网格体
UStaticMesh* UPlantBase::GetStageMesh() const
{
	switch (GrowthStage)
	{
	case EGrowthStage::Seeding:
		return PlantConfig.SeedlingMesh.Get();//生长进度小于等于阈值，返回种苗子网格体
		//小于花期长期返回增长网格体
	case EGrowthStage::Growing:
		return PlantConfig.GrowthMesh.Get();//生长进度小于花期长期阈值，返回增长网格体
		//小于成熟期返回开花网格体
	case EGrowthStage::Flowering:
		return PlantConfig.FlowerMesh.Get();//生长进度小于成熟期阈值，返回开花网格体
		//大于等于成熟阈值返回成熟网格体
	default:
		return nullptr;//生长进度大于等于成熟阈值，返回成熟网格体
	}
}
void UPlantBase::OnMature()
{
	UE_LOG(A_LogAshFarm, Warning, TEXT("植物名称：%s，植物成熟"), *GetPlantName());
}
