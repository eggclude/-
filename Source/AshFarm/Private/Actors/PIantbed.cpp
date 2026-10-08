// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PIantbed.h"
#include "Plants/PlantBase.h"
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
	
	SoilFertility = plantBedDefaults::FERTILITY_FERTILE_THRESHOLD; // 土壤肥力
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
	PlantMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("植物网格体"));
	PlantMesh->AttachToComponent(PlantingPoint, FAttachmentTransformRules::KeepRelativeTransform);//植物网格体附着到种植点
	PlantMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);//关闭植物网格体的碰撞
	
	
	//碰撞盒：名字必须和"种植点"区分开，挂在种植点下面
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
	UpdatePlantMesh();//初始化植物网格体
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
	Super::Tick(DeltaTime); //

	
	//初始化过度次数为0
	TransitionCount = 0;
	//UE_LOG(A_LogAshFarm, Warning, TEXT("TotalCount: %d"), TotalCount);
	SetFertilityLossPerSecond(DeltaTime);		//设置土壤肥力流失率
	
		//设置土壤肥力恢复速率
    	//无辐射时，土地肥力自愈速率（每秒）
    	if (RadiationLevel == 0)
    	{
    		SoilFertility += plantBedDefaults::FERTILITY_RECOVER_REC_SECOND * DeltaTime;
    		//确保土壤肥力在最大肥力以下
    		SoilFertility = FMath::Clamp(SoilFertility,0.0f,MaxSoilFertility);
    	}
	SetMoistureLossPerSecond(DeltaTime);			//设置土壤水分流失率
	
	//植物生长与土壤消耗
	if (CurrentPlant != nullptr)//首先判断是不是指针
	{
		//调用植物的生长函数 并传递Grow内参数运行
		CurrentPlant->Grow(
			DeltaTime,
			SoilQuality,
			SoilType,
			SoilFertility,
			Moisture,
			Temperature,
			RadiationLevel,
			ToxicityLevel);

		//土壤水分消耗
		if (Moisture >= 0.0f)//如果土壤湿度大于等于0
		{
			//开始耗水
			Moisture -= CurrentPlant->WaterConsumption * DeltaTime;
			//确保土壤湿度在最大土壤水分含量以下
			Moisture = FMath::Clamp(Moisture,0.0f,MaxMoisture);
		}
		//土壤肥力消耗
		if (SoilFertility >= 0.0f)//如果土壤肥力大于等于0
		{
			//开始耗肥
			SoilFertility -= CurrentPlant->FertilityConsumption * DeltaTime;
			//确保土壤肥力在最大肥力以下
			SoilFertility = FMath::Clamp(SoilFertility,0.0f,MaxSoilFertility);
		}

		//设置网格体
		UpdatePlantMesh();
	}
	
	//更新土壤肥力状态
	UpdateSoilQuality();
	
	FVector TEXTLoaction= GetActorLocation()+FVector(0,0,100.0f);  //土壤肥力文本位置，FVector(0,0,1 00.0f) 表示在当前位置的上方
	DrawDebugString(
		GetWorld(), 
		TEXTLoaction,
		FString::Printf(TEXT("种植床ID:%d \n 土壤肥力:%.f 土壤品质:%s ,土壤类型:%s  土壤湿度:%.f \n 当前作物:%s ,当前文本的生长阶段:%s(进度：%f)，逆境值此刻为:(%.f),品质为:%s\n"),
		BadID,
		SoilFertility,//土壤肥力
		*GetSoilTypeText(),//土壤类型
		*GetSoilQualityText(),//土壤品质
		Moisture,//土壤湿度
		*CurrentPlant->GetPlantName(),//当前作物
		*CurrentPlant->GetGrowthStageText(),//当前生长阶段
		CurrentPlant->GrowthProgress,//当前进度
		CurrentPlant->Stress,//当前逆境值
		*CurrentPlant->GetQualityText()),//当前品质
		nullptr,
		FColor::White,
		0.0f
		);   //显示时间间隔为0.5秒
}

//结束播放事件
void APIantbed::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	APIantbed::TotalCount--;
}
//获取土壤肥力
float APIantbed::GetSoilFertility() const
{
	return SoilFertility;
}

