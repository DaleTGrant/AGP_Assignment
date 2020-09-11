// Fill out your copyright notice in the Description page of Project Settings.


#include "AIManager.h"

#include "EngineUtils.h"
#include "EnemyCharacter.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
AAIManager::AAIManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AllowedAngle = 0.4f;
	bSteepnessPreventConnection = true;
	bIsRandomSpawnBlockers = true;
	RandomSpawnBlockerRate = 0.1f;

	Heuristic = EHeuristicType::EUCLIDEAN;
	Pathfinding = EPathfindingType::A_STAR;

	bIsRandomSpawn = true;
	SpawnIndex = 0;
}

// Called when the game starts or when spawned
void AAIManager::BeginPlay()
{
	Super::BeginPlay();
	
	PopulateNodes();
	CreateAgents();
}

// Called every frame
void AAIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<ANavigationNode*> AAIManager::GeneratePath(ANavigationNode* StartNode, ANavigationNode* EndNode)
{
	if(Pathfinding==EPathfindingType::A_STAR)
	{
		return GenerateAStarPath(StartNode,EndNode);
	}

	if(Pathfinding==EPathfindingType::JPS)
	{
		return  GenerateJPSPath(StartNode,EndNode);
	}
	
	UE_LOG(LogTemp, Error, TEXT("NO PATH FOUND"));
	return TArray<ANavigationNode*>();
}

TArray<ANavigationNode*> AAIManager::GenerateJPSPath(ANavigationNode* StartNode, ANavigationNode* EndNode)
{
	TArray<ANavigationNode*> OpenSet;
	
	for (ANavigationNode* Node : AllNodes)
	{
		Node->GScore = TNumericLimits<float>::Max();
	}

	StartNode->GScore = 0;
	StartNode->HScore = CalculateHeuristic(StartNode->GetActorLocation(), EndNode->GetActorLocation());

	// Choose an available direction to begin search
	StartNode->DirectionFromParentToNode = StartNode->ConnectedNodes[0]->GridLocation - StartNode->GridLocation;

	OpenSet.Add(StartNode);
	
	while (OpenSet.Num() > 0)
	{
		// Get node in openset with lowest F-Score, make it current node, remove from open set
		int32 IndexLowestFScore = 0;
		for (int32 i = 1; i < OpenSet.Num(); i++)
		{
			if (OpenSet[i]->FScore() < OpenSet[IndexLowestFScore]->FScore())
			{
				IndexLowestFScore = i;
			}
		}
		ANavigationNode* CurrentNode = OpenSet[IndexLowestFScore];
		OpenSet.Remove(CurrentNode);

		// If end goal reached, back trace to start and return path
		if (CurrentNode == EndNode)
		{
			TArray<ANavigationNode*> Path;
			Path.Push(EndNode);
			CurrentNode = EndNode;
			while (CurrentNode != StartNode)
			{
				CurrentNode = CurrentNode->CameFrom;
				Path.Add(CurrentNode);
			}
			return Path;
		}

		// For the current node, identify the neighbours, see if new G-score better than current one
		TArray<ANavigationNode*> JumpNodes = SearchDiagonal(CurrentNode,EndNode,CurrentNode->DirectionFromParentToNode.X,CurrentNode->DirectionFromParentToNode.Y,CurrentNode->GScore);
		for(ANavigationNode* JumpNode : JumpNodes)
		{
			if(!OpenSet.Contains(JumpNode))
			{
				OpenSet.Add(JumpNode);
			}
		}
	}
	// UE_LOG(LogTemp, Error, TEXT("NO PATH FOUND"));
	return TArray<ANavigationNode*>();
}

