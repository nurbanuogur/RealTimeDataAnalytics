/* Process model C++ form file: EDOpenFlow.pr.cpp */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char EDOpenFlow_pr_cpp [] = "MIL_3_Tfile_Hdr_ 145A 30A op_runsim 7 5BBF6885 5BBF6885 1 HBI-JCGS160 Mohammed@Hussein 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1e80 8                                                                                                                                                                                                                                                                                                                                                                                           ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

/* Include files					*/
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <vector>
using namespace std;
#include "headers\wpan_struct.h"
#include "headers\wpan_params.h"
#include "headers\wpan_math.h"

/* Node configuration constants		*/


/* Special attribute values			*/


/* Interrupt code values			*/
#define APP_REQ_CODE						9
#define START_APP_CODE						13

/* Input and output streams		*/

#define	STRM_FROM_MAC_TO_PKTD				3
#define	STRM_FROM_MAC_TO_Flow				0
#define	STRM_FROM_GTS_TO_PKTD				4

#define	STRM_FROM_PKTD_TO_UPL				2
#define	STRM_FROM_PKTD_TO_MAC				1
#define STRM_FROM_BE_TO_PKTD				1


/* types of incoming streams CAP and CFP*/
#define CAP_TYPE		0
#define CFP_TYPE		1
#define SLEEP_PERIOD	2

#define FLOW_TABLE_TO_PCKT_DECISION 701

/* Flow list action codes */
#define ACT_DROP			0
#define ACT_FORWARD_MAC  	1
#define ACT_FORWARD_UP		2

/* State machine conditions 		*/
#define IAM_PAN_COORDINATOR 	(device_mode == PAN_COORDINATOR)
#define IAM_ROUTER 				(device_mode == ROUTER)
#define IAM_END_DEVICE 			(device_mode == END_DEVICE)

#define RECEIVE_PCK_FROM_GTS	(intrpt_type == OPC_INTRPT_STRM && intrpt_strm == STRM_FROM_GTS_TO_PKTD)
#define RECEIVE_PCK_FROM_BE		(intrpt_type == OPC_INTRPT_STRM && intrpt_strm == STRM_FROM_BE_TO_PKTD)
#define RECEIVE_PCK_FROM_MAC	(intrpt_type == OPC_INTRPT_STRM && intrpt_strm == STRM_FROM_MAC_TO_PKTD)
#define INCOMING_FLOW_TABLE 	(intrpt_type == OPC_INTRPT_STRM && intrpt_strm == STRM_FROM_MAC_TO_Flow)
#define CONNECTION_REQ_FROM_APP	(intrpt_type == OPC_INTRPT_REMOTE && intrpt_code == APP_REQ_CODE)
/* Route List (flow table) sturct definition */
typedef struct {
	int Source_Condition;	// if entry from this source address
	int Action; 			// then take this action (F forward/D drop)
	int Decided_Destination;// if forward then route to this destination address
	int Relative_Statistics;// timer of this rule if note update after this time, this entry will expire and deleted
} SDN_Route_List;

// flow entries structure (new)
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
	
/* Function prototypes.				*/
static void wpan_nwk_init (void);  
static void wpan_print_parameters (void);
static void wpan_receive_pck_from_gts (void);
static void wpan_receive_pck_from_unack (void);
static void wpan_receive_pck_from_mac (void);
static int wpan_next_hop_get (int);
static void wpan_receive_pck_from_ack ();


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
class EDOpenFlow_state
	{
	private:
		/* Internal state tracking for FSM */
		FSM_SYS_STATE

	public:
		EDOpenFlow_state (void);

		/* Destructor contains Termination Block */
		~EDOpenFlow_state (void);

		/* State Variables */
		char	                   		device_mode[20]                                 ;
		char	                   		device_name[20]                                 ;
		int	                    		my_address                                      ;
		Boolean	                		enable_log                                      ;
		int	                    		intrpt_type                                     ;
		int	                    		intrpt_strm                                     ;
		int	                    		intrpt_code                                     ;
		Stathandle	             		ete_delay_be_stathandle                         ;
		Stathandle	             		ete_delay_rt_stathandle                         ;
		FILE*	                  		log                                             ;
		Stathandle	             		pktssec_rcvd_stathandle                         ;
		FlowEntery	             		flow_table_ptr                                  ;
		vector<FlowEntery*>	    		NodeFlowEntries                                 ;
		Objid	                  		parent_id                                       ;
		Boolean	                		con_req_on_hold                                 ;
		int	                    		receiveFromSRC_BE                               ;
		int	                    		receiveFromMAC                                  ;
		int	                    		receiveFromMACtoAPP                             ;

		/* FSM code */
		void EDOpenFlow (OP_SIM_CONTEXT_ARG_OPT);
		/* Diagnostic Block */
		void _op_EDOpenFlow_diag (OP_SIM_CONTEXT_ARG_OPT);

#if defined (VOSD_NEW_BAD_ALLOC)
		void * operator new (size_t) throw (VOSD_BAD_ALLOC);
#else
		void * operator new (size_t);
#endif
		void operator delete (void *);

		/* Memory management */
		static VosT_Obtype obtype;
	};

