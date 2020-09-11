// Fill out your copyright notice in the Description page of Project Settings.


#include "Behaviours.h"

/*
 * player chooses the closest node to the player as a destination and heads towards it
 * if the player has reached that node then return 'success', otherwise return 'running'
 */
Node::EStatus FCharge::Update(Blackboard* Blackboard)
{
    Blackboard->EnemyAI[0]->AgentEngage();
    if (Blackboard->EnemyAI[0]->Manager->FindNearestNode(Blackboard->EnemyAI[0]->DetectedActor->GetActorLocation())
        == Blackboard->EnemyAI[0]->CurrentNode) return EStatus::Success;
    return EStatus::Running;
}

/*
* player chooses the furthest node to the player as a destination and heads towards it
* if the player has reached that node then return 'success', otherwise return 'running'
*/
Node::EStatus FRetreat::Update(Blackboard* Blackboard)
{
    Blackboard->EnemyAI[0]->AgentEvade();
    if (Blackboard->EnemyAI[0]->Manager->FindFurthestNode(Blackboard->EnemyAI[0]->DetectedActor->GetActorLocation())
        == Blackboard->EnemyAI[0]->CurrentNode) return EStatus::Success;
    return EStatus::Running;
}

/*
 * player chooses a random node as a destination and heads towards it
 * if the player has reached that node then return 'success', otherwise return 'running'
 */
Node::EStatus FPatrol::Update(Blackboard* Blackboard)
{
    /*
    blackboard->EnemyAI[0]->AgentPatrol();
    if (blackboard->EnemyAI[0]->Manager->AllNodes[FMath::RandRange(0, blackboard->EnemyAI[0]->Manager->AllNodes.Num() - 1)])
        == blackboard->EnemyAI[0]->CurrentNode) return Status::Success;
        */
    return EStatus::Running;
}

/*
* if the player is visible to the AI then return 'success' otherwise return 'failure'
*/
Node::EStatus FPlayerCheck::Update(Blackboard* Blackboard)
{
    if (Blackboard->IsPlayerVisible()) return EStatus::Success;
    return EStatus::Failure;
}

/*
* AI to fire it's weapon towards the player and then return 'success'

Node::Status Shoot::Update(Blackboard* blackboard)
{
    //fire weapon
    return Status::Success;
}
*/
/*
* this decorator will check if the AI's health is below a defined threshold
* if it is, then run the child node, otherwise return 'failure'
*/
Node::EStatus FHealthCheck::Update(Blackboard* Blackboard)
{
    if (Blackboard->AIHealth < Blackboard->AIThreshold) return child.Update(Blackboard);
    return EStatus::Failure;
}

/*
* this decorator will keep running it's child until the child returns 'success'
* once that event occurs, it itself will return 'success'
*/
//NOTE: this current repeater will loop even if the AI gets health back
Node::EStatus FRepeater::Update(Blackboard* Blackboard)
{
    while (child.Update(Blackboard) != EStatus::Success)
        {
            child.Update(Blackboard);
        }
    return EStatus::Success;
}