// Searches for jump points iteratively horizontally along the grid
TArray<ANavigationNode*> AAIManager::SearchHorizontal(ANavigationNode* ParentNode, ANavigationNode* EndNode, float HorDir, float Dist)
{
	ANavigationNode* StartNode = ParentNode;
	FVector2D ParentDir = FVector2D(HorDir,0);
	ANavigationNode* ExpandingNode = StartNode;
	TArray<ANavigationNode*> JumpPointNodes;
	TArray<FVector2D> JumpPointDirFromParent;

	FVector2D NextLocation = StartNode->GridLocation;

	// if no jump points have been found
	while(JumpPointNodes.Num()==0)
	{
		// Iterate to next node to check
		NextLocation = ParentDir+NextLocation;
		ANavigationNode* Temp = nullptr;
		for(ANavigationNode* node :ExpandingNode->AllConnectedNodes)
		{
			if(node->GridLocation==NextLocation)
			{
				if(node->bIsTraversible)
				{
					Temp = node;
					break;
				}
				// If hit a wall, return empty list
				return TArray<ANavigationNode*>();
			}
		}
		ExpandingNode = Temp;

		// If leave the grid, end iteration
		if(NextLocation.X < 0 || NextLocation.X >=GridWidth)
		{
			return TArray<ANavigationNode*>();
		}
		if(NextLocation.Y < 0 || NextLocation.Y >=GridHeight)
		{
			return TArray<ANavigationNode*>();
		}
		
		// If we reach destination on the next step, we want to travel here, store as jump and break
		if(ExpandingNode==EndNode)
		{
			// If better path, set parent to this jump point
			float ng = ParentNode->GScore + CalculateHeuristic(ParentNode->GetActorLocation(),ExpandingNode->GetActorLocation());
			if(ng < ExpandingNode->GScore)
			{
				ExpandingNode->GScore = ng;
				ExpandingNode->HScore = CalculateHeuristic(ExpandingNode->GetActorLocation(),EndNode->GetActorLocation());
				ExpandingNode->CameFrom = ParentNode;
				ExpandingNode->DirectionFromParentToNode = ParentDir;
			}
			JumpPointNodes.Add(ExpandingNode);
			// JumpPointDirFromParent.Add(ParentDir);
			return JumpPointNodes;
		}

		// Check for forced neighbours around the expanded point
		TArray<ANavigationNode*>ForcedNeighbourList = ForcedNeighbours(ExpandingNode,ParentDir);
		// If some have been found, add them to the jump point list if they are improvements to the existing path
		if(ForcedNeighbourList.Num()>0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Force Neighbour Found"));
			for(ANavigationNode* Jump:ForcedNeighbourList)
			{
				UE_LOG(LogTemp, Warning, TEXT("Forced Neighbour Name: %s"),*Jump->GetName());
				
				float ng = ParentNode->GScore + CalculateHeuristic(ParentNode->GetActorLocation(),ExpandingNode->GetActorLocation());
				if(ng < ExpandingNode->GScore)
				{
					ExpandingNode->GScore = ng;
					ExpandingNode->HScore = CalculateHeuristic(ExpandingNode->GetActorLocation(),EndNode->GetActorLocation());
					ExpandingNode->CameFrom = ParentNode;
					ExpandingNode->DirectionFromParentToNode = Jump->GridLocation - ExpandingNode->GridLocation;

					JumpPointNodes.Add(ExpandingNode);
					JumpPointDirFromParent.Add(Jump->GridLocation - ExpandingNode->GridLocation);
				}
			}
		}

		// If there are forced neighbours, then this node is a jump point
		if(ForcedNeighbourList.Num()>0)
		{
			float ng = ParentNode->GScore + CalculateHeuristic(ParentNode->GetActorLocation(),ExpandingNode->GetActorLocation());
			if(ng < ExpandingNode->GScore)
			{
				ExpandingNode->GScore = ng;
				ExpandingNode->HScore = CalculateHeuristic(ExpandingNode->GetActorLocation(),EndNode->GetActorLocation());
				ExpandingNode->CameFrom = ParentNode;
				ExpandingNode->DirectionFromParentToNode = ParentDir;

				JumpPointNodes.Add(ExpandingNode);
				JumpPointDirFromParent.Add(ParentDir);
			}
			
			return JumpPointNodes;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("End of loop break"));
	return JumpPointNodes;
}

// Same as horizontal search but vertically
TArray<ANavigationNode*> AAIManager::SearchVertical(ANavigationNode* ParentNode, ANavigationNode* EndNode,
	float VertDir, float Dist)
{
	ANavigationNode* StartNode = ParentNode;
	FVector2D ParentDir = FVector2D(0,VertDir);
	ANavigationNode* ExpandingNode = StartNode;
	TArray<ANavigationNode*> JumpPointNodes;
	TArray<FVector2D> JumpPointDirFromParent;

	FVector2D NextLocation = StartNode->GridLocation;
	
	while(JumpPointNodes.Num()==0)
	{
		NextLocation = ParentDir+NextLocation;
		ANavigationNode* Temp = nullptr;
		for(ANavigationNode* node :ExpandingNode->AllConnectedNodes)
		{
			if(node->GridLocation==NextLocation)
			{
				if(node->bIsTraversible)
				{
					Temp = node;
					break;
				}
				// If hit a wall, return empty list
				return TArray<ANavigationNode*>();
			}
		}
		ExpandingNode = Temp;
		
		if(NextLocation.X < 0 || NextLocation.X >=GridWidth)
		{
			return TArray<ANavigationNode*>();
		}
		if(NextLocation.Y < 0 || NextLocation.Y >=GridHeight)
		{
			return TArray<ANavigationNode*>();
		}
		// If we reach destination next step, we want to travel here, store as jump and break
		if(ExpandingNode==EndNode)
		{
			float ng = ParentNode->GScore + CalculateHeuristic(ParentNode->GetActorLocation(),ExpandingNode->GetActorLocation());
			if(ng < ExpandingNode->GScore)
			{
				ExpandingNode->GScore = ng;
				ExpandingNode->HScore = CalculateHeuristic(ExpandingNode->GetActorLocation(),EndNode->GetActorLocation());
				ExpandingNode->CameFrom = ParentNode;
				ExpandingNode->DirectionFromParentToNode = ParentDir;
			}
			JumpPointNodes.Add(ExpandingNode);
			// JumpPointDirFromParent.Add(ParentDir);
			return JumpPointNodes;
		}

		// Check for forced neighbours around the expanded point
		TArray<ANavigationNode*>ForcedNeighbourList = ForcedNeighbours(ExpandingNode,ParentDir);
		if(ForcedNeighbourList.Num()>0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Force Neighbour Found"));
			for(ANavigationNode* Jump:ForcedNeighbourList)
			{
				UE_LOG(LogTemp, Warning, TEXT("Forced Neighbour Name: %s"),*Jump->GetName());
				
				float ng = ParentNode->GScore + CalculateHeuristic(ParentNode->GetActorLocation(),ExpandingNode->GetActorLocation());
				if(ng < ExpandingNode->GScore)
				{
					ExpandingNode->GScore = ng;
					ExpandingNode->HScore = CalculateHeuristic(ExpandingNode->GetActorLocation(),EndNode->GetActorLocation());
					ExpandingNode->CameFrom = ParentNode;
					ExpandingNode->DirectionFromParentToNode = Jump->GridLocation - ExpandingNode->GridLocation;

					JumpPointNodes.Add(ExpandingNode);
					JumpPointDirFromParent.Add(Jump->GridLocation - ExpandingNode->GridLocation);
				}
			}
		}
		
		if(ForcedNeighbourList.Num()>0)
		{
			float ng = ParentNode->GScore + CalculateHeuristic(ParentNode->GetActorLocation(),ExpandingNode->GetActorLocation());
			if(ng < ExpandingNode->GScore)
			{
				ExpandingNode->GScore = ng;
				ExpandingNode->HScore = CalculateHeuristic(ExpandingNode->GetActorLocation(),EndNode->GetActorLocation());
				ExpandingNode->CameFrom = ParentNode;
				ExpandingNode->DirectionFromParentToNode = ParentDir;

				JumpPointNodes.Add(ExpandingNode);
				JumpPointDirFromParent.Add(ParentDir);
			}
			
			return JumpPointNodes;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("End of loop break"));
	return JumpPointNodes;
}

// Search Diagonally for jump points, First it checks if it has a diagonal point to expand to.
// First checks for forced neighbours around new diagonal node
// If it finds any, adds them to jump node list to expand on later
// If none are found, then search horizontally and vertically for more jump points
// If there are forced neighbours, this node is also a jump point itself, so send it to the open list for further expansion
TArray<ANavigationNode*> AAIManager::SearchDiagonal(ANavigationNode* ParentNode, ANavigationNode* EndNode, float HorDir,
	float VertDir, float Dist)
{
	ANavigationNode* StartNode = ParentNode;
	FVector2D ParentDir = FVector2D(HorDir,VertDir);
	ANavigationNode* ExpandingNode = StartNode;
	TArray<ANavigationNode*> JumpPointNodes;
	TArray<FVector2D> JumpPointDirFromParent;

	bool bHorSearchDone = false;
	bool bVertSearchDone = false;
	

	FVector2D NextLocation = StartNode->GridLocation;
	
	while(JumpPointNodes.Num()==0)
	{
		NextLocation = ParentDir+NextLocation;
		ANavigationNode* Temp = nullptr;
		for(ANavigationNode* node :ExpandingNode->AllConnectedNodes)
		{
			if(node->GridLocation==NextLocation)
			{
				if(node->bIsTraversible)
				{
					Temp = node;
					break;
				}
				// If hit a wall, return empty list
				return TArray<ANavigationNode*>();
			}
		}
		ExpandingNode = Temp;
		
		if(NextLocation.X < 0 || NextLocation.X >=GridWidth)
		{
			return TArray<ANavigationNode*>();
		}
		if(NextLocation.Y < 0 || NextLocation.Y >=GridHeight)
		{
			return TArray<ANavigationNode*>();
		}
		// If we reach destination next step, we want to travel here, store as jump and break
		if(ExpandingNode==EndNode)
		{
			float ng = ParentNode->GScore + CalculateHeuristic(ParentNode->GetActorLocation(),ExpandingNode->GetActorLocation());
			if(ng < ExpandingNode->GScore)
			{
				ExpandingNode->GScore = ng;
				ExpandingNode->HScore = CalculateHeuristic(ExpandingNode->GetActorLocation(),EndNode->GetActorLocation());
				ExpandingNode->CameFrom = ParentNode;
				ExpandingNode->DirectionFromParentToNode = ParentDir;
			}
			JumpPointNodes.Add(ExpandingNode);
			// JumpPointDirFromParent.Add(ParentDir);
			return JumpPointNodes;
		}

		// Check for forced neighbours around the expanded point
		TArray<ANavigationNode*>ForcedNeighbourList = ForcedNeighbours(ExpandingNode,ParentDir);
		if(ForcedNeighbourList.Num()>0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Force Neighbour Found"));
			for(ANavigationNode* Jump:ForcedNeighbourList)
			{
				UE_LOG(LogTemp, Warning, TEXT("Forced Neighbour Name: %s"),*Jump->GetName());
				
				float ng = ParentNode->GScore + CalculateHeuristic(ParentNode->GetActorLocation(),ExpandingNode->GetActorLocation());
				if(ng < ExpandingNode->GScore)
				{
					ExpandingNode->GScore = ng;
					ExpandingNode->HScore = CalculateHeuristic(ExpandingNode->GetActorLocation(),EndNode->GetActorLocation());
					ExpandingNode->CameFrom = ParentNode;
					ExpandingNode->DirectionFromParentToNode = Jump->GridLocation - ExpandingNode->GridLocation;

					JumpPointNodes.Add(ExpandingNode);
					JumpPointDirFromParent.Add(Jump->GridLocation - ExpandingNode->GridLocation);
				}
			}
		}
		bHorSearchDone = false;
		bVertSearchDone = false;

		if(JumpPointNodes.Num()==0)
		{
			TArray<ANavigationNode*> SubNodes = SearchHorizontal(ExpandingNode,EndNode,HorDir,Dist);
			bHorSearchDone = true;
			if(SubNodes.Num()>0)
			{
				JumpPointNodes.Add(SubNodes.Last());
			}
		}

		if(JumpPointNodes.Num()==0)
		{
			TArray<ANavigationNode*> SubNodes = SearchVertical(ExpandingNode,EndNode,VertDir,Dist);
			bVertSearchDone = true;
			if(SubNodes.Num()>0)
			{
				JumpPointNodes.Add(SubNodes.Last());
			}
		}
		
		if(ForcedNeighbourList.Num()>0)
		{
			float ng = ParentNode->GScore + CalculateHeuristic(ParentNode->GetActorLocation(),ExpandingNode->GetActorLocation());
			if(ng < ExpandingNode->GScore)
			{
				ExpandingNode->GScore = ng;
				ExpandingNode->HScore = CalculateHeuristic(ExpandingNode->GetActorLocation(),EndNode->GetActorLocation());
				ExpandingNode->CameFrom = ParentNode;
				ExpandingNode->DirectionFromParentToNode = ParentDir;

				JumpPointNodes.Add(ExpandingNode);
				JumpPointDirFromParent.Add(ParentDir);
			}
			
			return JumpPointNodes;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("End of loop break"));
	return JumpPointNodes;
}

// Method prunes the neighbouring nodes around the currently considered node
// returns traversible nodes in direction of travel from parent node (Natural Neighbours)
// i.e. if Parent Dir = (1,1) returns traversible nodes in (1,1), (1,0), (0,1) directions 

TArray<ANavigationNode*> AAIManager::PruneNeighbours(ANavigationNode* CurrentNode, FVector2D ParentDir)
{
	// If no parent direction (start node), then return all traversible neighbours
	if(ParentDir == FVector2D::ZeroVector)
	{
		return CurrentNode->ConnectedNodes;
	}

	TArray<ANavigationNode*> NaturalNeighbours;
	
	for(int i=0;i<(CurrentNode->AllConnectedDir).Num();++i)
	{
		FVector2D ConsideredDir = CurrentNode->AllConnectedDir[i];
		ANavigationNode* ConsideredNode = CurrentNode->AllConnectedNodes[i];
		if(ConsideredNode->bIsTraversible)
		{
			if(ParentDir == ConsideredDir)
			{
				NaturalNeighbours.Add(ConsideredNode);
			}
			else if(ParentDir.X == ConsideredDir.X && ConsideredDir.Y ==0)
			{
				NaturalNeighbours.Add(ConsideredNode);
			}
			else if(ParentDir.Y == ConsideredDir.Y && ConsideredDir.X ==0)
			{
				NaturalNeighbours.Add(ConsideredNode);
			}
		}
	}
	return NaturalNeighbours;
}

// When considering a new node to expand, searches for "Forced Neighbours".
// These are nodes that require pathing through the new node to reach from the parent node
TArray<ANavigationNode*> AAIManager::ForcedNeighbours(ANavigationNode* CurrentNode, FVector2D ParentDir)
{
	TArray<ANavigationNode*> ForcedNeighbours;
	if(CurrentNode->ConnectedNonTraversableNodes.Num()==0)
	{
		return TArray<ANavigationNode*>();
	}
	
	for(int i=0;i<(CurrentNode->AllConnectedDir).Num();++i)
	{
		FVector2D ConsideredDir = CurrentNode->AllConnectedDir[i];
		ANavigationNode* ConsideredNode = CurrentNode->AllConnectedNodes[i];

		// If parent from diagonal
		if(abs(ParentDir.X)==abs(ParentDir.Y))
		{
			if(ConsideredDir==FVector2D(-ParentDir.X,0)||ConsideredDir==FVector2D(0,-ParentDir.Y))
			{
				if(!ConsideredNode->bIsTraversible)
				{
					for(int j=0;j<(CurrentNode->AllConnectedDir).Num();++j)
					{
						FVector2D PossibleForcedNodeDir = CurrentNode->AllConnectedDir[j];
						if(PossibleForcedNodeDir == ParentDir+ 2*ConsideredDir)
						{
							 ANavigationNode* PossibleForcedNode = CurrentNode->AllConnectedNodes[j];
							if(PossibleForcedNode->bIsTraversible)
							{
								ForcedNeighbours.Add(PossibleForcedNode);
							}
						}
					}
				}
			}	
		}
		// If parent from horizontal
		if(abs(ParentDir.X)==1 && ParentDir.Y== 0)
		{
			if((ConsideredDir.X==0) && ConsideredDir.Y !=0)
			{
				if(!ConsideredNode->bIsTraversible)
				{
					for(int j=0;j<(CurrentNode->AllConnectedDir).Num();++j)
					{
						FVector2D PossibleForcedNodeDir = CurrentNode->AllConnectedDir[j];
						if(PossibleForcedNodeDir == ParentDir + ConsideredDir)
						{
							ANavigationNode* PossibleForcedNode = CurrentNode->AllConnectedNodes[j];
							if(PossibleForcedNode->bIsTraversible)
							{
								ForcedNeighbours.Add(PossibleForcedNode);
							}
						}
					}
				}
			}	
		}
		// If parent is from vertical direction
		if(ParentDir.X==0 && abs(ParentDir.Y)== 1)
		{
			if(ConsideredDir.Y==0 && ConsideredDir.X !=0)
			{
				if(!ConsideredNode->bIsTraversible)
				{
					for(int j=0;j<(CurrentNode->AllConnectedDir).Num();++j)
					{
						FVector2D PossibleForcedNodeDir = CurrentNode->AllConnectedDir[j];
						if(PossibleForcedNodeDir == ParentDir + ConsideredDir)
						{
							ANavigationNode* PossibleForcedNode = CurrentNode->AllConnectedNodes[j];
							if(PossibleForcedNode->bIsTraversible)
							{
								ForcedNeighbours.Add(PossibleForcedNode);
							}
						}
					}
				}
			}	
		}
	}
	return ForcedNeighbours;
}

TArray<ANavigationNode*> AAIManager::GenerateAStarPath(ANavigationNode* StartNode, ANavigationNode* EndNode)
{
	TArray<ANavigationNode*> OpenSet;
	for (ANavigationNode* Node : AllNodes)
	{
		Node->GScore = TNumericLimits<float>::Max();
	}

	StartNode->GScore = 0;
	StartNode->HScore = CalculateHeuristic(StartNode->GetActorLocation(), EndNode->GetActorLocation());

	OpenSet.Add(StartNode);

	while (OpenSet.Num() > 0)
	{
		int32 IndexLowestFScore = 0;
		for (int32 i = 1; i < OpenSet.Num(); i++)
		{
			if (OpenSet[i]->FScore() < OpenSet[IndexLowestFScore]->FScore())
			{
				IndexLowestFScore = i;
			}
		}
		ANavigationNode* CurrentNode = OpenSet[IndexLowestFScore];

		OpenSet.Remove(CurrentNode);

		if (CurrentNode == EndNode) {
			TArray<ANavigationNode*> Path;
			Path.Push(EndNode);
			CurrentNode = EndNode;
			while (CurrentNode != StartNode)
			{
				CurrentNode = CurrentNode->CameFrom;
				Path.Add(CurrentNode);
			}
			return Path;
		}

		for (ANavigationNode* ConnectedNode : CurrentNode->ConnectedNodes)
		{
			float TentativeGScore = CurrentNode->GScore + CalculateHeuristic(CurrentNode->GetActorLocation(), ConnectedNode->GetActorLocation());
			if (TentativeGScore < ConnectedNode->GScore)
			{
				ConnectedNode->CameFrom = CurrentNode;
				ConnectedNode->GScore = TentativeGScore;
				ConnectedNode->HScore = CalculateHeuristic(ConnectedNode->GetActorLocation(),EndNode->GetActorLocation());
				if (!OpenSet.Contains(ConnectedNode))
				{
					OpenSet.Add(ConnectedNode);
				}
			}
		}
	}

	//If it leaves this loop without finding the end node then return an empty path.
	UE_LOG(LogTemp, Error, TEXT("NO PATH FOUND"));
	return TArray<ANavigationNode*>();
}

float AAIManager::CalculateHeuristic(FVector CurrentNodeLocation, FVector GoalNodeLocation)
{
	// Finds the H-Score value between two nodes based on a chosen Heuristic
	// Euclidean is straightline distance (use for if not on grid)
	// Octile and Chebyshev are 8-direction grid steps between the nodes (Octile Diagonal cost = sqrt(2), Cheby = 1)
	if(Heuristic==EHeuristicType::EUCLIDEAN)
	{
		return FVector::Distance(CurrentNodeLocation, GoalNodeLocation);
	}

	if(Heuristic == EHeuristicType::OCTILE || Heuristic == EHeuristicType::CHEBYSHEV)
	{
		float D1 = 1.0f; // Horizontal/Vertical Cost
		float D2 = Heuristic == EHeuristicType::OCTILE ? FMath::Sqrt(2.0f):1.0f; // Diagonal Move cost
		float Dx = abs(GoalNodeLocation.X - CurrentNodeLocation.X);
		float Dy = abs(GoalNodeLocation.Y - CurrentNodeLocation.Y);
		return D1*(Dx+Dy) + (D2-2*D1)*FMath::Min(Dx,Dy); 
	}

	UE_LOG(LogTemp, Error, TEXT("No Heuristic Set"));
	return TNumericLimits<float>::Max();
}

void AAIManager::PopulateNodes()
{
	AllNodes.Empty();
	AllTraversableNodes.Empty();

	for (TActorIterator<ANavigationNode> It(GetWorld()); It; ++It)
	{
		AllNodes.Add(*It);
		if((*It)->bIsTraversible)
		{
			AllTraversableNodes.Add(*It);
		}
	}
}

void AAIManager::CreateAgents()
{
	for (int32 i = 0; i < NumAI; i++)
	{
		int32 RandIndex = SpawnIndex;
		if(bIsRandomSpawn)
		{
			RandIndex = FMath::RandRange(0, AllTraversableNodes.Num()-1);
		}
		AEnemyCharacter* Agent = GetWorld()->SpawnActor<AEnemyCharacter>(AgentToSpawn, AllTraversableNodes[RandIndex]->GetActorLocation(), FRotator(0.f, 0.f, 0.f));
		Agent->Manager = this;
		Agent->CurrentNode = AllTraversableNodes[RandIndex];
		AllAgents.Add(Agent);
	}
}

void AAIManager::GenerateNodes(const TArray<FVector>& Vertices, int32 Width, int32 Height)
{
	AllNodes.Empty();
	AllTraversableNodes.Empty();

	for (TActorIterator<ANavigationNode> It(GetWorld()); It; ++It)
	{
		It->Destroy();
	}

	GridWidth = (float)Width;
	GridHeight = (float)Height;

	for (int32 Col = 0; Col < Width; Col++)
	{
		for (int32 Row = 0; Row < Height; Row++)
		{
			//Create and add the nodes to the AllNodes array.
			ANavigationNode* Node = GetWorld()->SpawnActor<ANavigationNode>(Vertices[Row * Width + Col], FRotator::ZeroRotator, FActorSpawnParameters());
			AllNodes.Add(Node);
			Node->GridLocation = FVector2D(Col,Row);
			if(bIsRandomSpawnBlockers)
			{
				Node->bIsTraversible = FMath::FRand()>RandomSpawnBlockerRate;
			}
			if(Col==0 ||Col ==Width-1)
			{
				Node->bIsTraversible = false;
			}
			if(Row==0 ||Row ==Width-1)
			{
				Node->bIsTraversible = false;
			}
			
			if(Node->bIsTraversible)
			{
				AllTraversableNodes.Add(Node);
			}
		}
		
	}

	for (int32 Col = 0; Col < Width; Col++)
	{
		for (int32 Row = 0; Row < Height; Row++)
		{
			//Add the connections.

			// CORNER CASES:
			if (Row == 0 && Col == 0)
			{
				//   - Bottom Corner where Row = 0 and Col = 0
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + Col]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + (Col + 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col + 1)]);
			}
			else if (Row == 0 && Col == Width - 1)
			{
				//   - Bottom Corner where Row = 0 and Col = Width - 1
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + Col]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + (Col - 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col - 1)]);
			}
			else if (Row == Height - 1 && Col == 0)
			{
				//   - Top Corner where Row = Height - 1 and Col = 0
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + Col]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + (Col + 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col + 1)]);
			}
			else if (Row == Height - 1 && Col == Width - 1)
			{
				//   - Top Corner where Row = Height - 1 and Col = Width - 1
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + Col]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + (Col - 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col - 1)]);
			}
			// EDGE CASES:
			else if (Col == 0)
			{
				//   - Left Edge where Col = 0
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + Col]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + (Col + 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col + 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + Col]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + (Col + 1)]);
			}
			else if (Row == Height - 1)
			{
				//   - Top Edge where Row = Height - 1
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col - 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col + 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + (Col - 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + Col]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + (Col + 1)]);
			}
			else if (Col == Width - 1)
			{
				//   - Right Edge where Col = Width - 1
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + Col]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + (Col - 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col - 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + Col]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + (Col - 1)]);
			}
			else if (Row == 0)
			{
				//   - Bottom Edge where Row = 0
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col - 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col + 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + (Col - 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + Col]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + (Col + 1)]);
			}
			// NORMAL CASES
			else
			{
				//Connect Top Left
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + (Col - 1)]);
				//Connect Top
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + Col]);
				//Connect Top Right
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + (Col + 1)]);
				//Connect Middle Left
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col - 1)]);
				//Connect Middle Right
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col + 1)]);
				//Connect Bottom Left
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + (Col - 1)]);
				//Connect Bottom Middle
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + Col]);
				//Connect Bottom Right
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + (Col + 1)]);
			}
		}
	}
}

