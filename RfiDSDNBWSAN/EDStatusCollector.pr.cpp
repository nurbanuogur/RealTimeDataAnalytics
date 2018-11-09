/* Process model C++ form file: EDStatusCollector.pr.cpp */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char EDStatusCollector_pr_cpp [] = "MIL_3_Tfile_Hdr_ 145A 30A op_runsim 7 5A2F81DE 5A2F81DE 1 DESKTOP-JCGS160 Mohammed@Hussein 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1e80 8                                                                                                                                                                                                                                                                                                                                                                                       ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

 
#include <iostream>
#include <cstdlib>
#include "headers/Node.h"
using namespace std;
#define OUT_STRM   4
#define SendStss   210

// NDP neighbour list indeces
#define node_address_index		0
#define node_rssi_index    		1


#define		SEND_STATUS		(intrpt_type == OPC_INTRPT_SELF && intrpt_code == SendStss)
#define		RECEIVE_STATUS	(intrpt_type == OPC_INTRPT_STRM)

//struct for neighbour list array
/*
typedef struct {
int node_address;
double node_rssi;
} neighbour_list_struct;

// status info structure
typedef struct {
double energy_level;
int neighbours_no;
int parent_address;
neighbour_list_struct nighbourList [10];
} SDNWSAN_status_values;
*/
//functinos used
static void  statusCollector_log_file_init();

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
class EDStatusCollector_state
	{
	private:
		/* Internal state tracking for FSM */
		FSM_SYS_STATE

	public:
		EDStatusCollector_state (void);

		/* Destructor contains Termination Block */
		~EDStatusCollector_state (void);

		/* State Variables */
		double	                 		current_energy_level                            ;
		int	                    		a                                               ;
		double	                 		updated_rssi                                    ;
		double	                 		updated_rssi_src                                ;
		double	                 		updated_rssi_dst                                ;
		int	                    		intrpt_code                                     ;
		int	                    		intrpt_type                                     ;
		int	                    		intrpt_stream_indx                              ;
		double	                 		neighbour_list[15][2]                           ;
		int	                    		no_neighbours                                   ;
		neighbour_list_struct	  		neighbour_listS[15]                             ;
		FILE*	                  		Slog_file                                       ;
		Boolean	                		enable_log                                      ;
		int	                    		counterPassedThreshold                          ;

		/* FSM code */
		void EDStatusCollector (OP_SIM_CONTEXT_ARG_OPT);
		/* Diagnostic Block */
		void _op_EDStatusCollector_diag (OP_SIM_CONTEXT_ARG_OPT);

#if defined (VOSD_NEW_BAD_ALLOC)
		void * operator new (size_t) throw (VOSD_BAD_ALLOC);
#else
		void * operator new (size_t);
#endif
		void operator delete (void *);

		/* Memory management */
		static VosT_Obtype obtype;
	};

VosT_Obtype EDStatusCollector_state::obtype = (VosT_Obtype)OPC_NIL;

#define pr_state_ptr            		((EDStatusCollector_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))
#define current_energy_level    		pr_state_ptr->current_energy_level
#define a                       		pr_state_ptr->a
#define updated_rssi            		pr_state_ptr->updated_rssi
#define updated_rssi_src        		pr_state_ptr->updated_rssi_src
#define updated_rssi_dst        		pr_state_ptr->updated_rssi_dst
#define intrpt_code             		pr_state_ptr->intrpt_code
#define intrpt_type             		pr_state_ptr->intrpt_type
#define intrpt_stream_indx      		pr_state_ptr->intrpt_stream_indx
#define neighbour_list          		pr_state_ptr->neighbour_list
#define no_neighbours           		pr_state_ptr->no_neighbours
#define neighbour_listS         		pr_state_ptr->neighbour_listS
#define Slog_file               		pr_state_ptr->Slog_file
#define enable_log              		pr_state_ptr->enable_log
#define counterPassedThreshold  		pr_state_ptr->counterPassedThreshold

