/*
 * NetworkTopology.cpp
 *
 *  Created on: Nov 29, 2016
 *      Author: wsan
 */

#include <iostream>
#include "NetworkTopology.h"
#include "Fuzzy.h"
//#include "noFuzzy.h"
#include "GetSNR.h"
#include <vector>
#include "node.h"
using namespace std;
vector<int> DijkPath;
NetworkTopology::NetworkTopology()
{
//	// TODO Auto-generated constructor stub
//	NetworkTopology::nodes=NULL;
//	NetworkTopology::connections=NULL;
//	NetworkTopology::path=NULL;
//	NetworkTopology::nodeBackups=NULL;

}

NetworkTopology::~NetworkTopology()
{
	// TODO Auto-generated destructor stub
	//clean the heap ...

	for(int i=0;i<nodesBackup.size(); i++)
		delete nodesBackup[i];//cout<<nodesBackup[i];
	for(int i=0;i<connections.size(); i++)
		delete connections[i];
}

void NetworkTopology::addNode(Node * node)
{
	nodes.push_back(node);

}

void NetworkTopology::addConnection(Connection * connection)
{
	connections.push_back(connection);

}

Node * NetworkTopology::findNode(int nodeName)
{
	for(int i=0;i<nodes.size();i++)
		if ((nodes[i])->getLabel()==nodeName)
			return nodes[i];
	return NULL;
}

bool NetworkTopology::findNode(Node* node)
{
	for(int i=0;i<nodes.size();i++)
		if (nodes[i]->getLabel()==node->getLabel())
			return true;
	return false;
}

Node* NetworkTopology::findTheNearestNode()
{

	if (!nodes.size()) return NULL;
	int nearestIndex = 0;
	Node* nearestNode = nodes.at(0);
	for (int i=1; i<nodes.size(); ++i)
	{
		Node* current = nodes.at(i);
		if (current->getCostToSource()< nearestNode->getCostToSource())
		{
			nearestNode = current;
			nearestIndex = i;
		}
	}
	nodes.erase(nodes.begin() + nearestIndex); //cout<<endl<<nearestIndex;
	return nearestNode;
}

// Return neighbours of the node

vector<Node*>* NetworkTopology::neighbourNodes(Node* node)
{
	vector<Node*>* neighbourNodes = new vector<Node*>();
	for(int i=0; i<connections.size(); ++i)
	{
		Connection* connection = connections.at(i);
		Node* neighbourNode = NULL;
		if (connection->getFirst() == node)
		{
			neighbourNode = connection->getSecond();
		}
		else if (connection->getSecond() == node)
		{
			neighbourNode = connection->getFirst();
		}
		if (neighbourNode && findNode(neighbourNode))
		{
			neighbourNodes->push_back(neighbourNode);//cout<<neighbourNode->id;
		}
		//cout<<neighbourNode->id<<endl;
	}
	return neighbourNodes;
}

 
int NetworkTopology::getTheCostBetweenNodes(Node* node1, Node* node2)
{

	for(int i=0; i<connections.size(); ++i)
	{
		Connection* connection = connections.at(i);
		if (connection->isConnected(node1, node2))
		{
			return connection->getCost();
		}
	}
	return false;
}
 


