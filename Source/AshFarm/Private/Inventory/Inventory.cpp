// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Inventory.h"

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

