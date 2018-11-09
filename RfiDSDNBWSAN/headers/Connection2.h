/*
 * Connection.cpp
 *
 *  Created on: Nov 29, 2016
 *      Author: wsan
 */

#include "Connection.h"

Connection::Connection(Node* first, Node* second, int cost)
{
	// TODO Auto-generated constructor stub

	this->setFirst(first);
	this->setSecond(second);
	this->setCost(cost);

}

Connection::~Connection()
{
	// TODO Auto-generated destructor stub
}

bool Connection::isConnected(Node* node1, Node* node2)
{
	return ( (node1 == this->getFirst() && node2 == this->getSecond()) ||
			(node1 == this->getSecond() && node2 == this->getFirst())
	);
}

Node* Connection::getFirst() const
{
	return first;
}

void Connection::setFirst(Node* first)
{
	this->first = first;
}

Node* Connection::getSecond() const
{
	return second;
}

void Connection::setSecond(Node* second)
{
	this->second = second;
}

int Connection::getCost() const
{
	return cost;
}

void Connection::setCost(int cost)
{
	this->cost = cost;
}

