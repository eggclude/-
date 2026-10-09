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
	
}

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
};
//植物品质
UENUM(BlueprintType)
enum class EPlantQuality: uint8 
{
	Premium		UMETA(DisplayName="优质"),
	Normal		UMETA(DisplayName="普通"),
	Withered	UMETA(DisplayName="干瘪"),
};

