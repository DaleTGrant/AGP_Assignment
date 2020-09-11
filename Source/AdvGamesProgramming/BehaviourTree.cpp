// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviourTree.h"

BehaviourTree::BehaviourTree()
{
    Sequence sequence1;
    Sequence sequence2;
    Sequence sequence3;
    Selector selector;
    HealthCheck healthCheck;
    PlayerCheck playerCheck;
    Charge charge;
    Retreat retreat;
    //Shoot shoot;

    sequence1.AddChildren(sequence2);
    sequence1.AddChildren(sequence3);
    sequence2.AddChildren(playerCheck);
    //sequence2.AddChildren(shoot);
    sequence2.AddChildren(selector);
    selector.AddChildren(healthCheck);
    selector.AddChildren(charge);
    healthCheck.SetChild(retreat);
    SetRoot(sequence1);
}

BehaviourTree::~BehaviourTree()
{
}

//hard coded root to be a sequence
void BehaviourTree::SetRoot(Sequence Root)
{
    *root = Root;
}

void BehaviourTree::ClearNodes()
{
    nodes.Empty();
}

void BehaviourTree::Run()
{
    //while a certain condition hasn't been met e.g. player is dead
    while (blackboard->playerHealth != 0)
    {
        //Update the behaviour tree
        root->Update(blackboard);
    }
}




