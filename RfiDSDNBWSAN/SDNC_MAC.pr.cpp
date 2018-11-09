/* Process model C++ form file: SDNC_MAC.pr.cpp */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char SDNC_MAC_pr_cpp [] = "MIL_3_Tfile_Hdr_ 145A 30A modeler 7 5B55C011 5B55C011 1 DESKTOP-JCGS160 Mohammed@Hussein 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1e80 8                                                                                                                                                                                                                                                                                                                                                                                         ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

#include <math.h>
#include <iostream>
#include <cstdlib>
#include <vector>
using namespace std;
#include "headers\wpan_struct.h"
#include "headers\wpan_params.h"
#include "headers\wpan_math.h"



//#include <prg_list_funcs.h>

// NDP flag ON OFF
#define flag_ON		1
#define flag_OFF	0
#define NDP_BEACON_FRAME_TYPE	4

// NDP neighbour list indeces
#define node_address_index		0
#define node_rssi_index    		1

/* TDP codes */
#define DEFAULT_SOBO	0


/* Input and output streams		*/
#define STRM_FROM_NWK_TO_MAC 			3
#define STRM_FROM_MAC_TO_NWK			2
#define STRM_FROM_RADIO_TO_MAC			1
#define STRM_FROM_MAC_TO_RADIO			1
#define STRM_FROM_MAC_TO_STATUS_COL		5
#define STRM_FROM_STATUS_COL_TO_MAC		6
#define STRM_FROM_NeIntel_TO_MAC  		9
#define STRM_FROM_MAC_TO_SINK    		8
#define STRM_FROM_TDP_TO_MAC			2


//GTS and NeIntel remots
#define GTS_request_to_NeIntel 		901

/* Statistics Input Index */
#define TX_BUSY_STAT 		0
#define RX_BUSY_STAT 		1
#define RX_COLLISION_STAT 	2


/* Constants */
#define epsilon 0.000001


// OPC_FALSE = initally full GTS buffer, OPC_TRUE = initially empty GTS buffer
#define INITIANLLY_EMPTY_GTS_BUFFER OPC_TRUE

// subqueues
#define SUBQ_CAP 0
#define SUBQ_GTS 1

#define TIME_SLOT_INDEX_MAX (aNumSuperframeSlots-1)

// device->parent
#define GTS_DIRECTION_TRANSMIT 	0
// parent->device
#define GTS_DIRECTION_RECEIVE 	1

// superframe parameters
#define SF_in	1
#define SF_out	0

// define the time spend in the CCA = 8 symbols
#define	WPAN_CCA_DETECTION_TIME_Symbols	8


/* GTS characteristics type */
#define GTS_DEALLOCATION 	0
#define GTS_ALLOCATION 		1


/* Interrupt types		*/
#define DEFAULT_CODE 					0
#define BEACON_INTERVAL_CODE 			1
#define TIME_SLOT_CODE 					2
#define TRY_PACKET_TRANSMISSION_CODE 	3
#define BACKOFF_EXPIRATION_CODE 		4
#define START_OF_CAP_PERIOD_CODE 		5
#define END_OF_CFP_PERIOD_CODE 			6

// the same value should be in the Battery Module for remote process
#define PACKET_TX_CODE 				101
#define PACKET_RX_CODE 				102
#define END_OF_SLEEP_PERIOD 		103
#define END_OF_ACTIVE_PERIOD_CODE 	104
#define END_OF_CAP_PERIOD_CODE 		105
#define MAC_TO_TDP					106

//CSMA
#define CCA_START_CODE 			10
#define CCA_EXPIRATION_CODE 	11
#define RETURN_TO_BACKOFF_CODE 	12
#define BACKOFF_RESUME_CODE		13
#define CCA_RESUME_CODE			14
#define BACKOFF_DEFER_CODE		15
#define CCA_DEFER_CODE			16
#define START_OF_CSMA			17

#define FAILURE_CODE 			20
#define SUCCESS_CODE 			21
#define START_TRANSMISSION_CODE 22

#define WAITING_ACK_END_CODE 	31
#define ACK_SUCCESS_CODE 		32
#define ACK_SEND_CODE 			33

// GTS
#define START_OF_TRANSMIT_GTS_USE 			200
#define STOP_OF_TRANSMIT_GTS_USE 			201
#define START_OF_RECEIVE_GTS_USE 			202
#define STOP_OF_RECEIVE_GTS_USE 			203
#define START_OF_GTS_PERIOD 				210
#define END_OF_GTS_PERIOD 					211
#define TRY_GTS_PACKET_TRANSMISSION_CODE 	230
#define START_GTS_TRANSMISSION_CODE			231



/* State machine conditions 		*/
#define IAM_PAN_COORDINATOR 	(device_mode == PAN_COORDINATOR)
#define IAM_ROUTER 				(device_mode == ROUTER)
#define IAM_END_DEVICE 			(device_mode == END_DEVICE)

#define IAM_WAITING_ACK (node_attributes.wait_ack == OPC_TRUE)

#define BACKOFF_EXPIRED (op_intrpt_type () == OPC_INTRPT_SELF && op_intrpt_code () == BACKOFF_EXPIRATION_CODE) 
#define	DEFAULT_INTRPT	(op_intrpt_type () == OPC_INTRPT_SELF && op_intrpt_code () == DEFAULT_CODE) 

/*#define PACKET_READY_TO_SEND (((SF_params[SF_in].CAP_ACTIVE==OPC_TRUE) || (SF_params[SF_out].CAP_ACTIVE==OPC_TRUE)) &&\
                              !op_subq_empty(SUBQ_CAP) && \
                              (op_stat_local_read(TX_BUSY_STAT)==0.0) && \
                              (op_intrpt_type () == OPC_INTRPT_SELF && op_intrpt_code () == TRY_PACKET_TRANSMISSION_CODE))
*/

#define START_CSMA (op_intrpt_type () == OPC_INTRPT_SELF && op_intrpt_code () == START_OF_CSMA)

#define RESUME_BACKOFF (!op_subq_empty(SUBQ_CAP) && (op_intrpt_type () == OPC_INTRPT_SELF && op_intrpt_code () == BACKOFF_RESUME_CODE))
	
#define RESUME_CCA (!op_subq_empty(SUBQ_CAP) && (op_intrpt_type () == OPC_INTRPT_SELF && op_intrpt_code () == CCA_RESUME_CODE))	
	
#define DEFER_BACKOFF (op_intrpt_type () == OPC_INTRPT_SELF && op_intrpt_code () == BACKOFF_DEFER_CODE)

#define CHANNEL_BUSY (op_intrpt_type () == OPC_INTRPT_SELF && op_intrpt_code () == RETURN_TO_BACKOFF_CODE)

#define TX_FAILURE 		(op_intrpt_type () == OPC_INTRPT_SELF && op_intrpt_code () == FAILURE_CODE)
#define TX_SUCCESS 		(op_intrpt_type () == OPC_INTRPT_SELF && op_intrpt_code () == SUCCESS_CODE)
#define RX_ACK_SUCCESS 	(op_intrpt_type () == OPC_INTRPT_SELF && op_intrpt_code () == ACK_SUCCESS_CODE)
#define CCA_DEFER		(op_intrpt_type () == OPC_INTRPT_SELF && op_intrpt_code () == CCA_DEFER_CODE)
	
#define CCA_END (TX_FAILURE || (TX_SUCCESS && !IAM_WAITING_ACK) || RX_ACK_SUCCESS || CCA_DEFER)
	
#define GTS_START ((SF_params[SF_in].CFP_ACTIVE || SF_params[SF_out].CFP_ACTIVE) && (op_intrpt_type () == OPC_INTRPT_SELF && op_intrpt_code () == START_OF_GTS_PERIOD))

#define GTS_STOP ((SF_params[SF_in].CFP_ACTIVE || SF_params[SF_out].CFP_ACTIVE) && (op_intrpt_type () == OPC_INTRPT_SELF && op_intrpt_code () == END_OF_GTS_PERIOD))

/*#define GTS_START ((SF.CFP_ACTIVE==OPC_TRUE) && (SF.CAP_ACTIVE==OPC_FALSE) &&\
					(SF.SLEEP==OPC_FALSE) && (GTS.GTS_ACTIVE==OPC_TRUE))

#define GTS_STOP ((SF.CFP_ACTIVE==OPC_TRUE) && (SF.CAP_ACTIVE==OPC_FALSE) &&\
					(SF.SLEEP==OPC_FALSE) && (GTS.GTS_ACTIVE==OPC_FALSE))


#define GTS_STOP_IDLE	((SF.CFP_ACTIVE==OPC_TRUE) && (SF.CAP_ACTIVE==OPC_FALSE) &&\
						(SF.SLEEP==OPC_FALSE) && (GTS.GTS_ACTIVE==OPC_FALSE) && (SF.CCA_DEFERRED==OPC_FALSE))
#define GTS_STOP_CCA	((SF.CFP_ACTIVE==OPC_TRUE) && (SF.CAP_ACTIVE==OPC_FALSE) &&\
						(SF.SLEEP==OPC_FALSE) && (GTS.GTS_ACTIVE==OPC_FALSE) && (SF.CCA_DEFERRED==OPC_TRUE))
*/
/* Structures		*/
/* statistic vectors */
typedef struct {
	Stathandle failure;
	Stathandle success;
	Stathandle ch_busy_cca1;
	Stathandle ch_busy_cca2;
	Stathandle ch_idle_cca1;
	Stathandle ch_idle_cca2;
	Stathandle deferred_cca;
	Stathandle dropped_packets;
	Stathandle retransmission_attempts;
	Stathandle success_ack_packets;
	Stathandle backoff_delay;
	Stathandle mac_delay;
	Stathandle backoff_units;
	Stathandle sent_pkt;
	Stathandle deferred_cca_backoff;	
} wpan_statistic_vector;

// flow table structure
typedef struct {
int node_address;
int src;
int act;
int dest;
} flow_table;

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

// node gts characteristics structure
typedef struct {
int node_address;
int type;
int direction;
int length;
} node_gts_chrcts;

typedef struct {
	Stathandle failure;
	Stathandle success;
	Stathandle ch_busy_cca1;
	Stathandle ch_busy_cca2;
	Stathandle ch_idle_cca1;
	Stathandle ch_idle_cca2;
	Stathandle deferred_cca;
	Stathandle dropped_packets;
	Stathandle retransmission_attempts;
	Stathandle success_ack_packets;
	Stathandle backoff_delay;
	Stathandle mac_delay;
	Stathandle backoff_units;
	Stathandle sent_pkt;
	Stathandle rnd_generator;
} wpan_global_statistic_vector; 

typedef struct {
	Stathandle sent_frames;	// number of sent MAC frames
	Stathandle sent_framesG;
	Stathandle frame_delay;	// MAC frame delay (time between the MSDU generation and frame trasmission)
	Stathandle wasted_bandwidth_pc; // [%]
	Stathandle throughput_pc;	// [%]
	Stathandle buffer_saturation_pc;	// [%]
} wpan_gts_statistic_vector;

typedef struct {
	double band_wast_sum; // [%] sum of the wasted bandwidth (IFS, waiting, ACK) during the using of GTS
	double frame_delay_max;	// MAC frame delay (time between the MSDU generation and frame trasmission)
	double frame_delay_min;
	double frame_delay_sum;
	int sent_frames;	// total number of MAC frames sent during the using of GTS
	double sent_bits;	// total number of data bits sent during the using of GTS
	int stored_frames;	// total number of MAC frames stored in the buffer
	double stored_bits;	// total number of data bits stored in the buffer
	int dropped_frames;	// number of MAC frames which is dropped because of full buffer
	double dropped_bits;	// number of data bits which is dropped because of full buffer
	int gts_periods_count;	// number of GTS periods during the using of GTS	
	int received_frames;
	double received_bits;
} wpan_gts_statictics;


/* Function prototypes.				*/
static void wpan_mac_init (void);
static void wpan_log_file_init (void);
static void wpan_print_parameters (void);
static void wpan_parse_incoming_frame (void);
static void wpan_encapsulate_and_enqueue_command_frame (Packet* cmd_frame, Boolean ack, int dest_address);
static void wpan_encapsulate_and_enqueue_data_frame (Packet* data_frame, Boolean ack, int dest_address, int traffic_type);
//static void wpan_extract_beacon_frame (Packet* mac_frame);
static void wpan_extract_beacon_frame (Packet* mac_frame,int Beacon_source_address);
static void wpan_extract_command_frame (Packet* mac_frame);
static void wpan_extract_ack_frame (Packet* mac_frame);
static void wpan_extract_data_frame (Packet* mac_frame);
static void wpan_send_beacon_frame (void);
static void wpan_send_ack_frame (int sequence_number);
static void wpan_mac_interrupt_process (void);
static int 	wpan_CAP_packet_search (void);

// backoff functions
static void   wpan_backoff_init (void);
static void   wpan_backoff_delay_set (void);
static int    wpan_backoff_period_index_get (void);
static double wpan_backoff_period_boundary_get (void);
static int 	  wpan_backoff_period_index_get_ (double Time);
static double wpan_backoff_period_boundary_get_ (double Time);

// cca functions
static void	wpan_cca_defer (void);
static void wpan_cca_perform (int cca_index);
static void wpan_cca_expire (void);

static int wpan_current_time_slot_get (void);
static int wpan_ifs_symbol_get (Packet* frame_to_send);
static int wpan_update_sequence_number (void);

// battery update functions
static void wpan_battery_update_tx (double pksize);
static void wpan_battery_update_rx (double pksize, int frame_type);

// GTS functions
static void wpan_gts_scheduling (void);
static void wpan_gts_allocation_request (int);
static void wpan_gts_deallocation_request (int);
static void wpan_gts_reallocation_request (void);
static void wpan_gts_start (int);
static void wpan_gts_stop (int);
static void wpan_gts_request_extract (Packet* mac_frame);
static Boolean wpan_gts_check_time (Packet* frame_to_send);
static void wpan_gts_slot_init (void);
int compare_GTS_listc (const void *aptr, const void *bptr);
//static void Extract_NeIntel_decisions_gts_characteristics (PrgT_List* gts_characteristics);
// utility functions
static void* gts_nested_list_mem_copy_create (void *from_ptr, unsigned int size);
static void gts_nested_list_mem_free (void *from_ptr);
static void queue_status (void);

/* Global variables	*/
//extern double PPDU_sent_bits;	// Total number of bits (PPDU) dispatched to the network [kbits]

static void* flowTables_nested_list_mem_copy_create (void* list_ptr, unsigned int size);
static void sdn_wsan_send_ndp_beacon_frame();

// start time list of the nodes dynamic approach
typedef struct {
int nodes_no_to_be_set;
int node_address[15];
double startTime[15];
int so[15];
int bo[15];
} startTimeSoBo_list;

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
class SDNC_MAC_state
	{
	private:
		/* Internal state tracking for FSM */
		FSM_SYS_STATE

	public:
		SDNC_MAC_state (void);

		/* Destructor contains Termination Block */
		~SDNC_MAC_state (void);

		/* State Variables */
		wpan_node_attributes	   		node_attributes                                 ;	/* the attributes of the node (MAC Address, PAN ID, ...) */
		wpan_csma_attributes	   		csma_params                                     ;	/* the attributes of csma/ca (backoff, BE, ...) */
		wpan_gts_attributes	    		GTS_params[2]                                   ;
		wpan_gts_attributes*	   		GTS_ptr                                         ;	/* pointer to the current GTS structure */
		wpan_gts_list_record*	  		GTS_list_ptr                                    ;
		List*	                  		GTS_list_temp                                   ;
		wpan_superframe_structure			SF_params[2]                                    ;	/* outgoing superframe, i.e. my own superframe (I am sending beacon). */
		                        		                                                	/* incoming superframe, i.e. superframe of my parent node             */
		wpan_superframe_structure*			SF_ptr                                          ;	/* pointer to the current Superframe structure (incoming or outgoing) */
		wpan_statistic_vector	  		statistic_vector                                ;	/* the statistic vector */
		wpan_global_statistic_vector			statistic_global_vector                         ;
		int	                    		ack_sequence_number                             ;	/* sequence number of the received packet */
		double	                 		backoff_start_time                              ;	/* the time when the packet enters the backoff */
		int	                    		Final_CAP_slot_next                             ;	/* value of SF.Final_CAP_slot variable valid in the next superframe */
		int	                    		sent_gts_bits                                   ;	/* number of bits sent during one GTS period */
		wpan_gts_statistic_vector			gts_statistic_vector                            ;	/* GTS statistic vector */
		wpan_gts_statictics	    		gts_statistics                                  ;	/* GTS statistic scalars */
		double	                 		gts_pkt_delay                                   ;	/* delay of the GTS packet - from the creation time to the transmission time */
		int	                    		cfp_queue_capacity                              ;	/* the capacity of the CFP buffer [bits] */
		int	                    		cap_queue_capacity                              ;	/* the capacity of the CAP buffer [bits] */
		Boolean	                		start_GTS_transmission                          ;
		Boolean	                		waiting_for_first_beacon                        ;	/* GTS request was issued before reception of first beacon         */
		                        		                                                	/* (usually when GTS.Start Time = 0) - wait for first beacon and   */
		                        		                                                	/* then check if my parent accepts GTS request (GTS permit = TRUE) */
		int	                    		superframe_owner                                ;	/* owner of the current SD */
		Objid	                  		self_id                                         ;
		Objid	                  		parent_id                                       ;
		Objid	                  		baterry_id                                      ;
		int	                    		my_address                                      ;
		int	                    		device_mode                                     ;
		char	                   		device_name[20]                                 ;
		Boolean	                		enable_log                                      ;	/* if enabled, all the operation will be saved in a log file */
		FILE *	                 		log_file                                        ;	/* a log file to store the operations for each node */
		int	                    		my_parent_address                               ;
		int	                    		packet_to_send_index                            ;
		int	                    		collision_occurred                              ;	/* Packet collision occurred */
		double	                 		rx_power_threshold                              ;
		PrgT_List*	             		Flow_Tables_list                                ;
		int	                    		flowTable_sending_needed                        ;
		int	                    		PANsink_mac_address                             ;
		Boolean	                		NDP_is_required                                 ;
		int	                    		NDP_beacon_seq_num                              ;
		double	                 		Neighbour_list[15][2]                           ;
		int	                    		No_of_neighbours                                ;
		Objid	                  		tdp_id                                          ;
		Boolean	                		tdp_decision                                    ;
		int	                    		tdp_code                                        ;
		startTimeSoBo_list*	    		startTimeSoBo_list_ptr                          ;
		vector<FlowEntery*>	    		flowEntriesP                                    ;
		Objid	                  		grandfatherID                                   ;
		int	                    		Run1time                                        ;

		/* FSM code */
		void SDNC_MAC (OP_SIM_CONTEXT_ARG_OPT);
		/* Diagnostic Block */
		void _op_SDNC_MAC_diag (OP_SIM_CONTEXT_ARG_OPT);

#if defined (VOSD_NEW_BAD_ALLOC)
		void * operator new (size_t) throw (VOSD_BAD_ALLOC);
#else
		void * operator new (size_t);
#endif
		void operator delete (void *);

		/* Memory management */
		static VosT_Obtype obtype;
	};

VosT_Obtype SDNC_MAC_state::obtype = (VosT_Obtype)OPC_NIL;

#define pr_state_ptr            		((SDNC_MAC_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))
#define node_attributes         		pr_state_ptr->node_attributes
#define csma_params             		pr_state_ptr->csma_params
#define GTS_params              		pr_state_ptr->GTS_params
#define GTS_ptr                 		pr_state_ptr->GTS_ptr
#define GTS_list_ptr            		pr_state_ptr->GTS_list_ptr
#define GTS_list_temp           		pr_state_ptr->GTS_list_temp
#define SF_params               		pr_state_ptr->SF_params
#define SF_ptr                  		pr_state_ptr->SF_ptr
#define statistic_vector        		pr_state_ptr->statistic_vector
#define statistic_global_vector 		pr_state_ptr->statistic_global_vector
#define ack_sequence_number     		pr_state_ptr->ack_sequence_number
#define backoff_start_time      		pr_state_ptr->backoff_start_time
#define Final_CAP_slot_next     		pr_state_ptr->Final_CAP_slot_next
#define sent_gts_bits           		pr_state_ptr->sent_gts_bits
#define gts_statistic_vector    		pr_state_ptr->gts_statistic_vector
#define gts_statistics          		pr_state_ptr->gts_statistics
#define gts_pkt_delay           		pr_state_ptr->gts_pkt_delay
#define cfp_queue_capacity      		pr_state_ptr->cfp_queue_capacity
#define cap_queue_capacity      		pr_state_ptr->cap_queue_capacity
#define start_GTS_transmission  		pr_state_ptr->start_GTS_transmission
#define waiting_for_first_beacon		pr_state_ptr->waiting_for_first_beacon
#define superframe_owner        		pr_state_ptr->superframe_owner
#define self_id                 		pr_state_ptr->self_id
#define parent_id               		pr_state_ptr->parent_id
#define baterry_id              		pr_state_ptr->baterry_id
#define my_address              		pr_state_ptr->my_address
#define device_mode             		pr_state_ptr->device_mode
#define device_name             		pr_state_ptr->device_name
#define enable_log              		pr_state_ptr->enable_log
#define log_file                		pr_state_ptr->log_file
#define my_parent_address       		pr_state_ptr->my_parent_address
#define packet_to_send_index    		pr_state_ptr->packet_to_send_index
#define collision_occurred      		pr_state_ptr->collision_occurred
#define rx_power_threshold      		pr_state_ptr->rx_power_threshold
#define Flow_Tables_list        		pr_state_ptr->Flow_Tables_list
#define flowTable_sending_needed		pr_state_ptr->flowTable_sending_needed
#define PANsink_mac_address     		pr_state_ptr->PANsink_mac_address
#define NDP_is_required         		pr_state_ptr->NDP_is_required
#define NDP_beacon_seq_num      		pr_state_ptr->NDP_beacon_seq_num
#define Neighbour_list          		pr_state_ptr->Neighbour_list
#define No_of_neighbours        		pr_state_ptr->No_of_neighbours
#define tdp_id                  		pr_state_ptr->tdp_id
#define tdp_decision            		pr_state_ptr->tdp_decision
#define tdp_code                		pr_state_ptr->tdp_code
#define startTimeSoBo_list_ptr  		pr_state_ptr->startTimeSoBo_list_ptr
#define flowEntriesP            		pr_state_ptr->flowEntriesP
#define grandfatherID           		pr_state_ptr->grandfatherID
#define Run1time                		pr_state_ptr->Run1time

