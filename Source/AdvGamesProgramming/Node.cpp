// Fill out your copyright notice in the Description page of Project Settings.


#include "Node.h"

Node::Node()
{
    CurrentStatus = EStatus::Invalid;
}

Node::~Node()
{
}

//Returns the status of the current node
bool Node::IsStatus(EStatus Status)
{
    return CurrentStatus == Status;
}

//Resets the status of the current node
void Node::Reset()
{
    CurrentStatus = EStatus::Invalid;
}

//Adds a child to the node's children
void Node::AddChildren(Node* Child)
{
    Children.Push(*Child);
}

//Checks whether the node has children
bool Node::HasChildren()
{
    if (Children.Num() != 0)
        return true;
    
    return false;
}

//Returns the children of the node
TArray<Node> Node::GetChildren()
{
    return Children;
}

//Removes all children from the node
void Node::ClearChildren()
{
    Children.Empty();
}

Node::EStatus Node::Update(Blackboard* Blackboard)
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


