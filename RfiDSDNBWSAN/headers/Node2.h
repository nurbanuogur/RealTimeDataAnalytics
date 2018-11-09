/*
 * Node.cpp
 *
 *  Created on: Nov 29, 2016
 *      Author: wsan
 */

#include <iostream>
#include "Node.h"

Node::Node(int label)
{
	// TODO Auto-generated constructor stub
	this->setLabel(label);
	this->setCostToSource(100000);
	//this->setPrecedingNode(NULL);
	this->precedingNode=NULL;

}

Node::~Node()
{
	// TODO Auto-generated destructor stub
}

int Node::getCostToSource() const
{
	return costToSource;
}

void Node::setCostToSource(int costToSource)
{
	this->costToSource = costToSource;
}
int Node::getLabel() const
{
	return label;
}

void Node::setLabel(int label)
{
	this->label = label;
} 

 

Node* Node::getPrecedingNode() const
{
	return precedingNode;
}

void Node::setPrecedingNode(Node* precedingNode)
{
	this->precedingNode = precedingNode;
}

