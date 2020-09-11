// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Composite.h"
#include "CoreMinimal.h"

/**
 * 
 */
class ADVGAMESPROGRAMMING_API FSequence : public Composite
{
public:
	FSequence();
	~FSequence();
	//virtual void OnInitialise() override;
	virtual EStatus Update(Blackboard* Blackboard) override;
};
