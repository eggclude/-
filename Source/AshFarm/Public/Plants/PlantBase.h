// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/StaticMesh.h"
#include "PlantBase.generated.h"

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
UCLASS(Abstract, BlueprintType, Blueprintable,meta=(DisplayName ="植物基类"))
class ASHFARM_API UPlantBase : public UObject
{
	GENERATED_BODY()
	
	public:
	//构造函数
	UPlantBase() = default;
	//析构函数
	~UPlantBase() = default;
	
#pragma region 植物属性
	
	//植物名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="植物", meta=(DisplayName="植物名称"))
	FString PlantName;
	
	//生长速度:每秒推进的成熟进度比例（归一化）
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="植物|生长数据",meta=(Displayname="生长速度"))
	float GrowthSpeed= 1.0F;
	
	//每秒耗水量: 每秒消耗的湿度占最大湿度的比例（归一化）
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="植物|生长数据",meta=(Displayname="每秒耗水量"))
	float WaterConsumption=0.01F;
	
	//每秒肥料消耗
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="植物|生长数据",meta=(Displayname="每秒肥料消耗"))
	float FertilityConsumption=0.01f;
	
	//生长进度
	UPROPERTY(EditAnywhere,Category="植物|生长数据",meta=(DisplayName = "生长进度"))
	float GrowthProgress =0.0f;
	
	//成熟所需进度
	UPROPERTY(EditAnywhere,Category="植物|生长数据",meta=(DisplayName = "成熟所需进度"))
	float MatureProgress =100.0f;
	
	//是否成熟
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="植物|生长数据",meta=(DisplayName = "是否成熟"))
	bool bIsMature = false;
	
#pragma endregion 植物属性
	
#pragma region 植物外观
	 
	//幼苗期网格体：植物刚种下时显示的模型
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="植物|外观",meta=(DisplayName = "幼苗期网格体"))
	TObjectPtr<UStaticMesh> SeedlingMesh;
	
	//成熟期网格体：bIsMature 变成 true 之后换上的模型
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="植物|外观",meta=(DisplayName = "成熟期网格体"))
	TObjectPtr<UStaticMesh> MatureMesh;
#pragma endregion		
	
	
#pragma region 植物函数功能
	
	//获取植物名称
	UFUNCTION(BlueprintCallable,Category="植物",meta=(DisplayName="获取植物名称"))
	virtual FString GetPlantName() const { return PlantName; }; 
	
	//获取植物描述
	UFUNCTION(BlueprintCallable,Category="植物",meta=(DisplayName="获取植物描述"))
	virtual FString GetDescription() const ; 
	
	/**
	 * 生长: 由种植床在 Tick 中调用, 传入当前土壤环境, 推进生长进度
	 * @param DeltaTime			距离上一帧的时间 (秒)
	 * @param Fertility			当前土壤肥力
	 * @param Moisture			当前土壤湿度 (归一化 0~1)
	 * @param Temperature		当前土壤温度
	 * @param RadiationLevel	当前辐射等级
	 * @param ToxicityLevel		当前毒性等级
	 */
	UFUNCTION(BlueprintCallable,Category="植物",meta=(DisplayName="生长"))
	virtual void Grow(  
		UPARAM(DisplayName="帧间隔时间") float DeltaTime,
		UPARAM(DisplayName="土壤肥力") float Fertility,
		UPARAM(DisplayName="土壤湿度") float Moisture,
		UPARAM(DisplayName="环境温度") float Temperature,
		UPARAM(DisplayName="辐射等级") int32 RadiationLevel,
		UPARAM(DisplayName="毒性等级") float ToxicityLevel);
	
	//当成熟时调用
	UFUNCTION(BlueprintCallable,Category="植物",meta=(DisplayName="当成熟时"))
	virtual void OnMature();

	
#pragma endregion	
	
};
