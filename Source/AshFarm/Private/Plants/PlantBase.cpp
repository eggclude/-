#include "Plants/PlantBase.h"

#include <rapidjson/internal/meta.h>

#include "AshFarm.h"
//析构函数
/*UPlantBase::~UPlantBase() = default;*/

//初始化植物数据表变量
TObjectPtr<UDataTable> UPlantBase::PlantDataTable = nullptr;// 植物配置表初始化
//PostInitProperties 当构造函数完成后调用，用于初始化属性
void UPlantBase::PostInitProperties()
{
	//调用父类的PostInitProperties方法
	Super::PostInitProperties();
	//加载植物配置数据表
	LoadPlantDataTable();
	//从植物配置表加载到植物属性中
	LoadFromPlantDataTable();
}
//
void UPlantBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	//调用父类的PostEditChangeProperty方法 因为是他继承的UObject
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	// Name_None 为fname的空值
	//当RowName不为空值时，从植物配置表加载到植物属性中
	if (RowName!=NAME_None)
	{
		//加载植物配置数据表
		LoadPlantDataTable();
		//从植物配置表加载到植物属性中
		LoadFromPlantDataTable();
	}
}


//加载植物配置表LoadPlantDataTable
void UPlantBase::LoadPlantDataTable()
{
	//加载植物配置表
	UPlantBase::PlantDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/0_/Plants/DT_Plants.DT_Plants"));
	//如果加载失败，输出错误信息
	if (!UPlantBase::PlantDataTable)
	{
		UE_LOG(A_LogAshFarm,Error,TEXT("加载植物配置表失败"));
	}
	else
	{
		UE_LOG(A_LogAshFarm,Warning,TEXT("加载植物配置表成功"));
	}
}
//通过RowName从植物配置表加载到植物属性中
bool UPlantBase::LoadFromPlantDataTable()
{
	if (!UPlantBase::PlantDataTable)
	{
		UE_LOG(A_LogAshFarm,Warning,TEXT("植物配置数据表未加载"));
		return false;
	}
	
//根据RowName查找植物配置行 FPlantConfig*指针是为了找到数据表中间某一行的内存地址  FPlantConfig*是指针 *加到前面是把指针变回数据
	FPlantConfig* FoundRow = UPlantBase::PlantDataTable->FindRow<FPlantConfig>(RowName,TEXT(""));
	//重要*
	
	
	
	if (!FoundRow)
	{
		UE_LOG(A_LogAshFarm,Warning,TEXT("未找到植物配置行"));
		return false;
	}
	UE_LOG(A_LogAshFarm, Warning, TEXT("表加载前GrowthSpeed=%.2f"), PlantConfig.GrowthSpeed);//输出加载前的生长速度
	//将找到的植物配置行赋值给植物属性
	PlantConfig = *FoundRow;
	UE_LOG(A_LogAshFarm, Warning, TEXT("表加载后GrowthSpeed=%.2f"), PlantConfig.GrowthSpeed);//输出加载后的生长速度
	UE_LOG(A_LogAshFarm,Warning,TEXT("植物配置表:加载成功,已找到行命名:%s,生长速度：%.2f"),*RowName.ToString(),PlantConfig.GrowthSpeed);
	return true;
}

//获取所有植物名称(提供给编辑器下拉菜单使用)

TArray<FName> UPlantBase::GetPlantNames() const //获取所有植物名称
{
	TArray<FName> PlantNames;
	if (UPlantBase::PlantDataTable)//如果植物配置表已加载
	{
		PlantNames =  UPlantBase::PlantDataTable->GetRowNames();//获取所有植物名称
	}
	return PlantNames;
}



//TObjectPtr 植物配置表指针 
//设置生长阶段
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
	TotalGrowthTime += DeltaTime;
	//综合评估倍率 
	float EvaluatedMulti = 
			EvaluateSoilType(Context.SoilType)				//评估土壤类型
		*	EvaluateSoilQuality(Context.SoilQuality) 		//评估土壤品质
		*	EvaluateFertility(Context.Fertility) 			//评估土壤肥力
		*	EvaluateMoisture(Context.Moisture) 				//评估土壤湿度
		*	EvaluateTemperature(Context.Temperature) 		//评估环境温度
		*	EvaluateRadiation(Context.RadiationLevel) 		//评估辐射等级
		*	EvaluateToxicity(Context.Toxicity) 			//评估毒性等级
		*	EvaluateWindSpeed(Context.WindSpeed) 			//评估风速等级
		*	EvaluateLightIntensity(Context.LightIntensity);	//评估光照强度

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
		return PlantConfig.SeedlingMesh.LoadSynchronous();//生长进度小于等于阈值，返回种苗子网格体 LoadSynchronous同步加载
		//小于花期长期返回增长网格体
	case EGrowthStage::Growing:
		return PlantConfig.GrowthMesh.LoadSynchronous();//生长进度小于花期长期阈值，返回增长网格体
		//小于成熟期返回开花网格体
	case EGrowthStage::Flowering:
		return PlantConfig.FlowerMesh.LoadSynchronous();//生长进度小于成熟期阈值，返回开花网格体
		//大于等于成熟阈值返回成熟网格体
	case EGrowthStage::Mature:
		return PlantConfig.MatureMesh.LoadSynchronous();//生长进度大于等于成熟阈值，返回成熟网格体 LoadSynchronous同步加载
	
	default:
		return nullptr;
	}
}
	
void UPlantBase::OnMature()
{
	UE_LOG(A_LogAshFarm, Warning, TEXT("植物名称：%s，植物成熟"), *GetPlantName());
}


//计算收货产量
float UPlantBase::CalculateHarvest() const
{
	if (GrowthStage == EGrowthStage::Dead)
	{
		return 0.0f;//死亡不收获
	}
	//计算品质倍率
	float QualityMulti = 1.0f;
	switch (CurrentQuality)
	{
	case EPlantQuality::Premium:
		QualityMulti = PlantDefaults::EPlant_Quality_PREMIUM;//优质品质倍率
		break;
	case EPlantQuality::Normal:
		QualityMulti = PlantDefaults::EPlant_Quality_NORMAL;//普通品质倍率
		break;
	case EPlantQuality::Withered:
		QualityMulti = PlantDefaults::EPlant_Quality_WITHERED;//干扁品质倍率
		break;
	default:
		break;
	}
	return PlantConfig.BaseHarvestAmount*QualityMulti ;//BaseHarvestAmount 基础产量 * 品质倍率
	
}

//记录植物从种植到成熟的总时长
//每次 Grow 累加 DeltaTime 到TotalGrowthTime
float UPlantBase::GetTotalGrowthTime() const
{
	if (CurrentQuality == EPlantQuality::Premium)//优质品质
	{
		return TotalGrowthTime;
	}
}