//更新植物网格体
void APIantbed::UpdatePlantMesh()
{
	if (CurrentPlant != nullptr) //更新网格体 不等于空指针
	{
		UStaticMesh *StageMesh = CurrentPlant->GetStageMesh(); //获取当前植物的阶段网格体
		if (StageMesh != nullptr&&PlantMesh ->GetStaticMesh()!=StageMesh)//如果阶段网格体不是空的并且植物网格体不是当前阶段网格体
		{
			PlantMesh ->SetStaticMesh(StageMesh);//设置植物网格体为当前阶段网格体
		}
	}
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
	//获取土壤品质文本
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
//获取土壤类型文本
FString APIantbed::GetSoilTypeText()const
{
	switch (SoilType)
	{
	case ESoilType::Sand:
		return TEXT("沙土");
	case ESoilType::Loam:
		return TEXT("粘土");
	case ESoilType::Clay:
		return TEXT("粘土");
	default:
		return TEXT("未知");
	}
}

//设置土壤肥力
void APIantbed::SetSoilFertility(float Fertility)
{
	SoilFertility = Fertility;
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
	if ( SoilFertility < plantBedDefaults::FERTILITY_POOR_THRESHOLD)
		NewQuality = EsoilQuality::poor;
	//土壤肥力低于肥沃阈值，高于贫瘠阈值，为正常
	else if (SoilFertility < plantBedDefaults::FERTILITY_FERTILE_THRESHOLD)
	{
		NewQuality = EsoilQuality::Normal;
	}
	//盐碱地状态
	else if (SoilFertility < plantBedDefaults::FERTILITY_SALINE_THRESHOLD)
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
		BadID, *GetSoilQualityText(),SoilFertility,TransitionCount);
	
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

//根据土壤类型计算肥力流失倍率
float APIantbed::GetFertilityLossRateBySoilType() const
{
	switch (SoilType)
	{
	case ESoilType::Sand:
		return plantBedDefaults::MOISTURE_SAND_LOSS_MULTI;
	case ESoilType::Loam:
		return plantBedDefaults::MOISTURE_LOAM_LOSS_MULTI;
	case ESoilType::Clay:
		return plantBedDefaults::MOISTURE_CLAY_LOSS_MULTI;
	default:
		return 1.0f;
	}
}
//根据土壤类型计算湿度流失倍率
float APIantbed::GetMoistureLossRateBySoilType() const
{
	switch (SoilType)
	{
	case ESoilType::Sand:
		return plantBedDefaults::FERTILITY_SAND_LOSS_MULTI;//沙土湿度水分自然流失量
	case ESoilType::Loam:
		return plantBedDefaults::FERTILITY_LOAM_LOSS_MULTI;//泥土湿度水分自然流失量
	case ESoilType::Clay:
		return plantBedDefaults::FERTILITY_CLAY_LOSS_MULTI;//黏土湿度水分自然流失量
	default:
		return 1.0f;
	}
}

//设置土壤湿度流失
void APIantbed::SetMoistureLossPerSecond(float DeltaTime)
{
	//todo: 实现土壤流失率
	//土壤流失率 = 获取默认土壤流失率 * 根据土壤类型计算湿度流失倍率 * 时间间隔
	Moisture -=
	plantBedDefaults::MOISTURE_LOSS_PER_SECOND 
	* GetMoistureLossRateBySoilType() * DeltaTime;
	
	//土壤湿度不能小于0,土壤湿度不能大于最大湿度
	Moisture = FMath::Clamp(Moisture, 0.0f, MaxMoisture);
}



//设置土壤肥力流失 
void APIantbed::SetFertilityLossPerSecond(float DeltaTime)
{
	//土壤肥力损失 = 每单位辐射等级的乘数*辐射等级*根据土壤类型计算肥力流失倍率*时间间隔
	SoilFertility -= 
	RadiationLevel * plantBedDefaults::FERTILITY_LOSS_PER_RADIATION_LEVEL *GetFertilityLossRateBySoilType() * DeltaTime	//每单位辐射等级*所流失的土壤损失量
	+ plantBedDefaults::FERTILITY_LOSS_PER_SECOND * DeltaTime;							//时间乘数* 和上面合并加上土壤自然损失量
	
	//土壤肥力损失量不能小于0,土壤肥力损失量不能大于最大肥力
	SoilFertility = FMath::Clamp(SoilFertility, 0.0f, MaxSoilFertility);
	
}

