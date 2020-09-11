// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Composite.h"
#include "CoreMinimal.h"
#include "Containers/Array.h"

/**
 * 
 */
class ADVGAMESPROGRAMMING_API FSelector : public Composite
{
public:
	FSelector();
	~FSelector();
	//virtual void OnInitialise() override;
	virtual EStatus Update(Blackboard* Blackboard) override;

	
	//TCheckedPointerIterator<Node, int>& CurrentChild;
	//TIndexedContainerIterator<const TArray<Node>,const Node, int> CurrentChild;
	//= children.CreateConstIterator();
	
};