VosT_Obtype EDOpenFlow_state::obtype = (VosT_Obtype)OPC_NIL;

#define pr_state_ptr            		((EDOpenFlow_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))
#define device_mode             		pr_state_ptr->device_mode
#define device_name             		pr_state_ptr->device_name
#define my_address              		pr_state_ptr->my_address
#define enable_log              		pr_state_ptr->enable_log
#define intrpt_type             		pr_state_ptr->intrpt_type
#define intrpt_strm             		pr_state_ptr->intrpt_strm
#define intrpt_code             		pr_state_ptr->intrpt_code
#define ete_delay_be_stathandle 		pr_state_ptr->ete_delay_be_stathandle
#define ete_delay_rt_stathandle 		pr_state_ptr->ete_delay_rt_stathandle
#define log                     		pr_state_ptr->log
#define pktssec_rcvd_stathandle 		pr_state_ptr->pktssec_rcvd_stathandle
#define flow_table_ptr          		pr_state_ptr->flow_table_ptr
#define NodeFlowEntries         		pr_state_ptr->NodeFlowEntries
#define parent_id               		pr_state_ptr->parent_id
#define con_req_on_hold         		pr_state_ptr->con_req_on_hold
#define receiveFromSRC_BE       		pr_state_ptr->receiveFromSRC_BE
#define receiveFromMAC          		pr_state_ptr->receiveFromMAC
#define receiveFromMACtoAPP     		pr_state_ptr->receiveFromMACtoAPP

