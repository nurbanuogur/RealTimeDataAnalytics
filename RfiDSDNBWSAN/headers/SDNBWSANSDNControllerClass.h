/*
 * SDNBWSANSDNControllerClass.h
 *
 *  Created on: Dec 26, 2016
 *      Author: wsan
 */


#ifndef SDNBWSANSDNControllerClass_H
#define SDNBWSANSDNControllerClass_H
#include <iostream>
 
#include <cstdlib>
#include <vector>
#include "CombinedFlowTables.h"
#include "CombinedFlowTables2.h"
#define nUMBERnODES 10
 

using namespace std;
 	// status info structure
 

	typedef struct {
	int	node_mac_address;
	double rssi;
	int rssi_source;
	int rssi_destination;
	double energy_level;
	} wpan_status_values;

 struct NodeStatus{
	int nodeAddress;
	double rssi;
	int rssi_source;
	int rssi_destination;
	double BatteryLevel;
	int neighbor[nUMBERnODES];
	NodeStatus* next;
	} ;
 typedef struct NodeStatus* NodeStatusPtr;
	
 
 struct AppStatus{
	int NodeSrc;
	int Counter;
	} ;
 
///        ////////////////////      class   ///////////////
class SDNBWSANSDNControllerClass
{ 
	private:

		NodeStatusPtr nsHead;
		NodeStatusPtr nsCurr;
		NodeStatusPtr nsTemp;
		vector<CombinedFlowTables*> Applications;
		
//	protected:	
	 
		
	
public:
		int k;
        SDNBWSANSDNControllerClass();
		void  CreateApplication(int ApplicationID,int  ApplicationSrc);
		vector<FlowEntery*> CreatePathEntry(vector<int> DijkPath,int ApplicationID  );
		vector<int> GetSrcApplication(int nodeAddress);
		int GetApplicationSize();
		vector<AppStatus>  GetApplicationStatus();
		
//        void PrintFT();
		
	
		
        
  
};

#endif // SDNBWSANSDNControllerClass_H 

