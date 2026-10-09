// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/StaticMesh.h"
#include "Actors/PlantBedTypes.h"
#include "PlantTypes.h"
#include "PlantBase.generated.h"
 
namespace PlantDefaults
{
	const float GROWTH_PROGRESS_THRES = 0.3f;		//生长期网格体阈值
	const float FERTILITY_PROGRESS_THRES = 0.6f;	//开花期网格体阈值
	//植物阶段
}

/**
 * 植物基类
 * 
 * UObject：没有坐标、不能放进关卡，生命周期由 GC 管理
 * 
 * Abstract： 是抽象类 不能实例化
 * 
 * BlueprintType： 蓝图类型 可在蓝图中作为蓝图变量
 * 
 * Blueprintable： 允许使用C++类作为蓝图类的父类
 */
UCLASS(Abstract, BlueprintType, Blueprintable,DefaultToInstanced,EditInlineNew,meta=(DisplayName ="植物基类"))
class ASHFARM_API UPlantBase : public UObject
{
	GENERATED_BODY()
	
	public:
	//构造函数
	UPlantBase() = default;
	//析构函数
	~UPlantBase() = default;

	//植物配置表
	static TObjectPtr<UDataTable> PlantDataTable;//植物配置表
	
	
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="植物",meta=(DisplayName = "植物属性配置"))
	FPlantConfig PlantConfig;
	
#pragma region 植物属性 Config
	
	//------------------------------------------------
	// 运行时状态		RunTimeStatus
	//------------------------------------------------
	
	//生长进度
	UPROPERTY(EditAnywhere,Category="植物|生长数据",meta=(DisplayName = "生长进度"))
	float GrowthProgress =0.0f;
	
	//生长阶段 
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="植物|生长数据",meta=(DisplayName = "生长阶段"))
	EGrowthStage GrowthStage = EGrowthStage::Seeding;
	
	//逆境值（生长过程中，只要环境综合倍率(环境适应程度)低于 1.0（作物在受罪），就按差距累积逆境值：Stress += (1.0f - 环境倍率) × DeltaTime）
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="植物|生长数据",meta=(DisplayName = "逆境值"))
	float Stress = 0.0f;
	
	//当前品质
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="植物|生长数据",meta=(DisplayName = "当前品质"))
	EPlantQuality CurrentQuality = EPlantQuality::Premium;
	
	//是否成熟
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="植物|生长数据",meta=(DisplayName = "是否成熟"))
	bool bIsMature = false;
	
#pragma endregion 植物属性

#pragma region 植物函数功能
	void SetPlantQuality(const EPlantQuality Quality);//重设置植物品质
	
	//获取植物名称
	UFUNCTION(BlueprintCallable,Category="植物",meta=(DisplayName="获取植物名称"))
	virtual FString GetPlantName() const { return PlantConfig.PlantName; }; 
	
	//获取植物描述  后续的废弃的函数 原因是已经在PlantBedTypes.h中的plantconfig定义了
	UFUNCTION(BlueprintCallable,Category="植物",meta=(DisplayName="获取植物描述"))
	virtual FText GetDescription() const {return  PlantConfig.Description;}; 
	/* PURE_VIRTUAL(,return TEXT("一种灰烬时代的作物"););*/ 
	//定义的基础类，其他字类可以重写这个函数必须不能和他的text文字一致*/
	 
	//根据生长进度更新网格体
	UFUNCTION(BlueprintCallable,Category="植物",meta=(DisplayName="根据生长进度更新网格体"))
	virtual UStaticMesh* GetStageMesh() const ; //函数的返回值不能用TObjectPtr
	
	//设置生长阶段	
	UFUNCTION(Blueprintable,Category="植物",meta=(DisplayName="设置生长阶段"))
	void SetGrowthStage();
	
	//获取生长阶段
	UFUNCTION(BlueprintCallable,Category="植物",meta=(DisplayName="获取生长阶段"))
	virtual EGrowthStage GetGrowthStage() const { return GrowthStage; };//获取生长阶段
	
	//获取生长阶段文本
	UFUNCTION(Blueprintable,Category="植物",meta=(DisplayName="获取生长阶段文本")) 
	virtual FString GetGrowthStageText() const;//获取生长阶段文本 
	
	#pragma endregion
	
	/**
	 * 生长: 由种植床在 Tick 中调用, 传入当前土壤环境, 推进生长进度
	 * @param DeltaTime			距离上一帧的时间 (秒)
	 * @param Fertility			当前土壤肥力
	 * @param Moisture			当前土壤湿度 (归一化 0~1)
	 * @param Temperature		当前土壤温度
	 * @param RadiationLevel	当前辐射等级
	 * @param ToxicityLevel		当前毒性等级
	 */
	//生长 override虚函数
	UFUNCTION(BlueprintCallable,Category="植物",meta=(DisplayName="生长"))
	virtual void Grow(  
		UPARAM(DisplayName="帧间隔时间") float DeltaTime,
		const FPlantGrowthContext& Context);
	//生长 不加const 的话会直接修改原来的数据&管的是"要不要拷贝"，const 管的是"能不能改" &是为了不拷贝 const 是为了不许改（只读输入）const& = 不拷贝 + 不许改
		
	//设置品质 根据逆境值来计算出品质 不需要参数
	void SetPlantQuality();
	
	//获取品质文本
	UFUNCTION(BlueprintCallable,Category="植物",meta=(DisplayName="获取品质文本"))
	virtual FString GetQualityText() const ;
	
	//当成熟时调用
	UFUNCTION(BlueprintCallable,Category="植物",meta=(DisplayName="当成熟时"))
	virtual void OnMature();
	virtual float EvaluateSoilQuality(EsoilQuality SoilQuality) const { return 1.0f; };//评估土壤品质
	virtual float EvaluateSoilType(ESoilType SoilType) const { return 1.0f; };//评估土壤类型
	virtual float EvaluateFertility(float Fertility) const { return 1.0f; };//评估土壤肥力
	virtual float EvaluateMoisture(float Moisture) const { return 1.0f; };//评估土壤湿度
	virtual float EvaluateTemperature(float Temperature) const { return 1.0f; };//评估环境温度
	virtual float EvaluateRadiation(int32 RadiationLevel) const { return 1.0f; };//评估辐射等级
	virtual float EvaluateToxicity(float ToxicityLevel) const{return 1.0f; };//评估毒性等级
};
	
	


