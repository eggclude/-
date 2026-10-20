// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Inventory.h"

#include "AshFarm.h"
#include "IEditableSkeleton.h"
#include "StateTreeTypes.h"
#include "Components/BoxComponent.h"
#include "Engine/ContentEncryptionConfig.h"
#include "Misc/MapErrors.h"

// Sets default values
AInventory::AInventory()
{
 	// 开启Tick函数
	PrimaryActorTick.bCanEverTick = true;
	// 设置Tick间隔
	PrimaryActorTick.TickInterval = 0.5f;
	
	// -----------------------------
	// 创建Root组件 
	// -----------------------------
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root); //SetRootComponent 设置根组件为Root
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
	
	//MeshAsset 静态网格体资产
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Inventory/InventoryMesh/InventoryMesh"));
	if (MeshAsset.Succeeded())
	{
		Mesh ->SetStaticMesh(MeshAsset.Object.Get());// 设置静态网格体 Mesh -> 网格体（MeshAsset.Object.Get() Get获取 MeshAsset静态网格体 Object资产）
	}
	
	
	//-------------------------------------
	//碰撞盒子
	//-------------------------------------
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));// 创建碰撞盒子组件 CollisionBox= 碰撞盒子组件
	CollisionBox ->AttachToComponent(Mesh,FAttachmentTransformRules::KeepWorldTransform);// 碰撞盒子组件CollisionBox -> 静态网格体Mesh,FAttachmentTransformRule 保持世界变换 KeepWorldTransform 保持世界变换（保持碰撞盒子组件在静态网格体中的位置和方向）
	// 设置碰撞盒子组件CollisionBox的大小
	CollisionBox ->SetBoxExtent(FVector(100.0f,100.0f,100.0f));
	// 设置碰撞盒子组件CollisionBox的相对位置
	CollisionBox ->SetRelativeLocation(FVector(0.0f,0.0f,20.0f));
	//碰撞配置文件预设 
	CollisionBox ->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

