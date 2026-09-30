// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PIantbed.h"
#include "AshFarm.h"
#include "UObject/ConstructorHelpers.h"
#include "DrawDebugHelpers.h"

//初始化静态变量
int32 APIantbed::TotalCount = 0;
// Sets default values
//构造函数
APIantbed::APIantbed()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//开启Tick函数 ,interval 100ms
	PrimaryActorTick.bCanEverTick = true;
	//设置Tick函数的间隔为500ms
	PrimaryActorTick.TickInterval = 0.5f;
	
	Soilfertility = plantBedDefaults::FERTILITY_FERTILE_THRESHOLD; // 土壤肥力
	MaxSoilFertility = plantBedDefaults::MAX_SOIL_FERTILITY; // 最大肥力
	Moisture = 0.1f; // 土壤水分
	MaxMoisture = 1.0f; // 最大土壤水分含量
	Temperature = 26.0f; // 土壤温度
	RadiationLevel = 0; // 辐射等级
	RadiationLevel = 0; // 辐射等级
	ToxicityLevel = 0; // 毒性等级
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));//创建Mesh的对象
	
	
	//nameSpace命令空间 用于指定资源的路径
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (MeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(MeshAsset.Object.Get());
	}
	
	
	Mesh->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
	
	
	SetRootComponent(Mesh);
	//CreateDefaultSubobject 在构造函数内生一个组件、
	//组件可以举例为 人类的手机 挂在人的身上 人虽然有手机但是不是出生就有的
	//Mesh和Root是类的对象
	//AttachToComponent附着到组件 
	//FAttachmentTransformRule附着的方式
	//KeepRelativeTransform相对位置
	//整段翻译就是 我设定了一个值mesh 他在构造函数内生成一个组件CreateDefaultSubobject 然后他说 ustaticMeshComponent 然后名字Text名字 设定好了后指向吧Mesh指向
	//AttachToComponent附着到组件Root 他附着的方式是FAttachmentTransformRule 然后获取相对位置KeepRelativeTransform
	//nameSpace命令空间
	
	
	
	PlantingPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("种植点"));
	PlantingPoint->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform);
	PlantingPoint->SetRelativeLocation( FVector(0.0f, 0.0f, 20.0f) );
	
	//植物网格体：名字必须和"种植点"区分开，挂在种植点下面
	PlantMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlantMesh"));
	PlantMesh->AttachToComponent(PlantingPoint, FAttachmentTransformRules::KeepRelativeTransform);//植物网格体附着到种植点
	PlantMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);//关闭植物网格体的碰撞
	
	
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("碰撞盒"));
	CollisionBox->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform);//碰撞盒附着到Mesh组件
	CollisionBox->SetBoxExtent(FVector(100.0f, 100.0f,100.0f));//设置碰撞盒的大小为(100,100,100)
	CollisionBox->SetRelativeLocation(FVector(0.0f, 0.0f,20.0f));//设置碰撞盒的相对位置为(0,0,20)
	CollisionBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));//设置碰撞盒的碰撞配置文件为OverlapAllDynamic
}
void APIantbed::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);  //调用父类的构造函数
	
	UpdateSoilQuality();//初始化土壤肥力状态
}
//析构函数
APIantbed::~APIantbed()
{
}

//开始播放事件
void APIantbed::BeginPlay()
{
	Super::BeginPlay();
	APIantbed::TotalCount++;  //TotalCount = 0 自增1
	
	//初始化土壤肥力状态
	UpdateSoilQuality();
	ensureAlwaysMsgf(MaxSoilFertility > 0.0f, TEXT("最大肥力MaxSoilFertility不能小于0"));
	ensureAlwaysMsgf(MaxMoisture > 0.0f, TEXT("最大土壤水分含量MaxMoisture不能小于0"));
	
	//获取土壤肥力状态
	UE_LOG(A_LogAshFarm, Warning, TEXT("种植床ID: %d"), BadID);
	
}


