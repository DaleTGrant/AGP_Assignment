// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavigationNode.h"
#include "AIManager.generated.h"

class AEnemyCharacter;

UENUM()
enum class EHeuristicType : uint8 
{
	EUCLIDEAN,
    OCTILE,
    CHEBYSHEV
};

UENUM()
enum class EPathfindingType : uint8 
{
	A_STAR,
    JPS
};

UCLASS()
class ADVGAMESPROGRAMMING_API AAIManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAIManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category="Pathfinding Properties")
	EHeuristicType Heuristic;
	UPROPERTY(EditAnywhere, Category="Pathfinding Properties")
	EPathfindingType Pathfinding;
	UPROPERTY(EditAnywhere, Category="AI Properties")
	int32 NumAI;
	UPROPERTY(EditAnywhere, Category="AI Properties")
	bool bIsRandomSpawn;
	UPROPERTY(EditAnywhere, Category="AI Properties")
	int32 SpawnIndex;
	UPROPERTY(VisibleAnywhere, Category = "Navigation Nodes")
	TArray<ANavigationNode*> AllNodes;
	UPROPERTY(VisibleAnywhere, Category = "Navigation Nodes")
	TArray<ANavigationNode*> AllTraversableNodes;
	UPROPERTY(EditAnywhere, Category = "Navigation Nodes")
	bool bSteepnessPreventConnection;
	UPROPERTY(EditAnywhere, Category = "Navigation Nodes")
	bool bIsRandomSpawnBlockers;
	UPROPERTY(EditAnywhere, Category = "Navigation Nodes")
	float RandomSpawnBlockerRate;
	UPROPERTY(VisibleAnywhere, Category = "Agents")
	TArray<AEnemyCharacter*> AllAgents;
	UPROPERTY(EditAnywhere, Category = "Agents")
	TSubclassOf<AEnemyCharacter> AgentToSpawn;
	UPROPERTY(EditAnywhere, Category = "Navigation Nodes")
	float GridWidth;
	UPROPERTY(EditAnywhere, Category = "Navigation Nodes")
	float GridHeight;

	TArray<ANavigationNode*> GeneratePath(ANavigationNode* StartNode, ANavigationNode* EndNode);

	TArray<ANavigationNode*> GenerateJPSPath(ANavigationNode* StartNode, ANavigationNode* EndNode);
	TArray<ANavigationNode*> GenerateAStarPath(ANavigationNode* StartNode, ANavigationNode* EndNode);
	float CalculateHeuristic(FVector CurrentLocation, FVector GoalLocation);

	TArray<ANavigationNode*> SearchHorizontal(ANavigationNode* ParentNode,ANavigationNode* EndNode,float HorDir, float Dist);
	TArray<ANavigationNode*> SearchVertical(ANavigationNode* ParentNode,ANavigationNode* EndNode,float VertDir, float Dist);
	TArray<ANavigationNode*> SearchDiagonal(ANavigationNode* ParentNode,ANavigationNode* EndNode,float HorDir, float VertDir, float Dist);
	TArray<ANavigationNode*> PruneNeighbours(ANavigationNode* CurrentNode, FVector2D ParentDir);
	TArray<ANavigationNode*> ForcedNeighbours(ANavigationNode* CurrentNode, FVector2D ParentDir);

	/**
	Finds the nearest navigation node from the given location.
	@param Location - The location that you want to find the nearest node from.
	@return NearestNode - The nearest node to the given location.
	*/
	ANavigationNode* FindNearestNode(const FVector& Location);
	/**
	Finds the furthest navigation node from the given location.
	@param Location - The location that you want to find the furthest node from.
	@return FurthestNode - The furthest node from the given location.
	*/
	ANavigationNode* FindFurthestNode(const FVector& Location);

	void PopulateNodes();
	void CreateAgents();

	void GenerateNodes(const TArray<FVector>& Vertices, int32 Width, int32 Height);
	void AddConnection(ANavigationNode* FromNode, ANavigationNode* ToNode);
	UPROPERTY(EditAnywhere)
	float AllowedAngle;
};
