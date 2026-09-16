// Fill out your copyright notice in the Description page of Project Settings.


#include "PIantbed.h"
#include "AshFarm.h"
#include "UObject/ConstructorHelpers.h"
#include "DrawDebugHelpers.h"

//初始化静态变量
int32 APIantbed::TotalCount = 0;
// Sets default values
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
	ToxicityLevel = 0; // 毒性等级
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));//创建Mesh的对象
	
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
	
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform);
	CollisionBox->SetBoxExtent(FVector(100.0f, 100.0f,100.0f));
	CollisionBox->SetRelativeLocation(FVector(0.0f, 0.0f,20.0f));
	CollisionBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}
void APIantbed::OnConstruction(const FTransform& Transform)
{
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

	UE_LOG(A_LogAshFarm, Warning, TEXT("种植床ID: %d"), BadID);
	
}


// Called every frame
void APIantbed::Tick(float DeltaTime)
{
	//DeltaTime 时间间隔 帧与帧的时间间隔
	Super::Tick(DeltaTime);

	//UE_LOG(A_LogAshFarm, Warning, TEXT("TotalCount: %d"), TotalCount);
	//土壤肥力损失 = 每单位辐射等级的乘数*辐射等级*时间间隔
	Soilfertility -= 
		RadiationLevel * plantBedDefaults::FERTILITY_LOSS_PER_RADIATION_LEVEL * DeltaTime	//每单位辐射等级*所流失的土壤损失量
	    + plantBedDefaults::FERTILITY_LOSS_PER_SECOND * DeltaTime;							//时间乘数* 和上面合并加上土壤自然损失量
	
	//土壤肥力损失量不能小于0,土壤肥力损失量不能大于最大肥力
	Soilfertility = FMath::Clamp(Soilfertility, 0.0f, MaxSoilFertility);
	//更新土壤肥力状态
	UpdateSoilQuality();
	
	FVector TEXTLoaction= GetActorLocation()+FVector(0,0,100.0f);  //土壤肥力文本位置，FVector(0,0,1 00.0f) 表示在当前位置的上方
	DrawDebugString(
		GetWorld(), 
		TEXTLoaction,
		FString::Printf(TEXT("种植床ID:%d,土壤肥力:%.f,土壤状态:%s"),
			BadID,Soilfertility,*GetSoilQualityText(SoilQuality)),
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
	case EsoilQuality::poor:
		return 0.5f;
	
	case EsoilQuality::Normal:
		return 1.0f;
		
	case EsoilQuality::Fertlie:
		return 1.5f;
	
	default:
		UE_LOG(A_LogAshFarm, Warning, TEXT("种植树ID：%d,未知土壤肥力状态"), BadID);
		return 1.0f;
	}
}
//获取土壤状态文本
	FString APIantbed::GetSoilQualityText(EsoilQuality Quality) const
	{
	switch (Quality)
	{
	case EsoilQuality::poor:
		return TEXT("贫瘠");
	case EsoilQuality::Normal:
		return TEXT("正常");
	case EsoilQuality::Fertlie:
		return TEXT("肥沃");
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
	//土壤肥力高于肥沃阈值，为肥沃
	else
	{
		NewQuality = EsoilQuality::Fertlie;
	}
	if (NewQuality != SoilQuality)
	{
		SoilQuality = NewQuality;
		//输出当前土壤肥力状态
		UE_LOG(A_LogAshFarm, Warning, TEXT("种植床ID: %d,当前土壤肥力状态: %s,土壤肥力:%.2f"), BadID, *GetSoilQualityText(SoilQuality),Soilfertility);
	}
}




