//============================================================================
// Name        : MyDijkstras.cpp
// Author      : Ömer Faruk Çeken
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include <iostream>
#include <vector>
#include "Node.h"
#include "Connection.h"
#include "NetworkTopology.h"

using namespace std;

class Test
{
	public:
		void buidTopologyAndTestIt(int source, int destination)
		{
			NetworkTopology* network=new NetworkTopology();


			network->addNode(new Node(1));//a
			network->addNode(new Node(2));//b
			network->addNode(new Node(3));//c
			network->addNode(new Node(4));//d
			network->addNode(new Node(5));//e
			network->addNode(new Node(6));//f
			network->addNode(new Node(7));//g
			network->addNode(new Node(8));//h
			network->addConnection(new Connection(network->findNode(1), network->findNode(3), 2));
			network->addConnection(new Connection(network->findNode(1), network->findNode(4), 5));
			network->addConnection(new Connection(network->findNode(1), network->findNode(2), 8));
			network->addConnection(new Connection(network->findNode(2), network->findNode(4), 2));
			network->addConnection(new Connection(network->findNode(4), network->findNode(3), 2));
			network->addConnection(new Connection(network->findNode(2), network->findNode(6), 13));
			network->addConnection(new Connection(network->findNode(4), network->findNode(6), 6));
			network->addConnection(new Connection(network->findNode(4), network->findNode(7), 3));
			network->addConnection(new Connection(network->findNode(4), network->findNode(5), 1));
			network->addConnection(new Connection(network->findNode(7), network->findNode(6), 2));
			network->addConnection(new Connection(network->findNode(6), network->findNode(8), 3));
			network->addConnection(new Connection(network->findNode(7), network->findNode(8), 6));
			network->addConnection(new Connection(network->findNode(5), network->findNode(7), 1));
			(network->findNode(source))->setCostToSource(0);
			//	cout<<"Algoritma çalışmadan önceki  nodes:"<<network->nodes[3]->id;
			network->copyNodesVector();
			network->dijkstra(destination);
			delete network;
			//	cout<<"Algoritma çalıştıktan sonraki nodes:"<<network->nodes[3]->id;


		}

};


int main()
{
	Test test;
	test.buidTopologyAndTestIt(2,8);
	return 0;
}
