// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HandPump.generated.h"

namespace HandPump
{
	static constexpr float DEFAULT_CURR_ENT_WATER					= 10.0f;		//默认当前水位
	static constexpr float MIN_WATER_FOR_REPAIR						= 10.0f;		//默认最小修复水量
	static constexpr float DEFAULT_MAX_WATER						= 100.0f;		//默认最大水位
	static constexpr float DEFAULT_MAX_ADD_WATER_PER_Time			= 10.0f;		//默认每次按下增加增加的水位
	static constexpr float DEFAULT_DURABILITY_PERCENTAGE			= 1.0f;			//默认耐久度
	static constexpr float DEFAULT_MAX_DURABILITY_PERCENTAGE		= 1.0f;			//默认最大耐久度
	static constexpr float DEFAULT_DURABILITY_LOSS_PER_PUMP			= 0.2f;			//默认每次泵水损耗的耐久度
	static constexpr float DEFAULT_MAX_ADD_DURABILITY_PERCENTAGE	= 10.0f;		//默认耐久度危险阈值
	
	static constexpr float MIN_WATER_FOR_PERAIR						= 10.0f;		//默认最小修复水量
	static constexpr float PEPAIR_RESTORE_PERCENT					= 0.8f;			//修复耐久度占比
	static constexpr int32 PEP_AIR_ATTEMPTS							= 20;			//修复次数限制
}


UCLASS()
class ASHFARM_API AHandPump : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHandPump();
	//ClampMin = "50", ClampMax = "500" clap钳住 限制值 
	
	//constexpr 定义常量,效率最高,编译时候计算，不能在运行时修改
	//constexpr 仅用于int,float,bool C++原生数据类型
	//constexpr 不需要使用UPROPERTY，因为constexpr是编译时计算的，不能在运行时修改

	
	//检查手压井是否损坏
	#pragma region 水位参数
	//水箱当前水位
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "水位", meta = ( AllowPrivateAccess = "当前水位",ClampMin = "0"))//最小水位0
	float CurrentWater = HandPump::DEFAULT_CURR_ENT_WATER;
	//水箱最大水量
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "水位", meta = ( AllowPrivateAccess = "当前水位" ,ClampMin = "50", ClampMax = "500"))//最小水位50，最大水位500))
	float MaxWater = HandPump::DEFAULT_MAX_WATER;
	//每次按下泵增加的水位
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="水位",meta=(Displayname="每次按下增加的水位"))
	float AddWaterPerTime = HandPump::DEFAULT_MAX_ADD_WATER_PER_Time;
	#pragma endregion 
	
	#pragma region 耐久度相关参数
	//耐久度
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="水箱状态",meta=(Displayname="耐久度"))
	float Durabiliity = HandPump::DEFAULT_DURABILITY_PERCENTAGE;
	//最大耐久度
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="水箱状态",meta=(Displayname="最大耐久度",ClampMin = "0", ClampMax = "200"))
	float MaxDurability = HandPump::DEFAULT_MAX_DURABILITY_PERCENTAGE;
	//每次泵水损耗的耐久度
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="水箱状态",meta=(Displayname="每次泵水损耗的耐久度",ClampMin = "0.5", ClampMax = "10.0"))
	float DurabiliityPumpLossPerPump = HandPump::DEFAULT_DURABILITY_LOSS_PER_PUMP;
	
	//耐久度危险阈值
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="水箱状态",meta=(Displayname="耐久度危险阈值",ClampMin = "0", ClampMax = "10.0"))
	float DurabiliityCriticalThreshold = HandPump::DEFAULT_MAX_ADD_DURABILITY_PERCENTAGE;
	//Threshold 危险阈值
	
	//手压井是否损坏
	//当耐久度低于危险阈值时，手压更容易损坏
	//当耐久度<=0时，手压井损坏 无法使用泵水
	
	//手压井是否损坏
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="手压井功能",meta=(Displayname="是否损坏"))
	bool bIsBroken = false;  //Ture :已损坏，false：未损坏

	#pragma endregion
	
	#pragma region 手压井状态
	//修复次数限制
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="手压井状态",meta=(DisplayName="修复次数限制",ClampMin = "0"))
	int32 RepairAttempts = HandPump::PEP_AIR_ATTEMPTS;
	
	//手压井是否正在泵水
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="水箱状态",meta=(DisplayName="是否在泵水"))
	bool bIsPumping = false; //true ：正在蹦水，false：未泵水
	
	//检查手压井是否损坏
	UFUNCTION(BlueprintCallable,Category="手压井状态",meta=(DisplayName="是否损坏"))
	bool IsBroken() const;
	
	//是否在泵水
	UFUNCTION(BlueprintCallable,Category="手压井状态",meta=(DisplayName="是否在泵水"))
	bool IsPumping() const;
	#pragma endregion
	
	#pragma region 设备名称及参数
	//设备名称
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="手压井状态",meta=(DisplayName= "设备名称"))
	FString DeviceName = TEXT("手压井");
	
	//设备ID	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="手压井状态",meta=(DisplayName= "设备ID"))
	FName DeviceID = FName("Pump_Default");
	
	//设备描述
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="手压井状态",meta=(DisplayName= "设备描述"))
	FText DeviceDescription = FText::FromString(TEXT("一口老旧但是还能用的手压井"));
	#pragma endregion
	
	#pragma region 功能函数区
	

	
	
	//取水 TakeWater()
	UFUNCTION(BlueprintCallable,Category="手压井功能",meta=(DisplayName="取水"))
	float TakeWater(float WaterAmount);
	
	
	//泵水 PumpWater()
	UFUNCTION(BlueprintCallable,Category="手压井功能",meta=(DisplayName="泵水"))
	float PumpWater();
	
	//修复手压井
	UFUNCTION(BlueprintCallable,Category="手压井功能",meta=(DisplayName="修复"))
	bool Repair();
	
	//检查测试是否需要修复
	UFUNCTION(CallInEditor,Category="手压井状态",meta=(DisplayName="检查测试是否需要修复"))
	void TestRepair(){Repair();}
	
	
	
	
	
	#pragma endregion
	
	#pragma region 统计和数据函数区
	//记录泵水的次数
	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category="手压井状态",meta=(DisplayName="泵水次数"))
	int32 PumpCount = 0; 
	
	//记录空转次数
	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category="手压井状态",meta=(DisplayName="空转次数"))
	int32 DryRunCount = 0;
	
	//统计和数据函数
	//获取当前水位占比
	UFUNCTION(BlueprintCallable,Category="手压井状态",meta=(DisplayName="获取水位百分比"))
	float GetWaterPercentage() const;
	
	//获取当前耐久度占比
	UFUNCTION(BlueprintCallable,Category="手压井状态",meta=(DisplayName="获取耐久度百分比"))
	float GetDurabiliityPercentage() const;

	#pragma endregion
	
protected:
	// Called when the game starts or when spawned   
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Root")
	TObjectPtr<USceneComponent>  Root; 
	//静态网格体组件
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite,Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> Mesh;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