// Called when the game starts or when spawned
void AInventory::BeginPlay()
{
	Super::BeginPlay();
	
	
	
}
// Called every frame
void AInventory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
//获取资源配置
FString AInventory::GetResourceTypeText(EResourcesType Type) const
{
	switch (Type)//获取里的所有资源配置类型
	{
		case EResourcesType::Water:
			return FString("水");
		case EResourcesType::Wood:
			return FString("木材");
		case EResourcesType::Soil:
			return FString("土壤");
		case EResourcesType::Fertilizer:
			return FString("肥料");
		case EResourcesType::Waste:
			return FString("垃圾");
		case EResourcesType::Gold:
			return FString("黄金");
		case EResourcesType::Tool:
			return FString("工具");
		case EResourcesType::Plant:
			return FString("种子");
		default:
			return FString("未知资源");
		break;
	}
}
//添加资源
int32 AInventory::AddResource(EResourcesType Type, int32 Count)
{
	if (Count == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddResource:添加资源数量必须大于0"));
		return 0;
	}
	
	//限制添加资源数量，不能超过最大容量
	
	//下面所有的一切都是为了查询Tarray中的资源配置（通过Type查询）
	//FindByPredicate 根据条件查找到第一个资源配置 每次碰到的话都应该返回指针
	
	//Lambda表达式：匿名函数  可以把它当成一个参数传递
	//[Type] 表示捕获Type变量	，使Lambda表达式可以访问Type变量中的值
	//(const FResourceConfig* Config)将遍历的数组元素传给Config参数
	//如果 Config.Type里的Type等于我们查到的Type，就返回true 那他就会讲获取到的Type给FindByPredicate函数 没有必要直接返回 我只需要返回资源返回到他的地址就可以了
	//难点1 lambda表达式 以及 Tmap的使用
	const FResourceConfig* Config = ResourcesConfigs.FindByPredicate(
		[Type](const FResourceConfig& Config)
		{
			return Config.Type == Type;
		}
	);
	
	// 为了满足 CurrentCount + Count <= Config->MaxCapacity;
	//首先要获取CurrentCount这个Type类型的当前数量
	//难点2  ResourcesInventory.Find(Type) 他其实是通过Type键来查找值 
	//因为前面定义的Tmap是EResourcesType, int32 前面是键 其实就是整数 通过这个整数来寻找到我们要的资源数量 后面是值 返回的其实是指针
	//如果返回没问题能找到的话就将指针的数值提取出来也就是*CurrentCount 
	// *CurrentCount指向当前数量的指针 从指针解引用 就是获取当前值
	const int32* CurrentCount = ResourcesInventory.Find(Type);//通过Tmap.Find(Type)来key键值对查找Value值 const int32* 是一个指针 指向int32类型的变量 
	int32 Current = CurrentCount ? *CurrentCount : 0; //如果CurrentCount存在 就返回*CurrentCount 如果不存在 就返回0
	
	//还可以塞多少物资资源
	//难点3 把它拆开来理解 Config->MaxCapacity-Current
	//1) 先算剩余容量：暂存类 = Config->MaxCapacity - Current
	//2) 再取较小值：CanAdd = FMath::Min(Count, Remaining)
	//如果CanAdd大于Count 就返回Count 如果小于等于 就返回CanAdd
	int32 CanAdd =  FMath::Min(Count,Config->MaxCapacity-Current);// //如果CanAdd大于Count 就返回Count 如果小于等于 就返回CanAdd
	
	if (CanAdd <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddResource:添加资源数量已超过最大容量"));
		return 0;
	}
	//ADD:如果Type这个键存在 就更新Value值（Current+CanAdd覆盖旧的Value值） 如果不存在 就添加键值对（Type,Current+CanAdd）
	ResourcesInventory.Add(Type,Current+CanAdd);//如果能塞进来的话 通过映射Tmap：ResourcesInventory来添加
	

	UE_LOG(LogTemp, Warning, TEXT("仓库资源%s] 添加数量为%d"), *GetResourceTypeText(Type), CanAdd);
	return CanAdd;//最后这一大堆还是会返回CanAdd 但是CanAdd已经限制了不能超过最大容量
}	
//移除资源
int32 AInventory::RemoveResource(EResourcesType Type, int32 Count)
{
	if (Count<= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("仓库资源%s] 移除数量必须大于0"), *GetResourceTypeText(Type));
		return Count;
	}
	//查询一下库存数量
	const int32* CurrentCount = ResourcesInventory.Find(Type);
	if (!CurrentCount || *CurrentCount < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("仓库资源%s] 不存在"), *GetResourceTypeText(Type));
		return 0;
	}
	int32 Current = *CurrentCount;//把指针的数值提取出来也就是*CurrentCount 从指针解引用 就是获取当前值
	int32 CanRemove = FMath::Min(Count,Current);
	
	//移除资源
	int32 NewCount = Current-CanRemove;//如果能移除进来的话 通过映射Tmap的Type键找到对应的ResourcesInventory来移除
		if (NewCount > 0)
	{
		ResourcesInventory.Add(Type,NewCount);//更新资源数量
	}
	else
	{
		ResourcesInventory.Remove(Type);//移除该类型库存 如果移除进来的话 通过映射Tmap的Type键找到对应的ResourcesInventory来移除
	}
	UE_LOG(A_LogAshFarm, Warning, TEXT("仓库资源%s] 剩余数量为%d"), *GetResourceTypeText(Type), NewCount);
		return CanRemove;//返回实际移除的资源数量
	}
//查询资源数量
int32 AInventory::GetResourceCount(EResourcesType Type)
{
	return 0;
}
//查询资源数量是否足够
bool AInventory::HasEnoughResource(EResourcesType Type, int32 Count) const
{
	return false;
}