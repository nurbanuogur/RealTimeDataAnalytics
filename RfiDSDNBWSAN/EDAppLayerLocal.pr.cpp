/* Process model C++ form file: EDAppLayerLocal.pr.cpp */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char EDAppLayerLocal_pr_cpp [] = "MIL_3_Tfile_Hdr_ 145A 30A op_runsim 7 5BCF736F 5BCF736F 1 DESKTOP-L8S8FMO Mohammed@Hussein 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1e80 8                                                                                                                                                                                                                                                                                                                                                                                       ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

/* Include files					*/
/* Distribution Modeling sub-package*/

#include <oms_string_support.h>
#include <oms_dist_support.h>
#include <vector>
#include <iostream>
#include "headers\wpan_math.h"
#include "headers\wpan_params.h"
#include "headers\wpan_struct.h"
#include "headers\CsocketKafkaLocal.h"
//#include "headers\CsocketKafkaUDP.h"


using namespace std;
/* Node configuration constants		*/



/* Special attribute values			*/

/* Interrupt code values			*/
#define	SC_SEND_BE			0
#define	SC_SEND_RT			1
#define APP_REQ_CODE		9
#define START_APP_CODE		13


/* Input and output streams		*/
#define	STRM_FROM_APPL_TO_NWK	0
#define	STRM_FROM_NWK_TO_APPL	3
#define STRM_FROM_RFID_TO_APPL  5

/* State machine conditions 		*/
#define IAM_PAN_COORDINATOR 	(device_mode == PAN_COORDINATOR)
#define IAM_ROUTER 				(device_mode == ROUTER)
#define IAM_END_DEVICE 			(device_mode == END_DEVICE)

#define	SEND_BE			((intrpt_type == OPC_INTRPT_SELF) && (intrpt_code == SC_SEND_BE) && (dest_mac_address != my_address))
#define	SEND_RT			((intrpt_type == OPC_INTRPT_SELF) && (intrpt_code == SC_SEND_RT) && (dest_mac_address != my_address))
#define RECEIVE_PCK		(intrpt_type == OPC_INTRPT_STRM && intrpt_strm == STRM_FROM_NWK_TO_APPL)
#define	START_BE_APP	((intrpt_type == OPC_INTRPT_REMOTE) && (intrpt_code == START_APP_CODE))
#define ReceiveRFID 	(intrpt_type == OPC_INTRPT_STRM && intrpt_strm == STRM_FROM_RFID_TO_APPL)

// list pointers definations
struct RFIDList
		{	
		int RfidID;
		int patientID;
		int age;
		int gender;
		int	height_cm;
		int weight_kg;
		}; 

vector<RFIDList*> NumberofRFID;
	
/* Function prototypes.				*/
static void wpan_appl_init (void); 
static void wpan_print_parameters (void);
static void wpan_be_packet_generate (void);
static void wpan_rt_packet_generate (void);
static void wpan_receive_pck_from_nwk (void);
static int wpan_sequence_number(void); 

/* Global variables */
int app_sent_msdu_bits = 0;
int app_sent_msdu_nbr = 0;


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
class EDAppLayerLocal_state
	{
	private:
		/* Internal state tracking for FSM */
		FSM_SYS_STATE

	public:
		EDAppLayerLocal_state (void);

		/* Destructor contains Termination Block */
		~EDAppLayerLocal_state (void);

		/* State Variables */
		Objid	                  		self_id                                         ;
		Objid	                  		parent_id                                       ;
		int	                    		device_mode                                     ;
		char	                   		device_name[20]                                 ;
		int	                    		my_address                                      ;
		Boolean	                		enable_log                                      ;
		int	                    		intrpt_type                                     ;
		int	                    		intrpt_strm                                     ;
		int	                    		intrpt_code                                     ;
		OmsT_Dist_Handle	       		be_packet_interarrival_dist_ptr                 ;
		OmsT_Dist_Handle	       		be_packet_size_dist_ptr                         ;
		char	                   		be_packet_interarrival_dist_str[128]            ;
		char	                   		be_packet_size_dist_str[128]                    ;
		double	                 		be_start_time                                   ;
		double	                 		be_stop_time                                    ;
		Boolean	                		be_ack                                          ;
		OmsT_Dist_Handle	       		rt_packet_interarrival_dist_ptr                 ;
		OmsT_Dist_Handle	       		rt_packet_size_dist_ptr                         ;
		char	                   		rt_packet_interarrival_dist_str[128]            ;
		char	                   		rt_packet_size_dist_str[128]                    ;
		double	                 		rt_start_time                                   ;
		double	                 		rt_stop_time                                    ;
		Boolean	                		rt_ack                                          ;
		int	                    		dest_mac_address                                ;
		Stathandle	             		bits_rcvd_MSDU_be_stathandle                    ;
		Stathandle	             		bits_rcvd_MSDU_rt_stathandle                    ;
		Stathandle	             		bitssec_rcvd_MSDU_be_stathandle                 ;
		Stathandle	             		bits_rcvd_NSDU_be_stathandle                    ;
		Stathandle	             		bits_rcvd_NSDU_rt_stathandle                    ;
		Stathandle	             		bitssec_rcvd_NSDU_be_stathandle                 ;
		Stathandle	             		pkts_rcvd_be_stathandle                         ;
		Stathandle	             		pkts_rcvd_rt_stathandle                         ;
		Stathandle	             		pktssec_rcvd_be_stathandle                      ;
		Stathandle	             		pktssec_rcvd_rt_stathandle                      ;
		Stathandle	             		bits_sent_MSDU_be_stathandle                    ;
		Stathandle	             		bits_sent_MSDU_rt_stathandle                    ;
		Stathandle	             		bits_sent_NSDU_be_stathandle                    ;
		Stathandle	             		bits_sent_NSDU_rt_stathandle                    ;
		Stathandle	             		pkts_sent_be_stathandle                         ;
		Stathandle	             		pkts_sent_rt_stathandle                         ;
		Stathandle	             		ete_delay_be_stathandle                         ;
		Stathandle	             		ete_delay_rt_stathandle                         ;
		Stathandle	             		bits_rcvd_MSDU_be_gstathandle                   ;
		Stathandle	             		bits_rcvd_MSDU_rt_gstathandle                   ;
		Stathandle	             		bitssec_rcvd_gstathandle                        ;
		Stathandle	             		pkts_rcvd_be_gstathandle                        ;
		Stathandle	             		pkts_rcvd_rt_gstathandle                        ;
		Stathandle	             		pktssec_rcvd_gstathandle                        ;
		Stathandle	             		ete_delay_be_gstathandle                        ;
		Stathandle	             		ete_delay_rt_gstathandle                        ;
		Stathandle	             		bits_sent_MSDU_be_gstathandle                   ;
		Stathandle	             		bits_sent_MSDU_rt_gstathandle                   ;
		Stathandle	             		pkts_sent_be_gstathandle                        ;
		Stathandle	             		pkts_sent_rt_gstathandle                        ;
		int	                    		rt_generated_frames                             ;
		int	                    		be_generated_frames                             ;
		Stathandle	             		bitsec_sent_NSDU_be_stathandle                  ;
		Stathandle	             		pktsec_sent_be_stathandle                       ;
		int	                    		DATArecieved                                    ;
		int	                    		sendDATA                                        ;
		FILE*	                  		Applog_file                                     ;
		Boolean	                		RFID_log                                        ;
		vector<RFIDList*>	      		ListOfRFID                                      ;
		int	                    		Lsize                                           ;
		int	                    		RFIDno                                          ;

		/* FSM code */
		void EDAppLayerLocal (OP_SIM_CONTEXT_ARG_OPT);
		/* Diagnostic Block */
		void _op_EDAppLayerLocal_diag (OP_SIM_CONTEXT_ARG_OPT);

#if defined (VOSD_NEW_BAD_ALLOC)
		void * operator new (size_t) throw (VOSD_BAD_ALLOC);
#else
		void * operator new (size_t);
#endif
		void operator delete (void *);

		/* Memory management */
		static VosT_Obtype obtype;
	};

VosT_Obtype EDAppLayerLocal_state::obtype = (VosT_Obtype)OPC_NIL;

