// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"

/**
 * 
 */
class ADVGAMESPROGRAMMING_API Blackboard
{
public:
	Blackboard();
	~Blackboard();
	float GetAIHealth();
	float IsPlayerVisible();
	float AIThreshold = 0.4;
	float playerHealth = 0;
	float AIHealth = 0;
	bool playerVisible = false;
	void AddAI(AEnemyCharacter* Enemy);
	TArray<AEnemyCharacter*> EnemyAI;
	
private:


};
