// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviourTree.h"

FBehaviourTree::FBehaviourTree()
{
    
    //Initialise necessary nodes
    FSequence Sequence1;
    FSequence Sequence2;
    FSequence Sequence3;
    FSelector Selector1;
    FHealthCheck HealthCheck1;
    FPlayerCheck PlayerCheck1;
    FCharge Charge1;
    FRetreat Retreat1;
    //Shoot Shoot;

    //Create pointers to nodes to prevent object slicing when added as children
    FSequence * Sequence2Ptr = &Sequence2;
    FSequence * Sequence3Ptr = &Sequence3;
    FSelector * Selector1Ptr = &Selector1;
    FHealthCheck * HealthCheck1Ptr = &HealthCheck1;
    FPlayerCheck * PlayerCheck1Ptr = &PlayerCheck1;
    FCharge * Charge1Ptr = &Charge1;
    FRetreat * Retreat1Ptr = &Retreat1;

    //Adds nodes to the behaviour tree with appropriate hierarchy
    Sequence1.AddChildren(Sequence2Ptr);
    Sequence1.AddChildren(Sequence3Ptr);
    Sequence2.AddChildren(PlayerCheck1Ptr);
    //sequence2.AddChildren(shoot);
    Sequence2.AddChildren(Selector1Ptr);
    Selector1.AddChildren(HealthCheck1Ptr);
    Selector1.AddChildren(Charge1Ptr);
    HealthCheck1.SetChild(Retreat1Ptr);
    SetRoot(Sequence1);
}

FBehaviourTree::~FBehaviourTree()
{
}

//hard coded root to be a sequence
void FBehaviourTree::SetRoot(const FSequence Root) const
{
    *TreeRoot = Root;
}

//Removes all nodes from the behaviour tree
void FBehaviourTree::ClearNodes()
{
    Nodes.Empty();
}

void FBehaviourTree::Run() const
{
    //while a certain condition hasn't been met e.g. player is dead
    while (Blackboard->playerHealth != 0)
    {
        //Update the behaviour tree
        TreeRoot->Update(Blackboard);
    }
}




