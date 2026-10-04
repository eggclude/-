#pragma once

#include "CoreMinimal.h"
#include "PlantBedTypes.generated.h"


#define GROWTH_SPEED_SALINE		0.0F
#define GROWTH_SPEED_POOR		0.5F
#define GROWTH_SPEED_NORMAL		1.0F
#define GROWTH_SPEED_FERTILE	1.5F

//种植床默认值 阈值类
namespace plantBedDefaults
{
	static constexpr float DEFAULT_SOIL_FERTILITY					= 60.0f;	//默认土壤肥力
	static constexpr float MAX_SOIL_FERTILITY						= 100.0f;	//最大土壤肥力
	static constexpr float FERTILITY_POOR_THRESHOLD					= 30.0f;	//土壤肥力贫瘠阈值，低于该值为贫瘠
	static constexpr float FERTILITY_FERTILE_THRESHOLD				= 70.0f;	//土壤肥沃阈值，高于该值为土壤肥沃	
	static constexpr float FERTILITY_SALINE_THRESHOLD				= 10.0f;	//土壤肥力盐碱地阈值，低于该值为盐碱地
	
	static constexpr float FERTILITY_LOSS_PER_RADIATION_LEVEL		= 0.01f;	//每单位辐射等级的乘数,土壤肥力损失量
	static constexpr float FERTILITY_LOSS_PER_SECOND				= 0.01f;	//土壤肥力自然损失量	
	
	static constexpr float MOISTURE_LOSS_PER_SECOND					= 0.005f;	//归一化，土壤湿度自然流失量
	static constexpr float MOISTURE_SALINE_LOSS_MULTI				= 1.5f;		//乘数，盐碱地土壤湿度自然流失量
	static constexpr float MOISTURE_POOR_LOSS_MULTI					= 2.0f;		//乘数，贫瘠地土壤湿度自然流失量
	static constexpr float MOISTURE_NORMAL_LOSS_MULTI				= 1.0f; 	//乘数，正常土壤湿度自然流失量
	static constexpr float MOISTURE_FERTILE_LOSS_MULTI				= 0.5f;		//乘数，肥沃土壤湿度自然流失量
	static constexpr float FERTILITY_RECOVER_REC_SECOND				= 0.2f;		//无辐射时，土地肥力自愈速率（每秒）
	
}

UENUM(BlueprintType)
enum class EsoilQuality: uint8 
{
	Saline UMETA(DisplayName=	"土壤状态:盐碱地"),
	poor   UMETA(DisplayName=	"土壤状态：贫瘠"),
	Normal UMETA(DisplayName=	"土壤状态：正常"),
	Fertlie UMETA(DisplayName=	"土壤状态：肥沃"),
};
