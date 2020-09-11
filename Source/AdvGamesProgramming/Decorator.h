// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Node.h"

/**
 * 
 */
class ADVGAMESPROGRAMMING_API FDecorator : public Node
{
public:
	FDecorator();
	virtual ~FDecorator();
	void SetChild(Node * Child);
	static bool HasChild();
	virtual EStatus Update(Blackboard* Blackboard) override;
	Node child;
	
private:

};