#define pr_state_ptr            		((EDAppLayerLocal_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))
#define self_id                 		pr_state_ptr->self_id
#define parent_id               		pr_state_ptr->parent_id
#define device_mode             		pr_state_ptr->device_mode
#define device_name             		pr_state_ptr->device_name
#define my_address              		pr_state_ptr->my_address
#define enable_log              		pr_state_ptr->enable_log
#define intrpt_type             		pr_state_ptr->intrpt_type
#define intrpt_strm             		pr_state_ptr->intrpt_strm
#define intrpt_code             		pr_state_ptr->intrpt_code
#define be_packet_interarrival_dist_ptr		pr_state_ptr->be_packet_interarrival_dist_ptr
#define be_packet_size_dist_ptr 		pr_state_ptr->be_packet_size_dist_ptr
#define be_packet_interarrival_dist_str		pr_state_ptr->be_packet_interarrival_dist_str
#define be_packet_size_dist_str 		pr_state_ptr->be_packet_size_dist_str
#define be_start_time           		pr_state_ptr->be_start_time
#define be_stop_time            		pr_state_ptr->be_stop_time
#define be_ack                  		pr_state_ptr->be_ack
#define rt_packet_interarrival_dist_ptr		pr_state_ptr->rt_packet_interarrival_dist_ptr
#define rt_packet_size_dist_ptr 		pr_state_ptr->rt_packet_size_dist_ptr
#define rt_packet_interarrival_dist_str		pr_state_ptr->rt_packet_interarrival_dist_str
#define rt_packet_size_dist_str 		pr_state_ptr->rt_packet_size_dist_str
#define rt_start_time           		pr_state_ptr->rt_start_time
#define rt_stop_time            		pr_state_ptr->rt_stop_time
#define rt_ack                  		pr_state_ptr->rt_ack
#define dest_mac_address        		pr_state_ptr->dest_mac_address
#define bits_rcvd_MSDU_be_stathandle		pr_state_ptr->bits_rcvd_MSDU_be_stathandle
#define bits_rcvd_MSDU_rt_stathandle		pr_state_ptr->bits_rcvd_MSDU_rt_stathandle
#define bitssec_rcvd_MSDU_be_stathandle		pr_state_ptr->bitssec_rcvd_MSDU_be_stathandle
#define bits_rcvd_NSDU_be_stathandle		pr_state_ptr->bits_rcvd_NSDU_be_stathandle
#define bits_rcvd_NSDU_rt_stathandle		pr_state_ptr->bits_rcvd_NSDU_rt_stathandle
#define bitssec_rcvd_NSDU_be_stathandle		pr_state_ptr->bitssec_rcvd_NSDU_be_stathandle
#define pkts_rcvd_be_stathandle 		pr_state_ptr->pkts_rcvd_be_stathandle
#define pkts_rcvd_rt_stathandle 		pr_state_ptr->pkts_rcvd_rt_stathandle
#define pktssec_rcvd_be_stathandle		pr_state_ptr->pktssec_rcvd_be_stathandle
#define pktssec_rcvd_rt_stathandle		pr_state_ptr->pktssec_rcvd_rt_stathandle
#define bits_sent_MSDU_be_stathandle		pr_state_ptr->bits_sent_MSDU_be_stathandle
#define bits_sent_MSDU_rt_stathandle		pr_state_ptr->bits_sent_MSDU_rt_stathandle
#define bits_sent_NSDU_be_stathandle		pr_state_ptr->bits_sent_NSDU_be_stathandle
#define bits_sent_NSDU_rt_stathandle		pr_state_ptr->bits_sent_NSDU_rt_stathandle
#define pkts_sent_be_stathandle 		pr_state_ptr->pkts_sent_be_stathandle
#define pkts_sent_rt_stathandle 		pr_state_ptr->pkts_sent_rt_stathandle
#define ete_delay_be_stathandle 		pr_state_ptr->ete_delay_be_stathandle
#define ete_delay_rt_stathandle 		pr_state_ptr->ete_delay_rt_stathandle
#define bits_rcvd_MSDU_be_gstathandle		pr_state_ptr->bits_rcvd_MSDU_be_gstathandle
#define bits_rcvd_MSDU_rt_gstathandle		pr_state_ptr->bits_rcvd_MSDU_rt_gstathandle
#define bitssec_rcvd_gstathandle		pr_state_ptr->bitssec_rcvd_gstathandle
#define pkts_rcvd_be_gstathandle		pr_state_ptr->pkts_rcvd_be_gstathandle
#define pkts_rcvd_rt_gstathandle		pr_state_ptr->pkts_rcvd_rt_gstathandle
#define pktssec_rcvd_gstathandle		pr_state_ptr->pktssec_rcvd_gstathandle
#define ete_delay_be_gstathandle		pr_state_ptr->ete_delay_be_gstathandle
#define ete_delay_rt_gstathandle		pr_state_ptr->ete_delay_rt_gstathandle
#define bits_sent_MSDU_be_gstathandle		pr_state_ptr->bits_sent_MSDU_be_gstathandle
#define bits_sent_MSDU_rt_gstathandle		pr_state_ptr->bits_sent_MSDU_rt_gstathandle
#define pkts_sent_be_gstathandle		pr_state_ptr->pkts_sent_be_gstathandle
#define pkts_sent_rt_gstathandle		pr_state_ptr->pkts_sent_rt_gstathandle
#define rt_generated_frames     		pr_state_ptr->rt_generated_frames
#define be_generated_frames     		pr_state_ptr->be_generated_frames
#define bitsec_sent_NSDU_be_stathandle		pr_state_ptr->bitsec_sent_NSDU_be_stathandle
#define pktsec_sent_be_stathandle		pr_state_ptr->pktsec_sent_be_stathandle
#define DATArecieved            		pr_state_ptr->DATArecieved
#define sendDATA                		pr_state_ptr->sendDATA
#define Applog_file             		pr_state_ptr->Applog_file
#define RFID_log                		pr_state_ptr->RFID_log
#define ListOfRFID              		pr_state_ptr->ListOfRFID
#define Lsize                   		pr_state_ptr->Lsize
#define RFIDno                  		pr_state_ptr->RFIDno

