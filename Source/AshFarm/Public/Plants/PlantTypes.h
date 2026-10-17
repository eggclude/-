#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"          
#include "UObject/SoftObjectPtr.h"    
#include "Actors/PlantBedTypes.h" 
#include "PlantTypes.generated.h"
namespace PlantDefaults
{
	//植物品质阈值
	const float QUALITY_PREMIUM_THRESHOLD = 5.0f;	//优质阈值
	const float QUALITY_NORMAL_THRESHOLD = 15.0f;	//普通阈值
	const float QUALITY_WITHERED_THRESHOLD = 30.0f;	//干瘪阈值
	
	const float EPlant_Quality_PREMIUM = 1.2f;	//优质倍率
	const float EPlant_Quality_NORMAL = 1.0f;	//普通倍率
	const float EPlant_Quality_WITHERED = 0.5f; //干扁倍率
}
//选择配置参数




USTRUCT(BlueprintType)//结构体
struct FPlantGrowthContext//FPlantGrowth植物生长环境 上下文(context)/参数(Params)
	{
	GENERATED_BODY()//自动生成构造函数和析构函数
	
	
	//构造函数
	FPlantGrowthContext():
	SoilQuality(EsoilQuality::Normal),
	SoilType(ESoilType::Loam),
	Fertility(50.0f),
	Moisture(0.5f),
	Temperature(25.0f),
	RadiationLevel(0),
	Toxicity(0.0f),
	WindSpeed(0.0f),
	LightIntensity(0.0f)
	{
		
	}
	//土壤数据
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="植物|生长环境",meta=(DisplayName="土壤品质"))
	EsoilQuality SoilQuality=EsoilQuality::Saline;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="植物|生长环境",meta=(DisplayName="土壤类型"))
	ESoilType SoilType=ESoilType::Loam;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="植物|生长环境",meta=(DisplayName="土壤肥力"))
	float Fertility = 50.0f;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="植物|生长环境",meta=(DisplayName="土壤湿度"))
	float Moisture = 0.5f;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="植物|生长环境",meta=(DisplayName="环境温度"))
	float Temperature = 25.0f;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="植物|生长环境",meta=(DisplayName="环境辐射等级"))
	int32 RadiationLevel = 0;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="植物|生长环境",meta=(DisplayName="环境毒度等级"))
	float Toxicity = 0.0f;
	
	//环境数据
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="植物|生长环境",meta=(DisplayName="环境风速",ClampMin="0",ClampMax="10"))
	float WindSpeed = 0.0f;
		
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="植物|生长环境",meta=(DisplayName="环境光照强度",ClampMin="0",ClampMax="1"))
	float LightIntensity = 0.5f;
};
USTRUCT(BlueprintType,meta=(DisplayName="植物环境值范围"))//结构体 //温度范围 水分范围 肥料范围 光照强度范围 风速范围 辐射等级范围 毒度等级范围 
//植物范围
struct FPlantRange
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="植物|生长环境",meta=(DisplayName="环境最小值"))
	float Min =0;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="植物|生长环境",meta=(DisplayName="环境最优最小值"))
	float OptimalMin = 0.4f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="植物|生长环境",meta=(DisplayName="环境最优最大值"))
	float OptimalMax = 0.6f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="植物|生长环境",meta=(DisplayName="环境最大值"))
	float Max = 1;
	//评估环境值
	//结构体内不能使用UFUNCTION
	float Evaluate(float Value)const
	{
		if (Value < Min || Value > Max)//如果环境值超出范围
		{
			return 0.0F;//完全不生长
		}
		if (Value >= OptimalMin && Value <= OptimalMax)//如果环境值在最优范围内
		{
			return 1.0F;//完全生长
		}
		else if (Value<OptimalMin)//如果环境值小于最优最小值
		{
			//计算环境值value范围
			float Range = OptimalMin - Min;
			if (Range <= 0.0F)//
			{
				return 1.0F;//完全生长
			}
			//上面计算环境值value范围
			//下面计算value在范围的哪个位置
			float Alpha = (Value - Min) / Range;//滑轨值为环境值-最小值/最优最小值-最小值
			
			return FMath::Lerp(0.0F,1.0F,Alpha);// 环境值-最小值/ 最优最小值-最小值
		}
		
		if (Value > OptimalMax)//如果环境值大于最优最大值
		{
			float Range =  Max -OptimalMax;
			if (Range <= 0.0F)//
			{
				return 1.0F;//完全生长
			}
		
		float Alpha = (Value - OptimalMax)/ Range;//滑轨值为环境值-最优最大值/最优最大值-最优最大值
		
		return FMath::Lerp(1.0F,0.0F,Alpha);//A是舒适区最大值 B是环境最大值 Alpha是滑轨值
		}
		
		return 1.0F;//完全生长
	}
};


