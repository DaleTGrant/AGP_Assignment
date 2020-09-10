// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "NavigationNode.generated.h"

UCLASS()
class ADVGAMESPROGRAMMING_API ANavigationNode : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANavigationNode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Connected Nodes")
	TArray<ANavigationNode*> ConnectedNodes;
	UPROPERTY(EditAnywhere, Category = "Connected Nodes")
	TArray<ANavigationNode*> ConnectedNonTraversableNodes;
	UPROPERTY(EditAnywhere, Category = "Connected Nodes")
	TArray<ANavigationNode*> AllConnectedNodes;
	UPROPERTY(EditAnywhere, Category = "Connected Nodes")
	TArray<FVector2D> AllConnectedDir;
	UPROPERTY(EditAnywhere, Category = "Connected Nodes")
	FVector2D GridLocation;
	
	USceneComponent* LocationComponent;

	float GScore;
	float HScore;
	float FScore();

	UPROPERTY(EditAnywhere, Category="Non-Traversible Parameters")
	bool bIsTraversible;
	UPROPERTY(EditAnywhere, Category="Non-Traversible Parameters")
	float CircleRadius;
	UPROPERTY(EditAnywhere, Category="Non-Traversible Parameters")
	int32 SegmentCount;

	UPROPERTY(EditAnywhere, Category="Non-Traversible Parameters")
	UStaticMeshComponent* BlockerMesh;
	
	ANavigationNode* CameFrom;

	void BlockNonTraversable();

	UStaticMesh* CubeMesh;

	UPROPERTY(EditAnywhere)
	float FinalWidth;
	UPROPERTY(EditAnywhere)
	float FinalHeight;
};
