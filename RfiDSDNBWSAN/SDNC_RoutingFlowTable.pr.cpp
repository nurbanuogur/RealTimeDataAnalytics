/* Process model C++ form file: SDNC_RoutingFlowTable.pr.cpp */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char SDNC_RoutingFlowTable_pr_cpp [] = "MIL_3_Tfile_Hdr_ 145A 30A op_runsim 7 5BC865A3 5BC865A3 1 HBI-JCGS160 Mohammed@Hussein 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1e80 8                                                                                                                                                                                                                                                                                                                                                                                           ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

#include <prg_list_funcs.h>
#include "C:\OPNET\14.5.A\sys\include\prg.h"
 
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
 
#include "headers/SDNBWSANSDNControllerClass2.h"
#include "headers/Node2.h"
#include "headers/Node.h"
#include "headers/Connection2.h"
#include "headers/NetworkTopology2.h"
#include "headers/Node.h"
#include "headers/wpan_params.h"
#include "headers\SDNCsocketKafka.h"

using namespace std;
 

#define STATUS_REPORT 12
#define PRINT_LIST_CODE 		350
#define TO_MAC 					0
#define ToTDProcess				1

#define GTS_request_to_NeIntel 	901
#define TTLCode 		888
#define APP_ROUTE_REQ			13

#define	PRNT_LST	(op_intrpt_type () == OPC_INTRPT_SELF && op_intrpt_code () == PRINT_LIST_CODE)
#define INCOMING_PACKET (op_intrpt_type () == OPC_INTRPT_STRM)
#define GTS_request (op_intrpt_type () == OPC_INTRPT_REMOTE && op_intrpt_code () == GTS_request_to_NeIntel)
#define	TTL	(op_intrpt_type () == OPC_INTRPT_SELF && op_intrpt_code () == TTLCode)

// status info structure
 
// flow table structure
typedef struct {
int node_address;
int src;
int act;
int dest;
} flow_table;

// node gts characteristics structure
typedef struct {
int node_address;
int type;
int direction;
int length;
} node_gts_chrcts;


int ApplicationID=0;


// functions used
static void RunDijkstraFuzzy(int src,int dest,int batteryChanged );
static void NIntel_log_file_init(void);
static void checkNeighboursParents(void);
static Boolean IsSameConnectionInList(PrgT_List* list_ptr, int data_dest_address,int data_src_address);
static void UpdateConnectionList(PrgT_List* list_ptr, wpan_status_values* incoming_values);
static Boolean IsSameNodeInList(PrgT_List* list_ptr, int Node_address);
static void UpdateNodeList(PrgT_List* list_ptr, wpan_status_values* incoming_values);
static void printNSlistToLogFile(PrgT_List* Connection_list_print, PrgT_List* Node_list_print);
static void* flowTables_nested_list_mem_copy_create (void* list_ptr, unsigned int size);
static void gts_nested_list_mem_free (void* from_ptr);
static int GetClusterDirection(int nextHop, int nodeAddress);

/* End of Header Block */

#if !defined (VOSD_NO_FIN)
#undef	BIN
#undef	BOUT
#define	BIN		FIN_LOCAL_FIELD(_op_last_line_passed) = __LINE__ - _op_block_origin;
#define	BOUT	BIN
#define	BINIT	FIN_LOCAL_FIELD(_op_last_line_passed) = 0; _op_block_origin = __LINE__;
#else
#define	BINIT
#endif /* #if !defined (VOSD_NO_FIN) */



/* State variable definitions */
class SDNC_RoutingFlowTable_state
	{
	private:
		/* Internal state tracking for FSM */
		FSM_SYS_STATE

	public:
		SDNC_RoutingFlowTable_state (void);

		/* Destructor contains Termination Block */
		~SDNC_RoutingFlowTable_state (void);

		/* State Variables */
		Stathandle	             		bits_rcvd_stathandle                            ;
		Stathandle	             		bitssec_rcvd_stathandle                         ;
		Stathandle	             		pkts_rcvd_stathandle                            ;
		Stathandle	             		pktssec_rcvd_stathandle                         ;
		Stathandle	             		ete_delay_stathandle                            ;
		Stathandle	             		bits_rcvd_gstathandle                           ;
		Stathandle	             		bitssec_rcvd_gstathandle                        ;
		Stathandle	             		pkts_rcvd_gstathandle                           ;
		Stathandle	             		pktssec_rcvd_gstathandle                        ;
		Stathandle	             		ete_delay_gstathandle                           ;
		Packet*	                		pkptr                                           ;
		FILE*	                  		wpan_log                                        ;
		int	                    		co                                              ;
		PrgT_List*	             		wpan_Cnctn_list                                 ;
		PrgT_List*	             		wpan_Nds_list                                   ;
		PrgT_List*	             		flowTables                                      ;
		PrgT_List*	             		gtsChrstcs                                      ;
		Boolean	                		IS_TRANSMIT                                     ;
		Packet*	                		DecPkPtr                                        ;
		vector<Node*>	          		Nodes                                           ;
		SDNBWSANSDNControllerClass*			obSDNC                                          ;
		int	                    		rerout                                          ;
		int	                    		EDBatteryDown                                   ;
		int	                    		FuzzyLogic                                      ;
		int	                    		RunTTL                                          ;

		/* FSM code */
		void SDNC_RoutingFlowTable (OP_SIM_CONTEXT_ARG_OPT);
		/* Diagnostic Block */
		void _op_SDNC_RoutingFlowTable_diag (OP_SIM_CONTEXT_ARG_OPT);

#if defined (VOSD_NEW_BAD_ALLOC)
		void * operator new (size_t) throw (VOSD_BAD_ALLOC);
#else
		void * operator new (size_t);
#endif
		void operator delete (void *);

		/* Memory management */
		static VosT_Obtype obtype;
	};

VosT_Obtype SDNC_RoutingFlowTable_state::obtype = (VosT_Obtype)OPC_NIL;

#define pr_state_ptr            		((SDNC_RoutingFlowTable_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))
#define bits_rcvd_stathandle    		pr_state_ptr->bits_rcvd_stathandle
#define bitssec_rcvd_stathandle 		pr_state_ptr->bitssec_rcvd_stathandle
#define pkts_rcvd_stathandle    		pr_state_ptr->pkts_rcvd_stathandle
#define pktssec_rcvd_stathandle 		pr_state_ptr->pktssec_rcvd_stathandle
#define ete_delay_stathandle    		pr_state_ptr->ete_delay_stathandle
#define bits_rcvd_gstathandle   		pr_state_ptr->bits_rcvd_gstathandle
#define bitssec_rcvd_gstathandle		pr_state_ptr->bitssec_rcvd_gstathandle
#define pkts_rcvd_gstathandle   		pr_state_ptr->pkts_rcvd_gstathandle
#define pktssec_rcvd_gstathandle		pr_state_ptr->pktssec_rcvd_gstathandle
#define ete_delay_gstathandle   		pr_state_ptr->ete_delay_gstathandle
#define pkptr                   		pr_state_ptr->pkptr
#define wpan_log                		pr_state_ptr->wpan_log
#define co                      		pr_state_ptr->co
#define wpan_Cnctn_list         		pr_state_ptr->wpan_Cnctn_list
#define wpan_Nds_list           		pr_state_ptr->wpan_Nds_list
#define flowTables              		pr_state_ptr->flowTables
#define gtsChrstcs              		pr_state_ptr->gtsChrstcs
#define IS_TRANSMIT             		pr_state_ptr->IS_TRANSMIT
#define DecPkPtr                		pr_state_ptr->DecPkPtr
#define Nodes                   		pr_state_ptr->Nodes
#define obSDNC                  		pr_state_ptr->obSDNC
#define rerout                  		pr_state_ptr->rerout
#define EDBatteryDown           		pr_state_ptr->EDBatteryDown
#define FuzzyLogic              		pr_state_ptr->FuzzyLogic
#define RunTTL                  		pr_state_ptr->RunTTL

