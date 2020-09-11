// Fill out your copyright notice in the Description page of Project Settings.


#include "Behaviours.h"

/*
 * player chooses the closest node to the player as a destination and heads towards it
 * if the player has reached that node then return 'success', otherwise return 'running'
 */
Node::Status Charge::Update(Blackboard* blackboard)
{
    blackboard->EnemyAI[0]->AgentEngage();
    if (blackboard->EnemyAI[0]->Manager->FindNearestNode(blackboard->EnemyAI[0]->DetectedActor->GetActorLocation())
        == blackboard->EnemyAI[0]->CurrentNode) return Status::Success;
    return Status::Running;
}

/*
* player chooses the furthest node to the player as a destination and heads towards it
* if the player has reached that node then return 'success', otherwise return 'running'
*/
Node::Status Retreat::Update(Blackboard* blackboard)
{
    blackboard->EnemyAI[0]->AgentEvade();
    if (blackboard->EnemyAI[0]->Manager->FindFurthestNode(blackboard->EnemyAI[0]->DetectedActor->GetActorLocation())
        == blackboard->EnemyAI[0]->CurrentNode) return Status::Success;
    return Status::Running;
}

/*
* if the player is visible to the AI then return 'success' otherwise return 'failure'
*/
Node::Status PlayerCheck::Update(Blackboard* blackboard)
{
    if (blackboard->IsPlayerVisible()) return Status::Success;
    return Status::Failure;
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
Node::Status HealthCheck::Update(Blackboard* blackboard)
{
    if (blackboard->AIHealth < blackboard->AIThreshold) return child.Update(blackboard);
    return Status::Failure;
}

/*
* this decorator will keep running it's child until the child returns 'success'
* once that event occurs, it itself will return 'success'
*/
//NOTE: this current repeater will loop even if the AI gets health back
Node::Status Repeater::Update(Blackboard* blackboard)
{
    while (child.Update(blackboard) != Status::Success)
        {
            child.Update(blackboard);
        }
    return Status::Success;
}