/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	EDOpenFlow_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((EDOpenFlow_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


/* Function Block */

#if !defined (VOSD_NO_FIN)
enum { _op_block_origin = __LINE__ + 2};
#endif

/*
                           GNU General Public License  

IEEE 802.15.4/ZigBee OPNET Simulation Model is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later version.
 
IEEE 802.15.4/ZigBee OPNET Simulation Model is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with IEEE 802.15.4/ZigBee OPNET Simulation Model; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 

*********************************************************************************/

/*--------------------------------------------------------------------------------
 * Function:	wpan_nwk_init
 *
 * Description:	- initialize the process
 *				- read the values of attributes
 *              - schedule a self interrupts 
 *
 * No parameters
 *--------------------------------------------------------------------------------*/

static void wpan_nwk_init () {

	Objid sl;
	char directory_path_name[120];
	char log_name[132];
	
	/* Stack tracing enrty point */
	FIN(wpan_nwk_init);
	
	/* obtain self object ID of the surrounding processor or queue */

	/* obtain object ID of the parent object (node) */
	parent_id = op_topo_parent (op_id_self ());
	
	/* get value to check if this node is PAN coordinator, Router, or End node */
	op_ima_obj_attr_get (parent_id, "Device Mode", &device_mode);
	
	/* get name of the node */
	op_ima_obj_attr_get (parent_id, "name", device_name);
	
	op_ima_obj_attr_get (parent_id, "Enable Logging", &enable_log);
	
	/* get address of the node */
	op_ima_obj_attr_get (parent_id, "MAC Address", &my_address);
	sl = op_id_from_name (parent_id, OPC_OBJTYPE_QUEUE, "wpan_mac");
	 
	if (my_address == -2) {
		my_address = sl;
	}


	
	op_ima_obj_attr_get (parent_id, "Log File Directory", directory_path_name);
	if (enable_log) {
	/* verification if the directory_path_name is a valid directory */
	if (prg_path_name_is_dir (directory_path_name) == PrgC_Path_Name_Is_Not_Dir) {
		char msg[128];		
		sprintf (msg, " \"%s\" is not valid directory name. The output will not be logged.\n", directory_path_name); 
		/* Display an appropriate warning.	*/
		op_prg_odb_print_major ("Warning from wpan_mac process: ", msg, OPC_NIL);		
	}	
	
		sprintf (log_name, "%s%s%s.ak", directory_path_name, device_name, "Packet_decision"); 
		//log_name = "c:\packetDecision.ak";
		log = fopen(log_name,"w");
		fprintf (log, "|-----------------------------------------------------------------------------|\n");
		fprintf (log, "| Packet decision process \n");
		}
	/* get network parameters */
	
	/* Stack tracing exit point */
	FOUT;
}

/*--------------------------------------------------------------------------------
 * Function:	 wpan_print_parameters
 *
 * Description:	Print the parameters.
 *				
 * No parameters
 *--------------------------------------------------------------------------------*/

static void wpan_print_parameters () {

	/* Stack tracing enrty point */	
	FIN(wpan_print_parameters);

	if (enable_log) {		
		printf ("|--------------------------- NETWORK LAYER -----------------------------------|\n");
		printf (" [Node %s (#%d)] \n", device_name, my_address);
		printf ("|-----------------------------------------------------------------------------|\n\n");
		fprintf (log, "|--------------------------- NETWORK LAYER -----------------------------------|\n");
		fprintf (log, " [Node %s (#%d)] \n", device_name, my_address);
		fprintf (log, "|-----------------------------------------------------------------------------|\n\n");
		
	}
	
	/* Stack tracing exit point */
	FOUT;
}



static void wpan_receive_pck_from_gts () {

	Packet* packet_ptr;	
	int ack_request;
	Ici* iciptr;
	/* Stack tracing enrty point */	
	FIN(wpan_receive_pck_from_gts);
	
	/* get the ICI information */
		iciptr = op_intrpt_ici ();
	
	/* Get packet from the input stream  */
		packet_ptr = op_pk_get (intrpt_strm);
		op_ici_attr_get (iciptr, "ACK", &ack_request);
		
		for(int i=0;i<NodeFlowEntries.size();i++)
		if (NodeFlowEntries[i]->srcAdrMatchRule == my_address)	
			{
				if (NodeFlowEntries[i]->action==1)
					{
						/* Update ICI */
						op_ici_attr_set (iciptr, "Next Hop Address", NodeFlowEntries[i]->NextHop);
						op_ici_attr_set (iciptr, "ACK", ack_request);
		
						/* send ICI to MAC layer together with packet. */
						op_ici_install (iciptr);
						
						op_pk_send (packet_ptr, STRM_FROM_PKTD_TO_MAC);
						op_ici_install (OPC_NIL);
						if (enable_log) {
								printf (" [%s (#%d)] t=%f -> (NWK) Packet is received from above intended to be sent \n", device_name, my_address, op_sim_time());
								printf ("\t + Packet is coming from #%d (destination of the next node is #%d) \n\n", my_address, NodeFlowEntries[i]->NextHop);
				
								}
					}
				else if (NodeFlowEntries[i]->action==0)
					{
						op_pk_destroy(packet_ptr);
					}
			}	else {op_pk_destroy(packet_ptr);}

	FOUT;
}

/*--------------------------------------------------------------------------------
 * Function:	 wpan_receive_pck_from_gts
 *
 * Description:	Received packet from gts Layer. Calculate next hop address and send it to
 *				to the MAC layer.
 *				
 * No parameters
 *--------------------------------------------------------------------------------*/

static void SDNWSAN_receive_pck_from_BE () {

	Packet* packet_ptr;	
//	int next_hop_address;
//	int* F_Dest_address;
	Ici* iciptr;
	/* Stack tracing enrty point */	
	FIN(SDNWSAN_receive_pck_from_BE);
	
	/* get the ICI information */
		iciptr = op_intrpt_ici ();
		//op_ici_attr_get (iciptr, "ACK", &ack);
	/* Get packet from the input stream  */
		packet_ptr = op_pk_get (intrpt_strm);
	Boolean entryFound = OPC_FALSE;
	for(int i=0;i<NodeFlowEntries.size();i++)
		if (NodeFlowEntries[i]->srcAdrMatchRule == my_address)
			{
				entryFound = OPC_TRUE;
				if (NodeFlowEntries[i]->action==1)
					{
					//receiveFromSRC_BE++;
					NodeFlowEntries[i]->statistics++; //=NodeFlowEntries[i]->statistics+receiveFromSRC_BE;
						/* Update ICI */
						op_ici_attr_set (iciptr, "Next Hop Address", NodeFlowEntries[i]->NextHop);
						op_ici_attr_set (iciptr, "Traffic Type", BEST_EFFORT_TRAFFIC);
						// set cluster direction
						op_ici_attr_set (iciptr, "clusterDir", NodeFlowEntries[i]->clusterDir);
						/* send ICI to MAC layer together with packet. */
						op_ici_install (iciptr);
						op_pk_send (packet_ptr, STRM_FROM_PKTD_TO_MAC);
						op_ici_install (OPC_NIL);
						if (enable_log) {
							fprintf (log," [%s (#%d)] t=%f -> (NWK) Packet is received from above intended to be sent \n", device_name, my_address, op_sim_time());
fprintf (log,"\t AppId=%d Packet is coming from #%d (destination of the next node is =%d) statistics=%d \n\n",NodeFlowEntries[i]->ApplicationID , my_address, NodeFlowEntries[i]->NextHop,NodeFlowEntries[i]->statistics);
				
							}
						
					}
				else if (NodeFlowEntries[i]->action==0)
					{
						op_pk_destroy(packet_ptr);
						// when we create flow table we should add rule for action drop
						// and add the statistics of action drop
						//NodeFlowEntries[i]->statistics++;
					}
				break;
				}
	
	if (!entryFound)
		{op_pk_destroy(packet_ptr);}
	FOUT;
	}


/*--------------------------------------------------------------------------------
 * Function:	 wpan_receive_pck_from_mac
 *
 * Description:	Received packet's payload (MSDU) from MAC Layer. Route or resend MSDU to Application layer. 
 *				
 * No parameters
 *--------------------------------------------------------------------------------*/

static void wpan_receive_pck_from_mac () {

	Packet* packet_ptr;
//	Packet* pkt_payload;
	int source_address;
	double e2e_delay;
	int traffic_type;
	int ack_request;
	Ici* iciptr;
	FIN(wpan_receive_pck_from_mac);
	
	
	
	/* get the ICI information */
	iciptr = op_intrpt_ici ();
	/*
	for(int i=0;i<NodeFlowEntries.size();i++){
		cout<<"Node="<<my_address<<"r Flow table size is="<< NodeFlowEntries.size()<<endl;
		cout<<"r srcAdrMatchRule="<<NodeFlowEntries[i]->srcAdrMatchRule<<endl;
		cout<<"r action="<< NodeFlowEntries[i]->action<<endl;
		cout<<"r NextNode="<< NodeFlowEntries[i]->NextHop<<endl;
		cout<<"r clusterDir="<< NodeFlowEntries[i]->clusterDir<<endl;
		} */
			/* Get packet from the input stream  */
	packet_ptr = op_pk_get (intrpt_strm);
	//cout<<"arrived from MAC to PKTD op_pk_print(packet_ptr) "<<endl;
	//op_pk_print(packet_ptr);
	
 //if (op_pk_nfd_get (packet_ptr, "Source Address", &source_address) == OPC_COMPCODE_SUCCESS)
	//{
	 
	/* get ici information concerning ACK request and traffic type (CAP CFT */	
	op_ici_attr_get (iciptr, "Traffic Type", &traffic_type);
	op_ici_attr_get (iciptr, "srcAddress", &source_address);
	op_ici_attr_get (iciptr, "ACK", &ack_request);
	e2e_delay = op_sim_time () - op_pk_stamp_time_get (packet_ptr);
		
	if (enable_log) {
		printf (" [%s (#%d)] t=%f -> (PKTD) Received packet - Flow list will be checked for further instructions \n", device_name, my_address, op_sim_time());
		printf ("\t\t End-to-end delay (MAC)   : %f sec \n", e2e_delay);				
		}
	Boolean entryFound = OPC_FALSE;
	
	for(int i=0;i<NodeFlowEntries.size();i++){ 
	//cout<<"my_address="<<my_address<<" NodeFlowEntries[i]->srcAdrMatchRule == source_address"<<NodeFlowEntries[i]->srcAdrMatchRule<<"=source_address"<<source_address<<endl;
	if (NodeFlowEntries[i]->srcAdrMatchRule == source_address)
			{
				entryFound = OPC_TRUE;
				if (NodeFlowEntries[i]->NextHop == my_address)
					{
					// receiveFromMACtoAPP++;
	 NodeFlowEntries[i]->statistics++; //=NodeFlowEntries[i]->statistics+receiveFromMACtoAPP;
					//Packet* packet_payload;
if (enable_log)					//if (op_pk_nfd_get (packet_ptr, "MAC Frame Payload", &packet_payload) != OPC_COMPCODE_FAILURE)
fprintf (log,"\t AppId=%d src= %d received data packet to application: this is the last hop next hop=%d statistics=%d \n",NodeFlowEntries[i]->ApplicationID ,source_address, NodeFlowEntries[i]->NextHop, NodeFlowEntries[i]->statistics);
					op_ici_attr_set (iciptr, "Traffic Type", traffic_type);
					op_ici_install (iciptr);
					op_pk_send (packet_ptr, STRM_FROM_PKTD_TO_UPL);
					op_ici_install (OPC_NIL);
					
	 	//cout<<"send it to STRM_FROM_PKTD_TO_UPL "<<endl;
	 //else
					//	op_pk_destroy(packet_payload);
					//op_pk_destroy(packet_ptr);
					}
				else if (NodeFlowEntries[i]->action == ACT_FORWARD_MAC)
					{
	// receiveFromMAC++;
	 NodeFlowEntries[i]->statistics++;//=NodeFlowEntries[i]->statistics+receiveFromMAC;
if (enable_log)				//op_pk_nfd_get (packet_ptr, "MAC Frame Payload", &pkt_payload);
fprintf (log,"\tAppId=%d src =%d action =%d  intermediate hop= %d next hop= %d  statistics= %d;\n",NodeFlowEntries[i]->ApplicationID ,source_address,ACT_FORWARD_MAC, my_address, NodeFlowEntries[i]->NextHop,NodeFlowEntries[i]->statistics);
					op_ici_attr_set (iciptr, "Next Hop Address", NodeFlowEntries[i]->NextHop);
					op_ici_attr_set (iciptr, "ACK", ack_request);
					// set cluster direction
					op_ici_attr_set (iciptr, "clusterDir", NodeFlowEntries[i]->clusterDir);
					op_ici_install (iciptr);
					op_pk_send (packet_ptr, STRM_FROM_PKTD_TO_MAC);
					op_ici_install (OPC_NIL);
					
	 //op_pk_destroy(packet_ptr);
					}
				
				/*else if (FlowList.Action == ACT_FORWARD_UP)
					{
						op_pk_send (packet_ptr, STRM_FROM_PKTD_TO_UPL);
					}*/
				else if (NodeFlowEntries[i]->action == ACT_DROP)
					{
					printf ("\t\t src= %d action == ACT_DROP  %d \n",source_address, NodeFlowEntries[i]->NextHop);
						//printf ("\t\t following data packet: packet dropped for action field drop instruction next hop %d \n", FlowList.NextHop);
						op_pk_destroy(packet_ptr);
					}
				break;
				}

/*
				if (!entryFound)// if the source address of the packet is not available in the flow list	
				{
					printf ("\t\t following data packet: packet dropped for no entry match the packet next hop %d \n", NodeFlowEntries[i]->NextHop);
					op_pk_destroy(packet_ptr);
				 }
				*/
				}//enf for				
		
		
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
#undef device_mode
#undef device_name
#undef my_address
#undef enable_log
#undef intrpt_type
#undef intrpt_strm
#undef intrpt_code
#undef ete_delay_be_stathandle
#undef ete_delay_rt_stathandle
#undef log
#undef pktssec_rcvd_stathandle
#undef flow_table_ptr
#undef NodeFlowEntries
#undef parent_id
#undef con_req_on_hold
#undef receiveFromSRC_BE
#undef receiveFromMAC
#undef receiveFromMACtoAPP

/* Access from C kernel using C linkage */
extern "C"
{
	VosT_Obtype _op_EDOpenFlow_init (int * init_block_ptr);
	VosT_Address _op_EDOpenFlow_alloc (VosT_Obtype, int);
	void EDOpenFlow (OP_SIM_CONTEXT_ARG_OPT)
		{
		((EDOpenFlow_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))->EDOpenFlow (OP_SIM_CONTEXT_PTR_OPT);
		}

	void _op_EDOpenFlow_svar (void *, const char *, void **);

	void _op_EDOpenFlow_diag (OP_SIM_CONTEXT_ARG_OPT)
		{
		((EDOpenFlow_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))->_op_EDOpenFlow_diag (OP_SIM_CONTEXT_PTR_OPT);
		}

	void _op_EDOpenFlow_terminate (OP_SIM_CONTEXT_ARG_OPT)
		{
		/* The destructor is the Termination Block */
		delete (EDOpenFlow_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr);
		}


} /* end of 'extern "C"' */




/* Process model interrupt handling procedure */


void
EDOpenFlow_state::EDOpenFlow (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (EDOpenFlow_state::EDOpenFlow ());
	try
		{
		/* Temporary Variables */
		Packet* command_PKT;
		
		/* End of Temporary Variables */


		FSM_ENTER ("EDOpenFlow")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (init) enter executives **/
			FSM_STATE_ENTER_FORCED_NOLABEL (0, "init", "EDOpenFlow [init enter execs]")
				FSM_PROFILE_SECTION_IN ("EDOpenFlow [init enter execs]", state0_enter_exec)
				{
				/* Initialization */
				wpan_nwk_init ();
				wpan_print_parameters ();
				pktssec_rcvd_stathandle 	= op_stat_reg ("Traffic Received (packets/sec)",	OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
				con_req_on_hold = OPC_FALSE;
				 
					 receiveFromSRC_BE=0;
					 receiveFromMAC=0;
					 receiveFromMACtoAPP=0;
				}
				FSM_PROFILE_SECTION_OUT (state0_enter_exec)

			/** state (init) exit executives **/
			FSM_STATE_EXIT_FORCED (0, "init", "EDOpenFlow [init exit execs]")


			/** state (init) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "init", "idle", "tr_11", "EDOpenFlow [init -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (idle) enter executives **/
			FSM_STATE_ENTER_UNFORCED (1, "idle", state1_enter_exec, "EDOpenFlow [idle enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (3,"EDOpenFlow")


			/** state (idle) exit executives **/
			FSM_STATE_EXIT_UNFORCED (1, "idle", "EDOpenFlow [idle exit execs]")
				FSM_PROFILE_SECTION_IN ("EDOpenFlow [idle exit execs]", state1_exit_exec)
				{
				intrpt_type = op_intrpt_type ();
				if (intrpt_type == OPC_INTRPT_STRM)
					intrpt_strm = op_intrpt_strm ();
				else
					intrpt_code = op_intrpt_code ();
				}
				FSM_PROFILE_SECTION_OUT (state1_exit_exec)


			/** state (idle) transition processing **/
			FSM_PROFILE_SECTION_IN ("EDOpenFlow [idle trans conditions]", state1_trans_conds)
			FSM_INIT_COND (RECEIVE_PCK_FROM_GTS)
			FSM_TEST_COND (RECEIVE_PCK_FROM_MAC)
			FSM_TEST_COND (RECEIVE_PCK_FROM_BE)
			FSM_TEST_COND (INCOMING_FLOW_TABLE)
			FSM_TEST_COND (CONNECTION_REQ_FROM_APP)
			FSM_TEST_LOGIC ("idle")
			FSM_PROFILE_SECTION_OUT (state1_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "RECEIVE_PCK_FROM_GTS", "", "idle", "from GTS", "receive packet from APPL", "EDOpenFlow [idle -> from GTS : RECEIVE_PCK_FROM_GTS / ]")
				FSM_CASE_TRANSIT (1, 3, state3_enter_exec, ;, "RECEIVE_PCK_FROM_MAC", "", "idle", "from MAC", "tr_14", "EDOpenFlow [idle -> from MAC : RECEIVE_PCK_FROM_MAC / ]")
				FSM_CASE_TRANSIT (2, 4, state4_enter_exec, ;, "RECEIVE_PCK_FROM_BE", "", "idle", "frm BE", "tr_19", "EDOpenFlow [idle -> frm BE : RECEIVE_PCK_FROM_BE / ]")
				FSM_CASE_TRANSIT (3, 5, state5_enter_exec, ;, "INCOMING_FLOW_TABLE", "", "idle", "rcv FT", "tr_21", "EDOpenFlow [idle -> rcv FT : INCOMING_FLOW_TABLE / ]")
				FSM_CASE_TRANSIT (4, 6, state6_enter_exec, ;, "CONNECTION_REQ_FROM_APP", "", "idle", "AppReq", "tr_26", "EDOpenFlow [idle -> AppReq : CONNECTION_REQ_FROM_APP / ]")
				}
				/*---------------------------------------------------------*/



			/** state (from GTS) enter executives **/
			FSM_STATE_ENTER_FORCED (2, "from GTS", state2_enter_exec, "EDOpenFlow [from GTS enter execs]")
				FSM_PROFILE_SECTION_IN ("EDOpenFlow [from GTS enter execs]", state2_enter_exec)
				{
				wpan_receive_pck_from_gts ();
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** state (from GTS) exit executives **/
			FSM_STATE_EXIT_FORCED (2, "from GTS", "EDOpenFlow [from GTS exit execs]")


			/** state (from GTS) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "from GTS", "idle", "tr_13", "EDOpenFlow [from GTS -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (from MAC) enter executives **/
			FSM_STATE_ENTER_FORCED (3, "from MAC", state3_enter_exec, "EDOpenFlow [from MAC enter execs]")
				FSM_PROFILE_SECTION_IN ("EDOpenFlow [from MAC enter execs]", state3_enter_exec)
				{
				op_stat_write (pktssec_rcvd_stathandle, 	1.0);
				//op_stat_write (pktssec_rcvd_stathandle, 	0.0);
				wpan_receive_pck_from_mac ();
				}
				FSM_PROFILE_SECTION_OUT (state3_enter_exec)

			/** state (from MAC) exit executives **/
			FSM_STATE_EXIT_FORCED (3, "from MAC", "EDOpenFlow [from MAC exit execs]")


			/** state (from MAC) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "from MAC", "idle", "tr_15", "EDOpenFlow [from MAC -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (frm BE) enter executives **/
			FSM_STATE_ENTER_FORCED (4, "frm BE", state4_enter_exec, "EDOpenFlow [frm BE enter execs]")
				FSM_PROFILE_SECTION_IN ("EDOpenFlow [frm BE enter execs]", state4_enter_exec)
				{
				op_stat_write (pktssec_rcvd_stathandle, 	1.0);
				SDNWSAN_receive_pck_from_BE ();
				}
				FSM_PROFILE_SECTION_OUT (state4_enter_exec)

			/** state (frm BE) exit executives **/
			FSM_STATE_EXIT_FORCED (4, "frm BE", "EDOpenFlow [frm BE exit execs]")


			/** state (frm BE) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "frm BE", "idle", "tr_20", "EDOpenFlow [frm BE -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (rcv FT) enter executives **/
			FSM_STATE_ENTER_FORCED (5, "rcv FT", state5_enter_exec, "EDOpenFlow [rcv FT enter execs]")
				FSM_PROFILE_SECTION_IN ("EDOpenFlow [rcv FT enter execs]", state5_enter_exec)
				{
				//Ici* iciptr;
				Packet*	pkptr;
				FlowEntery* flowEntery = new FlowEntery;
				 
				Boolean entryFound = OPC_FALSE;
				
				pkptr = op_pk_get (op_intrpt_strm ());					
				
				if (op_pk_nfd_get_ptr(pkptr, "FlowTables", (void**) &flowEntery) == OPC_COMPCODE_SUCCESS)
					{
				 // cout<<"flowEntery->nodeAddress="<<flowEntery->nodeAddress<<endl;
				 // cout<<"flowEntery->srcAdrMatchRule="<<flowEntery->srcAdrMatchRule<<endl;
				 // cout<<"flowEntery->action="<<flowEntery->action<<endl;
				 // cout<<"flowEntery->NextHop="<<flowEntery->NextHop<<endl;
				 // cout<<"flowEntery->clusterDir="<<flowEntery->clusterDir<<endl;
				 // cout<<"flowEntery->ApplicationID="<<flowEntery->ApplicationID<<endl;
				 if (enable_log)
					 fprintf(log, "\n flowEntery->ApplicationID = %d \n", flowEntery->ApplicationID);
				 
				 if (con_req_on_hold /*&& route_ok */)
					 {
					 con_req_on_hold = OPC_FALSE; 
					 op_intrpt_schedule_remote (op_sim_time(), START_APP_CODE, op_id_from_name (parent_id, OPC_OBJTYPE_PROC, "application layer"));	
					 }	
				 // else if (con_re	_on_hold && route_not_ok)
				 //{ con_req_on_hold = OPC_FALSE;}
				
				
				 for (int i = 0; i < NodeFlowEntries.size(); i++)
					if (NodeFlowEntries[i]->srcAdrMatchRule == flowEntery->srcAdrMatchRule){ 
							NodeFlowEntries.erase(NodeFlowEntries.begin()+i);
						//NodeFlowEntries[i]->nodeAddress = flowEntery->nodeAddress;
						//NodeFlowEntries[i]->srcAdrMatchRule = flowEntery->srcAdrMatchRule;
						//NodeFlowEntries[i]->action = flowEntery->action;
						//NodeFlowEntries[i]->NextHop = flowEntery->NextHop;
						//NodeFlowEntries[i]->clusterDir = flowEntery->srcAdrMatchRule;
						//NodeFlowEntries[i]->ApplicationID = flowEntery->ApplicationID;
						//cout<<"overwriteflowEntery->nodeAddress="<<flowEntery->nodeAddress<<endl;
						//cout<<"overwrite flowEntery->srcAdrMatchRule="<<flowEntery->srcAdrMatchRule<<endl;
						//cout<<"overwrite flowEntery->action="<<flowEntery->action<<endl;
						//cout<<"overwrite flowEntery->NextHop="<<flowEntery->NextHop<<endl;
						//cout<<"overwrite flowEntery->clusterDir="<<flowEntery->clusterDir<<endl;
						//cout<<"overwrite flowEntery->ApplicationID="<<flowEntery->ApplicationID<<endl;
					//	entryFound=OPC_TRUE;
						}
				
				//if (entryFound== OPC_FALSE)
				// {
				 flowEntery->statistics = 0;
				 NodeFlowEntries.push_back(flowEntery);// add new flowEntery to the node's NodeFlowEntries
					//delete flowEntery;		
				 //cout<<"the flow entries at the node of address "<< my_address <<endl;
				for(int i=0;i<NodeFlowEntries.size();i++){
				if (enable_log)
				fprintf (log, "ApplicatioID=%d & Node== %d & srcAdrMatchRule=%d & Next hop=%d Action=%d clusterDir=%d\n",
					NodeFlowEntries[i]->ApplicationID,NodeFlowEntries[i]->nodeAddress, NodeFlowEntries[i]->srcAdrMatchRule,NodeFlowEntries[i]->NextHop,NodeFlowEntries[i]->action,NodeFlowEntries[i]->clusterDir );
				  
					 }//end for
					//	} // end if (entryFound == OPC_FALSE) else if 
					}//end if  
				 
				}
				FSM_PROFILE_SECTION_OUT (state5_enter_exec)

			/** state (rcv FT) exit executives **/
			FSM_STATE_EXIT_FORCED (5, "rcv FT", "EDOpenFlow [rcv FT exit execs]")


			/** state (rcv FT) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "rcv FT", "idle", "tr_22", "EDOpenFlow [rcv FT -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (AppReq) enter executives **/
			FSM_STATE_ENTER_FORCED (6, "AppReq", state6_enter_exec, "EDOpenFlow [AppReq enter execs]")
				FSM_PROFILE_SECTION_IN ("EDOpenFlow [AppReq enter execs]", state6_enter_exec)
				{
				con_req_on_hold = OPC_TRUE;
				command_PKT = op_pk_create_fmt ("wpan_command_frame_format");	
				op_pk_nfd_set (command_PKT, "Command Frame Identifier", APP_ROUTE_REQ);
				//op_pk_nfd_set_ptr (command_PKT, "Command Payload", , op_prg_mem_copy_create, op_prg_mem_free, );
				op_pk_send (command_PKT, STRM_FROM_PKTD_TO_MAC);
				}
				FSM_PROFILE_SECTION_OUT (state6_enter_exec)

			/** state (AppReq) exit executives **/
			FSM_STATE_EXIT_FORCED (6, "AppReq", "EDOpenFlow [AppReq exit execs]")


			/** state (AppReq) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "AppReq", "idle", "tr_27", "EDOpenFlow [AppReq -> idle : default / ]")
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (0,"EDOpenFlow")
		}
	catch (...)
		{
		Vos_Error_Print (VOSC_ERROR_ABORT,
			(const char *)VOSC_NIL,
			"Unhandled C++ exception in process model (EDOpenFlow)",
			(const char *)VOSC_NIL, (const char *)VOSC_NIL);
		}
	}




void
EDOpenFlow_state::_op_EDOpenFlow_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}

void
EDOpenFlow_state::operator delete (void* ptr)
	{
	FIN (EDOpenFlow_state::operator delete (ptr));
	Vos_Poolmem_Dealloc (ptr);
	FOUT
	}

EDOpenFlow_state::~EDOpenFlow_state (void)
	{

	FIN (EDOpenFlow_state::~EDOpenFlow_state ())


	/* No Termination Block */


	FOUT
	}


#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

void *
EDOpenFlow_state::operator new (size_t)
#if defined (VOSD_NEW_BAD_ALLOC)
		throw (VOSD_BAD_ALLOC)
#endif
	{
	void * new_ptr;

	FIN_MT (EDOpenFlow_state::operator new ());

	new_ptr = Vos_Alloc_Object (EDOpenFlow_state::obtype);
#if defined (VOSD_NEW_BAD_ALLOC)
	if (new_ptr == VOSC_NIL) throw VOSD_BAD_ALLOC();
#endif
	FRET (new_ptr)
	}

/* State constructor initializes FSM handling */
/* by setting the initial state to the first */
/* block of code to enter. */

EDOpenFlow_state::EDOpenFlow_state (void) :
		_op_current_block (0)
	{
#if defined (OPD_ALLOW_ODB)
		_op_current_state = "EDOpenFlow [init enter execs]";
#endif
	}

VosT_Obtype
_op_EDOpenFlow_init (int * init_block_ptr)
	{
	FIN_MT (_op_EDOpenFlow_init (init_block_ptr))

	EDOpenFlow_state::obtype = Vos_Define_Object_Prstate ("proc state vars (EDOpenFlow)",
		sizeof (EDOpenFlow_state));
	*init_block_ptr = 0;

	FRET (EDOpenFlow_state::obtype)
	}

VosT_Address
_op_EDOpenFlow_alloc (VosT_Obtype, int)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	EDOpenFlow_state * ptr;
	FIN_MT (_op_EDOpenFlow_alloc ())

	/* New instance will have FSM handling initialized */
#if defined (VOSD_NEW_BAD_ALLOC)
	try {
		ptr = new EDOpenFlow_state;
	} catch (const VOSD_BAD_ALLOC &) {
		ptr = VOSC_NIL;
	}
#else
	ptr = new EDOpenFlow_state;
#endif
	FRET ((VosT_Address)ptr)
	}



void
_op_EDOpenFlow_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	EDOpenFlow_state		*prs_ptr;

	FIN_MT (_op_EDOpenFlow_svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (EDOpenFlow_state *)gen_ptr;

	if (strcmp ("device_mode" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->device_mode);
		FOUT
		}
	if (strcmp ("device_name" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->device_name);
		FOUT
		}
	if (strcmp ("my_address" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->my_address);
		FOUT
		}
	if (strcmp ("enable_log" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->enable_log);
		FOUT
		}
	if (strcmp ("intrpt_type" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->intrpt_type);
		FOUT
		}
	if (strcmp ("intrpt_strm" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->intrpt_strm);
		FOUT
		}
	if (strcmp ("intrpt_code" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->intrpt_code);
		FOUT
		}
	if (strcmp ("ete_delay_be_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->ete_delay_be_stathandle);
		FOUT
		}
	if (strcmp ("ete_delay_rt_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->ete_delay_rt_stathandle);
		FOUT
		}
	if (strcmp ("log" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->log);
		FOUT
		}
	if (strcmp ("pktssec_rcvd_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pktssec_rcvd_stathandle);
		FOUT
		}
	if (strcmp ("flow_table_ptr" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->flow_table_ptr);
		FOUT
		}
	if (strcmp ("NodeFlowEntries" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->NodeFlowEntries);
		FOUT
		}
	if (strcmp ("parent_id" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->parent_id);
		FOUT
		}
	if (strcmp ("con_req_on_hold" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->con_req_on_hold);
		FOUT
		}
	if (strcmp ("receiveFromSRC_BE" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->receiveFromSRC_BE);
		FOUT
		}
	if (strcmp ("receiveFromMAC" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->receiveFromMAC);
		FOUT
		}
	if (strcmp ("receiveFromMACtoAPP" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->receiveFromMACtoAPP);
		FOUT
		}
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

