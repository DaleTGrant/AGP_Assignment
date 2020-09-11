// Fill out your copyright notice in the Description page of Project Settings.


#include "Node.h"

Node::Node()
{
    CurrentStatus = Status::Invalid;
}

Node::~Node()
{
}

bool Node::IsStatus(Status Status)
{
    return CurrentStatus == Status;
}

void Node::Reset()
{
    CurrentStatus = Status::Invalid;
}

void Node::AddChildren(Node Child)
{
    children.Push(Child);
}

bool Node::HasChildren()
{
    if (children.Num() != 0)
        return true;
    
    return false;
}

TArray<Node> Node::GetChildren()
{
    return children;
}

void Node::ClearChildren()
{
    children.Empty();
}

Node::Status Node::Update(Blackboard* Blackboard)
{
    return CurrentStatus;
}

/*
Node::Status Node::Tick()
{
    if (CurrentStatus != Status::Running) OnInitialise();
    CurrentStatus = Update();
    if (CurrentStatus != Status::Running) OnTerminate(CurrentStatus);
    return CurrentStatus;
}

void Node::OnInitialise()
{
}


void Node::OnTerminate(Status status)
{
}
*/


