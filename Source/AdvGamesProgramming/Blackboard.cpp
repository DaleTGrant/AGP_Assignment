// Fill out your copyright notice in the Description page of Project Settings.


#include "Blackboard.h"

Blackboard::Blackboard()
{
}

Blackboard::~Blackboard()
{
}

float Blackboard::GetAIHealth()
{
    return AIHealth;
}

float Blackboard::IsPlayerVisible()
{
    return playerVisible;
}

void Blackboard::AddAI(AEnemyCharacter* Enemy)
{
    EnemyAI.Push(Enemy);
}
