#pragma once

#include "CoreMinimal.h"

//作物生长阶段{幼苗期, 生长期, 开花期, 成熟期}
UENUM(BlueprintType)
enum class EGrowthStage : uint8
{
	Seeding UMETA(DisplayName="播种期"),
	Growing UMETA(DisplayName="生长期"),
	Flowering UMETA(DisplayName="开花期"),
	Mature UMETA(DisplayName="成熟期"),
};