// Called every frame
void APIantbed::Tick(float DeltaTime)
{
	//DeltaTime 时间间隔 帧与帧的时间间隔
	Super::Tick(DeltaTime);

	
	//初始化过度次数为0
	TransitionCount = 0;
	//UE_LOG(A_LogAshFarm, Warning, TEXT("TotalCount: %d"), TotalCount);
	SetFertilityLossPerSecond(DeltaTime);		//设置土壤肥力流失率
	SetMoistureLossPerSecond(DeltaTime);			//设置土壤水分流失率
	
	//设置土壤肥力恢复速率
	//无辐射时，土地肥力自愈速率（每秒）
	if (RadiationLevel == 0)
	{
		Soilfertility += plantBedDefaults::FERTILITY_RECOVER_REC_SECOND * DeltaTime;
		//确保土壤肥力在最大肥力以下
		Soilfertility = FMath::Clamp(Soilfertility,0.0f,MaxSoilFertility);
	}
	
	//更新土壤肥力状态
	UpdateSoilQuality();
	
	FVector TEXTLoaction= GetActorLocation()+FVector(0,0,100.0f);  //土壤肥力文本位置，FVector(0,0,1 00.0f) 表示在当前位置的上方
	DrawDebugString(
		GetWorld(), 
		TEXTLoaction,
		FString::Printf(TEXT("种植床ID:%d,土壤肥力:%.f,土壤状态:%s,土壤湿度:%.f"),
			BadID,Soilfertility,*GetSoilQualityText(),Moisture),
			nullptr ,
			FColor::White,
			0.5f,
			true);   //显示时间间隔为0.5秒
}

//Endplay Event
void APIantbed::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	Super::EndPlay(EndPlayReason);
	APIantbed::TotalCount--;
}
//获取土壤肥力
float APIantbed::GetSoilfertility() const
{
	return Soilfertility;
}
//获取生长速度
float APIantbed::GetGrowthSpeed() const
{
	switch (SoilQuality)
	{
	//贫瘠状态
	case EsoilQuality::poor:
		return GROWTH_SPEED_POOR;
	//正常状态
	case EsoilQuality::Normal:
		return GROWTH_SPEED_NORMAL;
	//肥沃状态
	case EsoilQuality::Fertlie:
		return GROWTH_SPEED_FERTILE;
	//盐碱地状态
	case EsoilQuality::Saline:
		return GROWTH_SPEED_SALINE;
	
	default:
		UE_LOG(A_LogAshFarm, Warning, TEXT("种植树ID：%d,未知土壤肥力状态"), BadID);
		return GROWTH_SPEED_NORMAL;
	}
}
	//获取土壤状态文本
	FString APIantbed::GetSoilQualityText() const
	{
	switch (SoilQuality)
	{
	case EsoilQuality::poor:
		return TEXT("贫瘠");
	case EsoilQuality::Normal:
		return TEXT("正常");
	case EsoilQuality::Fertlie:
		return TEXT("肥沃");
	case EsoilQuality::Saline:
		return TEXT("盐碱地");
	default:
		return TEXT("未知");
	}
}



//设置土壤肥力
void APIantbed::SetSoilfertility(float Fertility)
{
	Soilfertility = Fertility;
}
//获取所有种植床的数量
int32 APIantbed::GetTotalCount()
{
	return APIantbed::TotalCount;
}
//更新土壤肥力状态
 void APIantbed::UpdateSoilQuality()
{
	EsoilQuality NewQuality;
	
	//todo: 实现土壤肥力状态
	//土壤肥力状态判断
	//土壤肥力低于贫瘠阈值，为贫瘠
	if ( Soilfertility < plantBedDefaults::FERTILITY_POOR_THRESHOLD)
		NewQuality = EsoilQuality::poor;
	//土壤肥力低于肥沃阈值，高于贫瘠阈值，为正常
	else if (Soilfertility < plantBedDefaults::FERTILITY_FERTILE_THRESHOLD)
	{
		NewQuality = EsoilQuality::Normal;
	}
	//盐碱地状态
	else if (Soilfertility < plantBedDefaults::FERTILITY_SALINE_THRESHOLD)
	{
		NewQuality = EsoilQuality::Saline;
	}
	//土壤肥力高于肥沃阈值，为肥沃
	else 
	{
		NewQuality = EsoilQuality::Fertlie;
	}
	if (NewQuality != SoilQuality)
	{
		SoilQuality = NewQuality;
		//输出当前土壤肥力状态
		TransitionCount++;
		UE_LOG(A_LogAshFarm, Warning, TEXT("种植床ID: %d,当前土壤肥力状态: %s,土壤肥力:%.2f,(土壤转换次数:%d)"), 
		BadID, *GetSoilQualityText(),Soilfertility,TransitionCount);
	
	}
}
//获取土壤湿度流失率
float APIantbed::GetMoistureLossRate() const
{
	switch (SoilQuality)
	{
		//贫瘠状态
	case EsoilQuality::poor:
		return plantBedDefaults::MOISTURE_POOR_LOSS_MULTI;
		//正常状态
	case EsoilQuality::Normal:
		return plantBedDefaults::MOISTURE_NORMAL_LOSS_MULTI;
		//肥沃状态
	case EsoilQuality::Fertlie:
		return plantBedDefaults::MOISTURE_FERTILE_LOSS_MULTI;
	case EsoilQuality::Saline:
		//盐碱地状态
		return plantBedDefaults::MOISTURE_SALINE_LOSS_MULTI;
	default:
		//未知状态
		UE_LOG(A_LogAshFarm, Warning, TEXT("种植树ID：%d,未知土壤肥力状态"), BadID);
		return 1.0f;
	}
}

