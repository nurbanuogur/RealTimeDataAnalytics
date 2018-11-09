/* Process model C++ form file: SDNC_TopologyDiscovery.pr.cpp */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char SDNC_TopologyDiscovery_pr_cpp [] = "MIL_3_Tfile_Hdr_ 145A 30A op_runsim 7 5A2F81E4 5A2F81E4 1 DESKTOP-JCGS160 Mohammed@Hussein 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1e80 8                                                                                                                                                                                                                                                                                                                                                                                       ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

#include <stdio.h>
#include <vector>
#include <headers/node.h>
#define MAC_TO_TDP			106
#define DEFAULT_SOBO		0
#define NOT_DEFAULT_SOBO	1
#define TDP_OUT_INDEX		0
#define CHECK_STRUCT		22
#define BASE_START_TIME		0.03072
#define NOT_DEFINED			-1

using namespace std;
//struct for neighbour list array
//typedef struct {
//int node_address;
//double SNR;
//} neighbour_list_struct;

// status info structure
//typedef struct {
//double energy_level;
//int neighbours_no;
//int nodeAddress;
//int parentAddress;
//neighbour_list_struct nighbourList [10];
//} SDNWSAN_status_values;

typedef struct {
int nodes_no_to_be_set;
int nodeAddress[15];
double startTime[15];
int so[15];
int bo[15];
} startTimeSoBo_list;



#define INCOMING_PACKET 	(op_intrpt_type () == OPC_INTRPT_STRM)
#define INCOMING_ndp_N_LIST 	(intrpt_type == OPC_INTRPT_REMOTE && intrpt_code == MAC_TO_TDP)
#define CHECK_NODES_STRUCT_TO_SET_NET_PARAMS 	(op_intrpt_type () == OPC_INTRPT_SELF && intrpt_code == CHECK_STRUCT)

static void topoDis_log_file_init();

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
class SDNC_TopologyDiscovery_state
	{
	private:
		/* Internal state tracking for FSM */
		FSM_SYS_STATE

	public:
		SDNC_TopologyDiscovery_state (void);

		/* Destructor contains Termination Block */
		~SDNC_TopologyDiscovery_state (void);

		/* State Variables */
		int	                    		total_no_nodes                                  ;
		int	                    		intrpt_code                                     ;
		int	                    		intrpt_type                                     ;
		int	                    		intrpt_strm                                     ;
		startTimeSoBo_list*	    		startTimeSoBo_list_ptr                          ;
		vector<SDNWSAN_status_values*>			node_list                                       ;
		FILE*	                  		topoLog                                         ;

		/* FSM code */
		void SDNC_TopologyDiscovery (OP_SIM_CONTEXT_ARG_OPT);
		/* Diagnostic Block */
		void _op_SDNC_TopologyDiscovery_diag (OP_SIM_CONTEXT_ARG_OPT);

#if defined (VOSD_NEW_BAD_ALLOC)
		void * operator new (size_t) throw (VOSD_BAD_ALLOC);
#else
		void * operator new (size_t);
#endif
		void operator delete (void *);

		/* Memory management */
		static VosT_Obtype obtype;
	};

VosT_Obtype SDNC_TopologyDiscovery_state::obtype = (VosT_Obtype)OPC_NIL;

#define pr_state_ptr            		((SDNC_TopologyDiscovery_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))
#define total_no_nodes          		pr_state_ptr->total_no_nodes
#define intrpt_code             		pr_state_ptr->intrpt_code
#define intrpt_type             		pr_state_ptr->intrpt_type
#define intrpt_strm             		pr_state_ptr->intrpt_strm
#define startTimeSoBo_list_ptr  		pr_state_ptr->startTimeSoBo_list_ptr
#define node_list               		pr_state_ptr->node_list
#define topoLog                 		pr_state_ptr->topoLog

