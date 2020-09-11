// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Decorator.h"
#include "Node.h"
#include "Blackboard.h"

/**
 * This file stores specific behaviours required for the enemy AI
 */

class ADVGAMESPROGRAMMING_API Charge : public Node
{
	virtual Status Update(Blackboard* blackboard) override;
};

class ADVGAMESPROGRAMMING_API Retreat : public Node
{
	virtual Status Update(Blackboard* blackboard) override;
	
};

class ADVGAMESPROGRAMMING_API PlayerCheck : public Node
{
	virtual Status Update(Blackboard* blackboard) override;
	
};
/*
class ADVGAMESPROGRAMMING_API Shoot : public Node
{
	virtual Status Update(Blackboard* blackboard) override;
	
};
*/
class ADVGAMESPROGRAMMING_API HealthCheck : public Decorator
{
	virtual Status Update(Blackboard* blackboard) override;
	
};

class ADVGAMESPROGRAMMING_API Repeater : public Decorator
{
	virtual Status Update(Blackboard* blackboard) override;
	
};