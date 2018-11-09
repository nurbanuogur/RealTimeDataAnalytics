/*
 * NetworkTopology.h
 *
 *  Created on: Nov 29, 2016
 *      Author: wsan
 */

#ifndef NETWORKTOPOLOGY_H_
#define NETWORKTOPOLOGY_H_

#include <iostream>
#include <vector>
#include "Node.h"
#include "Connection.h"

class NetworkTopology
{
	public:
		std::vector<Node*> nodes;
		std::vector<Connection*> connections;
		std::vector<Node*> path;
		std::vector<Node*> nodesBackup; // for cleaning operation

	public:
		NetworkTopology();
		virtual ~NetworkTopology();
		void addNode(Node * node);


		void addConnection(Connection * connection);

		Node * findNode(int nodeName);


		bool findNode(Node* node);


		Node* findTheNearestNode();

		// Return neighbours of the node

		std::vector<Node*>* neighbourNodes(Node* node);
		int getTheCostBetweenNodes(Node* node1, Node* node2);
       // void FuzzyDijkstra(int destination,vector<Node*> neighbours, int FuzzyLogic);
		void FuzzyDijkstra(int destination,vector<Node*> neighbours, int FuzzyLogic, int NBTdecrease);
	 
		 vector<int> GetPathandPrint(Node* destination, int source );
		//vector<int>  GetPathandPrint(Node* destination,Node* source);
		void copyNodesVector();

};

#endif /* NETWORKTOPOLOGY_H_ */
