// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Inventory.generated.h"

UCLASS(Blueprintable,ClassGroup="AshFarm | 库存系统")
class ASHFARM_API AInventory : public AActor //AInventory是AActor的子类,用于表示游戏中的仓库
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInventory();//构造函数
protected:
	virtual void BeginPlay() override;
	//组件TObjectPtr
	//Root根组件
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Root")
	TObjectPtr<USceneComponent>  Root; 
	
	//静态网格体组件
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Root")
	TObjectPtr<UStaticMeshComponent> Mesh;

	//碰撞盒组件
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Box")
	TObjectPtr<UBoxComponent> CollisionBox;

	
	
	
	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
