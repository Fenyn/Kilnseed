#include "Items/CarriableBase.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"

#if WITH_EDITOR
#include "Materials/Material.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialExpressionFresnel.h"
#include "Materials/MaterialExpressionOneMinus.h"
#include "Materials/MaterialExpressionMultiply.h"
#endif

ACarriableBase::ACarriableBase()
{
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetWorldScale3D(FVector(0.2f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(CubeMesh.Object);
	}
}

static UMaterial* GetGreyboxColorMaterial()
{
	static TWeakObjectPtr<UMaterial> CachedMat;
	if (CachedMat.IsValid()) return CachedMat.Get();

#if WITH_EDITOR
	UMaterial* Mat = NewObject<UMaterial>(GetTransientPackage(), TEXT("M_GreyboxColor"), RF_Transient);
	Mat->SetShadingModel(EMaterialShadingModel::MSM_Unlit);

	// Color parameter
	auto* ColorParam = NewObject<UMaterialExpressionVectorParameter>(Mat);
	ColorParam->ParameterName = TEXT("Color");
	ColorParam->DefaultValue = FLinearColor::White;
	Mat->GetExpressionCollection().AddExpression(ColorParam);

	// Fresnel: bright edges → 1, center → 0
	auto* Fresnel = NewObject<UMaterialExpressionFresnel>(Mat);
	Fresnel->Exponent = 1.5f;
	Fresnel->BaseReflectFraction = 0.0f;
	Mat->GetExpressionCollection().AddExpression(Fresnel);

	// Invert: center → 1, edges → 0
	auto* OneMinus = NewObject<UMaterialExpressionOneMinus>(Mat);
	OneMinus->Input.Connect(0, Fresnel);
	Mat->GetExpressionCollection().AddExpression(OneMinus);

	// Color * (1 - Fresnel): bright center, dark edges
	auto* Multiply = NewObject<UMaterialExpressionMultiply>(Mat);
	Multiply->A.Connect(0, ColorParam);
	Multiply->B.Connect(0, OneMinus);
	Mat->GetExpressionCollection().AddExpression(Multiply);

	Mat->GetEditorOnlyData()->EmissiveColor.Connect(0, Multiply);
	Mat->PostEditChange();

	Mat->AddToRoot();
	CachedMat = Mat;
	return Mat;
#else
	return UMaterial::GetDefaultMaterial(MD_Surface);
#endif
}

UMaterialInstanceDynamic* ACarriableBase::CreateColoredMaterial(UObject* Outer, FLinearColor Color)
{
	UMaterial* BaseMat = GetGreyboxColorMaterial();
	if (!BaseMat) return nullptr;

	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(BaseMat, Outer);
	MID->SetVectorParameterValue(TEXT("Color"), Color);
	return MID;
}

void ACarriableBase::SetItemColor(FLinearColor Color)
{
	if (UMaterialInstanceDynamic* MID = CreateColoredMaterial(this, Color))
	{
		MeshComponent->SetMaterial(0, MID);
	}
}

void ACarriableBase::SetCarried(bool bNewCarried)
{
	bCarried = bNewCarried;
	MeshComponent->SetSimulatePhysics(!bNewCarried);
	MeshComponent->SetCollisionEnabled(bNewCarried ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
}

void ACarriableBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACarriableBase, ItemType);
	DOREPLIFETIME(ACarriableBase, PlantType);
	DOREPLIFETIME(ACarriableBase, ItemColor);
	DOREPLIFETIME(ACarriableBase, bCarried);
}

void ACarriableBase::OnRep_bCarried()
{
	MeshComponent->SetSimulatePhysics(!bCarried);
	MeshComponent->SetCollisionEnabled(bCarried ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
}
