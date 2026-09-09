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
	Soilfertility = 1.0f; // 土壤肥力
	MaxFertility = 1.0f; // 最大肥力
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

// Called when the game starts or when spawned
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





