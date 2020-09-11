// Fill out your copyright notice in the Description page of Project Settings.


#include "Decorator.h"

Decorator::Decorator()
{
}

Decorator::~Decorator()
{
}

void Decorator::SetChild(Node Child)
{
    child = Child;
}

bool Decorator::HasChild()
{
    //return child != nullptr;
    return false;
}

Node::Status Decorator::Update(Blackboard* blackboard)
{
    return child.Update(blackboard);
}
