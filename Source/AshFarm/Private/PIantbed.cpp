// Fill out your copyright notice in the Description page of Project Settings.


#include "PIantbed.h"
#include "UObject/ConstructorHelpers.h"

//初始化静态变量
int32 APIantbed::TotalCount = 0;
// Sets default values
APIantbed::APIantbed()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Soilfertility = plantBedDefaults::FERTILITY_FERTILE_THRESHOLD; // 土壤肥力
	MaxFertility = plantBedDefaults::MAX_SOIL_FERTILITY; // 最大肥力
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
//析构函数
APIantbed::~APIantbed()
{
	
}

//开始播放事件
void APIantbed::BeginPlay()
{
	Super::BeginPlay();
	APIantbed::TotalCount++;  //TotalCount = 0 自增1
	
	UE_LOG(LogTemp, Warning, TEXT("种植床ID: %d"), BadID);
	
}


// Called every frame
void APIantbed::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("TotalCount: %d"), TotalCount);
	//更新土壤肥力状态
	UpdateSoilQuality();
}

//Endplay Event
void APIantbed::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
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
		UE_LOG(LogTemp, Warning, TEXT("种植树ID：%d,未知土壤肥力状态"), BadID);
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
	else if (Soilfertility< plantBedDefaults::FERTILITY_FERTILE_THRESHOLD)
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
		UE_LOG(LogTemp, Warning, TEXT("种植床ID: %d,当前土壤肥力状态: %s,土壤肥力:%.2f"), BadID, *UEnum::GetValueAsString(SoilQuality),Soilfertility);
	}
}




