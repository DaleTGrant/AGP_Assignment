// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Node.h"

/**
 * 
 */
class ADVGAMESPROGRAMMING_API Decorator : public Node
{
public:
	Decorator();
	virtual ~Decorator();
	void SetChild(Node Child);
	bool HasChild();
	virtual Status Update(Blackboard* blackboard) override;
	Node child;
	
private:

};
