// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Node.h"
#include "Composite.h"
#include "Selector.h"
#include "Sequence.h"
#include "Decorator.h"
#include "Blackboard.h"
#include "Behaviours.h"

/**
 * 
 */
class ADVGAMESPROGRAMMING_API FBehaviourTree
{
public:
	FBehaviourTree();
	~FBehaviourTree();
	void SetRoot(FSequence Root) const;
	void ClearNodes();
	void Run() const;

private:
	FSequence* TreeRoot;
	TArray<Node> Nodes;
	Blackboard* Blackboard;
};
