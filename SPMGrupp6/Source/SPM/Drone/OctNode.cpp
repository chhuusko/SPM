// Fill out your copyright notice in the Description page of Project Settings.


#include "OctNode.h"
#include "SVOGrid.h"

void FOctNode::AddChildren()
{
	if (Children[0] == nullptr)
	{
		float Quarter = Size.X / 2.0f;
		int i = 0;
        
        for (int x = -1; x <= 1; x += 2) {
        	for (int y = -1; y <= 1; y += 2) {
        		for (int z = -1; z <= 1; z += 2) {
        			FVector Offset(x * Quarter, y * Quarter, z * Quarter);
        			FOctNode* ChildCube = new FOctNode(Position+Offset, Size / 2);
        			Children[i] = ChildCube;
        			i++;
        		}
        	}
        }
	} else
	{
		for (FOctNode* Node : Children)
		{
			Node->AddChildren();
		}
	}
	
}
