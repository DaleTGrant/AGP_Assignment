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
class ADVGAMESPROGRAMMING_API BehaviourTree
{
public:
	BehaviourTree();
	~BehaviourTree();
	void SetRoot(Sequence Root);
	void ClearNodes();
	void Run();

private:
	Sequence* root;
	TArray<Node> nodes;
	Blackboard* blackboard;
};
