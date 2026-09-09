// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ObjectPtr.h"
#include "Components/BoxComponent.h"
#include "PIantbed.generated.h"

UCLASS(ClassGroup = "AshFarm | 种植系统")
class ASHFARM_API APIantbed : public AActor
{
	GENERATED_BODY()
//publicg：公开，在他区域内的函数和变量都可以在外部调用	（蓝图，其他类）
public:	
	// Sets default values for this actor's properties
	
	//构造函数
	APIantbed();
	
	//析构函数
	~APIantbed();
	
	//Bad id
	UPROPERTY(EditInstanceOnly,BlueprintReadWrite,Category="种植床",meta=(DisplayName="种植床ID"))
	int32 BadID = 0;
	
	//Uint8 : 无符号8位整数
	//int16 : 16位整数
	//int32 : 32位整数
	//float ：浮点型数（小数）
	//土壤肥力
	//UPROPERTY() : 属性，告诉UE下面的变量，这个变量要注册进反射系统，可以在蓝图中访问
	//BlueprintReadWrite ： 可读可编辑
	//Category : 词条变体名称
	//meta=DisplayName : meta是属性名称 DisplayName是表述 因为ue只识别英文属性名
	//const : 常量，不能被修改
	
	//土壤肥力
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="土壤",meta=(DisplayName="土壤肥力"))
	float Soilfertility = 1.0f ;
	
	//MaxFertility最大肥力 设置最大肥力
	UPROPERTY(EditAnywhere,Category="土壤",meta=(DisplayName= "最大肥力"))
	float MaxFertility = 1.0f; 
	// const 加入后变为常量无法变更
	
	//土壤温度
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="土壤",meta=(Displayname="土壤温度"))
	float Moisture = 0.1f;
	
	//获取最大土壤水分含量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="土壤", meta=(DisplayName="土壤水分含量"))
	float MaxMoisture  = 1.0f;
	
	//土壤温度
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="土壤",meta=(DisPlayName="土壤温度"))
	float Temperature  = 26.0f;
	
	//辐射等级
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="土壤",meta=(DisplayName = "获取辐射等级"))
	int32 RadiationLevel= 0;
	
	//毒性等级
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="土壤",meta=(DisplayName = "毒性等级"))
	float ToxicityLevel = 0.0f;
	
	// 获取土壤肥力
	UFUNCTION(BlueprintCallable,Category="土壤",meta=(DisplayName = "获取土壤肥力"))
	float GetSoilfertility() const;
	
	//设置 土壤肥力
	UFUNCTION(BlueprintCallable,Category="土壤",meta=(DisplayName = "设置土壤肥力"))
	//void 没有返回如何值
	void SetSoilfertility(float Fertility);
	
	
	//获取所有ApiantBed实例的数量
	UFUNCTION(BlueprintCallable,Category="统计",meta=(DisplayName="获取所有ApiantBed实例的数量"))
	static  int32 GetTotalCount();
	
	//组件不需要ue
	//protected 保护，下面的函数和变量可以在本类内部和子类调用，其他类不能调用
protected:
	//组件TObjectPtr
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Root")
	TObjectPtr<USceneComponent>  Root; 
	//静态网格体组件
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite,Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> Mesh;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//种植点组件
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite,Category = "PlantingPoint")
	TObjectPtr<USceneComponent> PlantingPoint;
	//碰撞盒组件
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite,Category = "Box")
	UBoxComponent* CollisionBox; //碰撞盒组件
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	//EndPlay Event
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;	
	
private:
	//static 静态变量
	//统计所有Plant Bad的实例的数量
	static  int32 TotalCount;
	//private 私有，下面的函数智能在本类内部调用
	
	
};
//张三的属性 外貌等都需要在public写入 需要从外部调用的都写在里面
//某种遗传疾病放在protected，稀有属性也写在里面 不需要从外部调用的都写在里面
//私有的内心想法可以在private
