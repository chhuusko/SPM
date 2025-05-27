// Fill out your copyright notice in the Description page of Project Settings.


#include "Node.h"


bool FNode::IsClearAndNotVisited() const
{
	return IsClear && !IsVisited;
}
