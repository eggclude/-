// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Plants/PlantBase.h"
#include "PIantbed.generated.h"

//基类：种植床




#define GROWTH_SPEED_SALINE		0.0F
#define GROWTH_SPEED_POOR		0.5F
#define GROWTH_SPEED_NORMAL		1.0F
#define GROWTH_SPEED_FERTILE	1.5F

//种植床默认值 阈值类
namespace plantBedDefaults
{
	static constexpr float DEFAULT_SOIL_FERTILITY					= 60.0f;	//默认土壤肥力
	static constexpr float MAX_SOIL_FERTILITY						= 100.0f;	//最大土壤肥力
	static constexpr float FERTILITY_POOR_THRESHOLD					= 30.0f;	//土壤肥力贫瘠阈值，低于该值为贫瘠
	static constexpr float FERTILITY_FERTILE_THRESHOLD				= 70.0f;	//土壤肥沃阈值，高于该值为土壤肥沃	
	static constexpr float FERTILITY_SALINE_THRESHOLD				= 10.0f;	//土壤肥力盐碱地阈值，低于该值为盐碱地
	
	static constexpr float FERTILITY_LOSS_PER_RADIATION_LEVEL		= 0.01f;	//每单位辐射等级的乘数,土壤肥力损失量
	static constexpr float FERTILITY_LOSS_PER_SECOND				= 0.01f;	//土壤肥力自然损失量	
	
	static constexpr float MOISTURE_LOSS_PER_SECOND					= 0.005f;	//归一化，土壤湿度自然流失量
	static constexpr float MOISTURE_SALINE_LOSS_MULTI				= 1.5f;		//乘数，盐碱地土壤湿度自然流失量
	static constexpr float MOISTURE_POOR_LOSS_MULTI					= 2.0f;		//乘数，贫瘠地土壤湿度自然流失量
	static constexpr float MOISTURE_NORMAL_LOSS_MULTI				= 1.0f; 	//乘数，正常土壤湿度自然流失量
	static constexpr float MOISTURE_FERTILE_LOSS_MULTI				= 0.5f;		//乘数，肥沃土壤湿度自然流失量
	static constexpr float FERTILITY_RECOVER_REC_SECOND				= 0.2f;		//无辐射时，土地肥力自愈速率（每秒）
	
}

UENUM(BlueprintType)
enum class EsoilQuality: uint8 
{
	Saline UMETA(DisplayName=	"土壤状态:盐碱地"),
	poor   UMETA(DisplayName=	"土壤状态：贫瘠"),
	Normal UMETA(DisplayName=	"土壤状态：正常"),
	Fertlie UMETA(DisplayName=	"土壤状态：肥沃"),
};

//UENUM(BlueprintType)
//enum class ECropType: uint8 
//{
//	None UMETA(DisplayName=	"无作物"), 
//	Wheat UMETA(DisplayName="小麦"), 
//	Corn UMETA(DisplayName=	"玉米")
//};

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
	//目前种植的作物类型
	UPROPERTY(EditAnywhere,Instanced,BlueprintReadWrite,Category="种植",meta=(DisplayName="种植作物类型"))
	TObjectPtr<UPlantBase> CurrentPlant;
	
	//土壤状态
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="土壤",meta=(DisplayName="土壤状态"))
	EsoilQuality	SoilQuality = EsoilQuality::Normal;
	
	//土壤肥力 
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="土壤",meta=(DisplayName="土壤肥力"))
	float Soilfertility = plantBedDefaults::DEFAULT_SOIL_FERTILITY;
	
	//MaxSoilFertility 最大肥力 设置最大肥力
	UPROPERTY(EditAnywhere,Category="土壤",meta=(DisplayName= "最大肥力"))
	float MaxSoilFertility = plantBedDefaults::MAX_SOIL_FERTILITY; 
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
	
	//获取生长速度
	UFUNCTION(BlueprintCallable,Category="土壤",meta=(DisplayName = "获取生长速度"))
	float GetGrowthSpeed() const;
	
	//获取土壤状态文本
	UFUNCTION(BlueprintPure,Category="土壤",meta=(DisplayName="获取土壤状态文本"))
	FString GetSoilQualityText() const;
	
	//获取土壤湿度流失率
	UFUNCTION(BlueprintPure,Category="土壤",meta=(DisplayName="获取土壤湿度流失率"))
	float GetMoistureLossRate() const;
		
	//设置土壤流失率
	UFUNCTION(BlueprintCallable,Category="土壤",meta=(DisplayName="设置土壤流失率"))
	void SetMoistureLossPerSecond(float DeltaTime);
	
	//设置土壤肥力流失率
	UFUNCTION(BlueprintCallable,Category="土壤",meta=(DisplayName="设置土壤肥力流失率"))
	void SetFertilityLossPerSecond(float DeltaTime);
	
	//构造函数
	//OnConstruction 构造函数，当实例化时调用,类似于构造函数() 每次拖动或者修改值时调用
	virtual void OnConstruction(const FTransform& Transform) override;
	
	//评估种植结果
	//UFUNCTION(BlueprintCallable,Category="种植",meta=(DisplayName="评估种植结果"))
	//FString EvaluatePlanting(ECropType Crop) const;
	
	//获取作物名称
	/*UFUNCTION(BlueprintPure,Category="种植",meta=(DisplayName="获取作物名称"))
	FString GetCropName(ECropType Crop) const;*/
	
	//获取所有ApiantBed实例的数量
	UFUNCTION(BlueprintCallable,Category="统计",meta=(DisplayName="获取所有ApiantBed实例的数量"))
	static  int32 GetTotalCount();
	
	
	#pragma region 土壤状态机
	
	//土壤状态机
	//记录土壤转换次数
	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category="土壤",meta=(DisplayName="土壤转换次数"))
	int32 TransitionCount = 0;
	
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
	
	//静态网格体组件
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite,Category = "Mesh",meta=(DisplayName="植物网格体"))
	TObjectPtr<UStaticMeshComponent> PlantMesh;
	
	
#pragma region 功能函数
	
	
	//更新土壤肥力状态
	UFUNCTION(BlueprintCallable,Category="土壤",meta=(DisplayName="更新土壤肥力状态"))
	void UpdateSoilQuality();
	
	
   //更新植物网格体
	UFUNCTION(BlueprintCallable,Category="土壤",meta=(DisplayName="更新植物网格体"))
	void UpdatePlantMesh();
	
	#pragma endregion
	
	
	
	
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
