// Fill out your copyright notice in the Description page of Project Settings.


#include "Sequence.h"

FSequence::FSequence()
{
}

FSequence::~FSequence()
{
}
/*
void Sequence::OnInitialise()
{
}
*/
Node::EStatus FSequence::Update(Blackboard* Blackboard)
{
    //Loop through children until a child returns failure or pass through all the children
    for (auto It = GetChildren().CreateIterator(); It; ++It)
    {
        if ((*It).Update(Blackboard) == EStatus::Failure)
        {
            return EStatus::Failure;
        }
    }
    return EStatus::Success;
}
