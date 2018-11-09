#ifndef CombinedFlowTables_H
#define CombinedFlowTables_H

#include <iostream>
#include <string>
#include <vector>
 
using namespace std;


 	// status info structure
	typedef enum Operation
		{
		Equal = 0,         
		Greater= 1,      
		Lower = 2 		  
		} Operation;
		

 
 typedef enum ActionRule
		{
		Drop = 0,        // Entry is Drop
		Forward= 1,      // Entry is Forward
		Modify = 2 		 // Entry is Modify
		} ActionRule;
 	  
	// routing table entry status  
struct FlowEntery
		{	
		int nodeAddress;
		int srcAdrMatchRule;
		Operation op;
		ActionRule action;
		int NextHop;
		int clusterDir;
		int	ApplicationID;
		int statistics;
		};  


struct ApplicationPath{
	  vector<FlowEntery*>  Nodes;
	};
	
struct FlowApplication{
	int	ApplicationID;
	int ApplicationSRC;
	ApplicationPath* Path;
	} ;
	 
 

class CombinedFlowTables
{
public:
FlowApplication Application;
	CombinedFlowTables();
 
};

#endif 