/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	EDStatusCollector_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((EDStatusCollector_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


/* Function Block */

#if !defined (VOSD_NO_FIN)
enum { _op_block_origin = __LINE__ + 2};
#endif

static void  statusCollector_log_file_init() {
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
sprintf (log_name, "%s%s%s.ak", folder_path, name,"_StatusCollector"); 
printf ("Log file name: %s \n\n", log_name);
Slog_file = fopen(log_name,"w");
fprintf (Slog_file, "|-----------------------------------------------------------------------------|\n");
fprintf (Slog_file, "| NODE NAME          : %sStatusCollector \n", name);
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
#undef current_energy_level
#undef a
#undef updated_rssi
#undef updated_rssi_src
#undef updated_rssi_dst
#undef intrpt_code
#undef intrpt_type
#undef intrpt_stream_indx
#undef neighbour_list
#undef no_neighbours
#undef neighbour_listS
#undef Slog_file
#undef enable_log
#undef counterPassedThreshold

/* Access from C kernel using C linkage */
extern "C"
{
	VosT_Obtype _op_EDStatusCollector_init (int * init_block_ptr);
	VosT_Address _op_EDStatusCollector_alloc (VosT_Obtype, int);
	void EDStatusCollector (OP_SIM_CONTEXT_ARG_OPT)
		{
		((EDStatusCollector_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))->EDStatusCollector (OP_SIM_CONTEXT_PTR_OPT);
		}

	void _op_EDStatusCollector_svar (void *, const char *, void **);

	void _op_EDStatusCollector_diag (OP_SIM_CONTEXT_ARG_OPT)
		{
		((EDStatusCollector_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))->_op_EDStatusCollector_diag (OP_SIM_CONTEXT_PTR_OPT);
		}

	void _op_EDStatusCollector_terminate (OP_SIM_CONTEXT_ARG_OPT)
		{
		/* The destructor is the Termination Block */
		delete (EDStatusCollector_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr);
		}


} /* end of 'extern "C"' */




/* Process model interrupt handling procedure */


void
EDStatusCollector_state::EDStatusCollector (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (EDStatusCollector_state::EDStatusCollector ());
	try
		{
		/* Temporary Variables */
		Packet*		pkptr;
		//Objid self_id;
		/* End of Temporary Variables */


		FSM_ENTER ("EDStatusCollector")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (WAIT) enter executives **/
			FSM_STATE_ENTER_UNFORCED (0, "WAIT", state0_enter_exec, "EDStatusCollector [WAIT enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (1,"EDStatusCollector")


			/** state (WAIT) exit executives **/
			FSM_STATE_EXIT_UNFORCED (0, "WAIT", "EDStatusCollector [WAIT exit execs]")
				FSM_PROFILE_SECTION_IN ("EDStatusCollector [WAIT exit execs]", state0_exit_exec)
				{
				intrpt_code = op_intrpt_code ();
				intrpt_type = op_intrpt_type ();
				
				if (intrpt_type == OPC_INTRPT_STRM){
				intrpt_stream_indx = op_intrpt_strm();}
				}
				FSM_PROFILE_SECTION_OUT (state0_exit_exec)


			/** state (WAIT) transition processing **/
			FSM_PROFILE_SECTION_IN ("EDStatusCollector [WAIT trans conditions]", state0_trans_conds)
			FSM_INIT_COND (RECEIVE_STATUS)
			FSM_TEST_COND (SEND_STATUS)
			FSM_TEST_LOGIC ("WAIT")
			FSM_PROFILE_SECTION_OUT (state0_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "RECEIVE_STATUS", "", "WAIT", "BL_rcv", "tr_2", "EDStatusCollector [WAIT -> BL_rcv : RECEIVE_STATUS / ]")
				FSM_CASE_TRANSIT (1, 3, state3_enter_exec, ;, "SEND_STATUS", "", "WAIT", "sendStss", "tr_4", "EDStatusCollector [WAIT -> sendStss : SEND_STATUS / ]")
				}
				/*---------------------------------------------------------*/



			/** state (INIT) enter executives **/
			FSM_STATE_ENTER_FORCED_NOLABEL (1, "INIT", "EDStatusCollector [INIT enter execs]")
				FSM_PROFILE_SECTION_IN ("EDStatusCollector [INIT enter execs]", state1_enter_exec)
				{
				a = 0; counterPassedThreshold = 0;
				current_energy_level = 0;
				updated_rssi = 0;
				updated_rssi_src = -1;
				updated_rssi_dst = -1;
				
				//self_id   = op_id_self ();
				//parent_id = op_topo_parent (self_id);
				op_ima_obj_attr_get (op_topo_parent (op_id_self ()), "Enable Logging", &enable_log);
				
				if (enable_log)
					statusCollector_log_file_init();
				}
				FSM_PROFILE_SECTION_OUT (state1_enter_exec)

			/** state (INIT) exit executives **/
			FSM_STATE_EXIT_FORCED (1, "INIT", "EDStatusCollector [INIT exit execs]")


			/** state (INIT) transition processing **/
			FSM_TRANSIT_FORCE (0, state0_enter_exec, ;, "default", "", "INIT", "WAIT", "tr_12", "EDStatusCollector [INIT -> WAIT : default / ]")
				/*---------------------------------------------------------*/



			/** state (BL_rcv) enter executives **/
			FSM_STATE_ENTER_FORCED (2, "BL_rcv", state2_enter_exec, "EDStatusCollector [BL_rcv enter execs]")
				FSM_PROFILE_SECTION_IN ("EDStatusCollector [BL_rcv enter execs]", state2_enter_exec)
				{
				
				//   here we get node statisics from incoming packet 
				pkptr = op_pk_get (op_intrpt_strm ());
				
				if (intrpt_stream_indx==0)
				{// 
				op_pk_nfd_get (pkptr, "energy_level", &current_energy_level);
				//cout << " this packets come from Battery process to collector " << current_energy_level <<endl;
				}
				else if (intrpt_stream_indx==1) {
				//cout<<"SNR values inside packets come from MAC process to collector"<<endl;
				SDNWSAN_status_values *tempSvalues;
				op_pk_nfd_get_ptr (pkptr, "Stts_info", (void**)&tempSvalues);
				//op_pk_nfd_get (pkptr, "no_neighbours", &no_neighbours);
				no_neighbours = tempSvalues->neighbours_no;
				//if(no_neighbours<11)
				for (int i=0; i<no_neighbours; i++){
					neighbour_listS[i].node_address = tempSvalues->nighbourList[i].node_address;
					neighbour_listS[i].SNR = tempSvalues->nighbourList[i].SNR;
					}
				op_prg_mem_free(tempSvalues);
				}
				op_pk_destroy(pkptr);
				if (enable_log)
					fprintf (Slog_file, "neighbour list at this time\n");
				//	printf ("neighbour list at this time\n");
					for (int i=0; i<no_neighbours; i++) {
					if (enable_log)
						fprintf (Slog_file, "%d. node address: %d of rssi of: %f  \n", i,  neighbour_listS[i].node_address, neighbour_listS[i].SNR);
				//		printf ("%d. node address: %d of rssi of: %f \n", i,  neighbour_listS[i].node_address, neighbour_listS[i].SNR);
						}
				if (enable_log)
					fprintf (Slog_file, "*****************end of neighbour list*************************\n");
				//	printf ("************************end of neighbour list***************************\n");	
											
				
				
				if (a == 0)
					{
					a = 1;
					//send one time only 
					//op_pk_send(pkptr,OUT_STRM);
					
					//op_intrpt_schedule_self ((op_sim_time()+1.0),SendStss);
					op_intrpt_schedule_self ((op_sim_time() + op_dist_uniform (1.0)), SendStss);
				
					}
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** state (BL_rcv) exit executives **/
			FSM_STATE_EXIT_FORCED (2, "BL_rcv", "EDStatusCollector [BL_rcv exit execs]")


			/** state (BL_rcv) transition processing **/
			FSM_TRANSIT_FORCE (0, state0_enter_exec, ;, "default", "", "BL_rcv", "WAIT", "tr_3", "EDStatusCollector [BL_rcv -> WAIT : default / ]")
				/*---------------------------------------------------------*/



			/** state (sendStss) enter executives **/
			FSM_STATE_ENTER_FORCED (3, "sendStss", state3_enter_exec, "EDStatusCollector [sendStss enter execs]")
				FSM_PROFILE_SECTION_IN ("EDStatusCollector [sendStss enter execs]", state3_enter_exec)
				{
				
				 
					SDNWSAN_status_values *tempSvalues;
					pkptr = op_pk_create_fmt ("status_repo");
					tempSvalues = (SDNWSAN_status_values *) op_prg_mem_alloc (sizeof (SDNWSAN_status_values));
					tempSvalues->neighbours_no = no_neighbours;
					tempSvalues->energy_level = current_energy_level;
					
					//printf("Node Status BT level=%f\n",tempSvalues->energy_level);
					//if(no_neighbours<11)
					for (int i=0; i<no_neighbours; i++){
						tempSvalues->nighbourList[i].node_address = neighbour_listS[i].node_address;
						if (neighbour_listS[i].SNR<=1) neighbour_listS[i].SNR=1;
						tempSvalues->nighbourList[i].SNR = neighbour_listS[i].SNR;
						}
										
					op_pk_nfd_set_ptr (pkptr, "Stts_info", tempSvalues, op_prg_mem_copy_create, op_prg_mem_free, sizeof (SDNWSAN_status_values));
					//op_pk_print (pkptr);
					//setup the sending status acourding to threshold value 2
					if (current_energy_level >= 2) 		op_pk_send (pkptr,OUT_STRM);
					else if (current_energy_level < 2 && counterPassedThreshold < 4)
							{
							counterPassedThreshold++;// counter try after passed threshold
							op_pk_send (pkptr, OUT_STRM);
							
							}
					//op_pk_destroy(pkptr);
				
					//op_intrpt_schedule_self ((op_sim_time()+10),SendStss);
					
					if (a < 3)
						{
					op_intrpt_schedule_self ((op_sim_time() + op_dist_uniform (2.0)), SendStss);
					a++;
					} else 
						op_intrpt_schedule_self ((op_sim_time() + op_dist_uniform (25.0)), SendStss);
					//send the status every several seconds
				}
				FSM_PROFILE_SECTION_OUT (state3_enter_exec)

			/** state (sendStss) exit executives **/
			FSM_STATE_EXIT_FORCED (3, "sendStss", "EDStatusCollector [sendStss exit execs]")


			/** state (sendStss) transition processing **/
			FSM_TRANSIT_FORCE (0, state0_enter_exec, ;, "default", "", "sendStss", "WAIT", "tr_5", "EDStatusCollector [sendStss -> WAIT : default / ]")
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (1,"EDStatusCollector")
		}
	catch (...)
		{
		Vos_Error_Print (VOSC_ERROR_ABORT,
			(const char *)VOSC_NIL,
			"Unhandled C++ exception in process model (EDStatusCollector)",
			(const char *)VOSC_NIL, (const char *)VOSC_NIL);
		}
	}




void
EDStatusCollector_state::_op_EDStatusCollector_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}

void
EDStatusCollector_state::operator delete (void* ptr)
	{
	FIN (EDStatusCollector_state::operator delete (ptr));
	Vos_Poolmem_Dealloc (ptr);
	FOUT
	}

EDStatusCollector_state::~EDStatusCollector_state (void)
	{

	FIN (EDStatusCollector_state::~EDStatusCollector_state ())


	/* No Termination Block */


	FOUT
	}


#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

void *
EDStatusCollector_state::operator new (size_t)
#if defined (VOSD_NEW_BAD_ALLOC)
		throw (VOSD_BAD_ALLOC)
#endif
	{
	void * new_ptr;

	FIN_MT (EDStatusCollector_state::operator new ());

	new_ptr = Vos_Alloc_Object (EDStatusCollector_state::obtype);
#if defined (VOSD_NEW_BAD_ALLOC)
	if (new_ptr == VOSC_NIL) throw VOSD_BAD_ALLOC();
#endif
	FRET (new_ptr)
	}

/* State constructor initializes FSM handling */
/* by setting the initial state to the first */
/* block of code to enter. */

EDStatusCollector_state::EDStatusCollector_state (void) :
		_op_current_block (2)
	{
#if defined (OPD_ALLOW_ODB)
		_op_current_state = "EDStatusCollector [INIT enter execs]";
#endif
	}

VosT_Obtype
_op_EDStatusCollector_init (int * init_block_ptr)
	{
	FIN_MT (_op_EDStatusCollector_init (init_block_ptr))

	EDStatusCollector_state::obtype = Vos_Define_Object_Prstate ("proc state vars (EDStatusCollector)",
		sizeof (EDStatusCollector_state));
	*init_block_ptr = 2;

	FRET (EDStatusCollector_state::obtype)
	}

VosT_Address
_op_EDStatusCollector_alloc (VosT_Obtype, int)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	EDStatusCollector_state * ptr;
	FIN_MT (_op_EDStatusCollector_alloc ())

	/* New instance will have FSM handling initialized */
#if defined (VOSD_NEW_BAD_ALLOC)
	try {
		ptr = new EDStatusCollector_state;
	} catch (const VOSD_BAD_ALLOC &) {
		ptr = VOSC_NIL;
	}
#else
	ptr = new EDStatusCollector_state;
#endif
	FRET ((VosT_Address)ptr)
	}



void
_op_EDStatusCollector_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	EDStatusCollector_state		*prs_ptr;

	FIN_MT (_op_EDStatusCollector_svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (EDStatusCollector_state *)gen_ptr;

	if (strcmp ("current_energy_level" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->current_energy_level);
		FOUT
		}
	if (strcmp ("a" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->a);
		FOUT
		}
	if (strcmp ("updated_rssi" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->updated_rssi);
		FOUT
		}
	if (strcmp ("updated_rssi_src" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->updated_rssi_src);
		FOUT
		}
	if (strcmp ("updated_rssi_dst" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->updated_rssi_dst);
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
	if (strcmp ("intrpt_stream_indx" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->intrpt_stream_indx);
		FOUT
		}
	if (strcmp ("neighbour_list" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->neighbour_list);
		FOUT
		}
	if (strcmp ("no_neighbours" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->no_neighbours);
		FOUT
		}
	if (strcmp ("neighbour_listS" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->neighbour_listS);
		FOUT
		}
	if (strcmp ("Slog_file" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->Slog_file);
		FOUT
		}
	if (strcmp ("enable_log" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->enable_log);
		FOUT
		}
	if (strcmp ("counterPassedThreshold" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->counterPassedThreshold);
		FOUT
		}
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