void AAIManager::AddConnection(ANavigationNode* FromNode, ANavigationNode* ToNode)
{
	bool bCanConnect = true;
	if(bSteepnessPreventConnection)
	{
		FVector Direction = FromNode->GetActorLocation() - ToNode->GetActorLocation();
		Direction.Normalize();
		bCanConnect = (Direction.Z < AllowedAngle && Direction.Z > AllowedAngle * -1.0f);
	}
	
	if (bCanConnect)
	{
		if(!ToNode->bIsTraversible)
		{
			FromNode->ConnectedNonTraversableNodes.Add(ToNode);
		}
		else
		{
			FromNode->ConnectedNodes.Add(ToNode);
		}
		FromNode->AllConnectedNodes.Add(ToNode);

		// For a node, store the directions of all nodes (i.e Right node = (-1,0), Top-Right = (-1,1), etc
		FVector2D Dir2D = ToNode->GridLocation-FromNode->GridLocation;
		FromNode->AllConnectedDir.Add(Dir2D);
	}

}

ANavigationNode* AAIManager::FindNearestNode(const FVector& Location)
{
	ANavigationNode* NearestNode = nullptr;
	float NearestDistance = TNumericLimits<float>::Max();
	//Loop through the nodes and find the nearest one in distance
	for (ANavigationNode* CurrentNode : AllTraversableNodes)
	{
		float CurrentNodeDistance = FVector::Distance(Location, CurrentNode->GetActorLocation());
		if (CurrentNodeDistance < NearestDistance)
		{
			NearestDistance = CurrentNodeDistance;
			NearestNode = CurrentNode;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Nearest Node: %s"), *NearestNode->GetName())
	return NearestNode;
}

ANavigationNode* AAIManager::FindFurthestNode(const FVector& Location)
{
	ANavigationNode* FurthestNode = nullptr;
	float FurthestDistance = 0.0f;
	//Loop through the nodes and find the nearest one in distance
	for (ANavigationNode* CurrentNode : AllTraversableNodes)
	{
		float CurrentNodeDistance = FVector::Distance(Location, CurrentNode->GetActorLocation());
		if (CurrentNodeDistance > FurthestDistance)
		{
			FurthestDistance = CurrentNodeDistance;
			FurthestNode = CurrentNode;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Furthest Node: %s"), *FurthestNode->GetName())
	return FurthestNode;
}