void NetworkTopology::FuzzyDijkstra(int destination,vector<Node*> neighbours, int FuzzyLogic, int NBTdecrease)
{
//	cout<<" from FuzzyDijkstra the node BT level get down= "<<NBTdecrease<<endl;
	//Node * destinationNode=this->findNode(destination);
	Node* neighbour=NULL; int cost=0;
	// nearest=this->findNode(destination);
	while (nodes.size() > 0)
	{
		Node* nearest = this->findTheNearestNode();
	  /* if (NBTdecrease!=-1)
		for (int i=0; i<nodes.size(); ++i)
			 if (NBTdecrease==nodes[i]->nodeAddress){
				nodes.erase(nodes.begin()+i);
				nearest = this->findTheNearestNode();
				}
	  	*/
		for (int i=0; i<nearest->neighbours_no; ++i)
		{
			int Myneighbour=nearest->nighbourList[i].node_address; 
			for (int k=0; k<neighbours.size() ; k++)
			if (Myneighbour==neighbours[k]->nodeAddress)
				neighbour=neighbours[k]; // take the address of the neighbour node
			
		if(neighbour!=NULL && (nearest->nodeAddress!=neighbour->nodeAddress))
		{
				if (nearest->nighbourList[i].SNR<=1) 	nearest->nighbourList[i].SNR=1;
				if (nearest->energy_level<=1) 			nearest->energy_level=1;
			// SDNC can run the algorithm with fuzzy and without fuzzy logic to get the link cost between devices
			switch (FuzzyLogic) {
				case 0: // this case for Only SNR cost
				{ 	 
				cost=GetSNR (nearest->nighbourList[i].SNR)+ nearest->getCostToSource();
				//cost = nofuzzy( nearest->nighbourList[i].SNR  , nearest->energy_level) + nearest->getCostToSource();
				printf("SNR only in Dijkstra results  between EDs address =%d <-> %d  cost=%d  \n",nearest->nodeAddress,nearest->nighbourList[i].node_address,cost);
				break;
				};
				case 1: // fuzzy option
				{	int BTcost=0;
				if (nearest->nodeAddress==NBTdecrease){
					BTcost=200;
					printf(" ddiijjj  NBTdecrease=%d     \n",NBTdecrease );
				} 
				cost = fuzzy( nearest->nighbourList[i].SNR  , nearest->energy_level) + nearest->getCostToSource(); //0 next will be 0+12 +10
				cost=cost+BTcost; BTcost=0;	 // for other costs
				printf(" FuzzyDijkstra results  between EDs address =%d <-> %d  cost=%d  \n",nearest->nodeAddress,nearest->nighbourList[i].node_address,cost);
				break;
				};
				case 2: //fixed
				{
				 cost=15;// here the cost set as constant for all links
				// printf(" ddiijjj fixed cost= %d   \n",cost);	 
				break;
				}; 
				default:
				{
				printf (  "No option was selected \n");
				};				
		} // 	end switch	
		
				//cout<<"nearest = "<<nearest->nodeAddress<<" cost before for node = "<<neighbour->nodeAddress<< " cost = "<<neighbour->getCostToSource()<<endl;
				if (cost < neighbour->getCostToSource()) // will compare cost with max value and avoid infinit route
				{ // dest start to set min cost to the source
					neighbour->setCostToSource(cost); // save the cost from dest to ED address 
					neighbour->setPrecedingNode(nearest);// save ED as next ED from dest 
				//	cout<<"--- neighbour is set as preceding ---"<<endl;
				}
				
			}// end if 
		}//end for 
 
	}
	
	// cout<<"application total path cost = "<<neighbour->getCostToSource()<<endl;
	//this->printPath(destinationNode);

}

vector<int> NetworkTopology::GetPathandPrint(Node* destination, int source )
{
 
	Node* previous=  destination ;
	cout <<"GetPathandPrint nodes Source="<< source << " destination="<< destination->getLabel()<<endl; 
	//DijkPath.empty();
	DijkPath.clear();

	 int infinitLoop = 0;
	 int totalCost = 0;
	while (previous)
	{
		DijkPath.push_back(previous->getLabel()); // first one is the dest ED
		totalCost=totalCost+previous->getCostToSource();
		previous = previous->getPrecedingNode(); //dest->getPrecedingNode (next ED)
		if (infinitLoop++ > 10) 			 break;
	}
	cout << endl;
	reverse(DijkPath.begin(),DijkPath.end());
	cout << "the resultant route in header: ";
    for (int i = 0; i<DijkPath.size(); i++){
	  cout << DijkPath[i]<< "> ";
	 }
	 cout << endl;
	cout<<"application total path cost = "<< totalCost<<endl;		
	
return DijkPath;
	}


	
	
void NetworkTopology::copyNodesVector()
{
	nodesBackup.assign(nodes.begin(),nodes.end());

}

