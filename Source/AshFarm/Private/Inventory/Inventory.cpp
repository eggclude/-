// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Inventory.h"
#include "AshFarm.h"	


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
	
	PrintResourceInventory();//打印资源库存
}
//获取资源配置
FString AInventory::GetResourceTypeText(EResourcesType Type) const
{
	switch (Type)//获取里的所有资源配置类型
	{
		case EResourcesType::Water:
			return TEXT("水");
		case EResourcesType::Wood:
			return TEXT("木材");
		case EResourcesType::Soil:
			return TEXT("土壤");
		case EResourcesType::Fertilizer:
			return TEXT("肥料");
		case EResourcesType::Waste:
			return TEXT("垃圾");
		case EResourcesType::Gold:
			return TEXT("黄金");
		case EResourcesType::Tool:
			return TEXT("工具");
		case EResourcesType::Plant:
			return TEXT("种子");
		default:
			return TEXT("未知资源");
		break;
	}
}
//添加资源
int32 AInventory::AddResource(EResourcesType Type, int32 Count)
{
	if (Count == 0)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("AddResource:添加资源数量必须大于0"));
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
	//检查类型资源是否存在 返回0 中断函数
	if (!Config)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("AddResource:添加%s资源类型不存在"), *GetResourceTypeText(Type));
		return 0;
	}
	// 为了满足 CurrentCount + Count <= Config->MaxCapacity;
	//首先要获取CurrentCount这个Type类型的当前数量
	//难点2  ResourcesInventory.Find(Type) 他其实是通过Type键来查找值 
	//因为前面定义的Tmap是EResourcesType, int32 前面是键 其实就是整数 通过这个整数来寻找到我们要的资源数量 后面是值 返回的其实是指针
	//如果返回没问题能找到的话就将指针的数值提取出来也就是*CurrentCount 
	// *CurrentCount指向当前数量的指针 从指针解引用 就是获取当前值
	//其实也是查询资源获取数量
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
		UE_LOG(A_LogAshFarm, Warning, TEXT("AddResource:添加资源数量已超过最大容量"));
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
		UE_LOG(A_LogAshFarm, Warning, TEXT("仓库资源%s] 移除数量必须大于0"), *GetResourceTypeText(Type));
		return Count;
	}
	//查询一下库存数量
	const int32* CurrentCount = ResourcesInventory.Find(Type);
	if (!CurrentCount || *CurrentCount < 0)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("仓库资源%s] 不存在"), *GetResourceTypeText(Type));
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
//通过Type键来查询资源数量 保存在int32* CurrentCount的地址上 然后判断是否存在 如果存在 就返回*CurrentCount的数值 如果不存在 就返回0
int32 AInventory::GetResourceCount(EResourcesType Type)
{
	const int32* CurrentCount = ResourcesInventory.Find(Type);
	return CurrentCount ? *CurrentCount : 0;
}
//查询资源数量是否足够
bool AInventory::HasEnoughResource(EResourcesType Type, int32 Count) const
{
	const int32* CurrentCount = ResourcesInventory.Find(Type);
	return CurrentCount ? *CurrentCount >= Count : false;
}

//打印库存
void AInventory::PrintResourceInventory()
{
	FString PrintString = TEXT("=========仓库资源库存=========\n");
	
	
	if (ResourcesInventory.IsEmpty())
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("\n仓库资源库存为空"));
		return;
	} 
	//& 是引用 会改变原始数据 不会改变原始数据的地址
	for (const auto& Pair : ResourcesInventory)
	{
		//PrintString += FString::Printf(TEXT("%s: %d\n"), 是打印资源类型和数量的格式
		//*GetResourceTypeText(Pair.Key)  拿到了Pair.Key的资源类型文本然后转换成类型的中文名然后由%s 然后Pair.Value是资源数量 %d
		PrintString += FString::Printf(TEXT("\n%s: %d"), *GetResourceTypeText(Pair.Key), Pair.Value);
	}
	FVector TextLocation = GetActorLocation()+FVector(0,0,200.0f);
	
	DrawDebugString(
		GetWorld(), 
		TextLocation,
		PrintString,
		nullptr,//不绘制文本
		FColor::White,//是否死亡 死亡返回红色 正常返回白色
		0.5f//显示时间间隔为0.5秒
		);
	
}
//批量添加资源
void AInventory::AddMultipleResources(const TArray<FResourceBundle>& Bundles)
{
	//首先遍历TArray<FResourceBundle>& Bundles的 Bundles资源包数组 对每个元素调用AddResource函数
	for (const auto& Bundle : Bundles)
	{
		if(Bundle.Count <=0)
		{
			UE_LOG(A_LogAshFarm,Warning,TEXT("AddResource:添加资源数量必须大于0"));
			continue;//continue是跳过当前循环 继续下一次循环
		}
		AddResource(Bundle.Type, Bundle.Count);
	}
	
}
//解锁种子类型 集合不会添加重复元素
void AInventory::UnlockPlantType(TSubclassOf<UPlantBase>PlantType)
{
	//种子类型为空 → 直接返回
	if (!PlantType)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("UnlockPlantType:解锁的种子类型不存在为空"));
		return;
	}

	//已经解锁过 → 不重复添加，直接返回
	if (PlantResources.Contains(PlantType))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("该种子类型已解锁：%s"), *PlantType->GetName());
		return;
	}

	//首次解锁 → 加入集合并打日志
	PlantResources.Add(PlantType);
	UE_LOG(A_LogAshFarm, Warning, TEXT("解锁种子类型成功：%s"), *PlantType->GetName());
}

//获取已解锁的种子类型
TArray<TSubclassOf<UPlantBase>> AInventory::GetUnlockedPlantTypes()
{
	//TSet 转成 TArray 返回（蓝图里更好遍历）
	return PlantResources.Array();
}

bool AInventory::IsPlantTypeUnlocked(TSubclassOf<UPlantBase>PlantType) const
{
	return PlantResources.Contains(PlantType);
}






//查看容量上限
int32 AInventory::GetResourceCapacity(EResourcesType Type) const
{
	//- 在 ResourcesConfigs 里查找对应 Type 的配置。
	const FResourceConfig* Config = ResourcesConfigs.FindByPredicate(
		[Type](const FResourceConfig& cfg)
		{
			return cfg.Type == Type;
		}
		);
		//如果找不到配置，返回 0，并打印 Warning 日志
		if (!Config)
		{
			UE_LOG(A_LogAshFarm, Warning, TEXT("仓库资源%s] 不存在"), *GetResourceTypeText(Type));
			return 0;
		}
		//- 返回 MaxCapacity。
		return  Config->MaxCapacity;
}