/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	SDNC_MAC_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((SDNC_MAC_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


/* Function Block */

#if !defined (VOSD_NO_FIN)
enum { _op_block_origin = __LINE__ + 2};
#endif

 
/**********************************************************************************
                           GNU General Public License  

IEEE 802.15.4/ZigBee OPNET Simulation Model is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later version.
 
IEEE 802.15.4/ZigBee OPNET Simulation Model is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with IEEE 802.15.4/ZigBee OPNET Simulation Model; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 

*********************************************************************************/
/*------------------------------------------------------------------------------
 * Function:	enableNode()
 *
 * Description:	 This function enable end device to join network
 * No parameters
 *-----------------------------------------------------------------------------*/
static void enableNode() {
 	int getVenable;
	char err_str[100];
	self_id = op_id_self(); 
	parent_id = op_topo_parent(self_id);
	grandfatherID = op_topo_parent(parent_id);
	getVenable = op_id_from_name (grandfatherID, OPC_OBJTYPE_NODE_FIX, "R9") ;
	op_ima_obj_attr_set (getVenable, "condition", 1 );// make it active
	sprintf (err_str, "Active R9 hbi [id=%d (SDNC#%d)] t=%f  This SDNC = ", getVenable, my_address, op_sim_time());
	op_sim_message ("results=", err_str);
 
}

/*------------------------------------------------------------------------------
 * Function:	DisableNode()
 *
 * Description:	 This function Disable end device to join network
 * No parameters
 *-----------------------------------------------------------------------------*/
static void DisableNode() {
 	int getVenable;
	char err_str[100];
	self_id = op_id_self(); 
	parent_id = op_topo_parent(self_id);
	grandfatherID = op_topo_parent(parent_id);
	getVenable = op_id_from_name (grandfatherID, OPC_OBJTYPE_NODE_FIX, "R9") ;
	op_ima_obj_attr_set (getVenable, "condition", 0 );// make it active
	sprintf (err_str, "Active R9 hbi [id=%d (SDNC#%d)] t=%f  This SDNC = ", getVenable, my_address, op_sim_time());
	op_sim_message ("results=", err_str);
 
}
 
//////////////////////////////////////////////////////
/*------------------------------------------------------------------------------
 * Function:	sdn_wsan_send_ndp_beacon_frame()
 *
 * Description:	each node receiving a beacon with a ndp flag on should send a ndp beacon  
 *				frame to collect neighbour addresses by each node receiving this beacon
 *				ndp beacon is the same as normal beacon except it has no network parameters
 *				any kind only adresses and beacon type of ndp beacon 
 * 				it has nothing to with timings and similar things
 * No parameters
 *-----------------------------------------------------------------------------*/

static void sdn_wsan_send_ndp_beacon_frame() {

//declare things needed here
Packet* beacon_MSDU;
Packet* beacon_MPDU;
//Packet* beacon_PPDU;
int packet_size;


FIN(sdn_wsan_send_ndp_beacon_frame);


/* create a beacon payload (MSDU) */
	beacon_MSDU = op_pk_create_fmt ("wpan_beacon_MSDU_format");

/* create MAC frame (MPDU) with no need to encapsulate MSDU,,, No!!! it will encapsulate because it will read just before transmition :( */
	beacon_MPDU = op_pk_create_fmt("wpan_frame_MPDU_format");

/* the sequence number of ndp beacon will be the same as the sequence number of the original initiator beacon */
	
	op_pk_nfd_set (beacon_MPDU, "Frame Type", NDP_BEACON_FRAME_TYPE);
	op_pk_nfd_set (beacon_MPDU, "Sequence Number", NDP_beacon_seq_num); 
	op_pk_nfd_set (beacon_MPDU, "Source PAN Identifier", node_attributes.pan_id);
	op_pk_nfd_set (beacon_MPDU, "Destination PAN Identifier", node_attributes.pan_id);
	op_pk_nfd_set (beacon_MPDU, "Source Address", my_address);
	op_pk_nfd_set (beacon_MPDU, "Destination Address", BROADCAST_ADDRESS);
	op_pk_nfd_set (beacon_MPDU, "Ack Request", 0);

	op_pk_nfd_set_pkt (beacon_MPDU, "MSDU", beacon_MSDU); // wrap beacon payload (MSDU) in MAC Frame (MPDU) though its empty

if (enable_log) {
		fprintf (log_file, "t=%f  -> NDP Beacon Frame Transmission (PAN ID = %d). \n",op_sim_time(), node_attributes.pan_id);
		//printf (" [%s (#%d)] t=%f  -> NDP Beacon Frame Transmission (PAN ID = %d). \n",device_name, my_address, op_sim_time(), node_attributes.pan_id);
		fprintf (log_file, "\n");
	//	printf ("\n");
	}

/* create PHY frame (PPDU) that encapsulates beacon MAC frame (MPDU) */
//	beacon_PPDU = op_pk_create_fmt("wpan_frame_PPDU_format");
	
/* wrap beacon MAC frame (MPDU) in PHY frame (PPDU) */
//	op_pk_nfd_set_pkt (beacon_PPDU, "PSDU", beacon_MPDU); 
//	op_pk_nfd_set (beacon_PPDU, "Frame Length", ((double) op_pk_total_size_get(beacon_MPDU))/8); //[bytes]
	
//	wpan_battery_update_tx ((double)op_pk_total_size_get(beacon_PPDU));
	packet_size = op_pk_total_size_get (beacon_MPDU);
	/* best-effort traffic transmition */
	
		/* put it into the queue waiting for transmission */
		if (op_subq_pk_insert(SUBQ_CAP, beacon_MPDU, OPC_QPOS_TAIL) == OPC_QINS_OK) {
			if (enable_log) {
				fprintf (log_file,"t=%f  -> Enqueuing of best-effort (CAP) NDP Beacon packet (MPDU [size: %d bits]) [SEQ = %d, ACK = %s] \n\t + available CAP subqueue capacity: %.0f bits \n\n", op_sim_time(), packet_size, NDP_beacon_seq_num, boolean2string(OPC_FALSE), op_subq_stat (SUBQ_CAP, OPC_QSTAT_FREE_BITSIZE));
			//	printf (" [%s (#%d)] t=%f  -> Enqueuing of best-effort (CAP) NDP Beacon packet (MPDU [size: %d bits]) [SEQ = %d, ACK = %s] \n\t + available CAP subqueue capacity: %.0f bits \n\n", device_name, my_address, op_sim_time(), packet_size, NDP_beacon_seq_num, boolean2string(OPC_FALSE), op_subq_stat (SUBQ_CAP, OPC_QSTAT_FREE_BITSIZE));
				#define btoa(x) ((x)?"true":"false")
			//	printf (" dest adress 'Broadcasting' sfparams active in %s sfparams active out %s acknowledge %s packet to send index %d \n\n", btoa(SF_params[SF_in].CAP_ACTIVE), btoa(SF_params[SF_out].CAP_ACTIVE), btoa(node_attributes.wait_ack), packet_to_send_index);
				}
			
			/* try to send the packet if CAP is activet or previous packet is not waiting for ACK */
			if (SF_params[SF_in].CAP_ACTIVE)
				op_intrpt_schedule_self (op_sim_time(), TRY_PACKET_TRANSMISSION_CODE);
			
		} else {
			if (enable_log) {
				fprintf (log_file,"t=%f  -> Best-effort (CAP) NDP BEACON packet  (MPDU [size: %d bits]) cannot be enqueuing  \n\t + CAP subqueue is full -> PACKET IS DROPPED !!!! \n\n", op_sim_time(), packet_size);
				//printf (" [%s (#%d)] t=%f  -> Best-effort (CAP) NDP BEACON packet (MPDU [size: %d bits]) cannot be enqueuing \n\t + CAP subqueue is full -> PACKET IS DROPPED !!!! \n\n", device_name, my_address, op_sim_time(), packet_size);
				
				queue_status ();
			}
		
			/* destroy the packet */
			op_pk_destroy (beacon_MPDU);
		}
	
	
	
	//op_pk_send (beacon_PPDU, STRM_FROM_MAC_TO_RADIO);

FOUT;

}

/*--------------------------------------------------------------------------------
 * Function:	wpan_mac_init
 *
 * Description:	- initialize the process
 *				- read the attributes and set the global variables
 *
 * No parameters
 *--------------------------------------------------------------------------------*/

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

static void* flowTables_nested_list_mem_copy_create (void* list_ptr, unsigned int size)
{
	PrgT_List* list_copy;
	void* from_list_ptr;
	flow_table* flow_table_ptr;
	int i;	
	
	/* allocate an empty list */
	list_copy = op_prg_list_create ();
	
	for (i=0; i<op_prg_list_size ((PrgT_List* ) list_ptr); i++) {
		from_list_ptr = op_prg_list_access ((PrgT_List* ) list_ptr, i);
		
		/* memory allocation */
		flow_table_ptr	= (flow_table *) op_prg_mem_alloc (sizeof (flow_table)); 
		
		flow_table_ptr = (flow_table *) from_list_ptr;
		flow_table_ptr->node_address = ((flow_table *) from_list_ptr)->node_address;
		flow_table_ptr->src = ((flow_table *) from_list_ptr)->src;
		flow_table_ptr->act = ((flow_table *) from_list_ptr)->act;
		flow_table_ptr->dest = ((flow_table *) from_list_ptr)->dest;
		
		op_prg_list_insert (list_copy, flow_table_ptr, OPC_LISTPOS_TAIL);
	
	
		}	
	
	return (void* ) list_copy;
}

 

static void wpan_mac_init() {

	Objid csma_attributes_comp_id;
	Objid csma_attributes_id;	
	Objid mac_attributes_comp_id;
	Objid mac_attributes_id;
	Objid gts_attributes_comp_id;
	Objid gts_attributes_id;
	Objid gts_dir_attributes_comp_id;
	Objid queue_objid;	
	Objid subq_objid;
	Objid rcvr_objid;
	double rx_power_threshold_dbm;
	int i;
	/* NDP flag setting */
	NDP_is_required = OPC_TRUE;
	tdp_decision = OPC_FALSE;
	tdp_code = -1;
	Run1time = 0;
	/* Stack tracing enrty point */
	FIN(wpan_mac_init);	
	
	/* Get module Objids */
	/* obtain self object ID of the surrounding processor or queue */
	self_id   = op_id_self ();
	
	/* obtain object ID of the parent object (node) */
	parent_id = op_topo_parent (self_id);
	
	/*get Battery attribute ID*/	
	baterry_id = op_id_from_name (parent_id, OPC_OBJTYPE_PROC, "battery");
	tdp_id = op_id_from_name (parent_id, OPC_OBJTYPE_PROC, "Topology Discovery");
	//cout<<"parent_id="<<parent_id<<endl;
	//cout<<"baterry_id="<<baterry_id<<endl;
	
	if (baterry_id == OPC_NIL) 
		op_sim_end("Canno find the 'battery' ID.","Check if the name of battery modele is 'baterry'.","","");
	
	
	/* get the value to check if this node is PAN coordinator, Router, or End node */
	op_ima_obj_attr_get_int32 (parent_id, "Device Mode", &device_mode);
	
	/* get the name of the node */
	op_ima_obj_attr_get (parent_id, "name", device_name);
	
	op_ima_obj_attr_get (parent_id, "Enable Logging", &enable_log);
	
	/* get the MAC address of the node */
	op_ima_obj_attr_get (parent_id, "MAC Address", &my_address);
	
	/* get address of my parent */
	op_ima_obj_attr_get (parent_id, "Parent Address", &my_parent_address);
	
	//power threshold setting
	rcvr_objid = op_id_from_name (parent_id, OPC_OBJTYPE_RARX, "wireless_rx"); 
	op_ima_obj_attr_get (self_id, "power threshold", &rx_power_threshold_dbm);
	
	rx_power_threshold = pow (10.0, rx_power_threshold_dbm / 10.0) / 1000.0;
	
	op_ima_obj_state_set (rcvr_objid, &rx_power_threshold);
	
	/* get the geographic position of the node */
	op_ima_obj_attr_get (parent_id, "x position", &node_attributes.x);
	op_ima_obj_attr_get (parent_id, "y position", &node_attributes.y);
	op_ima_obj_attr_get (parent_id, "altitude", &node_attributes.altitude);	
	
	/* get GTS parameters */
	op_ima_obj_attr_get (self_id, "GTS Parameters", &gts_attributes_id);
	gts_attributes_comp_id = op_topo_child (gts_attributes_id, OPC_OBJTYPE_GENERIC, 0);
	
	op_ima_obj_attr_get (gts_attributes_comp_id, "GTS Permit", &node_attributes.macGTSPermit); /* TRUE if I (PAN Coordinator or Roouter) accept GTS request. FALSE otherwise. */
	op_ima_obj_attr_get (gts_attributes_comp_id, "Buffer Capacity", &cfp_queue_capacity);	/* get capacity of CFP buffer (for real-time traffic)  */
	
	op_ima_obj_attr_get (gts_attributes_comp_id, "Receive GTS", &gts_attributes_id);
	gts_dir_attributes_comp_id = op_topo_child (gts_attributes_id, OPC_OBJTYPE_GENERIC, 0);
	
	op_ima_obj_attr_get (gts_dir_attributes_comp_id, "Start Time", &GTS_params[GTS_DIRECTION_RECEIVE].start_time);
	op_ima_obj_attr_get (gts_dir_attributes_comp_id, "Stop Time", &GTS_params[GTS_DIRECTION_RECEIVE].stop_time);
	op_ima_obj_attr_get (gts_dir_attributes_comp_id, "Length", &GTS_params[GTS_DIRECTION_RECEIVE].length);
	GTS_params[GTS_DIRECTION_RECEIVE].direction = GTS_DIRECTION_RECEIVE;
	
	op_ima_obj_attr_get (gts_attributes_comp_id, "Transmit GTS", &gts_attributes_id);
	gts_dir_attributes_comp_id = op_topo_child (gts_attributes_id, OPC_OBJTYPE_GENERIC, 0);	
	
	op_ima_obj_attr_get (gts_dir_attributes_comp_id, "Start Time", &GTS_params[GTS_DIRECTION_TRANSMIT].start_time);
	op_ima_obj_attr_get (gts_dir_attributes_comp_id, "Stop Time", &GTS_params[GTS_DIRECTION_TRANSMIT].stop_time);
	op_ima_obj_attr_get (gts_dir_attributes_comp_id, "Length", &GTS_params[GTS_DIRECTION_TRANSMIT].length);
	GTS_params[GTS_DIRECTION_TRANSMIT].direction = GTS_DIRECTION_TRANSMIT;
	
	/* inicialization of the GTS variables */	
	for (i=0;i<2;i++) {
		GTS_params[i].retries_nbr = 0;
		GTS_params[i].start_slot = 0;
		GTS_params[i].is_waiting_GTS_confirm = OPC_FALSE;
		GTS_params[i].wait_GTS_confirm_count = 0;
		GTS_params[i].is_deallocation_req_sent = OPC_FALSE;
	}
	
	waiting_for_first_beacon = OPC_FALSE;
	
	packet_to_send_index = NOT_DEFINED;
	
	node_attributes.my_parent_GTSPermit = NOT_DEFINED;
	
	collision_occurred = OPC_FALSE;
	
	flowTable_sending_needed = 0;
	
	/* get MAC parameters */
	op_ima_obj_attr_get (self_id, "MAC Parameters", &mac_attributes_id);	
	mac_attributes_comp_id = op_topo_child (mac_attributes_id, OPC_OBJTYPE_GENERIC, 0);	

	/* get the Superframe settings for PAN Coordinator and Router*/
	if (!IAM_END_DEVICE) {
		op_ima_obj_attr_get (mac_attributes_comp_id, "Beacon Order", &node_attributes.beacon_order);
		op_ima_obj_attr_get (mac_attributes_comp_id, "Superframe Order", &node_attributes.superframe_order);		
		
		if ((node_attributes.superframe_order > node_attributes.beacon_order) && (node_attributes.superframe_order != 15)) {			
			//printf (" [%s (#%d)] Superframe Order(%d) > Beacon Order(%d) (0<=SO<=B0<=14). \n", device_name, my_address, node_attributes.superframe_order, node_attributes.beacon_order);
			op_sim_end ("Superframe Order > Beacon Order", "See simulation console output for details."," ", " ");
		}
	}
	else { /* if the node is END DEVICE, use these value as default. */
		node_attributes.beacon_order = 15;
		node_attributes.superframe_order = 15;
	}
	
	op_ima_obj_attr_get (mac_attributes_comp_id, "PAN ID", &node_attributes.pan_id);	
	op_ima_obj_attr_get (mac_attributes_comp_id, "StartTime", &node_attributes.startTime);
	
	/* get capacity of CAP buffer (for best-effort traffic) */
	op_ima_obj_attr_get (mac_attributes_comp_id, "Buffer Capacity", &cap_queue_capacity);
	
	/* get the CSMA-CA settings */
	op_ima_obj_attr_get (mac_attributes_comp_id, "CSMA Parameters", &csma_attributes_id);
	csma_attributes_comp_id = op_topo_child (csma_attributes_id, OPC_OBJTYPE_GENERIC, 0);
	
	op_ima_obj_attr_get (csma_attributes_comp_id, "Maximum Backoff Number", &csma_params.macMaxCSMABackoffs);
	op_ima_obj_attr_get (csma_attributes_comp_id, "Minimum Backoff Exponent", &csma_params.macMinBE);	
	op_ima_obj_attr_get (csma_attributes_comp_id, "Battery Life Extension", &csma_params.macBattLifeExt);	
	
	/* get the subqueues object ID */
	op_ima_obj_attr_get (self_id, "subqueue", &queue_objid);
	
	/* obtain object ID of the SUBQ_CAP subqueue */
	subq_objid = op_topo_child (queue_objid, OPC_OBJMTYPE_ALL, SUBQ_CAP);
	
	/* set the bit capacity of the appropriate subqueue */
	op_ima_obj_attr_set (subq_objid, "bit capacity", (double) cap_queue_capacity);	
	
	/* obtain object ID of the SUBQ_GTS subqueue */
	subq_objid = op_topo_child (queue_objid, OPC_OBJMTYPE_ALL, SUBQ_GTS);
	
	/* set the bit capacity of the appropriate subqueue */
	op_ima_obj_attr_set (subq_objid, "bit capacity", (double) cfp_queue_capacity);	
	
	node_attributes.GTS_list = op_prg_list_create();	/* allocate an empty list */
	//GTS_list_temp = op_prg_list_create();
	
	/* memory allocation for GTS lists */
//	if (node_attributes.macGTSPermit && (!IAM_END_DEVICE)) {
//		node_attributes.GTS_list = op_prg_list_create();	/* allocate an empty list */
//		GTS_list_temp = op_prg_list_create();
//	}

	/* incoming supeframe, i.e. superframe of my parent node - I am receiving beacon from my parent */
	/* outgoing superframe, i.e. my own superframe - I am sending beacon to my child nodes */
	for (i=0;i<2;i++) {
		SF_params[i].current_slot = 0;
		SF_params[i].Final_CAP_Slot = TIME_SLOT_INDEX_MAX; /* no CFP (GTSs) */
		SF_params[i].Final_CAP_Slot_next = TIME_SLOT_INDEX_MAX;
		SF_params[i].CAP_ACTIVE = OPC_FALSE;
		SF_params[i].CFP_ACTIVE = OPC_FALSE;
		SF_params[i].SLEEP = OPC_TRUE;
	}	
	
	SF_params[SF_out].SD = aBaseSuperframeDuration * exponent(2, node_attributes.superframe_order);
	SF_params[SF_out].BI = aBaseSuperframeDuration * exponent(2, node_attributes.beacon_order);
	SF_params[SF_out].slot_duration = aBaseSlotDuration * exponent(2, node_attributes.superframe_order);
	SF_params[SF_out].sleep_period = SF_params[SF_out].BI - SF_params[SF_out].SD;
	
	SF_ptr = OPC_NIL; /* pointer to the current Superframe structure (incoming or outgoing ) */
	
	GTS_ptr = OPC_NIL;
	GTS_list_ptr = OPC_NIL;
	

	/* get the ACK settings */
	op_ima_obj_attr_get (mac_attributes_comp_id, "Number of Retransmissions", &node_attributes.macMaxFrameRetries);
	
	node_attributes.wait_ack = OPC_FALSE;
	node_attributes.wait_ack_seq_num = 0;	

	/* CSMA initialization	*/
	for (i=0;i<2;i++) {
		SF_params[i].csma.NB = 0;
		SF_params[i].csma.CW = 2;
		SF_params[i].csma.BE = csma_params.macMinBE;
		SF_params[i].csma.retries_nbr = 0;
		SF_params[i].csma.CCA_CHANNEL_IDLE = OPC_TRUE;
		SF_params[i].csma.backoff_timer = 0;
		SF_params[i].csma.RESUME_BACKOFF_TIMER = OPC_FALSE;
		SF_params[i].csma.CCA_DEFERRED = OPC_FALSE;
		
		if (csma_params.macBattLifeExt == OPC_TRUE)
			SF_params[i].csma.BE = min_int(2, csma_params.macMinBE);
	}	
	
	if (INITIANLLY_EMPTY_GTS_BUFFER) {
		/* start the GTS transmission directly - initially empty buffer*/
		start_GTS_transmission = OPC_TRUE;
	} else {
		/* start the GTS transmission after the buffer will be full - initially full buffer */
		start_GTS_transmission = OPC_FALSE;	 
	}
	
	/* register the statistics */
	statistic_vector.failure = op_stat_reg ("CSMA.Transmission Failure", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	statistic_vector.success = op_stat_reg ("CSMA.Transmission Success", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	statistic_vector.ch_busy_cca1 = op_stat_reg ("CSMA.|CCA1| Busy Channel", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	statistic_vector.ch_busy_cca2 = op_stat_reg ("CSMA.|CCA2| Busy Channel", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	statistic_vector.ch_idle_cca1 = op_stat_reg ("CSMA.|CCA1| Idle Channel", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	statistic_vector.ch_idle_cca2 = op_stat_reg ("CSMA.|CCA2| Idle Channel", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	statistic_vector.deferred_cca = op_stat_reg ("CSMA.CCA Deference", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	statistic_vector.dropped_packets = op_stat_reg ("CSMA.Dropped Packets", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	statistic_vector.retransmission_attempts = op_stat_reg ("CSMA.Retransmission Attempts", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	statistic_vector.success_ack_packets = op_stat_reg ("CSMA.Successfully Acknowledged Packets", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	statistic_vector.backoff_delay = op_stat_reg ("CSMA.Backoff Delay (sec)", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	statistic_vector.mac_delay = op_stat_reg ("CSMA.Medium Access Delay (sec)", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	statistic_vector.backoff_units = op_stat_reg ("CSMA.Backoff Delay (Backoff Period Units)", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	statistic_vector.sent_pkt = op_stat_reg ("Network Output Load (bits/sec)", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);	
	statistic_vector.deferred_cca_backoff = op_stat_reg ("CSMA.CCA and Backoff Deference", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);	
	
	/* register the GLOBAL statistics */	
	statistic_global_vector.dropped_packets = op_stat_reg ("CSMA.Dropped Packets", OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
	statistic_global_vector.mac_delay = op_stat_reg ("CSMA.Medium Access Delay (sec)", OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
	statistic_global_vector.success_ack_packets = op_stat_reg ("CSMA.Successfully Acknowledged Packets", OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
	statistic_global_vector.sent_pkt = op_stat_reg ("Network Output Load (bits/sec)", OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
	statistic_global_vector.rnd_generator = op_stat_reg ("Random Number Generator", OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
	
	
	/* GTS statistics */
	gts_statistic_vector.sent_frames = op_stat_reg ("GTS.Network Output Load (bits/sec)", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL); 
	gts_statistic_vector.sent_framesG = op_stat_reg ("GTS.Network Output Load (bits/sec)", OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL); 
	gts_statistic_vector.frame_delay = op_stat_reg ("GTS.Packets Medium Access Delay (sec)", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	gts_statistic_vector.wasted_bandwidth_pc = op_stat_reg ("GTS.Wasted bandwith due to IFS (%)", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	gts_statistic_vector.buffer_saturation_pc = op_stat_reg ("GTS.Buffer saturation (%)", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	gts_statistic_vector.throughput_pc = op_stat_reg ("GTS.Throughput (%)", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	
	gts_statistics.band_wast_sum = 0.0;
	gts_statistics.dropped_frames = 0;	//  number of droped GTS frames	
	gts_statistics.dropped_bits = 0.0;	// MPDU bits	
	gts_statistics.frame_delay_sum = 0.0;
	gts_statistics.frame_delay_max = 0.0;
	gts_statistics.frame_delay_min = LONG_MAX;
	gts_statistics.sent_frames = 0;	// number of dispatched GTS frames
	gts_statistics.sent_bits = 0.0;	// PPDU bits
	gts_statistics.gts_periods_count = 0;
	gts_statistics.stored_frames = 0;	// number of generated GTS frames
	gts_statistics.stored_bits = 0.0;	// MSDU bits
	gts_statistics.received_frames = 0; // number of received GTS frames
	gts_statistics.received_bits = 0.0; // MSDU bits

	wpan_log_file_init ();
	
	/* Stack tracing exit point */
	FOUT;
}


/*--------------------------------------------------------------------------------
 * Function:	 wpan_log_file_init
 *
 * Description:	log file init
 *				
 * No parameters
 *--------------------------------------------------------------------------------*/

static void  wpan_log_file_init() {
 
	char directory_path_name[120];
	char log_name[132];
	
	/* Stack tracing enrty point */
	FIN(wpan_log_file_init);

	op_ima_obj_attr_get (parent_id, "Log File Directory", directory_path_name);
	
	/* verification if the directory_path_name is a valid directory */
	if (prg_path_name_is_dir (directory_path_name) == PrgC_Path_Name_Is_Not_Dir) {
		char msg[128];		
		sprintf (msg, " \"%s\" is not valid directory name. The output will not be logged.\n", directory_path_name); 
		
		/* Display an appropriate warning.	*/
		op_sim_error(OPC_SIM_ERROR_WARNING, "Warning from wpan_mac process", msg);
			
		enable_log = OPC_FALSE;
	}	
	
	if (enable_log) {
		sprintf (log_name, "%s%s.ak", directory_path_name, device_name); 
		printf (" [%s (#%d)] Log file name: %s \n\n", device_name, my_address, log_name);
		log_file = fopen(log_name,"w");
	}
	
	/* Stack tracing exit point */
	FOUT;
}


/*--------------------------------------------------------------------------------*
 * Function:	wpan_print_parameters
 *
 * Description:	print all the mac attributes of the current node 
 *
 * No parameters
 *--------------------------------------------------------------------------------*/

static void wpan_print_parameters () {

	int i;

	/* Stack tracing enrty point */
	FIN(wpan_print_parameters);
		
	if (enable_log) {	
		fprintf (log_file, "|-----------------------------------------------------------------------------|\n");
		fprintf (log_file, "| NODE NAME          : %s \n", device_name);
		
		switch (device_mode) {
			case PAN_COORDINATOR:
			{
				fprintf (log_file, "| DEVICE MODE        : PAN Coordinator \n");
				break;
			};
			case ROUTER:
			{
				fprintf (log_file, "| DEVICE MODE        : Router \n");
				break;
			};
			default:
			{
				fprintf (log_file, "| DEVICE MODE        : End Device \n");
			};				
		}		
		
		fprintf (log_file, "| MAC ADDRESS        : %d \n",my_address);
		
		if (IAM_ROUTER)
			fprintf (log_file, "| StartTime          : %f sec \n", node_attributes.startTime);
	
		fprintf (log_file, "| NUMBER OF RETRANSMISSIONS : %d \n\n",node_attributes.macMaxFrameRetries);
	
		fprintf (log_file, "| GEOGRAPHIC POSITION \n");
		fprintf (log_file, "|   POSITION x       : %f \n",node_attributes.x);
		fprintf (log_file, "|   POSITION y       : %f \n",node_attributes.y);
		fprintf (log_file, "|   ALTITUTE z       : %f \n\n",node_attributes.altitude);		
	
		if (IAM_PAN_COORDINATOR) {
			fprintf (log_file, "| SUPERFRAME PARAMETERS \n");
			fprintf (log_file, "|   BEACON ORDER             : %d \n",node_attributes.beacon_order);
			fprintf (log_file, "|   SUPERFRAME ORDER         : %d \n",node_attributes.superframe_order);
			fprintf (log_file, "|   PAN ID                   : %d \n\n",node_attributes.pan_id);
			fprintf (log_file, "| GTS Permit : %s \n", boolean2string(node_attributes.macGTSPermit));
		} else {
			if (IAM_ROUTER) {
				fprintf (log_file, "| GTS Permit : %s \n", boolean2string(node_attributes.macGTSPermit));
			}
		}
		
		for (i=0;i<2;i++) {
			if (GTS_params[i].start_time != START_DISABLED) {
				if (i == GTS_DIRECTION_TRANSMIT)
					fprintf (log_file, "| GTS TRANSMIT PARAMETERS \n");
				else
					fprintf (log_file, "| GTS RECEIVE PARAMETERS \n");	
		
				fprintf (log_file, "|   START TIME : %f \n", GTS_params[i].start_time);
			
				if (GTS_params[i].stop_time == END_OF_SIMULATION)
					fprintf (log_file, "|   STOP TIME  : End of Simulation \n");
				else
					fprintf (log_file, "|   STOP TIME  : %f \n", GTS_params[i].stop_time);
			
				fprintf (log_file, "|   DIRECTION  : %d \n", GTS_params[i].direction);
				fprintf (log_file, "|   LENGTH     : %d slots\n", GTS_params[i].length);
				
				if (INITIANLLY_EMPTY_GTS_BUFFER) 
					fprintf (log_file, "|   (initially empty buffer)\n\n");
				else
					fprintf (log_file, "|   (initially full buffer)\n\n");	
			}
		}
		
	
		fprintf (log_file, "| CSMA PARAMETERS \n");	
		fprintf (log_file, "|   MAXIMUM BACKOFF NUMBER    : %d \n",csma_params.macMaxCSMABackoffs);
		fprintf (log_file, "|   MAXIMUM BACKOFF EXPONENT  : %d \n",csma_params.macMinBE);
		fprintf (log_file, "|   BATTERY LIFE EXTENSION    : %s \n",boolean2string(csma_params.macBattLifeExt));
		fprintf (log_file, "|-----------------------------------------------------------------------------|\n\n");
		/*
		
		printf( "|-----------------------------------------------------------------------------|\n");
		printf( "| NODE NAME          : %s \n",device_name);
		
		switch (device_mode) {
			case PAN_COORDINATOR:
			{
				printf ("| DEVICE MODE        : Pan Coordinator \n");
				break;
			};
			case ROUTER:
			{
				printf ("| DEVICE MODE        : Router \n");
				break;
			};
			default:
			{
				printf ("| DEVICE MODE        : End Device \n");
			};				
			}	
				
		printf( "| MAC ADDRESS        : %d \n",my_address);
		if (IAM_ROUTER)
			printf ("| StartTime          : %f sec \n", node_attributes.startTime);
	
		printf ("| NUMBER OF RETRANSMISSIONS : %d \n\n",node_attributes.macMaxFrameRetries);
	
		printf ( "| GEOGRAPHIC POSITION \n");
		printf ( "|   POSITION x      : %f \n",node_attributes.x);
		printf ( "|   POSITION y      : %f \n",node_attributes.y);
		printf ( "|   ALTITUTE z      : %f \n\n",node_attributes.altitude);
	
		if (IAM_PAN_COORDINATOR) {
			printf ( "| SUPERFRAME PARAMETERS \n");
			printf ( "|   BEACON ORDER              : %d \n",node_attributes.beacon_order);
			printf ( "|   SUPERFRAME ORDER          : %d \n",node_attributes.superframe_order);
			printf ( "|   PAN ID                    : %d \n\n",node_attributes.pan_id);
			printf ( "| GTS Permit : %s \n", boolean2string(node_attributes.macGTSPermit));
		} else {
			if (IAM_ROUTER) {
				printf ( "| GTS Permit : %s \n", boolean2string(node_attributes.macGTSPermit));
			}
		}
		
		for (i=0;i<2;i++) {
			if (GTS_params[i].start_time != START_DISABLED) {
				if (i == GTS_DIRECTION_TRANSMIT)
					printf ("| GTS TRANSMIT PARAMETERS \n");
				else
					printf ("| GTS RECEIVE PARAMETERS \n");	
		
				printf ("|   START TIME : %f \n", GTS_params[i].start_time);
			
				if (GTS_params[i].stop_time == END_OF_SIMULATION)
					printf ("|   STOP TIME  : End of Simulation \n");
				else
					printf ("|   STOP TIME  : %f \n", GTS_params[i].stop_time);
			
				printf ("|   DIRECTION  : %d \n", GTS_params[i].direction);
				printf ("|   LENGTH     : %d slots\n", GTS_params[i].length);
				
				if (INITIANLLY_EMPTY_GTS_BUFFER) 
					printf ("|   (initially empty buffer)\n\n");
				else
					printf ("|   (initially full buffer)\n\n");	
			}
			}

		printf ( "| CSMA PARAMETERS \n");	
		printf ( "|   MAXIMUM BACKOFF NUMBER     : %d \n", csma_params.macMaxCSMABackoffs);
		printf ( "|   MAXIMUM BACKOFF EXPONENT   : %d \n", csma_params.macMinBE);
		printf ( "|   BATTERY LIFE EXTENSION     : %s \n", boolean2string(csma_params.macBattLifeExt));
		printf( "|-----------------------------------------------------------------------------|\n\n");
	*/
		}
	
	
	/* Stack tracing exit point */
	FOUT;
}


/*--------------------------------------------------------------------------------
 * Function:	wpan_parse_incoming_frame
 *
 * Description:	Parse the incoming packet and do the adequate processing.
 *
 * No parameters
 *--------------------------------------------------------------------------------*/

static void wpan_parse_incoming_frame() {

	//Ici* iciptr;
	Packet* frame_MPDU;
	Packet* rcv_frame;
	int intrpt_strm;
	int frame_type_fd;
	int source_address;
	int dest_address;
	int pan_id;
	int i;
	//int ack;
	//int traffic_type;
	double packet_size;
	
	double rnd;
		////////////////////
	int Current_beacon_seq_num;
	Boolean neighbour_available;
	/////////////////////
	
	/* stack tracing enrty point */
	FIN(wpan_parse_incoming_frame);
	
	/* get the packet from the input stream	*/
	intrpt_strm = op_intrpt_strm ();
	rcv_frame = op_pk_get (intrpt_strm);
 /*
	if (op_sim_time()>220 && Run1time == 0) 
	 
	{
		Run1time = 1;
		enableNode();	//hbi
	}
	 
	if (op_sim_time()>666 && op_sim_time()<668) 
	{
		 
		DisableNode();	//hbi
	}
	*/
	/* check from input stream the packet was received and do the right processing	*/		
	switch (intrpt_strm) {
	case STRM_FROM_TDP_TO_MAC:   /* INCOMING DECISION FROM TDP */
		{
			int node_no_with_children = 0;
			//printf("packet from tdp to mac received in SDNC mac\n");
			fprintf (log_file, "t=%f  -> packet from tdp to mac received in SDNC mac . \n",op_sim_time());
			startTimeSoBo_list_ptr = new startTimeSoBo_list;
			int nodes_SI_sum, needed_BI, needed_BO;
			tdp_decision = OPC_TRUE;
			op_pk_nfd_get(rcv_frame,"TD_code", &tdp_code);
			op_pk_nfd_get_ptr(rcv_frame,"startTimeSoBo_list", (void**) &startTimeSoBo_list_ptr);
			fprintf (log_file, "TD_code = %d\n\n", tdp_code);
			
			for (int i = 0; i < startTimeSoBo_list_ptr->nodes_no_to_be_set; i++)
				{
					//printf ("node address = %d\n", startTimeSoBo_list_ptr->node_address[i]);
					//printf ("start time = %f\n", startTimeSoBo_list_ptr->startTime[i]);
				//	printf ("node so = %d\n", startTimeSoBo_list_ptr->so[i]);
				//	printf ("node bo = %d\n", startTimeSoBo_list_ptr->bo[i]);
					fprintf (log_file, "node address = %d\n", startTimeSoBo_list_ptr->node_address[i]);
					fprintf (log_file, "start time = %f\n", startTimeSoBo_list_ptr->startTime[i]);
					fprintf (log_file, "node so = %d\n", startTimeSoBo_list_ptr->so[i]);
					fprintf (log_file, "node bo = %d\n", startTimeSoBo_list_ptr->bo[i]);
					// if it has a value it should be more than 0 if its not it should be not defined
					// which is -1 that mean it has no children and its not counting here
					if (startTimeSoBo_list_ptr->so[i] > 0 && startTimeSoBo_list_ptr->bo[i] > 0)
 						node_no_with_children++;
				}
			
			//printf ("nodes with no children = %d\n", node_no_with_children);
			fprintf (log_file, "nodes with no children = %d\n", node_no_with_children);
			// setting of so bo of the sdnc
			node_attributes.superframe_order = 1; // SO is fixed to 1 and BO will be changed according to number of nieghbours
			if (tdp_code == 0) // default tdp settings means so bo for all nodes 1 1 
				nodes_SI_sum = No_of_neighbours * aBaseSuperframeDuration * exponent(2, 1); // 1 is intended so of the nodes
			else 
				nodes_SI_sum = node_no_with_children * aBaseSuperframeDuration * exponent(2, 1); // 1 is intended so of the nodes
			
			needed_BI = nodes_SI_sum + (aBaseSuperframeDuration * exponent(2, node_attributes.superframe_order));
			needed_BO = 2;
			
			while ((aBaseSuperframeDuration * exponent(2, needed_BO)) < needed_BI)
				{
					needed_BO++;
				}
			node_attributes.beacon_order =  needed_BO;
						
			// SF_inParametersNotSet = OPC_FALSE; // this variable is not needed though it might be needed in end devices
			// changes for dynamic so bo and start time, note taken
			SF_params[SF_out].SD = aBaseSuperframeDuration * exponent(2, node_attributes.superframe_order);
			SF_params[SF_out].slot_duration = aBaseSlotDuration * exponent(2, node_attributes.superframe_order);
			SF_params[SF_out].BI = aBaseSuperframeDuration * exponent(2, node_attributes.beacon_order);
			SF_params[SF_out].sleep_period = SF_params[SF_out].BI - SF_params[SF_out].SD;
			// how to send so bo settings to 
			break;
		};
	
	case STRM_FROM_NeIntel_TO_MAC:	/* INCOMMING DECISION FROM THE THE NETWORK INTELLIGENCE PROCESS */
		{			
//		PrgT_List* gts_characteristics;
		//PrgT_List* flowTables;
//		flow_table* from_FT_ptr;
//		flow_table* to_FT_ptr;
//		int i; 
		
		
		 int flowEntriesNo;
		/* printing of incoming flow tables class */
		 op_pk_nfd_access (rcv_frame, "FlowTables", &flowEntriesP);
		 op_pk_nfd_access (rcv_frame, "flow_entries_no", &flowEntriesNo);
		 printf("MAC Module: Incoming flow table entries (%d entries) from NeIntel Process\n", flowEntriesNo);
		 flowEntriesP.resize(flowEntriesNo);
		 printf("flowEntriesP.begin() = %d\n", flowEntriesP.begin());
		 printf("flowEntriesP.end() = %d\n", flowEntriesP.end());
		 printf("flowEntriesP.size() = %d\n", flowEntriesP.size());
		 
		int infinitLoop = 0;
		for (int i = 0; i < flowEntriesNo; i++)
			{
				infinitLoop++;
				printf("%d. flow entries of node address: %d\n", i+1, flowEntriesP[i]->nodeAddress);
				printf("source address condition:    	  %d\n", flowEntriesP[i]->srcAdrMatchRule);
				//printf("comparator operation: %d\n", flowEntriesP[i]->op);
				printf("Action:                      	  %d\n", flowEntriesP[i]->action);
				printf("Next hop address:            	  %d\n", flowEntriesP[i]->NextHop);
				printf("ApplicationID:					  %d\n\n", flowEntriesP[i]->ApplicationID);
				if (infinitLoop > 20)
					op_sim_end("infinit loop","foor loop of the linked list went too far","","");
				flowTable_sending_needed = 3; // number of try to send flow table
				}
			 

		
		
		//op_pk_nfd_access (rcv_frame, "FlowTables", &flowTables);
		//op_pk_nfd_access (rcv_frame, "FlowTables", &Flow_Tables_list);
		//op_pk_nfd_access (rcv_frame, "GtsCharacteristics", &gts_characteristics);
		//op_pk_nfd_get (rcv_frame, "FlowTables", &flowTables);
		//op_pk_nfd_get (rcv_frame, "GtsCharacteristics", &gts_characteristics);
		//if (op_prg_list_size(flowTables) >0)	
		//	{
			
			//op_prg_list_transfer (flowTables, Flow_Tables_list, sizeof(flow_table));
		//	Flow_Tables_list = op_prg_list_create();
		//	op_prg_list_free(Flow_Tables_list);
		/*
			for (i = 0; i < op_prg_list_size(flowTables); i++)
				{
				from_FT_ptr = (flow_table*) op_prg_list_access(flowTables, i);
				to_FT_ptr = (flow_table*) op_prg_mem_alloc(sizeof(flow_table));
				to_FT_ptr->node_address = from_FT_ptr->node_address;
				to_FT_ptr->src = from_FT_ptr->src;
				to_FT_ptr->act = from_FT_ptr->act;
				to_FT_ptr->dest = from_FT_ptr->dest;
				op_prg_list_insert(Flow_Tables_list,to_FT_ptr, OPC_LISTPOS_TAIL);
				}
		*/
		
		//}
		//else 
		//	cout<<"**************** ERROR: no flow table from NeIntel *****************\n";
		//	fprintf (wpan_log, "**************** ERROR: no flow table from NeIntel *****************\n");
			
//		flow_table* flow_table_ptr;
//		node_gts_chrcts* node_gts_chrcts_ptr;
		

//		cout<<"**************** printing incoming packet from NeIntel alish do*****************\n";
//		cout<< "********************* flow table ***********************************************\n" ;
		
//		for (i=0; i<op_prg_list_size (Flow_Tables_list); i++) {
//			if ((flow_table_ptr = (flow_table *) op_prg_list_access (Flow_Tables_list, i)) != OPC_NIL)
//			{
//			cout<< "----------------------------- %d --------------------------------- \n"<<"i="<< i ;
//			cout<< "t = %f -> flow table of node of address %d \n"<< op_sim_time()<<"address "<< flow_table_ptr->node_address ;	
//			cout<< "t = %f -> source condition is %d \n"<< op_sim_time()<<"src "<< flow_table_ptr->src ;
//			cout<< "t = %f -> action is %d \n"<< op_sim_time()<<"act " <<flow_table_ptr->act ;
//			cout<< "t = %f -> destination is %d \n"<< op_sim_time()<<"dest "<< flow_table_ptr->dest ;
//			
//			}
//		}
//		cout<< "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n" ;
//		cout<< "&&&&&&&&&&&&&&&&&&&&&& gts characteristics list &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n" ;

//		for (i=0; i<op_prg_list_size (gts_characteristics); i++) {
//			if ((node_gts_chrcts_ptr = (node_gts_chrcts *) op_prg_list_access (gts_characteristics, i)) != OPC_NIL)
//			{
//			cout<< "----------------------------- %d --------------------------------- \n"<< i ;
//			cout<< "t = %f -> gts characteristics of node of address %d \n"<< op_sim_time()<<"node_address"<< node_gts_chrcts_ptr->node_address ;	
//			cout<< "t = %f -> gts type (0 allocation/ 1 deallocation) is %d \n"<< op_sim_time()<<"type"<< node_gts_chrcts_ptr->type ;
//			cout<< "t = %f -> gts direction (0 transmit/ 1 receive) is %d \n"<< op_sim_time()<<"direction"<< node_gts_chrcts_ptr->direction ;
//			cout<< "t = %f -> no. of gts slots is %d \n"<< op_sim_time()<<"length"<< node_gts_chrcts_ptr->length ;
//			
//	}
//		}
	
	
//	cout<< "********************************************************************************\n" ;
//	cout<< "********************************************************************************\n" ;
	
//	Final_CAP_slot_next = TIME_SLOT_INDEX_MAX;
//	Extract_NeIntel_decisions_gts_characteristics (gts_characteristics);
	
	
	
			break;
	};
 
		case STRM_FROM_RADIO_TO_MAC: /* PHY packet (PPDU) from radio receiver */
		{
			/* get MAC packet (MPDU=PSDU) from received PHY packet (PPDU) */
//		Packet* msduu; 
		//SimT_Packet* Beacon_Payload;	
		//cout<<"The Packet comes from RAdio to MAC "<<endl;
		
		op_pk_nfd_get_pkt (rcv_frame, "PSDU", &frame_MPDU);
			//op_pk_print(frame_MPDU);
//			printf("[%s (#%d)] t=%f ::STRM_FROM_RADIO_TO_MAC  print pk  \n",device_name, my_address, op_sim_time());
//			op_pk_print (frame_MPDU);
		
		op_pk_nfd_get (frame_MPDU, "Frame Type", &frame_type_fd);
		//////////////////// end rhbi
			/* acquire "Frame Type" field */
		
		//cout<<"inside the msdu Beacon Payload="<<endl;
		//op_pk_nfd_get (msduu, "Beacon Payload", &Beacon_Payload);
		//if(Beacon_Payload!=NULL)
			//cout<<"Beacon_Payload="<<Beacon_Payload<<endl;
		//op_pk_print (Beacon_Payload);
			/* update the battery */
			packet_size = (double) op_pk_total_size_get(rcv_frame);
			wpan_battery_update_rx (packet_size, frame_type_fd);
			
		
		//////////////////////////////////////////////////////////////////////////////
		/*
		
							double d; Packet* stts_pkt;
							stts_pkt = op_pk_create_fmt ("status_repo");
							op_pk_nfd_get (rcv_frame, "Source Address", &d);
							op_pk_nfd_set (stts_pkt, "RSSI", op_td_get_dbl (rcv_frame, OPC_TDA_RA_SNR));
							op_pk_nfd_set (stts_pkt, "RSSI_dst",  my_address);
							op_pk_nfd_set (stts_pkt, "RSSI_src", d);
							 cout<<"sending Rssi packet info from MAC to Status Collector"<<endl;
							 op_pk_print(stts_pkt);
							op_pk_send (stts_pkt,STRM_FROM_MAC_TO_STATUS_COL);
		*/
		///////////////////////////////////////////////////////////////////////////////
		
		/*Check "Frame Type" field*/	
			if (frame_type_fd == NDP_BEACON_FRAME_TYPE) {
			fprintf (log_file, "t=%f  -> ndp packet received by sdnc. \n",op_sim_time());
				op_pk_nfd_get (frame_MPDU, "Source Address", &source_address);
				if (!IAM_PAN_COORDINATOR) {
				
					op_pk_nfd_get (frame_MPDU, "Sequence Number", &Current_beacon_seq_num);		
					// check if i received this beacon before
					if (Current_beacon_seq_num != NDP_beacon_seq_num){
						//sdn_wsan_send_ndp_beacon_frame();
						NDP_beacon_seq_num = Current_beacon_seq_num;
						}
				
					if (IAM_ROUTER) {
					
					}
				
					} else { // if I am a pan coordinator
					
					
						}
				neighbour_available = OPC_FALSE;
					// take the beacon source address
					// check if its in neighbour list, if not add it to this list
				for (i=0; i<No_of_neighbours; i++){
					if (Neighbour_list[i][node_address_index] == source_address)
						neighbour_available = OPC_TRUE;
					}
					if (!neighbour_available){
						Neighbour_list[No_of_neighbours][node_address_index] = source_address;
						Neighbour_list[No_of_neighbours][node_rssi_index] = op_td_get_dbl (rcv_frame, OPC_TDA_RA_SNR);
						No_of_neighbours = No_of_neighbours + 1;
						if (enable_log) {
							fprintf (log_file, "t=%f  -> new neighbour added to list . \n",op_sim_time());
							//printf (" [%s (#%d)] t=%f  -> new neighbour added to list. \n",device_name, my_address, op_sim_time());
							fprintf (log_file, "\n");
							//printf ("\n");
							fprintf (log_file, "neighbour list at this time\n");
						//	printf ("[%s]neighbour list at this time\n", device_name);
							for (i=0; i<No_of_neighbours; i++) {
								fprintf (log_file, "%d. node address: %d of rssi of: %f  \n", i, (int) Neighbour_list[i][node_address_index], Neighbour_list[i][node_rssi_index]);
								//printf ("%d. node address: %d of rssi of: %f \n", i, (int) Neighbour_list[i][node_address_index], Neighbour_list[i][node_rssi_index]);
							}
								
							
							}
						
						}
					// send the current no of neighbours and addresses to tdp
					Ici* tdp_ici_ptr;
					//int ici_n_list[10];
					int *ici_n_list;
					ici_n_list = new int[15];
					for (i = 0; i<No_of_neighbours; i++)
						ici_n_list[i] = Neighbour_list[i][node_address_index];
									
					
					//printf ("\n sending sdnc neighbours to topo disc process\n");
					fprintf (log_file, "\n sending sdnc neighbours to topo disc process \n");
					tdp_ici_ptr = op_ici_create ("mac_to_tdp_ici");
					op_ici_attr_set (tdp_ici_ptr, "sdnc_neighbours_no", No_of_neighbours);
					op_ici_attr_set (tdp_ici_ptr, "addresses", ici_n_list);
					
					
					op_ici_install (tdp_ici_ptr);
					op_intrpt_schedule_remote (op_sim_time(), MAC_TO_TDP, tdp_id); 
					op_ici_install (OPC_NIL);
				
					}
			
			/*Check "Frame Type" field*/		
			if (frame_type_fd == BEACON_FRAME_TYPE) { /* I have received beacon packet */
		//////////////////// rhbi
			//printf("[%s (#%d)] t=%f ::STRM_FROM_RADIO_TO_MAC  print pk  \n",device_name, my_address, op_sim_time());
			//printf("I am reciving STRM_FROM_RADIO_TO_MAC type of Frame is BEACON_FRAME_TYPE=%d   \n", frame_type_fd );	
		//	op_pk_print (frame_MPDU);
		//////////////////// end rhbi
			//cout<<"inside the frame_MPDU packet MSDU="<<endl;
			//op_pk_nfd_get (frame_MPDU, "MSDU", &msduu);
			//op_pk_print (msduu);
							
			
			
				op_pk_nfd_get (frame_MPDU, "Source Address", &source_address);
				op_pk_nfd_get (frame_MPDU, "Source PAN Identifier", &pan_id);				
				
				//if ((source_address == my_parent_address) && (pan_id == node_attributes.pan_id))
			{ /* beacon packet from my parent */
				    wpan_extract_beacon_frame (frame_MPDU,source_address);
					
					if (enable_log) {
						fprintf (log_file,"t=%f  -> Beacon packet reception from a neighbour (#%d) \n\n", op_sim_time(), source_address);
						//printf (" [%s (#%d)] t=%f  -> Beacon packet reception from a neighbour (#%d) \n\n", device_name, my_address, op_sim_time(), source_address);
					}
				}
			} else if (SF_params[SF_in].CAP_ACTIVE || SF_params[SF_in].CFP_ACTIVE || SF_params[SF_out].CAP_ACTIVE || SF_params[SF_out].CFP_ACTIVE) { /* I am in CAP or CFP */						
				/* check "Frame Type" field */
				if (frame_type_fd == ACK_FRAME_TYPE) {	/* acknowledgment packet */	
//				printf("[%s (#%d)] t=%f ::STRM_FROM_RADIO_TO_MAC  print pk  \n",device_name, my_address, op_sim_time());
//				printf("I am reciving STRM_FROM_RADIO_TO_MAC type of Frame is ACK=%d   \n", frame_type_fd );
//				op_pk_print (frame_MPDU);
/*					
	

	 
if (enable_log) {
						fprintf (log_file,"t=%f  -> ACK frame Reception - waiting for ACK? = %s \n",op_sim_time(), boolean2string(node_attributes.wait_ack));
						printf (" [%s (#%d)] t=%f  -> ACK frame Reception - waiting for ACK? = %s \n",device_name, my_address, op_sim_time(), boolean2string(node_attributes.wait_ack));
					} 
*/
					wpan_extract_ack_frame (frame_MPDU);
				} else {   /* if it is not an ACK Frame or Beacon frame */			
					op_pk_nfd_get (frame_MPDU, "Source Address", &source_address);
					op_pk_nfd_get (frame_MPDU, "Destination Address", &dest_address);
					op_pk_nfd_get (frame_MPDU, "Destination PAN Identifier", &pan_id);				
					///new hbi
				//	if ((my_address==76 )&&(source_address==75)){
				//		dest_address=76;
					
					
					if (((dest_address == my_address) || (dest_address == BROADCAST_ADDRESS)) && (pan_id == node_attributes.pan_id)) {
						switch(frame_type_fd) {						
							case DATA_FRAME_TYPE: 	/* For a data frame */
							{		
/* random generator *///		printf("[%s (#%d)] t=%f ::STRM_FROM_RADIO_TO_MAC  print pk  \n",device_name, my_address, op_sim_time());
//							printf("I am reciving STRM_FROM_RADIO_TO_MAC type of Frame is Data=%d   \n", frame_type_fd );				
//							op_pk_print (frame_MPDU);								
							rnd = op_dist_uniform (10.0);
								
								//rnd = 10;
// printf ("AAAA [%s (#%d)] t=%f rnd = %f \n\n", device_name, my_address, op_sim_time(), rnd);
								
								op_stat_write (statistic_global_vector.rnd_generator, rnd);
								
								if (!collision_occurred) {								
									if (rnd<2.0) {
										//printf ("[%s (#%d)] t=%f Randomly dropped packet !!!!! \n\n", device_name, my_address, op_sim_time());								
									} else {
										if (enable_log) {
											fprintf (log_file,"t=%f  Data frame reception from node #%d \n\n", op_sim_time(), source_address);
											//printf (" [%s (#%d)] t=%f  Data frame reception from node #%d \n\n", device_name, my_address, op_sim_time(), source_address);
										}
									 
							/*
							 double d; Packet* stts_pkt;
							stts_pkt = op_pk_create_fmt ("status_repo");
							op_pk_nfd_get (rcv_frame, "Source Address", &d);
							op_pk_nfd_set (stts_pkt, "RSSI", op_td_get_dbl (rcv_frame, OPC_TDA_RA_SNR));
							op_pk_nfd_set (stts_pkt, "RSSI_dst",  my_address);
							op_pk_nfd_set (stts_pkt, "RSSI_src", d);
							 cout<<"sending Rssi packet info from MAC to Status Collector"<<endl;
							 op_pk_print(stts_pkt);
							op_pk_send (stts_pkt,STRM_FROM_MAC_TO_STATUS_COL);
							*/
										wpan_extract_data_frame (frame_MPDU);
									}
								} else {
									collision_occurred = OPC_FALSE;
								}
								
								break;
							};
							case COMMAND_FRAME_TYPE:	/* For a Command frame */
							{
//							printf("[%s (#%d)] t=%f ::STRM_FROM_RADIO_TO_MAC  print pk  \n",device_name, my_address, op_sim_time());
//							printf("I am reciving STRM_FROM_RADIO_TO_MAC type of Frame is COMMAND_FRAME_TYPE=%d   \n", frame_type_fd );
//							op_pk_print (frame_MPDU);
								if (!collision_occurred) {
									if (enable_log) {
										fprintf (log_file,"t=%f  Command frame reception from node #%d \n\n", op_sim_time(), source_address);
										//printf (" [%s (#%d)] t=%f  Command frame reception from node #%d \n\n", device_name, my_address, op_sim_time(), source_address);
									} 
									wpan_extract_command_frame (frame_MPDU);
								} else {
									collision_occurred = OPC_FALSE;
								}
								
								break;
							};
							default:	/* OTHER FRAME TYPES */
							{
							};														
						}
					//}//new hbi
						} else if (source_address == my_address) {
/*						if (enable_log) {
							fprintf(log_file,"t=%f  -> Loop: DISCARD FRAME \n\n",op_sim_time());
							printf (" [%s (#%d)] t=%f  -> Loop: DISCARD FRAME \n\n",device_name, my_address, op_sim_time());
						} 
*/
						op_pk_destroy (frame_MPDU);					
					} else { // ((dest_address != my_address) && (dest_address!= BROADCAST_ADDRESS))
/*						if (enable_log) {
							fprintf (log_file,"t=%f  -> Frame reception from node #%d \n",op_sim_time(), source_address);
							fprintf (log_file,"t=%f  -> Wrong destination address (my_address: %d <-> dest_address: %d) : DISCARD FRAME \n\n",op_sim_time(), my_address, dest_address);

							printf (" [%s (#%d)] t=%f  -> Frame reception from node #%d \n",device_name, my_address, op_sim_time(), source_address);							
							printf (" [%s (#%d)] t=%f  -> Wrong destination address (my_address: %d <-> dest_address: %d) : DISCARD FRAME \n\n",device_name, my_address, op_sim_time(), my_address, dest_address);
						} 
*/
						op_pk_destroy (frame_MPDU);					
					}				
				}
			}
			op_pk_destroy (rcv_frame);
			break;
			};
	
		default :
		{
		};
	}	

	/* Stack tracing exit point */
	FOUT;
}

/*--------------------------------------------------------------------------------
 * Function:	wpan_extract_beacon_frame
 *
 * Description:	extract the beacon frame received from my parent node             
 *
 * Input :  mac_frame - the received MAC frame (MPDU)
 *--------------------------------------------------------------------------------*/

static void wpan_extract_beacon_frame(Packet* mac_frame, int Beacon_source_address){

	Packet* beacon_frame;
	List* gts_list;
	wpan_gts_descriptor* gts_descriptor_ptr;
	Boolean is_it_from_PAN_Coord;
	double beacon_frame_creation_time;	
	int beacon_order;
	int superframe_order;
	int gts_directions_mask;
	int gts_descriptor_count;
	int direction;
	int i;
	double time;
	
//	int Current_beacon_seq_num;
//	int ndp_flag;
//	Boolean neighbour_available;
	
	/* Stack tracing enrty point */
	FIN(wpan_extract_beacon_frame);
	
	op_pk_nfd_get_pkt (mac_frame, "MSDU", &beacon_frame);		
		
	/* if I am a End Device or Router, I get the superframe information, and I sychronize myself */
	if (!IAM_PAN_COORDINATOR) {
	//////////////////////// there is no chance that a sdn controller    ///////////////////////////
	//////////////////////// would receive a normal beacon frame         ////////////////////////////////////////	
//	printf ("Here we make Neighbour_list[i] \n");	
//		// check if NDP flag is on
//		op_pk_nfd_get (beacon_frame, "NDP flag", &ndp_flag);
//		if (ndp_flag == 1) {
		
//			// check if i received this beacon before
//			op_pk_nfd_get (mac_frame, "Sequence Number", &Current_beacon_seq_num);	
//			if (Current_beacon_seq_num != NDP_beacon_seq_num){
//				NDP_beacon_seq_num = Current_beacon_seq_num;
//				neighbour_available = OPC_FALSE;
				// take the beacon source address
				// check if its in neighbour list, if not add it to this list
//				for (i=0; i<No_of_neighbours; i++){
//					if (Neighbour_list[i] == Beacon_source_address)
//						neighbour_available = OPC_TRUE;
//					}
//				if (!neighbour_available) {
//					Neighbour_list[No_of_neighbours] = Beacon_source_address;
//					No_of_neighbours = No_of_neighbours + 1;
//					if (enable_log) {
//						fprintf (log_file, "t=%f  -> new neighbour added to list . \n",op_sim_time());
//						printf (" [%s (#%d)] t=%f  -> new neighbour added to list. \n",device_name, my_address, op_sim_time());
//						fprintf (log_file, "\n");
//						printf ("\n");
//						
//						fprintf (log_file, "neighbour list at this time\n");
//							printf ("[%s]neighbour list at this time\n", device_name);
//							for (i=0; i<No_of_neighbours; i++) {
//								fprintf (log_file, "%d. node address: %d \n", i, Neighbour_list[i]);
//								printf ("%d. node address: %d \n", i, Neighbour_list[i]);
//							}
//							fprintf (log_file, "*****************end of neighbour list*************************\n");
//							printf ("************************end of neighbour list***************************\n");	
//						}
//					}
//				sdn_wsan_send_ndp_beacon_frame();
//				// make no further instructions
//				} else {  // if Current_beacon_seq_num = NDP_beacon_seq_num which means i ve already received this one
//						// nothing to do here
//			}
//			}
	//////////////////////////////////////////////////////////////
	
		op_pk_nfd_get (beacon_frame, "Beacon Order", &beacon_order);
		op_pk_nfd_get (beacon_frame, "Superframe Order", &superframe_order);
		op_pk_nfd_get (beacon_frame, "Final CAP Slot", &SF_params[SF_in].Final_CAP_Slot);
		//op_pk_nfd_get (beacon_frame, "Battery Life Ext", &csma_params.macBattLifeExt);
		op_pk_nfd_get (beacon_frame, "PAN Coordinator", &is_it_from_PAN_Coord);			
		
		beacon_frame_creation_time = op_pk_creation_time_get (beacon_frame);
		
		if (waiting_for_first_beacon) {
			if ((GTS_params[GTS_DIRECTION_TRANSMIT].start_time < op_sim_time()) && (GTS_params[GTS_DIRECTION_TRANSMIT].start_time != START_DISABLED)) 		
				op_intrpt_schedule_self (op_sim_time()+epsilon, START_OF_TRANSMIT_GTS_USE);
			
			if ((GTS_params[GTS_DIRECTION_RECEIVE].start_time < op_sim_time()) && (GTS_params[GTS_DIRECTION_RECEIVE].start_time != START_DISABLED)) 		
				op_intrpt_schedule_self (op_sim_time()+epsilon, START_OF_RECEIVE_GTS_USE);
			
			waiting_for_first_beacon = OPC_FALSE;			
		}
			
		/* update incoming superframe parameters	*/		 
		SF_params[SF_in].SD = aBaseSuperframeDuration * exponent(2, superframe_order);
		SF_params[SF_in].BI = aBaseSuperframeDuration * exponent(2, beacon_order);
		SF_params[SF_in].slot_duration = aBaseSlotDuration * exponent(2, superframe_order);
		SF_params[SF_in].sleep_period = SF_params[SF_in].BI - SF_params[SF_in].SD;
		SF_params[SF_in].BI_Boundary = beacon_frame_creation_time;	
			
		/* GTS fields */
		op_pk_nfd_get (beacon_frame, "GTS Descriptor Count", &gts_descriptor_count);
		op_pk_nfd_get (beacon_frame, "GTS Permit", &node_attributes.my_parent_GTSPermit);
			
		/* find appropriate GTS_descriptor in the received GTS_list */
		if (gts_descriptor_count > 0) {			
			op_pk_nfd_get (beacon_frame, "GTS Directions", &gts_directions_mask);
			
			/* obtain the GTS list */
			op_pk_nfd_access (beacon_frame, "GTS List", &gts_list);
		
			for (i=0; i<op_prg_list_size (gts_list); i++) {					
				if ((gts_descriptor_ptr = (wpan_gts_descriptor *) op_prg_list_access (gts_list, i)) != OPC_NIL) {	
					direction = floor((double)gts_directions_mask/(exponent(2, (6-i)))); /* get GTS direction */
					gts_directions_mask = gts_directions_mask - direction*exponent(2, (6-i)); /* update direction mask */
									  
					if (gts_descriptor_ptr->device_short_address == my_address) { 
						/* this GTS_descriptor is for me */
						if (gts_descriptor_ptr->start_slot == 0) {
							/* GTS allocation was not successful */								
							if (GTS_params[direction].is_waiting_GTS_confirm) {	
								if (direction == GTS_DIRECTION_TRANSMIT)
									op_intrpt_disable (OPC_INTRPT_SELF, STOP_OF_TRANSMIT_GTS_USE, OPC_FALSE);	/* disable already scheduled GTS deallocation */
								else
									op_intrpt_disable (OPC_INTRPT_SELF, STOP_OF_RECEIVE_GTS_USE, OPC_FALSE);	/* disable already scheduled GTS deallocation */
								
								if (enable_log) {
									fprintf (log_file, "t=%f  -> GTS allocation (in direction %d) was not successful - GTS request was rejected by parent. \n", op_sim_time(), direction);
									fprintf (log_file, "\t Parent has not enough resources - the maximum available length for the GTS is %d slot(s). \n\n", gts_descriptor_ptr->length);
								//	printf (" [%s (#%d)] t=%f  -> GTS allocation (in direction %d) was not successful - GTS request was rejected by parent. \n", device_name, my_address, op_sim_time(), direction);
								//	printf ("\t Parent has not enough resources - the maximum available length for the GTS is %d slot(s). \n\n", gts_descriptor_ptr->length);
								}
							} 
						} else { /* (gts_descriptor_ptr->start_slot != 0) - GTS allocation was successful */
							GTS_params[direction].start_slot = gts_descriptor_ptr->start_slot;	/* allocated GTS start slot */								
						
							if (enable_log) {
								fprintf (log_file, "t=%f  -> GTS allocation (in direction %d) was successful. \n\n", op_sim_time(), direction);
								//printf (" [%s (#%d)] t=%f  -> GTS allocation (in direction %d) was successful. \n\n", device_name, my_address, op_sim_time(), direction);
							}								
						}
							
						/* I have already received beacon with GTS allocation response */
						GTS_params[direction].is_waiting_GTS_confirm = OPC_FALSE;
						GTS_params[direction].wait_GTS_confirm_count = 0;									
					} 
				}
			}
		} /* fi (gts_descriptor_count > 0) 
			
		/* deallocation request was sent to parent - stop using TRANSMIT/RECEIVE GTS */
		for (i=0;i<2;i++) {
			if (GTS_params[i].is_deallocation_req_sent) {
				GTS_params[i].start_slot = 0;	
				GTS_params[i].is_deallocation_req_sent = OPC_FALSE;
				op_stat_write (gts_statistic_vector.wasted_bandwidth_pc, 0.0); 
				op_stat_write (gts_statistic_vector.throughput_pc, 0.0);			
			}
		}
		
		/* request was sent to parent - waiting for confirmation in the beacon frame */
		for (i=0;i<2;i++) {
			if (GTS_params[i].is_waiting_GTS_confirm) {
				if (GTS_params[i].wait_GTS_confirm_count == aGTSDescPersistenceTime) {
					GTS_params[i].is_waiting_GTS_confirm = OPC_FALSE;	
					GTS_params[i].wait_GTS_confirm_count = 0;

					if (enable_log) {
						fprintf (log_file, "t=%f  -> !!!! GTS reqeuest (in direction %d) failed - I have not received response from my parent. \n\n", op_sim_time(), i);
						//printf (" [%s (#%d)] t=%f  -> !!!! GTS reqeuest (in direction %d) failed - I have not received response from my parent. \n\n", device_name, my_address, op_sim_time(), i);
					}	
				} else {
					GTS_params[i].wait_GTS_confirm_count = GTS_params[i].wait_GTS_confirm_count + 1;
				}
			}
		}
			
		if (enable_log) {
		/*	
		printf (" [%s (#%d)] t=%f  -> Beacon Frame Reception - synchronization. \n", device_name, my_address, op_sim_time());
			printf ("	Parameters of incoming superframe (from my parent)\n");
			printf ("   - PARENT's BEACON ORDER (BO)      : %d \n", beacon_order);
			printf ("   - PARENT's SUPERFRAME ORDER (SO)  : %d \n", superframe_order);
			printf ("   - BEACON INTERVAL BOUNDARY  : %f Sec \n", SF_params[SF_in].BI_Boundary);
			printf ("   - SUPERFRAME DURATION       : %d Symbols = %f sec \n", SF_params[SF_in].SD, Symbols2Sec(SF_params[SF_in].SD, WPAN_DATA_RATE));
			printf ("   - BEACON INTERVAL           : %d Symbols = %f sec\n", SF_params[SF_in].BI, Symbols2Sec(SF_params[SF_in].BI, WPAN_DATA_RATE));
			printf ("   - Sleep Time                : %d Symbols = %f sec\n", SF_params[SF_in].sleep_period, Symbols2Sec(SF_params[SF_in].sleep_period, WPAN_DATA_RATE));
			printf ("   - TIME SLOT DURATION        : %d Symbols = %f sec \n", SF_params[SF_in].slot_duration, Symbols2Sec(SF_params[SF_in].slot_duration, WPAN_DATA_RATE));
			printf ("   - FINAL CAP SLOT (0-15)     : %d \n", SF_params[SF_in].Final_CAP_Slot);
			//printf ("   - BATT LIFE EXTENSION       : %s \n", boolean2string(csma_params.macBattLifeExt));	
			printf ("   - MY PARENT PERMITS GTS     : %s \n\n", boolean2string(node_attributes.my_parent_GTSPermit));
			
			for (i=0;i<2;i++) {
				if (i == GTS_DIRECTION_TRANSMIT)
					printf ("   - TRANSMIT GTS \n");
				else
					printf ("   - RECEIVE GTS \n");
				
				if (GTS_params[i].start_slot > 0) {			
					printf ("\t\t ALLOCATED GTS START SLOT     : %d \n", GTS_params[i].start_slot);
					printf ("\t\t ALLOCATED LENGTH OF GTS      : %d slots = %f sec \n", GTS_params[i].length, Symbols2Sec(GTS_params[i].length*SF_params[SF_in].slot_duration, WPAN_DATA_RATE));
				} else {
					printf ("\t\t NONE ALLOCATED GTS.\n");
				}			
				}			
			
			printf ("\n");
			*/
			fprintf (log_file,"t=%f  -> Beacon Frame Reception - synchronization. \n",op_sim_time());
			fprintf (log_file,"	Parameters of incoming superframe (from my parent)\n");
			fprintf (log_file,"   - PARENT's BEACON ORDER (BO)      : %d \n", beacon_order);
			fprintf (log_file,"   - PARENT's SUPERFRAME ORDER (SO)  : %d \n", superframe_order);
			fprintf (log_file,"   - BEACON INTERVAL BOUNDARY    : %f Sec \n", SF_params[SF_in].BI_Boundary);
			fprintf (log_file,"   - SUPERFRAME DURATION         : %d Symbols = %f sec \n", SF_params[SF_in].SD, Symbols2Sec(SF_params[SF_in].SD, WPAN_DATA_RATE));
			fprintf (log_file,"   - BEACON INTERVAL             : %d Symbols = %f sec\n", SF_params[SF_in].BI, Symbols2Sec(SF_params[SF_in].BI, WPAN_DATA_RATE));
			fprintf (log_file,"   - TIME SLOT DURATION          : %d Symbols = %f sec \n", SF_params[SF_in].slot_duration, Symbols2Sec(SF_params[SF_in].slot_duration, WPAN_DATA_RATE));
			fprintf (log_file,"   - FINAL CAP SLOT (0-15)       : %d \n", SF_params[SF_in].Final_CAP_Slot);
			//fprintf (log_file,"   - BATT LIFE EXTENSION         : %s \n", boolean2string(csma_params.macBattLifeExt));
			fprintf (log_file,"   - MY PARENT PERMITS GTS       : %s \n\n", boolean2string(node_attributes.my_parent_GTSPermit));
			
			for (i=0;i<2;i++) {
				if (i == GTS_DIRECTION_TRANSMIT)
					fprintf (log_file,"   - TRANSMIT GTS \n");
				else
					fprintf (log_file,"   - RECEIVE GTS \n");
				
				if (GTS_params[i].start_slot > 0) {			
					fprintf (log_file,"\t\t ALLOCATED GTS START SLOT    : %d \n", GTS_params[i].start_slot);
					fprintf (log_file,"\t\t ALLOCATED LENGTH OF GTS     : %d slots = %f sec \n", GTS_params[i].length, Symbols2Sec(GTS_params[i].length*SF_params[SF_in].slot_duration, WPAN_DATA_RATE));
				} else {
					fprintf (log_file,"\t\t NONE ALLOCATED GTS.\n");
				}			
			}			
			
			fprintf (log_file,"\n");
		}
		
		SF_ptr = &SF_params[SF_in]; /* incoming superframe */
		
		/* schedule self-interrupts */
		op_intrpt_schedule_self (op_sim_time(), DEFAULT_CODE); // -> go to IDLE state in proccess model
		
		op_intrpt_schedule_self (op_sim_time(), START_OF_CAP_PERIOD_CODE);
		op_intrpt_schedule_self (SF_params[SF_in].BI_Boundary+Symbols2Sec(((SF_params[SF_in].Final_CAP_Slot+1)*SF_params[SF_in].slot_duration), WPAN_DATA_RATE), END_OF_CAP_PERIOD_CODE);
		
		op_intrpt_schedule_remote (SF_params[SF_in].BI_Boundary+Symbols2Sec(SF_params[SF_in].BI, WPAN_DATA_RATE), END_OF_SLEEP_PERIOD, baterry_id);
		
		
		if (IAM_ROUTER) {
			//time = SF_params[SF_in].BI_Boundary + wpan_backoff_period_boundary_get_(node_attributes.startTime);
time = SF_params[SF_in].BI_Boundary + node_attributes.startTime;

			if (time < op_sim_time()) {			
				//printf (" [%s (#%d)] t=%f  -> ERROR: Start time of my own beacon (%f) is less than simulation time.  \n\n", device_name, my_address, op_sim_time(), time);
				op_sim_end("ERROR - wpan_extract_beacon_frame","Start time of my own beacon is less than simulation time.","","");
			} else {	
				op_intrpt_schedule_self (time, BEACON_INTERVAL_CODE); /* schedule beacon transmission for my own cluster */

				if (enable_log) {
					fprintf (log_file,"t=%f  -> (Router) Schedule my own beacon at %f sec.\n\n", op_sim_time(), time);
					//printf (" [%s (#%d)] t=%f  -> (Router) Schedule my own beacon at %f sec.\n\n", device_name, my_address, op_sim_time(), time);
				}
			}
		}
		
		/* schdedule the GTS self-interrupts in incoming superframe - START_OF_GTS_PERIOD and END_OF_GTS_PERIOD*/
		for (i=0;i<2;i++) {
			if (GTS_params[i].start_slot > 0) {				
				op_intrpt_schedule_self (SF_params[SF_in].BI_Boundary+Symbols2Sec(((GTS_params[i].start_slot)*SF_params[SF_in].slot_duration), WPAN_DATA_RATE), START_OF_GTS_PERIOD);
				op_intrpt_schedule_self (SF_params[SF_in].BI_Boundary+Symbols2Sec(((GTS_params[i].start_slot+GTS_params[i].length)*SF_params[SF_in].slot_duration), WPAN_DATA_RATE), END_OF_GTS_PERIOD);
							
				if (enable_log) {
					fprintf (log_file, "t=%f  -> Schedule GTS (in direction %d) in current superframe (from %f sec to %f sec). \n\n", op_sim_time(), i, SF_params[SF_in].BI_Boundary+Symbols2Sec(((GTS_params[i].start_slot)*SF_params[SF_in].slot_duration), WPAN_DATA_RATE), SF_params[SF_in].BI_Boundary+Symbols2Sec(((GTS_params[i].start_slot+GTS_params[i].length)*SF_params[SF_in].slot_duration), WPAN_DATA_RATE));
					//printf (" [%s (#%d)] t=%f  -> Schedule GTS (in direction %d) in current superframe (from %f sec to %f sec). \n\n", device_name, my_address, op_sim_time(), i, SF_params[SF_in].BI_Boundary+Symbols2Sec(((GTS_params[i].start_slot)*SF_params[SF_in].slot_duration), WPAN_DATA_RATE), SF_params[SF_in].BI_Boundary+Symbols2Sec(((GTS_params[i].start_slot+GTS_params[i].length)*SF_params[SF_in].slot_duration), WPAN_DATA_RATE));
				}			
			}	
		}		
	} 
	
	op_pk_destroy (beacon_frame);
	op_pk_destroy (mac_frame);
	
	/* Stack tracing exit point */
	FOUT;	
}

/*------------------------------------------------------------------------------
 * Function:	compare_GTS_list()
 *
 * Description:	This function returns  1 if A should be closer to the head of the list.
 *				This function returns -1 if B should be closer to the head of the list.
 *				This function returns  0 if A and B have the same rank.
 *
 * No parameters
 *-----------------------------------------------------------------------------*/

int compare_GTS_listc (const void *aptr, const void *bptr) { 

	/* Stack tracing enrty point */
	FIN (compare_GTS_listc);

	
	if (((wpan_gts_list_record *) aptr)->direction < ((wpan_gts_list_record *) bptr)->direction) {
		FRET (1);
	} else if (((wpan_gts_list_record *) aptr)->direction > ((wpan_gts_list_record *) bptr)->direction)   {	
		FRET (-1);
	} else {
		FRET (0)
	}
}


/*------------------------------------------------------------------------------
 * Function:	wpan_send_beacon_frame()
 *
 * Description:	the PAN coordinator or router send a beacon frame to the associated nodes
 *              and schedule the transmission time of the next beacon
 *
 * No parameters
 *-----------------------------------------------------------------------------*/

static void wpan_send_beacon_frame() {
	Packet* beacon_MSDU;
	Packet* beacon_MPDU;	
	Packet* beacon_PPDU;
	Packet* payLoad_frame;
	List* gts_list;	
	wpan_gts_list_record* gts_list_record_ptr;
	wpan_gts_descriptor* gts_descriptor_ptr;	
	double beacon_frame_creation_time;
	int gts_descriptor_count = 0;
	int gts_directions_mask = 0;
	int seq_num;
	int i;
	
	/* Stack tracing enrty point */
	FIN(wpan_send_beacon_frame);
	
/*	for (i=0; i<op_prg_list_size (node_attributes.GTS_list); i++) {
		gts_list_record_ptr = (wpan_gts_list_record *) op_prg_list_access (node_attributes.GTS_list, i);
		printf("AAAA sort before %s - assoc_dev_address: %d, direction: %d, start_slot: %d \n ",device_name, gts_list_record_ptr->assoc_dev_address, gts_list_record_ptr->direction, gts_list_record_ptr->start_slot);
	} 
*/
	
	/* sort GTS_list according to the priority rule - first transmitting GTSs (0), then receiving GTSs (1) */
	op_prg_list_sort (node_attributes.GTS_list, compare_GTS_listc);
	
	/* update GTS_list */
	wpan_gts_reallocation_request ();
	
//	op_prg_list_transfer (GTS_list_temp, node_attributes.GTS_list, sizeof(wpan_gts_list_record)); /* copy GTS list */
	
	/* allocate an empty list */
	gts_list = op_prg_list_create ();
	
	/* create a beacon payload (MSDU) */
	beacon_MSDU = op_pk_create_fmt ("wpan_beacon_MSDU_format");
	
	/* set the fields of the beacon frame */
	op_pk_nfd_set (beacon_MSDU, "Beacon Order", node_attributes.beacon_order);
	op_pk_nfd_set (beacon_MSDU, "Superframe Order", node_attributes.superframe_order);
	op_pk_nfd_set (beacon_MSDU, "Final CAP Slot", SF_params[SF_out].Final_CAP_Slot);
	op_pk_nfd_set (beacon_MSDU, "Battery Life Ext", csma_params.macBattLifeExt);	
	op_pk_nfd_set (beacon_MSDU, "PAN Coordinator", IAM_PAN_COORDINATOR);
	

	
	
	if (NDP_is_required) {
	op_pk_nfd_set (beacon_MSDU, "NDP flag", flag_ON);
	NDP_is_required = OPC_FALSE;
	if (enable_log) {
		fprintf (log_file, "t=%f  -> ndp flag is set in beacon to initiate NDP. \n",op_sim_time());
		//printf (" [%s (#%d)] t=%f  -> ndp flag is set in beacon to initiate NDP. \n",device_name, my_address, op_sim_time());
		fprintf (log_file, "\n");
		//printf ("\n");
		}
	
	} else
		op_pk_nfd_set (beacon_MSDU, "NDP flag", flag_OFF);
	
	/* set the appropriate GTS fields of the beacon frame */
	op_pk_nfd_set (beacon_MSDU, "GTS Permit", node_attributes.macGTSPermit);
	
	 payLoad_frame = op_pk_create_fmt ("Pan_Sink_address_and_Flow_Tables_frame_tdpInfo");
	
	if (tdp_decision)
		{
			tdp_decision = OPC_FALSE;
			op_pk_nfd_set (payLoad_frame, "TDP_CODE", tdp_code);
			op_pk_nfd_set_ptr(payLoad_frame,"startTimeSoBo_list", startTimeSoBo_list_ptr, op_prg_mem_copy_create, op_prg_mem_free, sizeof (startTimeSoBo_list));
			tdp_code = -1;
		}
	else
		op_pk_nfd_set (payLoad_frame, "TDP_CODE", -1);
	
	
	// added by alish to add sink address to the beacon payload of the second beacon and on
	if ( PANsink_mac_address != -3)
		{
			op_pk_nfd_set (payLoad_frame, "Pan_Sink_Address",  PANsink_mac_address);		
			
		}
 if (flowTable_sending_needed > 1)
	{
	flowTable_sending_needed--;
	op_pk_nfd_set_ptr (payLoad_frame, "Flow_Tables", &flowEntriesP[0], op_prg_mem_copy_create, op_prg_mem_free, sizeof (FlowEntery) * flowEntriesP.size());
 	op_pk_nfd_set (payLoad_frame, "flow_entries_no", flowEntriesP.size());		
	}
 else 
	op_pk_nfd_set (payLoad_frame, "flow_entries_no", 0);	
 
 	
		
	op_pk_nfd_set_pkt (beacon_MSDU, "Beacon Payload", payLoad_frame);
	
	for (i=0; i<op_prg_list_size (node_attributes.GTS_list); i++) {
		gts_list_record_ptr = (wpan_gts_list_record *) op_prg_list_access (node_attributes.GTS_list, i);
		
		if (gts_list_record_ptr->beacon_persistence_time > 0) {
			/* memory allocation */
			gts_descriptor_ptr	= (wpan_gts_descriptor *) op_prg_mem_alloc (sizeof (wpan_gts_descriptor));
			
			gts_descriptor_ptr->device_short_address = gts_list_record_ptr->assoc_dev_address;
			gts_descriptor_ptr->start_slot = gts_list_record_ptr->start_slot;
			gts_descriptor_ptr->length = gts_list_record_ptr->length;
			
			/* insert the GTS descriptor in the list tail */
			op_prg_list_insert (gts_list, gts_descriptor_ptr, OPC_LISTPOS_TAIL);
			
			gts_directions_mask = gts_directions_mask + (exponent(2, (6-gts_descriptor_count)) * gts_list_record_ptr->direction);
			
			gts_list_record_ptr->beacon_persistence_time = gts_list_record_ptr->beacon_persistence_time - 1;	
			gts_descriptor_count = gts_descriptor_count + 1;			
		}		
	} 
	
	op_pk_nfd_set (beacon_MSDU, "GTS Descriptor Count", gts_descriptor_count);
	
	if (gts_descriptor_count > 0) {
		op_pk_nfd_set (beacon_MSDU, "GTS Directions", gts_directions_mask);		
		op_pk_nfd_set_ptr (beacon_MSDU, "GTS List", gts_list, gts_nested_list_mem_copy_create, gts_nested_list_mem_free, sizeof(wpan_gts_descriptor)*gts_descriptor_count);
		
		/* set the appropriate size of the GTS_Directions (8 bits) and GTS_List fields according the specification */
		op_pk_bulk_size_set (beacon_MSDU, (8 + gts_descriptor_count*24)); // the size of GTS_Descriptor = 24 bits
	}
	
	/* create MAC frame (MPDU) that encapsulates the beacon payload (MSDU) */
	beacon_MPDU = op_pk_create_fmt("wpan_beacon_MPDU_format");

	/* generate the sequence number for the packet to send */
	seq_num = wpan_update_sequence_number();		
		
	op_pk_nfd_set (beacon_MPDU, "Frame Type", BEACON_FRAME_TYPE);
	op_pk_nfd_set (beacon_MPDU, "Sequence Number", seq_num); 
	op_pk_nfd_set (beacon_MPDU, "Source PAN Identifier", node_attributes.pan_id);
	op_pk_nfd_set (beacon_MPDU, "Source Address", my_address);
	op_pk_nfd_set_pkt (beacon_MPDU, "MSDU", beacon_MSDU); // wrap beacon payload (MSDU) in MAC Frame (MPDU)
	op_pk_nfd_set (beacon_MPDU, "Ack Request", 0);
	
	beacon_frame_creation_time = op_pk_creation_time_get(beacon_MSDU);
		
	/* update outgoing superframe parameters */ 			
	SF_params[SF_out].BI_Boundary = beacon_frame_creation_time;
	
	
	if (enable_log) {
		fprintf (log_file, "t=%f  -> Beacon Frame Transmission (PAN ID = %d). \n",op_sim_time(), node_attributes.pan_id);
		//printf (" [%s (#%d)] t=%f  -> Beacon Frame Transmission (PAN ID = %d). \n",device_name, my_address, op_sim_time(), node_attributes.pan_id);
	
		fprintf (log_file, "\t %d GTS(s) is allocated in the actual superframe \n", op_prg_list_size (node_attributes.GTS_list));
		//printf ("\t  %d GTS(s) is allocated in the actual superframe \n", op_prg_list_size (node_attributes.GTS_list));					
		
		for (i=0; i<op_prg_list_size (node_attributes.GTS_list); i++) {
			gts_list_record_ptr = (wpan_gts_list_record *) op_prg_list_access (node_attributes.GTS_list, i);			
			
			/* schdedule the GTS self-interrupts in outgoing superframe - START_OF_GTS_PERIOD and END_OF_GTS_PERIOD */
			op_intrpt_schedule_self (SF_params[SF_out].BI_Boundary+Symbols2Sec(((gts_list_record_ptr->start_slot)*SF_params[SF_out].slot_duration), WPAN_DATA_RATE), START_OF_GTS_PERIOD);			
			op_intrpt_schedule_self (SF_params[SF_out].BI_Boundary+Symbols2Sec(((gts_list_record_ptr->start_slot+gts_list_record_ptr->length)*SF_params[SF_out].slot_duration), WPAN_DATA_RATE), END_OF_GTS_PERIOD);


			fprintf (log_file, "\t For node: #%d \n", gts_list_record_ptr->assoc_dev_address);
			fprintf (log_file, "\t  + Start slot: %d, Length: %d, Directions: %d, Beacon persistence time: %d (from %f sec to %f sec) \n", gts_list_record_ptr->start_slot, gts_list_record_ptr->length, gts_list_record_ptr->direction, gts_list_record_ptr->beacon_persistence_time, SF_params[SF_out].BI_Boundary+Symbols2Sec(((gts_list_record_ptr->start_slot)*SF_params[SF_out].slot_duration), WPAN_DATA_RATE), SF_params[SF_out].BI_Boundary+Symbols2Sec(((gts_list_record_ptr->start_slot+gts_list_record_ptr->length)*SF_params[SF_out].slot_duration), WPAN_DATA_RATE));
			
			//printf ("\t For node: #%d \n", gts_list_record_ptr->assoc_dev_address);
			//printf ("\t  + Start slot: %d, Length: %d, Direction: %d, Beacon persistence time: %d (from %f sec to %f sec) \n", gts_list_record_ptr->start_slot, gts_list_record_ptr->length, gts_list_record_ptr->direction, gts_list_record_ptr->beacon_persistence_time, SF_params[SF_out].BI_Boundary+Symbols2Sec(((gts_list_record_ptr->start_slot)*SF_params[SF_out].slot_duration), WPAN_DATA_RATE), SF_params[SF_out].BI_Boundary+Symbols2Sec(((gts_list_record_ptr->start_slot+gts_list_record_ptr->length)*SF_params[SF_out].slot_duration), WPAN_DATA_RATE));
		}
		
		fprintf (log_file, "\n");
		//printf ("\n");
	}
	
		
	/* create PHY frame (PPDU) that encapsulates beacon MAC frame (MPDU) */
	beacon_PPDU = op_pk_create_fmt("wpan_frame_PPDU_format");
	
	/* wrap beacon MAC frame (MPDU) in PHY frame (PPDU) */
	op_pk_nfd_set_pkt (beacon_PPDU, "PSDU", beacon_MPDU); 
	op_pk_nfd_set (beacon_PPDU, "Frame Length", ((double) op_pk_total_size_get(beacon_MPDU))/8); //[bytes]
	
	wpan_battery_update_tx ((double)op_pk_total_size_get(beacon_PPDU));
//	cout<<"Here is the packet:beacon_MPDU "<<endl;
//	op_pk_print(beacon_MPDU);
	   op_pk_send (beacon_PPDU, STRM_FROM_MAC_TO_RADIO);
	 //op_pk_send (beacon_MPDU, STRM_FROM_MAC_TO_RADIO);
	
	SF_ptr = &SF_params[SF_out]; /* outgoing superframe */

	/* schedule self-interrupts */	
	op_intrpt_schedule_self (SF_params[SF_out].BI_Boundary+Bits2Sec(op_pk_total_size_get(beacon_MPDU),WPAN_DATA_RATE)+Symbols2Sec(wpan_ifs_symbol_get(beacon_MPDU),WPAN_DATA_RATE), START_OF_CAP_PERIOD_CODE);
	op_intrpt_schedule_self (SF_params[SF_out].BI_Boundary+Symbols2Sec(((SF_params[SF_out].Final_CAP_Slot+1)*SF_params[SF_out].slot_duration), WPAN_DATA_RATE), END_OF_CAP_PERIOD_CODE);	
	op_intrpt_schedule_remote (SF_params[SF_out].BI_Boundary+Symbols2Sec(SF_params[SF_out].BI, WPAN_DATA_RATE), END_OF_SLEEP_PERIOD, baterry_id);
	
	if (IAM_PAN_COORDINATOR) {
		op_intrpt_schedule_self (SF_params[SF_out].BI_Boundary+Symbols2Sec(SF_params[SF_out].BI, WPAN_DATA_RATE), BEACON_INTERVAL_CODE);

		if (enable_log) {
			fprintf (log_file,"t=%f  -> (PAN Coordinator) Schedule Next Beacon at %f sec. \n\n", op_sim_time(), SF_params[SF_out].BI_Boundary+Symbols2Sec(SF_params[SF_out].BI, WPAN_DATA_RATE));
			//printf (" [%s (#%d)] t=%f  -> (PAN Coordinator) Schedule Next Beacon at %f sec. \n\n", device_name, my_address, op_sim_time(), SF_params[SF_out].BI_Boundary+Symbols2Sec(SF_params[SF_out].BI, WPAN_DATA_RATE));
		}
	}
	
	/* Stack tracing exit point */
	FOUT;	
}


/*-----------------------------------------------------------------------------
 * Function:	wpan_extract_data_frame
 *
 * Description:	extract the data frame from the MAC frame received from the network
 *              
 * Input :  frame_MPDU - the received MAC frame
 *-----------------------------------------------------------------------------*/

static void wpan_extract_data_frame (Packet* frame_MPDU) {

	Ici* iciptr;
	Packet* packet_MSDU_ptr;
	int ack_Request;
	int seq_num;
	
	/* Stack tracing entry point */
	FIN(wpan_extract_data_frame);
	
	/* send to network layer for routing */
	iciptr = op_ici_create ("wpan_packet_ici_format");
	
	/* check if any ACK is requested */	
	op_pk_nfd_get (frame_MPDU, "Ack Request", &ack_Request);
	op_ici_attr_set (iciptr, "ACK", ack_Request);

	
	if (ack_Request) {		
		op_pk_nfd_get (frame_MPDU, "Sequence Number", &seq_num);		
		wpan_send_ack_frame (seq_num);			
	}	
	
	op_pk_nfd_get_pkt (frame_MPDU, "MSDU", &packet_MSDU_ptr);
			
	if (SF_params[SF_in].CAP_ACTIVE || SF_params[SF_out].CAP_ACTIVE) {
		op_ici_attr_set (iciptr, "Traffic Type", BEST_EFFORT_TRAFFIC);
	} else { 
		op_ici_attr_set (iciptr, "Traffic Type", REAL_TIME_TRAFFIC);
		
		gts_statistics.received_frames++; 
		gts_statistics.received_bits = gts_statistics.received_bits + op_pk_total_size_get (packet_MSDU_ptr); 
		
	}	
//		printf("+++++++++++++++++++++++++++++++++++++\n");
//		 printf("hbisend pk STRM_FROM_MAC_TO_NWK    \n");
//		 op_pk_print (packet_MSDU_ptr);
	/* send the MSDU via the stream to the network layer.	*/
	/* send ICI to network layer together with packet. */
	op_ici_install (iciptr);
	op_pk_send (packet_MSDU_ptr, STRM_FROM_MAC_TO_NWK);
	op_ici_install (OPC_NIL);

	/* Stack tracing exit point */
	FOUT;	
}

/*-----------------------------------------------------------------------------
 * Function:	wpan_extract_command_frame
 *
 * Description:	extract the command frame from the MAC frame received from the network
 *              
 * Input :  frame_MPDU - the received MAC frame 
 *-----------------------------------------------------------------------------*/

static void wpan_extract_command_frame(Packet* frame_MPDU) {
	
	Packet* command_frame;
	Packet* frame_MPDU_copy;
	int Ack_Request;
	int seq_num;
	int command_frm_id;
	
	/* Stack tracing enrty point */
	FIN(wpan_extract_command_frame);
		
	/* check if any ACK is requested */	
	op_pk_nfd_get (frame_MPDU, "Ack Request", &Ack_Request);	
	if (Ack_Request) {		
		op_pk_nfd_get (frame_MPDU, "Sequence Number", &seq_num);
		wpan_send_ack_frame (seq_num);		
	}
	
	
	/* send to network layer for statistics - ICI is not created */		
	//op_pk_send (frame_MPDU, STRM_FROM_MAC_TO_NWK); // TODO command statistics	
	
	frame_MPDU_copy = op_pk_copy (frame_MPDU);
	op_pk_nfd_get_pkt (frame_MPDU_copy, "MSDU", &command_frame);
	op_pk_nfd_get (command_frame, "Command Frame Identifier", &command_frm_id);
	
	/* check the command type */
	switch(command_frm_id) {
		case GTS_REQ:	/* GTS request command - allocation & deallocation of GTS */
		{
			if (!IAM_END_DEVICE) {
				wpan_gts_request_extract (frame_MPDU);
			} else {				
				/* display an appropriate warning */
				op_sim_error(OPC_SIM_ERROR_WARNING, "Warning - GTS Request", "I have received GTS request command, but I AM END DEVICE!!");
			
				fprintf(log_file,"t=%f  -> Warning - I have received GTS request, but I AM END DEVICE!!.\n\n", op_sim_time());
			}
			break;
		};
		case SINK_INFO: // added by alish wpan sink address 
			{
			if (IAM_PAN_COORDINATOR) {
			//op_pk_nfd_get (command_frame, "Command Payload", &sink_characteristics_ptr);
			 PANsink_mac_address=4;
			}
				/* send to higher layer for statistics */
			// frame_MPDU =mac_frame
				op_pk_send (frame_MPDU, STRM_FROM_MAC_TO_SINK);
			break;
			}
	
		case STATUS_REPORT: // added by alish wpan sink address 
			{
			
			if (IAM_PAN_COORDINATOR) {
				/* send to network intelligence */
				op_pk_send (frame_MPDU, STRM_FROM_MAC_TO_SINK);
			}
			
			break;
			}
		case APP_ROUTE_REQ:
			{
			
			if (IAM_PAN_COORDINATOR) {
				/* send to network intelligence */
			//	printf("con_req_prog: SDNC mac incoming command from end device ID is %d\n\n", command_frm_id);
				op_pk_send (frame_MPDU, STRM_FROM_MAC_TO_SINK);
			}
			
			break;
			}
	
		default:
		{
		};		
	}	
	
	op_pk_destroy (frame_MPDU_copy);
	op_pk_destroy (command_frame);

	/* Stack tracing exit point */
	FOUT;	
}


/*-----------------------------------------------------------------------------
 * Function:	wpan_send_ack_frame
 *
 * Description:	send back to the sender ACK frame
 *
 * Input :  seq_num - expected sequence number
 *-----------------------------------------------------------------------------*/
static void wpan_send_ack_frame(int seq_num) {

	double ack_tx_time;
	
	/* Stack tracing enrty point */
	FIN(wpan_send_ack_frame);
	
	ack_sequence_number = seq_num;
				
	if (enable_log) {
		fprintf(log_file,"t=%f  -> ACK requested - sending of ACK packet [SEQ = %d] is scheduled at %f sec \n\n", op_sim_time(), seq_num, wpan_backoff_period_boundary_get_(op_sim_time()+ Symbols2Sec(aTurnaroundTime_Symbol, WPAN_DATA_RATE)));
		//printf(" [%s (#%d)] t=%f  -> ACK requested - Sending of ACK packet [SEQ = %d] is scheduled at %f sec \n\n", device_name, my_address,op_sim_time(), seq_num, wpan_backoff_period_boundary_get_(op_sim_time()+ Symbols2Sec(aTurnaroundTime_Symbol, WPAN_DATA_RATE)));
	}
	
	if (SF_ptr->CAP_ACTIVE)
		ack_tx_time = wpan_backoff_period_boundary_get_(op_sim_time()+ Symbols2Sec(aTurnaroundTime_Symbol, WPAN_DATA_RATE));	/* send ACK on the backoff period boundary */
	else
		ack_tx_time = op_sim_time() + Symbols2Sec(aTurnaroundTime_Symbol, WPAN_DATA_RATE); /* not use backoff period boundary in the GTS */

	/* send back the ACK*/
	op_intrpt_schedule_self (ack_tx_time , ACK_SEND_CODE);
	
	/* Stack tracing exit point */
	FOUT;	
}


/*-----------------------------------------------------------------------------
 * Function:	wpan_extract_ack_frame
 *
 * Description:	check if the sequence number of the received ACK frame is the
 *				expected one.
 *
 * Input :  akc_frame - the MAC ACK frame
 *-----------------------------------------------------------------------------*/

static void wpan_extract_ack_frame(Packet* ack_frame) {

	int seq_num;
	Packet* mac_frame_dup;
	
	/* Stack tracing enrty point */
	FIN(wpan_extract_ack_frame);
	
	op_pk_nfd_get (ack_frame, "Sequence Number", &seq_num);
	
	/* if I'm waiting for an ACK */
	if (node_attributes.wait_ack == OPC_TRUE) {
		if (node_attributes.wait_ack_seq_num == seq_num) { /* yes, I have received my ACK */
			node_attributes.wait_ack = OPC_FALSE;
		
			/* disable the invocation of only the next interrupt of WAITING_ACK_END_CODE */
			op_intrpt_disable (OPC_INTRPT_SELF, WAITING_ACK_END_CODE, OPC_TRUE);
			
			if (SF_ptr->CAP_ACTIVE) {
				/* remove the duplicated mac_frame from subqueue */
				mac_frame_dup = op_subq_pk_remove (SUBQ_CAP, packet_to_send_index);
			
			} else {
				/* remove the duplicated mac_frame from GTS subqueue */
				mac_frame_dup = op_subq_pk_remove (SUBQ_GTS, packet_to_send_index);	
				
				op_stat_write (gts_statistic_vector.frame_delay, gts_pkt_delay);

				gts_statistics.frame_delay_sum = gts_statistics.frame_delay_sum + gts_pkt_delay;
				gts_statistics.frame_delay_max = max_double (gts_statistics.frame_delay_max, gts_pkt_delay);
				gts_statistics.frame_delay_min = min_double (gts_statistics.frame_delay_min, gts_pkt_delay);			
			}			
		
			/* send the mac_frame to higher layer for statistics */
			//op_pk_send (mac_frame_dup, STRM_FROM_MAC_TO_NWK); // TODO command statistics
			
			if (enable_log) {
				fprintf(log_file,"t=%f  -> ACK frame Reception [SEQ = %d], \n\t  Duplicate packet was removed at the head of subqueue \n\t  ACK waiting timer disabled. \n\n", op_sim_time(), seq_num);
			//	printf(" [%s (#%d)] t=%f  -> ACK frame Reception [SEQ = %d]\n\t  Duplicate packet was removed at the head of subqueue  \n\t  ACK waiting timer disabled. \n\n", device_name, my_address, op_sim_time(), seq_num);
			}

			/* Try to send another packet */
			op_intrpt_schedule_self (op_sim_time(), ACK_SUCCESS_CODE);
			
		} else	{	/* No, this is not my ACK, I'm Still Waiting */
			if (enable_log) {
				fprintf(log_file,"t=%f  -> WRONG ACK reception [SEQ = %d], still Waiting ACK [SEQ = %d] \n\n", op_sim_time(), seq_num , node_attributes.wait_ack_seq_num );
				//printf(" [%s (#%d)] t=%f  -> WRONG ACK reception [SEQ = %d], still Waiting ACK [SEQ = %d] \n\n", device_name, my_address, op_sim_time(), seq_num , node_attributes.wait_ack_seq_num );
			} 
		}
	} else {/* if (node_attributes.wait_ack == OPC_FALSE) */ 
/*		if (enable_log) {
			fprintf (log_file,"t=%f  -> I'm not Waiting ACK - ACK packet was destroyed. \n\n", op_sim_time());	
			printf (" [%s (#%d)] t=%f  -> I'm not Waiting ACK - ACK packet was destroyed. \n\n", device_name, my_address, op_sim_time());
		} */
	}
	
	/* destroy the ACK packet */
	op_pk_destroy (ack_frame);
	
	/* Stack tracing exit point */
	FOUT;	
}

/*--------------------------------------------------------------------------------
 * Function: wpan_data_frame_processing
 *
 *--------------------------------------------------------------------------------*/
/*
static void wpan_data_frame_processing (Packet* msdu, Boolean ack, int queue_idx) {
	
	Packet* frame_MPDU_ptr, frame_MSDU_ptr;
	int dest_nwk_addressA, dest_nwk_addressB;
	int i;
	int num_pkts;

	//Stack tracing enrty point 
	FIN(wpan_data_frame_processing);
	
	op_pk_nfd_get (msdu, "Destination Address", &dest_nwk_addressA);
	
	// check the subqueue 
	if (!op_subq_empty(queue_idx)) {
					
		// determine the number of packets in subqueue 
		num_pkts = op_subq_stat (SUBQ_CAP, OPC_QSTAT_PKSIZE);
					
		for (i=0; i<num_pkts; i++) {
		
			frame_MPDU_ptr = op_subq_pk_access (queue_idx, i);				
			op_pk_nfd_get_pkt (frame_MPDU_ptr, "MSDU", &frame_MSDU_ptr);
			op_pk_nfd_get (frame_MSDU_ptr, "Destination Address", &dest_nwk_addressB);
			

		}		
	} 

	// Stack tracing exit point
	FOUT;	
} */

/*--------------------------------------------------------------------------------
 * Function: wpan_encapsulate_and_enqueue_data_frame
 *
 * Description:	encapsulates data MSDU into a MAC frame and enqueues it. Try to send out it.      
 *             
 * Input:	msdu 			- MSDU (MAC Frame Payload)
 *			ack 			- TRUE if the acknowledgment is required, otherwise FALSE
 *			dest_address 	- the destionation MAC address for packet
 *			traffic_type 	- type of traffic (REAL_TIME_TRAFFIC and BEST_EFFORT_TRAFFIC)
 *--------------------------------------------------------------------------------*/

static void wpan_encapsulate_and_enqueue_data_frame (Packet* msdu, Boolean ack, int dest_address, int traffic_type) {
	
	Packet* mac_frame;	
	int seq_num;
	int packet_size;
	
	/* Stack tracing enrty point */
	FIN(wpan_encapsulate_and_enqueue_data_frame);
		
	/* create a MAC frame that encapsulates the MSDU and send it! */
	mac_frame = op_pk_create_fmt ("wpan_frame_MPDU_format");
		
	/* generate the sequence number for the packet */
	seq_num = wpan_update_sequence_number();
		
	op_pk_nfd_set (mac_frame, "Frame Type", DATA_FRAME_TYPE);
	op_pk_nfd_set (mac_frame, "Sequence Number",seq_num); 
	op_pk_nfd_set (mac_frame, "Destination PAN Identifier", node_attributes.pan_id);
	op_pk_nfd_set (mac_frame, "Destination Address", dest_address);
	op_pk_nfd_set (mac_frame, "Source PAN Identifier", node_attributes.pan_id);
	op_pk_nfd_set (mac_frame, "Source Address", my_address);
	op_pk_nfd_set (mac_frame, "Ack Request", ack);
	op_pk_nfd_set_pkt (mac_frame, "MSDU", msdu); // The MSDU is encapsulated in the MAC Data Frame
	///// hbi here i print the packet in the console output
	// op_pk_print (mac_frame);
	//// end hbi here is the error of dest_address
	packet_size = op_pk_total_size_get (mac_frame);
	
	if (traffic_type == BEST_EFFORT_TRAFFIC) {
	 	/* best-effort data traffic */
	/// send packet here /////////////////////////best effort/////////////////////////////
		/* put mac_frame packet into the tail queue waiting for transmission */
		if (op_subq_pk_insert(SUBQ_CAP, mac_frame, OPC_QPOS_TAIL) == OPC_QINS_OK) {
			if (enable_log) {
				fprintf (log_file,"t=%f  -> Enqueuing of best-effort (CAP) DATA packet (MPDU [size: %d bits]) [SEQ = %d, ACK = %s] \n\t + available CAP subqueue capacity: %.0f bits \n\n", op_sim_time(), packet_size, seq_num, boolean2string(ack), op_subq_stat (SUBQ_CAP, OPC_QSTAT_FREE_BITSIZE));
				//printf (" [%s (#%d)] t=%f  -> Enqueuing of best-effort (CAP) DATA packet (MPDU [size: %d bits]) [SEQ = %d, ACK = %s] \n\t + available CAP subqueue capacity: %.0f bits \n\n", device_name, my_address, op_sim_time(), packet_size, seq_num, boolean2string(ack), op_subq_stat (SUBQ_CAP, OPC_QSTAT_FREE_BITSIZE));
				#define btoa(x) ((x)?"true":"false")
			//	printf (" dest adress %d sfparams active in %s sfparams active out %s acknowledge %s packet to send index %d \n\n", dest_address, btoa(SF_params[SF_in].CAP_ACTIVE), btoa(SF_params[SF_out].CAP_ACTIVE), btoa(node_attributes.wait_ack), packet_to_send_index);
				}
			
			/* try to send the packet if CAP is activet or previous packet is not waiting for ACK */
			if ((((dest_address == my_parent_address) && (SF_params[SF_in].CAP_ACTIVE)) || ((dest_address != my_parent_address) && (SF_params[SF_out].CAP_ACTIVE))) && !node_attributes.wait_ack && (packet_to_send_index == NOT_DEFINED))
				op_intrpt_schedule_self (op_sim_time(), TRY_PACKET_TRANSMISSION_CODE);
			
		} else {
			if (enable_log) {
				fprintf (log_file,"t=%f  -> Best-effort (CAP) DATA packet  (MPDU [size: %d bits]) cannot be enqueuing  \n\t + CAP subqueue is full -> PACKET IS DROPPED !!!! \n\n", op_sim_time(), packet_size);
				//printf (" [%s (#%d)] t=%f  -> Best-effort (CAP) DATA packet (MPDU [size: %d bits]) cannot be enqueuing \n\t + CAP subqueue is full -> PACKET IS DROPPED !!!! \n\n", device_name, my_address, op_sim_time(), packet_size);
				
				queue_status ();
			}
		
			/* destroy the packet */
			op_pk_destroy (mac_frame);
		}		 
	
	
	} else {
		/* real-time data traffic */
	
		gts_statistics.stored_frames++;
		gts_statistics.stored_bits = gts_statistics.stored_bits + op_pk_total_size_get (msdu);
		
	/// send packet here /////////////////////////real-time/////////////////////////////
		/* put mac_frame packet into the  tail GTS queue waiting for transmission */
		if (op_subq_pk_insert (SUBQ_GTS, mac_frame, OPC_QPOS_TAIL) == OPC_QINS_OK) {
	
			if (enable_log) {
				fprintf (log_file,"t=%f  -> Enqueuing of real-time (GTS) DATA packet (MPDU [size: %d bits]) [SEQ = %d, ACK = %s] \n\t + available GTS subqueue capacity: %.0f bits \n\n", op_sim_time(), packet_size, seq_num, boolean2string(ack), op_subq_stat (SUBQ_GTS, OPC_QSTAT_FREE_BITSIZE));
				//printf (" [%s (#%d)] t=%f  -> Enqueuing of real-time (GTS) DATA packet (MPDU [size: %d bits]) [SEQ = %d, ACK = %s] \n\t + available GTS subqueue capacity: %.0f bits \n\n", device_name, my_address, op_sim_time(), packet_size, seq_num, boolean2string(ack), op_subq_stat (SUBQ_GTS, OPC_QSTAT_FREE_BITSIZE));
			}	
		
			op_stat_write (gts_statistic_vector.buffer_saturation_pc, (double)op_subq_stat (SUBQ_GTS, OPC_QSTAT_BITSIZE)/cfp_queue_capacity);
		
			/* (incoming superframe) try to send a packet if transmit GTS (child_node->parent) is active or the previous packet is not waiting for ACK or TX time is not active */
			if (SF_params[SF_in].CFP_ACTIVE && !node_attributes.wait_ack && (packet_to_send_index == NOT_DEFINED) && (GTS_ptr != OPC_NIL))
				if (GTS_ptr->direction == GTS_DIRECTION_TRANSMIT)
					op_intrpt_schedule_self (op_sim_time(), TRY_GTS_PACKET_TRANSMISSION_CODE);
			
			/* (outgoing superframe) try to send a packet if receive GTS (parent->child_node) is active or the previous packet is not waiting for ACK or TX time is not active */
			if (SF_params[SF_out].CFP_ACTIVE && !node_attributes.wait_ack && (packet_to_send_index == NOT_DEFINED) && (GTS_list_ptr != OPC_NIL)) {
				if (GTS_list_ptr->direction == GTS_DIRECTION_RECEIVE)
					op_intrpt_schedule_self (op_sim_time(), TRY_GTS_PACKET_TRANSMISSION_CODE);
			}
					
		
		} else {	
			if (enable_log) {
				fprintf (log_file,"t=%f  -> Real-time (GTS) DATA packet (MPDU [size: %d bits]) cannot be enqueuing \n\t + GTS subqueue is full -> PACKET IS DROPPED !!!! \n\n", op_sim_time(), packet_size);
				//printf (" [%s (#%d)] t=%f  -> Real-time (GTS) DATA packet (MPDU [size: %d bits]) cannot be enqueuing \n\t + GTS subqueue is full -> PACKET IS DROPPED !!!! \n\n", device_name, my_address, op_sim_time(), packet_size);
			}
		

			if (start_GTS_transmission == OPC_FALSE) {
				start_GTS_transmission = OPC_TRUE;	/* buffer is full, GTS transmission is allowed - initially full buffer */
			}
		
			gts_statistics.dropped_frames++;
			gts_statistics.dropped_bits = gts_statistics.dropped_bits + op_pk_total_size_get (mac_frame);
		
			/* destroy the packet */
			op_pk_destroy (mac_frame);	
		}
	}		

	
	/* Stack tracing exit point */
	FOUT;	
}


/*--------------------------------------------------------------------------------
 * Function: wpan_encapsulate_and_enqueue_command_frame
 *
 * Description:	encapsulates the MSDU into a MAC frame and enqueues it (send it to my parent).      
 *             
 * Input:	msdu - MSDU (MAC Frame payload)
 *			ack - TRUE if the acknowledgment is required, otherwise FALSE
 *			dest_address - the destionation MAC address for packet
 *--------------------------------------------------------------------------------*/

static void wpan_encapsulate_and_enqueue_command_frame (Packet* msdu, Boolean ack, int dest_address) {
	
	Packet* mac_frame;	
	int seq_num;
	
	/* Stack tracing enrty point */
	FIN(wpan_encapsulate_and_enqueue_command_frame);
		
	/* create a MAC frame that encapsulates the MSDU and send it! */
	mac_frame = op_pk_create_fmt ("wpan_frame_MPDU_format");
		
	/* generate the sequence number for the packet */
	seq_num = wpan_update_sequence_number();
		
	op_pk_nfd_set (mac_frame, "Frame Type", COMMAND_FRAME_TYPE);
	op_pk_nfd_set (mac_frame, "Sequence Number", seq_num); 
	op_pk_nfd_set (mac_frame, "Destination PAN Identifier", node_attributes.pan_id);
	op_pk_nfd_set (mac_frame, "Destination Address", dest_address);
	op_pk_nfd_set (mac_frame, "Source PAN Identifier", node_attributes.pan_id);
	op_pk_nfd_set (mac_frame, "Source Address", my_address);
	op_pk_nfd_set (mac_frame, "Ack Request", ack);
	op_pk_nfd_set_pkt (mac_frame, "MSDU", msdu); // The MSDU is encapsulated in the MAC Command Frame
		
	
	/* put it into the queue waiting for transmission */
	if (op_subq_pk_insert(SUBQ_CAP, mac_frame, OPC_QPOS_TAIL) == OPC_QINS_OK) {
		if (enable_log) {
			fprintf (log_file,"t=%f  -> Enqueuing of COMMAND packet [SEQ = %d, ACK = %s] and try to send \n\n", op_sim_time(), seq_num, boolean2string(ack));
			//printf (" [%s (#%d)] t=%f  -> Enqueuing of COMMAND packet [SEQ = %d, ACK = %s] and try to send. \n\n", device_name, my_address, op_sim_time(), seq_num, boolean2string(ack));
		}
		
		/* try to send the packet if CAP is active or previous packet is not waiting for ACK */
//		if (((dest_address == my_parent_address) && (SF_params[SF_in].CAP_ACTIVE)) || ((dest_address != my_parent_address) && (SF_params[SF_out].CAP_ACTIVE)) && !node_attributes.wait_ack && (packet_to_send_index == NOT_DEFINED))
		if ((  (SF_params[SF_in].CAP_ACTIVE)) || ((dest_address != my_parent_address) && (SF_params[SF_out].CAP_ACTIVE)) && !node_attributes.wait_ack && (packet_to_send_index == NOT_DEFINED))
			op_intrpt_schedule_self (op_sim_time(), TRY_PACKET_TRANSMISSION_CODE);
	} else {
		if (enable_log) {
			fprintf (log_file,"t=%f  -> COMMAND packet cannot be enqueuing - PACKET IS DROPPED !!!! \n\n", op_sim_time());
			//printf (" [%s (#%d)] t=%f  -> COMMAND packet cannot be enqueuing - PACKETv IS DROPPED !!!! \n\n", device_name, my_address, op_sim_time());
		}
		
		/* destroy the packet */
		op_pk_destroy (mac_frame);
	}
		 
	
	/* Stack tracing exit point */
	FOUT;	
}

/*--------------------------------------------------------------------------------
 * Function:	wpan_gts_scheduling
 *
 * Description:	checks the validity of times and schedules GTS start times and stop times.
 *
 * No parameters  
 *--------------------------------------------------------------------------------*/
 
static void wpan_gts_scheduling () {

	int i;

	/* Stack tracing enrty point */
	FIN(wpan_gts_scheduling);

	/* GTS schedule just for End Device and Router */
	if (!IAM_PAN_COORDINATOR) {
		
		for (i=0;i<2;i++) {
			/* Make sure we have valid start and stop times, i.e. stop time is not earlier than start time.	*/
			if ((GTS_params[i].stop_time <= GTS_params[i].start_time) && (GTS_params[i].stop_time != END_OF_SIMULATION)) {
			
				/* Stop time is earlier than start time. Disable the GTS. */
				GTS_params[i].start_time = START_DISABLED;

				/* Display an appropriate warning. */
				op_sim_error(OPC_SIM_ERROR_WARNING, "Warning - the GTS scheduling", "A start time must be before stop time. GTS was disabled."); 
			}
			
			/* Schedule a self interrupt. */
			if ((GTS_params[i].start_time != START_DISABLED) && (GTS_params[i].length > 0)) {
				if (i == GTS_DIRECTION_TRANSMIT) {
					if (GTS_params[i].start_time == 0.0) 				
						op_intrpt_schedule_self (GTS_params[i].start_time+epsilon, START_OF_TRANSMIT_GTS_USE);
					else 
						op_intrpt_schedule_self (GTS_params[i].start_time, START_OF_TRANSMIT_GTS_USE);
		
					if (GTS_params[i].stop_time != END_OF_SIMULATION)
						op_intrpt_schedule_self (GTS_params[i].stop_time, STOP_OF_TRANSMIT_GTS_USE);				
				} else {
					if (GTS_params[i].start_time == 0.0) 				
						op_intrpt_schedule_self (GTS_params[i].start_time+epsilon, START_OF_RECEIVE_GTS_USE);
					else 
						op_intrpt_schedule_self (GTS_params[i].start_time, START_OF_RECEIVE_GTS_USE);
		
					if (GTS_params[i].stop_time != END_OF_SIMULATION)
						op_intrpt_schedule_self (GTS_params[i].stop_time, STOP_OF_RECEIVE_GTS_USE);				
				}			

				if (enable_log) {
					if (GTS_params[i].stop_time == END_OF_SIMULATION) {
						fprintf (log_file,"t=%f  -> Using of GTS (in direction %d) is scheduled from %f sec (allocation) to END of SIMULATION (deallocation). \n\n", op_sim_time(), i, GTS_params[i].start_time);
					//	printf (" [%s (#%d)] t=%f  -> Using of GTS (in direction %d) is scheduled from %f sec (allocation) to END of SIMULATION (deallocation). \n\n", device_name, my_address, op_sim_time(), i, GTS_params[i].start_time);
					}
					else {
						fprintf (log_file,"t=%f  -> Using of GTS (in direction %d) is scheduled from %f sec (allocation) to %f sec (deallocation). \n\n", op_sim_time(), i, GTS_params[i].start_time, GTS_params[i].stop_time);
					//	printf (" [%s (#%d)] t=%f  -> Using of GTS (in direction %d) is scheduled from %f sec (allocation) to %f sec (deallocation). \n\n", device_name, my_address, op_sim_time(), i, GTS_params[i].start_time, GTS_params[i].stop_time);
					}
				}
			}
		}	
	}
	
	/* Stack tracing exit point */
	FOUT;	
}


/*--------------------------------------------------------------------------------
 * Function:	wpan_gts_start
 *
 * Description:	body for START_OF_TRANSMIT_GTS_USE and START_OF_RECEIVE_GTS_USE self-interrupts
 *
 * Input:
 *		int direction - GTS_DIRECTION_TRANSMIT (0) or GTS_DIRECTION_RECEIVE (1)
 *--------------------------------------------------------------------------------*/

static void wpan_gts_start (int direction) {

	/* Stack tracing enrty point */
	FIN(wpan_gts_start);
	
	switch(node_attributes.my_parent_GTSPermit) {					
		case NOT_DEFINED: /* wait for first beacon and then check GTS permit again */
		{
			waiting_for_first_beacon = OPC_TRUE;
						
			if (enable_log) {
				fprintf (log_file,"t=%f  ->  GTS allocation request was postponed until the first beacon reception from my parent. <-   \n\n", op_sim_time());
				//printf (" [%s (#%d)] t=%f  ->  GTS allocation request was postponed until the first beacon reception from my parent. <-  \n\n", device_name, my_address, op_sim_time());
			}
						
			break;
		};
		case OPC_FALSE: /* My parent does not accept GTS request */
		{
			if (enable_log) {
				fprintf (log_file,"t=%f  ->  My parent (#%d) does not permit GTS allocation request (GTS Permit = disabled). GTS cannot be use. <-   \n\n", op_sim_time(), my_parent_address);
				//printf (" [%s (#%d)] t=%f  ->  My parent (#%d) does not permit GTS allocation request (GTS Permit = disabled). GTS cannot be use. <-  \n\n", device_name, my_address, op_sim_time(), my_parent_address);
			}
							
			break;
		};
		case OPC_TRUE:  /* My parent accepts GTS request */
		{
			wpan_gts_allocation_request (direction);	
						
			if (enable_log) {
				fprintf (log_file,"t=%f  ->  START OF GTS USING (GTS allocation request) - in direction %d (transmit=0, receive=1) <-   \n\n", op_sim_time(), direction);
				//printf (" [%s (#%d)] t=%f  ->  START OF GTS USING (GTS allocation request) - in direction %d (transmit=0, receive=1) <-  \n\n", device_name, my_address, op_sim_time(), direction);
			}
							
			break;
		};
		default : 
		{
		};
	}
	
	/* Stack tracing exit point */
	FOUT;	
}

/*--------------------------------------------------------------------------------
 * Function:	wpan_gts_stop
 *
 * Description:	body for STOP_OF_TRANSMIT_GTS_USE and STOP_OF_RECEIVE_GTS_USE self-interrupts
 *
 * Input:
 *		int direction - GTS_DIRECTION_TRANSMIT (0) or GTS_DIRECTION_RECEIVE (1)
 *--------------------------------------------------------------------------------*/

static void wpan_gts_stop (int direction) {

	/* Stack tracing enrty point */
	FIN(wpan_gts_stop);
	
	if (node_attributes.my_parent_GTSPermit) {
		wpan_gts_deallocation_request (direction);
					
		if (enable_log) {
			fprintf (log_file,"t=%f  -> STOP OF GTS USING (GTS deallocation request) - in direction %d (transmit=0, receive=1) <-   \n\n", op_sim_time(), direction);
			//printf (" [%s (#%d)] t=%f  -> STOP OF GTS USING (GTS deallocation request) - in direction %d (transmit=0, receive=1) <-  \n\n", device_name, my_address, op_sim_time(), direction);
		}
	}
	
	/* Stack tracing exit point */
	FOUT;	
}

/*--------------------------------------------------------------------------------
 * Function:	wpan_gts_allocation_request
 *
 * Description:	creates GTS Request command for GTS allocation. It will be send to my parent (router or PAN coordinator).
 *
 * Input:
 *		int direction - GTS_DIRECTION_TRANSMIT (0) or GTS_DIRECTION_RECEIVE (1)
 *--------------------------------------------------------------------------------*/

static void wpan_gts_allocation_request (int direction) {

	Packet* command_MSDU;
	wpan_gts_characteristics* gts_characteristics_ptr;
	
	/* Stack tracing enrty point */
	FIN(wpan_gts_allocation_request);
	
	/* convert number of bits to actual number of superframe slots */
	//nbr_slots = ceil(ceil(GTS.length/Symbol2Bits)/SF_params[SF_in].slot_duration);

	/* memory allocation */
	gts_characteristics_ptr = (wpan_gts_characteristics *) op_prg_mem_alloc (sizeof(wpan_gts_characteristics));
	
	gts_characteristics_ptr->length = GTS_params[direction].length;
	gts_characteristics_ptr->direction = direction;
	gts_characteristics_ptr->characteristics_type = GTS_ALLOCATION;	
		
	/* waiting for the reply in the beacon frame from the my parent */
	GTS_params[direction].is_waiting_GTS_confirm = OPC_TRUE;	

	
	/* create GTS allocation request command (MSDU) */
	command_MSDU = op_pk_create_fmt ("wpan_command_MSDU_format");	
	op_pk_nfd_set (command_MSDU, "Command Frame Identifier", GTS_REQ);
	op_pk_nfd_set_ptr (command_MSDU, "Command Payload", gts_characteristics_ptr, op_prg_mem_copy_create, op_prg_mem_free, sizeof (wpan_gts_characteristics));
	
	/* set the appropriate size of the payload according the specifiaction */
	op_pk_bulk_size_set (command_MSDU, 8); // the size of the GTS characteristics field is 8 bits
	
	if (enable_log) {
		fprintf(log_file,"t=%f  -> GTS ALLOCATION - GTS allocation request command was created [GTS_length = %d slots, GTS_direction = %d]. \n\n", op_sim_time(), gts_characteristics_ptr->length, gts_characteristics_ptr->direction);
		//printf(" [%s (#%d)] t=%f  -> GTS ALLOCATION - GTS allocation request command was created [GTS_length = %d slots, GTS_direction = %d]. \n\n", device_name, my_address, op_sim_time(), gts_characteristics_ptr->length, gts_characteristics_ptr->direction);
	}
	
	/* encapsulate and enqueue GTS request command frame */
	wpan_encapsulate_and_enqueue_command_frame (command_MSDU, OPC_TRUE, my_parent_address);	
	
	/* Stack tracing exit point */
	FOUT;	
}


/*--------------------------------------------------------------------------------
 * Function:	wpan_gts_deallocation_request
 *
 * Description:	creates GTS Request command for GTS deallocation. It will be send to my parent.
 *
 * Input:
 *		int direction - GTS_DIRECTION_TRANSMIT (0) or GTS_DIRECTION_RECEIVE (1)  
 *--------------------------------------------------------------------------------*/

static void wpan_gts_deallocation_request (int direction) {

	Packet* command_frame;
	wpan_gts_characteristics* gts_characteristics_ptr;
	
	/* Stack tracing enrty point */
	FIN(wpan_gts_deallocation_request);

	/* convert number of bits to actual number of superframe slots */
	//nbr_slots = ceil(ceil(GTS.length/Symbol2Bits)/SF_params[SF_in].slot_duration);

	/* memory allocation */
	gts_characteristics_ptr = (wpan_gts_characteristics *) op_prg_mem_alloc (sizeof(wpan_gts_characteristics));
	

	gts_characteristics_ptr->length = GTS_params[direction].length;
	gts_characteristics_ptr->direction = direction;
	gts_characteristics_ptr->characteristics_type = GTS_DEALLOCATION;
		
	GTS_params[direction].is_deallocation_req_sent = OPC_TRUE;	

	
	/* create a GTS request COMMAND frame */
	command_frame = op_pk_create_fmt ("wpan_command_MSDU_format");	
	op_pk_nfd_set (command_frame, "Command Frame Identifier", GTS_REQ);
	op_pk_nfd_set_ptr (command_frame, "Command Payload", gts_characteristics_ptr, op_prg_mem_copy_create, op_prg_mem_free, sizeof (wpan_gts_characteristics));
	
	/* set the appropriate size of the payload according the specifiaction */
	op_pk_bulk_size_set (command_frame, 8); // the size of the GTS characteristics field is 8 bits
	
	if (enable_log) {
		fprintf(log_file,"t=%f  -> --- GTS DEALLOCATION - GTS deallocation request command was created [GTS_length=%d slots, GTS_direction=%d]. \n\n", op_sim_time(), gts_characteristics_ptr->length, gts_characteristics_ptr->direction);
		//printf(" [%s (#%d)] t=%f  -> --- GTS DEALLOCATION - GTS deallocation request command was created [GTS_length=%d slots, GTS_direction=%d]. \n\n", device_name, my_address, op_sim_time(), gts_characteristics_ptr->length, gts_characteristics_ptr->direction);
	}
	
	/* encapsulate and enqueue GTS request command frame */
	wpan_encapsulate_and_enqueue_command_frame (command_frame, OPC_TRUE, my_parent_address);	
	
	/* Stack tracing exit point */
	FOUT;
}

/*--------------------------------------------------------------------------------
 * Function:	wpan_gts_reallocation_request
 *
 * Description:	update GTS_list list i.e. remove gaps from CFP.
 *
 * No parameters    
 *--------------------------------------------------------------------------------*/

static void wpan_gts_reallocation_request() {

	wpan_gts_list_record * gts_list_record_ptr; /* GTS structure storing in the GTS_list */
	int i;
	int last_start_slot = TIME_SLOT_INDEX_MAX+1;
	int changes_count = 0;
	
	/* Stack tracing enrty point */
	FIN(wpan_gts_reallocation_request);	
	
	for (i=(op_prg_list_size (node_attributes.GTS_list)-1); i>=0; i--) {

		gts_list_record_ptr = (wpan_gts_list_record *) op_prg_list_access (node_attributes.GTS_list, i);		
	
		if ((gts_list_record_ptr->start_slot == 0) && (gts_list_record_ptr->beacon_persistence_time == 0)) {
			/* remove information record for the unallocated GTS */
			op_prg_list_remove (node_attributes.GTS_list, i);
			changes_count = changes_count + 1;
		} else if (gts_list_record_ptr->start_slot >0) {
			if (gts_list_record_ptr->start_slot != (last_start_slot - gts_list_record_ptr->length)) {
				/* update start slot */
				gts_list_record_ptr->start_slot = last_start_slot - gts_list_record_ptr->length;
				
				/* propagate changes in the following beacons */
				gts_list_record_ptr->beacon_persistence_time = aGTSDescPersistenceTime;	
				
				changes_count = changes_count + 1;
			} 
			
			last_start_slot = gts_list_record_ptr->start_slot;
		}
	}
	
	/* update Final_CAP_slot value for next superframe*/
	SF_params[SF_out].Final_CAP_Slot_next = last_start_slot-1;
	
	if (enable_log && (changes_count>0)) {
		fprintf(log_file,"t=%f  -> GTS realocation - GTS list was updated (%d changes) - Final CAP slot is %d. \n\n", op_sim_time(), changes_count, SF_params[SF_out].Final_CAP_Slot_next);
		//printf(" [%s (#%d)] t=%f  -> GTS realocation - GTS list was updated (%d changes)- Final CAP slot is %d. \n\n", device_name, my_address, op_sim_time(), changes_count, SF_params[SF_out].Final_CAP_Slot_next);
	}
	
	/* Stack tracing exit point */
	FOUT;
}


/*--------------------------------------------------------------------------------
 * Function:	wpan_gts_request_extract
 *
 * Description:	extract incoming GTS request command and allocate appropriate GTS if
 *				there are available resources in actual superframe.
 *
 * Input :	mac_frame - received GTS request command 
 *--------------------------------------------------------------------------------*/

static void wpan_gts_request_extract (Packet* mac_frame) {

	Packet* command_frame;
	Objid NeIntelid;///////////added
	wpan_gts_characteristics* gts_characteristics_ptr;	// GTS structure receiving from the command frame
	wpan_gts_list_record * gts_list_record_ptr; // GTS structure storing in the GTS_list
	int source_address;
	int min_space; // [symbols]
	int i;
	
	/* Stack tracing enrty point */
	FIN(wpan_gts_request_extract);
	
	/* obtain packet source address */
	op_pk_nfd_get (mac_frame, "Source Address", &source_address);
	
	/* obtain GTS characteristics field (Command Payload) */
	op_pk_nfd_get_pkt (mac_frame, "MSDU", &command_frame);
	
	/* obtain GTS characteristics values */
	op_pk_nfd_get (command_frame, "Command Payload", &gts_characteristics_ptr);		
	
	/* minimum superframe space which must stay unused */
	min_space = aMinCAPLength + (PHY_HEADER_SIZE+BEACON_HEADER_SIZE+BEACON_MSDU_SIZE)/Symbol2Bits; /* [symbols] */
	
	if (gts_characteristics_ptr->characteristics_type == GTS_ALLOCATION) { /* allocation of GTS slot(s) */	
		
		
		        /* Send a request to NeIntel about the curent gts request source address Pan Sink, other info should be known */
						
				// added to test the network intel process procedures
				NeIntelid = op_id_from_name ( parent_id, OPC_OBJTYPE_PROC, "OpenFlow Network Intelligence");
				op_ima_obj_attr_set (NeIntelid, "PAN Sink Address",  PANsink_mac_address);
				op_ima_obj_attr_set (NeIntelid, "Gts Source Address", source_address);
				//op_ima_obj_state_set (NeIntelid, &source_address);
				op_intrpt_schedule_remote (op_sim_time(), GTS_request_to_NeIntel, NeIntelid);
			
			
	
	
	/* check the available space in the GTS list */
		if (op_prg_list_size (node_attributes.GTS_list) >= 7) {	// maximum 7 GTSs can be allocated
			if (enable_log == OPC_TRUE) {
				fprintf (log_file, "t=%f  -> Received GTS request command for ALLOCATION from node #%d \n\t !!! GTS cannot be allocated because maximum number (7) of GTSs has already been reached.\n\n", op_sim_time(), source_address);
			//	printf (" [%s (#%d)] t=%f  -> Received GTS request command for ALLOCATION from node #%d \n\t !!! GTS cannot be allocated because maximum number (7) of GTSs has already been reached.\n\n", device_name, my_address, op_sim_time(), source_address);
			}		
		} else { /* the size of GTS_list < 7 */
			if (((SF_params[SF_out].Final_CAP_Slot_next-gts_characteristics_ptr->length+1)*SF_params[SF_out].slot_duration) < min_space) {	/* not enough space in superframe CAP */
				
				/* memory allocation */
				gts_list_record_ptr = (wpan_gts_list_record *) op_prg_mem_alloc (sizeof (wpan_gts_list_record));
				
				gts_list_record_ptr->start_slot = 0; 
				gts_list_record_ptr->length = SF_params[SF_out].Final_CAP_Slot_next - (int) ceil((double) min_space/SF_params[SF_out].slot_duration); // maximum available number of slots
				gts_list_record_ptr->direction = gts_characteristics_ptr->direction;
				gts_list_record_ptr->assoc_dev_address = source_address;				
				gts_list_record_ptr->beacon_persistence_time = aGTSDescPersistenceTime;	// propagate changes in the following beacons 
				gts_list_record_ptr->retries_nbr = 0;
				
				/* insert the GTS parameter record onto the list head */
				op_prg_list_insert (node_attributes.GTS_list, gts_list_record_ptr, OPC_LISTPOS_HEAD);				
			
				if (enable_log == OPC_TRUE) {
					fprintf (log_file, "t=%f  -> Received GTS request command for ALLOCATION from node #%d \n\t !!! GTS cannot be allocated because there is not enough space in CAP [only %d available slot(s)].\n\n", op_sim_time(), source_address, gts_list_record_ptr->length);
				//	printf (" [%s (#%d)] t=%f  -> Received GTS request command for ALLOCATION from node #%d \n\t !!! GTS cannot be allocated because there is not enough space in CAP [only %d available slot(s)].\n\n", device_name, my_address, op_sim_time(), source_address, gts_list_record_ptr->length);
				}
			} else { /* GTS can be allocated */
				
				/* update Final_CAP_slot value for next superframe*/
				SF_params[SF_out].Final_CAP_Slot_next = SF_params[SF_out].Final_CAP_Slot_next-gts_characteristics_ptr->length;
				
				/* memory allocation */
				gts_list_record_ptr = (wpan_gts_list_record *) op_prg_mem_alloc (sizeof (wpan_gts_list_record));
				
				gts_list_record_ptr->start_slot = SF_params[SF_out].Final_CAP_Slot_next+1;
				gts_list_record_ptr->length = gts_characteristics_ptr->length;
				gts_list_record_ptr->direction = gts_characteristics_ptr->direction;
				gts_list_record_ptr->assoc_dev_address = source_address;
				gts_list_record_ptr->beacon_persistence_time = aGTSDescPersistenceTime;	// propagate changes in the following beacons 
				
				/* insert the GTS parameter record onto the list head */
				op_prg_list_insert (node_attributes.GTS_list, gts_list_record_ptr, OPC_LISTPOS_HEAD);
				
				if (enable_log) {									
				/*	
				printf (" [%s (#%d)] t=%f  ->  Received GTS request command for ALLOCATION \n", device_name, my_address, op_sim_time());
					printf ("\t GTS slots for node #%d can be allocated - there are available resources in the actual superframe. \n ", gts_list_record_ptr->assoc_dev_address);
					printf ("\t\t + Start slot       = %d \n", gts_list_record_ptr->start_slot); 
					printf ("\t\t + Length of GTS    = %d slots \n", gts_list_record_ptr->length);
					printf ("\t\t + Direction of GTS = %d (transmit=0, receive=1) \n\n", gts_list_record_ptr->direction);
					printf ("\t Number of GTSs = %d \n", op_prg_list_size (node_attributes.GTS_list));
					printf ("\t Final CAP slot = %d \n\n", SF_params[SF_out].Final_CAP_Slot_next);
					*/
					fprintf (log_file, "t=%f  ->  Received GTS request command for ALLOCATION \n", op_sim_time());
					fprintf (log_file, "\t GTS slots for node #%d can be allocated - there are available resources in the actual superframe. \n ", gts_list_record_ptr->assoc_dev_address);
					fprintf (log_file, "\t\t + Start slot       = %d \n", gts_list_record_ptr->start_slot); 
					fprintf (log_file, "\t\t + Length of GTS    = %d slots \n", gts_list_record_ptr->length);
					fprintf (log_file, "\t\t + Direction of GTS = %d (transmit=0, receive=1) \n\n", gts_list_record_ptr->direction);
					fprintf (log_file, "\t Number of GTSs = %d \n", op_prg_list_size (node_attributes.GTS_list));
					fprintf (log_file, "\t Final CAP slot = %d \n\n", SF_params[SF_out].Final_CAP_Slot_next);
				}				
			}
		}	
	} else { /* deallocation of GTS slot(s) */
		
		if (enable_log) {
			fprintf (log_file,"t=%f  -> Received GTS deallocation request command from node #%d. \n\n", op_sim_time(), source_address);
			//printf (" [%s (#%d)] t=%f  -> Received GTS deallocation request command from node #%d. \n\n", device_name, my_address, op_sim_time(), source_address);
		}
		
		/* find the appropriate GTS_list record for given device */
		for (i=0; i<op_prg_list_size (node_attributes.GTS_list); i++) {
			gts_list_record_ptr = (wpan_gts_list_record *) op_prg_list_access (node_attributes.GTS_list, i);
			
			/* check the device GTS charackteristics from the received request with the GTS_list record - there can be 2 GTSs for each device (transmit, receive) */
			if ((gts_list_record_ptr->assoc_dev_address == source_address) && (gts_list_record_ptr->direction == gts_characteristics_ptr->direction)) {
				
				/* remove the record from the GTS_list. */
				if (op_prg_list_remove (node_attributes.GTS_list, i) != OPC_NIL) {
					/* update Final_CAP_slot value for next superframe */
					SF_params[SF_out].Final_CAP_Slot_next = SF_params[SF_out].Final_CAP_Slot_next+gts_list_record_ptr->length;
					
					if (enable_log) {
						fprintf (log_file,"t=%f  -> The GTS record (Address: %d, Length: %d, Direction: %d) was successfully removed from the list. \n", op_sim_time(), gts_list_record_ptr->assoc_dev_address, gts_list_record_ptr->length, gts_list_record_ptr->direction);
						fprintf (log_file,"\t  + updated Final_CAP_slot = %d \n\n", SF_params[SF_out].Final_CAP_Slot_next);
						
						//printf (" [%s (#%d)] t=%f  -> The GTS record (Address: %d, Length: %d, Direction: %d) was successfully removed from the list. \n", device_name, my_address, op_sim_time(), gts_list_record_ptr->assoc_dev_address, gts_list_record_ptr->length, gts_list_record_ptr->direction);;
						//printf ("\t  + updated Final_CAP_slot = %d \n\n", SF_params[SF_out].Final_CAP_Slot_next);
					}
					
					/* update the GTS_list */
					wpan_gts_reallocation_request ();				
				} else {
					op_sim_error(OPC_SIM_ERROR_WARNING, "Warning - wpan_gts_request_extract", "The appropriate record from GTS_list cannot be remove!");
				}		
			}
		}				
	}
	
	/* memory deallocation */
	op_prg_mem_free (gts_characteristics_ptr);
	
	/* destroy the packets */
	op_pk_destroy (command_frame);
	
	/* Stack tracing exit point */
	FOUT;
}


/*--------------------------------------------------------------------------------
 * Function: wpan_gts_slot_init
 *
 * Description:	initialization of the GTS transmission slot      
 *             
 * No parameters  
 *--------------------------------------------------------------------------------*/
static void wpan_gts_slot_init() {

	/* Stack tracing enrty point */
	FIN(wpan_gts_slot_init);

	if (enable_log) {	 
		fprintf(log_file,"t=%f  ->  GTS subqueue contains %.0f packets (MPDU) and  %.0f bits. \n\n", op_sim_time(), op_subq_stat (SUBQ_GTS, OPC_QSTAT_PKSIZE), op_subq_stat (SUBQ_GTS, OPC_QSTAT_BITSIZE));
		//printf(" [%s (#%d)] t=%f  -> GTS subqueue contains  %.0f packets (MPDU) and  %.0f bits. \n\n", device_name, my_address, op_sim_time(), op_subq_stat (SUBQ_GTS, OPC_QSTAT_PKSIZE), op_subq_stat (SUBQ_GTS, OPC_QSTAT_BITSIZE));
	}
	
	/* Stack tracing exit point */
	FOUT;
}

/*--------------------------------------------------------------------------------
 * Function: wpan_gts_check_time
 *
 * Description:	check if the requested transaction can be completed before the end of the current current GTS  
 *					|PHY|FRAME|-||macAckWaitDuration + |PHY|ACK||-|IFS| or
 *					|PHY|FRAME|-|IFS| or
 *             
 * Input: frame_MPDU - the pointer to the frame (MPDU) which should be send
 *
 * Output: TRUE if there is enough time in the actual GTS period
 *
 *--------------------------------------------------------------------------------*/

static Boolean wpan_gts_check_time(Packet* frame_MPDU) {

	int frame_len_Symbol;
	int IFS;
	Boolean ack_request;
	double end_gts_time;
	double remaining_gts_time;
	double tx_time;
	double ack_time_Symbol;
	
	/* Stack tracing enrty point */
	FIN(wpan_gts_check_time);

	/* get the frame size*/
	frame_len_Symbol = Bits2Symbols((int) op_pk_total_size_get(frame_MPDU));
	
	/* get the corresponding IFS of the frame*/
	IFS = wpan_ifs_symbol_get(frame_MPDU);
	
	/* check if any acknowledgment is necessary for this frame */
	op_pk_nfd_get (frame_MPDU, "Ack Request", &ack_request);
	
	if (ack_request)
		ack_time_Symbol = macAckWaitDuration;
		//ack_time_Symbol = aTurnaroundTime_Symbol+Bits2Symbols(ACK_FRAME_SIZE_BITS)+Bits2Symbols(PHY_HEADER_SIZE);
	else 
		ack_time_Symbol = 0;	
	
	
	/* time of the end of the GTS */
	if (GTS_ptr != OPC_NIL) {
		end_gts_time = SF_ptr->BI_Boundary + Symbols2Sec((GTS_ptr->start_slot*SF_ptr->slot_duration), WPAN_DATA_RATE) + Symbols2Sec((GTS_ptr->length*SF_ptr->slot_duration), WPAN_DATA_RATE);
	} else {
		end_gts_time = SF_ptr->BI_Boundary + Symbols2Sec((GTS_list_ptr->start_slot*SF_ptr->slot_duration), WPAN_DATA_RATE) + Symbols2Sec((GTS_list_ptr->length*SF_ptr->slot_duration), WPAN_DATA_RATE);
	}	
	
	remaining_gts_time = end_gts_time - op_sim_time();
	
	/* |PHY|FRAME|-||macAckWaitDuration + |PHY|ACK||-|IFS| or |PHY|FRAME|-|IFS|  */
	tx_time = Symbols2Sec((Bits2Symbols(PHY_HEADER_SIZE)+frame_len_Symbol+ack_time_Symbol+IFS), WPAN_DATA_RATE);
	
	/* check the time for packet transmission */
	if (tx_time <= remaining_gts_time) {	
		return OPC_TRUE;
		
		/* Stack tracing exit point */
		FRET(OPC_TRUE);
	}
	else { 	
		return OPC_FALSE;
		
		/* Stack tracing exit point */
		FRET(OPC_FALSE);
	}
}


/*--------------------------------------------------------------------------------
 * Function: wpan_mac_interrupt_process
 *
 * Description:	processes all the interrupts that occurs in an unforced state      
 *             
 * No parameters  
 *--------------------------------------------------------------------------------*/

static void wpan_mac_interrupt_process() {
	
	Packet* frame_MPDU;
	Packet* frame_MPDU_copy;
	Packet* frame_MPDU_to_send;
	Packet* frame_PPDU;
	Packet* ack_MPDU;
	Packet* frame_MSDU;
	double tx_time;
	double throughput;
	int ack_request;
	int seq_num;
	int dest_address;
	int i,j;
	
	/* Stack tracing enrty point */
	FIN(wpan_mac_interrupt_process);
	
	switch (op_intrpt_type()) {
	
		case OPC_INTRPT_STRM: // incomming packet 
		{			
			wpan_parse_incoming_frame ();	// parse the incoming packet		
			break;
		};/*end of OPC_INTRPT_STRM */	
	
		case OPC_INTRPT_SELF: 
		{			
			switch (op_intrpt_code()) { /* begin switch (op_intrpt_code()) */ 			
				case BEACON_INTERVAL_CODE: /* Beacon Interval Expiration - end of sleep period */
				{
				
					if (enable_log) {
						fprintf (log_file,"t=%f  -> ++++++++++ END OF SLEEP PERIOD ++++++++++ \n\n", op_sim_time());
						//printf (" [%s (#%d)] t=%f  -> ++++++++++  END OF SLEEP PERIOD ++++++++++ \n\n", device_name, my_address, op_sim_time());
					}
					
					SF_ptr = OPC_NIL;
					
					SF_params[SF_out].Final_CAP_Slot = SF_params[SF_out].Final_CAP_Slot_next; /* value for the current superframe. */
					
					if (!IAM_END_DEVICE) {	
						wpan_send_beacon_frame();	/* send next beacon for synchronization */					
					}
					break;
				};/*end of BEACON_INTERVAL_CODE */
			
				case START_OF_CAP_PERIOD_CODE: /* start of Contention Access Period (CAP) */
				{

					if (SF_ptr == OPC_NIL) {
					//	printf (" [%s (#%d)] t=%f  ->ERROR: Superframe pointer (SF_ptr) is empty (OPC_NIL) \n\n", device_name, my_address, op_sim_time());
						op_sim_end("ERROR - START_OF_CAP_PERIOD_CODE","Superframe pointer (SF_ptr) is empty (OPC_NIL)","","");
					}
					
					SF_ptr->current_slot = 0;
					SF_ptr->CAP_ACTIVE = OPC_TRUE;
					SF_ptr->CFP_ACTIVE = OPC_FALSE;
					SF_ptr->SLEEP = OPC_FALSE;
					packet_to_send_index = NOT_DEFINED;
					
					if (enable_log) {
						if (SF_ptr == &SF_params[SF_in]) {
							fprintf (log_file,"t=%f  -> ++++++++++ START OF INCOMING CAP (parent's cluster) ++++++++++ \n\n", op_sim_time());
						//	printf (" [%s (#%d)] t=%f  -> ++++++++++  START OF INCOMING CAP (parent's cluster) ++++++++++ \n\n", device_name, my_address, op_sim_time());
						} else {
							fprintf (log_file,"t=%f  -> ++++++++++ START OF OUTGOING CAP (my own cluster) ++++++++++ \n\n", op_sim_time());
						//	printf (" [%s (#%d)] t=%f  -> ++++++++++  START OF OUTGOING CAP (my own cluster) ++++++++++ \n\n", device_name, my_address, op_sim_time());
						}
					} 
					
					
				
					if (SF_ptr->csma.RESUME_BACKOFF_TIMER == OPC_TRUE) {	
						/* if a backoff timer was paused during the previous CAP, it is resumed in this CAP*/
						if (enable_log) {
							fprintf (log_file,"t=%f  -> RESUME A BACKOFF TIMER [%f] + BACKOFF BOUNDARY [%f] = EXPIRE AT %f sec  %f\n\n", op_sim_time(), SF_ptr->csma.backoff_timer, wpan_backoff_period_boundary_get(), wpan_backoff_period_boundary_get() + SF_ptr->csma.backoff_timer);
						//	printf (" [%s (#%d)] t=%f  ->  RESUME A BACKOFF TIMER [%f] + BACKOFF BOUNDARY [%f] = EXPIRE AT %f sec    \n\n", device_name, my_address, op_sim_time(), SF_ptr->csma.backoff_timer, wpan_backoff_period_boundary_get(), wpan_backoff_period_boundary_get() + SF_ptr->csma.backoff_timer);
						}
						
						/* is there any packet in the CAP subqueue which can be send in this superframe*/
						packet_to_send_index = wpan_CAP_packet_search ();
					
						if (packet_to_send_index == NOT_DEFINED) {
						//	printf (" [%s (#%d)] t=%f  -> ERROR: The backoff timer resumes but there is no packet for sending in the subqueue! \n\n", device_name, my_address, op_sim_time());
							op_sim_end("ERROR - START_OF_CAP_PERIOD_CODE","No packet for sending in the subqueue!","","");							
						}				
						
						op_intrpt_schedule_self (op_sim_time(), BACKOFF_RESUME_CODE); /* go to 'backoff_timer' state */
						op_intrpt_schedule_self (wpan_backoff_period_boundary_get() + SF_ptr->csma.backoff_timer, BACKOFF_EXPIRATION_CODE);						
						
						SF_ptr->csma.RESUME_BACKOFF_TIMER = OPC_FALSE;
						SF_ptr->csma.backoff_timer = 0;
						
					} else if (SF_ptr->csma.CCA_DEFERRED == OPC_TRUE) {
						/* if a CCA is deferred, it shall be resumed at the begining of the CAP of the superframe*/
						if (enable_log) {
							fprintf (log_file,"t=%f  -> RESUME THE CCA OPERATION (CCA WAS DEFERRED IN LAST CAP)  \n\n", op_sim_time());
						//	printf(" [%s (#%d)] t=%f  -> RESUME THE CCA OPERATION (CCA WAS DEFERRED IN LAST CAP)   \n\n", device_name, my_address, op_sim_time());
						}
						
						/* is there any packet in the CAP subqueue which can be send in this superframe */
						packet_to_send_index = wpan_CAP_packet_search ();
					
						if (packet_to_send_index == NOT_DEFINED) {
						//	printf (" [%s (#%d)] t=%f  -> ERROR: The CCA resumes but there is no packet for sending in the subqueue! \n\n", device_name, my_address, op_sim_time());
							op_sim_end("ERROR - START_OF_CAP_PERIOD_CODE","No packet for sending in the subqueue!","","");							
						}	
						
						op_intrpt_schedule_self (op_sim_time(), CCA_RESUME_CODE); /* go to 'CCA' state */
					
						SF_ptr->csma.CCA_DEFERRED = OPC_FALSE;
						wpan_cca_perform (3);
					} else {			
						op_intrpt_schedule_self (op_sim_time(), TRY_PACKET_TRANSMISSION_CODE);	
					}
					break;
				};/* end of START_OF_CAP_PERIOD_CODE */
			
				case END_OF_CAP_PERIOD_CODE:	/* end of current CAP */	
				{
// printf("AAAA [%s (#%d)] t=%f priority END_OF_CAP %d, BEACON %d \n\n", device_name, my_address, op_sim_time(), op_intrpt_priority_get (OPC_INTRPT_SELF, END_OF_CAP_PERIOD_CODE), op_intrpt_priority_get (OPC_INTRPT_SELF, BEACON_INTERVAL_CODE));
					
					// SF_ptr->current_slot = SF_ptr->Final_CAP_Slot;
					SF_ptr->CAP_ACTIVE = OPC_FALSE;								
										
					/* the end of CAP is the end of the superframe -> enter into sleep state*/
					if (SF_ptr->Final_CAP_Slot == TIME_SLOT_INDEX_MAX) { 
						SF_ptr->CFP_ACTIVE = OPC_FALSE;
						SF_ptr->SLEEP = OPC_TRUE;
						SF_ptr->current_slot = TIME_SLOT_INDEX_MAX;
						
						if (enable_log) {
							if (SF_ptr == &SF_params[SF_in]) {
								fprintf (log_file,"t=%f  -> ++++++++++ END OF INCOMING CAP PERIOD - GOING INTO SLEEP PERIOD ++++++++++ \n\n", op_sim_time());
							//	printf (" [%s (#%d)] t=%f  -> ++++++++++  END OF INCOMING CAP PERIOD - GOING INTO SLEEP PERIOD ++++++++++ \n\n", device_name, my_address, op_sim_time());
							} else {
								fprintf (log_file,"t=%f  -> ++++++++++ END OF OUTGOING CAP PERIOD - GOING INTO SLEEP PERIOD ++++++++++ \n\n", op_sim_time());
							//	printf (" [%s (#%d)] t=%f  -> ++++++++++  END OF OUTGOING CAP PERIOD - GOING INTO SLEEP PERIOD ++++++++++ \n\n", device_name, my_address, op_sim_time());							
							}
						}	
						
						/*Inform the Battery Module that the node enters the SLEEP state*/
						op_intrpt_schedule_remote (op_sim_time(), END_OF_ACTIVE_PERIOD_CODE, baterry_id);
						
						SF_ptr = OPC_NIL;
						
					} else { /* CFP follows after CAP */
						SF_ptr->CFP_ACTIVE = OPC_TRUE;
						SF_ptr->SLEEP = OPC_FALSE;
						
						if (enable_log) {
							if (SF_ptr == &SF_params[SF_in]) {
								fprintf (log_file,"t=%f  -> ++++++++++ END OF INCOMING CAP PERIOD - GOING INTO CFP PERIOD ++++++++++ \n\n", op_sim_time());
							//	printf (" [%s (#%d)] t=%f  -> ++++++++++  END OF INCOMING CAP PERIOD - GOING INTO CFP PERIOD ++++++++++ \n\n", device_name, my_address, op_sim_time());
							
								if (GTS_params[GTS_DIRECTION_RECEIVE].start_slot == 0) {
									SF_ptr->current_slot = GTS_params[GTS_DIRECTION_TRANSMIT].start_slot - 1;
								} else if (GTS_params[GTS_DIRECTION_TRANSMIT].start_slot == 0) {
									SF_ptr->current_slot = GTS_params[GTS_DIRECTION_RECEIVE].start_slot - 1;
								} else if (GTS_params[GTS_DIRECTION_RECEIVE].start_slot < GTS_params[GTS_DIRECTION_TRANSMIT].start_slot) {
									SF_ptr->current_slot = GTS_params[GTS_DIRECTION_RECEIVE].start_slot - 1;
								} else {
									SF_ptr->current_slot = GTS_params[GTS_DIRECTION_TRANSMIT].start_slot - 1;
								}							
											
							} else {
								fprintf (log_file,"t=%f  -> ++++++++++ END OF OUTGOING CAP PERIOD - GOING INTO CFP PERIOD ++++++++++ \n\n", op_sim_time());
							//	printf (" [%s (#%d)] t=%f  -> ++++++++++  END OF OUTGOING CAP PERIOD - GOING INTO CFP PERIOD ++++++++++ \n\n", device_name, my_address, op_sim_time());	
								
								SF_ptr->current_slot = SF_ptr->Final_CAP_Slot;
							}
						}	

						op_intrpt_schedule_self (SF_ptr->BI_Boundary+Symbols2Sec(((TIME_SLOT_INDEX_MAX+1)*SF_ptr->slot_duration), WPAN_DATA_RATE), END_OF_CFP_PERIOD_CODE);
						
					}
						
					break;
				};/*end of END_OF_CAP_PERIOD_CODE */
			
				case END_OF_CFP_PERIOD_CODE :	/* the end of the CFP Expiration */
				{				
					SF_ptr->current_slot = TIME_SLOT_INDEX_MAX;
					SF_ptr->CFP_ACTIVE = OPC_FALSE;
					SF_ptr->SLEEP = OPC_TRUE;
					
					if (enable_log) {
						fprintf (log_file,"t=%f  -> ++++++++++ END OF THE CFP PERIOD - GOING INTO SLEEP PERIOD ++++++++++ \n\n", op_sim_time());
						//printf (" [%s (#%d)] t=%f  -> ++++++++++  END OF THE CFP PERIOD - GOING INTO SLEEP PERIOD ++++++++++ \n\n", device_name, my_address, op_sim_time());
					}						
					
					/*Inform the Battery Module that the node enters the SLEEP state*/
					op_intrpt_schedule_remote (op_sim_time(), END_OF_ACTIVE_PERIOD_CODE, baterry_id);
					
					SF_ptr = OPC_NIL;
					
					break;
				};/*end of END_OF_CFP_PERIOD_CODE */
			
				case TRY_PACKET_TRANSMISSION_CODE: /* try to send packet during CAP */
				{
					/* is there any packet in the CAP subqueue which can be send in this superframe */
					packet_to_send_index = wpan_CAP_packet_search ();
					
					if (packet_to_send_index != NOT_DEFINED) {						
						op_intrpt_schedule_self (op_sim_time(), START_OF_CSMA);
					}				
				
					break;
				};
			
				case CCA_START_CODE: /*At the start of the CCA */
				{
					/* do check if the channel is idle at the start of cca */
					/* at the start the channel is assumed idle, any change to busy, the CCA will report a busy channel */
				
					/* check at the beginning of CCA, if the channel is busy */
					SF_ptr->csma.CCA_CHANNEL_IDLE = OPC_TRUE;
					if (op_stat_local_read (RX_BUSY_STAT) == 1.0)
						SF_ptr->csma.CCA_CHANNEL_IDLE = OPC_FALSE;
					
					if (enable_log) {
						fprintf (log_file,"t=%f  -------- START CCA CW = %d ------>  CHANNEL IDLE = %s  \n\n", op_sim_time(), SF_ptr->csma.CW, boolean2string(SF_ptr->csma.CCA_CHANNEL_IDLE));
						//printf (" [%s (#%d)] t=%f  -------- START CCA CW = %d ------>  CHANNEL IDLE = %s  \n\n", device_name, my_address, op_sim_time(), SF_ptr->csma.CW, boolean2string(SF_ptr->csma.CCA_CHANNEL_IDLE));
					}
	
					if (SF_ptr->csma.CW == 2) {
						op_stat_write(statistic_vector.ch_busy_cca1, 1-SF_ptr->csma.CCA_CHANNEL_IDLE);
						op_stat_write(statistic_vector.ch_idle_cca1, SF_ptr->csma.CCA_CHANNEL_IDLE);
					} else {
						op_stat_write(statistic_vector.ch_busy_cca2, 1-SF_ptr->csma.CCA_CHANNEL_IDLE);
						op_stat_write(statistic_vector.ch_idle_cca2, SF_ptr->csma.CCA_CHANNEL_IDLE);
					}
					break;
				};/*end of CCA_START_CODE */
			
				case CCA_EXPIRATION_CODE :/*At the end of the CCA */
				{
					wpan_cca_expire ();
					break;
				};
			
				case START_TRANSMISSION_CODE: /* successful end of backoff and CCA - send packet */
				{	
					/*backoff_start_time is initialized in the "init_backoff" state*/
					op_stat_write(statistic_vector.mac_delay, op_sim_time()-backoff_start_time);
					op_stat_write(statistic_global_vector.mac_delay, op_sim_time()-backoff_start_time);
					
					collision_occurred = OPC_FALSE;
					
					/* get the pointer to MAC frame (MPDU) stored in the SUBQ_CAP queue */
					frame_MPDU = op_subq_pk_access (SUBQ_CAP, packet_to_send_index); 
					op_pk_nfd_get(frame_MPDU, "Sequence Number", &seq_num);
					op_pk_nfd_get(frame_MPDU, "Ack Request", &ack_request);
					op_pk_nfd_get(frame_MPDU, "Destination Address", &dest_address);
					
					op_pk_nfd_get_pkt(frame_MPDU, "MSDU", &frame_MSDU);	
					
					if (op_topo_parent(op_pk_creation_mod_get(frame_MSDU)) == parent_id) { /* I am the creator of the packet */
						op_pk_stamp(frame_MSDU); /* stamp the packet */	
					}					
					op_pk_nfd_set_pkt(frame_MPDU, "MSDU", frame_MSDU);
					
					if (ack_request == OPC_FALSE) { /* packet without ACK */
						
						if (enable_log) {
							fprintf(log_file,"t=%f   ----------- START TX [MAC destination address = %d, SEQ = %d, no ACK], %d retries \n\n", op_sim_time(), dest_address, seq_num, SF_ptr->csma.retries_nbr);
							//printf(" [%s (#%d)] t=%f  ----------- START TX [MAC destination address = %d, SEQ = %d, no ACK], %d retries \n\n", device_name, my_address, op_sim_time(), dest_address, seq_num, SF_ptr->csma.retries_nbr);
						}
					
						op_stat_write(statistic_vector.retransmission_attempts, SF_ptr->csma.retries_nbr);
						
						SF_ptr->csma.retries_nbr = 0;
						node_attributes.wait_ack = OPC_FALSE;
						
						/* remove packet at the head of subqueue */
						frame_MPDU_to_send = op_subq_pk_remove (SUBQ_CAP, packet_to_send_index);
								
						/* create PHY frame (PPDU) that encapsulates MAC frame (MPDU) */
						frame_PPDU = op_pk_create_fmt("wpan_frame_PPDU_format");
						
						/* wrap MAC frame (MPDU) in PHY frame (PPDU) */
						op_pk_nfd_set_pkt (frame_PPDU, "PSDU", frame_MPDU_to_send); 
						op_pk_nfd_set (frame_PPDU, "Frame Length", ((double) op_pk_total_size_get(frame_MPDU_to_send))/8); //[bytes]
						
						/* |PACKET|+|IFS| */
						tx_time = Bits2Sec (op_pk_total_size_get(frame_PPDU), WPAN_DATA_RATE) + Symbols2Sec(wpan_ifs_symbol_get(frame_MPDU_to_send), WPAN_DATA_RATE);
						op_intrpt_schedule_self (op_sim_time()+tx_time, SUCCESS_CODE);
					
						wpan_battery_update_tx((double)op_pk_total_size_get(frame_PPDU));
					
						op_stat_write(statistic_global_vector.sent_pkt, (double)(op_pk_total_size_get(frame_PPDU)));
						op_stat_write(statistic_vector.sent_pkt, (double)(op_pk_total_size_get(frame_PPDU)));
						
						//PPDU_sent_bits = PPDU_sent_bits + ((double)(op_pk_total_size_get(frame_PPDU))/1000.0); // in kbits
						
					// after we take the pkt from head of queue we send it
//						cout<<"Here is the packet:frame_MPDU_to_send "<<endl;
//						op_pk_print(frame_MPDU_to_send);
						op_pk_send (frame_PPDU, STRM_FROM_MAC_TO_RADIO);
						//op_pk_send (frame_MPDU_to_send, STRM_FROM_MAC_TO_RADIO);
						
					} else {	/* packet with ACK */
												
						node_attributes.wait_ack = OPC_TRUE;						
						
						/* create copy of queued frame */
						frame_MPDU_copy = op_pk_copy(frame_MPDU);
						
						/* put in memory the requested ack seq number */
						op_pk_nfd_get(frame_MPDU_copy, "Sequence Number", &node_attributes.wait_ack_seq_num);
						
						/* create PHY frame (PPDU) that encapsulates MAC frame (MPDU) */
						frame_PPDU = op_pk_create_fmt("wpan_frame_PPDU_format");
						
						/* wrap MAC frame (MPDU) in PHY frame (PPDU) */
						op_pk_nfd_set_pkt (frame_PPDU, "PSDU", frame_MPDU_copy); 
						op_pk_nfd_set (frame_PPDU, "Frame Length", ((double) op_pk_total_size_get(frame_MPDU_copy))/8); //[bytes]	
	
						/* |PACKET| */
						tx_time = Bits2Sec(op_pk_total_size_get(frame_PPDU), WPAN_DATA_RATE);
					
						op_intrpt_schedule_self (op_sim_time()+tx_time, SUCCESS_CODE);
						op_intrpt_schedule_self (op_sim_time()+tx_time+Symbols2Sec(macAckWaitDuration, WPAN_DATA_RATE), WAITING_ACK_END_CODE);
						
						if (enable_log) {
							fprintf(log_file,"t=%f   ----------- START TX [MAC destination address = %d, SEQ = %d, with ACK expiring at %f sec], %d retries  \n\n", op_sim_time(), dest_address, node_attributes.wait_ack_seq_num, op_sim_time()+tx_time+Symbols2Sec(macAckWaitDuration, WPAN_DATA_RATE), SF_ptr->csma.retries_nbr);
						//	printf(" [%s (#%d)] t=%f  ----------- START TX [MAC destination address = %d, SEQ = %d, with ACK expiring at %f sec], %d retries \n\n", device_name, my_address, op_sim_time(), dest_address, node_attributes.wait_ack_seq_num, op_sim_time()+tx_time+Symbols2Sec(macAckWaitDuration, WPAN_DATA_RATE), SF_ptr->csma.retries_nbr);
						}
						
						wpan_battery_update_tx((double)op_pk_total_size_get(frame_PPDU));
						
						//PPDU_sent_bits = PPDU_sent_bits + ((double)(op_pk_total_size_get(frame_PPDU))/1000.0); // in kbits
						
						op_stat_write(statistic_global_vector.sent_pkt, (double)(op_pk_total_size_get(frame_PPDU)));
						op_stat_write(statistic_vector.sent_pkt, (double)(op_pk_total_size_get(frame_PPDU)));
//						cout<<"Here is the packet:frame_MPDU_copy "<<endl;
//						op_pk_print(frame_MPDU_copy);
						op_pk_send (frame_PPDU, STRM_FROM_MAC_TO_RADIO);
				//	 	op_pk_send (frame_MPDU_copy, STRM_FROM_MAC_TO_RADIO);
					}									
					
					break;
				}; /*end of START_TRANSMISSION_CODE */
			
				case WAITING_ACK_END_CODE:	/* the timer for waiting an ACK has expired, the packet must be retransmitted */
				{
					if (node_attributes.wait_ack == OPC_TRUE) {
						/* I'm still waiting for the ACK and the timer expires, try to send the same packet*/
						node_attributes.wait_ack = OPC_FALSE;
						
						if (enable_log) {
							fprintf(log_file,"t=%f   -> WAITING ACK TIMER EXPIRED WITHOUT RECEIVING ACK FRAME !!  \n\n",op_sim_time());							
						//	printf(" [%s (#%d)] t=%f  -> WAITING ACK TIMER EXPIRED WITHOUT RECEIVING ACK FRAME !! \n\n",device_name, my_address, op_sim_time());
						}
						
						/* try_to_send the same packet */
						op_intrpt_schedule_self (op_sim_time(), FAILURE_CODE);						
			
					} else {
						op_sim_end("ERROR - WAITING_ACK_END_CODE","Waiting an ACK without setting of the flag 'node_attributes.wait_ack'!","","");
					}
					
					break;				
				}; /*end of WAITING_ACK_END_CODE */
			
				case ACK_SEND_CODE: /* send ACK back to the source */
				{
					if (enable_log) {
						fprintf(log_file,"t=%f  -> Send ACK Frame [SEQ = %d] \n\n", op_sim_time(), ack_sequence_number);
						//printf(" [%s (#%d)] t=%f  -> Send ACK Frame [SEQ = %d] \n\n", device_name, my_address, op_sim_time(), ack_sequence_number);
					}
						
					if (op_stat_local_read(TX_BUSY_STAT) == 1.0)
						op_sim_end("ERROR - ACK_SEND_CODE","Try to send an ACK while the TX channel is busy!","","");
						
					/* create an ACK MAC frame (MPDU) */
					ack_MPDU = op_pk_create_fmt ("wpan_ack_MPDU_format");
						
					op_pk_nfd_set (ack_MPDU, "Frame Type", ACK_FRAME_TYPE);
					op_pk_nfd_set (ack_MPDU, "Sequence Number", ack_sequence_number);
					op_pk_nfd_set (ack_MPDU, "Ack Request", 0);
					
					/* create PHY frame (PPDU) that encapsulates MAC frame (MPDU) */
					frame_PPDU = op_pk_create_fmt("wpan_frame_PPDU_format");
						
					/* wrap MAC frame MPDU in PHY frame (PPDU) */
					op_pk_nfd_set_pkt (frame_PPDU, "PSDU", ack_MPDU); 
					op_pk_nfd_set (frame_PPDU, "Frame Length", ((double) op_pk_total_size_get(ack_MPDU))/8); //[bytes]						
					
					wpan_battery_update_tx ((double)op_pk_total_size_get(frame_PPDU));
//					cout<<"Here is the packet ack_MPDU "<<endl;
//					op_pk_print(ack_MPDU);
				 	op_pk_send (frame_PPDU, STRM_FROM_MAC_TO_RADIO);
				//	op_pk_send (ack_MPDU, STRM_FROM_MAC_TO_RADIO);
					break;
				};	/*end of ACK_SEND_CODE */
				
				case ACK_SUCCESS_CODE:
				{
					packet_to_send_index = NOT_DEFINED;
					
					node_attributes.wait_ack_seq_num = 0;
					node_attributes.wait_ack = OPC_FALSE;
					
					if (SF_params[SF_in].CAP_ACTIVE || SF_params[SF_out].CAP_ACTIVE) { /* cAP */
						if (enable_log) {
							fprintf(log_file,"t=%f  ACK RECEIVED WITH SUCCESS AFTER %d retries - TRY TO SEND NEXT PACKET \n\n", op_sim_time(), SF_ptr->csma.retries_nbr);
						//	printf(" [%s (#%d)] t=%f  ACK RECEIVED WITH SUCCESS AFTER %d retries - TRY TO SEND NEXT PACKET \n\n", device_name, my_address, op_sim_time(), SF_ptr->csma.retries_nbr);
						}
					
						op_stat_write(statistic_vector.retransmission_attempts, SF_ptr->csma.retries_nbr);
						SF_ptr->csma.retries_nbr = 0;
					
						op_stat_write(statistic_vector.dropped_packets, 0.0);
						op_stat_write(statistic_vector.success_ack_packets, 1.0);
					
						op_stat_write(statistic_global_vector.dropped_packets, 0.0);
						op_stat_write(statistic_global_vector.success_ack_packets, 1.0);
					
						op_intrpt_schedule_self (op_sim_time(), TRY_PACKET_TRANSMISSION_CODE);
					
					} else { /* CFP */					
						
						if (GTS_ptr != OPC_NIL) { /* incoming superframe */	
							if (enable_log) {
								fprintf(log_file,"t=%f  ACK RECEIVED WITH SUCCESS AFTER %d retries  - TRY TO SEND NEXT PACKET  \n\n", op_sim_time(), GTS_ptr->retries_nbr);
							//	printf(" [%s (#%d)] t=%f  ACK RECEIVED WITH SUCCESS AFTER %d retries  - TRY TO SEND NEXT PACKET \n\n", device_name, my_address, op_sim_time(), GTS_ptr->retries_nbr);
							}
						
							GTS_ptr->retries_nbr = 0;
						} else { /* outgoing superframe */
							if (enable_log) {
								fprintf(log_file,"t=%f  ACK RECEIVED WITH SUCCESS AFTER %d retries  - TRY TO SEND NEXT PACKET  \n\n", op_sim_time(), GTS_list_ptr->retries_nbr);
							//	printf(" [%s (#%d)] t=%f  ACK RECEIVED WITH SUCCESS AFTER %d retries  - TRY TO SEND NEXT PACKET \n\n", device_name, my_address, op_sim_time(), GTS_list_ptr->retries_nbr);
							}
						
							GTS_list_ptr->retries_nbr = 0;
						}
						
						op_intrpt_schedule_self (op_sim_time(), TRY_GTS_PACKET_TRANSMISSION_CODE);
					}							
					
					break;		
				};	/*end of ACK_SUCCESS_CODE */				
			
				case SUCCESS_CODE :	/* packet was succesffully sent to the network: |PACKET| or |PACKET|+|IFS| */
				{					
					if (enable_log) { 
						if (node_attributes.wait_ack) {
							fprintf(log_file,"t=%f  -> !!!!!!!!! PACKET TRANSMISSION FINISHED WITH SUCCESS - waiting for ACK !!!!!!!!!  \n\n",op_sim_time());
						//	printf(" [%s (#%d)] t=%f  !!!!!!!!! PACKET TRANSMISSION FINISHED WITH SUCCESS - waiting for ACK !!!!!!!!! \n\n",device_name, my_address, op_sim_time());
						} else {
							fprintf(log_file,"t=%f  -> !!!!!!!!! PACKET TRANSMISSION FINISHED WITH SUCCESS !!!!!!!!!  \n\n",op_sim_time());
						//	printf(" [%s (#%d)] t=%f  !!!!!!!!! PACKET TRANSMISSION FINISHED WITH SUCCESS !!!!!!!!! \n\n",device_name, my_address, op_sim_time());						
						}							
					}
					
					if (SF_params[SF_in].CAP_ACTIVE || SF_params[SF_out].CAP_ACTIVE) { /* CAP */	
						op_stat_write(statistic_vector.dropped_packets, 0.0);
						op_stat_write(statistic_global_vector.dropped_packets, 0.0);
					
						if (node_attributes.wait_ack == OPC_FALSE) {
							packet_to_send_index = NOT_DEFINED;
							op_intrpt_schedule_self (op_sim_time(), TRY_PACKET_TRANSMISSION_CODE);	/* try to send next packet */
						}
					} else { /* CFP */					
						if (node_attributes.wait_ack == OPC_FALSE) {
							packet_to_send_index = NOT_DEFINED;
							op_intrpt_schedule_self (op_sim_time(), TRY_GTS_PACKET_TRANSMISSION_CODE);	/* try to send next packet */ 								
						}
					}
					
					break;
				};	/*end of SUCCESS_CODE */
			
				case FAILURE_CODE: 
				{	
					if (enable_log) {
						fprintf(log_file,"t=%f  -> !!! TRANSMISSION FAILURE !!!  \n\n",op_sim_time());
					//	printf(" [%s (#%d)] t=%f  -> !!! TRANSMISSION FAILURE !!! \n\n",device_name, my_address, op_sim_time());
					}
					
					if (SF_params[SF_in].CAP_ACTIVE || SF_params[SF_out].CAP_ACTIVE) { /* CAP */
				
						if (SF_ptr->csma.retries_nbr < node_attributes.macMaxFrameRetries) {
							if (enable_log) {
								fprintf(log_file,"t=%f  -> Try to retransmit the same packet  (number of retries = %d) \n\n", op_sim_time(), SF_ptr->csma.retries_nbr);
							//	printf(" [%s (#%d)] t=%f  -> Try to retransmit the same packet (number of retries = %d) \n\n", device_name, my_address, op_sim_time(), SF_ptr->csma.retries_nbr);
							}							
							
							SF_ptr->csma.retries_nbr = SF_ptr->csma.retries_nbr + 1;
							
							op_stat_write(statistic_vector.dropped_packets, 0.0);
							op_stat_write(statistic_global_vector.dropped_packets, 0.0);						
						
							op_intrpt_schedule_self (op_sim_time(), TRY_PACKET_TRANSMISSION_CODE);	/* try to send the same packet once more */
						
						} else { /* SF_ptr->csma.retries_nbr >= node_attributes.macMaxFrameRetries */
							if (enable_log) {
								fprintf(log_file,"t=%f  -> Transmision failure (number of retries = %d = macMaxFrameRetries). Remove packet from CAP queue. \n", op_sim_time(), SF_ptr->csma.retries_nbr);
							//	printf(" [%s (#%d)] t=%f  -> Transmision failure (number of retries = %d = macMaxFrameRetries). Remove packet from CAP queue. \n",device_name, my_address, op_sim_time(), SF_ptr->csma.retries_nbr);
							}
						
							op_stat_write(statistic_vector.dropped_packets, 1.0);
							op_stat_write(statistic_global_vector.dropped_packets, 1.0);
						
							 /* remove MAC frame (MPDU) from CAP subqueue */
							frame_MPDU = op_subq_pk_remove(SUBQ_CAP, packet_to_send_index); 
							op_pk_destroy(frame_MPDU);
							
							packet_to_send_index = NOT_DEFINED;
	
							op_stat_write(statistic_vector.retransmission_attempts, SF_ptr->csma.retries_nbr);
							
							SF_ptr->csma.retries_nbr = 0;
							
							op_intrpt_schedule_self (op_sim_time(), TRY_PACKET_TRANSMISSION_CODE);	/* try to send next packet */					
						} 
					}else { /* CFP */
						if (GTS_ptr != OPC_NIL) { /* incoming superframe */
							if (GTS_ptr->retries_nbr < node_attributes.macMaxFrameRetries) {
								if (enable_log) {
									fprintf(log_file,"t=%f  -> Try to retransmit the same packet  (number of retries = %d) \n\n", op_sim_time(), GTS_ptr->retries_nbr);
								//	printf(" [%s (#%d)] t=%f  -> Try to retransmit the same packet  (number of retries = %d)  \n\n", device_name, my_address, op_sim_time(), GTS_ptr->retries_nbr);
								}
							
								GTS_ptr->retries_nbr = GTS_ptr->retries_nbr + 1;
							
								op_stat_write(statistic_vector.dropped_packets, 0.0);
								op_stat_write(statistic_global_vector.dropped_packets, 0.0);
							
								op_intrpt_schedule_self (op_sim_time(), TRY_GTS_PACKET_TRANSMISSION_CODE); /* try to send the same packet */
							
							} else { /* GTS_ptr->retries_nbr >= node_attributes.macMaxFrameRetries */ 
								if (enable_log) {
									fprintf(log_file,"t=%f  -> Transmision failure (number of retries = %d = macMaxFrameRetries). Remove packet from CFP queue. \n\n", op_sim_time(), GTS_ptr->retries_nbr);							
									//printf(" [%s (#%d)] t=%f  -> Transmision failure (number of retries = %d = macMaxFrameRetries). Remove packet from CFP queue. \n\n", device_name, my_address, op_sim_time(), GTS_ptr->retries_nbr);							
								}
							
								op_stat_write(statistic_vector.dropped_packets, 1.0);
								op_stat_write(statistic_global_vector.dropped_packets, 1.0);
							
								/* remove MAC frame (MPDU) from CFP subqueue */
								frame_MPDU = op_subq_pk_remove(SUBQ_GTS, packet_to_send_index);
								op_pk_destroy(frame_MPDU);
							
								packet_to_send_index = NOT_DEFINED;
	
								GTS_ptr->retries_nbr = 0;
							
								op_intrpt_schedule_self (op_sim_time(), TRY_GTS_PACKET_TRANSMISSION_CODE);	/* try to send next packet */
							}						
 						} else { /* outgoing superframe */
							if (GTS_list_ptr->retries_nbr < node_attributes.macMaxFrameRetries) {
								if (enable_log) {
									fprintf(log_file,"t=%f  -> Try to retransmit the same packet  (number of retries = %d) \n\n", op_sim_time(), GTS_list_ptr->retries_nbr);
								//	printf(" [%s (#%d)] t=%f  -> Try to retransmit the same packet  (number of retries = %d)  \n\n", device_name, my_address, op_sim_time(), GTS_list_ptr->retries_nbr);
								}
							
								GTS_list_ptr->retries_nbr = GTS_list_ptr->retries_nbr + 1;
							
								op_stat_write(statistic_vector.dropped_packets, 0.0);
								op_stat_write(statistic_global_vector.dropped_packets, 0.0);
							
								op_intrpt_schedule_self (op_sim_time(), TRY_GTS_PACKET_TRANSMISSION_CODE); /* try to send the same packet */
							
							} else { /* GTS_list_ptr->retries_nbr >= node_attributes.macMaxFrameRetries */ 
								if (enable_log) {
									fprintf(log_file,"t=%f  -> Transmision failure (number of retries = %d = macMaxFrameRetries). Remove packet from CFP queue. \n\n", op_sim_time(), GTS_list_ptr->retries_nbr);							
								//	printf(" [%s (#%d)] t=%f  -> Transmision failure (number of retries = %d = macMaxFrameRetries). Remove packet from CFP queue. \n\n", device_name, my_address, op_sim_time(), GTS_list_ptr->retries_nbr);							
								}
							
								op_stat_write(statistic_vector.dropped_packets, 1.0);
								op_stat_write(statistic_global_vector.dropped_packets, 1.0);
							
								/* remove MAC frame (MPDU) from CFP subqueue */
								frame_MPDU = op_subq_pk_remove(SUBQ_GTS, packet_to_send_index);
								op_pk_destroy(frame_MPDU);
							
								packet_to_send_index = NOT_DEFINED;	
								GTS_list_ptr->retries_nbr = 0;
							
								op_intrpt_schedule_self (op_sim_time(), TRY_GTS_PACKET_TRANSMISSION_CODE);	/* try to send next packet */
							}						
						}									
					}
					
					break;
				}; /*end of FAILURE_CODE */
				
				case START_OF_TRANSMIT_GTS_USE:	/* try to allocate appropriate TRANSMIT GTS slots */ 				
				{
					wpan_gts_start (GTS_DIRECTION_TRANSMIT);						
					break;						
				};	/* end of START_OF_TRANSMIT_GTS_USE */
				
				case START_OF_RECEIVE_GTS_USE:	/* try to allocate appropriate RECEIVE GTS slots */ 				
				{
					wpan_gts_start (GTS_DIRECTION_RECEIVE);						
					break;						
				};	/* end of START_OF_RECEIVE_GTS_USE */		
			
				case STOP_OF_RECEIVE_GTS_USE: 
				{
					wpan_gts_stop (GTS_DIRECTION_RECEIVE);					
					break;
				};	/* end of STOP_OF_RECEIVE_GTS_USE */
				
				case STOP_OF_TRANSMIT_GTS_USE: 
				{
					wpan_gts_stop (GTS_DIRECTION_TRANSMIT);					
					break;
				};	/* end of STOP_OF_TRANSMIT_GTS_USE */
				
				case START_OF_GTS_PERIOD:
				{
					if (SF_ptr == OPC_NIL) {
					//	printf (" [%s (#%d)] t=%f  ->ERROR: Superframe pointer (SF_ptr) is empty (OPC_NIL) \n\n", device_name, my_address, op_sim_time());
						op_sim_end("ERROR - START_OF_GTS_PERIOD","Superframe pointer (SF_ptr) is not defined (OPC_NIL)","","");
					}
					
// printf("AAAA [%s (#%d)] t=%f START_OF_GTS_PERIOD SF_ptr = %d, SF_params[SF_in] = %d, SF_params[SF_out] = %d  \n\n", device_name, my_address, op_sim_time(), SF_ptr, &SF_params[SF_in], &SF_params[SF_out]);
					
					if (SF_ptr == &SF_params[SF_in]) { /* incoming superframe */
						for (i=0;i<2;i++) {	

// printf("AAAA [%s (#%d)] t=%f SF_ptr->current_slot = %d, GTS_params[%d].start_slot = %d  \n\n", device_name, my_address, op_sim_time(), SF_ptr->current_slot, i, GTS_params[i].start_slot);
						
							if ((SF_ptr->current_slot + 1) == GTS_params[i].start_slot) {
								if (enable_log) {
									fprintf (log_file,"t=%f  ---> START of child's GTS (incoming superframe) [length: %d slots (%0.3f msec), direction: %d] <---   \n\n", op_sim_time(), GTS_params[i].length, Symbols2ms(GTS_params[i].length*SF_ptr->slot_duration, WPAN_DATA_RATE), GTS_params[i].direction);
									//printf (" [%s (#%d)] t=%f  ---> START of child's GTS (incoming superframe) [length: %d slots (%0.3f msec), direction: %d] <---  \n\n", device_name, my_address, op_sim_time(), GTS_params[i].length, Symbols2ms(GTS_params[i].length*SF_ptr->slot_duration, WPAN_DATA_RATE), GTS_params[i].direction);
								}
							
								sent_gts_bits = 0;
								gts_statistics.gts_periods_count++;
							
								/* TRANSMIT DIRECTION: child_node->parent */
								if (GTS_params[i].direction == GTS_DIRECTION_TRANSMIT)
									op_intrpt_schedule_self (op_sim_time(), TRY_GTS_PACKET_TRANSMISSION_CODE);
								
/* schedule end of current GTS */
// op_intrpt_schedule_self (op_sim_time() + Symbols2Sec(GTS_params[i].length * SF_ptr->slot_duration, WPAN_DATA_RATE), END_OF_GTS_PERIOD);
								
								GTS_ptr = &GTS_params[i];
								
								break;
							}
						}
						
					} else { /* outgoing superframe */					
						for (i=0; i<op_prg_list_size (node_attributes.GTS_list); i++) {							
							if ((SF_ptr->current_slot + 1) == ((wpan_gts_list_record *) op_prg_list_access (node_attributes.GTS_list, i))->start_slot) {
								GTS_list_ptr = (wpan_gts_list_record *) op_prg_list_access (node_attributes.GTS_list, i);
								
								if (enable_log) {
									fprintf (log_file,"t=%f  ---> START of parent's GTS (outgoing superframe) [length: %d slots (%0.3f msec), direction: %d] <---   \n\n", op_sim_time(), GTS_list_ptr->length, Symbols2ms(GTS_list_ptr->length*SF_ptr->slot_duration, WPAN_DATA_RATE), GTS_list_ptr->direction);
								//	printf (" [%s (#%d)] t=%f  ---> START of parent's GTS (outgoing superframe) [length: %d slots (%0.3f msec), direction: %d] <---  \n\n", device_name, my_address, op_sim_time(), GTS_list_ptr->length, Symbols2ms(GTS_list_ptr->length*SF_ptr->slot_duration, WPAN_DATA_RATE), GTS_list_ptr->direction);
								}
								
								sent_gts_bits = 0;
								gts_statistics.gts_periods_count++;
							
								/* RECEIVE DIRECTION: parent->child_node */
								if (GTS_list_ptr->direction == GTS_DIRECTION_RECEIVE)
									op_intrpt_schedule_self (op_sim_time(), TRY_GTS_PACKET_TRANSMISSION_CODE);
								
/* schedule end of current GTS */
// op_intrpt_schedule_self (op_sim_time() + Symbols2Sec(GTS_list_ptr->length * SF_ptr->slot_duration, WPAN_DATA_RATE), END_OF_GTS_PERIOD);
								
								break;
							}					
						}
					}									
				
					break;
				};	/* end of START_OF_GTS_PERIOD */
				
				case END_OF_GTS_PERIOD:
				{				
// printf("AAAA - node: %s, t=%f END_OF_GTS_PERIOD (SF_ptr: %d) GTS_ptr: %d, GTS_list_ptr: %d   \n\n",device_name, op_sim_time(), SF_ptr, GTS_ptr, GTS_list_ptr);

					if (SF_ptr == &SF_params[SF_in]) { /* incoming superframe */
					
						/* update SF_ptr->current_slot */
						if ((GTS_params[GTS_DIRECTION_RECEIVE].start_slot == 0) || (GTS_params[GTS_DIRECTION_TRANSMIT].start_slot == 0)) {
							SF_ptr->current_slot = TIME_SLOT_INDEX_MAX;
						} else if (GTS_params[GTS_DIRECTION_TRANSMIT].start_slot > GTS_params[GTS_DIRECTION_RECEIVE].start_slot) {
							SF_ptr->current_slot = GTS_params[GTS_DIRECTION_TRANSMIT].start_slot - 1;
						} else {
							SF_ptr->current_slot = GTS_params[GTS_DIRECTION_RECEIVE].start_slot - 1;
						}					

						// SF_ptr->current_slot = SF_ptr->current_slot + GTS_ptr->length;
					
						if (GTS_ptr->direction == GTS_DIRECTION_TRANSMIT) { /* TRANSMIT DIRECTION: child_node -> parent */						
							j = GTS_ptr->length * SF_ptr->slot_duration; /* GTS length [symbols] */
							throughput = (double)sent_gts_bits/(j * Symbol2Bits); /* throughput */
						
							gts_statistics.band_wast_sum = gts_statistics.band_wast_sum + (1.0 - throughput);

							op_stat_write (gts_statistic_vector.wasted_bandwidth_pc, (1.0 - throughput)); 
							op_stat_write (gts_statistic_vector.throughput_pc, throughput);
						
							if (enable_log) {							
								fprintf (log_file,"t=%f  ---> END of child's GTS (incoming superframe) [length: %d slots (%0.3f msec), direction: %d]  <---  \n", op_sim_time(), GTS_ptr->length, Symbols2ms(GTS_ptr->length*SF_ptr->slot_duration, WPAN_DATA_RATE), GTS_ptr->direction);
								fprintf (log_file,"\t + GTS period length: %d bits (%0.3f msec), sent bits (PPDU): %d, wasted bandwidth: %0.2f %%.\n\n", j*Symbol2Bits, Symbols2ms(j, WPAN_DATA_RATE), sent_gts_bits, (1.0 - throughput)*100);
																
								//printf (" [%s (#%d)] t=%f  ---> END of child's GTS (incoming superframe) [length: %d slots (%0.3f msec), direction: %d] <--- \n",device_name, my_address, op_sim_time(), GTS_ptr->length, Symbols2ms(GTS_ptr->length*SF_ptr->slot_duration, WPAN_DATA_RATE), GTS_ptr->direction);
								//printf ("\t + GTS period length: %d bits (%0.3f msec), sent bits (PPDU): %d, wasted bandwidth: %0.2f %%.\n\n", j*Symbol2Bits, Symbols2ms(j, WPAN_DATA_RATE), sent_gts_bits, (1.0 - throughput)*100);
							}
						} else { /* RECEIVE DIRECTION: parent -> child_node */
							if (enable_log) {
								fprintf (log_file,"t=%f  ---> END of child's GTS (incoming superframe) [length: %d slots (%0.3f msec), direction: %d]  <---  \n\n", op_sim_time(), GTS_ptr->length, Symbols2ms(GTS_ptr->length*SF_ptr->slot_duration, WPAN_DATA_RATE), GTS_ptr->direction);
								//printf (" [%s (#%d)] t=%f  ---> END of child's GTS (incoming superframe) [length: %d slots (%0.3f msec), direction: %d] <--- \n\n",device_name, my_address, op_sim_time(), GTS_ptr->length, Symbols2ms(GTS_ptr->length*SF_ptr->slot_duration, WPAN_DATA_RATE), GTS_ptr->direction);
							}						
						}					
						GTS_ptr = OPC_NIL;					
					} else { /* outgoing superframe */						
						SF_ptr->current_slot = SF_ptr->current_slot + GTS_list_ptr->length;
					
						/* RECEIVE DIRECTION: parent->child_node */
						if ( GTS_list_ptr->direction == GTS_DIRECTION_RECEIVE) {
							j = GTS_list_ptr->length * SF_ptr->slot_duration; /* GTS length [symbols] */
							throughput = (double)sent_gts_bits/(j * Symbol2Bits); /* throughput */
						
							gts_statistics.band_wast_sum = gts_statistics.band_wast_sum + (1.0 - throughput);

							op_stat_write (gts_statistic_vector.wasted_bandwidth_pc, (1.0 - throughput)); 
							op_stat_write (gts_statistic_vector.throughput_pc, throughput);
						
							if (enable_log) {
								fprintf (log_file,"t=%f  ---> END of parent's GTS (outgoing superframe) [length: %d slots (%0.3f msec), direction: %d]  <---  \n", op_sim_time(),  GTS_list_ptr->length, Symbols2ms( GTS_list_ptr->length*SF_ptr->slot_duration, WPAN_DATA_RATE),  GTS_list_ptr->direction);
								fprintf (log_file,"\t + GTS period length: %d bits (%0.3f msec), sent bits (PPDU): %d, wasted bandwidth: %0.2f %%.\n\n", j*Symbol2Bits, Symbols2ms(j, WPAN_DATA_RATE), sent_gts_bits, (1.0 - throughput)*100);

								
								//printf (" [%s (#%d)] t=%f  ---> END of parent's GTS (outgoing superframe) [length: %d slots (%0.3f msec), direction: %d] <--- \n", device_name, my_address, op_sim_time(),  GTS_list_ptr->length, Symbols2ms( GTS_list_ptr->length*SF_ptr->slot_duration, WPAN_DATA_RATE),  GTS_list_ptr->direction);
								//printf ("\t + GTS period length: %d bits (%0.3f msec), sent bits (PPDU): %d, wasted bandwidth: %0.2f %%.\n\n", j*Symbol2Bits, Symbols2ms(j, WPAN_DATA_RATE), sent_gts_bits, (1.0 - throughput)*100);
							}
						} else {
							if (enable_log) {
								fprintf (log_file,"t=%f  ---> END of parent's GTS (outgoing superframe) [length: %d slots (%0.3f msec), direction: %d]  <---  \n\n", op_sim_time(),  GTS_list_ptr->length, Symbols2ms( GTS_list_ptr->length*SF_ptr->slot_duration, WPAN_DATA_RATE),  GTS_list_ptr->direction);
								//printf (" [%s (#%d)] t=%f  ---> END of parent's GTS (outgoing superframe) [length: %d slots (%0.3f msec), direction: %d] <--- \n\n", device_name, my_address, op_sim_time(),  GTS_list_ptr->length, Symbols2ms( GTS_list_ptr->length*SF_ptr->slot_duration, WPAN_DATA_RATE),  GTS_list_ptr->direction);
							}						
						}						
						GTS_list_ptr = OPC_NIL;					
					}
						
					break;
				};	/* end of END_OF_GTS_PERIOD */
				
				case TRY_GTS_PACKET_TRANSMISSION_CODE:
				{	
					/* check the subqueue SUBQ_CAP - is there any packet in the GTS subqueue which can be send in this superframe */
					if (!op_subq_empty(SUBQ_GTS)) {
					
						/* determine the number of packets in subqueue */
						j = op_subq_stat (SUBQ_GTS, OPC_QSTAT_PKSIZE);
					
						for (i=0; i<j; i++) {
							frame_MPDU = op_subq_pk_access (SUBQ_GTS, i);
							op_pk_nfd_get(frame_MPDU, "Destination Address", &dest_address);
							
							if (((dest_address == my_parent_address) && SF_params[SF_in].CFP_ACTIVE) || ((dest_address == GTS_list_ptr->assoc_dev_address) && SF_params[SF_out].CFP_ACTIVE)) {
								packet_to_send_index = i;
								op_intrpt_schedule_self (op_sim_time(), START_GTS_TRANSMISSION_CODE);
								break;
							}
						}
					} else {
						if (enable_log) {
							fprintf(log_file,"t=%f  -> The GTS subqueue is EMPTY. Waiting for the incomming real-time (GTS) packet. <- \n\n", op_sim_time());
							//printf(" [%s (#%d)] t=%f  -> The GTS subqueue is EMPTY. Waiting for the incomming real-time (GTS) packet. <- \n\n", device_name, my_address, op_sim_time());
						}
					
					}
				
					break;
				};/*end of TRY_GTS_PACKET_TRANSMISSION_CODE */
				
				case START_GTS_TRANSMISSION_CODE:
				{
					collision_occurred = OPC_FALSE;
					
					if (start_GTS_transmission) { /* waiting for full buffer if it is required */
						frame_MPDU = op_subq_pk_access (SUBQ_GTS, packet_to_send_index);						
						op_pk_nfd_get(frame_MPDU, "Sequence Number", &seq_num);
						op_pk_nfd_get(frame_MPDU, "Ack Request", &ack_request);
						
						op_pk_nfd_get_pkt(frame_MPDU, "MSDU", &frame_MSDU);					
						
						if (op_topo_parent(op_pk_creation_mod_get(frame_MSDU)) == parent_id) { /* I am the creator of the packet */
							op_pk_stamp(frame_MSDU); /* stamp the packet */	
						}					
						op_pk_nfd_set_pkt(frame_MPDU, "MSDU", frame_MSDU);
						
						/* check at the beginning, if the channel is busy */						
						if (op_stat_local_read (TX_BUSY_STAT) == 1.0)
							op_sim_end("ERROR - TRY_GTS_PACKET_TRANSMISSION_CODE","Try to send GTS packet while the TX channel is busy.","","");
						
						if (wpan_gts_check_time (frame_MPDU)) {
							if (ack_request == OPC_FALSE) { /* packet without ACK */
								/* remove MAC frame (MPDU) from the subqueue */
								frame_MPDU_to_send = op_subq_pk_remove (SUBQ_GTS, packet_to_send_index); 						
						
								if (enable_log) {
									fprintf(log_file,"t=%f  ----------- START GTS TX [SEQ = %d, no ACK] ----------- \n\n", op_sim_time(), seq_num);
									//printf(" [%s (#%d)] t=%f  ----------- START GTS TX [SEQ = %d, no ACK] ----------- \n\n", device_name, my_address, op_sim_time(), seq_num);
								}

								node_attributes.wait_ack = OPC_FALSE;								
								
								/* create PHY frame (PPDU) that encapsulates MAC frame (MPDU) */
								frame_PPDU = op_pk_create_fmt("wpan_frame_PPDU_format");
								
								/* wrap MAC frame MPDU in PHY frame (PPDU) */
								op_pk_nfd_set_pkt (frame_PPDU, "PSDU", frame_MPDU_to_send); 
								op_pk_nfd_set (frame_PPDU, "Frame Length", ((double) op_pk_total_size_get(frame_MPDU_to_send))/8); //[bytes]	
							
								/* |PACKET|+|IFS| */
								tx_time = Symbols2Sec(Bits2Symbols((int) op_pk_total_size_get(frame_PPDU)), WPAN_DATA_RATE) + Symbols2Sec(wpan_ifs_symbol_get(frame_MPDU_to_send), WPAN_DATA_RATE); 
							
								op_intrpt_schedule_self (op_sim_time()+tx_time, SUCCESS_CODE);
					
								wpan_battery_update_tx((double)op_pk_total_size_get(frame_PPDU));
//								cout<<"Here is the packet frame_MPDU_to_send "<<endl;
//								op_pk_print(frame_MPDU_to_send);
								  op_pk_send (frame_PPDU, STRM_FROM_MAC_TO_RADIO);
							//	op_pk_send (frame_MPDU_to_send, STRM_FROM_MAC_TO_RADIO);
								
								op_stat_write (gts_statistic_vector.sent_frames, (double)(op_pk_total_size_get(frame_PPDU)));
								op_stat_write (gts_statistic_vector.sent_framesG, (double)(op_pk_total_size_get(frame_PPDU)));								
								op_stat_write (gts_statistic_vector.frame_delay, (op_sim_time() - op_pk_creation_time_get(frame_MPDU_to_send)));								
								op_stat_write (statistic_global_vector.sent_pkt, (double)(op_pk_total_size_get(frame_PPDU)));
								op_stat_write (statistic_vector.sent_pkt, (double)(op_pk_total_size_get(frame_PPDU)));								
								
								gts_statistics.frame_delay_sum = gts_statistics.frame_delay_sum + (op_sim_time() - op_pk_creation_time_get(frame_MPDU_to_send));
								gts_statistics.frame_delay_max = max_double (gts_statistics.frame_delay_max, (op_sim_time() - op_pk_creation_time_get(frame_MPDU_to_send)));
								gts_statistics.frame_delay_min = min_double (gts_statistics.frame_delay_min, (op_sim_time() - op_pk_creation_time_get(frame_MPDU_to_send)));	
								gts_statistics.sent_frames++;
								gts_statistics.sent_bits = gts_statistics.sent_bits + op_pk_total_size_get(frame_PPDU);
								
								sent_gts_bits = sent_gts_bits + op_pk_total_size_get(frame_PPDU);
							
							} else { /* packet with ACK */
								node_attributes.wait_ack = OPC_TRUE;
								
								frame_MPDU_copy = op_pk_copy(frame_MPDU);
								
								/* put in memory the requested ack seq number */
								op_pk_nfd_get (frame_MPDU_copy, "Sequence Number", &node_attributes.wait_ack_seq_num);
						
								/* create PHY frame (PPDU) that encapsulates MAC frame (MPDU) */
								frame_PPDU = op_pk_create_fmt("wpan_frame_PPDU_format");
						
								/* wrap MAC frame (MPDU) in PHY frame (PPDU) */
								op_pk_nfd_set_pkt (frame_PPDU, "PSDU", frame_MPDU_copy); 
								op_pk_nfd_set (frame_PPDU, "Frame Length", ((double) op_pk_total_size_get(frame_MPDU_copy))/8); //[bytes]	
						
								/* |PACKET| */
								tx_time = Symbols2Sec(Bits2Symbols((int) op_pk_total_size_get(frame_PPDU)), WPAN_DATA_RATE);
						
								op_intrpt_schedule_self (op_sim_time()+tx_time, SUCCESS_CODE);
								op_intrpt_schedule_self (op_sim_time()+tx_time+Symbols2Sec(macAckWaitDuration, WPAN_DATA_RATE), WAITING_ACK_END_CODE);
						
								if (enable_log) {
									fprintf(log_file,"t=%f   ----------- START GTS TX [SEQ = %d, with ACK expiring at %f] ----------- \n\n", op_sim_time(), seq_num, op_sim_time()+tx_time+Symbols2Sec(macAckWaitDuration, WPAN_DATA_RATE));
									//printf(" [%s (#%d)] t=%f  ----------- START GTS TX [SEQ = %d, with ACK expiring at %f] ----------- \n\n", device_name, my_address, op_sim_time(), seq_num, op_sim_time()+tx_time+Symbols2Sec(macAckWaitDuration, WPAN_DATA_RATE));
								}							
						
								wpan_battery_update_tx((double)op_pk_total_size_get(frame_PPDU));								
//								cout<<"Here is the packet frame_MPDU_copy "<<endl;
//								op_pk_print(frame_MPDU_copy);
								  op_pk_send (frame_PPDU, STRM_FROM_MAC_TO_RADIO);
							//	op_pk_send (frame_MPDU_copy, STRM_FROM_MAC_TO_RADIO);
								
								gts_pkt_delay = op_sim_time() - op_pk_creation_time_get(frame_MPDU);					
								
								op_stat_write (gts_statistic_vector.sent_frames, (double)(op_pk_total_size_get(frame_PPDU)));
								op_stat_write (gts_statistic_vector.sent_framesG, (double)(op_pk_total_size_get(frame_PPDU)));								
								op_stat_write (statistic_global_vector.sent_pkt, (double)(op_pk_total_size_get(frame_PPDU)));
								op_stat_write (statistic_vector.sent_pkt, (double)(op_pk_total_size_get(frame_PPDU)));
								
								gts_statistics.sent_frames++;
								gts_statistics.sent_bits = gts_statistics.sent_bits + op_pk_total_size_get(frame_PPDU);
								
								sent_gts_bits = sent_gts_bits + op_pk_total_size_get(frame_PPDU);							
							}
						
						} else { /* wpan_gts_check_time (frame_MPDU) */
	
							if (enable_log) {
								fprintf (log_file,"t=%f  -> NOT ENOUGH TIME IN THE GTS PERIOD - DEFER FRAME TRANSMISSION UNTIL NEXT GTS. \n", op_sim_time());
								fprintf (log_file,"\t + GTS Subqueue still contains %.0f frames and %.0f bits. \n\n", op_subq_stat (SUBQ_GTS, OPC_QSTAT_PKSIZE), op_subq_stat (SUBQ_GTS, OPC_QSTAT_BITSIZE));
								//printf (" [%s (#%d)] t=%f  -> NOT ENOUGH TIME IN THE GTS PERIOD - DEFER FRAME TRANSMISSION UNTIL NEXT GTS. \n", device_name, my_address, op_sim_time());
								//printf ("\t + GTS Subqueue still contains %.0f frames and %.0f bits. \n\n", op_subq_stat (SUBQ_GTS, OPC_QSTAT_PKSIZE), op_subq_stat (SUBQ_GTS, OPC_QSTAT_BITSIZE));
							}							
						}						
					} else { /* !start_GTS_transmission */
						if (enable_log) {
							fprintf(log_file,"t=%f  -> The GTS subqueue is not FULL. Waiting for full GTS subqueue. <- \n\n", op_sim_time());
							//printf(" [%s (#%d)] t=%f  -> The GTS subqueue is not FULL. Waiting for full GTS subqueue. <- \n\n", device_name, my_address, op_sim_time());
						}	
					}
				
					
								
					break;
				};/*end of START_GTS_TRANSMISSION_CODE */
				
				default :
				{
				};
			}	/* end switch (op_intrpt_code()) */
			
			break;
				
		}; /* end of OPC_INTRPT_SELF */
		case OPC_INTRPT_ENDSIM:
		{
			if (enable_log) {
				fprintf (log_file, "t=%f  ***********   GAME OVER END - OF SIMULATION  ********************  \n\n",op_sim_time());
			//	printf (" [%s (#%d)] t=%f  ***********   GAME OVER - END OF SIMULATION  *******************\n\n", device_name, my_address, op_sim_time());
				
				if (gts_statistics.gts_periods_count > 0) {
					fprintf (log_file, "|------------------------------ GTS STATISTICS -------------------------------|\n");
					fprintf (log_file, " Node %s (#%d) \n", device_name, my_address);
					fprintf (log_file, "  Data frames STORED in the buffer                = %d = %.3f Kbits (MSDU) \n", gts_statistics.stored_frames, gts_statistics.stored_bits*0.001);
					fprintf (log_file, "  Data frames DISPATCHED to the network           = %d = %.3f Kbits (PPDU) \n", gts_statistics.sent_frames, gts_statistics.sent_bits*0.001);
					fprintf (log_file, "  DROPPED data frames (overfull buffer)           = %d = %.3f Kbits -> %.3f %% (MPDU) \n", gts_statistics.dropped_frames, gts_statistics.dropped_bits*0.001, ((double) gts_statistics.dropped_frames/gts_statistics.stored_frames)*100);
					fprintf (log_file, "  Data frames RECEIVED from network               = %d = %.3f Kbits (MSDU) \n", gts_statistics.received_frames, gts_statistics.received_bits*0.001);					
					fprintf (log_file, "  Number of the GTS periods during the simulation = %d \n", gts_statistics.gts_periods_count);
					fprintf (log_file, "  GTS Throughtput: \n");
					fprintf (log_file, "    Average wasted bandwidth = %f %% \n", (gts_statistics.band_wast_sum/gts_statistics.gts_periods_count)*100);
					fprintf (log_file, "    Average throughput = %f %% \n", (1-gts_statistics.band_wast_sum/gts_statistics.gts_periods_count)*100);
					
				
					if (gts_statistics.sent_frames > 0) {
						fprintf (log_file, "  Frame delay in the buffer (from storing to the buffer to dispatching):\n");
						fprintf (log_file, "    max = %.3f sec \n", gts_statistics.frame_delay_max);
						fprintf (log_file, "    min = %.3f sec \n", gts_statistics.frame_delay_min);
						fprintf (log_file, "    average = %.3f sec \n", gts_statistics.frame_delay_sum/gts_statistics.sent_frames);
					}
					
					fprintf (log_file, "|-----------------------------------------------------------------------------|\n\n");
/*
					printf ("|------------------------------ GTS STATISTICS -------------------------------|\n");
					printf (" Node %s (#%d) \n", device_name, my_address);
					printf ("  Data frames STORED in the buffer                = %d = %.3f Kbits (MSDU) \n", gts_statistics.stored_frames, gts_statistics.stored_bits*0.001);
					printf ("  Data frames DISPATCHED to the network           = %d = %.3f Kbits (MPDU) \n", gts_statistics.sent_frames, gts_statistics.sent_bits*0.001);
					printf ("  DROPPED data frames (overfull buffer)           = %d = %.3f Kbits -> %.3f %% (PPDU) \n", gts_statistics.dropped_frames, gts_statistics.dropped_bits*0.001, ((double) gts_statistics.dropped_frames/gts_statistics.stored_frames)*100);
					printf ("  Data frames RECEIVED from network               = %d = %.3f Kbits (MSDU) \n", gts_statistics.received_frames, gts_statistics.received_bits*0.001);	
					printf ("  Number of the GTS periods during the simulation = %d \n", gts_statistics.gts_periods_count);
					printf ("  GTS Throughtput: \n");
					printf ("    Average wasted bandwidth = %.3f %% \n", (gts_statistics.band_wast_sum/gts_statistics.gts_periods_count)*100);
					printf ("    Average throughput = %.3f %% \n", (1-gts_statistics.band_wast_sum/gts_statistics.gts_periods_count)*100);
					
				
					if (gts_statistics.sent_frames > 0) {
						 printf ("  Frame delay in the buffer (from storing to the bufefr to dispatching):\n");
						printf ("    max = %.3f sec \n", gts_statistics.frame_delay_max);
						printf ("    min = %.3f sec \n", gts_statistics.frame_delay_min);
						printf ("    average = %.3f sec \n", gts_statistics.frame_delay_sum/gts_statistics.sent_frames);
					}
					
					printf ("|-----------------------------------------------------------------------------|\n\n");
					
					*/
					/* scalar statistics */
					op_stat_scalar_write ("Real-time data frames STORED in the buffer", gts_statistics.stored_frames);
					op_stat_scalar_write ("Real-time data frames DISPATCHED to the network (GTS)", gts_statistics.sent_frames);
					op_stat_scalar_write ("Real-time data frames RECEIVED from network (GTS)", gts_statistics.received_frames);
				}				
				
				fclose(log_file);
			}

			break;
		};	/*end of OPC_INTRPT_ENDSIM */
		
		case OPC_INTRPT_STAT: /* statistic interrupt from PHY layer */
		{
		
			switch (op_intrpt_stat()) {	/*begin switch (op_intrpt_stat())*/ 
				
				case RX_BUSY_STAT :	/* represents whether any packets are currently being received or transmitted on a channel */
				{
					/* if during the CCA the channel was busy for a while, then SF_ptr->csma.CCA_CHANNEL_IDLE = OPC_FALSE*/
				//	 if (op_stat_local_read(RX_BUSY_STAT) == 1.0)
				//		SF_ptr->csma.CCA_CHANNEL_IDLE = OPC_FALSE;
				
					/*Try to send a packet if any*/
				//	if (op_stat_local_read (TX_BUSY_STAT) == 0.0) {
				//		op_intrpt_schedule_self (op_sim_time(), TRY_PACKET_TRANSMISSION_CODE);						
				//	}
					
					break;
				}
				
				case TX_BUSY_STAT : /* represents whether any packets are currently being received or transmitted on a channel */
				{
				//	if (op_stat_local_read (TX_BUSY_STAT) == 0.0) {						
				//		op_intrpt_schedule_self (op_sim_time(), TRY_PACKET_TRANSMISSION_CODE);
				//	} 
					
					break;
				}
				
				case RX_COLLISION_STAT :
				{
// printf("AAAA [%s (#%d)] SF_out SLEEP %d, CAP %d, CFP %d, SF_in SLEEP %d, CAP %d, CFP %d \n\n", device_name, my_address, SF_params[SF_out].SLEEP,SF_params[SF_out].CAP_ACTIVE, SF_params[SF_out].CFP_ACTIVE, SF_params[SF_in].SLEEP,SF_params[SF_in].CAP_ACTIVE, SF_params[SF_in].CFP_ACTIVE);

					if ((SF_params[SF_out].SLEEP == OPC_FALSE) || (SF_params[SF_in].SLEEP == OPC_FALSE)) {					
						collision_occurred = OPC_TRUE;
						
						if (enable_log){
							fprintf(log_file,"t=%f  -> PACKETS COLLISION !!!!!!  \n\n", op_sim_time());
							//printf(" [%s (#%d)] t=%f  -> PACKETS COLLISION !!!!!! \n", device_name, my_address, op_sim_time());
						}
					}
					
					break;
				}
			}/*end switch (op_intrpt_stat())*/
			
			break;
		};/*end of OPC_INTRPT_STAT */
		
		default :
		{
		};
		
	} /*end switch (op_intrpt_type())*/
	
	/* Stack tracing exit point */
	FOUT;
}

/*--------------------------------------------------------------------------------
 * Function:	wpan_CAP_packet_search
 *
 * Description:	Is there any packet for sending in the CAP subqueue?
 *        
 *             
 * Output:
 *		int 	index of the packet in the CAP subqueue
 *--------------------------------------------------------------------------------*/
static int wpan_CAP_packet_search () {	

	Packet* frame_MPDU;
	int dest_address;
	int num_pkts; 
	int i;

	/* Stack tracing enrty point */
	FIN(wpan_CAP_packet_search);
	
	/* check the subqueue SUBQ_CAP */
	if (!op_subq_empty(SUBQ_CAP)) {
					
		/* determine the number of packets in subqueue */
		num_pkts = op_subq_stat (SUBQ_CAP, OPC_QSTAT_PKSIZE);
					
		for (i=0; i<num_pkts; i++) {
			frame_MPDU = op_subq_pk_access (SUBQ_CAP, i);
			op_pk_nfd_get(frame_MPDU, "Destination Address", &dest_address);
			
// printf("AAAA [%s (#%d)] dest_address = %d, my_parent_address = %d, SF_in = %d,SF_out = %d \n\n",device_name, my_address, dest_address, my_parent_address, SF_params[SF_in].CAP_ACTIVE,SF_params[SF_out].CAP_ACTIVE);
							
			if (((dest_address == my_parent_address) && SF_params[SF_in].CAP_ACTIVE) || ((dest_address != my_parent_address) && SF_params[SF_out].CAP_ACTIVE)) {
				return i;
				
				/* Stack tracing exit point */
				FRET(i);				
			} 				
		}		
	} else {
		if (enable_log) {
			fprintf(log_file,"t=%f  -> The CAP subqueue is EMPTY. Waiting for the incomming best-effort packet <- \n\n", op_sim_time());
			//printf(" [%s (#%d)] t=%f  -> The CAP subqueue is EMPTY. Waiting for the incomming best-effort packet <- \n\n", device_name, my_address, op_sim_time());
		}	
	
	}
	
	return NOT_DEFINED;
	
	/* Stack tracing exit point */
	FRET(NOT_DEFINED);
}


/*--------------------------------------------------------------------------------
 * Function:	wpan_backoff_init
 *
 * Description:	Initializes the backoff process
 *
 *    			NB = 0   the number of times the CSMA/CA was required to backoff while
 *              		attempting to access the current channel. this value is initialized
 *              		to zero before each new transmission attempts
 *     
 *    			CW = 2   in the contention window length which defines the number of backoff 
 *             			periods that need to be clear of channel activity before starting transmission
 *              		CW initialized to 2 before each transmission attempt and reset to 2 each 
 *              		time the channel is assessed to be busy. 
 *     
 *     			BE = min(2,macMinBe)   is the backoff exponent, which is related to how many backoff period 
 *                           		a device must wait before attempting to assess the channel activity.
 *        
 *             
 * No parameters 
 *--------------------------------------------------------------------------------*/
static void wpan_backoff_init() {	

	/* Stack tracing enrty point */
	FIN(wpan_backoff_init);

	if (enable_log) {
		fprintf(log_file," -> BACKOFF INIT t=%f \n\n", op_sim_time());
		//printf(" [%s (#%d)] t=%f  -> BACKOFF INIT  \n\n", device_name, my_address, op_sim_time());
	}
	
	SF_ptr->csma.NB = 0;
	SF_ptr->csma.CW = 2;
	SF_ptr->csma.BE = csma_params.macMinBE;

	if (csma_params.macBattLifeExt == OPC_TRUE)
		SF_ptr->csma.BE = min_int(2, csma_params.macMinBE);
	
	/* Stack tracing exit point */
	FOUT;
}


/*--------------------------------------------------------------------------------
 * Function:	wpan_backoff_delay_set()
 *
 * Description:	set the backoff timer to a random value
 *             
 * No parameters
 *--------------------------------------------------------------------------------*/

static void wpan_backoff_delay_set () {	
	
	int backoff_unit;
	int backoff_max;
	double backoff_time;
	double end_cap_time;
	double backoff_expiration_time;
	double remaining_cap_time;
	double cap_length;
	
	/* Stack tracing enrty point */
	FIN(wpan_backoff_delay_set);
	
	backoff_max = (int) (01<<SF_ptr->csma.BE) - 1;      //(2^BE-1)
	backoff_unit = floor (op_dist_uniform (backoff_max+1)); // Randon number of backoffunits
	op_stat_write(statistic_vector.backoff_units, backoff_unit);
	
	backoff_time = ((double) backoff_unit * Symbols2Sec(aUnitBackoffPeriod, WPAN_DATA_RATE));
	op_stat_write(statistic_vector.backoff_delay, backoff_time);
	
	/*check if the backoff time will exceed the remaining time in the CAP */	
	end_cap_time = SF_ptr->BI_Boundary + Symbols2Sec(((SF_ptr->Final_CAP_Slot+1)*SF_ptr->slot_duration), WPAN_DATA_RATE);
	backoff_expiration_time = wpan_backoff_period_boundary_get() + backoff_time;	
	remaining_cap_time = end_cap_time - wpan_backoff_period_boundary_get();	
	cap_length = (SF_ptr->Final_CAP_Slot+1)*Symbols2Sec(SF_ptr->slot_duration, WPAN_DATA_RATE);		
	
	/* the backoff is accepted if it expires at most aUnitBackoffPeriod before the end of CAP */	
	if (compare_doubles((remaining_cap_time - Symbols2Sec(aUnitBackoffPeriod, WPAN_DATA_RATE)), backoff_time) >=0) {	// THERE IS A PROBLEM WITH EQUALITY IN DOUBLE
	
		if (enable_log) {
			fprintf(log_file,"t=%f  -> ENTERS BACKOFF  - BE = %d; RANDOM BACKOFF = % d; EXPIRATION AT %f  \n\n", op_sim_time(), SF_ptr->csma.BE,backoff_unit,backoff_expiration_time );
			//printf(" [%s (#%d)] t=%f  -> ENTERS BACKOFF  - BE = %d; RANDOM BACKOFF = % d; EXPIRATION AT %f  \n\n", device_name, my_address, op_sim_time(), SF_ptr->csma.BE,backoff_unit,backoff_expiration_time );
		}

		SF_ptr->csma.RESUME_BACKOFF_TIMER = OPC_FALSE;
		SF_ptr->csma.backoff_timer = 0;
		op_intrpt_schedule_self (backoff_expiration_time, BACKOFF_EXPIRATION_CODE);			
	}
	else {	/* if the remaining period of the CAP is lower than the backoff period the timer must wake up at the CAP of the next superframe */
		
		if (enable_log) {
			fprintf(log_file,"t=%f  -> BACKOFF PAUSED UNTIL THE CAP OF THE NEXT SUPERFRAME  \n\n", op_sim_time());	
			//printf(" [%s (#%d)] t=%f  -> BACKOFF PAUSED UNTIL THE CAP OF THE NEXT SUPERFRAME  \n\n", device_name, my_address, op_sim_time());
		}

		SF_ptr->csma.RESUME_BACKOFF_TIMER = OPC_TRUE;
		SF_ptr->csma.backoff_timer = backoff_time - remaining_cap_time;
		backoff_expiration_time = NOT_DEFINED;
		op_stat_write (statistic_vector.deferred_cca_backoff, SF_ptr->csma.RESUME_BACKOFF_TIMER);
		
		op_intrpt_schedule_self (op_sim_time()+remaining_cap_time, BACKOFF_DEFER_CODE); /* go to 'IDLE' state */
	}		
	
	if (enable_log) {	
	/*
	printf ("-------------------------- BACKOFF -----------------------------------\n");
		printf ("  ENTERS BACKOFF STATUT AT %f\n", op_sim_time());
		printf ("  Beacon Boundary = %f\n", SF_ptr->BI_Boundary);
		printf ("  NB = %d (macMaxCSMABackoffs = %d)\n", SF_ptr->csma.NB, csma_params.macMaxCSMABackoffs);
		printf ("  CW = %d\n", SF_ptr->csma.CW);
		printf ("  BE = %d\n", SF_ptr->csma.BE);
		printf ("  2^BE-1 = %d\n", backoff_max);
		printf ("  Random Backoff units = %d\n", backoff_unit);
		printf ("    + Random Backoff time  = %f sec \n", backoff_time);
		printf ("    + Remaining CAP Length = %f sec \n", remaining_cap_time);
		printf ("    + Resume backoff time  = %f sec \n", SF_ptr->csma.backoff_timer);
		printf ("  Total CAP Length = %f sec\n", cap_length);
		printf ("  Beacon Interval = %f sec = %d Symbols\n", Symbols2Sec(SF_ptr->BI, WPAN_DATA_RATE), SF_ptr->BI);
		printf ("  Slot Duration = %f sec = %d Symbols \n", Symbols2Sec(SF_ptr->slot_duration, WPAN_DATA_RATE), SF_ptr->slot_duration);
		printf ("  Backoff Boundary Index = %d\n", wpan_backoff_period_index_get());
		printf ("  Current Time Slot = %d\n", wpan_current_time_slot_get());
		printf ("  \n  CAP STATUT = %s \n", boolean2string(SF_ptr->CAP_ACTIVE));
		printf ("  CFP STATUT = %s \n", boolean2string(SF_ptr->CFP_ACTIVE));
		printf ("  SLEEP STATUT = %s \n", boolean2string(SF_ptr->SLEEP));
		printf ("  Backoff Boundary = %f sec \n", wpan_backoff_period_boundary_get());
		printf ("  End CAP Time     = %f sec \n", end_cap_time);
		printf ("  Difference       = %f sec \n", end_cap_time-wpan_backoff_period_boundary_get());
		
		if (backoff_expiration_time == NOT_DEFINED)
			printf ("  BackOff deferred \n");
		else
			printf ("  BackOff Expiration Time  = %f sec\n", backoff_expiration_time);
		
		printf ("----------------------------------------------------------------------------\n\n");
	*/
		fprintf (log_file, "-------------------------- BACKOFF -----------------------------------\n");
		fprintf (log_file, "ENTERS BACKOFF STATUT AT %f\n", op_sim_time());
		fprintf (log_file, "  Beacon Boundary = %f\n", SF_ptr->BI_Boundary);
		fprintf (log_file, "  NB = %d (macMaxCSMABackoffs = %d)\n", SF_ptr->csma.NB, csma_params.macMaxCSMABackoffs);
		fprintf (log_file, "  CW = %d\n", SF_ptr->csma.CW);
		fprintf (log_file, "  BE = %d\n", SF_ptr->csma.BE);
		fprintf (log_file, "  2^BE-1 = %d\n", backoff_max);
		fprintf (log_file, "  Random Backoff units = %d\n", backoff_unit);
		fprintf (log_file, "    + Random Backoff time  = %f sec \n", backoff_time);
		fprintf (log_file, "    + Remaining CAP Length = %f sec \n", remaining_cap_time);
		fprintf (log_file, "    + RESUME BACKOFF TIME  = %f sec \n", SF_ptr->csma.backoff_timer);
		fprintf (log_file, "  Total CAP Length = %f sec \n", cap_length);
		fprintf (log_file, "  Beacon Interval = %f sec = %d Symbols\n", Symbols2Sec(SF_ptr->BI, WPAN_DATA_RATE), SF_ptr->BI);
		fprintf (log_file, "  Slot Duration = %f sec = %d Symbols \n", Symbols2Sec(SF_ptr->slot_duration, WPAN_DATA_RATE), SF_ptr->slot_duration);
		fprintf (log_file, "  Backoff Boundary Index = %d\n", wpan_backoff_period_index_get());
		fprintf (log_file, "  Current Time Slot = %d\n", wpan_current_time_slot_get());
		fprintf (log_file, "  \n  CAP STATUT = %s \n", boolean2string(SF_ptr->CAP_ACTIVE));
		fprintf (log_file, "  CFP STATUT = %s \n", boolean2string(SF_ptr->CFP_ACTIVE));
		fprintf (log_file, "  SLEEP STATUT = %s \n", boolean2string(SF_ptr->SLEEP));
		fprintf (log_file, "  Backoff Boundary = %f sec \n", wpan_backoff_period_boundary_get());
		fprintf (log_file, "  End CAP Time     = %f sec \n", end_cap_time);
		fprintf (log_file, "  Difference       = %f sec \n", end_cap_time-wpan_backoff_period_boundary_get());
		
		if (backoff_expiration_time != NOT_DEFINED)
			fprintf (log_file, "  BackOff deferred \n");
		else
			fprintf (log_file, "  BackOff Expiration Time  = %f sec \n", backoff_expiration_time);
		
		fprintf (log_file, "----------------------------------------------------------------------------\n\n");
	}
	
	/* Stack tracing exit point */
	FOUT;
}

/*--------------------------------------------------------------------------------
 * Function:	wpan_backoff_period_index_get         BI_Boundary                      
 *
 * Description:	 the next backoff period index,               |-----|-----|-----|--t--|-----|
 *             
 *                                                                                    ^
 * No parameters                                                                      |
 *                                                                           next backoff period index
 *--------------------------------------------------------------------------------*/

static int wpan_backoff_period_index_get() {

	double aUnitBackoffPeriod_Sec;
	int next_backoff_period_index;
	
	/* Stack tracing enrty point */
	FIN(wpan_backoff_period_index_get);
	
	if (SF_ptr == OPC_NIL) {
		printf (" [%s (#%d)] t=%f  -> ERROR: Superframe pointer (SF_ptr) is empty (OPC_NIL) \n\n", device_name, my_address, op_sim_time());
		op_sim_end("ERROR - wpan_backoff_period_index_get","Superframe pointer (SF_ptr) is empty (OPC_NIL)","","");
	}
	
	aUnitBackoffPeriod_Sec = Symbols2Sec(aUnitBackoffPeriod, WPAN_DATA_RATE);
	next_backoff_period_index = (int) ceil((double)(op_sim_time()-SF_ptr->BI_Boundary)/aUnitBackoffPeriod_Sec);
	
	
	return (next_backoff_period_index);
	
	/* Stack tracing exit point */
	FRET(next_backoff_period_index);
}


/*--------------------------------------------------------------------------------
 * Function:	wpan_current_time_slot_get
 *
 * Description:	get index of the current time slot (superframe slot)              
                                                                                     
 * No parameters                           
 *--------------------------------------------------------------------------------*/
static int wpan_current_time_slot_get() {

	int current_time_slot_index;
	
	/* Stack tracing enrty point */
	FIN(wpan_current_time_slot_get);

	if (SF_ptr == OPC_NIL) {
		//printf (" [%s (#%d)] t=%f  -> ERROR: Superframe pointer (SF_ptr) is empty (OPC_NIL) \n\n", device_name, my_address, op_sim_time());
		op_sim_end("ERROR - wpan_current_time_slot_get","Superframe pointer (SF_ptr) is empty (OPC_NIL)","","");
	}
		
	current_time_slot_index = floor ((op_sim_time()-SF_ptr->BI_Boundary)/Symbols2Sec(SF_ptr->slot_duration, WPAN_DATA_RATE));
		
	return (current_time_slot_index);
	
	/* Stack tracing exit point */
	FRET(current_time_slot_index);
}


/*--------------------------------------------------------------------------------
 * Function:	wpan_backoff_period_index_get_                              
 *
 * Description:	 the next backoff period index                           
 *                                                                            
 * Input: time                                                                         
 *--------------------------------------------------------------------------------*/
static int wpan_backoff_period_index_get_(double Time) {

	double aUnitBackoffPeriod_Sec;
	int next_backoff_period_index;
	
	/* Stack tracing enrty point */
	FIN(wpan_backoff_period_index_get_);
	
	if (SF_ptr == OPC_NIL) {
		//printf (" [%s (#%d)] t=%f  -> ERROR: Superframe pointer (SF_ptr) is empty (OPC_NIL) \n\n", device_name, my_address, op_sim_time());
		op_sim_end("ERROR - wpan_backoff_period_index_get_","Superframe pointer (SF_ptr) is empty (OPC_NIL)","","");
	}
	
	aUnitBackoffPeriod_Sec = Symbols2Sec(aUnitBackoffPeriod, WPAN_DATA_RATE);
	next_backoff_period_index = (int) ceil((double)(Time-SF_ptr->BI_Boundary)/aUnitBackoffPeriod_Sec);	
	
	return (next_backoff_period_index);
	
	/* Stack tracing exit point */
	FRET(next_backoff_period_index);
}

/*--------------------------------------------------------------------------------
 * Function:	wpan_backoff_period_boundary_get
 *
 * Description:	it is the absolue time that represents the backoff period boundary, 
 *             
 * No parameters  
 *--------------------------------------------------------------------------------*/

static double wpan_backoff_period_boundary_get () {

	double next_backoff_period_boundary;
	double aUnitBackoffPeriod_Sec;
	
	/* Stack tracing enrty point */
	FIN(wpan_backoff_period_boundary_get);
	
	if (SF_ptr == OPC_NIL) {
		//printf (" [%s (#%d)] t=%f  -> ERROR: Superframe pointer (SF_ptr) is empty (OPC_NIL) \n\n", device_name, my_address, op_sim_time());
		op_sim_end("ERROR - wpan_backoff_period_boundary_get","Superframe pointer (SF_ptr) is empty (OPC_NIL)","","");
	}
	
	aUnitBackoffPeriod_Sec = Symbols2Sec(aUnitBackoffPeriod, WPAN_DATA_RATE);
	next_backoff_period_boundary = SF_ptr->BI_Boundary + (wpan_backoff_period_index_get() * aUnitBackoffPeriod_Sec);

	return (next_backoff_period_boundary);
	
	/* Stack tracing exit point */
	FRET(next_backoff_period_boundary);
}


/*--------------------------------------------------------------------------------
 * Function:	wpan_backoff_period_boundary_get_
 *
 * Description:	it is the absolue time that represents the backoff period boundary, 
 *             
 * Input: time  
 *--------------------------------------------------------------------------------*/

static double wpan_backoff_period_boundary_get_(double Time) {

	double next_backoff_period_boundary;
	double aUnitBackoffPeriod_Sec;
	
	/* Stack tracing enrty point */
	FIN(wpan_backoff_period_boundary_get_);
	
	if (SF_ptr == OPC_NIL) {
		//printf (" [%s (#%d)] t=%f  -> ERROR: Superframe pointer (SF_ptr) is empty (OPC_NIL) \n\n", device_name, my_address, op_sim_time());
		op_sim_end("ERROR - wpan_backoff_period_boundary_get_","Superframe pointer (SF_ptr) is empty (OPC_NIL)","","");
	}
	
	aUnitBackoffPeriod_Sec = Symbols2Sec(aUnitBackoffPeriod, WPAN_DATA_RATE);
	next_backoff_period_boundary = SF_ptr->BI_Boundary + (wpan_backoff_period_index_get_(Time) * aUnitBackoffPeriod_Sec);

	return (next_backoff_period_boundary);
	
	/* Stack tracing exit point */
	FRET(next_backoff_period_boundary);
}


/*--------------------------------------------------------------------------------
 * Function:	wpan_cca_defer
 *
 * Description:	check if the requested transaction can be completed before the end of the current CAP
 *							|CCA1|-|CCA2|-|PHY|FRAME|-||macAckWaitDuration + |PHY|ACK||-|IFS| or
 *							|CCA1|-|CCA2|-|PHY|FRAME|-|IFS|
 *             
 * No parameters  
 *--------------------------------------------------------------------------------*/

static void wpan_cca_defer() {

	double end_cap_time;
	double remaining_cap_time ;
	double tx_time;
	Packet* frame_to_send;
	int IFS;
	int ack_request;
	int frame_len_symbol;
	double ack_time_Symbol; // time need for ACK

	/* Stack tracing enrty point */
	FIN(wpan_cca_defer);	
	
	if (SF_params[SF_in].CAP_ACTIVE || SF_params[SF_out].CAP_ACTIVE) { /* CAP is active */
	
		frame_to_send = op_subq_pk_access(SUBQ_CAP, packet_to_send_index); /* obtains a pointer to a packet stored in the specified queue */
	
		/* get the frame size*/
		frame_len_symbol = Bits2Symbols((int) op_pk_total_size_get(frame_to_send));
	
		/* get the corresponding IFS of the frame*/
		IFS = wpan_ifs_symbol_get(frame_to_send);
	
		/*check if any acknowledgment is necessary for this frame */
		op_pk_nfd_get (frame_to_send, "Ack Request", &ack_request);
	
		if (ack_request)
			ack_time_Symbol = macAckWaitDuration;
			//ack_time_Symbol = (aTurnaroundTime_Symbol + aUnitBackoffPeriod)+Bits2Symbols(ACK_FRAME_SIZE_BITS)+Bits2Symbols(PHY_HEADER_SIZE);
		else 
				ack_time_Symbol = 0;
	
		/*check the time to perform complete transaction*/
		end_cap_time = SF_ptr->BI_Boundary + Symbols2Sec(((SF_ptr->Final_CAP_Slot+1)*SF_ptr->slot_duration), WPAN_DATA_RATE);
		remaining_cap_time = end_cap_time - wpan_backoff_period_boundary_get();	
	
		/* |CCA1|+|CCA2|+|PHY|FRAME|+|macAckWaitDuration|+|IFS| or |CCA1|+|CCA2|+|PHY|FRAME|+|IFS|*/ 
		tx_time = Symbols2Sec(((2*aUnitBackoffPeriod)+Bits2Symbols(PHY_HEADER_SIZE)+frame_len_symbol+ack_time_Symbol+IFS),WPAN_DATA_RATE);	
	
		SF_ptr->csma.CCA_DEFERRED = OPC_TRUE; /* CCA deferred to next CAP (cca_time > remaining_cap_time) */
		if (tx_time <= remaining_cap_time) {	/* perform CCA in this CAP */
			SF_ptr->csma.CCA_DEFERRED = OPC_FALSE;
			wpan_cca_perform (1);	
		}
	
		if (enable_log && SF_ptr->csma.CCA_DEFERRED) {			
			fprintf (log_file,"t=%f   -> -------------- CCA DEFERRED --------------   \n\n", op_sim_time());
			//printf (" [%s (#%d)] t=%f  -> --------------  CCA DEFERRED --------------   \n\n", device_name, my_address, op_sim_time());
			
			op_intrpt_schedule_self (op_sim_time() + remaining_cap_time, CCA_DEFER_CODE); /* go to 'IDLE' state */
		}
	/*
		if (enable_log) {
			printf ("-------------------------- CCA -----------------------------------\n", device_name);
			printf ("  ENTERS CCA AT %f\n", op_sim_time());
			printf ("   Beacon Boundary = %f\n", SF_ptr->BI_Boundary);
			printf ("   NB = %d\n", SF_ptr->csma.NB);
			printf ("   CW = %d\n", SF_ptr->csma.CW);
			printf ("   BE = %d\n", SF_ptr->csma.BE);	
			printf ("   * Frame Size  = %d Symbols\n", frame_len_symbol);
			printf ("   * IFS         = %d Symbols\n", IFS);	
			printf ("       + Requiered TX Time  = %f sec \n", tx_time);
			printf ("       + CCA Deferred       = %s  \n", boolean2string(SF_ptr->csma.CCA_DEFERRED));
			printf ("       + Remaining CAP Time = %f sec \n", remaining_cap_time);
			printf ("   Beacon Interval = %f  sec (%d symbols)\n", Symbols2Sec(SF_ptr->BI, WPAN_DATA_RATE), SF_ptr->BI);
			printf ("   Slot Duration = %f sec \n", Symbols2Sec(SF_ptr->slot_duration, WPAN_DATA_RATE));
			printf ("   Backoff Boundary Index = %d  \n",wpan_backoff_period_index_get());
			printf ("   Current Time Slot = %d \n", wpan_current_time_slot_get());
			printf ("  \n   CAP STATUT = %s  \n", boolean2string(SF_ptr->CAP_ACTIVE));
			printf ("   CFP STATUT = %s  \n", boolean2string(SF_ptr->CFP_ACTIVE));
			printf ("   SLEEP STATUT = %s \n", boolean2string(SF_ptr->SLEEP));
			printf ("----------------------------------------------------------------------------\n\n\n");
		}
	*/
		op_stat_write(statistic_vector.deferred_cca, SF_ptr->csma.CCA_DEFERRED);
		op_stat_write(statistic_vector.deferred_cca_backoff, SF_ptr->csma.CCA_DEFERRED);
	
	} else {
		//printf (" [%s (#%d)] t=%f  -> CAP IS NOT ACTIVE \n", device_name, my_address, op_sim_time());	
/*
		if (SF_ptr != OPC_NIL) {
			if (SF_ptr == &SF_params[SF_in])
				printf ("  Incoming CAP \n");
			else
				printf ("  Outgoing CAP \n");
	
			//printf ("\t\t CAP_ACTIVE = %s \n", boolean2string(SF_ptr->CAP_ACTIVE));
			printf ("\t\t CFP_ACTIVE = %s \n", boolean2string(SF_ptr->CFP_ACTIVE));
			printf ("\t\t SLEEP_ACTIVE = %s \n", boolean2string(SF_ptr->SLEEP));	
		} else {
			printf ("\t\t SF_ptr is equal to OPC_NIL. \n");
		}
		*/
		op_sim_end("ERROR - CSMA/CA","Back-off expired but CAP is not active!","","");	
	}
	
	/* Stack tracing exit point */
	FOUT;
}

/*--------------------------------------------------------------------------------
 * Function:	wpan_cca_perform
 *
 * Description:	check if it is possible to perform CCA analyses [CCA 1 (CW=2) -> CCA 2 (CW=1)]
 *             
 *
 * Input :  cca_index - CCA index
 *--------------------------------------------------------------------------------*/

static void wpan_cca_perform (int cca_index) {
	
	double cca_period;
	
	/* Stack tracing enrty point */
	FIN(wpan_cca_perform);	
	
	if (enable_log) {
		fprintf (log_file,"t=%f  -------------- PERFORM CCA %d (CW=%d) --------------  \n\n", op_sim_time(), 3-SF_ptr->csma.CW, SF_ptr->csma.CW);
		//printf (" [%s (#%d)] t=%f  -------------- PERFORM CCA %d (CW=%d) -------------- \n\n", device_name, my_address, op_sim_time(), 3-SF_ptr->csma.CW, SF_ptr->csma.CW);
	}

	if (SF_ptr->csma.CCA_DEFERRED == OPC_FALSE) {	
		if (cca_index == 1){ /* CCA 1 (CW=2)*/
			if (SF_ptr->csma.CW != 2)			
				op_sim_end ("wpan_cca_perform", "CW !=2 AT CCA 1","","");
			
			cca_period = wpan_backoff_period_boundary_get() + Symbols2Sec(WPAN_CCA_DETECTION_TIME_Symbols, WPAN_DATA_RATE);			
			op_intrpt_schedule_self (max_double(wpan_backoff_period_boundary_get(), op_sim_time()), CCA_START_CODE);			
		} else if (cca_index == 2) {/* CCA 2 (CW=1)*/
			if (SF_ptr->csma.CW != 1)
				op_sim_end ("wpan_cca_perform", "CW !=1 AT CCA 2","","");
			
			cca_period = wpan_backoff_period_boundary_get() + Symbols2Sec(WPAN_CCA_DETECTION_TIME_Symbols, WPAN_DATA_RATE);		
			op_intrpt_schedule_self (max_double(wpan_backoff_period_boundary_get(), op_sim_time()), CCA_START_CODE);
		} else if (cca_index == 3) {/* Deferred CCA resumed after the start of the CAP (CW=2)*/
			if (SF_ptr->csma.CW != 2)
				op_sim_end("wpan_cca_perform", "CW !=2 AT CCA 1 After a Resume","","");
			
			cca_period = wpan_backoff_period_boundary_get()+Symbols2Sec(WPAN_CCA_DETECTION_TIME_Symbols, WPAN_DATA_RATE);
			op_intrpt_schedule_self (max_double(wpan_backoff_period_boundary_get(), op_sim_time()), CCA_START_CODE);
		}	

		op_intrpt_schedule_self (cca_period, CCA_EXPIRATION_CODE);
	}
	else {
	/*do nothing, just wait when the CCA comes back */
	}	

	/* Stack tracing exit point */
	FOUT;
}


/*--------------------------------------------------------------------------------
 * Function:	wpan_ifs_symbol_get
 *
 * Description:	get the intra-frame spacing of a given packet based on its size (MPDU)
 *             
 *  Input :  frame_MPDU - MAC frame (MPDU)
 *  Output:  LIFS or SIFS [symbols]
 *--------------------------------------------------------------------------------*/

static int wpan_ifs_symbol_get(Packet* frame_MPDU) {

	int frame_len;
	int ifs;
	
	/* Stack tracing enrty point */
	FIN(wpan_ifs_symbol_get);
	
	frame_len = op_pk_total_size_get(frame_MPDU);
	
	ifs = aMinSIFSPeriod;
	
	if (frame_len > aMaxSIFSFrameSize_Bits) 
		ifs = aMinLIFSPeriod;

	return (ifs);
	
	/* Stack tracing exit point */
	FRET(ifs);
}


/*--------------------------------------------------------------------------------
 * Function:	wpan_cca_expire
 *
 * Description:	check if it is possible to perform next CCA analyses 
 *             
 * No parameters
 *--------------------------------------------------------------------------------*/

static void wpan_cca_expire() {	

	/* Stack tracing enrty point */
	FIN(wpan_cca_expire);	

	/* check at the CCA end, if the channel is busy */
	if (op_stat_local_read (RX_BUSY_STAT) == 1.0)
		SF_ptr->csma.CCA_CHANNEL_IDLE = OPC_FALSE;
	
	if (enable_log) {
		fprintf(log_file,"t=%f   -------- CCA EXPIRES (CW = %d) ----->  CHANNEL IDLE = %s \n\n", op_sim_time(), SF_ptr->csma.CW, boolean2string(SF_ptr->csma.CCA_CHANNEL_IDLE));
		//printf(" [%s (#%d)] t=%f  -------- CCA EXPIRES (CW = %d) ----->  CHANNEL IDLE = %s \n", device_name, my_address, op_sim_time(), SF_ptr->csma.CW, boolean2string(SF_ptr->csma.CCA_CHANNEL_IDLE));
	}
	
	if (SF_ptr->csma.CW == 2) {
		op_stat_write(statistic_vector.ch_busy_cca1, 1-SF_ptr->csma.CCA_CHANNEL_IDLE);
		op_stat_write(statistic_vector.ch_idle_cca1, SF_ptr->csma.CCA_CHANNEL_IDLE);
	} else {
		op_stat_write(statistic_vector.ch_busy_cca2, 1-SF_ptr->csma.CCA_CHANNEL_IDLE);
		op_stat_write(statistic_vector.ch_idle_cca2, SF_ptr->csma.CCA_CHANNEL_IDLE);
	}	
	
	/*THE CHANNEL IS BUSY */			
	if (SF_ptr->csma.CCA_CHANNEL_IDLE == OPC_FALSE) {
		SF_ptr->csma.NB = SF_ptr->csma.NB+1;
		SF_ptr->csma.CW = 2;
		SF_ptr->csma.BE = min_int(SF_ptr->csma.BE+1,aMaxBE);		
				
		if (SF_ptr->csma.NB <= csma_params.macMaxCSMABackoffs) { /* redo another backoff */
			if (enable_log) {
				fprintf(log_file,"t=%f  ------ CCA BUSY CHANNEL - RETURN BACK TO BACKOFF ------> NB = %d \n\n",op_sim_time(), SF_ptr->csma.NB);
				//printf(" [%s (#%d)] t=%f  ------ CCA BUSY CHANNEL - RETURN BACK TO BACKOFF ------> NB = %d \n", device_name, my_address, op_sim_time(), SF_ptr->csma.NB);
			}		
		
			op_intrpt_schedule_self (op_sim_time(), RETURN_TO_BACKOFF_CODE);
		}
		else {	/* report a transmission failure */
			
			if (enable_log) {
				fprintf(log_file,"t=%f  -------- CCA TRANSMISSION FAILURE - PACKET WAS NOT SENT ------> \t\t NB = %d (macMaxCSMABackoffs = %d) \n\n", op_sim_time(), SF_ptr->csma.NB, csma_params.macMaxCSMABackoffs);
				//printf(" [%s (#%d)] t=%f  -------- CCA TRANSMISSION FAILURE - PACKET WAS NOT SENT ------> \t\t NB = %d (macMaxCSMABackoffs = %d) \n", device_name, my_address, op_sim_time(), SF_ptr->csma.NB, csma_params.macMaxCSMABackoffs);
			}
			
			op_stat_write(statistic_vector.failure, 1.0);
			op_stat_write(statistic_vector.success, 0.0);
			op_intrpt_schedule_self (op_sim_time(), FAILURE_CODE); /* go to 'IDLE' state */
		}
	} else { /*THE CHANNEL IS IDLE */
		/*check the value of the contention windows*/
		SF_ptr->csma.CW = SF_ptr->csma.CW-1;
			
		if (SF_ptr->csma.CW != 0){ /* perform the second CCA. we don't test if cca will be deferred only in the first test */
				
			if (enable_log) {
				fprintf (log_file, "t=%f  -------- IDLE CHANNEL -> GO TO 2nd STEP of CCA  ------- \n\n", op_sim_time());	
				//printf (" [%s (#%d)] t=%f  -------- IDLE CHANNEL -> GO TO 2nd STEP of CCA  ------- \n", device_name, my_address, op_sim_time());
			}

			wpan_cca_perform (2);
		} else { /*SUCCESS (CW=0) - FRAME TRANSMISSION */
				
			if (enable_log) {
				fprintf(log_file,"t=%f  -------- IDLE CHANNEL - TX IS SCHEDULED ON BACKOFF BOUNDARY [%f sec]  \n\n",op_sim_time(), wpan_backoff_period_boundary_get());
				//printf("\n [%s (#%d)] t=%f  -------- IDLE CHANNEL - TX IS SCHEDULED ON BACKOFF BOUNDARY [%f sec]  \n\n", device_name, my_address, op_sim_time(), wpan_backoff_period_boundary_get());
			}
		
			op_stat_write(statistic_vector.failure, 0.0);
			op_stat_write(statistic_vector.success, 1.0);
			op_intrpt_schedule_self (wpan_backoff_period_boundary_get(), START_TRANSMISSION_CODE); 				
		}				
	}
	
	/* Stack tracing exit point */
	FOUT;
}


/*--------------------------------------------------------------------------------
 * Function:	wpan_update_sequence_number()
 *
 * Description:	generate a random sequence number to not interfere with others
 *
 * No parameters  
 *--------------------------------------------------------------------------------*/

static int wpan_update_sequence_number() {
	
	int seq_num;
	int max_seq;
	
	/* Stack tracing enrty point */
	FIN(wpan_update_sequence_number);
	
	max_seq = 255;
		
	seq_num = floor (op_dist_uniform (max_seq));
	
	return(seq_num);
	
	/* Stack tracing exit point */
	FRET(seq_num);
}


/*--------------------------------------------------------------------------------
 * Function:	wpan_battery_update_tx
 *
 * Description:	send information about the operation to do to the battery module 
 *
 * Input : 	pksize - size of sent packet [bits]
 *--------------------------------------------------------------------------------*/
static void wpan_battery_update_tx(double pksize) {
	
	Ici * iciptr;
	
	/* Stack tracing enrty point */
	FIN(wpan_battery_update_tx);
	
	iciptr = op_ici_create ("wpan_battery_ici_format");
	op_ici_attr_set (iciptr, "Packet Size", pksize);
	op_ici_attr_set (iciptr, "WPAN DATA RATE", WPAN_DATA_RATE);
	op_ici_install (iciptr);
	op_intrpt_schedule_remote (op_sim_time(), PACKET_TX_CODE, baterry_id); 
	op_ici_install (OPC_NIL);
	
	/* Stack tracing exit point */
	FOUT;
}




/*--------------------------------------------------------------------------------
 * Function:	wpan_battery_update_rx
 *
 * Description:	send information about the operation to do to the battery module 
 *
 * Input :	pksize - size of received packet
 *			frame_type - type of received packet
 *--------------------------------------------------------------------------------*/
static void wpan_battery_update_rx(double pksize, int frame_type) {
	
	Ici * iciptr;
	
	/* Stack tracing enrty point */
	FIN(wpan_battery_update_rx);
	
	iciptr = op_ici_create ("wpan_battery_ici_format");
	op_ici_attr_set (iciptr, "Packet Size", pksize);
	op_ici_attr_set (iciptr, "WPAN DATA RATE", WPAN_DATA_RATE);
	op_ici_attr_set (iciptr, "Frame Type", frame_type);
	op_ici_install (iciptr);
	op_intrpt_schedule_remote (op_sim_time(), PACKET_RX_CODE, baterry_id); 
	op_ici_install (OPC_NIL);
	
	/* Stack tracing exit point */
	FOUT;
}


/*-----------------------------------------------------------------
 *     UTILITY FUNCTIONS
 *-----------------------------------------------------------------
*/

/*-----------------------------------------------------------------------------
 * Function:	gts_nested_list_mem_copy_create
 *
 * Description:	provide a hard-copy of the GTS_list (list containing wpan_gts_descriptor records).
 *				This function is used in the "op_pk_nfd_set_ptr()" as a copy procedure and therefore
 *				it has to match the following function prototype: void* fce (void*, usingned int);
 *              
 * Input :	list_ptr - pointer to the GTS_list to be copied
 *			size - formal parameter - is not used in this function
 *
 * Return :	void* - pointer to the copy of the GTS_list
 *-----------------------------------------------------------------------------*/

static void* gts_nested_list_mem_copy_create(void* list_ptr, unsigned int size)
{
	List* list_copy;
	void* from_list_ptr;
	wpan_gts_descriptor* gts_descriptor_ptr;
	int i;	
	
	/* Stack tracing enrty point */
	FIN(gts_nested_list_mem_copy_create);
	
	/* allocate an empty list */
	list_copy = op_prg_list_create ();
	
	for (i=0; i<op_prg_list_size ((List* ) list_ptr); i++) {
		from_list_ptr = op_prg_list_access ((List* ) list_ptr, i);
		
		/* memory allocation */
		gts_descriptor_ptr	= (wpan_gts_descriptor *) op_prg_mem_alloc (sizeof (wpan_gts_descriptor));
		
		gts_descriptor_ptr = (wpan_gts_descriptor *) from_list_ptr;
		gts_descriptor_ptr->device_short_address = ((wpan_gts_descriptor *) from_list_ptr)->device_short_address;
		gts_descriptor_ptr->start_slot = ((wpan_gts_descriptor *) from_list_ptr)->start_slot;
		gts_descriptor_ptr->length = ((wpan_gts_descriptor *) from_list_ptr)->length;
		
		/* insert the GTS descriptor onto the list tail */
		op_prg_list_insert (list_copy, gts_descriptor_ptr, OPC_LISTPOS_TAIL);
	}	
	
	return (void* ) list_copy;
	
	/* Stack tracing exit point */
	FRET((void* ) list_copy);
}


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

static void gts_nested_list_mem_free(void* from_ptr)
{
	/* Stack tracing enrty point */
	FIN(gts_nested_list_mem_free);
	
	/* deallocate memory of the list */
	op_prg_mem_free (from_ptr);
	
	/* Stack tracing exit point */
	FOUT;
}


/*-----------------------------------------------------------------------------
 * Function:	queue_status
 *
 * Description:	print the status of each subqueue
 *              
 * No parameters
 *-----------------------------------------------------------------------------*/

static void queue_status() {
	
	Objid queue_objid;
	Objid subq_objid;
	double bit_capacity;
	double pk_capacity;
	int subq_count;
	int i;
	char* queue_name;
	
	/* Stack tracing enrty point */
	FIN(queue_status);

	/* get the subqueues object ID */
	op_ima_obj_attr_get (self_id, "subqueue", &queue_objid);
	
	/* obtain how many subqueues exist */
	subq_count = op_topo_child_count (queue_objid, OPC_OBJMTYPE_ALL);
	
	/* get the object IDs of each subqueue and get subqueue attributes  */
	for (i = 0; i < subq_count; i++)
	{
		/* Obtain object ID of the ith subqueue */
		subq_objid = op_topo_child (queue_objid, OPC_OBJMTYPE_ALL, i);
		
		/* Get current subqueue attribute settings */
		op_ima_obj_attr_get (subq_objid, "bit capacity", &bit_capacity);
		op_ima_obj_attr_get (subq_objid, "pk capacity", &pk_capacity);
		
		if (i == SUBQ_CAP)
			queue_name = "CAP";
		else
			queue_name = "CFP";

		
		if (op_subq_empty(i)) {
			if (enable_log) {	
				fprintf(log_file,"t=%f  -> Subqueue %s (#%d) is empty, waiting for MAC frames \n\t -> capacity [%.0f frames, %.0f bits]. \n\n", op_sim_time(), queue_name, i, pk_capacity, bit_capacity);
			//	printf(" [%s (#%d)] t=%f  -> Subqueue %s (#%d) is empty, waiting for MAC frames \n\t -> capacity [%.0f frames, %.0f bits]. \n\n", device_name, my_address, op_sim_time(), queue_name, i, pk_capacity, bit_capacity);
			}
		} else {
			if (enable_log) {	 
				fprintf(log_file,"t=%f  -> Subqueue %s (#%d) is non empty, \n\t -> occupied space [%.0f frames, %.0f bits] - empty space [%.0f frames, %.0f bits] \n\n", op_sim_time(), queue_name, i, op_subq_stat (i, OPC_QSTAT_PKSIZE), op_subq_stat (i, OPC_QSTAT_BITSIZE), op_subq_stat (i, OPC_QSTAT_FREE_PKSIZE), op_subq_stat (i, OPC_QSTAT_FREE_BITSIZE));
				//printf(" [%s (#%d)] t=%f  -> Subqueue %s (#%d) is non empty,\n\t -> occupied space [%.0f frames, %.0f bits] - empty space [%.0f frames, %.0f bits] \n\n", device_name, my_address, op_sim_time(), queue_name, i, op_subq_stat (i, OPC_QSTAT_PKSIZE), op_subq_stat (i, OPC_QSTAT_BITSIZE), op_subq_stat (i, OPC_QSTAT_FREE_PKSIZE), op_subq_stat (i, OPC_QSTAT_FREE_BITSIZE));
			}
		}	
	}
	
	/* Stack tracing exit point */
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
#undef node_attributes
#undef csma_params
#undef GTS_params
#undef GTS_ptr
#undef GTS_list_ptr
#undef GTS_list_temp
#undef SF_params
#undef SF_ptr
#undef statistic_vector
#undef statistic_global_vector
#undef ack_sequence_number
#undef backoff_start_time
#undef Final_CAP_slot_next
#undef sent_gts_bits
#undef gts_statistic_vector
#undef gts_statistics
#undef gts_pkt_delay
#undef cfp_queue_capacity
#undef cap_queue_capacity
#undef start_GTS_transmission
#undef waiting_for_first_beacon
#undef superframe_owner
#undef self_id
#undef parent_id
#undef baterry_id
#undef my_address
#undef device_mode
#undef device_name
#undef enable_log
#undef log_file
#undef my_parent_address
#undef packet_to_send_index
#undef collision_occurred
#undef rx_power_threshold
#undef Flow_Tables_list
#undef flowTable_sending_needed
#undef PANsink_mac_address
#undef NDP_is_required
#undef NDP_beacon_seq_num
#undef Neighbour_list
#undef No_of_neighbours
#undef tdp_id
#undef tdp_decision
#undef tdp_code
#undef startTimeSoBo_list_ptr
#undef flowEntriesP
#undef grandfatherID
#undef Run1time

/* Access from C kernel using C linkage */
extern "C"
{
	VosT_Obtype _op_SDNC_MAC_init (int * init_block_ptr);
	VosT_Address _op_SDNC_MAC_alloc (VosT_Obtype, int);
	void SDNC_MAC (OP_SIM_CONTEXT_ARG_OPT)
		{
		((SDNC_MAC_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))->SDNC_MAC (OP_SIM_CONTEXT_PTR_OPT);
		}

	void _op_SDNC_MAC_svar (void *, const char *, void **);

	void _op_SDNC_MAC_diag (OP_SIM_CONTEXT_ARG_OPT)
		{
		((SDNC_MAC_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))->_op_SDNC_MAC_diag (OP_SIM_CONTEXT_PTR_OPT);
		}

	void _op_SDNC_MAC_terminate (OP_SIM_CONTEXT_ARG_OPT)
		{
		/* The destructor is the Termination Block */
		delete (SDNC_MAC_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr);
		}


} /* end of 'extern "C"' */




/* Process model interrupt handling procedure */


void
SDNC_MAC_state::SDNC_MAC (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (SDNC_MAC_state::SDNC_MAC ());
	try
		{


		FSM_ENTER ("SDNC_MAC")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (init) enter executives **/
			FSM_STATE_ENTER_FORCED_NOLABEL (0, "init", "SDNC_MAC [init enter execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_MAC [init enter execs]", state0_enter_exec)
				{
				/* Initialization */
				wpan_mac_init ();
				wpan_print_parameters ();
				wpan_gts_scheduling ();
				
				op_intrpt_priority_set (OPC_INTRPT_SELF, BEACON_INTERVAL_CODE,		0);
				op_intrpt_priority_set (OPC_INTRPT_SELF, START_OF_GTS_PERIOD,		2);
				op_intrpt_priority_set (OPC_INTRPT_SELF, END_OF_CAP_PERIOD_CODE,	8);
				op_intrpt_priority_set (OPC_INTRPT_SELF, END_OF_CFP_PERIOD_CODE, 	8);
				op_intrpt_priority_set (OPC_INTRPT_SELF, END_OF_GTS_PERIOD, 		10);
				op_intrpt_priority_set (OPC_INTRPT_SELF, BACKOFF_EXPIRATION_CODE, 	15); // the highest priority
				/*
				if ( my_address==4) 
						PANsink_mac_address=4;
				else  
						PANsink_mac_address = -3; // in case of coordinator and end device
					 
				*/
				}
				FSM_PROFILE_SECTION_OUT (state0_enter_exec)

			/** state (init) exit executives **/
			FSM_STATE_EXIT_FORCED (0, "init", "SDNC_MAC [init exit execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_MAC [init exit execs]", state0_exit_exec)
				{
				if (!IAM_PAN_COORDINATOR)
					printf(" [%s (#%d)] I am waiting for the beacon from my parent node (#%d). \n\n", device_name, my_address, my_parent_address);
				}
				FSM_PROFILE_SECTION_OUT (state0_exit_exec)


			/** state (init) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "init", "beacon", "tr_0", "SDNC_MAC [init -> beacon : default / ]")
				/*---------------------------------------------------------*/



			/** state (beacon) enter executives **/
			FSM_STATE_ENTER_UNFORCED (1, "beacon", state1_enter_exec, "SDNC_MAC [beacon enter execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_MAC [beacon enter execs]", state1_enter_exec)
				{
				/************************************************************************
				In this state a node check if it is a PAN Coordinator:
				- Node is a PAN Coordinator : it generates first beacon frame
				- Node is a End Device/Router : it waits for a beacon frame from its parent 
				*************************************************************************/
				
				if IAM_PAN_COORDINATOR { // node is PAN coordinator
					if (node_attributes.beacon_order < 15) { // beacon-enable mode
						wpan_send_beacon_frame ();		
						op_intrpt_schedule_self (op_sim_time (), DEFAULT_CODE); // -> go to IDLE state
					}
				} else { // node is not PAN coordinator
					// wait for the received beacon
					switch (op_intrpt_type()) {
						case OPC_INTRPT_STRM : 
						{
							wpan_parse_incoming_frame (); // -> go to IDLE state in wpan_extract_beacon_frame()			
							break;
						};/*end of OPC_INTRPT_STRM */
						
						case OPC_INTRPT_SELF:		
						{
							switch (op_intrpt_code()) {
								case START_OF_TRANSMIT_GTS_USE:	/* try to allocate appropriate TRANSMIT GTS slots */ 				
								{
									wpan_gts_start (GTS_DIRECTION_TRANSMIT);						
									break;						
								};	/* end of START_OF_TRANSMIT_GTS_USE */
							
								case START_OF_RECEIVE_GTS_USE:	/* try to allocate appropriate RECEIVE GTS slots */ 				
								{
									wpan_gts_start (GTS_DIRECTION_RECEIVE);						
									break;						
								};	/* end of START_OF_RECEIVE_GTS_USE */	
							
							}
						};
						default :
						{
						};
					}
				}
					
				  
				}
				FSM_PROFILE_SECTION_OUT (state1_enter_exec)

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (3,"SDNC_MAC")


			/** state (beacon) exit executives **/
			FSM_STATE_EXIT_UNFORCED (1, "beacon", "SDNC_MAC [beacon exit execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_MAC [beacon exit execs]", state1_exit_exec)
				{
				//wpan_mac_interrupt_process();
				}
				FSM_PROFILE_SECTION_OUT (state1_exit_exec)


			/** state (beacon) transition processing **/
			FSM_PROFILE_SECTION_IN ("SDNC_MAC [beacon trans conditions]", state1_trans_conds)
			FSM_INIT_COND (DEFAULT_INTRPT)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("beacon")
			FSM_PROFILE_SECTION_OUT (state1_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, queue_status();, "DEFAULT_INTRPT", "queue_status()", "beacon", "idle", "tr_1", "SDNC_MAC [beacon -> idle : DEFAULT_INTRPT / queue_status()]")
				FSM_CASE_TRANSIT (1, 1, state1_enter_exec, ;, "default", "", "beacon", "beacon", "tr_7", "SDNC_MAC [beacon -> beacon : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (idle) enter executives **/
			FSM_STATE_ENTER_UNFORCED (2, "idle", state2_enter_exec, "SDNC_MAC [idle enter execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_MAC [idle enter execs]", state2_enter_exec)
				{
				//printf ("[Node %s] IDLE \n\n", device_name);
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (5,"SDNC_MAC")


			/** state (idle) exit executives **/
			FSM_STATE_EXIT_UNFORCED (2, "idle", "SDNC_MAC [idle exit execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_MAC [idle exit execs]", state2_exit_exec)
				{
				/* Processes all interrupts */
				wpan_mac_interrupt_process();
				}
				FSM_PROFILE_SECTION_OUT (state2_exit_exec)


			/** state (idle) transition processing **/
			FSM_PROFILE_SECTION_IN ("SDNC_MAC [idle trans conditions]", state2_trans_conds)
			FSM_INIT_COND (START_CSMA)
			FSM_TEST_COND (GTS_START)
			FSM_TEST_COND (RESUME_CCA)
			FSM_TEST_COND (RESUME_BACKOFF)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("idle")
			FSM_PROFILE_SECTION_OUT (state2_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 3, state3_enter_exec, ;, "START_CSMA", "", "idle", "init_backoff", "tr_2", "SDNC_MAC [idle -> init_backoff : START_CSMA / ]")
				FSM_CASE_TRANSIT (1, 6, state6_enter_exec, wpan_gts_slot_init();, "GTS_START", "wpan_gts_slot_init()", "idle", "gts_slot", "tr_8", "SDNC_MAC [idle -> gts_slot : GTS_START / wpan_gts_slot_init()]")
				FSM_CASE_TRANSIT (2, 5, state5_enter_exec, ;, "RESUME_CCA", "", "idle", "CCA", "tr_13", "SDNC_MAC [idle -> CCA : RESUME_CCA / ]")
				FSM_CASE_TRANSIT (3, 4, state4_enter_exec, ;, "RESUME_BACKOFF", "", "idle", "backoff_timer", "tr_14", "SDNC_MAC [idle -> backoff_timer : RESUME_BACKOFF / ]")
				FSM_CASE_TRANSIT (4, 2, state2_enter_exec, ;, "default", "", "idle", "idle", "tr_12", "SDNC_MAC [idle -> idle : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (init_backoff) enter executives **/
			FSM_STATE_ENTER_FORCED (3, "init_backoff", state3_enter_exec, "SDNC_MAC [init_backoff enter execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_MAC [init_backoff enter execs]", state3_enter_exec)
				{
				/* This state initializes the csma/ca parameters */
				wpan_backoff_init ();
				
				/*capture the time that the packet enters the backoff */
				backoff_start_time = op_sim_time();
				}
				FSM_PROFILE_SECTION_OUT (state3_enter_exec)

			/** state (init_backoff) exit executives **/
			FSM_STATE_EXIT_FORCED (3, "init_backoff", "SDNC_MAC [init_backoff exit execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_MAC [init_backoff exit execs]", state3_exit_exec)
				{
				/* set the backoff timer to a random value */
				wpan_backoff_delay_set ();
				}
				FSM_PROFILE_SECTION_OUT (state3_exit_exec)


			/** state (init_backoff) transition processing **/
			FSM_TRANSIT_FORCE (4, state4_enter_exec, ;, "default", "", "init_backoff", "backoff_timer", "tr_6", "SDNC_MAC [init_backoff -> backoff_timer : default / ]")
				/*---------------------------------------------------------*/



			/** state (backoff_timer) enter executives **/
			FSM_STATE_ENTER_UNFORCED (4, "backoff_timer", state4_enter_exec, "SDNC_MAC [backoff_timer enter execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_MAC [backoff_timer enter execs]", state4_enter_exec)
				{
				//printf ("[Node %s] backoff_timer \n\n", device_name);
				}
				FSM_PROFILE_SECTION_OUT (state4_enter_exec)

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (9,"SDNC_MAC")


			/** state (backoff_timer) exit executives **/
			FSM_STATE_EXIT_UNFORCED (4, "backoff_timer", "SDNC_MAC [backoff_timer exit execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_MAC [backoff_timer exit execs]", state4_exit_exec)
				{
				/* Call the interrupt processing routing for each interrupt */
				wpan_mac_interrupt_process();
				}
				FSM_PROFILE_SECTION_OUT (state4_exit_exec)


			/** state (backoff_timer) transition processing **/
			FSM_PROFILE_SECTION_IN ("SDNC_MAC [backoff_timer trans conditions]", state4_trans_conds)
			FSM_INIT_COND (DEFER_BACKOFF)
			FSM_TEST_COND (BACKOFF_EXPIRED)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("backoff_timer")
			FSM_PROFILE_SECTION_OUT (state4_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "DEFER_BACKOFF", "", "backoff_timer", "idle", "tr_3", "SDNC_MAC [backoff_timer -> idle : DEFER_BACKOFF / ]")
				FSM_CASE_TRANSIT (1, 5, state5_enter_exec, wpan_cca_defer();, "BACKOFF_EXPIRED", "wpan_cca_defer()", "backoff_timer", "CCA", "tr_5", "SDNC_MAC [backoff_timer -> CCA : BACKOFF_EXPIRED / wpan_cca_defer()]")
				FSM_CASE_TRANSIT (2, 4, state4_enter_exec, ;, "default", "", "backoff_timer", "backoff_timer", "tr_16", "SDNC_MAC [backoff_timer -> backoff_timer : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (CCA) enter executives **/
			FSM_STATE_ENTER_UNFORCED (5, "CCA", state5_enter_exec, "SDNC_MAC [CCA enter execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_MAC [CCA enter execs]", state5_enter_exec)
				{
				//printf ("[Node %s] CCA \n\n", device_name);
				}
				FSM_PROFILE_SECTION_OUT (state5_enter_exec)

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (11,"SDNC_MAC")


			/** state (CCA) exit executives **/
			FSM_STATE_EXIT_UNFORCED (5, "CCA", "SDNC_MAC [CCA exit execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_MAC [CCA exit execs]", state5_exit_exec)
				{
				/* Call the interrupt processing routing for each interrupt */
				wpan_mac_interrupt_process();
				}
				FSM_PROFILE_SECTION_OUT (state5_exit_exec)


			/** state (CCA) transition processing **/
			FSM_PROFILE_SECTION_IN ("SDNC_MAC [CCA trans conditions]", state5_trans_conds)
			FSM_INIT_COND (CCA_END)
			FSM_TEST_COND (CHANNEL_BUSY)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("CCA")
			FSM_PROFILE_SECTION_OUT (state5_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, queue_status();, "CCA_END", "queue_status()", "CCA", "idle", "tr_4", "SDNC_MAC [CCA -> idle : CCA_END / queue_status()]")
				FSM_CASE_TRANSIT (1, 4, state4_enter_exec, wpan_backoff_delay_set();, "CHANNEL_BUSY", "wpan_backoff_delay_set()", "CCA", "backoff_timer", "tr_10", "SDNC_MAC [CCA -> backoff_timer : CHANNEL_BUSY / wpan_backoff_delay_set()]")
				FSM_CASE_TRANSIT (2, 5, state5_enter_exec, ;, "default", "", "CCA", "CCA", "tr_11", "SDNC_MAC [CCA -> CCA : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (gts_slot) enter executives **/
			FSM_STATE_ENTER_UNFORCED (6, "gts_slot", state6_enter_exec, "SDNC_MAC [gts_slot enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (13,"SDNC_MAC")


			/** state (gts_slot) exit executives **/
			FSM_STATE_EXIT_UNFORCED (6, "gts_slot", "SDNC_MAC [gts_slot exit execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_MAC [gts_slot exit execs]", state6_exit_exec)
				{
				/* Call the interrupt processing routing for each interrupt */
				wpan_mac_interrupt_process();
				
				//define SF_in	1
				//define SF_out	0
				
				// define GTS_START ((SF_params[SF_in].CFP_ACTIVE || SF_params[SF_out].CFP_ACTIVE) && (op_intrpt_type () == OPC_INTRPT_SELF && op_intrpt_code () == START_OF_GTS_PERIOD))
				
				// define GTS_STOP ((SF_params[SF_in].CFP_ACTIVE || SF_params[SF_out].CFP_ACTIVE) && (op_intrpt_type () == OPC_INTRPT_SELF && op_intrpt_code () == END_OF_GTS_PERIOD))
				}
				FSM_PROFILE_SECTION_OUT (state6_exit_exec)


			/** state (gts_slot) transition processing **/
			FSM_PROFILE_SECTION_IN ("SDNC_MAC [gts_slot trans conditions]", state6_trans_conds)
			FSM_INIT_COND (GTS_STOP)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("gts_slot")
			FSM_PROFILE_SECTION_OUT (state6_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "GTS_STOP", "", "gts_slot", "idle", "tr_9", "SDNC_MAC [gts_slot -> idle : GTS_STOP / ]")
				FSM_CASE_TRANSIT (1, 6, state6_enter_exec, ;, "default", "", "gts_slot", "gts_slot", "tr_15", "SDNC_MAC [gts_slot -> gts_slot : default / ]")
				}
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (0,"SDNC_MAC")
		}
	catch (...)
		{
		Vos_Error_Print (VOSC_ERROR_ABORT,
			(const char *)VOSC_NIL,
			"Unhandled C++ exception in process model (SDNC_MAC)",
			(const char *)VOSC_NIL, (const char *)VOSC_NIL);
		}
	}




void
SDNC_MAC_state::_op_SDNC_MAC_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}

void
SDNC_MAC_state::operator delete (void* ptr)
	{
	FIN (SDNC_MAC_state::operator delete (ptr));
	Vos_Poolmem_Dealloc (ptr);
	FOUT
	}

SDNC_MAC_state::~SDNC_MAC_state (void)
	{

	FIN (SDNC_MAC_state::~SDNC_MAC_state ())


	/* No Termination Block */


	FOUT
	}


#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

void *
SDNC_MAC_state::operator new (size_t)
#if defined (VOSD_NEW_BAD_ALLOC)
		throw (VOSD_BAD_ALLOC)
#endif
	{
	void * new_ptr;

	FIN_MT (SDNC_MAC_state::operator new ());

	new_ptr = Vos_Alloc_Object (SDNC_MAC_state::obtype);
#if defined (VOSD_NEW_BAD_ALLOC)
	if (new_ptr == VOSC_NIL) throw VOSD_BAD_ALLOC();
#endif
	FRET (new_ptr)
	}

/* State constructor initializes FSM handling */
/* by setting the initial state to the first */
/* block of code to enter. */

SDNC_MAC_state::SDNC_MAC_state (void) :
		_op_current_block (0)
	{
#if defined (OPD_ALLOW_ODB)
		_op_current_state = "SDNC_MAC [init enter execs]";
#endif
	}

VosT_Obtype
_op_SDNC_MAC_init (int * init_block_ptr)
	{
	FIN_MT (_op_SDNC_MAC_init (init_block_ptr))

	SDNC_MAC_state::obtype = Vos_Define_Object_Prstate ("proc state vars (SDNC_MAC)",
		sizeof (SDNC_MAC_state));
	*init_block_ptr = 0;

	FRET (SDNC_MAC_state::obtype)
	}

VosT_Address
_op_SDNC_MAC_alloc (VosT_Obtype, int)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	SDNC_MAC_state * ptr;
	FIN_MT (_op_SDNC_MAC_alloc ())

	/* New instance will have FSM handling initialized */
#if defined (VOSD_NEW_BAD_ALLOC)
	try {
		ptr = new SDNC_MAC_state;
	} catch (const VOSD_BAD_ALLOC &) {
		ptr = VOSC_NIL;
	}
#else
	ptr = new SDNC_MAC_state;
#endif
	FRET ((VosT_Address)ptr)
	}



void
_op_SDNC_MAC_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	SDNC_MAC_state		*prs_ptr;

	FIN_MT (_op_SDNC_MAC_svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (SDNC_MAC_state *)gen_ptr;

	if (strcmp ("node_attributes" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->node_attributes);
		FOUT
		}
	if (strcmp ("csma_params" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->csma_params);
		FOUT
		}
	if (strcmp ("GTS_params" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->GTS_params);
		FOUT
		}
	if (strcmp ("GTS_ptr" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->GTS_ptr);
		FOUT
		}
	if (strcmp ("GTS_list_ptr" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->GTS_list_ptr);
		FOUT
		}
	if (strcmp ("GTS_list_temp" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->GTS_list_temp);
		FOUT
		}
	if (strcmp ("SF_params" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->SF_params);
		FOUT
		}
	if (strcmp ("SF_ptr" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->SF_ptr);
		FOUT
		}
	if (strcmp ("statistic_vector" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->statistic_vector);
		FOUT
		}
	if (strcmp ("statistic_global_vector" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->statistic_global_vector);
		FOUT
		}
	if (strcmp ("ack_sequence_number" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->ack_sequence_number);
		FOUT
		}
	if (strcmp ("backoff_start_time" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->backoff_start_time);
		FOUT
		}
	if (strcmp ("Final_CAP_slot_next" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->Final_CAP_slot_next);
		FOUT
		}
	if (strcmp ("sent_gts_bits" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->sent_gts_bits);
		FOUT
		}
	if (strcmp ("gts_statistic_vector" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->gts_statistic_vector);
		FOUT
		}
	if (strcmp ("gts_statistics" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->gts_statistics);
		FOUT
		}
	if (strcmp ("gts_pkt_delay" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->gts_pkt_delay);
		FOUT
		}
	if (strcmp ("cfp_queue_capacity" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->cfp_queue_capacity);
		FOUT
		}
	if (strcmp ("cap_queue_capacity" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->cap_queue_capacity);
		FOUT
		}
	if (strcmp ("start_GTS_transmission" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->start_GTS_transmission);
		FOUT
		}
	if (strcmp ("waiting_for_first_beacon" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->waiting_for_first_beacon);
		FOUT
		}
	if (strcmp ("superframe_owner" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->superframe_owner);
		FOUT
		}
	if (strcmp ("self_id" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->self_id);
		FOUT
		}
	if (strcmp ("parent_id" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->parent_id);
		FOUT
		}
	if (strcmp ("baterry_id" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->baterry_id);
		FOUT
		}
	if (strcmp ("my_address" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->my_address);
		FOUT
		}
	if (strcmp ("device_mode" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->device_mode);
		FOUT
		}
	if (strcmp ("device_name" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->device_name);
		FOUT
		}
	if (strcmp ("enable_log" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->enable_log);
		FOUT
		}
	if (strcmp ("log_file" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->log_file);
		FOUT
		}
	if (strcmp ("my_parent_address" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->my_parent_address);
		FOUT
		}
	if (strcmp ("packet_to_send_index" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->packet_to_send_index);
		FOUT
		}
	if (strcmp ("collision_occurred" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->collision_occurred);
		FOUT
		}
	if (strcmp ("rx_power_threshold" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->rx_power_threshold);
		FOUT
		}
	if (strcmp ("Flow_Tables_list" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->Flow_Tables_list);
		FOUT
		}
	if (strcmp ("flowTable_sending_needed" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->flowTable_sending_needed);
		FOUT
		}
	if (strcmp ("PANsink_mac_address" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->PANsink_mac_address);
		FOUT
		}
	if (strcmp ("NDP_is_required" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->NDP_is_required);
		FOUT
		}
	if (strcmp ("NDP_beacon_seq_num" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->NDP_beacon_seq_num);
		FOUT
		}
	if (strcmp ("Neighbour_list" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->Neighbour_list);
		FOUT
		}
	if (strcmp ("No_of_neighbours" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->No_of_neighbours);
		FOUT
		}
	if (strcmp ("tdp_id" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->tdp_id);
		FOUT
		}
	if (strcmp ("tdp_decision" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->tdp_decision);
		FOUT
		}
	if (strcmp ("tdp_code" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->tdp_code);
		FOUT
		}
	if (strcmp ("startTimeSoBo_list_ptr" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->startTimeSoBo_list_ptr);
		FOUT
		}
	if (strcmp ("flowEntriesP" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->flowEntriesP);
		FOUT
		}
	if (strcmp ("grandfatherID" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->grandfatherID);
		FOUT
		}
	if (strcmp ("Run1time" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->Run1time);
		FOUT
		}
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