/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	SDNC_TopologyDiscovery_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((SDNC_TopologyDiscovery_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


/* Function Block */

#if !defined (VOSD_NO_FIN)
enum { _op_block_origin = __LINE__ + 2};
#endif

static void  topoDis_log_file_init() {
Objid selfID;
Objid fatherID;
char folder_path[120];
char log_name[132];
char name [20];
//"Coordinator.NIntel_process";
//strncpy(log_name, "Coordinator.NIntel_process", 132);
FIN (topoDis_log_file_init ());
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
sprintf (log_name, "%s%s%s.ak", folder_path, name,"_topoDisModule"); 
printf ("Log file name: %s \n\n", topoLog);
topoLog = fopen(log_name,"w");
fprintf (topoLog, "|-----------------------------------------------------------------------------|\n");
fprintf (topoLog, "| NODE NAME          : %stopoDisModule \n", name);
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
#undef total_no_nodes
#undef intrpt_code
#undef intrpt_type
#undef intrpt_strm
#undef startTimeSoBo_list_ptr
#undef node_list
#undef topoLog

/* Access from C kernel using C linkage */
extern "C"
{
	VosT_Obtype _op_SDNC_TopologyDiscovery_init (int * init_block_ptr);
	VosT_Address _op_SDNC_TopologyDiscovery_alloc (VosT_Obtype, int);
	void SDNC_TopologyDiscovery (OP_SIM_CONTEXT_ARG_OPT)
		{
		((SDNC_TopologyDiscovery_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))->SDNC_TopologyDiscovery (OP_SIM_CONTEXT_PTR_OPT);
		}

	void _op_SDNC_TopologyDiscovery_svar (void *, const char *, void **);

	void _op_SDNC_TopologyDiscovery_diag (OP_SIM_CONTEXT_ARG_OPT)
		{
		((SDNC_TopologyDiscovery_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))->_op_SDNC_TopologyDiscovery_diag (OP_SIM_CONTEXT_PTR_OPT);
		}

	void _op_SDNC_TopologyDiscovery_terminate (OP_SIM_CONTEXT_ARG_OPT)
		{
		/* The destructor is the Termination Block */
		delete (SDNC_TopologyDiscovery_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr);
		}


} /* end of 'extern "C"' */




/* Process model interrupt handling procedure */


void
SDNC_TopologyDiscovery_state::SDNC_TopologyDiscovery (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (SDNC_TopologyDiscovery_state::SDNC_TopologyDiscovery ());
	try
		{
		/* Temporary Variables */
		Packet*		pkptr;
		/* End of Temporary Variables */


		FSM_ENTER ("SDNC_TopologyDiscovery")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (idle) enter executives **/
			FSM_STATE_ENTER_UNFORCED (0, "idle", state0_enter_exec, "SDNC_TopologyDiscovery [idle enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (1,"SDNC_TopologyDiscovery")


			/** state (idle) exit executives **/
			FSM_STATE_EXIT_UNFORCED (0, "idle", "SDNC_TopologyDiscovery [idle exit execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_TopologyDiscovery [idle exit execs]", state0_exit_exec)
				{
				intrpt_type = op_intrpt_type ();
				if (intrpt_type == OPC_INTRPT_STRM)
					intrpt_strm = op_intrpt_strm ();
				else
					intrpt_code = op_intrpt_code ();
				}
				FSM_PROFILE_SECTION_OUT (state0_exit_exec)


			/** state (idle) transition processing **/
			FSM_PROFILE_SECTION_IN ("SDNC_TopologyDiscovery [idle trans conditions]", state0_trans_conds)
			FSM_INIT_COND (INCOMING_PACKET)
			FSM_TEST_COND (INCOMING_ndp_N_LIST)
			FSM_TEST_COND (CHECK_NODES_STRUCT_TO_SET_NET_PARAMS)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("idle")
			FSM_PROFILE_SECTION_OUT (state0_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "INCOMING_PACKET", "", "idle", "inPkt", "tr_2", "SDNC_TopologyDiscovery [idle -> inPkt : INCOMING_PACKET / ]")
				FSM_CASE_TRANSIT (1, 3, state3_enter_exec, ;, "INCOMING_ndp_N_LIST", "", "idle", "iciFrmMac", "tr_4", "SDNC_TopologyDiscovery [idle -> iciFrmMac : INCOMING_ndp_N_LIST / ]")
				FSM_CASE_TRANSIT (2, 4, state4_enter_exec, ;, "CHECK_NODES_STRUCT_TO_SET_NET_PARAMS", "", "idle", "checkStruct", "tr_6", "SDNC_TopologyDiscovery [idle -> checkStruct : CHECK_NODES_STRUCT_TO_SET_NET_PARAMS / ]")
				FSM_CASE_TRANSIT (3, 0, state0_enter_exec, ;, "default", "", "idle", "idle", " ", "SDNC_TopologyDiscovery [idle -> idle : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (INIT) enter executives **/
			FSM_STATE_ENTER_FORCED_NOLABEL (1, "INIT", "SDNC_TopologyDiscovery [INIT enter execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_TopologyDiscovery [INIT enter execs]", state1_enter_exec)
				{
				total_no_nodes = 0;
				op_intrpt_schedule_self (op_sim_time() + 16, CHECK_STRUCT);
				startTimeSoBo_list_ptr = new startTimeSoBo_list;
				topoDis_log_file_init();
				}
				FSM_PROFILE_SECTION_OUT (state1_enter_exec)

			/** state (INIT) exit executives **/
			FSM_STATE_EXIT_FORCED (1, "INIT", "SDNC_TopologyDiscovery [INIT exit execs]")


			/** state (INIT) transition processing **/
			FSM_TRANSIT_FORCE (0, state0_enter_exec, ;, "default", "", "INIT", "idle", "tr_12", "SDNC_TopologyDiscovery [INIT -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (inPkt) enter executives **/
			FSM_STATE_ENTER_FORCED (2, "inPkt", state2_enter_exec, "SDNC_TopologyDiscovery [inPkt enter execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_TopologyDiscovery [inPkt enter execs]", state2_enter_exec)
				{
				//op_intrpt_schedule_self (op_sim_time() + 5, CHECK_STRUCT); // check topology after 5 seconds
				int source_address;
				int i = 0;
				int a = -1;
				Packet* command_frame;
				SDNWSAN_status_values* SDNWSAN_status_values_ptr;// = new SDNWSAN_status_values ; 
				pkptr = op_pk_get (op_intrpt_strm ());
				op_pk_nfd_get (pkptr, "Source Address", &source_address);
				
				
					
				if (op_pk_nfd_get_pkt (pkptr, "MSDU", &command_frame) == OPC_COMPCODE_SUCCESS )
					{
						if (op_pk_nfd_get_ptr (command_frame, "Command Payload", (void**)&SDNWSAN_status_values_ptr)!= OPC_COMPCODE_FAILURE)
							{
							
							//printf("----t = %f ->---------  this is TDP  ----------------------\n", op_sim_time());
							
							
							//printf ("********************* statuses of current incoming node %d *************************\n", SDNWSAN_status_values_ptr->nodeAddress);		
							//printf (" The energy level is		  %f \n", SDNWSAN_status_values_ptr->energy_level);	
							//printf (" Number of neighbours is 	  %d \n", SDNWSAN_status_values_ptr->neighbours_no);	
							//printf (" Parent address is 	  %d \n", SDNWSAN_status_values_ptr->parentAddress);
							//printf ("*******************  The neighbour node list of this node **********************\n");	
							//for (int l=0; l < SDNWSAN_status_values_ptr->neighbours_no; l++) {
								//printf ("%d. node address: %d of SNR of: %f  \n", l+1,  SDNWSAN_status_values_ptr->nighbourList[l].node_address, SDNWSAN_status_values_ptr->nighbourList[l].SNR);
							//	}
							// if the source is not in the list added it
							while (i < total_no_nodes) {
							if (node_list[i++]->nodeAddress == source_address)
								a = i - 1;// src is here
							} 
							if (a == -1) {// if the src not exist add it
								a = total_no_nodes++;// get the new index for src in vector list
								node_list.push_back(SDNWSAN_status_values_ptr);	
								}
							else
								{
				
							node_list[a]->neighbours_no = SDNWSAN_status_values_ptr->neighbours_no;
							node_list[a]->energy_level = SDNWSAN_status_values_ptr->energy_level;
							node_list[a]->parentAddress = SDNWSAN_status_values_ptr->parentAddress;
							for (int j=0; j<SDNWSAN_status_values_ptr->neighbours_no; j++) {
								node_list[a]->nighbourList[j].node_address = SDNWSAN_status_values_ptr->nighbourList[j].node_address;
								node_list[a]->nighbourList[j].SNR = SDNWSAN_status_values_ptr->nighbourList[j].SNR;
								}
							}
							}
						op_pk_destroy(command_frame);
						}
				
				op_pk_destroy(pkptr);
				
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** state (inPkt) exit executives **/
			FSM_STATE_EXIT_FORCED (2, "inPkt", "SDNC_TopologyDiscovery [inPkt exit execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_TopologyDiscovery [inPkt exit execs]", state2_exit_exec)
				{
				/*
							printf("----t = %f ->---------  this is TDP  ----------------------\n", op_sim_time());
							printf("-------------------  Total node list  ----------------------\n");
							printf("-------------------  Total number of nodes %d  ----------------------\n", total_no_nodes);
							
							
							for (int k = 0; k < total_no_nodes; k++)
								{
							printf ("********************* statuses of node address %d *************************\n", node_list[k]->nodeAddress);		
							printf (" The energy level is		  %f \n", node_list[k]->energy_level);	
							printf (" Number of neighbours is 	  %d \n", node_list[k]->neighbours_no);	
							printf (" Parent address is 	  %d \n", node_list[k]->parentAddress);
							printf ("*******************  The neighbour node list of this node **********************\n");	
							for (int l=0; l<node_list[k]->neighbours_no; l++) {
								printf ("%d. node address: %d of rssi of: %f  \n", l+1,  node_list[k]->nighbourList[l].node_address, node_list[k]->nighbourList[l].SNR);
								}
							}
				
				*/
				}
				FSM_PROFILE_SECTION_OUT (state2_exit_exec)


			/** state (inPkt) transition processing **/
			FSM_TRANSIT_FORCE (0, state0_enter_exec, ;, "default", "", "inPkt", "idle", "tr_3", "SDNC_TopologyDiscovery [inPkt -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (iciFrmMac) enter executives **/
			FSM_STATE_ENTER_FORCED (3, "iciFrmMac", state3_enter_exec, "SDNC_TopologyDiscovery [iciFrmMac enter execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_TopologyDiscovery [iciFrmMac enter execs]", state3_enter_exec)
				{
				Ici* iciptr;
				int sdnc_no_neighbours;
				int *n_addresses;
				
				iciptr = op_intrpt_ici ();
				op_ici_attr_get (iciptr, "sdnc_neighbours_no", &sdnc_no_neighbours);
				op_ici_attr_get (iciptr, "addresses", &n_addresses);
				//startTimeSoBo_list* startTimeSoBo_list_ptr = new startTimeSoBo_list;	
				//printf ("-----t = %f------ this is sdnc tdp neighbour list --------------------- \n", op_sim_time());
				fprintf (topoLog, "--t = %f------- this is sdnc tdp neighbour list --------------------|\n", op_sim_time());
				//printf ("number of neighbour nodes:) %d \n", sdnc_no_neighbours);
				//printf ("addresses are: ");
				fprintf (topoLog, "number of neighbour nodes:) %d \n", sdnc_no_neighbours);
				fprintf (topoLog, "addresses are: ");
				for (int i = 0; i < sdnc_no_neighbours; i++)
				//printf ("%d - ", n_addresses[i]);
				//printf ("\n-------------------------------------------------------------- \n");
				for (int i = 0; i < sdnc_no_neighbours; i++)
				fprintf (topoLog, "%d - ", n_addresses[i]);
				fprintf (topoLog, "\n-------------------------------------------------------------- \n");
				
				double startTime = BASE_START_TIME;
						startTimeSoBo_list_ptr->nodes_no_to_be_set = sdnc_no_neighbours;
						Packet* TDPpkt;
						TDPpkt = op_pk_create_fmt("TDPpkt");
						for (int i = 0; i < sdnc_no_neighbours; i++)
							{
								startTimeSoBo_list_ptr->nodeAddress[i] = n_addresses[i];
								startTimeSoBo_list_ptr->startTime[i] = startTime;
								startTime = startTime + BASE_START_TIME; // 
							}
						for (int i = 0; i < sdnc_no_neighbours; i++)
							{
								fprintf (topoLog, "%d. node address: %d \n", i + 1, startTimeSoBo_list_ptr->nodeAddress[i]);
								fprintf (topoLog, "%d. node start time: %f \n", i + 1, startTimeSoBo_list_ptr->startTime[i]);
							}
				
				
				fprintf (topoLog, " sending tdp packet to SDNC mac \n");
				
						op_pk_nfd_set_ptr (TDPpkt, "startTimeSoBo_list", startTimeSoBo_list_ptr, op_prg_mem_copy_create, op_prg_mem_free, sizeof (startTimeSoBo_list));
						op_pk_nfd_set(TDPpkt, "TD_code", DEFAULT_SOBO);
						op_pk_send(TDPpkt, TDP_OUT_INDEX);
				}
				FSM_PROFILE_SECTION_OUT (state3_enter_exec)

			/** state (iciFrmMac) exit executives **/
			FSM_STATE_EXIT_FORCED (3, "iciFrmMac", "SDNC_TopologyDiscovery [iciFrmMac exit execs]")


			/** state (iciFrmMac) transition processing **/
			FSM_TRANSIT_FORCE (0, state0_enter_exec, ;, "default", "", "iciFrmMac", "idle", "tr_5", "SDNC_TopologyDiscovery [iciFrmMac -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (checkStruct) enter executives **/
			FSM_STATE_ENTER_FORCED (4, "checkStruct", state4_enter_exec, "SDNC_TopologyDiscovery [checkStruct enter execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_TopologyDiscovery [checkStruct enter execs]", state4_enter_exec)
				{
				//op_intrpt_schedule_self (op_sim_time() + 300, CHECK_STRUCT);
				fprintf(topoLog, "\n\n intterupt for checking network structure updates \n\n");
				int startTimeSoBo_list_ptr_counter = 0;
				double startTime = BASE_START_TIME;
				//int no_nodes_with_children = 0; // we may need it or will be calculated in mac
				
				for (int i = 0; i < total_no_nodes; i++)
					{
						if (node_list[i]->parentAddress == 0)// SDNC address
						{
						Boolean node_with_child_found = OPC_FALSE;
						int outerLoopBreak = 0;
						for (int j = 0; j < node_list[i]->neighbours_no; j++)
							{
								//if (node_list[i]->nighbourList[j].node_address == 0)
								//	continue;
								//else
									for (int k = 0; k < total_no_nodes; k++)
									{// if the node is SDNC nighbourList
										if (node_list[k]->nodeAddress == node_list[i]->nighbourList[j].node_address)
											{
												if (node_list[i]->nodeAddress == node_list[k]->parentAddress )// or it is SDNC nighbourList node
													{// here is the problem when we have router with no child in the SDNC cluster
													 // in this case, if this node selected in the route then app traffic droped 
														startTimeSoBo_list_ptr->nodeAddress[startTimeSoBo_list_ptr_counter] = node_list[i]->nodeAddress;
														startTimeSoBo_list_ptr->startTime[startTimeSoBo_list_ptr_counter] = startTime;
														startTimeSoBo_list_ptr->so[startTimeSoBo_list_ptr_counter] = 1;
														startTimeSoBo_list_ptr->bo[startTimeSoBo_list_ptr_counter++] = 1;
														node_with_child_found = OPC_TRUE;
														startTime = startTime + BASE_START_TIME;
														outerLoopBreak = 1;
														break;
													}
											}
									}
								if (outerLoopBreak == 1)
									break;
							}
						if (!node_with_child_found)
							{
								startTimeSoBo_list_ptr->nodeAddress[startTimeSoBo_list_ptr_counter] = node_list[i]->nodeAddress;
								startTimeSoBo_list_ptr->startTime[startTimeSoBo_list_ptr_counter] = NOT_DEFINED;
								startTimeSoBo_list_ptr->so[startTimeSoBo_list_ptr_counter] = NOT_DEFINED;
								startTimeSoBo_list_ptr->bo[startTimeSoBo_list_ptr_counter++] = NOT_DEFINED;
							}
						}
					}
				
				startTimeSoBo_list_ptr->nodes_no_to_be_set = startTimeSoBo_list_ptr_counter;
				
					fprintf (topoLog, "t = %f -> number of nodes to be set: %d \n", op_sim_time(), startTimeSoBo_list_ptr->nodes_no_to_be_set);
						for (int i = 0; i < startTimeSoBo_list_ptr->nodes_no_to_be_set; i++)
							{
								fprintf (topoLog, "%d. node address: %d \n", i + 1, startTimeSoBo_list_ptr->nodeAddress[i]);
								fprintf (topoLog, "%d. node start time: %f \n", i + 1, startTimeSoBo_list_ptr->startTime[i]);
								fprintf (topoLog, "%d. so: %d \n", i + 1, startTimeSoBo_list_ptr->so[i]);
								fprintf (topoLog, "%d. bo: %d \n", i + 1, startTimeSoBo_list_ptr->bo[i]);
							}
				
				Packet* TDPpkt;
				TDPpkt = op_pk_create_fmt("TDPpkt");
				op_pk_nfd_set_ptr (TDPpkt, "startTimeSoBo_list", startTimeSoBo_list_ptr, op_prg_mem_copy_create, op_prg_mem_free, sizeof (startTimeSoBo_list));
				op_pk_nfd_set(TDPpkt, "TD_code", NOT_DEFAULT_SOBO);
				op_pk_send(TDPpkt, TDP_OUT_INDEX);
				}
				FSM_PROFILE_SECTION_OUT (state4_enter_exec)

			/** state (checkStruct) exit executives **/
			FSM_STATE_EXIT_FORCED (4, "checkStruct", "SDNC_TopologyDiscovery [checkStruct exit execs]")


			/** state (checkStruct) transition processing **/
			FSM_TRANSIT_FORCE (0, state0_enter_exec, ;, "default", "", "checkStruct", "idle", "tr_7", "SDNC_TopologyDiscovery [checkStruct -> idle : default / ]")
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (1,"SDNC_TopologyDiscovery")
		}
	catch (...)
		{
		Vos_Error_Print (VOSC_ERROR_ABORT,
			(const char *)VOSC_NIL,
			"Unhandled C++ exception in process model (SDNC_TopologyDiscovery)",
			(const char *)VOSC_NIL, (const char *)VOSC_NIL);
		}
	}




void
SDNC_TopologyDiscovery_state::_op_SDNC_TopologyDiscovery_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}

void
SDNC_TopologyDiscovery_state::operator delete (void* ptr)
	{
	FIN (SDNC_TopologyDiscovery_state::operator delete (ptr));
	Vos_Poolmem_Dealloc (ptr);
	FOUT
	}

SDNC_TopologyDiscovery_state::~SDNC_TopologyDiscovery_state (void)
	{

	FIN (SDNC_TopologyDiscovery_state::~SDNC_TopologyDiscovery_state ())


	/* No Termination Block */


	FOUT
	}


#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

void *
SDNC_TopologyDiscovery_state::operator new (size_t)
#if defined (VOSD_NEW_BAD_ALLOC)
		throw (VOSD_BAD_ALLOC)
#endif
	{
	void * new_ptr;

	FIN_MT (SDNC_TopologyDiscovery_state::operator new ());

	new_ptr = Vos_Alloc_Object (SDNC_TopologyDiscovery_state::obtype);
#if defined (VOSD_NEW_BAD_ALLOC)
	if (new_ptr == VOSC_NIL) throw VOSD_BAD_ALLOC();
#endif
	FRET (new_ptr)
	}

/* State constructor initializes FSM handling */
/* by setting the initial state to the first */
/* block of code to enter. */

SDNC_TopologyDiscovery_state::SDNC_TopologyDiscovery_state (void) :
		_op_current_block (2)
	{
#if defined (OPD_ALLOW_ODB)
		_op_current_state = "SDNC_TopologyDiscovery [INIT enter execs]";
#endif
	}

VosT_Obtype
_op_SDNC_TopologyDiscovery_init (int * init_block_ptr)
	{
	FIN_MT (_op_SDNC_TopologyDiscovery_init (init_block_ptr))

	SDNC_TopologyDiscovery_state::obtype = Vos_Define_Object_Prstate ("proc state vars (SDNC_TopologyDiscovery)",
		sizeof (SDNC_TopologyDiscovery_state));
	*init_block_ptr = 2;

	FRET (SDNC_TopologyDiscovery_state::obtype)
	}

VosT_Address
_op_SDNC_TopologyDiscovery_alloc (VosT_Obtype, int)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	SDNC_TopologyDiscovery_state * ptr;
	FIN_MT (_op_SDNC_TopologyDiscovery_alloc ())

	/* New instance will have FSM handling initialized */
#if defined (VOSD_NEW_BAD_ALLOC)
	try {
		ptr = new SDNC_TopologyDiscovery_state;
	} catch (const VOSD_BAD_ALLOC &) {
		ptr = VOSC_NIL;
	}
#else
	ptr = new SDNC_TopologyDiscovery_state;
#endif
	FRET ((VosT_Address)ptr)
	}



void
_op_SDNC_TopologyDiscovery_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	SDNC_TopologyDiscovery_state		*prs_ptr;

	FIN_MT (_op_SDNC_TopologyDiscovery_svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (SDNC_TopologyDiscovery_state *)gen_ptr;

	if (strcmp ("total_no_nodes" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->total_no_nodes);
		FOUT
		}
	if (strcmp ("intrpt_code" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->intrpt_code);
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
	if (strcmp ("startTimeSoBo_list_ptr" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->startTimeSoBo_list_ptr);
		FOUT
		}
	if (strcmp ("node_list" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->node_list);
		FOUT
		}
	if (strcmp ("topoLog" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->topoLog);
		FOUT
		}
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

