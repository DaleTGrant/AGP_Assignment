// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Decorator.h"
#include "Node.h"
#include "Blackboard.h"

/**
 * This file stores specific behaviours required for the enemy AI
 */

class ADVGAMESPROGRAMMING_API FCharge : public Node
{
	virtual EStatus Update(Blackboard* Blackboard) override;
};

class ADVGAMESPROGRAMMING_API FRetreat : public Node
{
	virtual EStatus Update(Blackboard* Blackboard) override;
	
};

class ADVGAMESPROGRAMMING_API FPlayerCheck : public Node
{
	virtual EStatus Update(Blackboard* Blackboard) override;
	
};
/*
class ADVGAMESPROGRAMMING_API Shoot : public Node
{
	virtual EStatus Update(Blackboard* Blackboard) override;
	
};
*/
class ADVGAMESPROGRAMMING_API FHealthCheck : public FDecorator
{
	virtual EStatus Update(Blackboard* Blackboard) override;
	
};

class ADVGAMESPROGRAMMING_API FRepeater : public FDecorator
{
	virtual EStatus Update(Blackboard* Blackboard) override;
	
};

class ADVGAMESPROGRAMMING_API FPatrol : public Node
{
	virtual EStatus Update(Blackboard* Blackboard) override;
};
