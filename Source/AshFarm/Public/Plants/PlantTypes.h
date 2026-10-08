#pragma once

#include "CoreMinimal.h"

namespace PlantDefaults
{
	//植物品质阈值
	const float QUALITY_PREMIUM_THRESHOLD = 5.0f;		//优质阈值
	const float QUALITY_NORMAL_THRESHOLD = 15.0f;		//普通阈值
	const float QUALITY_WITHERED_THRESHOLD = 30.0f;		//干瘪阈值
	
}




//作物生长阶段{幼苗期, 生长期, 开花期, 成熟期}
UENUM(BlueprintType)
enum class EGrowthStage : uint8
{
	Seeding UMETA(DisplayName="播种期"),
	Growing UMETA(DisplayName="生长期"),
	Flowering UMETA(DisplayName="开花期"),
	Mature UMETA(DisplayName="成熟期"),
};
//植物品质
UENUM(BlueprintType)
enum class EPlantQuality: uint8 
{
	Premium		UMETA(DisplayName="优质"),
	Normal		UMETA(DisplayName="普通"),
	Withered	UMETA(DisplayName="干瘪"),
};

