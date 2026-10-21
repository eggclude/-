#pragma once

#include "CoreMinimal.h"
#include "InventoryTypes.generated.h"

//自动写入头文件


UENUM(BlueprintType)//资源类型枚举
enum class EResourcesType : uint8 //EResourcesType 他是资源仓库映射key
{
	//资源类型
	Water UMETA(DisplayName="水"),
	Wood UMETA(DisplayName="木材"),
	Soil UMETA(DisplayName="泥土"),
	Fertilizer UMETA(DisplayName="肥料"),
	Waste UMETA(DisplayName="废料"),
	Gold UMETA(DisplayName="金币"),
	Tool UMETA(DisplayName="工具"),
	Plant UMETA(DisplayName="种子"),
};
//资源配置
USTRUCT(BlueprintType)//资源配置结构体
struct FResourceConfig
{
	GENERATED_BODY()//自动生成构造函数和析构函数
	
public:
	//构造函数
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="资源配置",meta=(DisplayName="资源类型"))
	EResourcesType Type = EResourcesType::Water;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="资源配置",meta=(DisplayName="资源显示名称"))
	FText DisplayName = FText::FromString(TEXT("水")); //FromString是讲文本转为FText类型
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="资源配置",meta=(DisplayName="资源描述"))
	FText Description = FText::FromString(TEXT("水是植物生长的基础,用于植物的生长和生存"));
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="资源配置",meta=(DisplayName="最大容量"))
	int32 MaxCapacity = 20000;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="资源配置",meta=(DisplayName="图标"))
	TSoftObjectPtr<UTexture2D> Icon;
};
//资源包结构体
USTRUCT(BlueprintType)
struct FResourceBundle//资源包结构体
{
	GENERATED_BODY()//自动生成构造函数
	
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "资源包",meta=(DisplayName="资源类型"))
	EResourcesType Type;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "资源包",meta=(DisplayName="资源数量",ClampMin="0"))
	int32 Count;
};