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
	//Ensure Mesh组件存在
	ensure(Mesh != nullptr);
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
//取水 Takewater
float AHandPump::TakeWater(float WaterAmount)
{
	//先检查以下几种情况
	//1.请求不合理(WaterAmount <= 0.0f)
	if (WaterAmount <= 0.0F)
	{
		UE_LOG(LogTemp,Warning,TEXT("手压井ID: %s,取水请求不合理"),*DeviceID.ToString());
		return 0.0f;
	}
	//2.设备坏了（bIsBroken == true）
	if (bIsBroken == true)
	{
		UE_LOG(LogTemp,Warning,TEXT("手压井ID: %s,设备坏了,不能取水"),*DeviceID.ToString());
		return 0.0f;
	}
	//4.当前水位为0（CurrentWater == 0.0f）
	if (CurrentWater == 0.0f)
	{
		UE_LOG(LogTemp,Warning,TEXT("手压井ID: %s,当前水位为0,不能取水"),*DeviceID.ToString());
		return 0.0f;
	}
	//3.水位不足（CurrentWater < WaterAmount）
	float FinalWaterAmount = FMath::Min(CurrentWater,WaterAmount);
	CurrentWater -= FinalWaterAmount;
	
	UE_LOG(LogTemp,Warning,TEXT("手压井ID:%s,取水请求%.2f,实际取水%.2f"),*DeviceID.ToString(),WaterAmount,FinalWaterAmount);
	
	//同时打印到屏幕
	GEngine->AddOnScreenDebugMessage(-1,5.0F,FColor::Green, FString::Printf(TEXT("手压井ID:%s,取水请求%.2f,实际取水%.2f"),*DeviceID.ToString(),WaterAmount,FinalWaterAmount));
	 
	return WaterAmount;
	
}

//泵水 PumpWater
float AHandPump::PumpWater()
{	
	// 检查手压井是否损坏 和 水位是否已满最大水位	
	if (!bIsBroken && CurrentWater < MaxWater)//!bIsBroken 取反
	{
		
		float lastWater = CurrentWater;
		
		CurrentWater = FMath::Clamp(CurrentWater + AddWaterPerTime,0.0F,MaxWater);
		
		//耐久度损耗
		Durabiliity = FMath::Clamp(Durabiliity - DurabiliityPumpLossPerPump,0.0F,MaxDurability);
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
	//检查最大水位是否为0
	ensure(MaxWater > 0.0f);
	if (MaxWater == 0.0f)
	{
		return 0.0f;
	}
	return CurrentWater/MaxWater;
}

//获取当前耐久度占比
float AHandPump::GetDurabiliityPercentage() const
{
	//检查最大耐久度是否为0
	ensure(MaxDurability > 0.0f);
	if (MaxDurability == 0.0f)
	{
		return 0.0f;
	}
	return Durabiliity/MaxDurability;
}
//修复手压井
void AHandPump::Repair()
{
	if (bIsBroken)
	{
		//检查当前水位是否足够修复
		if (CurrentWater >= HandPump::MIN_WATER_FOR_REPAIR)
		{
			//当前水位-最小水位保证水位不小于0
			CurrentWater -= HandPump::MIN_WATER_FOR_REPAIR;
			//当前水位=当前水位-保证水位不大于最大水位
			CurrentWater = FMath::Clamp(CurrentWater,0.0f,MaxWater);
			
			//todo:需要耗材修复
			
			//修复耐久度
			Durabiliity = MaxDurability * HandPump::PEPAIR_RESTORE_PERCENT;
			
			//bIsBroken 修复后设置为false，表示已修复
			bIsBroken = false;
			//输出修复成功信息
			UE_LOG(LogTemp,Warning,TEXT("手压井ID: %s,已修复"),*DeviceID.ToString());
		}
		//当前水位不足修复
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("手压井ID: %s,当前水位:%2f,无法修复"),*DeviceID.ToString(),CurrentWater);
		}
	}	
}


