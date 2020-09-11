// Fill out your copyright notice in the Description page of Project Settings.


#include "Selector.h"

FSelector::FSelector()
{
    //CurrentChild = children.CreateConstIterator();
    
}

FSelector::~FSelector()
{
}

/*
void Selector::OnInitialise()
{  
}
*/

Node::EStatus FSelector::Update(Blackboard* Blackboard)
{
    //Loop through children until a child returns success or pass through all the children
    for (auto It = GetChildren().CreateIterator(); It; ++It)
    {
        if ((*It).Update(Blackboard) == EStatus::Success)
        {
            return EStatus::Success;
        }
    }
    return EStatus::Failure;
    
    /*
    //Until a child is running
    while(true)
    {
        Status s = (*CurrentChild)->Tick();
        //if a child is not successful, keep running
        if (s != Status::Success) return s;
        //continue until we pass through all children
        if (*++CurrentChild == *children.end()) return Status::Success;
    }
    */
}