//植物配置
USTRUCT(Blueprintable,meta=(DisplayName="植物配置"))
struct FPlantConfig: public FTableRowBase//植物配置
{
	GENERATED_BODY()
	/*配置Config*/
	
	//构造函数
	FPlantConfig():
	 PlantName(TEXT("未命名植物")),
	 Description(FText::FromString("未命名植物的描述")),
	 GrowthSpeed(1.0F),
	 WaterConsumption(0.01F),
	 FertilityConsumption(0.01f),
	 MatureProgress(100.0f),
	 Sensitivity(1.0f)
	{
		
	}
	//==========================
	//========收获物配置============
	//==========================
	
	//收获名称
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="植物|产量",meta=(DisplayName="收获名称"))
	FName HarvestName;
	
	//基础产量
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="植物|产量",meta=(DisplayName="基础产量"))
	float BaseHarvestAmount=10.0f;
	
	//收获副产物名称
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="植物|产量",meta=(DisplayName="收获副产物名称"))
	FName ByproductName;
	
	//收获副产物概率
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="植物|产量",meta=(DisplayName="收获副产物概率",ClampMin="0",ClampMax="1"))
	float ByproductChance = 0.0f;
	
	//收获副产物数量
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="植物|产量",meta=(DisplayName="收获副产物数量"))
	float ByproductAmount = 1.0f;
	
	//====================
	//	环境需求
	//====================
	//偏好土壤类型
	UPROPERTY(EditAnywhere,Blueprintable,Category="植物|生长环境",meta=(DisplayName="偏好土壤类型"))
	ESoilType PreferredSoilType = ESoilType::Loam;//
	//土壤肥力范围
	UPROPERTY(EditAnywhere,Blueprintable,Category="植物|生长环境",meta=(DisplayName="肥力范围"))
	FPlantRange FertilityRange = FPlantRange(0.0f,10.0f,90.0f,100.0f);
	
	//土壤湿度范围
	UPROPERTY(EditAnywhere,Blueprintable,Category="植物|生长环境",meta=(DisplayName="湿度范围"))
	FPlantRange MoistureRange = FPlantRange(0.0f,0.3f,1.0f,1.0f);
	
	//环境温度范围
	UPROPERTY(EditAnywhere,Blueprintable,Category="植物|生长环境",meta=(DisplayName="温度范围"))
	FPlantRange TemperatureRange = FPlantRange(0.0f,20.0f,30.0f,40.0f);
	
	//环境辐射等级范围
	UPROPERTY(EditAnywhere,Blueprintable,Category="植物|生长环境",meta=(DisplayName="辐射范围"))
	FPlantRange RadiationRange = FPlantRange(0.0f,0.0f,0.0f,0.0f);
	
	//环境毒度等级范围
	UPROPERTY(EditAnywhere,Blueprintable,Category="植物|生长环境",meta=(DisplayName="毒度等级范围"))
	FPlantRange ToxicityRange = FPlantRange(0.0f,0.0f,0.0f,0.0f);
		
	//环境风速范围
	UPROPERTY(EditAnywhere,Blueprintable,Category="植物|生长环境",meta=(DisplayName="风速范围"))
	FPlantRange WindSpeedRange = FPlantRange(0.0F,0.0F,3.0F,3.0F);
	
	//环境光照强度范围
	UPROPERTY(EditAnywhere,Blueprintable,Category="植物|生长环境",meta=(DisplayName="光照范围"))
	FPlantRange LightIntensityRange = FPlantRange(0.3f,0.8f,1.0F,1.0F);
	

	//----------------------------------
	//-------------植物属性配置-----------
	//=---------------------------------
	
	//植物名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="植物", meta=(DisplayName="植物名称"))
	FString PlantName;
	
	//植物描述
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="植物", meta=(DisplayName="植物描述"))
	FText Description;
	
	//生长速度
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="植物|生长数据",meta=(Displayname="生长速度"))
	float GrowthSpeed= 1.0F;
	
	//每秒耗水量
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="植物|生长数据",meta=(Displayname="每秒耗水量"))
	float WaterConsumption=0.01F;
	
	//每秒肥料消耗
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="植物|生长数据",meta=(Displayname="每秒肥料消耗"))
	float FertilityConsumption=0.01f;
	
	//成熟所需进度
	UPROPERTY(EditAnywhere,Category="植物|生长数据",meta=(DisplayName = "成熟所需进度"))
	float MatureProgress =100.0f;
	
	//敏感度（敏感度会影响逆境值，比如）
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="植物|生长数据",meta=(DisplayName = "敏感度"))
	float Sensitivity = 1.0f;
	
	//最大生长天数（如果超出MaxGrowthDays的时间还没有成熟，植物就会死亡）
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="植物|产量",meta=(DisplayName="最大生长时间(sec)"))
	float MaxGrowthTime = 10.0f;
	
	/*//生长速度倍数 已废弃 因为生长速度倍数在生长数据中已经实现了
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="植物|生长数据",meta=(Displayname="生长速度倍数"))
	float GrowthSpeedMulti = 1.0F;*/
	
	//-----------------------------------植物外观---------------------------------------------------
	
	//幼苗期网格体：植物刚种下时显示的模型
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="植物|外观",meta=(DisplayName = "幼苗期网格体"))
	TSoftObjectPtr<UStaticMesh> SeedlingMesh;//TSoftObjectPtr 是一个软指针，用于延迟加载资源 用于指向项目文件中的某一个资源文件
	
	//成长期网格体：植物在成熟前的网格体
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="植物|外观",meta=(DisplayName = "成长期网格体"))
	TSoftObjectPtr<UStaticMesh> GrowthMesh;
	
	//开花期网格体：植物成熟后显示的模型
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="植物|外观",meta=(DisplayName = "开花期网格体"))
	TSoftObjectPtr<UStaticMesh> FlowerMesh;
	
	//成熟期网格体：bIsMature 变成 true 之后换上的模型
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="植物|外观",meta=(DisplayName = "成熟期网格体"))
	TSoftObjectPtr<UStaticMesh> MatureMesh;
	
	
};

//作物生长阶段{幼苗期, 生长期, 开花期, 成熟期}
UENUM(BlueprintType)
enum class EGrowthStage : uint8
{
	Seeding UMETA(DisplayName="播种期"),
	Growing UMETA(DisplayName="生长期"),
	Flowering UMETA(DisplayName="开花期"),
	Mature UMETA(DisplayName="成熟期"),
	Dead UMETA(DisplayName="死亡期"),
};
//植物品质
UENUM(BlueprintType)
enum class EPlantQuality: uint8 
{
	Premium		UMETA(DisplayName="优质"),
	Normal		UMETA(DisplayName="普通"),
	Withered	UMETA(DisplayName="干瘪"),
};


