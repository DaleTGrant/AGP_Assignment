// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Composite.h"
#include "CoreMinimal.h"

/**
 * 
 */
class ADVGAMESPROGRAMMING_API Sequence : public Composite
{
public:
	Sequence();
	~Sequence();
	//virtual void OnInitialise() override;
	virtual Status Update(Blackboard* Blackboard) override;
};
