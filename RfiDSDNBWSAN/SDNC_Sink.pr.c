/* Process model C form file: SDNC_Sink.pr.c */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char SDNC_Sink_pr_c [] = "MIL_3_Tfile_Hdr_ 145A 30A op_runsim 7 5A2F81E4 5A2F81E4 1 DESKTOP-JCGS160 Mohammed@Hussein 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1e80 8                                                                                                                                                                                                                                                                                                                                                                                       ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

/* Include files		*/
#include "headers\wpan_params.h"
#include <stdlib.h>
#include <math.h>


/* Function prototypes */
static void wpan_parse_incoming_packet (void);


#define TO_NETWORK_INTELLIGENCE  0
#define STATUS_REPORT  12

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
typedef struct
	{
	/* Internal state tracking for FSM */
	FSM_SYS_STATE
	/* State Variables */
	Stathandle	             		bits_rcvd_stathandle                            ;
	Stathandle	             		bitssec_rcvd_stathandle                         ;
	Stathandle	             		pkts_rcvd_stathandle                            ;
	Stathandle	             		pktssec_rcvd_stathandle                         ;
	Stathandle	             		ete_delay_stathandle                            ;	/* end to end delay (seconds) */
	Stathandle	             		ete_delay_stathandleG                           ;
	Stathandle	             		ete_delay_command_stathandle                    ;
	Stathandle	             		ete_delay_command_stathandleG                   ;
	Stathandle	             		ete_delay_data_stathandle                       ;
	Stathandle	             		ete_delay_data_stathandleG                      ;
	Stathandle	             		ete_delay_ack_stathandle                        ;
	Stathandle	             		ete_delay_ack_stathandleG                       ;
	double	                 		ete_delay_prev                                  ;
	Stathandle	             		jitter_stathandle                               ;
	} SDNC_Sink_state;

#define pr_state_ptr            		((SDNC_Sink_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))
#define bits_rcvd_stathandle    		pr_state_ptr->bits_rcvd_stathandle
#define bitssec_rcvd_stathandle 		pr_state_ptr->bitssec_rcvd_stathandle
#define pkts_rcvd_stathandle    		pr_state_ptr->pkts_rcvd_stathandle
#define pktssec_rcvd_stathandle 		pr_state_ptr->pktssec_rcvd_stathandle
#define ete_delay_stathandle    		pr_state_ptr->ete_delay_stathandle
#define ete_delay_stathandleG   		pr_state_ptr->ete_delay_stathandleG
#define ete_delay_command_stathandle		pr_state_ptr->ete_delay_command_stathandle
#define ete_delay_command_stathandleG		pr_state_ptr->ete_delay_command_stathandleG
#define ete_delay_data_stathandle		pr_state_ptr->ete_delay_data_stathandle
#define ete_delay_data_stathandleG		pr_state_ptr->ete_delay_data_stathandleG
#define ete_delay_ack_stathandle		pr_state_ptr->ete_delay_ack_stathandle
#define ete_delay_ack_stathandleG		pr_state_ptr->ete_delay_ack_stathandleG
#define ete_delay_prev          		pr_state_ptr->ete_delay_prev
#define jitter_stathandle       		pr_state_ptr->jitter_stathandle