/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	EDAppLayerLocal_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((EDAppLayerLocal_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


/* Function Block */

#if !defined (VOSD_NO_FIN)
enum { _op_block_origin = __LINE__ + 2};
#endif

/*--------------------------------------------------------------------------------
 * Function:	Application_log_file_init
 *
 * Description:	- initialize the folder path and Log File Directory
 *				- read the values of attributes
 *              
 *
 * No parameters
 *--------------------------------------------------------------------------------*/

static void  Application_log_file_init() {
Objid selfID;
Objid fatherID;
char folder_path[120];
char log_name[132];
char name [20];

FIN (Application_log_file_init());
selfID = op_id_self();
fatherID = op_topo_parent(selfID);
op_ima_obj_attr_get (fatherID, "Log File Directory", folder_path);
op_ima_obj_attr_get (fatherID, "name", &name);

if (prg_path_name_is_dir (folder_path) == PrgC_Path_Name_Is_Not_Dir) {
		char msg[128];		
		sprintf (msg, " \"%s\" is not valid directory name. The output will not be logged.\n", folder_path); 
		/* Display an appropriate warning.	*/
		op_prg_odb_print_major ("Warning from Tag process: ", msg, OPC_NIL);
	}	
sprintf (log_name, "%s%s%s.ak", folder_path, name,"_Application_RFID"); 
printf ("Log file name: %s \n\n", log_name);
Applog_file = fopen(log_name,"w");
fprintf (Applog_file, "|-----------------------------------------------------------------------------|\n");
fprintf (Applog_file, "| NODE NAME          : %Reader RFID info \n", name);
FOUT;
}
/*--------------------------------------------------------------------------------
 * Function:	wpan_appl_init
 *
 * Description:	- initialize the process
 *				- read the values of attributes
 *              - schedule a self interrupts 
 *
 * No parameters
 *--------------------------------------------------------------------------------*/

static void wpan_appl_init() {

	Objid traffic_id;
	Objid traffic_comp_id;	
	
	/* Stack tracing enrty point */
	FIN(wpan_appl_init);
 // some counter used in the program
	DATArecieved=0;
	sendDATA=0;
	Lsize=0;
	RFIDno=0;
	
	/* obtain self object ID of the surrounding processor or queue */
	self_id = op_id_self ();

	/* obtain object ID of the parent object (node) */
	parent_id = op_topo_parent (self_id);
	
	/* get the value to check if this node is PAN coordinator, Router, or End node */
	op_ima_obj_attr_get_int32 (parent_id, "Device Mode", &device_mode);
	
	/* get the name of the node */
	op_ima_obj_attr_get (parent_id, "name", device_name);
	
	op_ima_obj_attr_get (parent_id, "Enable Logging", &enable_log);
	
	/* get the MAC address of the node */
	op_ima_obj_attr_get (parent_id, "MAC Address", &my_address);
	
	/* get the RFID activation of the node */
	op_ima_obj_attr_get (parent_id, "RFID Active", &RFID_log);		
	if (RFID_log) 	Application_log_file_init();
	
	/* Read the parameters of best-effort traffic */	
	op_ima_obj_attr_get (self_id, "Best-effort", &traffic_id);
	traffic_comp_id = op_topo_child (traffic_id, OPC_OBJTYPE_GENERIC, 0);
	
	op_ima_obj_attr_get_str (traffic_comp_id, "Packet Interarrival Time",128,	be_packet_interarrival_dist_str);
	op_ima_obj_attr_get_str (traffic_comp_id, "Packet Size", 128,             	be_packet_size_dist_str);
	op_ima_obj_attr_get (traffic_comp_id, "Start Time",							&be_start_time);
	op_ima_obj_attr_get (traffic_comp_id, "Stop Time",             				&be_stop_time);	
	op_ima_obj_attr_get (traffic_comp_id, "Acknowledgment",            			&be_ack);	
	
	/* Read the parameters of real-time traffic */	
	op_ima_obj_attr_get (self_id, "Real-time", &traffic_id);
	traffic_comp_id = op_topo_child (traffic_id, OPC_OBJTYPE_GENERIC, 0);
	
	op_ima_obj_attr_get_str (traffic_comp_id, "Packet Interarrival Time",128,	rt_packet_interarrival_dist_str);
	op_ima_obj_attr_get_str (traffic_comp_id, "Packet Size", 128,             	rt_packet_size_dist_str);
	op_ima_obj_attr_get (traffic_comp_id, "Start Time",							&rt_start_time);
	op_ima_obj_attr_get (traffic_comp_id, "Stop Time",             				&rt_stop_time);	
	op_ima_obj_attr_get (traffic_comp_id, "Acknowledgment",            			&rt_ack);
	
	/* get destination MAC address */
	op_ima_obj_attr_get (parent_id, "Final Dest", &dest_mac_address);	
	
	
	if (dest_mac_address == my_address) {
		be_start_time = START_DISABLED;
		rt_start_time = START_DISABLED;
	}
	
	/* if the destination MAC address is broadcast do not send acknowledged packets */
	if ((dest_mac_address == BROADCAST_ADDRESS) && (be_start_time != START_DISABLED) && (be_ack == OPC_TRUE)) {
		be_start_time = START_DISABLED;
		if (enable_log) {
			printf (" [%s (#%d)] The best-effort acknowledged data cannot be generated, since the Destination MAC Adddress is set to BROADCAST. \n", device_name, my_address);
		}
	}
	
	/* if the destination MAC address is broadcast do not send acknowledged packets */
	if ((dest_mac_address == BROADCAST_ADDRESS) && (rt_start_time != START_DISABLED) && (rt_ack == OPC_TRUE)) {
		rt_start_time = START_DISABLED;
		if (enable_log) {
			printf (" [%s (#%d)] The real-time acknowledged data cannot be generated, since the Destination MAC Adddress is set to BROADCAST. \n", device_name, my_address);
		}
	}
	
	rt_generated_frames = 0;
	be_generated_frames = 0;
	
	/* Load the PDFs that will be used in computing the Packet Interarrival Times and Packet Sizes. */
	be_packet_interarrival_dist_ptr = oms_dist_load_from_string (be_packet_interarrival_dist_str);
	be_packet_size_dist_ptr       = oms_dist_load_from_string (be_packet_size_dist_str);
	
	/* Load the PDFs that will be used in computing the Packet Interarrival Times and Packet Sizes. */
	rt_packet_interarrival_dist_ptr = oms_dist_load_from_string (rt_packet_interarrival_dist_str);
	rt_packet_size_dist_ptr       = oms_dist_load_from_string (rt_packet_size_dist_str);
	
	
	/* Make sure we have valid start and stop times, i.e. start time is before stop time.	*/
	if ((be_stop_time <= be_start_time) && (be_stop_time != END_OF_SIMULATION)) {
		/* stop time is before start time. Disable the source.	*/
		be_start_time = START_DISABLED;

		/* Display an appropriate warning.								*/
		op_prg_odb_print_major ("Warning from a Application model:", 
								"Although the best-effort generator is not disabled (start time is set to a finite value), stop time is before start time.",
								"Disabling the best-effort generator.", OPC_NIL);
	}
	
		/* Make sure we have valid start and stop times, i.e. start time is before stop time.	*/
	if ((rt_stop_time <= rt_start_time) && (rt_stop_time != END_OF_SIMULATION)) {
		/* stop time is before start time. Disable the source.	*/
		rt_start_time = START_DISABLED;

		/* Display an appropriate warning.								*/
		op_prg_odb_print_major ("Warning from a Application model:", 
								"Although the real-time generator is not disabled (start time is set to a finite value), stop time is before start time.",
								"Disabling the real-time generator.", OPC_NIL);
	}
	
	/* Schedule a self interrupt that will indicate transition to next state.	*/
	if (be_start_time != START_DISABLED)
		op_intrpt_schedule_remote (be_start_time, APP_REQ_CODE, op_id_from_name (parent_id, OPC_OBJTYPE_PROC, "PKSW"));
//		op_intrpt_schedule_self (be_start_time, SC_SEND_BE);	
		
	if (rt_start_time != START_DISABLED)
		op_intrpt_schedule_self (rt_start_time, SC_SEND_RT);
	
	
	/* register the LOCAL statistics */
	bits_rcvd_MSDU_be_stathandle 	= op_stat_reg ("Application.Traffic Received - best-effort MSDU (bit)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	bitssec_rcvd_MSDU_be_stathandle = op_stat_reg ("Application.Traffic Received - best-effort MSDU (bit/sec)",	OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	bits_rcvd_NSDU_be_stathandle 	= op_stat_reg ("Application.Traffic Received - best-effort NSDU (bit)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	bitssec_rcvd_NSDU_be_stathandle = op_stat_reg ("Application.Traffic Received - best-effort NSDU (bit/sec)",	OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);	
	pkts_rcvd_be_stathandle 		= op_stat_reg ("Application.Traffic Received - best-effort (packet)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	pktssec_rcvd_be_stathandle 		= op_stat_reg ("Application.Traffic Received - best-effort (packet/sec)",	OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	
	bits_rcvd_MSDU_rt_stathandle 	= op_stat_reg ("Application.Traffic Received - real-time MSDU (bit)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	bits_rcvd_NSDU_rt_stathandle 	= op_stat_reg ("Application.Traffic Received - real-time NSDU (bit)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	pkts_rcvd_rt_stathandle 		= op_stat_reg ("Application.Traffic Received - real-time (packet)",			OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	pktssec_rcvd_rt_stathandle 		= op_stat_reg ("Application.Traffic Received - real-time (packet/sec)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	
	ete_delay_be_stathandle			= op_stat_reg ("Application.End-to-End Delay - best-effort (sec)",	OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	ete_delay_rt_stathandle			= op_stat_reg ("Application.End-to-End Delay - real-time (sec)",	OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	
	bits_sent_MSDU_be_stathandle	= op_stat_reg ("Application.Traffic Sent - best-effort MSDU (bit)",	OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	bits_sent_MSDU_rt_stathandle	= op_stat_reg ("Application.Traffic Sent - real-time MSDU (bit)",	OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	bits_sent_NSDU_be_stathandle	= op_stat_reg ("Application.Traffic Sent - best-effort NSDU (bit)",	OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	bitsec_sent_NSDU_be_stathandle	= op_stat_reg ("Application.Traffic Sent - best-effort NSDU (bit/sec)",	OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	bits_sent_NSDU_rt_stathandle	= op_stat_reg ("Application.Traffic Sent - real-time NSDU (bit)",	OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);	
	pkts_sent_be_stathandle			= op_stat_reg ("Application.Traffic Sent - best-effort (packet)",	OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	pktsec_sent_be_stathandle		= op_stat_reg ("Application.Traffic Sent - best-effort (packet/sec)",	OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	pkts_sent_rt_stathandle			= op_stat_reg ("Application.Traffic Sent - real-time (packet)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	
	/* register the GLOBAL statistics */
	bits_rcvd_MSDU_be_gstathandle 	= op_stat_reg ("Application.Traffic Received - best-effort MSDU (bit)",	OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
	pkts_rcvd_be_gstathandle 		= op_stat_reg ("Application.Traffic Received - best-effort (packet)",	OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
	
	bits_rcvd_MSDU_rt_gstathandle 	= op_stat_reg ("Application.Traffic Received - real-time MSDU (bit)",	OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
	pkts_rcvd_rt_gstathandle 		= op_stat_reg ("Application.Traffic Received - real-time (packet)",		OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
	
	bitssec_rcvd_gstathandle 		= op_stat_reg ("Application.Traffic Received (bit/sec)",			OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
	pktssec_rcvd_gstathandle 		= op_stat_reg ("Application.Traffic Received (packet/sec)",			OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
	
	ete_delay_be_gstathandle		= op_stat_reg ("Application.End-to-End Delay - best-effort (sec)",	OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
	ete_delay_rt_gstathandle		= op_stat_reg ("Application.End-to-End Delay - real-time (sec)",	OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
	
	bits_sent_MSDU_be_gstathandle	= op_stat_reg ("Application.Traffic Sent - best-effort MSDU (bit)",	OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
	bits_sent_MSDU_rt_gstathandle	= op_stat_reg ("Application.Traffic Sent - real-time MSDU (bit)",	OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
	pkts_sent_be_gstathandle		= op_stat_reg ("Application.Traffic Sent - best-effort (packet)",	OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
	pkts_sent_rt_gstathandle		= op_stat_reg ("Application.Traffic Sent - real-time (packet)",		OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
	
	if ((be_start_time != START_DISABLED) || (rt_start_time != START_DISABLED))
		wpan_print_parameters();
	
	/* Stack tracing exit point */
	FOUT;
}



/*--------------------------------------------------------------------------------
 * Function:	wpan_be_packet_generate
 *
 * Description:	Creates a best-effort packet (MSDU) based on the packet generation		
 *				 specifications and sends it to the network layer.
 *
 * No parameters
 *--------------------------------------------------------------------------------*/

static void wpan_be_packet_generate() {
	
	Ici* iciptr;
	Packet* packet_NSDU_ptr;
	int	nsdu_size; /* size in bits */
//	int msdu_size;
//	int header_size;
 	//int sequence_number;
	double next_intarr_time;	/*  interarrival time of next packet */
	double abs_next_intarr_time; /* absolute interarrival time of next packet */

	/* Stack tracing enrty point */
	FIN (wpan_be_packet_generate);
	
	iciptr = op_ici_create ("wpan_packet_ici_format");
	op_ici_attr_set (iciptr, "ACK", be_ack);
	op_ici_attr_set (iciptr, "Traffic Type", BEST_EFFORT_TRAFFIC);
	
	/* create a MAC payload (MSDU) */
//	packet_MSDU_ptr = op_pk_create_fmt ("wpan_frame_NPDU_format");
	
//	sequence_number = wpan_sequence_number();	
	/*
	packet generate
	Complex modeling scenarios require stochastic values that conform to 
	a variety of probability distributions, such as the “exponential”, 
	“normal”, and “pareto” dis-tributions. Probability distribution 
	arguments are specified as a mean (when only a mean is required), 
	or as a pair of arguments when two parameters are required 
	(for example, mean and variance for a “normal” distribution).
	*/

	
	/* Size of the NWk header */ 
//	header_size = (int) op_pk_total_size_get(packet_MSDU_ptr);
		/////////////////////////////////////////////////////////////////////////////
	//char Data[100];i++;

	// int= 32 bit= 4 byte for each one and they are 8
	// packet size = 8*32=256 bit which is less than 280 bit
	// so much print for this sentence
	//fprintf (Applog_file, "ListOfRFID.size()=%d\n", Lsize);
	
	int qrs = rand() % 150; /*QRS random int between 0 and 200 */ 	
	int pr = rand() % 150;	/*PR random int between 0 and 350 */
	int qt = rand() % 550;	/*QT random int between 0 and 550 */
	/* Here we are sending the info of packets to Kafka consumer */
	Packet*	RfidPkptr= op_pk_create_fmt("rfid_hbi_data_packet");
    op_pk_nfd_set (RfidPkptr, "RFID ID",ListOfRFID[Lsize]->RfidID);
    	
	op_pk_nfd_set (RfidPkptr, "PatientID",ListOfRFID[Lsize]->patientID);
    op_pk_nfd_set (RfidPkptr, "Age",ListOfRFID[Lsize]->age);
	op_pk_nfd_set (RfidPkptr, "Gender",ListOfRFID[Lsize]->gender);
	op_pk_nfd_set (RfidPkptr, "Height_cm",ListOfRFID[Lsize]->height_cm);
	op_pk_nfd_set (RfidPkptr, "Weight_kg",ListOfRFID[Lsize]->weight_kg);
	op_pk_nfd_set (RfidPkptr, "QRS",qrs);
	op_pk_nfd_set (RfidPkptr, "PR",pr );
	op_pk_nfd_set (RfidPkptr, "QT",qt);
	
	 
	if (Lsize<ListOfRFID.size()-1) Lsize++;
	
	
	sendDATA++;
	
		/* Generate a NSDU size outcome	*/
	nsdu_size = (int) ceil (oms_dist_outcome (be_packet_size_dist_ptr));
	/* 0 <= nsdu_size <= (aMaxMACSafePayloadSize_Octet*8-header_size) */
//	if (nsdu_size > (aMaxMACSafePayloadSize_Octet*8-header_size))
//		nsdu_size = (aMaxMACSafePayloadSize_Octet*8-header_size);	
	/* The size of generated NSDU is bigger than the maximum - the size is set to the maximum. */
	if (nsdu_size > (aMaxMACSafePayloadSize_Octet*8))
		nsdu_size = (aMaxMACSafePayloadSize_Octet*8);	
	/* The size of generated NSDU is bigger than the maximum - the size is set to the maximum. */
//		aMaxMACSafePayloadSize_Octet=991
	// aMaxMACSafePayloadSize_Octet*8=7928
	if (nsdu_size < 0)
		nsdu_size = 0;
	cout<<"This is the first pck from Source and Number ="<<sendDATA<<endl;
	/* unformatted payload	*/
	packet_NSDU_ptr = op_pk_create (nsdu_size);
	
	//op_pk_nfd_set_pkt (packet_NSDU_ptr, "NSDU",RfidPkptr );
	
	/* send the MSDU via the stream to the network layer.	*/
	/* send ICI to network layer together with packet. */
	op_ici_install (iciptr);
	//op_pk_send (packet_NSDU_ptr, STRM_FROM_APPL_TO_NWK);
	op_pk_send (RfidPkptr, STRM_FROM_APPL_TO_NWK);
	op_ici_install (OPC_NIL);
	// 
	/* schedule next packet generation */
	next_intarr_time = oms_dist_outcome (be_packet_interarrival_dist_ptr);

	/* Make sure that interarrival time is not negative. In that case it will be set to 0. */
	if (next_intarr_time <0)
		next_intarr_time = 0.0;

	abs_next_intarr_time = op_sim_time () + next_intarr_time;
	//fprintf (Applog_file, "hi hbi abs_next_intarr_time=%f\n",abs_next_intarr_time);

	if (enable_log) {
		printf (" [%s (#%d)] t=%f -> (APL) Best-effort packet's payload (MSDU size =  bits, SEQ = ) was generated and sent to Network layer \n", device_name, my_address, op_sim_time()/*, (int) op_pk_total_size_get(packet_MSDU_ptr), sequence_number*/);
	}
	
	if ((abs_next_intarr_time <= be_stop_time) || (be_stop_time == END_OF_SIMULATION)) {
		op_intrpt_schedule_self (abs_next_intarr_time, SC_SEND_BE);
		
		if (enable_log)
			printf ("\t (APL) Next best-effort packet's payload (MSDU) will be generated at %f sec.\n\n", abs_next_intarr_time);
	} else {
		if (enable_log)
			printf ("\t (APL) STOP best-effort packet's payload (MSDU) generation.\n\n", abs_next_intarr_time);
	}
	
//	msdu_size = (int) op_pk_total_size_get (packet_NSDU_ptr);	
	
	/* Update the packet generation statistics.			*/
	op_stat_write (bits_sent_MSDU_be_stathandle, nsdu_size);
	op_stat_write (bits_sent_NSDU_be_stathandle, nsdu_size);
	op_stat_write (bitsec_sent_NSDU_be_stathandle, nsdu_size);
	op_stat_write (pkts_sent_be_stathandle, 1.0);
	op_stat_write (pktsec_sent_be_stathandle, 1.0);
	op_stat_write (bits_sent_MSDU_be_gstathandle, nsdu_size);
	op_stat_write (pkts_sent_be_gstathandle, 1.0);
	
	be_generated_frames++;
	 
	//printf("SRC gen=%d",be_generated_frames);	
	//printf("SRC SendData=%d",sendDATA);
	
	//app_sent_msdu_bits = app_sent_msdu_bits + (msdu_size/1000.0);	
	//app_sent_msdu_nbr = app_sent_msdu_nbr + 1;

		
	/* Stack tracing exit point */
	FOUT;
}


/*--------------------------------------------------------------------------------
 * Function:	wpan_receive_pck_from_nwk
 *
 * Description:	Receive a packet's payload (MSDU) from NWK layer and update statistics.
 *
 * No parameters
 *--------------------------------------------------------------------------------*/

static void wpan_receive_pck_from_nwk() {
	
	Ici* iciptr;
	Packet* packet_MSDU_ptr;
//	Packet* packet_NSDU_ptr; /* payload */
	double packet_MSDU_size;
//	double packet_NSDU_size; 
	double e2e_delay; 
 	int TagID ;
	int traffic_type;
	//char Data[100];
	char sendThisMsgToKafka[500];// here it was the problem
	 
	int patientID , age, gender,height_cm, weight_kg;
	int qrs,pr,qt;
	
	/* Stack tracing enrty point */
	FIN(wpan_receive_packet);	
	
	/* get the ICI information */
	iciptr = op_intrpt_ici ();
	//cout<<"Packets arrived ************************************** "  <<endl;
	op_ici_attr_get (iciptr, "Traffic Type", &traffic_type);
	
	/* Get packet from the input stream  */
	packet_MSDU_ptr = op_pk_get (intrpt_strm);
	
	/* Caclulate metrics to be updated	*/
	packet_MSDU_size = (double) op_pk_total_size_get (packet_MSDU_ptr);
	e2e_delay = op_sim_time () - op_pk_creation_time_get (packet_MSDU_ptr);
	
	//printf("DEstination DATArecieved=%d",DATArecieved);
	if (enable_log) {
		printf (" [%s (#%d)] t=%f -> (APL) Received packet's payload (MSDU) from Network layer. I am the destination. \n", device_name, my_address, op_sim_time());
		printf ("\t\t End-to-end delay    : %f sec \n", e2e_delay);
		if (traffic_type == REAL_TIME_TRAFFIC) {
			printf ("\t\t Traffic type    : REAL_TIME_TRAFFIC \n\n");
		} else {
			printf ("\t\t Traffic type    : BEST_EFFORT_TRAFFIC \n\n");
		}		
	} 
	
	/* update the statistics */	
	if (traffic_type == REAL_TIME_TRAFFIC) {
		op_stat_write (ete_delay_rt_stathandle, 		e2e_delay);
		op_stat_write (bits_rcvd_MSDU_rt_stathandle, 	packet_MSDU_size);
		op_stat_write (bits_rcvd_NSDU_rt_stathandle, 	packet_MSDU_size);
		op_stat_write (pkts_rcvd_rt_stathandle, 		1.0);
		op_stat_write (pktssec_rcvd_rt_stathandle, 		1.0);
		op_stat_write (pktssec_rcvd_rt_stathandle, 		0.0);
		op_stat_write (ete_delay_rt_gstathandle, 		e2e_delay);
		op_stat_write (bits_rcvd_MSDU_rt_gstathandle, 	packet_MSDU_size);
		op_stat_write (pkts_rcvd_rt_gstathandle, 		1.0);
		
	
	} else { /* BEST_EFFORT_TRAFFIC */
		op_stat_write (ete_delay_be_stathandle, 		e2e_delay);
		op_stat_write (bits_rcvd_MSDU_be_stathandle, 	packet_MSDU_size);
		op_stat_write (bits_rcvd_NSDU_be_stathandle, 	packet_MSDU_size);
		op_stat_write (bitssec_rcvd_MSDU_be_stathandle, packet_MSDU_size);
		op_stat_write (bitssec_rcvd_MSDU_be_stathandle, 0.0);
		op_stat_write (bitssec_rcvd_NSDU_be_stathandle, packet_MSDU_size);
		op_stat_write (bitssec_rcvd_NSDU_be_stathandle, 0.0);
		op_stat_write (pkts_rcvd_be_stathandle, 		1.0);
		op_stat_write (pktssec_rcvd_be_stathandle, 		1.0);
		op_stat_write (pktssec_rcvd_be_stathandle, 		0.0);
		op_stat_write (ete_delay_be_gstathandle, 		e2e_delay);
		op_stat_write (bits_rcvd_MSDU_be_gstathandle, 	packet_MSDU_size);
		op_stat_write (pkts_rcvd_be_gstathandle, 		1.0);
	//here we are sending the packet size and the delay values to the kafka
		/*
		sprintf (sendThisMsgToKafka, "packet_MSDU_size:%f :%f :%lf ",
						op_sim_time(),packet_MSDU_size,e2e_delay  );		
		CsocketKafka(sendThisMsgToKafka);
		*/
		}
// write the statistics	
	op_stat_write (bitssec_rcvd_gstathandle, 	packet_MSDU_size);
	op_stat_write (bitssec_rcvd_gstathandle, 	0.0);
	op_stat_write (pktssec_rcvd_gstathandle, 	1.0);
	op_stat_write (pktssec_rcvd_gstathandle, 	0.0);
//////////////////////////////////////////////////////////////////	
	
	//cout<<"the _______________________________________________ "  <<endl;
	/* Here we are sending the info of packets to Kafka consumer */
	
 
 	op_pk_nfd_get(packet_MSDU_ptr, "RFID ID", &TagID);
 	op_pk_nfd_get(packet_MSDU_ptr, "PatientID", &patientID);
	op_pk_nfd_get(packet_MSDU_ptr, "Age", &age);
	op_pk_nfd_get(packet_MSDU_ptr, "Gender", &gender);
	op_pk_nfd_get(packet_MSDU_ptr, "Height_cm", &height_cm);
	op_pk_nfd_get(packet_MSDU_ptr, "Weight_kg", &weight_kg);
/*
	cout<<"patientID "<<patientID<<endl;
	cout<<"age "<<age<<endl;
	cout<<"gender "<<gender<<endl;
	cout<<"height_cm "<<height_cm<<endl;
	cout<<"weight_kg "<<weight_kg<<endl;
	*/
	// I have to make it for every One or put it in the list
	
	op_pk_nfd_get(packet_MSDU_ptr, "QRS", &qrs);
	op_pk_nfd_get(packet_MSDU_ptr, "PR", &pr);
	op_pk_nfd_get(packet_MSDU_ptr, "QT", &qt);
	//	cout<<"the QRS= "<<qrs <<endl;
	DATArecieved++;
	
	/* Here we are sending the info of packets to Kafka consumer */
	//printf ( "Time=%f , temperature(%d), humidity(%d) [%s (#%d)] sends this message (Number:%d) to Kafka ",op_sim_time(),temperature,humidity, device_name, my_address, DATArecieved  );		
// sprintf (sendThisMsgToKafka, "Time=%f , Temperature(%d), Humidity(%d) Co2=%d TagID=%d productID=%d RfidValue1=%d  RfidValue2=%d RfidValue3=%d  MessageNumber:(%d) to Kafka ",op_sim_time(),temperature,humidity,co2,TagID,productID,RfidValue1,RfidValue2,RfidValue3,  DATArecieved  );		
/* Here we are sending the info of packets to Kafka consumer */
	// the vules of temperature,humidity,co2
	
	//sprintf (sendThisMsgToKafka, "Temperature:%d:%d:%d:%d",qrs,pr,qt, DATArecieved);		
	//CsocketKafka(sendThisMsgToKafka);
	///////////////////////////////////////////////////////RFID
	//  the vules of RFID values
	//cout<<"ListOfRFID.size()-1 ="<<ListOfRFID.size()-1<<endl;
	//RFIDno = rand() % 4; /* random int between 0 and 30 */
	//if (RFIDno<ListOfRFID.size()-1) RFIDno++;
	
	
	RFIDList* Rfidlist = new RFIDList;

	Rfidlist->RfidID=TagID;
	Rfidlist->patientID = patientID;
	Rfidlist->age=age;
	Rfidlist->gender=gender;
	Rfidlist->height_cm=height_cm;
	Rfidlist->weight_kg=weight_kg;

// here we are update the vlaue of the Rfid if it has recieved before 
// otherwised it will add it to the list
	for (int c = 0; c < NumberofRFID.size(); c++)
	if (NumberofRFID[c]->RfidID == Rfidlist->RfidID){ 
			NumberofRFID.erase(NumberofRFID.begin()+c);
			}
	NumberofRFID.push_back(Rfidlist);// add new RFID to the RFID list
 
	//printf ("\t\t ::::::::::: Start sending to Server ::::::::::: \n\n");
	
	
	for (int c = 0; c < NumberofRFID.size(); c++){
	/*sprintf (sendThisMsgToKafka, "heath:%d:%d:%d:%d:%d:%d:%d",
	c,NumberofRFID[c]->RfidID,NumberofRFID[c]->productID,
	NumberofRFID[c]->RfidValue1,NumberofRFID[c]->RfidValue2,NumberofRFID[c]->RfidValue3, DATArecieved  );		
	*/
 /*
	sprintf (sendThisMsgToKafka, "health,%d,%d,%d,%d,%d,%d,%d,%d,%d",
	//sprintf (sendThisMsgToKafka, "heath,%d,%d,%d,%d,%d,%d",
	NumberofRFID[c]->RfidID,
	NumberofRFID[c]->patientID,
	NumberofRFID[c]->age,
	NumberofRFID[c]->gender,
	NumberofRFID[c]->height_cm,
	NumberofRFID[c]->weight_kg,
	 qrs,pr,qt  );
	*/
	sprintf (sendThisMsgToKafka, "0,%d,%d,%d,%d,%d,%d,%d",
	NumberofRFID[c]->patientID,
	NumberofRFID[c]->age,
	NumberofRFID[c]->gender,
	NumberofRFID[c]->height_cm,
	NumberofRFID[c]->weight_kg,
	 qrs,
	 pr   );
	
	// send all RFID info
	CsocketKafkaLocal(sendThisMsgToKafka);
	}
	//////////////////////////////////////////////////////////////////////////

	op_prg_mem_free(Rfidlist);
	/* Destroy the received packet */
	op_pk_destroy (packet_MSDU_ptr);
	op_ici_destroy (iciptr);
	/* Stack tracing exit point */
	FOUT;
}


/*--------------------------------------------------------------------------------
 * Function:	wpan_sequence_number()
 *
 * Description:	Generate a random sequence number. 
 *
 * Output: random number  
 *--------------------------------------------------------------------------------*/

static int wpan_sequence_number() {
	
	int seq_num;
	int max_seq;
	
	/* Stack tracing enrty point */
	FIN(wpan_sequence_number);
	
	max_seq = 255;
		
	seq_num = floor (op_dist_uniform (max_seq));
	
	/* Stack tracing exit point */
	FRET(seq_num);
}

/*--------------------------------------------------------------------------------
 * Function:	 wpan_print_parameters
 *
 * Description:	print the parameters
 *				
 * No parameters
 *--------------------------------------------------------------------------------*/

static void wpan_print_parameters() {

	/* Stack tracing enrty point */	
	FIN(wpan_print_parameters);

	if (enable_log) {
		printf ("|------------------------- APPLICATION TRAFFIC -------------------------------|\n", device_name, my_address);
		printf (" Node %s (#%d) \n", device_name, my_address);
		printf (" Best-effort traffic parameters:\n");
		printf ("\t Packet Interarival PDF : %s\n", be_packet_interarrival_dist_str);
		printf ("\t Packet Size PDF        : %s\n", be_packet_size_dist_str);
		
		if (be_start_time == -1) {
			printf ("\t Start time             : disabled \n");
		}
		else {
			printf ("\t Start time             : %f\n", be_start_time);
		}
		
		if (be_stop_time == -1) {
			printf ("\t Stop time              : end of simulation \n");
		}
		else {
			printf ("\t Stop time              : %f\n", be_stop_time);
		}
		
		printf ("\t Acknowledgment	 	: %d\n", be_ack);
		
		printf (" real-time traffic parameters:\n");
		printf ("\t Packet Interarival PDF : %s\n", rt_packet_interarrival_dist_str);
		printf ("\t Packet Size PDF        : %s\n", rt_packet_size_dist_str);
		
		if (rt_start_time == -1) {
			printf ("\t Start time             : disabled \n");
		}
		else {
			printf ("\t Start time             : %f\n", rt_start_time);
		}
		
		if (rt_stop_time == -1) {
			printf ("\t Stop time              : end of simulation \n");
		}
		else {
			printf ("\t Stop time              : %f\n", rt_stop_time);
		}
		
		printf ("\t Acknowledgment	 	: %d\n", rt_ack);
		
		if (dest_mac_address == 0xFFFF) {
			printf (" Destination MAC Address : BROADCAST \n");
		}
		else {
			printf (" Destination MAC Address : #%d (%X hex)\n", dest_mac_address, dest_mac_address);
		}
		printf ("|-----------------------------------------------------------------------------|\n\n");
	}
	
	/* Stack tracing exit point */
	FOUT;
}


/*--------------------------------------------------------------------------------
 * Function:	wpan_rt_packet_generate
 *
 * Description:	Creates a real-time packet (MSDU) based on the packet generation		
 *				 specifications and sends it to the network layer.
 *
 * No parameters
 *--------------------------------------------------------------------------------*/

static void wpan_rt_packet_generate() {

	Ici* iciptr;
	Packet* packet_MSDU_ptr;
	Packet* packet_NSDU_ptr;
	int	nsdu_size; /* size in bits */
	int msdu_size;
	int header_size;
	int sequence_number;
	double next_intarr_time;	/*  interarrival time of next packet */
	double abs_next_intarr_time; /* absolute interarrival time of next packet */

	/* Stack tracing enrty point */
	FIN (wpan_rt_packet_generate);
	
	iciptr = op_ici_create ("wpan_packet_ici_format");
	op_ici_attr_set (iciptr, "ACK", rt_ack);
	op_ici_attr_set (iciptr, "Traffic Type", REAL_TIME_TRAFFIC);
	
	/* create a MAC payload (MSDU) */
	packet_MSDU_ptr = op_pk_create_fmt ("wpan_frame_NPDU_format");
	
	sequence_number = wpan_sequence_number();
	
	/* set the fields of the beacon frame */
	op_pk_nfd_set (packet_MSDU_ptr, "Frame Type", NWK_DATA_FRAME_TYPE);
	op_pk_nfd_set (packet_MSDU_ptr, "Protocol Version", 2);
	op_pk_nfd_set (packet_MSDU_ptr, "Discover Route", SUPPRESS_ROUTE_DISCOVERY);
	op_pk_nfd_set (packet_MSDU_ptr, "Multicast Flag", 0); // unicast or broatcast	
	op_pk_nfd_set (packet_MSDU_ptr, "Security", 0);		// no NWK security
	op_pk_nfd_set (packet_MSDU_ptr, "Source Route", 0);	// source route subframe is not present	
	op_pk_nfd_set (packet_MSDU_ptr, "Destination IEEE Address", 0);
	op_pk_nfd_set (packet_MSDU_ptr, "Source IEEE Address", 0);	
	op_pk_nfd_set (packet_MSDU_ptr, "Destination Address", dest_mac_address);
	op_pk_nfd_set (packet_MSDU_ptr, "Source Address", my_address);	
	op_pk_nfd_set (packet_MSDU_ptr, "Radius", RADIUS);
	op_pk_nfd_set (packet_MSDU_ptr, "Sequence Number", sequence_number);
	
	/* Generate a NSDU size outcome	*/
	nsdu_size = (int) ceil (oms_dist_outcome (rt_packet_size_dist_ptr));
	
	/* Size of the NWk header */ 
	header_size = (int) op_pk_total_size_get(packet_MSDU_ptr);
	
	
	/* 0 <= nsdu_size <= (aMaxMACSafePayloadSize_Octet*8-header_size) */
	if (nsdu_size > (aMaxMACSafePayloadSize_Octet*8-header_size))
		nsdu_size = (aMaxMACSafePayloadSize_Octet*8-header_size);	/* The size of generated NSDU is bigger than the maximum - the size is set to the maximum. */
		
	if (nsdu_size < 0)
		nsdu_size = 0;
	
	/* unformatted payload	*/
	packet_NSDU_ptr = op_pk_create (nsdu_size);
	
	op_pk_nfd_set_pkt (packet_MSDU_ptr, "NSDU", packet_NSDU_ptr);
	
	/* send the MSDU via the stream to the network layer.	*/
	/* send ICI to network layer together with packet. */
	op_ici_install (iciptr);
	op_pk_send (packet_MSDU_ptr, STRM_FROM_APPL_TO_NWK);
	op_ici_install (OPC_NIL);


	if (enable_log) {
		printf (" [%s (#%d)] t=%f -> (APL) Real-time packet's payload (MSDU size = %d bits, SEQ = %d) was generated and sent to Network layer.\n", device_name, my_address, op_sim_time(), (int) op_pk_total_size_get(packet_MSDU_ptr), sequence_number);
	}
	
	/* schedule next packet generation */
	next_intarr_time = oms_dist_outcome (rt_packet_interarrival_dist_ptr);

	/* Make sure that interarrival time is not negative. In that case it will be set to 0. */
	if (next_intarr_time <0)
		next_intarr_time = 0.0;

	abs_next_intarr_time = op_sim_time () + next_intarr_time;
	
	if ((abs_next_intarr_time <= rt_stop_time) || (rt_stop_time == END_OF_SIMULATION)) {
		op_intrpt_schedule_self (abs_next_intarr_time, SC_SEND_RT);
		
		if (enable_log)
			printf ("\t (APL) Next real-time packet's payload (MSDU) will be generated at %f sec.\n\n", abs_next_intarr_time);
	} else {
		if (enable_log)
			printf ("\t (APL) STOP real-time packet's payload (MSDU) generation.\n\n", abs_next_intarr_time);
	}
	
	msdu_size = (int) op_pk_total_size_get (packet_MSDU_ptr);
	
	/* Update the packet generation statistics.			*/
	op_stat_write (bits_sent_MSDU_rt_stathandle, msdu_size);
	op_stat_write (bits_sent_NSDU_rt_stathandle, nsdu_size);
	op_stat_write (pkts_sent_rt_stathandle, 1.0);
	op_stat_write (bits_sent_MSDU_rt_gstathandle, msdu_size);	
	op_stat_write (pkts_sent_rt_gstathandle, 1.0);
	
	rt_generated_frames++;
	
	 
	
	//app_sent_msdu_bits = app_sent_msdu_bits + (msdu_size/1000.0);	
	//app_sent_msdu_nbr = app_sent_msdu_nbr + 1;
	
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
#undef self_id
#undef parent_id
#undef device_mode
#undef device_name
#undef my_address
#undef enable_log
#undef intrpt_type
#undef intrpt_strm
#undef intrpt_code
#undef be_packet_interarrival_dist_ptr
#undef be_packet_size_dist_ptr
#undef be_packet_interarrival_dist_str
#undef be_packet_size_dist_str
#undef be_start_time
#undef be_stop_time
#undef be_ack
#undef rt_packet_interarrival_dist_ptr
#undef rt_packet_size_dist_ptr
#undef rt_packet_interarrival_dist_str
#undef rt_packet_size_dist_str
#undef rt_start_time
#undef rt_stop_time
#undef rt_ack
#undef dest_mac_address
#undef bits_rcvd_MSDU_be_stathandle
#undef bits_rcvd_MSDU_rt_stathandle
#undef bitssec_rcvd_MSDU_be_stathandle
#undef bits_rcvd_NSDU_be_stathandle
#undef bits_rcvd_NSDU_rt_stathandle
#undef bitssec_rcvd_NSDU_be_stathandle
#undef pkts_rcvd_be_stathandle
#undef pkts_rcvd_rt_stathandle
#undef pktssec_rcvd_be_stathandle
#undef pktssec_rcvd_rt_stathandle
#undef bits_sent_MSDU_be_stathandle
#undef bits_sent_MSDU_rt_stathandle
#undef bits_sent_NSDU_be_stathandle
#undef bits_sent_NSDU_rt_stathandle
#undef pkts_sent_be_stathandle
#undef pkts_sent_rt_stathandle
#undef ete_delay_be_stathandle
#undef ete_delay_rt_stathandle
#undef bits_rcvd_MSDU_be_gstathandle
#undef bits_rcvd_MSDU_rt_gstathandle
#undef bitssec_rcvd_gstathandle
#undef pkts_rcvd_be_gstathandle
#undef pkts_rcvd_rt_gstathandle
#undef pktssec_rcvd_gstathandle
#undef ete_delay_be_gstathandle
#undef ete_delay_rt_gstathandle
#undef bits_sent_MSDU_be_gstathandle
#undef bits_sent_MSDU_rt_gstathandle
#undef pkts_sent_be_gstathandle
#undef pkts_sent_rt_gstathandle
#undef rt_generated_frames
#undef be_generated_frames
#undef bitsec_sent_NSDU_be_stathandle
#undef pktsec_sent_be_stathandle
#undef DATArecieved
#undef sendDATA
#undef Applog_file
#undef RFID_log
#undef ListOfRFID
#undef Lsize
#undef RFIDno

/* Access from C kernel using C linkage */
extern "C"
{
	VosT_Obtype _op_EDAppLayerLocal_init (int * init_block_ptr);
	VosT_Address _op_EDAppLayerLocal_alloc (VosT_Obtype, int);
	void EDAppLayerLocal (OP_SIM_CONTEXT_ARG_OPT)
		{
		((EDAppLayerLocal_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))->EDAppLayerLocal (OP_SIM_CONTEXT_PTR_OPT);
		}

	void _op_EDAppLayerLocal_svar (void *, const char *, void **);

	void _op_EDAppLayerLocal_diag (OP_SIM_CONTEXT_ARG_OPT)
		{
		((EDAppLayerLocal_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))->_op_EDAppLayerLocal_diag (OP_SIM_CONTEXT_PTR_OPT);
		}

	void _op_EDAppLayerLocal_terminate (OP_SIM_CONTEXT_ARG_OPT)
		{
		/* The destructor is the Termination Block */
		delete (EDAppLayerLocal_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr);
		}


} /* end of 'extern "C"' */




/* Process model interrupt handling procedure */


void
EDAppLayerLocal_state::EDAppLayerLocal (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (EDAppLayerLocal_state::EDAppLayerLocal ());
	try
		{
		/* Temporary Variables */
		Packet*		RfidPkptr;
		
		 
		/* End of Temporary Variables */


		FSM_ENTER ("EDAppLayerLocal")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (init) enter executives **/
			FSM_STATE_ENTER_FORCED_NOLABEL (0, "init", "EDAppLayerLocal [init enter execs]")
				FSM_PROFILE_SECTION_IN ("EDAppLayerLocal [init enter execs]", state0_enter_exec)
				{
				/* Initialization */
				wpan_appl_init ();
				}
				FSM_PROFILE_SECTION_OUT (state0_enter_exec)

			/** state (init) exit executives **/
			FSM_STATE_EXIT_FORCED (0, "init", "EDAppLayerLocal [init exit execs]")


			/** state (init) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "init", "idle", "tr_1", "EDAppLayerLocal [init -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (idle) enter executives **/
			FSM_STATE_ENTER_UNFORCED (1, "idle", state1_enter_exec, "EDAppLayerLocal [idle enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (3,"EDAppLayerLocal")


			/** state (idle) exit executives **/
			FSM_STATE_EXIT_UNFORCED (1, "idle", "EDAppLayerLocal [idle exit execs]")
				FSM_PROFILE_SECTION_IN ("EDAppLayerLocal [idle exit execs]", state1_exit_exec)
				{
				intrpt_type = op_intrpt_type ();
				
				/*
				if (intrpt_type == OPC_INTRPT_STRM)
					intrpt_strm = op_intrpt_strm ();
				else
					intrpt_code = op_intrpt_code ();
				*/
				
				switch (intrpt_type) {
					case OPC_INTRPT_STRM:
					{
						intrpt_strm = op_intrpt_strm ();
						break;
					};
					
					case OPC_INTRPT_SELF:
					case OPC_INTRPT_REMOTE:
					{
						intrpt_code = op_intrpt_code ();
						break;
					};
					
					case OPC_INTRPT_ENDSIM:
					{
						printf ("|------------------------ APPLICATION STATISTICS -----------------------------|\n");
						printf (" Node %s (#%d) \n", device_name, my_address);
						printf ("  Number of GENERATED real-time data frames = %d  \n", rt_generated_frames);
						printf ("  Number of GENERATED best-effort data frames = %d  \n\n", be_generated_frames);
						printf ("|-----------------------------------------------------------------------------|\n\n");
						
						/* scalar statistics */
						op_stat_scalar_write ("Real-time data frames GENERATED", rt_generated_frames);
						op_stat_scalar_write ("Best-effort data frames GENERATED", be_generated_frames);
						
						break;
					};
					
					default :
					{
					};	
				}
				}
				FSM_PROFILE_SECTION_OUT (state1_exit_exec)


			/** state (idle) transition processing **/
			FSM_PROFILE_SECTION_IN ("EDAppLayerLocal [idle trans conditions]", state1_trans_conds)
			FSM_INIT_COND (SEND_BE)
			FSM_TEST_COND (SEND_RT)
			FSM_TEST_COND (RECEIVE_PCK)
			FSM_TEST_COND (START_BE_APP)
			FSM_TEST_COND (ReceiveRFID)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("idle")
			FSM_PROFILE_SECTION_OUT (state1_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "SEND_BE", "", "idle", "send BE", "send best-effort packet", "EDAppLayerLocal [idle -> send BE : SEND_BE / ]")
				FSM_CASE_TRANSIT (1, 3, state3_enter_exec, ;, "SEND_RT", "", "idle", "send RT", "send real-time packet", "EDAppLayerLocal [idle -> send RT : SEND_RT / ]")
				FSM_CASE_TRANSIT (2, 4, state4_enter_exec, ;, "RECEIVE_PCK", "", "idle", "receive pck", "receive packet from NWK", "EDAppLayerLocal [idle -> receive pck : RECEIVE_PCK / ]")
				FSM_CASE_TRANSIT (3, 2, state2_enter_exec, ;, "START_BE_APP", "", "idle", "send BE", "tr_14", "EDAppLayerLocal [idle -> send BE : START_BE_APP / ]")
				FSM_CASE_TRANSIT (4, 5, state5_enter_exec, ;, "ReceiveRFID", "", "idle", "receive RFID", "tr_16", "EDAppLayerLocal [idle -> receive RFID : ReceiveRFID / ]")
				FSM_CASE_TRANSIT (5, 1, state1_enter_exec, ;, "default", "", "idle", "idle", "tr_13", "EDAppLayerLocal [idle -> idle : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (send BE) enter executives **/
			FSM_STATE_ENTER_FORCED (2, "send BE", state2_enter_exec, "EDAppLayerLocal [send BE enter execs]")
				FSM_PROFILE_SECTION_IN ("EDAppLayerLocal [send BE enter execs]", state2_enter_exec)
				{
				wpan_be_packet_generate();
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** state (send BE) exit executives **/
			FSM_STATE_EXIT_FORCED (2, "send BE", "EDAppLayerLocal [send BE exit execs]")


			/** state (send BE) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "send BE", "idle", "tr_6", "EDAppLayerLocal [send BE -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (send RT) enter executives **/
			FSM_STATE_ENTER_FORCED (3, "send RT", state3_enter_exec, "EDAppLayerLocal [send RT enter execs]")
				FSM_PROFILE_SECTION_IN ("EDAppLayerLocal [send RT enter execs]", state3_enter_exec)
				{
				wpan_rt_packet_generate();
				}
				FSM_PROFILE_SECTION_OUT (state3_enter_exec)

			/** state (send RT) exit executives **/
			FSM_STATE_EXIT_FORCED (3, "send RT", "EDAppLayerLocal [send RT exit execs]")


			/** state (send RT) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "send RT", "idle", "tr_8", "EDAppLayerLocal [send RT -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (receive pck) enter executives **/
			FSM_STATE_ENTER_FORCED (4, "receive pck", state4_enter_exec, "EDAppLayerLocal [receive pck enter execs]")
				FSM_PROFILE_SECTION_IN ("EDAppLayerLocal [receive pck enter execs]", state4_enter_exec)
				{
				wpan_receive_pck_from_nwk();
				}
				FSM_PROFILE_SECTION_OUT (state4_enter_exec)

			/** state (receive pck) exit executives **/
			FSM_STATE_EXIT_FORCED (4, "receive pck", "EDAppLayerLocal [receive pck exit execs]")


			/** state (receive pck) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "receive pck", "idle", "tr_10", "EDAppLayerLocal [receive pck -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (receive RFID) enter executives **/
			FSM_STATE_ENTER_FORCED (5, "receive RFID", state5_enter_exec, "EDAppLayerLocal [receive RFID enter execs]")
				FSM_PROFILE_SECTION_IN ("EDAppLayerLocal [receive RFID enter execs]", state5_enter_exec)
				{
				//if (RFID_log)
					{
				fprintf (Applog_file, "hi hbi this application layer the RFID info list arrived to ED*************************\n");
				
				//   here we get node statisics from incoming packet 
				int TagID, patientID,age,gender, height_cm,weight_kg;
				
				RfidPkptr = op_pk_get (op_intrpt_strm ());
				//if (op_pk_nfd_get_ptr(RfidPkptr, "FlowTables", (void**) &flowEntery) == OPC_COMPCODE_SUCCESS)
				op_pk_nfd_get(RfidPkptr, "TagID", &TagID);
				op_pk_nfd_get(RfidPkptr, "PatientID", &patientID);
				op_pk_nfd_get(RfidPkptr, "Age", &age);
				op_pk_nfd_get(RfidPkptr, "Gender", &gender);
				op_pk_nfd_get(RfidPkptr, "Height_cm", &height_cm);
				op_pk_nfd_get(RfidPkptr, "Weight_kg", &weight_kg);
				
				fprintf(Applog_file,"Reader sent ProductID: %d**\n",patientID);
				fprintf(Applog_file,"Reader sent TagID: %d**\n",TagID);
				fprintf(Applog_file,"Reader sent Age: %d**\n",age);
				fprintf(Applog_file,"Reader sent Gender: %d**\n",gender);
				fprintf(Applog_file,"Reader sent Height_cm: %d**\n",height_cm);
				fprintf(Applog_file,"Reader sent Weight_kg: %d**\n",weight_kg);
				
				
				RFIDList* Rfidlist = new RFIDList;
				
				Rfidlist->RfidID=TagID;
				Rfidlist->patientID = patientID;
				Rfidlist->age=age;
				Rfidlist->gender=gender;
				Rfidlist->height_cm=height_cm;
				Rfidlist->weight_kg=weight_kg;
				// here we are update the vlaue of the Rfid if it has recieved before 
				// otherwised it will add it to the list
				for (int c = 0; c < ListOfRFID.size(); c++)
					if (ListOfRFID[c]->RfidID == Rfidlist->RfidID){ 
							ListOfRFID.erase(ListOfRFID.begin()+c);
				 }
				
				ListOfRFID.push_back(Rfidlist);// add new RFID to the RFID list
				op_prg_mem_free(Rfidlist);
				
				for(int c=0;c<ListOfRFID.size();c++){
				//if (RFID_log)
				fprintf (Applog_file, "TagID=%d & productID== %d & RfidValue1=%d & RfidValue2=%d RfidValue3=%d \n",
					ListOfRFID[c]->RfidID,
					ListOfRFID[c]->patientID,
					ListOfRFID[c]->age,
					ListOfRFID[c]->gender,
					ListOfRFID[c]->height_cm, 
					ListOfRFID[c]->weight_kg
					);
				  
				}//end for
				}// if RFID log
				}
				FSM_PROFILE_SECTION_OUT (state5_enter_exec)

			/** state (receive RFID) exit executives **/
			FSM_STATE_EXIT_FORCED (5, "receive RFID", "EDAppLayerLocal [receive RFID exit execs]")


			/** state (receive RFID) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "receive RFID", "idle", "tr_15", "EDAppLayerLocal [receive RFID -> idle : default / ]")
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (0,"EDAppLayerLocal")
		}
	catch (...)
		{
		Vos_Error_Print (VOSC_ERROR_ABORT,
			(const char *)VOSC_NIL,
			"Unhandled C++ exception in process model (EDAppLayerLocal)",
			(const char *)VOSC_NIL, (const char *)VOSC_NIL);
		}
	}




void
EDAppLayerLocal_state::_op_EDAppLayerLocal_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}

void
EDAppLayerLocal_state::operator delete (void* ptr)
	{
	FIN (EDAppLayerLocal_state::operator delete (ptr));
	Vos_Poolmem_Dealloc (ptr);
	FOUT
	}

EDAppLayerLocal_state::~EDAppLayerLocal_state (void)
	{

	FIN (EDAppLayerLocal_state::~EDAppLayerLocal_state ())


	/* No Termination Block */


	FOUT
	}


#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

void *
EDAppLayerLocal_state::operator new (size_t)
#if defined (VOSD_NEW_BAD_ALLOC)
		throw (VOSD_BAD_ALLOC)
#endif
	{
	void * new_ptr;

	FIN_MT (EDAppLayerLocal_state::operator new ());

	new_ptr = Vos_Alloc_Object (EDAppLayerLocal_state::obtype);
#if defined (VOSD_NEW_BAD_ALLOC)
	if (new_ptr == VOSC_NIL) throw VOSD_BAD_ALLOC();
#endif
	FRET (new_ptr)
	}

/* State constructor initializes FSM handling */
/* by setting the initial state to the first */
/* block of code to enter. */

EDAppLayerLocal_state::EDAppLayerLocal_state (void) :
		_op_current_block (0)
	{
#if defined (OPD_ALLOW_ODB)
		_op_current_state = "EDAppLayerLocal [init enter execs]";
#endif
	}

VosT_Obtype
_op_EDAppLayerLocal_init (int * init_block_ptr)
	{
	FIN_MT (_op_EDAppLayerLocal_init (init_block_ptr))

	EDAppLayerLocal_state::obtype = Vos_Define_Object_Prstate ("proc state vars (EDAppLayerLocal)",
		sizeof (EDAppLayerLocal_state));
	*init_block_ptr = 0;

	FRET (EDAppLayerLocal_state::obtype)
	}

VosT_Address
_op_EDAppLayerLocal_alloc (VosT_Obtype, int)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	EDAppLayerLocal_state * ptr;
	FIN_MT (_op_EDAppLayerLocal_alloc ())

	/* New instance will have FSM handling initialized */
#if defined (VOSD_NEW_BAD_ALLOC)
	try {
		ptr = new EDAppLayerLocal_state;
	} catch (const VOSD_BAD_ALLOC &) {
		ptr = VOSC_NIL;
	}
#else
	ptr = new EDAppLayerLocal_state;
#endif
	FRET ((VosT_Address)ptr)
	}



void
_op_EDAppLayerLocal_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	EDAppLayerLocal_state		*prs_ptr;

	FIN_MT (_op_EDAppLayerLocal_svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (EDAppLayerLocal_state *)gen_ptr;

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
	if (strcmp ("be_packet_interarrival_dist_ptr" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->be_packet_interarrival_dist_ptr);
		FOUT
		}
	if (strcmp ("be_packet_size_dist_ptr" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->be_packet_size_dist_ptr);
		FOUT
		}
	if (strcmp ("be_packet_interarrival_dist_str" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->be_packet_interarrival_dist_str);
		FOUT
		}
	if (strcmp ("be_packet_size_dist_str" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->be_packet_size_dist_str);
		FOUT
		}
	if (strcmp ("be_start_time" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->be_start_time);
		FOUT
		}
	if (strcmp ("be_stop_time" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->be_stop_time);
		FOUT
		}
	if (strcmp ("be_ack" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->be_ack);
		FOUT
		}
	if (strcmp ("rt_packet_interarrival_dist_ptr" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->rt_packet_interarrival_dist_ptr);
		FOUT
		}
	if (strcmp ("rt_packet_size_dist_ptr" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->rt_packet_size_dist_ptr);
		FOUT
		}
	if (strcmp ("rt_packet_interarrival_dist_str" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->rt_packet_interarrival_dist_str);
		FOUT
		}
	if (strcmp ("rt_packet_size_dist_str" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->rt_packet_size_dist_str);
		FOUT
		}
	if (strcmp ("rt_start_time" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->rt_start_time);
		FOUT
		}
	if (strcmp ("rt_stop_time" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->rt_stop_time);
		FOUT
		}
	if (strcmp ("rt_ack" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->rt_ack);
		FOUT
		}
	if (strcmp ("dest_mac_address" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->dest_mac_address);
		FOUT
		}
	if (strcmp ("bits_rcvd_MSDU_be_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bits_rcvd_MSDU_be_stathandle);
		FOUT
		}
	if (strcmp ("bits_rcvd_MSDU_rt_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bits_rcvd_MSDU_rt_stathandle);
		FOUT
		}
	if (strcmp ("bitssec_rcvd_MSDU_be_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bitssec_rcvd_MSDU_be_stathandle);
		FOUT
		}
	if (strcmp ("bits_rcvd_NSDU_be_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bits_rcvd_NSDU_be_stathandle);
		FOUT
		}
	if (strcmp ("bits_rcvd_NSDU_rt_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bits_rcvd_NSDU_rt_stathandle);
		FOUT
		}
	if (strcmp ("bitssec_rcvd_NSDU_be_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bitssec_rcvd_NSDU_be_stathandle);
		FOUT
		}
	if (strcmp ("pkts_rcvd_be_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pkts_rcvd_be_stathandle);
		FOUT
		}
	if (strcmp ("pkts_rcvd_rt_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pkts_rcvd_rt_stathandle);
		FOUT
		}
	if (strcmp ("pktssec_rcvd_be_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pktssec_rcvd_be_stathandle);
		FOUT
		}
	if (strcmp ("pktssec_rcvd_rt_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pktssec_rcvd_rt_stathandle);
		FOUT
		}
	if (strcmp ("bits_sent_MSDU_be_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bits_sent_MSDU_be_stathandle);
		FOUT
		}
	if (strcmp ("bits_sent_MSDU_rt_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bits_sent_MSDU_rt_stathandle);
		FOUT
		}
	if (strcmp ("bits_sent_NSDU_be_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bits_sent_NSDU_be_stathandle);
		FOUT
		}
	if (strcmp ("bits_sent_NSDU_rt_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bits_sent_NSDU_rt_stathandle);
		FOUT
		}
	if (strcmp ("pkts_sent_be_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pkts_sent_be_stathandle);
		FOUT
		}
	if (strcmp ("pkts_sent_rt_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pkts_sent_rt_stathandle);
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
	if (strcmp ("bits_rcvd_MSDU_be_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bits_rcvd_MSDU_be_gstathandle);
		FOUT
		}
	if (strcmp ("bits_rcvd_MSDU_rt_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bits_rcvd_MSDU_rt_gstathandle);
		FOUT
		}
	if (strcmp ("bitssec_rcvd_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bitssec_rcvd_gstathandle);
		FOUT
		}
	if (strcmp ("pkts_rcvd_be_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pkts_rcvd_be_gstathandle);
		FOUT
		}
	if (strcmp ("pkts_rcvd_rt_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pkts_rcvd_rt_gstathandle);
		FOUT
		}
	if (strcmp ("pktssec_rcvd_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pktssec_rcvd_gstathandle);
		FOUT
		}
	if (strcmp ("ete_delay_be_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->ete_delay_be_gstathandle);
		FOUT
		}
	if (strcmp ("ete_delay_rt_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->ete_delay_rt_gstathandle);
		FOUT
		}
	if (strcmp ("bits_sent_MSDU_be_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bits_sent_MSDU_be_gstathandle);
		FOUT
		}
	if (strcmp ("bits_sent_MSDU_rt_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bits_sent_MSDU_rt_gstathandle);
		FOUT
		}
	if (strcmp ("pkts_sent_be_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pkts_sent_be_gstathandle);
		FOUT
		}
	if (strcmp ("pkts_sent_rt_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pkts_sent_rt_gstathandle);
		FOUT
		}
	if (strcmp ("rt_generated_frames" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->rt_generated_frames);
		FOUT
		}
	if (strcmp ("be_generated_frames" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->be_generated_frames);
		FOUT
		}
	if (strcmp ("bitsec_sent_NSDU_be_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bitsec_sent_NSDU_be_stathandle);
		FOUT
		}
	if (strcmp ("pktsec_sent_be_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pktsec_sent_be_stathandle);
		FOUT
		}
	if (strcmp ("DATArecieved" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->DATArecieved);
		FOUT
		}
	if (strcmp ("sendDATA" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->sendDATA);
		FOUT
		}
	if (strcmp ("Applog_file" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->Applog_file);
		FOUT
		}
	if (strcmp ("RFID_log" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->RFID_log);
		FOUT
		}
	if (strcmp ("ListOfRFID" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->ListOfRFID);
		FOUT
		}
	if (strcmp ("Lsize" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->Lsize);
		FOUT
		}
	if (strcmp ("RFIDno" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->RFIDno);
		FOUT
		}
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

