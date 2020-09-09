// Fill out your copyright notice in the Description page of Project Settings.

#include "NavigationNode.h"

#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ANavigationNode::ANavigationNode()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LocationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Location Component"));
	RootComponent = LocationComponent;

	bIsTraversible = true;
	
	CircleRadius = 50.0f;
	SegmentCount = 32;

	FinalHeight = 250.0f;
	FinalWidth = 250.0f;

	bIsTraversible = FMath::FRand()>0.1f;
	
	BlockerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Blocker Mesh"));
	BlockerMesh->SetStaticMesh(nullptr);
	BlockerMesh->CastShadow=0;
	BlockerMesh->SetupAttachment(RootComponent);
	BlockerMesh->SetRelativeLocation(FVector(0,0,100));
	BlockerMesh->SetRelativeScale3D(FVector(FinalWidth/130,FinalHeight/130,1));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("StaticMesh'/Engine/EngineMeshes/Cube.Cube'"));
	CubeMesh = Mesh.Object;

	
	
	
}

// Called when the game starts or when spawned
void ANavigationNode::BeginPlay()
{
	Super::BeginPlay();

	BlockNonTraversable();

	for (auto It = ConnectedNodes.CreateConstIterator(); It; ++It)
	{
		if(bIsTraversible)
		{
			DrawDebugLine(GetWorld(), GetActorLocation(), (*It)->GetActorLocation(), FColor::Blue, true,-1,0,1);
		}
	}
	
	for (auto It = ConnectedNonTraversableNodes.CreateConstIterator(); It; ++It)
	{
		DrawDebugLine(GetWorld(), GetActorLocation(), (*It)->GetActorLocation(), FColor::Red, true,-1,0,1);
	}

	if(bIsTraversible==false)
	{
		DrawDebugCircle(GetWorld(),GetActorLocation(),CircleRadius,SegmentCount,FColor::Red,true,-1,0,3,FVector(0,1,0),FVector(1,0,0),false);
	}
	else
	{
		DrawDebugCircle(GetWorld(),GetActorLocation(),CircleRadius,SegmentCount,FColor::Blue,true,-1,0,3,FVector(0,1,0),FVector(1,0,0),false);
	}
	
}

// Called every frame
void ANavigationNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ANavigationNode::FScore()
{
	return GScore + HScore;
}

void ANavigationNode::BlockNonTraversable()
{

	if(bIsTraversible)
	{
		BlockerMesh->SetStaticMesh(nullptr);
		return;
	}
	
	if(bIsTraversible==false)
	{
		BlockerMesh->SetStaticMesh(CubeMesh);
	}
}

