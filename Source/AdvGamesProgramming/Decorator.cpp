// Fill out your copyright notice in the Description page of Project Settings.


#include "Decorator.h"

FDecorator::FDecorator()
{
}

FDecorator::~FDecorator()
{
}

void FDecorator::SetChild(Node * Child)
{
    child = *Child;
}

bool FDecorator::HasChild()
{
    //return child != nullptr;
    return false;
}

Node::EStatus FDecorator::Update(Blackboard* Blackboard)
{
    return child.Update(Blackboard);
}
