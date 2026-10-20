// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Inventory/InventoryTypes.h"
#include "Plants/PlantBase.h"
#include "Inventory.generated.h"

UCLASS(Blueprintable,ClassGroup="AshFarm | 库存系统")
class ASHFARM_API AInventory : public AActor //AInventory是AActor的子类,用于表示游戏中的仓库
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInventory();//构造函数
	
	//TArray 数组 资源配置数组 可重复添加 适用于不同类型的资源 有序、可重复、索引访问快
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="仓库",meta=(DisplayName="资源配置数组"))
	TArray<FResourceConfig> ResourcesConfigs;
	
	//资源仓库(资源数量映射)<key=资源类型,value=资源数量> TMap 是无序、元素唯一、快速去重
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="仓库",meta=(DisplayName="资源仓库"))
	TMap<EResourcesType, int32> ResourcesInventory;
	
	//TSet集合 种子资源集合 已经解锁的种子类型   无序、键唯一、键查找快
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="仓库",meta=(DisplayName="种子资源集合"))
	TSet<TSubclassOf<UPlantBase>> PlantResources;//TSubclassOf
	//UPlantBase之前用的都是TObjectPtr的对象指针  是一个实实在在的对象  TSubclassOf是一个指针类
	
	//--------------------------------------------------
	//======仓库功能======//
	//--------------------------------------------------
	//添加资源
	UFUNCTION(BlueprintCallable,Category="仓库",meta=(DisplayName="添加资源"))
	int32 AddResource(EResourcesType Type, int32 Count); //什么样的资源 EResourcesType 添加多少Count  Type是资源类型 Count是资源数量
	
	//移除资源
	UFUNCTION(BlueprintCallable,Category="仓库",meta=(DisplayName="移除资源"))
	int32 RemoveResource(EResourcesType Type, int32 Count);
	
	//查找资源
	UFUNCTION(BlueprintCallable,Category="仓库",meta=(DisplayName="查找资源"))
	int32 GetResourceCount(EResourcesType Type);
	
	//检查资源是否足够
	UFUNCTION(BlueprintCallable,Category="仓库",meta=(DisplayName="检查资源是否足够"))
	bool HasEnoughResource(EResourcesType Type, int32 Count) const;

	//获取资源配置
	UFUNCTION(BlueprintCallable,Category="仓库",meta=(DisplayName="获取资源类型文本"))
	FString GetResourceTypeText(EResourcesType Type) const;
	
protected:
	virtual void BeginPlay() override;
	//组件TObjectPtr
	//Root根组件
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category = "Root")
	TObjectPtr<USceneComponent>  Root; 
	
	//静态网格体组件
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category = "Root")
	TObjectPtr<UStaticMeshComponent> Mesh;

	//碰撞盒组件
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category = "Box")
	TObjectPtr<UBoxComponent> CollisionBox;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