/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	SDNC_RoutingFlowTable_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((SDNC_RoutingFlowTable_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


/* Function Block */

#if !defined (VOSD_NO_FIN)
enum { _op_block_origin = __LINE__ + 2};
#endif

////////////////////// RunDijkstraFuzzy()
static void RunDijkstraFuzzy(int src,int dest,int batteryChanged ){

  if(batteryChanged==1 && FuzzyLogic==1){// when battery went down the application path recalled
  			fprintf (wpan_log, "time =%f first node's battery down =%d   \n", op_sim_time() ,src );	   
			EDBatteryDown=src; // save the address of this node
			vector<int> resultSRC=obSDNC->GetSrcApplication(src);//node's battery passed threshold
			for(int  i=0;i<resultSRC.size();i++){// app's effected from battery changes
			fprintf (wpan_log, "resultSRC[i]=%d resultSRC.size()=%d  \n",resultSRC[i],resultSRC.size());
			RunDijkstraFuzzy(resultSRC[i],12,NOT_DEFINED);// move to next option down	 
			}//end for all src affected
	 
		}// end batteryChanged
	else if(batteryChanged==NOT_DEFINED)
		{ // do it normal
		Node* destination=new Node(dest);
		int appSize=obSDNC->GetApplicationSize();
		fprintf (wpan_log, "time =%f   application size=%d \n", op_sim_time() ,appSize );
 
		NetworkTopology* network=new NetworkTopology();
		for(int i=0;i<Nodes.size();i++){
		 if (Nodes[i]->nodeAddress==dest)
			 destination=Nodes[i];
		 	network->addNode(Nodes[i]);
		}
 
// cout<<"oop----dijkstra output =="<<endl;

 for (int i = 0; i < network->nodes.size(); i++)
	 {
	 network->nodes[i]->setCostToSource(10000);
	 network->nodes[i]->setPrecedingNode(NULL);
	 }
 if (network->findNode(src))
	 (network->findNode(src))->setCostToSource(0);
 
 network->copyNodesVector();
 fprintf (wpan_log, "time =%f Fuzzy option=%d \n", op_sim_time(),FuzzyLogic );
// cout<<"the min path cost from source="<<src<<" to destination="<<dest<<endl;
 // here we are calling fuzzy and dijkstra algorithms

 network->FuzzyDijkstra(dest,Nodes,FuzzyLogic,EDBatteryDown);
 
// the path return from Fuzzy and Dijkstra
 vector<int> DijkPath =  network->GetPathandPrint(destination,src);
 
 char err_str[300];
char sendThisMsgToKafka[300];
 fprintf (wpan_log, "DijkPath The resultant route for application ID=%d  \n",ApplicationID);   
  sprintf (err_str, " %d  =>  %d => %d  => %d  =>  %d =>  %d ", DijkPath[0], DijkPath[1], DijkPath[2], DijkPath[3], DijkPath[4], DijkPath[5]);
 sprintf (sendThisMsgToKafka, "path%d:%d:%d:%d:%d:%d:%d",ApplicationID, DijkPath[0], DijkPath[1], DijkPath[2], DijkPath[3], DijkPath[4], DijkPath[5]);
// SDNCsocketKafka(sendThisMsgToKafka);
 op_sim_message ("The path is=",  err_str);

 
 //cout << "The resultant route: ";
 
 for (int i = 0; i<DijkPath.size(); ++i){
	 fprintf (wpan_log, "  %d  => ", DijkPath[i]);
	// sprintf (err_str, "=> %d  => ", DijkPath[i]);
	// op_sim_message ("The path is=",  err_str);
	 }
  
	 
 
 
	 
 vector<FlowEntery*> reFlowTable = obSDNC->CreatePathEntry(DijkPath,ApplicationID );

 fprintf (wpan_log, "flow entries %d entires is needed to be sent \n", reFlowTable.size());
	  for(int i=0;i<reFlowTable.size();i++){
	  	fprintf (wpan_log, "application ID = %d \n\n", reFlowTable[i]->ApplicationID);
		fprintf (wpan_log, "application Path node address= %d \n", reFlowTable[i]->nodeAddress);
		fprintf (wpan_log, "application Path srcAddress  = %d \n", reFlowTable[i]->srcAdrMatchRule);
		fprintf (wpan_log, "application Path action      = %d \n", reFlowTable[i]->action);
		fprintf (wpan_log, "application Path NextHop     = %d \n", reFlowTable[i]->NextHop);
		//finding cluster direction (important to nodes with two clusters to define to which cluster this connection should be forwarded)
		reFlowTable[i]->clusterDir = GetClusterDirection(reFlowTable[i]->NextHop, reFlowTable[i]->nodeAddress); 
		fprintf (wpan_log, "application Path clusterDir  = %d \n\n", reFlowTable[i]->clusterDir);
	  cout<<endl;
		}
 cout<<"---send FlowTable to relative nodes "<<endl;
 fprintf (wpan_log, "---send FlowTable to relative nodes \n\n");
  /* send flow tables  to mac  */
	DecPkPtr = op_pk_create_fmt ("NeIntelDecision");
	op_pk_nfd_set_ptr (DecPkPtr, "FlowTables", &reFlowTable[0], op_prg_mem_copy_create, op_prg_mem_free, sizeof (FlowEntery) * reFlowTable.size());
	op_pk_nfd_set (DecPkPtr, "flow_entries_no", reFlowTable.size());
 	op_pk_send(DecPkPtr, TO_MAC);

// delete destination;
// delete network;
 }//end else battery
  
FOUT;
}

////////////////////////////////////////////

// function for finding cluster direction
static int GetClusterDirection(int nextHop, int nodeAddress){

	FIN (GetClusterDirection());
	
	int dir = 0; // 0 for cluster in and 1 for cluster out.;
	
	for (int j = 0; j < Nodes.size(); j++)
		if (nextHop == Nodes[j]->nodeAddress)
			if (nodeAddress == Nodes[j]->parentAddress)
				dir = 1;
	FRET(dir);
	}
//////////////////////////////////

static void  NIntel_log_file_init() {
Objid selfID;
Objid fatherID;
char folder_path[120];
char log_name[132];
char name [20];
//"Coordinator.NIntel_process";
//strncpy(log_name, "Coordinator.NIntel_process", 132);
FIN (wpan_log_file_init ());
selfID = op_id_self();
fatherID = op_topo_parent(selfID);
op_ima_obj_attr_get (fatherID, "Log File Directory", folder_path);
op_ima_obj_attr_get (fatherID, "name", &name);

if (prg_path_name_is_dir (folder_path) == PrgC_Path_Name_Is_Not_Dir) {
		char msg[128];		
		sprintf (msg, " \"%s\" is not valid directory name. The output will not be logged.\n", folder_path); 
		/* Display an appropriate warning.	*/
		op_prg_odb_print_major ("Warning from network intelligence process: ", msg, OPC_NIL);
	}	
sprintf (log_name, "%s%s%s.ak", folder_path, name,"_NIntel_process"); 
printf ("Log file name: %s \n\n", log_name);
wpan_log = fopen(log_name,"w");
fprintf (wpan_log, "|-----------------------------------------------------------------------------|\n");
fprintf (wpan_log, "| NODE NAME          : %sNIntel_process \n", name);
FOUT;
}

static Boolean IsSameConnectionInList(PrgT_List* list_ptr, int data_dest_address,int data_src_address) {

int i;
wpan_status_values* wpan_status_values_ptr;
Boolean ANS;

FIN (IsSameConnectionInList(PrgT_List* list_ptr, int data_dest_address,int data_src_address));
ANS = OPC_FALSE;

for (i=0; i<op_prg_list_size (list_ptr); i++) {
if ((wpan_status_values_ptr = (wpan_status_values *) op_prg_list_access (list_ptr, i)) != OPC_NIL)
	{
if (wpan_status_values_ptr->rssi_destination==data_dest_address && wpan_status_values_ptr->rssi_source==data_src_address)
	{ 
		ANS = OPC_TRUE;
	}
} else {fprintf (wpan_log, "t = %f -> no elements in the list when looking for the same node mac address \n", op_sim_time());}
}


FRET (ANS);
}
//////////////////////////////////////////////// not included
static Boolean IsSameNodeInList(PrgT_List* list_ptr, int Node_address) {

int i;
wpan_status_values* wpan_status_values_ptr;
Boolean ANS;

FIN (IsSameNodeInList(PrgT_List* list_ptr, int Node_address));
ANS = OPC_FALSE;

for (i=0; i<op_prg_list_size (list_ptr); i++) {
if ((wpan_status_values_ptr = (wpan_status_values *) op_prg_list_access (list_ptr, i)) != OPC_NIL)
	{
if (Node_address==wpan_status_values_ptr->node_mac_address)
	{ 
		ANS = OPC_TRUE;
		break;
	}
} else {fprintf (wpan_log, "t = %f -> problem in reading from list \n", op_sim_time());}
}


FRET (ANS);
}


static void UpdateConnectionList(PrgT_List* list_ptr, wpan_status_values* incoming_values) {

int i;
wpan_status_values* wpan_status_values_ptr;


FIN(UpdateConnectionList(PrgT_List* list_ptr, wpan_status_values* incoming_values));

for (i=0; i<op_prg_list_size (list_ptr); i++) {
if ((wpan_status_values_ptr = (wpan_status_values *) op_prg_list_access (list_ptr, i))!= OPC_NIL)
	{
	if (wpan_status_values_ptr->rssi_destination==incoming_values->rssi_destination && wpan_status_values_ptr->rssi_source==incoming_values->rssi_source)
		{
			//wpan_status_values_ptr = incoming_values;
		op_prg_list_remove (list_ptr, i);
		op_prg_list_insert (list_ptr, incoming_values, i);
			}
	}
}

FOUT;
}

static void UpdateNodeList(PrgT_List* list_ptr, wpan_status_values* incoming_values) {

int i;
wpan_status_values* wpan_status_values_ptr;


FIN(UpdateNodeList(PrgT_List* list_ptr, wpan_status_values* incoming_values));

for (i=0; i<op_prg_list_size (list_ptr); i++) {
if ((wpan_status_values_ptr = (wpan_status_values *) op_prg_list_access (list_ptr, i))!= OPC_NIL)
	{
	if (incoming_values->node_mac_address==wpan_status_values_ptr->node_mac_address)
		{
			//wpan_status_values_ptr = incoming_values;
		op_prg_list_remove (list_ptr, i);
		op_prg_list_insert (list_ptr, incoming_values, i);
		break;
			}
	}
}

FOUT;
}

static void printNSlistToLogFile(PrgT_List* Connection_list_print, PrgT_List* Node_list_print) {
int i;
wpan_status_values* print_struct_ptr;
FIN(printNSlistToLogFile(PrgT_List* Connection_list_print, PrgT_List* Node_list_print));
	fprintf (wpan_log, "********************************************************************************\n");
	fprintf (wpan_log, "********************************************************************************\n");
	fprintf (wpan_log, "********************************************************************************\n");
	fprintf (wpan_log, "********************************************************************************\n");
	fprintf (wpan_log, "********************* Connections information available in the list*************\n");
	
	for (i=0; i<op_prg_list_size (Connection_list_print); i++) {
		if ((print_struct_ptr = (wpan_status_values *) op_prg_list_access (Connection_list_print, i)) != OPC_NIL)
			{
			fprintf (wpan_log, "----------------------------- %d --------------------------------- \n", i);
			fprintf (wpan_log, "t = %f -> List node of address %d \n", op_sim_time(), print_struct_ptr->node_mac_address);	
			fprintf (wpan_log, "t = %f -> List rssi is %f \n", op_sim_time(), print_struct_ptr->rssi);
			fprintf (wpan_log, "t = %f -> List rssi source is %d \n", op_sim_time(), print_struct_ptr->rssi_source);
			fprintf (wpan_log, "t = %f -> List rssi destination is %d \n", op_sim_time(), print_struct_ptr->rssi_destination);
			fprintf (wpan_log, "t = %f -> List energy level is %f \n", op_sim_time(), print_struct_ptr->energy_level);
			
			}
		}
	fprintf (wpan_log, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
	fprintf (wpan_log, "&&&&&&&&&&&&&&&&&&&&&& Nodes Infrormation available in the list &&&&&&&&&&&&&&&&\n");

	for (i=0; i<op_prg_list_size (Node_list_print); i++) {
		if ((print_struct_ptr = (wpan_status_values *) op_prg_list_access (Node_list_print, i)) != OPC_NIL)
			{
			fprintf (wpan_log, "----------------------------- %d --------------------------------- \n", i+1);
			fprintf (wpan_log, "t = %f -> List node of address %d \n", op_sim_time(), print_struct_ptr->node_mac_address);	
			fprintf (wpan_log, "t = %f -> List rssi is %f \n", op_sim_time(), print_struct_ptr->rssi);
			fprintf (wpan_log, "t = %f -> List rssi source is %d \n", op_sim_time(), print_struct_ptr->rssi_source);
			fprintf (wpan_log, "t = %f -> List rssi destination is %d \n", op_sim_time(), print_struct_ptr->rssi_destination);
			fprintf (wpan_log, "t = %f -> List energy level is %f \n", op_sim_time(), print_struct_ptr->energy_level);
			
	}
		}
	
	
	fprintf (wpan_log, "********************************************************************************\n");
	fprintf (wpan_log, "********************************************************************************\n");
	fprintf (wpan_log, "********************************************************************************\n");
	fprintf (wpan_log, "********************************************************************************\n");
	
	
	FOUT;
	}



static void GTS_request_decision() {


Objid selfID;
Objid parentID;
Objid grandfatherID;
int panSinkAddr;
int mainSrcAddr;
wpan_status_values* Nlist_struct_ptr;
wpan_status_values* mainSrc_Nlist;
PrgT_List* neighboursList;
PrgT_List* transmitNodes;
int* transmitNid;
flow_table* flow_table_ptr;
node_gts_chrcts* node_gts_chrcts_ptr;


int* neighbourAddr1;
int* neighbourAddr2;
int* neighbourAddr;
 //int GBO;
int i, n,  slots;
int NextNodeAddr;
int* scnd_node_addr;
double maxBatteryLevel;


FIN (GTS_request_decision())

transmitNodes = op_prg_list_create();
neighboursList = op_prg_list_create();

flowTables = op_prg_list_create();
gtsChrstcs = op_prg_list_create();
//if (op_prg_list_size(flowTables) > 0)
//	for (i=0; i<op_prg_list_size (flowTables); i++)
//		op_prg_list_remove (flowTables, i); 

//if (op_prg_list_size(gtsChrstcs) > 0)
//	for (i=0; i<op_prg_list_size (gtsChrstcs); i++)
//		op_prg_list_remove (gtsChrstcs, i); 
		

selfID = op_id_self();
parentID = op_topo_parent(selfID);
grandfatherID = op_topo_parent(parentID);

op_ima_obj_attr_get (selfID, "PAN Sink Address", &panSinkAddr);
op_ima_obj_attr_get (selfID, "Gts Source Address", &mainSrcAddr);
//mainSrcAddr = (int*) op_ima_obj_state_get (selfID);
mainSrc_Nlist = (wpan_status_values *) op_prg_mem_alloc (sizeof (wpan_status_values));
neighbourAddr1 = (int *) op_prg_mem_alloc (sizeof (int));
neighbourAddr2 = (int *) op_prg_mem_alloc (sizeof (int));
scnd_node_addr = (int *) op_prg_mem_alloc (sizeof (int));

for (i=0; i<op_prg_list_size (wpan_Nds_list); i++) {
	if ((Nlist_struct_ptr = (wpan_status_values *) op_prg_list_access (wpan_Nds_list, i)) != OPC_NIL)
		if (Nlist_struct_ptr->node_mac_address == mainSrcAddr)
			{ 
			mainSrc_Nlist->node_mac_address = Nlist_struct_ptr->node_mac_address; 
			mainSrc_Nlist->energy_level = Nlist_struct_ptr->energy_level;
			// if there were neighbour nodes field we should have been taken that too
			}
		
	}
			  //here we should look into the field of neighbour nodes but its not set yet in our case so
			  //we will set it manually cause we know those neghbour nodes.
			*neighbourAddr1 = op_id_from_name (op_id_from_name (grandfatherID, OPC_OBJTYPE_NODE_FIX, "R1"), OPC_OBJTYPE_QUEUE, "wpan_mac");
			fprintf (wpan_log, "t = %f -> alish alish  %d aaaaaaaaa  %d   ssssss %d\n", op_sim_time(), *neighbourAddr1,op_id_from_name (grandfatherID, OPC_OBJTYPE_NODE_FIX, "R1"), grandfatherID);
			prg_list_insert (neighboursList, neighbourAddr1, OPC_LISTPOS_TAIL);
			*neighbourAddr2 = op_id_from_name (op_id_from_name (grandfatherID, OPC_OBJTYPE_NODE_FIX, "R2"), OPC_OBJTYPE_QUEUE, "wpan_mac");
			prg_list_insert (neighboursList, neighbourAddr2, OPC_LISTPOS_TAIL);
			maxBatteryLevel = -9;
			for (i=0; i<op_prg_list_size (neighboursList); i++)
				if ((neighbourAddr = (int *) op_prg_list_access (neighboursList, i)) != OPC_NIL)
					{
						if (*neighbourAddr == panSinkAddr)
							{
							NextNodeAddr = panSinkAddr;
							break;
							}
						else
							for (n=0; n<op_prg_list_size (wpan_Nds_list); n++)
								if ((Nlist_struct_ptr = (wpan_status_values *) op_prg_list_access (wpan_Nds_list, n)) != OPC_NIL)
									if (Nlist_struct_ptr->node_mac_address == *neighbourAddr)
										if (Nlist_struct_ptr->energy_level > maxBatteryLevel)
											{
											maxBatteryLevel = Nlist_struct_ptr->energy_level;
											NextNodeAddr = Nlist_struct_ptr->node_mac_address;
											}
						}
		flow_table_ptr = (flow_table *) op_prg_mem_alloc (sizeof (flow_table));
		flow_table_ptr->node_address = mainSrcAddr;
		flow_table_ptr->src = mainSrcAddr;
		flow_table_ptr->act = 1;
		flow_table_ptr->dest = NextNodeAddr;
		prg_list_insert (flowTables, flow_table_ptr, OPC_LISTPOS_TAIL);
		prg_list_insert (transmitNodes, &mainSrcAddr, OPC_LISTPOS_TAIL);
		*scnd_node_addr = NextNodeAddr;
		
		for (i=0; i<op_prg_list_size (neighboursList); i++)
			op_prg_list_remove (neighboursList, i); 
		
		// now we should look at the neighbour nodes of the winner node BiggerBatteryLevelNodeAddr 
		// so again we should look at the fields of the main node list but we will add them manually as before
			*neighbourAddr1 = op_id_from_name (op_id_from_name (grandfatherID, OPC_OBJTYPE_NODE_FIX, "E11"), OPC_OBJTYPE_QUEUE, "wpan_mac");
		//fprintf (wpan_log, "t = %f -> alish alish2  %d aaaaaaaaa  %d\n", op_sim_time(), *neighbourAddr1,op_id_from_name (grandfatherID, OPC_OBJTYPE_NODE_FIX, "ASN1"));	
		prg_list_insert (neighboursList, neighbourAddr1, OPC_LISTPOS_TAIL);
			*neighbourAddr2 = op_id_from_name (op_id_from_name (grandfatherID, OPC_OBJTYPE_NODE_FIX, "E12"), OPC_OBJTYPE_QUEUE, "wpan_mac");
			prg_list_insert (neighboursList, neighbourAddr2, OPC_LISTPOS_TAIL);
		
		maxBatteryLevel = -9;
		for (i=0; i<op_prg_list_size (neighboursList); i++)
				if ((neighbourAddr = (int *) op_prg_list_access (neighboursList, i)) != OPC_NIL)
					{
						if (*neighbourAddr == panSinkAddr)
							{
							NextNodeAddr = panSinkAddr;
							break;
							}
						else
							for (n=0; n<op_prg_list_size (wpan_Nds_list); n++)
								if ((Nlist_struct_ptr = (wpan_status_values *) op_prg_list_access (wpan_Nds_list, n)) != OPC_NIL)
									if (Nlist_struct_ptr->node_mac_address == *neighbourAddr)
										if (Nlist_struct_ptr->energy_level > maxBatteryLevel)
											{
											maxBatteryLevel = Nlist_struct_ptr->energy_level;
											NextNodeAddr = Nlist_struct_ptr->node_mac_address;
											}
						}
		flow_table_ptr = (flow_table *) op_prg_mem_alloc (sizeof (flow_table));
		flow_table_ptr->node_address = *scnd_node_addr;
		flow_table_ptr->src = mainSrcAddr;
		flow_table_ptr->act = 1;
		flow_table_ptr->dest = NextNodeAddr;
		prg_list_insert (flowTables, flow_table_ptr, OPC_LISTPOS_TAIL);
		prg_list_insert (transmitNodes, scnd_node_addr, OPC_LISTPOS_TAIL);
		
		
		// and at last the flow table of the pan sink
		flow_table_ptr = (flow_table *) op_prg_mem_alloc (sizeof (flow_table));
		flow_table_ptr->node_address = panSinkAddr;
		flow_table_ptr->src = *scnd_node_addr;
		flow_table_ptr->act = 1;
		flow_table_ptr->dest = panSinkAddr;
		prg_list_insert (flowTables, flow_table_ptr, OPC_LISTPOS_TAIL);
	//	PrgT_List* psst;
	//	psst = flowTables;
		// go through the transmit list to set the gts characteristics list
		/*
		op_ima_obj_attr_get (parentID, "During GTS BO", &GBO);
			if (GBO == 2)
				slots = 13;
			else if (GBO == 1)
				slots = 11;
			else slots = 15;
		
		*/
		slots = 11;
		
		for (i=0; i<op_prg_list_size (transmitNodes); i++)
				if ((transmitNid = (int *) op_prg_list_access (transmitNodes, i)) != OPC_NIL)
					{
						//work on the gts characteristic list :)
						node_gts_chrcts_ptr = (node_gts_chrcts *) op_prg_mem_alloc (sizeof (node_gts_chrcts));
						node_gts_chrcts_ptr->node_address = *transmitNid;
						node_gts_chrcts_ptr->type = 1;
						node_gts_chrcts_ptr->direction = 0;
						//if (i==0 && (slots % op_prg_list_size (transmitNodes)) == 1)
						if (i==0)
							node_gts_chrcts_ptr->length = (slots/op_prg_list_size (transmitNodes))+(slots % op_prg_list_size (transmitNodes));
						//else if (i >= 0 && (slots % op_prg_list_size (transmitNodes)) > 1 && i < 3)
						//	node_gts_chrcts_ptr->length = (slots/op_prg_list_size (transmitNodes))+1;
						else	
							node_gts_chrcts_ptr->length = (slots/op_prg_list_size (transmitNodes));
						
						prg_list_insert (gtsChrstcs, node_gts_chrcts_ptr, i);
					}
							
	FOUT;
	}

/*-----------------------------------------------------------------------------
 * Function:	flowTables_nested_list_mem_copy_create
 *
 * Description:	provide a hard-copy of the flowTables_list (list containing flow_table records).
 *				This function is used in the "op_pk_nfd_set_ptr()" as a copy procedure and therefore
 *				it has to match the following function prototype: void* fce (void*, usingned int);
 *              
 * Input :	list_ptr - pointer to the GTS_list to be copied
 *			size - formal parameter - is not used in this function
 *
 * Return :	void* - pointer to the copy of the GTS_list
 *-----------------------------------------------------------------------------*/
/*
static void* flowTables_nested_list_mem_copy_create (PrgT_List* list_ptr, unsigned int size)
{
	PrgT_List* list_copy;
	flow_table* from_list_ptr;
	flow_table* flow_table_ptr;
	int i;	
	
	// allocate an empty list  
	list_copy = op_prg_list_create ();
	
	for (i=0; i<op_prg_list_size ((PrgT_List* ) list_ptr); i++) {
		from_list_ptr = op_prg_list_access ((PrgT_List* ) list_ptr, i);
		
		// memory allocation  
		flow_table_ptr	= (flow_table *) op_prg_mem_alloc (sizeof (flow_table)); 
		
		flow_table_ptr = (flow_table *) from_list_ptr;
		flow_table_ptr->node_address = ((flow_table *) from_list_ptr)->node_address;
		flow_table_ptr->src = ((flow_table *) from_list_ptr)->src;
		flow_table_ptr->act = ((flow_table *) from_list_ptr)->act;
		flow_table_ptr->dest = ((flow_table *) from_list_ptr)->dest;
		
		op_prg_list_insert (list_copy, flow_table_ptr, OPC_LISTPOS_TAIL);
	}	
	
	return (PrgT_List* ) list_copy;
}

*/

/*-----------------------------------------------------------------------------
 * Function:	gts_nested_list_mem_free
 *
 * Description:	deallocate a GTS_list (list containing wpan_gts_descriptor records)
 *				that was previously allocated. 
 *				This function is used in the "op_pk_nfd_set_ptr()" as a deallocation procedure 
 *				and therefore it has to match the following function prototype: void* fce (void* );
 *              
 * Input :  from_ptr - pointer to the GTS_list to be deallocated
 *-----------------------------------------------------------------------------*/

static void gts_nested_list_mem_free (void* from_ptr)
{
	/* deallocate memory of the list */
	op_prg_mem_free (from_ptr);
}

static void checkNeighboursParents(){
	
	FIN (checkNeighboursParents());
	printf("\n check neighbours function for different parents \n");
	for(int i=0;i<Nodes.size();i++)
		{
		for (int j = 0; j < Nodes[i]->neighbours_no; j++)
			if (Nodes[i]->nighbourList[j].node_address != 0 && Nodes[i]->nighbourList[j].node_address != Nodes[i]->parentAddress)
				for (int k = 0; k < Nodes.size(); k++)
					// here we check if the Nodes[i]->nighbourList[j].node_address in the K list
					if (Nodes[k]->nodeAddress == Nodes[i]->nighbourList[j].node_address)
						{// Nodes[i]->nighbourList[j].node_address +++ this ED is different parent
						int current_neighbour = Nodes[i]->nighbourList[j].node_address;
						int i_p = Nodes[i]->parentAddress; // i_p (i parent)
						int k_p = Nodes[k]->parentAddress; // k_p (k parent)
						if (Nodes[i]->parentAddress != Nodes[k]->parentAddress)
							{
							if ((Nodes[k]->parentAddress != Nodes[i]->nodeAddress) && (Nodes[i]->parentAddress != Nodes[k]->nodeAddress))
								{
								//for (int l = j--; l < Nodes[i]->neighbours_no; l++)
								for (int l = j; l < Nodes[i]->neighbours_no; l++)
									{
									Nodes[i]->nighbourList[l].node_address = Nodes[i]->nighbourList[l+1].node_address;
									Nodes[i]->nighbourList[l].SNR = Nodes[i]->nighbourList[l+1].SNR;
									}
								
								Nodes[i]->neighbours_no--;
								fprintf (wpan_log,"\n %d is deleted from %d neighbours \n", Nodes[k]->nodeAddress, Nodes[i]->nodeAddress);
								printf ("\n %d is deleted from %d neighbours \n", Nodes[k]->nodeAddress, Nodes[i]->nodeAddress);
								}
							}
						}
		}
	FOUT;
	}

/* End of Function Block */

/* Undefine optional tracing in FIN/FOUT/FRET */
/* The FSM has its own tracing code and the other */
/* functions should not have any tracing.		  */
#undef FIN_TRACING
#define FIN_TRACING

#undef FOUTRET_TRACING
#define FOUTRET_TRACING

/* Undefine shortcuts to state variables because the */
/* following functions are part of the state class */
#undef bits_rcvd_stathandle
#undef bitssec_rcvd_stathandle
#undef pkts_rcvd_stathandle
#undef pktssec_rcvd_stathandle
#undef ete_delay_stathandle
#undef bits_rcvd_gstathandle
#undef bitssec_rcvd_gstathandle
#undef pkts_rcvd_gstathandle
#undef pktssec_rcvd_gstathandle
#undef ete_delay_gstathandle
#undef pkptr
#undef wpan_log
#undef co
#undef wpan_Cnctn_list
#undef wpan_Nds_list
#undef flowTables
#undef gtsChrstcs
#undef IS_TRANSMIT
#undef DecPkPtr
#undef Nodes
#undef obSDNC
#undef rerout
#undef EDBatteryDown
#undef FuzzyLogic
#undef RunTTL

/* Access from C kernel using C linkage */
extern "C"
{
	VosT_Obtype _op_SDNC_RoutingFlowTable_init (int * init_block_ptr);
	VosT_Address _op_SDNC_RoutingFlowTable_alloc (VosT_Obtype, int);
	void SDNC_RoutingFlowTable (OP_SIM_CONTEXT_ARG_OPT)
		{
		((SDNC_RoutingFlowTable_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))->SDNC_RoutingFlowTable (OP_SIM_CONTEXT_PTR_OPT);
		}

	void _op_SDNC_RoutingFlowTable_svar (void *, const char *, void **);

	void _op_SDNC_RoutingFlowTable_diag (OP_SIM_CONTEXT_ARG_OPT)
		{
		((SDNC_RoutingFlowTable_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))->_op_SDNC_RoutingFlowTable_diag (OP_SIM_CONTEXT_PTR_OPT);
		}

	void _op_SDNC_RoutingFlowTable_terminate (OP_SIM_CONTEXT_ARG_OPT)
		{
		/* The destructor is the Termination Block */
		delete (SDNC_RoutingFlowTable_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr);
		}


} /* end of 'extern "C"' */




/* Process model interrupt handling procedure */


void
SDNC_RoutingFlowTable_state::SDNC_RoutingFlowTable (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (SDNC_RoutingFlowTable_state::SDNC_RoutingFlowTable ());
	try
		{
		/* Temporary Variables */
		double		pk_size;
		double		ete_delay;
		char	pk_format[25];
		
		/* End of Temporary Variables */


		FSM_ENTER ("SDNC_RoutingFlowTable")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (IDLE) enter executives **/
			FSM_STATE_ENTER_UNFORCED (0, "IDLE", state0_enter_exec, "SDNC_RoutingFlowTable [IDLE enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (1,"SDNC_RoutingFlowTable")


			/** state (IDLE) exit executives **/
			FSM_STATE_EXIT_UNFORCED (0, "IDLE", "SDNC_RoutingFlowTable [IDLE exit execs]")


			/** state (IDLE) transition processing **/
			FSM_PROFILE_SECTION_IN ("SDNC_RoutingFlowTable [IDLE trans conditions]", state0_trans_conds)
			FSM_INIT_COND (INCOMING_PACKET)
			FSM_TEST_COND (PRNT_LST)
			FSM_TEST_COND (GTS_request)
			FSM_TEST_COND (TTL)
			FSM_TEST_LOGIC ("IDLE")
			FSM_PROFILE_SECTION_OUT (state0_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "INCOMING_PACKET", "", "IDLE", "PKT RCV", "tr_2", "SDNC_RoutingFlowTable [IDLE -> PKT RCV : INCOMING_PACKET / ]")
				FSM_CASE_TRANSIT (1, 4, state4_enter_exec, ;, "PRNT_LST", "", "IDLE", "PRNT LST", "tr_7", "SDNC_RoutingFlowTable [IDLE -> PRNT LST : PRNT_LST / ]")
				FSM_CASE_TRANSIT (2, 5, state5_enter_exec, ;, "GTS_request", "", "IDLE", "gts rqst", "tr_12_0", "SDNC_RoutingFlowTable [IDLE -> gts rqst : GTS_request / ]")
				FSM_CASE_TRANSIT (3, 6, state6_enter_exec, ;, "TTL", "", "IDLE", "TimeToLeave", "tr_14", "SDNC_RoutingFlowTable [IDLE -> TimeToLeave : TTL / ]")
				}
				/*---------------------------------------------------------*/



			/** state (INIT) enter executives **/
			FSM_STATE_ENTER_FORCED_NOLABEL (1, "INIT", "SDNC_RoutingFlowTable [INIT enter execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_RoutingFlowTable [INIT enter execs]", state1_enter_exec)
				{
				/* Initilaize the statistic handles to keep	*/
				/* track of traffic sinked by this process.	*/
				bits_rcvd_stathandle 		= op_stat_reg ("Traffic Sink.Traffic Received (bits)",			OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
				bitssec_rcvd_stathandle 	= op_stat_reg ("Traffic Sink.Traffic Received (bits/sec)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
				pkts_rcvd_stathandle 		= op_stat_reg ("Traffic Sink.Traffic Received (packets)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
				pktssec_rcvd_stathandle 	= op_stat_reg ("Traffic Sink.Traffic Received (packets/sec)",	OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
				ete_delay_stathandle		= op_stat_reg ("Traffic Sink.End-to-End Delay (seconds)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
				
				bits_rcvd_gstathandle 		= op_stat_reg ("Traffic Sink.Traffic Received (bits)",			OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
				bitssec_rcvd_gstathandle 	= op_stat_reg ("Traffic Sink.Traffic Received (bits/sec)",		OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
				pkts_rcvd_gstathandle 		= op_stat_reg ("Traffic Sink.Traffic Received (packets)",		OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
				pktssec_rcvd_gstathandle 	= op_stat_reg ("Traffic Sink.Traffic Received (packets/sec)",	OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
				ete_delay_gstathandle		= op_stat_reg ("Traffic Sink.End-to-End Delay (seconds)",		OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
				
				NIntel_log_file_init();
				co = 1;
				RunTTL=0;
				EDBatteryDown=NOT_DEFINED;
				rerout=0;
				wpan_Cnctn_list = op_prg_list_create();
				wpan_Nds_list = op_prg_list_create();
				//op_intrpt_schedule_self (op_sim_time() + 7, PRINT_LIST_CODE);
				//op_intrpt_schedule_self (op_sim_time() + 6, DijkstraFuzzyCode);
						
				IS_TRANSMIT = OPC_FALSE;
				//flowTables = op_prg_list_create();
				//gtsChrstcs = op_prg_list_create();
				obSDNC = new SDNBWSANSDNControllerClass();
				
				Objid selfID;
				Objid parentID;
				Objid grandfatherID;
				selfID = op_id_self();
				parentID = op_topo_parent(selfID);
				grandfatherID = op_topo_parent(parentID);
				
				op_ima_obj_attr_get (selfID, "FuzzyOption", &FuzzyLogic);
				}
				FSM_PROFILE_SECTION_OUT (state1_enter_exec)

			/** state (INIT) exit executives **/
			FSM_STATE_EXIT_FORCED (1, "INIT", "SDNC_RoutingFlowTable [INIT exit execs]")


			/** state (INIT) transition processing **/
			FSM_TRANSIT_FORCE (0, state0_enter_exec, ;, "default", "", "INIT", "IDLE", "tr_12", "SDNC_RoutingFlowTable [INIT -> IDLE : default / ]")
				/*---------------------------------------------------------*/



			/** state (PKT RCV) enter executives **/
			FSM_STATE_ENTER_FORCED (2, "PKT RCV", state2_enter_exec, "SDNC_RoutingFlowTable [PKT RCV enter execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_RoutingFlowTable [PKT RCV enter execs]", state2_enter_exec)
				{
				/* Obtain the incoming packet.	*/
				pkptr = op_pk_get (op_intrpt_strm ());
				/* Caclulate metrics to be updated.		*/
				pk_size = (double) op_pk_total_size_get (pkptr);
				ete_delay = op_sim_time () - op_pk_creation_time_get (pkptr);
				
				/* Update local statistics.				*/
				op_stat_write (bits_rcvd_stathandle, 		pk_size);
				op_stat_write (pkts_rcvd_stathandle, 		1.0);
				op_stat_write (ete_delay_stathandle, 		ete_delay);
				
				op_stat_write (bitssec_rcvd_stathandle, 	pk_size);
				op_stat_write (bitssec_rcvd_stathandle, 	0.0);
				op_stat_write (pktssec_rcvd_stathandle, 	1.0);
				op_stat_write (pktssec_rcvd_stathandle, 	0.0);
				
				/* Update global statistics.	*/
				op_stat_write (bits_rcvd_gstathandle, 		pk_size);
				op_stat_write (pkts_rcvd_gstathandle, 		1.0);
				op_stat_write (ete_delay_gstathandle, 		ete_delay);
				
				op_stat_write (bitssec_rcvd_gstathandle, 	pk_size);
				op_stat_write (bitssec_rcvd_gstathandle, 	0.0);
				op_stat_write (pktssec_rcvd_gstathandle, 	1.0);
				op_stat_write (pktssec_rcvd_gstathandle, 	0.0);
				
				/* Destroy the received packet.	*/
				
				if (RunTTL == 0)// only for one time
					{ 	
					//send one time only 
					op_intrpt_schedule_self ((op_sim_time() + op_dist_uniform (1.0)), TTLCode);
					RunTTL = 1;
				
					}
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** state (PKT RCV) exit executives **/
			FSM_STATE_EXIT_FORCED (2, "PKT RCV", "SDNC_RoutingFlowTable [PKT RCV exit execs]")


			/** state (PKT RCV) transition processing **/
			FSM_TRANSIT_FORCE (3, state3_enter_exec, ;, "default", "", "PKT RCV", "sv to Cnctns", "tr_4", "SDNC_RoutingFlowTable [PKT RCV -> sv to Cnctns : default / ]")
				/*---------------------------------------------------------*/



			/** state (sv to Cnctns) enter executives **/
			FSM_STATE_ENTER_FORCED (3, "sv to Cnctns", state3_enter_exec, "SDNC_RoutingFlowTable [sv to Cnctns enter execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_RoutingFlowTable [sv to Cnctns enter execs]", state3_enter_exec)
				{
				Packet* command_frame;
				int comfrid;
				int source_address;
				//25920
				float   threshold=2;
				// change int to float for battery in fuzzy 
				
				int batteryChanged=NOT_DEFINED;
				
				// we recieve node status via command frame
				//wpan_status_values* wpan_status_values_ptr;
				if (op_pk_nfd_get_pkt (pkptr, "MSDU", &command_frame) == OPC_COMPCODE_SUCCESS )
					{
					
					op_pk_format(command_frame,pk_format);
				
					op_pk_nfd_get (pkptr, "Source Address", &source_address);
				 
				if (strcmp("wpan_command_frame_format",pk_format)==0)
					{ op_pk_nfd_get(command_frame,"Command Frame Identifier", &comfrid);
					
					if(comfrid == STATUS_REPORT){
					// this for sending statuses to topology discovery process TDP
					/// here to update the neighbours first we delete the older ones from Nodes list 
					Packet *FrameCopy;
					Packet *commandCopy;
					FrameCopy = op_pk_copy (pkptr);
					op_pk_nfd_set (FrameCopy, "Source Address", source_address);
					commandCopy = op_pk_copy (command_frame);
					op_pk_nfd_set_pkt (FrameCopy, "MSDU", commandCopy);
					op_pk_send(FrameCopy, ToTDProcess);
					/************************************************************/
					SDNWSAN_status_values* SDNWSAN_status_values_ptr;// = new SDNWSAN_status_values ; 
					int s=-2; 
					if (op_pk_nfd_get_ptr (command_frame, "Command Payload", (void**)&SDNWSAN_status_values_ptr)!= OPC_COMPCODE_FAILURE)
						{
						//newBattery=SDNWSAN_status_values_ptr->energy_level;
						
						for(int i=0;i<Nodes.size();i++)
							if(Nodes[i]->nodeAddress==source_address)// update ED  info  or niegbour info
							{
							s=1;
							//overwrite the data arrived
							Nodes[i]->energy_level=SDNWSAN_status_values_ptr->energy_level;
							Nodes[i]->parentAddress=  SDNWSAN_status_values_ptr->parentAddress;
							Nodes[i]->timeToLeave+=10;//update ttl value
				     		fprintf (wpan_log, "Update TTL by 10 %%%%% t = %f  node address= %d Time to Leave  = %d %%%%% \n\n", op_sim_time(), Nodes[i]->nodeAddress, Nodes[i]->timeToLeave);
							fprintf (wpan_log, "t = %f  node address= %d before_threshold= %f \n", op_sim_time(), Nodes[i]->nodeAddress,threshold);				 
							 
							// here we should delete the old neighbours then add the arrival as new
							/// here we will update the SNR of the neighbours
							for (int j=0; j<SDNWSAN_status_values_ptr->neighbours_no; j++) { // update SNR VALUE
								if(SDNWSAN_status_values_ptr->nighbourList[j].SNR<=1)
									SDNWSAN_status_values_ptr->nighbourList[j].SNR=1;	
								
								Nodes[i]->nighbourList[j].SNR =SDNWSAN_status_values_ptr->nighbourList[j].SNR;
						     
								}  
							// when node's battery fel down under the threshold 
							if (Nodes[i]->energy_level< threshold )  { 	
								batteryChanged=1; 				rerout++;
								fprintf (wpan_log, "t = %f passed rerout=%d ==> passed_threshold == %f node address=%d battery=%f \n", op_sim_time(),rerout, threshold, Nodes[i]->nodeAddress,Nodes[i]->energy_level);
							    // reselect path to avoid this ED and get another path without it 
								RunDijkstraFuzzy(Nodes[i]->nodeAddress,12,batteryChanged); 
								}
				
							
							}// end if src
							
						if(s==-2){ // if it is new node
							
							Node* NewNode=new Node(SDNWSAN_status_values_ptr->nodeAddress);
							NewNode->nodeAddress=SDNWSAN_status_values_ptr->nodeAddress;
							NewNode->energy_level=SDNWSAN_status_values_ptr->energy_level;
							NewNode->neighbours_no=  SDNWSAN_status_values_ptr->neighbours_no;
							NewNode->parentAddress=  SDNWSAN_status_values_ptr->parentAddress;
							
							NewNode->timeToLeave=120;//added new
							
						  	for (int j=0; j<SDNWSAN_status_values_ptr->neighbours_no; j++){  
							//cout<<"*********************************SDNWSAN_status_values_ptr->neighbours_no="<<SDNWSAN_status_values_ptr->neighbours_no<<endl;
							// new node for each niegbour 
							NewNode->nighbourList[j].node_address =SDNWSAN_status_values_ptr->nighbourList[j].node_address;
							if(SDNWSAN_status_values_ptr->nighbourList[j].SNR<=1)
									SDNWSAN_status_values_ptr->nighbourList[j].SNR=1;
							NewNode->nighbourList[j].SNR =SDNWSAN_status_values_ptr->nighbourList[j].SNR;
					
							}
							Nodes.push_back(NewNode);
							//delete NewNode;
								
							}
						fprintf (wpan_log, "********************************************************************************\n");
						fprintf (wpan_log, "********************* Status information of the current ************************\n");		
						fprintf (wpan_log, "********************* status report of node address %d *************************\n", source_address);		
						fprintf (wpan_log, "t = %f -> The energy level is		  %f \n", op_sim_time(), SDNWSAN_status_values_ptr->energy_level);	
						fprintf (wpan_log, "t = %f -> Number of neighbours is 	  %d \n", op_sim_time(), SDNWSAN_status_values_ptr->neighbours_no);
						fprintf (wpan_log, "t = %f -> parent address is		 	  %d \n", op_sim_time(), SDNWSAN_status_values_ptr->parentAddress);	
						fprintf (wpan_log, "*******************  The neighbour node list of this node **********************\n");	
						for (int i=0; i<SDNWSAN_status_values_ptr->neighbours_no; i++) {
						fprintf (wpan_log, "%d. node address: %d of SNR of: %f  \n", i,  SDNWSAN_status_values_ptr->nighbourList[i].node_address, SDNWSAN_status_values_ptr->nighbourList[i].SNR);
						}
						}
						op_prg_mem_free(SDNWSAN_status_values_ptr);
					}
					else if (comfrid == APP_ROUTE_REQ) {
							checkNeighboursParents();
							ApplicationID++;
							obSDNC->CreateApplication(ApplicationID,source_address);
							printf("con_req_prog: SDNC NeIntel incoming command from end device command ID is %d\n\n", comfrid);
							fprintf (wpan_log, "\n con_req_prog: SDNC NeIntel incoming command from end device command ID is %d \n\n", comfrid);	
							RunDijkstraFuzzy(source_address,12,NOT_DEFINED);
					}
					}
				op_pk_destroy(command_frame);
						}
				}
				FSM_PROFILE_SECTION_OUT (state3_enter_exec)

			/** state (sv to Cnctns) exit executives **/
			FSM_STATE_EXIT_FORCED (3, "sv to Cnctns", "SDNC_RoutingFlowTable [sv to Cnctns exit execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_RoutingFlowTable [sv to Cnctns exit execs]", state3_exit_exec)
				{
				op_pk_destroy (pkptr);
				
					if (IS_TRANSMIT)
						{
						GTS_request_decision();
						
						//PrgT_List* fttt;
						//fttt = flowTables;
						DecPkPtr = op_pk_create_fmt ("NeIntelDecision");
						//op_pk_nfd_set_ptr (DecPkPtr, "FlowTables", flowTables, op_prg_mem_copy_create, op_prg_mem_free, sizeof (flow_table));
						//op_pk_nfd_set_ptr (DecPkPtr, "FlowTables", flowTables, flowTables_nested_list_mem_copy_create, gts_nested_list_mem_free, sizeof(flow_table)*op_prg_list_size (flowTables));
						op_pk_nfd_set_ptr (DecPkPtr, "FlowTables", flowTables, op_prg_mem_copy_create, gts_nested_list_mem_free, sizeof(flow_table)*op_prg_list_size (flowTables));
						op_pk_nfd_set_ptr (DecPkPtr, "GtsCharacteristics", gtsChrstcs, op_prg_mem_copy_create, op_prg_mem_free, sizeof (node_gts_chrcts));
						op_pk_send(DecPkPtr, TO_MAC);
				
					}
				
				}
				FSM_PROFILE_SECTION_OUT (state3_exit_exec)


			/** state (sv to Cnctns) transition processing **/
			FSM_TRANSIT_FORCE (0, state0_enter_exec, ;, "default", "", "sv to Cnctns", "IDLE", "tr_11", "SDNC_RoutingFlowTable [sv to Cnctns -> IDLE : default / ]")
				/*---------------------------------------------------------*/



			/** state (PRNT LST) enter executives **/
			FSM_STATE_ENTER_FORCED (4, "PRNT LST", state4_enter_exec, "SDNC_RoutingFlowTable [PRNT LST enter execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_RoutingFlowTable [PRNT LST enter execs]", state4_enter_exec)
				{
				
				
				//op_intrpt_schedule_self (op_sim_time() + 2.5, PRINT_LIST_CODE);
				//printNSlistToLogFile(wpan_Cnctn_list,wpan_Nds_list);
						fprintf (wpan_log,"********************************************************************************\n");
						fprintf (wpan_log,"********************* following Status information print list ************************\n");		
						fprintf (wpan_log,"********************* total number of nodes %d *************************\n", Nodes.size());	
				
					for (int i = 0; i < Nodes.size(); i++)
						{
						fprintf (wpan_log,"**** Node of address %d ******************************************\n", Nodes[i]->nodeAddress);
						fprintf (wpan_log,"t = %f -> The energy level is		  %f \n", op_sim_time(), Nodes[i]->energy_level);	
						fprintf (wpan_log,"t = %f -> Number of neighbours is 	  %d \n", op_sim_time(), Nodes[i]->neighbours_no);
						fprintf (wpan_log,"t = %f -> parent address is		 	  %d \n", op_sim_time(), Nodes[i]->parentAddress);	
						fprintf (wpan_log,"*******************  The neighbour node list of this node **********************\n");	
						for (int j=0; j < Nodes[i]->neighbours_no; j++) {
						fprintf (wpan_log,"%d. node address: %d of rssi of: %f  \n", j+1,  Nodes[i]->nighbourList[j].node_address, Nodes[i]->nighbourList[j].SNR);
						}
						}
				}
				FSM_PROFILE_SECTION_OUT (state4_enter_exec)

			/** state (PRNT LST) exit executives **/
			FSM_STATE_EXIT_FORCED (4, "PRNT LST", "SDNC_RoutingFlowTable [PRNT LST exit execs]")


			/** state (PRNT LST) transition processing **/
			FSM_TRANSIT_FORCE (0, state0_enter_exec, ;, "default", "", "PRNT LST", "IDLE", "tr_8", "SDNC_RoutingFlowTable [PRNT LST -> IDLE : default / ]")
				/*---------------------------------------------------------*/



			/** state (gts rqst) enter executives **/
			FSM_STATE_ENTER_FORCED (5, "gts rqst", state5_enter_exec, "SDNC_RoutingFlowTable [gts rqst enter execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_RoutingFlowTable [gts rqst enter execs]", state5_enter_exec)
				{
				GTS_request_decision();
				}
				FSM_PROFILE_SECTION_OUT (state5_enter_exec)

			/** state (gts rqst) exit executives **/
			FSM_STATE_EXIT_FORCED (5, "gts rqst", "SDNC_RoutingFlowTable [gts rqst exit execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_RoutingFlowTable [gts rqst exit execs]", state5_exit_exec)
				{
				flow_table* flow_table_ptr;
				node_gts_chrcts* node_gts_chrcts_ptr;
				int i;
				
					fprintf (wpan_log, "********************************************************************************\n");
					fprintf (wpan_log, "********************* flow table ***********************************************\n");
					
					for (i=0; i<op_prg_list_size (flowTables); i++) {
						if ((flow_table_ptr = (flow_table *) op_prg_list_access (flowTables, i)) != OPC_NIL)
							{
							fprintf (wpan_log, "----------------------------- %d --------------------------------- \n", i);
							fprintf (wpan_log, "t = %f -> flow table of node of address %d \n", op_sim_time(), flow_table_ptr->node_address);	
							fprintf (wpan_log, "t = %f -> source condition is %d \n", op_sim_time(), flow_table_ptr->src);
							fprintf (wpan_log, "t = %f -> action is %d \n", op_sim_time(), flow_table_ptr->act);
							fprintf (wpan_log, "t = %f -> destination is %d \n", op_sim_time(), flow_table_ptr->dest);
							
							}
						}
					fprintf (wpan_log, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
					fprintf (wpan_log, "&&&&&&&&&&&&&&&&&&&&&& gts characteristics list&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
				
					for (i=0; i<op_prg_list_size (gtsChrstcs); i++) {
						if ((node_gts_chrcts_ptr = (node_gts_chrcts *) op_prg_list_access (gtsChrstcs, i)) != OPC_NIL)
							{
							fprintf (wpan_log, "----------------------------- %d --------------------------------- \n", i);
							fprintf (wpan_log, "t = %f -> gts characteristics of node of address %d \n", op_sim_time(), node_gts_chrcts_ptr->node_address);	
							fprintf (wpan_log, "t = %f -> gts type (0 allocation/ 1 deallocation) is %d \n", op_sim_time(), node_gts_chrcts_ptr->type);
							fprintf (wpan_log, "t = %f -> gts direction (0 transmit/ 1 receive) is %d \n", op_sim_time(), node_gts_chrcts_ptr->direction);
							fprintf (wpan_log, "t = %f -> no. of gts slots is %d \n", op_sim_time(), node_gts_chrcts_ptr->length);
							
					}
						}
					
					
					fprintf (wpan_log, "********************************************************************************\n");
					fprintf (wpan_log, "********************************************************************************\n");
					
				/* send flow tables and gts characteristics decisions to mac  */
					DecPkPtr = op_pk_create_fmt ("NeIntelDecision");
					op_pk_nfd_set_ptr (DecPkPtr, "FlowTables", flowTables, op_prg_mem_copy_create, op_prg_mem_free, sizeof (flow_table));
					op_pk_nfd_set_ptr (DecPkPtr, "GtsCharacteristics", gtsChrstcs, op_prg_mem_copy_create, op_prg_mem_free, sizeof (node_gts_chrcts));
					op_pk_send(DecPkPtr, TO_MAC);
					IS_TRANSMIT = OPC_TRUE;
					
				
					
					
					
					
					
					
				}
				FSM_PROFILE_SECTION_OUT (state5_exit_exec)


			/** state (gts rqst) transition processing **/
			FSM_TRANSIT_FORCE (0, state0_enter_exec, ;, "default", "", "gts rqst", "IDLE", "tr_12_1", "SDNC_RoutingFlowTable [gts rqst -> IDLE : default / ]")
				/*---------------------------------------------------------*/



			/** state (TimeToLeave) enter executives **/
			FSM_STATE_ENTER_FORCED (6, "TimeToLeave", state6_enter_exec, "SDNC_RoutingFlowTable [TimeToLeave enter execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_RoutingFlowTable [TimeToLeave enter execs]", state6_enter_exec)
				{
				 // Here how we decrement TLL value every 10 seconds
				
				 for(int i=0;i<Nodes.size();i++)
				 {
				 Nodes[i]->timeToLeave--;//update ttl value
				 /*
				 if(Nodes[i]->nodeAddress==9 && Nodes[i]->timeToLeave<50) 
					{
						DisableNode();	//hbi
					}
				 */
				 fprintf (wpan_log, " timeToLeave-- %%%%% t = %f  node address= %d Time to Leave  = %d %%%%% \n\n", op_sim_time(), Nodes[i]->nodeAddress, Nodes[i]->timeToLeave);
				 }
				 
				 op_intrpt_schedule_self (op_sim_time() + 10, TTLCode);
				
				}
				FSM_PROFILE_SECTION_OUT (state6_enter_exec)

			/** state (TimeToLeave) exit executives **/
			FSM_STATE_EXIT_FORCED (6, "TimeToLeave", "SDNC_RoutingFlowTable [TimeToLeave exit execs]")


			/** state (TimeToLeave) transition processing **/
			FSM_TRANSIT_FORCE (0, state0_enter_exec, ;, "default", "", "TimeToLeave", "IDLE", "tr_15", "SDNC_RoutingFlowTable [TimeToLeave -> IDLE : default / ]")
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (1,"SDNC_RoutingFlowTable")
		}
	catch (...)
		{
		Vos_Error_Print (VOSC_ERROR_ABORT,
			(const char *)VOSC_NIL,
			"Unhandled C++ exception in process model (SDNC_RoutingFlowTable)",
			(const char *)VOSC_NIL, (const char *)VOSC_NIL);
		}
	}




void
SDNC_RoutingFlowTable_state::_op_SDNC_RoutingFlowTable_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}

void
SDNC_RoutingFlowTable_state::operator delete (void* ptr)
	{
	FIN (SDNC_RoutingFlowTable_state::operator delete (ptr));
	Vos_Poolmem_Dealloc (ptr);
	FOUT
	}

SDNC_RoutingFlowTable_state::~SDNC_RoutingFlowTable_state (void)
	{

	FIN (SDNC_RoutingFlowTable_state::~SDNC_RoutingFlowTable_state ())


	/* No Termination Block */


	FOUT
	}


#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

void *
SDNC_RoutingFlowTable_state::operator new (size_t)
#if defined (VOSD_NEW_BAD_ALLOC)
		throw (VOSD_BAD_ALLOC)
#endif
	{
	void * new_ptr;

	FIN_MT (SDNC_RoutingFlowTable_state::operator new ());

	new_ptr = Vos_Alloc_Object (SDNC_RoutingFlowTable_state::obtype);
#if defined (VOSD_NEW_BAD_ALLOC)
	if (new_ptr == VOSC_NIL) throw VOSD_BAD_ALLOC();
#endif
	FRET (new_ptr)
	}

/* State constructor initializes FSM handling */
/* by setting the initial state to the first */
/* block of code to enter. */

SDNC_RoutingFlowTable_state::SDNC_RoutingFlowTable_state (void) :
		_op_current_block (2)
	{
#if defined (OPD_ALLOW_ODB)
		_op_current_state = "SDNC_RoutingFlowTable [INIT enter execs]";
#endif
	}

VosT_Obtype
_op_SDNC_RoutingFlowTable_init (int * init_block_ptr)
	{
	FIN_MT (_op_SDNC_RoutingFlowTable_init (init_block_ptr))

	SDNC_RoutingFlowTable_state::obtype = Vos_Define_Object_Prstate ("proc state vars (SDNC_RoutingFlowTable)",
		sizeof (SDNC_RoutingFlowTable_state));
	*init_block_ptr = 2;

	FRET (SDNC_RoutingFlowTable_state::obtype)
	}

VosT_Address
_op_SDNC_RoutingFlowTable_alloc (VosT_Obtype, int)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	SDNC_RoutingFlowTable_state * ptr;
	FIN_MT (_op_SDNC_RoutingFlowTable_alloc ())

	/* New instance will have FSM handling initialized */
#if defined (VOSD_NEW_BAD_ALLOC)
	try {
		ptr = new SDNC_RoutingFlowTable_state;
	} catch (const VOSD_BAD_ALLOC &) {
		ptr = VOSC_NIL;
	}
#else
	ptr = new SDNC_RoutingFlowTable_state;
#endif
	FRET ((VosT_Address)ptr)
	}



void
_op_SDNC_RoutingFlowTable_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	SDNC_RoutingFlowTable_state		*prs_ptr;

	FIN_MT (_op_SDNC_RoutingFlowTable_svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (SDNC_RoutingFlowTable_state *)gen_ptr;

	if (strcmp ("bits_rcvd_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bits_rcvd_stathandle);
		FOUT
		}
	if (strcmp ("bitssec_rcvd_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bitssec_rcvd_stathandle);
		FOUT
		}
	if (strcmp ("pkts_rcvd_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pkts_rcvd_stathandle);
		FOUT
		}
	if (strcmp ("pktssec_rcvd_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pktssec_rcvd_stathandle);
		FOUT
		}
	if (strcmp ("ete_delay_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->ete_delay_stathandle);
		FOUT
		}
	if (strcmp ("bits_rcvd_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bits_rcvd_gstathandle);
		FOUT
		}
	if (strcmp ("bitssec_rcvd_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bitssec_rcvd_gstathandle);
		FOUT
		}
	if (strcmp ("pkts_rcvd_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pkts_rcvd_gstathandle);
		FOUT
		}
	if (strcmp ("pktssec_rcvd_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pktssec_rcvd_gstathandle);
		FOUT
		}
	if (strcmp ("ete_delay_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->ete_delay_gstathandle);
		FOUT
		}
	if (strcmp ("pkptr" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pkptr);
		FOUT
		}
	if (strcmp ("wpan_log" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->wpan_log);
		FOUT
		}
	if (strcmp ("co" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->co);
		FOUT
		}
	if (strcmp ("wpan_Cnctn_list" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->wpan_Cnctn_list);
		FOUT
		}
	if (strcmp ("wpan_Nds_list" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->wpan_Nds_list);
		FOUT
		}
	if (strcmp ("flowTables" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->flowTables);
		FOUT
		}
	if (strcmp ("gtsChrstcs" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->gtsChrstcs);
		FOUT
		}
	if (strcmp ("IS_TRANSMIT" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->IS_TRANSMIT);
		FOUT
		}
	if (strcmp ("DecPkPtr" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->DecPkPtr);
		FOUT
		}
	if (strcmp ("Nodes" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->Nodes);
		FOUT
		}
	if (strcmp ("obSDNC" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->obSDNC);
		FOUT
		}
	if (strcmp ("rerout" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->rerout);
		FOUT
		}
	if (strcmp ("EDBatteryDown" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->EDBatteryDown);
		FOUT
		}
	if (strcmp ("FuzzyLogic" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->FuzzyLogic);
		FOUT
		}
	if (strcmp ("RunTTL" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->RunTTL);
		FOUT
		}
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

