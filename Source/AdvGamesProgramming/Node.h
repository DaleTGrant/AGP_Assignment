// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blackboard.h"
#include "CoreMinimal.h"

/**
*
*/
class ADVGAMESPROGRAMMING_API Node
{
	public:
	Node();
	virtual ~Node();
	enum class Status
	{
		Invalid,
        Success,
        Running,
        Failure
     };

	//Utility Functions
	virtual bool IsStatus(Status status);
	virtual void Reset();
	virtual void AddChildren(Node Child);
	virtual bool HasChildren();
	virtual TArray<Node> GetChildren();
	virtual void ClearChildren();

	//Core Functions
	virtual Status Update(Blackboard* Blackboard);

	private:
	Status CurrentStatus;
	TArray<Node> children;
};

	
//Performs the traversal of the tree
//Status Tick();
//Core Functions
//Called once before update
//virtual void OnInitialise();
//Called each time the BT is updated

//Called once after update
//virtual void OnTerminate(Status status);
