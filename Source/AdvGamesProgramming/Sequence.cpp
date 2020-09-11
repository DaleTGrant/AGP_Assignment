// Fill out your copyright notice in the Description page of Project Settings.


#include "Sequence.h"

Sequence::Sequence()
{
}

Sequence::~Sequence()
{
}
/*
void Sequence::OnInitialise()
{
}
*/
Node::Status Sequence::Update(Blackboard* Blackboard)
{
    //Loop through children until a child returns failure or pass through all the children
    for (auto It = GetChildren().CreateIterator(); It; ++It)
    {
        if ((*It).Update(Blackboard) == Status::Failure)
        {
            return Status::Failure;
        }
    }
    return Status::Success;
}
