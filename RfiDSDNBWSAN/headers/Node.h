/*
 * Node.h
 *
 *  Created on: Nov 29, 2016
 *      Author: wsan
 */

#ifndef NODE_H_
#define NODE_H_

//struct for neighbour list array
 typedef struct {
 int node_address;
 double SNR;
 } neighbour_list_struct;


// status info structure
typedef struct {
double energy_level;
int neighbours_no;
int nodeAddress;
int parentAddress;

neighbour_list_struct nighbourList [15];
} SDNWSAN_status_values;

class Node
{
	private:
		int label; //name of the node 
		int costToSource;  
		Node* precedingNode; // for path establishment

	public:
		Node(int label);
		virtual ~Node();
		double energy_level;
		int neighbours_no;
		int nodeAddress;
		int timeToLeave; // added new
		neighbour_list_struct nighbourList [15];
		int parentAddress;
		//neighbour_list_struct*  neighbour_list_struct_ptr;
		
		int getCostToSource() const;
		void setCostToSource(int costToSource);
		int getLabel() const;
		void setLabel(int label);
		Node* getPrecedingNode() const;
		void setPrecedingNode(Node* precedingNode);

};

#endif /* NODE_H_ */
