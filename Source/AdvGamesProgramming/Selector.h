// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Composite.h"
#include "CoreMinimal.h"
#include "Containers/Array.h"

/**
 * 
 */
class ADVGAMESPROGRAMMING_API Selector : public Composite
{
public:
	Selector();
	~Selector();
	//virtual void OnInitialise() override;
	virtual Status Update(Blackboard* blackboard) override;

	
	//TCheckedPointerIterator<Node, int>& CurrentChild;
	//TIndexedContainerIterator<const TArray<Node>,const Node, int> CurrentChild;
	//= children.CreateConstIterator();
	
};