/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	SDNC_Sink_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((SDNC_Sink_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


/* Function Block */

#if !defined (VOSD_NO_FIN)
enum { _op_block_origin = __LINE__ + 2};
#endif

/*--------------------------------------------------------------------------------
 * Function:	wpan_parse_incoming_packet
 *
 * Description:	Receive a packet from MAC layer (wpan_mac) and update statistics.
 *
 * No parameters
 *--------------------------------------------------------------------------------*/
static void wpan_parse_incoming_packet () {

	Packet*	pkptr;
	double	pk_size;
	double	ete_delay; // end-to-end delay
	int frame_type;
	char	pk_format[25];
	double jitter;
//	Packet* command_frame;
//	Packet* mac_copy_frame;
//	int command_frm_id;
	//Packet* commandCopy;
	/* Obtain the incoming packet.	*/
	pkptr = op_pk_get (op_intrpt_strm ());

	/* Caclulate metrics to be updated.		*/
	pk_size = (double) op_pk_total_size_get (pkptr);
	ete_delay = op_sim_time () - op_pk_stamp_time_get (pkptr);
	jitter = fabs(ete_delay - ete_delay_prev);
	ete_delay_prev = ete_delay;
	
	/* Update local statistics.				*/
	op_stat_write (bits_rcvd_stathandle, 		pk_size);
	op_stat_write (pkts_rcvd_stathandle, 		1.0);
	op_stat_write (bitssec_rcvd_stathandle, 	pk_size);
	op_stat_write (bitssec_rcvd_stathandle, 	0.0);
	op_stat_write (pktssec_rcvd_stathandle, 	1.0);
	op_stat_write (pktssec_rcvd_stathandle, 	0.0);
	op_stat_write (ete_delay_stathandle, 		ete_delay);
	op_stat_write (ete_delay_stathandleG, 		ete_delay);
	op_stat_write (jitter_stathandle, 		jitter);
	//op_pk_print(pkptr);
	//printf("hbi99 =====from sink to OpenFlow Network Intelligence=========\n");
	op_pk_format(pkptr,pk_format);
	
	if (strcmp("wpan_frame_MPDU_format",pk_format)==0)
	{
	op_pk_nfd_get(pkptr, "Frame Type", &frame_type);
	
	switch (frame_type) {
		case COMMAND_FRAME_TYPE:
		{
			op_stat_write (ete_delay_command_stathandle, 		ete_delay);
			op_stat_write (ete_delay_command_stathandleG, 		ete_delay);
//			mac_copy_frame = op_pk_copy (pkptr);
//			op_pk_nfd_get_pkt (pkptr, "MAC Frame Payload", &command_frame);
//			commandCopy = op_pk_copy(command_frame);
//			op_pk_nfd_get (command_frame, "Command Frame Identifier", &command_frm_id);
//			if(command_frm_id==STATUS_REPORT) 
//			{
op_pk_send(pkptr,TO_NETWORK_INTELLIGENCE);
//				} else {
//						op_pk_destroy (pkptr);
//						}
//			op_pk_destroy(mac_copy_frame);
			break;
		}
		
		case ACK_FRAME_TYPE:
		{
			op_stat_write (ete_delay_ack_stathandle, 		ete_delay);
			op_stat_write (ete_delay_ack_stathandleG, 		ete_delay);
			op_pk_destroy (pkptr);
			break;
		}
		
		case DATA_FRAME_TYPE:
		{
			op_stat_write (ete_delay_data_stathandle, 		ete_delay);
			op_stat_write (ete_delay_data_stathandleG, 		ete_delay);
			op_pk_destroy (pkptr);
			break;
		}
		default:{}
		}
	} 
	else{
			op_stat_write (ete_delay_data_stathandle, 		ete_delay);
			op_stat_write (ete_delay_data_stathandleG, 		ete_delay);
			op_pk_destroy (pkptr);
}

	/* Destroy the received packet.	*/
}

/* End of Function Block */

/* Undefine optional tracing in FIN/FOUT/FRET */
/* The FSM has its own tracing code and the other */
/* functions should not have any tracing.		  */
#undef FIN_TRACING
#define FIN_TRACING

#undef FOUTRET_TRACING
#define FOUTRET_TRACING

#if defined (__cplusplus)
extern "C" {
#endif
	void SDNC_Sink (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Obtype _op_SDNC_Sink_init (int * init_block_ptr);
	void _op_SDNC_Sink_diag (OP_SIM_CONTEXT_ARG_OPT);
	void _op_SDNC_Sink_terminate (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Address _op_SDNC_Sink_alloc (VosT_Obtype, int);
	void _op_SDNC_Sink_svar (void *, const char *, void **);


#if defined (__cplusplus)
} /* end of 'extern "C"' */
#endif




/* Process model interrupt handling procedure */


void
SDNC_Sink (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (SDNC_Sink ());

		{


		FSM_ENTER_NO_VARS ("SDNC_Sink")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (init) enter executives **/
			FSM_STATE_ENTER_FORCED_NOLABEL (0, "init", "SDNC_Sink [init enter execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_Sink [init enter execs]", state0_enter_exec)
				{
				/* Initilaize the statistic handles to keep	track of traffic sinked by this process. */
				bits_rcvd_stathandle 		= op_stat_reg ("Traffic Sink.Traffic Received (bits)",			OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
				bitssec_rcvd_stathandle 	= op_stat_reg ("Traffic Sink.Traffic Received (bits/sec)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
				pkts_rcvd_stathandle 		= op_stat_reg ("Traffic Sink.Traffic Received (packets)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
				pktssec_rcvd_stathandle 	= op_stat_reg ("Traffic Sink.Traffic Received (packets/sec)",	OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
				
				ete_delay_stathandle			= op_stat_reg ("Traffic Sink.End-to-End Delay (seconds)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
				ete_delay_command_stathandle	= op_stat_reg ("Traffic Sink.End-to-End Delay Command Frames (seconds)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
				ete_delay_data_stathandle		= op_stat_reg ("Traffic Sink.End-to-End Delay Data Frames (seconds)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
				ete_delay_ack_stathandle		= op_stat_reg ("Traffic Sink.End-to-End Delay ACK Frames (seconds)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
				
				ete_delay_stathandleG			= op_stat_reg ("Traffic Sink.End-to-End Delay (seconds)",		OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
				ete_delay_command_stathandleG	= op_stat_reg ("Traffic Sink.End-to-End Delay Command Frames (seconds)",		OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
				ete_delay_data_stathandleG		= op_stat_reg ("Traffic Sink.End-to-End Delay Data Frames (seconds)",		OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
				ete_delay_ack_stathandleG		= op_stat_reg ("Traffic Sink.End-to-End Delay ACK Frames (seconds)",		OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
				
				jitter_stathandle			= op_stat_reg ("Traffic Sink.Jitter (seconds)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
				
				
				
				ete_delay_prev = 0;
				}
				FSM_PROFILE_SECTION_OUT (state0_enter_exec)

			/** state (init) exit executives **/
			FSM_STATE_EXIT_FORCED (0, "init", "SDNC_Sink [init exit execs]")


			/** state (init) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "init", "idle", "tr_3", "SDNC_Sink [init -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (idle) enter executives **/
			FSM_STATE_ENTER_UNFORCED (1, "idle", state1_enter_exec, "SDNC_Sink [idle enter execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_Sink [idle enter execs]", state1_enter_exec)
				{
				switch (op_intrpt_type()) {
					case OPC_INTRPT_STRM : 
						{
							wpan_parse_incoming_packet ();
							break;
						};
				
					default :
						{
						};
				}
				}
				FSM_PROFILE_SECTION_OUT (state1_enter_exec)

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (3,"SDNC_Sink")


			/** state (idle) exit executives **/
			FSM_STATE_EXIT_UNFORCED (1, "idle", "SDNC_Sink [idle exit execs]")


			/** state (idle) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "idle", "idle", "tr_4", "SDNC_Sink [idle -> idle : default / ]")
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (0,"SDNC_Sink")
		}
	}




void
_op_SDNC_Sink_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}




void
_op_SDNC_Sink_terminate (OP_SIM_CONTEXT_ARG_OPT)
	{

	FIN_MT (_op_SDNC_Sink_terminate ())


	/* No Termination Block */

	Vos_Poolmem_Dealloc (pr_state_ptr);

	FOUT
	}


/* Undefine shortcuts to state variables to avoid */
/* syntax error in direct access to fields of */
/* local variable prs_ptr in _op_SDNC_Sink_svar function. */
#undef bits_rcvd_stathandle
#undef bitssec_rcvd_stathandle
#undef pkts_rcvd_stathandle
#undef pktssec_rcvd_stathandle
#undef ete_delay_stathandle
#undef ete_delay_stathandleG
#undef ete_delay_command_stathandle
#undef ete_delay_command_stathandleG
#undef ete_delay_data_stathandle
#undef ete_delay_data_stathandleG
#undef ete_delay_ack_stathandle
#undef ete_delay_ack_stathandleG
#undef ete_delay_prev
#undef jitter_stathandle

#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

VosT_Obtype
_op_SDNC_Sink_init (int * init_block_ptr)
	{
	VosT_Obtype obtype = OPC_NIL;
	FIN_MT (_op_SDNC_Sink_init (init_block_ptr))

	obtype = Vos_Define_Object_Prstate ("proc state vars (SDNC_Sink)",
		sizeof (SDNC_Sink_state));
	*init_block_ptr = 0;

	FRET (obtype)
	}

VosT_Address
_op_SDNC_Sink_alloc (VosT_Obtype obtype, int init_block)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	SDNC_Sink_state * ptr;
	FIN_MT (_op_SDNC_Sink_alloc (obtype))

	ptr = (SDNC_Sink_state *)Vos_Alloc_Object (obtype);
	if (ptr != OPC_NIL)
		{
		ptr->_op_current_block = init_block;
#if defined (OPD_ALLOW_ODB)
		ptr->_op_current_state = "SDNC_Sink [init enter execs]";
#endif
		}
	FRET ((VosT_Address)ptr)
	}



void
_op_SDNC_Sink_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	SDNC_Sink_state		*prs_ptr;

	FIN_MT (_op_SDNC_Sink_svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (SDNC_Sink_state *)gen_ptr;

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
	if (strcmp ("ete_delay_stathandleG" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->ete_delay_stathandleG);
		FOUT
		}
	if (strcmp ("ete_delay_command_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->ete_delay_command_stathandle);
		FOUT
		}
	if (strcmp ("ete_delay_command_stathandleG" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->ete_delay_command_stathandleG);
		FOUT
		}
	if (strcmp ("ete_delay_data_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->ete_delay_data_stathandle);
		FOUT
		}
	if (strcmp ("ete_delay_data_stathandleG" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->ete_delay_data_stathandleG);
		FOUT
		}
	if (strcmp ("ete_delay_ack_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->ete_delay_ack_stathandle);
		FOUT
		}
	if (strcmp ("ete_delay_ack_stathandleG" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->ete_delay_ack_stathandleG);
		FOUT
		}
	if (strcmp ("ete_delay_prev" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->ete_delay_prev);
		FOUT
		}
	if (strcmp ("jitter_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->jitter_stathandle);
		FOUT
		}
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

