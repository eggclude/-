// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/HandPump.h"

// Sets default values
AHandPump::AHandPump()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));//创建Mesh的对象
	
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (MeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(MeshAsset.Object.Get());
	}
	Mesh->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AHandPump::BeginPlay()
{
	Super::BeginPlay();
	
	//检查最大水位和最大耐久度是否为0
	ensure(MaxWater > 0.0f);
	ensure(MaxDurability > 0.0f);
}


void AHandPump::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
// 检查手压井是否损坏
bool AHandPump::IsBroken() const
{
	return bIsBroken;
}

//是否在泵水
bool AHandPump::IsPumping() const
{
	return bIsPumping;
}
//泵水 PumpWater
float AHandPump::PumpWater()
{	
	// 检查手压井是否损坏 和 水位是否已满最大水位	
	if (!bIsBroken && CurrentWater < MaxWater)//!bIsBroken 取反
	{
		
		float lastWater = CurrentWater;
		
		CurrentWater = FMath::Clamp(CurrentWater + AddWaterPerpress,0.0F,MaxWater);
		
		//耐久度损耗
		Durabiliity = FMath::Clamp(Durabiliity - DurabiliityPumpLossPerPump,0.0F,MaxDurabiliity);
		//检查手压井是否低于危险阈值
		if (Durabiliity <= DurabiliityCriticalThreshold)
		{
			UE_LOG(LogTemp,Warning,TEXT("手压井ID: %s,目前耐久度:%2f,已低于危险阈值%f"),*DeviceID.ToString(),Durabiliity,DurabiliityCriticalThreshold);
		}
		if (Durabiliity <= 0.0F)
		{
			UE_LOG(LogTemp,Warning,TEXT("手压井ID: %s,目前耐久度:%2f,已损坏"),*DeviceID.ToString(),Durabiliity);
			bIsBroken = true;
		}
		
		
		//Tchar* : 字符串指针, *DeviceID.ToString() : 字符串指针, CurrentWater : 指向一个TCHAR类型的变量
		GEngine->AddOnScreenDebugMessage(-1,5.0F,FColor::Green, FString::Printf(TEXT("手压井ID: %s,当前水位：%.2f"),*DeviceID.ToString(),CurrentWater));
		
		return CurrentWater-lastWater;
	}
	else
	{
		return 0.0f;
	}
}

//获取当前水位占比
float AHandPump::GetWaterPercentage() const
{
	if (MaxWater == 0.0f)
	{
		return 0.0f;
	}
	return CurrentWater/MaxWater;
}

//获取当前耐久度占比
float AHandPump::GetDurabiliityPercentage() const
{
	if (MaxDurability == 0.0f)
	{
		return 0.0f;
	}
	return Durabiliity/MaxDurability;
}