//设置土壤流失率
void APIantbed::SetMoistureLossPerSecond(float DeltaTime)
{
	//todo: 实现土壤流失率
	//土壤流失率 = 获取默认土壤流失率 * 时间间隔
	Moisture -= plantBedDefaults::MOISTURE_LOSS_PER_SECOND * GetMoistureLossRate() * DeltaTime;
	//土壤湿度不能小于0,土壤湿度不能大于最大湿度
	Moisture = FMath::Clamp(Moisture, 0.0f, MaxMoisture);
}



//设置土壤肥力流失率
void APIantbed::SetFertilityLossPerSecond(float DeltaTime)
{
	//土壤肥力损失 = 每单位辐射等级的乘数*辐射等级*时间间隔
	Soilfertility -= 
		RadiationLevel * plantBedDefaults::FERTILITY_LOSS_PER_RADIATION_LEVEL * DeltaTime	//每单位辐射等级*所流失的土壤损失量
		+ plantBedDefaults::FERTILITY_LOSS_PER_SECOND * DeltaTime;							//时间乘数* 和上面合并加上土壤自然损失量
	
	//土壤肥力损失量不能小于0,土壤肥力损失量不能大于最大肥力
	Soilfertility = FMath::Clamp(Soilfertility, 0.0f, MaxSoilFertility);
	
}


//获取作物名称
/*
FString APIantbed::GetCropName(ECropType Crop) const
{
	switch (Crop)
	{
	case ECropType::None:
		return TEXT("无作物");
	case ECropType::Wheat:
		return TEXT("小麦");
	case ECropType::Corn:
		return TEXT("玉米");
	default:
		return TEXT("未知");
	}
}
*/

//评估种植结果
//FString APIantbed::EvaluatePlanting(ECropType Crop) const
//{
	/*FString Result;

	if (SoilQuality == EsoilQuality::Saline)
	{
		Result = TEXT("盐碱地不支持种植");
	}
	else if (Moisture <0.3f)
	{
		Result = TEXT("土壤湿度不足,浇点水再来");
	}
	else if (RadiationLevel >0)
	{
		Result = TEXT("有辐射,不支持种植");
	}
	else
	{
		switch (Crop)
		{
			case ECropType::Wheat:
				Result = TEXT("小麦能种");
				break;
			case ECropType::Corn:
				if (Soilfertility < 50.0f || Moisture < 0.5f)
				{
					Result = TEXT("玉米不喜欢这块地");
				}
				else
				{
					Result = TEXT("玉米能种");
				}
				break;
			default:
				Result = TEXT("未知作物,不支持种植");
				break;
		}
	}  
	
	//【种植评估】作物: 玉米 | 土壤: 普通 (肥力 62) | 水分: 45% | 结论: 玉米嫌弃这块地
	return FString::Printf(
		TEXT("【种植评估】作物: %s | 土壤: %s (肥力 %.0f) | 水分: %.2f%% | 结论: %s"),
		*GetCropName(Crop), *GetSoilQualityText(), Soilfertility, Moisture * 100.0f, *Result);
}*/
