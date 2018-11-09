/*
 * Connection.h
 *
 *  Created on: Nov 29, 2016
 *      Author: wsan
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "Node.h"

class Connection
{
	private:
		Node* first;
		Node* second;
		int cost;

	public:
		Connection(Node* first, Node* second, int cost);
		virtual ~Connection();
		bool isConnected(Node* node1, Node* node2);


				Node* getFirst() const;


				void setFirst(Node* first);

				Node* getSecond() const;

				void setSecond(Node* second);

				int getCost() const;


				void setCost(int cost);




};

#endif /* CONNECTION_H_ */
