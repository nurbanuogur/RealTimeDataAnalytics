/* Process model C form file: RfidMACtagProcess.pr.c */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char RfidMACtagProcess_pr_c [] = "MIL_3_Tfile_Hdr_ 145A 30A modeler 7 5BCF6BC6 5BCF6BC6 1 DESKTOP-L8S8FMO Mohammed@Hussein 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1e80 8                                                                                                                                                                                                                                                                                                                                                                                         ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define EPSILON    0.000000000001


#define TX_OUT_STRM		(0)
#define SINK_OUT_STRM	(1)
#define RX_IN_STRM		(1)

#define SELECT (came_select==OPC_TRUE)
#define QUERY (came_query==OPC_TRUE || came_query_rep==OPC_TRUE || came_query_adj==OPC_TRUE) && (reply_mod==OPC_FALSE)
//#define QUERY_REP (came_query_rep==1)
#define REPLY (reply_state==OPC_TRUE)
#define ARBITRATE (arbitrate_state==OPC_TRUE)
#define RTRN_RCV_PKT (rtrn_rcv_pkt==OPC_TRUE)
#define ACK (came_ack==OPC_TRUE || came_read_req==OPC_TRUE) && (ack_mod==OPC_TRUE)
#define NEW_SESSION (came_kill==OPC_TRUE)
#define NEWRPLY (newreply_mod==OPC_TRUE)

#define tIMEoUTINTcODE	(1)
#define newRN16INTcODE (1)
#define replysendintcode (1)

#define ReceivePktRX	(current_intrpt_type == OPC_INTRPT_STRM) && (op_intrpt_strm () == RX_IN_STRM) 
#define wAITfORACK (packetWasSent==1)
#define qUEUEiSeMPTY 		((op_subq_empty(0)))
#define tIMEoUT 			(current_intrpt_type == OPC_INTRPT_SELF) && ((op_intrpt_code () == tIMEoUTINTcODE))

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
	Objid	                  		my_id                                           ;
	Objid	                  		my_node_id                                      ;
	double	                 		tx_data_rate                                    ;
	double	                 		slot_genisligi                                  ;
	int	                    		terminalAddress                                 ;
	int	                    		destinationAddress                              ;
	Evhandle	               		timeOutEndEvh                                   ;
	Stathandle	             		timerCancelStat                                 ;
	Boolean	                		came_select                                     ;
	Boolean	                		came_query                                      ;
	Boolean	                		reply_state                                     ;
	Boolean	                		arbitrate_state                                 ;
	Boolean	                		rtrn_rcv_pkt                                    ;
	int	                    		header                                          ;
	int	                    		man_number                                      ;
	int	                    		object_class                                    ;
	OpT_Int64	              		serial_number                                   ;
	Boolean	                		came_ack                                        ;
	Boolean	                		came_query_rep                                  ;
	int	                    		random_Qvalue                                   ;
	Boolean	                		came_read_req                                   ;
	Boolean	                		read_Complated                                  ;
	Boolean	                		came_query_adj                                  ;
	int	                    		Qvalue                                          ;
	int	                    		Qrange                                          ;
	Boolean	                		came_kill                                       ;
	int	                    		temp_Qvalue                                     ;
	Boolean	                		reply_mod                                       ;
	int	                    		hasta_2_kimlik                                  ;
	int	                    		hasta_2_yas                                     ;
	Packet *	               		came_packet                                     ;
	Evhandle	               		reply_send                                      ;
	Boolean	                		reply_was_send                                  ;
	int	                    		random_RN16                                     ;
	Boolean	                		ack_mod                                         ;
	int	                    		EDAddress                                       ;
	int	                    		SerialNumber                                    ;
	int	                    		patientID                                       ;
	int	                    		tagID                                           ;
	Evhandle	               		newRN16evh                                      ;
	Boolean	                		newreply_mod                                    ;
	FILE*	                  		tag_log                                         ;
	int	                    		age                                             ;
	int	                    		gender                                          ;
	int	                    		height_cm                                       ;
	int	                    		weight_kg                                       ;
	} RfidMACtagProcess_state;

#define pr_state_ptr            		((RfidMACtagProcess_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))
#define my_id                   		pr_state_ptr->my_id
#define my_node_id              		pr_state_ptr->my_node_id
#define tx_data_rate            		pr_state_ptr->tx_data_rate
#define slot_genisligi          		pr_state_ptr->slot_genisligi
#define terminalAddress         		pr_state_ptr->terminalAddress
#define destinationAddress      		pr_state_ptr->destinationAddress
#define timeOutEndEvh           		pr_state_ptr->timeOutEndEvh
#define timerCancelStat         		pr_state_ptr->timerCancelStat
#define came_select             		pr_state_ptr->came_select
#define came_query              		pr_state_ptr->came_query
#define reply_state             		pr_state_ptr->reply_state
#define arbitrate_state         		pr_state_ptr->arbitrate_state
#define rtrn_rcv_pkt            		pr_state_ptr->rtrn_rcv_pkt
#define header                  		pr_state_ptr->header
#define man_number              		pr_state_ptr->man_number
#define object_class            		pr_state_ptr->object_class
#define serial_number           		pr_state_ptr->serial_number
#define came_ack                		pr_state_ptr->came_ack
#define came_query_rep          		pr_state_ptr->came_query_rep
#define random_Qvalue           		pr_state_ptr->random_Qvalue
#define came_read_req           		pr_state_ptr->came_read_req
#define read_Complated          		pr_state_ptr->read_Complated
#define came_query_adj          		pr_state_ptr->came_query_adj
#define Qvalue                  		pr_state_ptr->Qvalue
#define Qrange                  		pr_state_ptr->Qrange
#define came_kill               		pr_state_ptr->came_kill
#define temp_Qvalue             		pr_state_ptr->temp_Qvalue
#define reply_mod               		pr_state_ptr->reply_mod
#define hasta_2_kimlik          		pr_state_ptr->hasta_2_kimlik
#define hasta_2_yas             		pr_state_ptr->hasta_2_yas
#define came_packet             		pr_state_ptr->came_packet
#define reply_send              		pr_state_ptr->reply_send
#define reply_was_send          		pr_state_ptr->reply_was_send
#define random_RN16             		pr_state_ptr->random_RN16
#define ack_mod                 		pr_state_ptr->ack_mod
#define EDAddress               		pr_state_ptr->EDAddress
#define SerialNumber            		pr_state_ptr->SerialNumber
#define patientID               		pr_state_ptr->patientID
#define tagID                   		pr_state_ptr->tagID
#define newRN16evh              		pr_state_ptr->newRN16evh
#define newreply_mod            		pr_state_ptr->newreply_mod
#define tag_log                 		pr_state_ptr->tag_log
#define age                     		pr_state_ptr->age
#define gender                  		pr_state_ptr->gender
#define height_cm               		pr_state_ptr->height_cm
#define weight_kg               		pr_state_ptr->weight_kg

/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	RfidMACtagProcess_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((RfidMACtagProcess_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


/* Function Block */

#if !defined (VOSD_NO_FIN)
enum { _op_block_origin = __LINE__ + 2};
#endif

static void Receive_packet(void) {

char pk_format[50];

FIN(Receive_packet ());
if((came_packet = op_pk_get (RX_IN_STRM))!=OPC_NIL)
{	
op_pk_format(came_packet,pk_format);
//fprintf (tag_log, "Tag packet Type -> %s    \n",pk_format);
 
	 if (strcmp("query_packet_format",pk_format)==0) {	
		 came_query=OPC_TRUE;
		 fprintf (tag_log, "|t = %f ->  receive pkt :query_packet_format \n",op_sim_time());
		 } 
	 else if (strcmp("rfid_ack_packet_format",pk_format)==0) {
		came_ack=OPC_TRUE;  
		fprintf (tag_log, "|t = %f ->  receive pkt :rfid_ack_packet_format \n",op_sim_time());
		} 
	 else if (strcmp("queryRep_packet_format",pk_format)==0){
		came_query_rep=OPC_TRUE; // here is the first most called case
		// so much print for this sentence
		//fprintf (tag_log, "|t = %f ->  receive pkt :queryRep_packet_format \n",op_sim_time());
		 } 
	 else if (strcmp("read_packet_format",pk_format)==0)	{
		came_read_req=OPC_TRUE;
		fprintf (tag_log, "|t = %f -> receive pkt :read_packet_format \n",op_sim_time());
		 } 
	 else if (strcmp("queryAdjust_packet_format",pk_format)==0){
		came_query_adj=OPC_TRUE;// here is the second most called case
		// so much print for this sentence
		//fprintf (tag_log, "|t = %f -> receive pkt :queryAdjust_packet_format \n",op_sim_time());
		} 
	 else if (strcmp("kill_packet",pk_format)==0) {
		came_kill=OPC_TRUE;
		fprintf (tag_log, "|t = %f -> receive pkt :kill_packet \n",op_sim_time());
		 } 
	}

if(ack_mod==OPC_TRUE)
	{
	 if (came_ack==OPC_FALSE && came_read_req==OPC_FALSE)
		 {
		 newRN16evh=op_intrpt_schedule_self ((op_sim_time()+0.05),newRN16INTcODE);
		 //newRN16evh=op_intrpt_schedule_self ((op_sim_time()+1),newRN16INTcODE);
		 if (op_ev_valid(newRN16evh))
				{
					newreply_mod=OPC_TRUE;
					op_ev_cancel(newRN16evh);
				}
		 }
	 }
	
FOUT;
}

static void  QueryTag() {
FIN (QueryTag ());
if(came_query==OPC_TRUE)
{
op_pk_nfd_get(came_packet, "Q", &Qvalue);
temp_Qvalue=Qvalue;
Qrange=pow(2,Qvalue)-1;
//random_Qvalue_temp=op_dist_load("uniform_int",Qrange,0);
//random_Qvalue=op_dist_outcome(random_Qvalue_temp);
//random_Qvalue2=op_dist_exponential(Qrange);
srand(time(NULL)/tagID);
random_Qvalue=rand()%Qrange;
fprintf (tag_log,"%d. Tag has update the Q=rand()%(Q^2-1) value : %d**\n",tagID,random_Qvalue);
came_query=OPC_FALSE;
}
else if (came_query_adj==OPC_TRUE)
	{int UpDn_value;
	op_pk_nfd_get(came_packet, "UpDn", &UpDn_value);
	if (UpDn_value==2)
		{
		Qvalue--;
		if (Qvalue==0)
			{
			Qvalue=temp_Qvalue;
			}
		Qrange=pow(2,Qvalue)-1; 	    srand(time(NULL)/tagID);
	    random_Qvalue=rand()%Qrange;
	    fprintf (tag_log,"%d. Label take a new  value: %d**\n",tagID,random_Qvalue);
		}
	    came_query_adj=OPC_FALSE;
	}
	
else if(came_query_rep==OPC_TRUE)
	{
	random_Qvalue=random_Qvalue-1;
	fprintf (tag_log,"%d. label random_Qvalue flaw: %d**\n",tagID,random_Qvalue);
	came_query_rep=OPC_FALSE;
	}


FOUT;
}


static void  ReplyTag() {
Packet *RN16_pkt;

FIN (ReplyTag ());
// create rfid_RN16_tag_packet_format
   RN16_pkt= op_pk_create_fmt("rfid_RN16_tag_packet_format");
   srand(time(NULL)/tagID);
   random_RN16=rand();
 //printf( "******** op_pk_print ReplyTag() *********\n" );
	//		op_pk_print (RN16_pkt);
   op_pk_nfd_set (RN16_pkt, "RN",random_RN16);
   fprintf (tag_log,"%d. rfid_RN16_tag_packet_format: Label RN16 another random_RN16: %d**\n",tagID,random_RN16);

   op_pk_send (RN16_pkt, TX_OUT_STRM);
   reply_state=OPC_FALSE;
   reply_mod=OPC_TRUE;
   ack_mod=OPC_TRUE;
   newreply_mod=OPC_FALSE;
   //reply_was_send=OPC_TRUE;

FOUT;
}


static void  AckTag() {
int came_RN16;
char pk_format[50];
Packet*	epc_pkt; Packet* send_epc_pkt; Packet* read_tag_reply;
FIN (AckTag ());

 	if (came_ack==OPC_TRUE)
		{
		//printf( "******** op_pk_print AckTag() *********\n" );
			//op_pk_print (came_packet);
		
 
		op_pk_format(came_packet,pk_format);
		if (strcmp("rfid_ack_packet_format",pk_format)==0) {
			op_pk_nfd_get(came_packet, "RN16", &came_RN16);
			came_ack=OPC_FALSE;
			if (random_RN16==came_RN16)
			{
			epc_pkt = op_pk_create_fmt ("rfid_epc_packet");
			op_pk_nfd_set (epc_pkt, "Header", header);
			op_pk_nfd_set (epc_pkt, "Man Number",man_number); 
			op_pk_nfd_set (epc_pkt, "Object Class", object_class);
			op_pk_nfd_set (epc_pkt, "Serial Number", SerialNumber);
			send_epc_pkt = op_pk_create_fmt ("rfid_epc_send_packet");
			op_pk_nfd_set_pkt (send_epc_pkt, "EPC_number", epc_pkt);
			op_pk_send (send_epc_pkt, TX_OUT_STRM);
			fprintf(tag_log,"%d.ACK send Label of EPC : **\n",tagID);
 
			}
			}
		}
	else if(came_read_req==OPC_TRUE)
		{	// from AckTag() function
// here we get the information of the RFID tag then send it to RFID Reader
		read_tag_reply= op_pk_create_fmt ("read_reply_tag_packet_format");
		op_pk_nfd_set (read_tag_reply, "TagID", tagID);
		op_pk_nfd_set (read_tag_reply, "PatientID", patientID);
		op_pk_nfd_set (read_tag_reply, "Age", age); 
		op_pk_nfd_set (read_tag_reply, "Gender", gender); 
		op_pk_nfd_set (read_tag_reply, "Height_cm", height_cm); 
		op_pk_nfd_set (read_tag_reply, "Weight_kg", weight_kg); 
		
		op_pk_send (read_tag_reply, TX_OUT_STRM);
		came_read_req=OPC_FALSE;
		read_Complated=OPC_TRUE;
		fprintf(tag_log,"**%d. label reading data has sent to the reader packet (read_reply_tag_packet_format)**\n",EDAddress);
		ack_mod=OPC_FALSE;
		}
	
FOUT;
}

static void  TAG_log_file_init() {
Objid selfID;
Objid fatherID;
char folder_path[120];
char log_name[132];
char name [20];
//"Coordinator.NIntel_process";
//strncpy(log_name, "Coordinator.NIntel_process", 132);
FIN (TAG_log_file_init ());
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
sprintf (log_name, "%s%s%s.ak", folder_path, name,"_Tag_process"); 
printf ("Log file name: %s \n\n", log_name);
tag_log = fopen(log_name,"w");
fprintf (tag_log, "|-----------------------------------------------------------------------------|\n");
fprintf (tag_log, "| NODE NAME          : %Tag info \n", name);
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

#if defined (__cplusplus)
extern "C" {
#endif
	void RfidMACtagProcess (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Obtype _op_RfidMACtagProcess_init (int * init_block_ptr);
	void _op_RfidMACtagProcess_diag (OP_SIM_CONTEXT_ARG_OPT);
	void _op_RfidMACtagProcess_terminate (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Address _op_RfidMACtagProcess_alloc (VosT_Obtype, int);
	void _op_RfidMACtagProcess_svar (void *, const char *, void **);


#if defined (__cplusplus)
} /* end of 'extern "C"' */
#endif




/* Process model interrupt handling procedure */


void
RfidMACtagProcess (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (RfidMACtagProcess ());

		{
		/* Temporary Variables */
		
		//Packet*	came_packet;
		
		
		Objid 	tx_id, comp_id, tx_ch_id; 
		int		current_intrpt_type;
		
		/* End of Temporary Variables */


		FSM_ENTER ("RfidMACtagProcess")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (init) enter executives **/
			FSM_STATE_ENTER_UNFORCED (0, "init", state0_enter_exec, "RfidMACtagProcess [init enter execs]")
				FSM_PROFILE_SECTION_IN ("RfidMACtagProcess [init enter execs]", state0_enter_exec)
				{
				TAG_log_file_init();// this only for log print
				
				Qvalue=0;
				Qrange=0;
				//OPC_TRUE  
				/*
				came_select=OPC_FALSE;//OPC_FALSE  
				came_query=OPC_FALSE;
				reply_state=OPC_FALSE;
				reply_mod=OPC_FALSE;
				came_ack=OPC_FALSE;
				came_read_req=OPC_FALSE;
				came_query_rep=OPC_FALSE;
				arbitrate_state=OPC_FALSE;
				rtrn_rcv_pkt=OPC_FALSE;
				read_Complated=OPC_FALSE;
				came_query_adj=OPC_FALSE;
				came_kill=OPC_FALSE;
				*/
				//https://www.binaryhexconverter.com/hex-to-decimal-converter
				header=0x10; //=16
				man_number=0x01F1C07;//=2038791
				object_class=0x0101B3;//=65971
				//serial_number=0x000836FFE;
				//hasta_2_kimlik=1111111111;
				//hasta_2_yas=1990;
				reply_was_send=OPC_FALSE;
				ack_mod=OPC_FALSE;
				newreply_mod=OPC_FALSE;
				 
				
					my_id = op_id_self();
								
					my_node_id = op_topo_parent (my_id);
												
					//op_ima_obj_attr_get (my_node_id, "Ad", &tag_name);
				    //printf("Du ngum Adi: %s**\n",tag_name);
					op_ima_obj_attr_get (my_node_id, "ED Address", &EDAddress);
					op_ima_obj_attr_get (my_node_id, "TagID", &tagID);
					op_ima_obj_attr_get (my_node_id, "Serial Number", &SerialNumber);
					op_ima_obj_attr_get (my_node_id, "PatientID", &patientID);
					op_ima_obj_attr_get (my_node_id, "Age", &age);
					op_ima_obj_attr_get (my_node_id, "Gender", &gender);
					op_ima_obj_attr_get (my_node_id, "Height_cm", &height_cm);
				op_ima_obj_attr_get (my_node_id, "Weight_kg", &weight_kg);
								
				
					/* Transmitter veri hizini al */
					tx_id =  op_topo_assoc (my_id, OPC_TOPO_ASSOC_OUT, OPC_OBJTYPE_RATX, 0);
					comp_id = op_topo_child (tx_id, OPC_OBJTYPE_COMP, 0);
					tx_ch_id = op_topo_child (comp_id, OPC_OBJTYPE_RATXCH, 0);
					//op_ima_obj_attr_get (tx_ch_id, "data rate", &tx_data_rate);
				}
				FSM_PROFILE_SECTION_OUT (state0_enter_exec)

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (1,"RfidMACtagProcess")


			/** state (init) exit executives **/
			FSM_STATE_EXIT_UNFORCED (0, "init", "RfidMACtagProcess [init exit execs]")
				FSM_PROFILE_SECTION_IN ("RfidMACtagProcess [init exit execs]", state0_exit_exec)
				{
				char	pk_format[50];
				int 	Target;
				char SL;
				if((came_packet = op_pk_get (RX_IN_STRM))!=OPC_NIL)
				{	
					op_pk_format(came_packet,pk_format);	
					if (strcmp("select_packet_format",pk_format)==0)
						{
						came_select=1;
						op_pk_nfd_get (came_packet, "Target", &Target);// select packet
						if(Target==1)
						SL=1;
						fprintf(tag_log,"at (init): The Tag receives select Packet with a Target=%d   **\n",EDAddress);
						}
					
				}
				}
				FSM_PROFILE_SECTION_OUT (state0_exit_exec)


			/** state (init) transition processing **/
			FSM_PROFILE_SECTION_IN ("RfidMACtagProcess [init trans conditions]", state0_trans_conds)
			FSM_INIT_COND (SELECT)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("init")
			FSM_PROFILE_SECTION_OUT (state0_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 1, state1_enter_exec, ;, "SELECT", "", "init", "Ready", "tr_39", "RfidMACtagProcess [init -> Ready : SELECT / ]")
				FSM_CASE_TRANSIT (1, 0, state0_enter_exec, ;, "default", "", "init", "init", "tr_40", "RfidMACtagProcess [init -> init : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (Ready) enter executives **/
			FSM_STATE_ENTER_UNFORCED (1, "Ready", state1_enter_exec, "RfidMACtagProcess [Ready enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (3,"RfidMACtagProcess")


			/** state (Ready) exit executives **/
			FSM_STATE_EXIT_UNFORCED (1, "Ready", "RfidMACtagProcess [Ready exit execs]")
				FSM_PROFILE_SECTION_IN ("RfidMACtagProcess [Ready exit execs]", state1_exit_exec)
				{
				current_intrpt_type = op_intrpt_type ();
				}
				FSM_PROFILE_SECTION_OUT (state1_exit_exec)


			/** state (Ready) transition processing **/
			FSM_PROFILE_SECTION_IN ("RfidMACtagProcess [Ready trans conditions]", state1_trans_conds)
			FSM_INIT_COND (ReceivePktRX)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("Ready")
			FSM_PROFILE_SECTION_OUT (state1_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "ReceivePktRX", "", "Ready", "Pkt_rcv", "tr_43", "RfidMACtagProcess [Ready -> Pkt_rcv : ReceivePktRX / ]")
				FSM_CASE_TRANSIT (1, 1, state1_enter_exec, ;, "default", "", "Ready", "Ready", "tr_41", "RfidMACtagProcess [Ready -> Ready : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (Pkt_rcv) enter executives **/
			FSM_STATE_ENTER_FORCED (2, "Pkt_rcv", state2_enter_exec, "RfidMACtagProcess [Pkt_rcv enter execs]")
				FSM_PROFILE_SECTION_IN ("RfidMACtagProcess [Pkt_rcv enter execs]", state2_enter_exec)
				{
				Receive_packet ();
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** state (Pkt_rcv) exit executives **/
			FSM_STATE_EXIT_FORCED (2, "Pkt_rcv", "RfidMACtagProcess [Pkt_rcv exit execs]")
				FSM_PROFILE_SECTION_IN ("RfidMACtagProcess [Pkt_rcv exit execs]", state2_exit_exec)
				{
				rtrn_rcv_pkt=OPC_FALSE;
				}
				FSM_PROFILE_SECTION_OUT (state2_exit_exec)


			/** state (Pkt_rcv) transition processing **/
			FSM_PROFILE_SECTION_IN ("RfidMACtagProcess [Pkt_rcv trans conditions]", state2_trans_conds)
			FSM_INIT_COND (QUERY)
			FSM_TEST_COND (ACK)
			FSM_TEST_COND (NEW_SESSION)
			FSM_TEST_COND (NEWRPLY)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("Pkt_rcv")
			FSM_PROFILE_SECTION_OUT (state2_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 5, state5_enter_exec, ;, "QUERY", "", "Pkt_rcv", "Query", "tr_58", "RfidMACtagProcess [Pkt_rcv -> Query : QUERY / ]")
				FSM_CASE_TRANSIT (1, 6, state6_enter_exec, ;, "ACK", "", "Pkt_rcv", "ACK", "tr_63", "RfidMACtagProcess [Pkt_rcv -> ACK : ACK / ]")
				FSM_CASE_TRANSIT (2, 0, state0_enter_exec, ;, "NEW_SESSION", "", "Pkt_rcv", "init", "tr_66", "RfidMACtagProcess [Pkt_rcv -> init : NEW_SESSION / ]")
				FSM_CASE_TRANSIT (3, 3, state3_enter_exec, ;, "NEWRPLY", "", "Pkt_rcv", "Reply", "tr_14", "RfidMACtagProcess [Pkt_rcv -> Reply : NEWRPLY / ]")
				FSM_CASE_TRANSIT (4, 1, state1_enter_exec, ;, "default", "", "Pkt_rcv", "Ready", "tr_57", "RfidMACtagProcess [Pkt_rcv -> Ready : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (Reply) enter executives **/
			FSM_STATE_ENTER_UNFORCED (3, "Reply", state3_enter_exec, "RfidMACtagProcess [Reply enter execs]")
				FSM_PROFILE_SECTION_IN ("RfidMACtagProcess [Reply enter execs]", state3_enter_exec)
				{
				ReplyTag();
				}
				FSM_PROFILE_SECTION_OUT (state3_enter_exec)

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (7,"RfidMACtagProcess")


			/** state (Reply) exit executives **/
			FSM_STATE_EXIT_UNFORCED (3, "Reply", "RfidMACtagProcess [Reply exit execs]")
				FSM_PROFILE_SECTION_IN ("RfidMACtagProcess [Reply exit execs]", state3_exit_exec)
				{
				//rtrn_rcv_pkt=OPC_TRUE;
				current_intrpt_type = op_intrpt_type ();
				}
				FSM_PROFILE_SECTION_OUT (state3_exit_exec)


			/** state (Reply) transition processing **/
			FSM_TRANSIT_ONLY ((ReceivePktRX), 2, state2_enter_exec, ;, Reply, "ReceivePktRX", "", "Reply", "Pkt_rcv", "tr_49", "RfidMACtagProcess [Reply -> Pkt_rcv : ReceivePktRX / ]")
				/*---------------------------------------------------------*/



			/** state (arbitrate) enter executives **/
			FSM_STATE_ENTER_UNFORCED (4, "arbitrate", state4_enter_exec, "RfidMACtagProcess [arbitrate enter execs]")
				FSM_PROFILE_SECTION_IN ("RfidMACtagProcess [arbitrate enter execs]", state4_enter_exec)
				{
				arbitrate_state=OPC_FALSE;
				}
				FSM_PROFILE_SECTION_OUT (state4_enter_exec)

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (9,"RfidMACtagProcess")


			/** state (arbitrate) exit executives **/
			FSM_STATE_EXIT_UNFORCED (4, "arbitrate", "RfidMACtagProcess [arbitrate exit execs]")
				FSM_PROFILE_SECTION_IN ("RfidMACtagProcess [arbitrate exit execs]", state4_exit_exec)
				{
				current_intrpt_type = op_intrpt_type ();
				}
				FSM_PROFILE_SECTION_OUT (state4_exit_exec)


			/** state (arbitrate) transition processing **/
			FSM_PROFILE_SECTION_IN ("RfidMACtagProcess [arbitrate trans conditions]", state4_trans_conds)
			FSM_INIT_COND (ReceivePktRX)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("arbitrate")
			FSM_PROFILE_SECTION_OUT (state4_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "ReceivePktRX", "", "arbitrate", "Pkt_rcv", "tr_61", "RfidMACtagProcess [arbitrate -> Pkt_rcv : ReceivePktRX / ]")
				FSM_CASE_TRANSIT (1, 4, state4_enter_exec, ;, "default", "", "arbitrate", "arbitrate", "tr_65", "RfidMACtagProcess [arbitrate -> arbitrate : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (Query) enter executives **/
			FSM_STATE_ENTER_FORCED (5, "Query", state5_enter_exec, "RfidMACtagProcess [Query enter execs]")
				FSM_PROFILE_SECTION_IN ("RfidMACtagProcess [Query enter execs]", state5_enter_exec)
				{
				QueryTag();
				}
				FSM_PROFILE_SECTION_OUT (state5_enter_exec)

			/** state (Query) exit executives **/
			FSM_STATE_EXIT_FORCED (5, "Query", "RfidMACtagProcess [Query exit execs]")
				FSM_PROFILE_SECTION_IN ("RfidMACtagProcess [Query exit execs]", state5_exit_exec)
				{
				if(random_Qvalue==0)
					reply_state=OPC_TRUE;
				else 
					arbitrate_state=OPC_TRUE;
				
				}
				FSM_PROFILE_SECTION_OUT (state5_exit_exec)


			/** state (Query) transition processing **/
			FSM_PROFILE_SECTION_IN ("RfidMACtagProcess [Query trans conditions]", state5_trans_conds)
			FSM_INIT_COND (ARBITRATE)
			FSM_TEST_COND (REPLY)
			FSM_TEST_LOGIC ("Query")
			FSM_PROFILE_SECTION_OUT (state5_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 4, state4_enter_exec, ;, "ARBITRATE", "", "Query", "arbitrate", "tr_60", "RfidMACtagProcess [Query -> arbitrate : ARBITRATE / ]")
				FSM_CASE_TRANSIT (1, 3, state3_enter_exec, ;, "REPLY", "", "Query", "Reply", "tr_62", "RfidMACtagProcess [Query -> Reply : REPLY / ]")
				}
				/*---------------------------------------------------------*/



			/** state (ACK) enter executives **/
			FSM_STATE_ENTER_FORCED (6, "ACK", state6_enter_exec, "RfidMACtagProcess [ACK enter execs]")
				FSM_PROFILE_SECTION_IN ("RfidMACtagProcess [ACK enter execs]", state6_enter_exec)
				{
				AckTag();
				}
				FSM_PROFILE_SECTION_OUT (state6_enter_exec)

			/** state (ACK) exit executives **/
			FSM_STATE_EXIT_FORCED (6, "ACK", "RfidMACtagProcess [ACK exit execs]")


			/** state (ACK) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "ACK", "Ready", "tr_64", "RfidMACtagProcess [ACK -> Ready : default / ]")
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (0,"RfidMACtagProcess")
		}
	}




void
_op_RfidMACtagProcess_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}




void
_op_RfidMACtagProcess_terminate (OP_SIM_CONTEXT_ARG_OPT)
	{

	FIN_MT (_op_RfidMACtagProcess_terminate ())


	/* No Termination Block */

	Vos_Poolmem_Dealloc (pr_state_ptr);

	FOUT
	}


/* Undefine shortcuts to state variables to avoid */
/* syntax error in direct access to fields of */
/* local variable prs_ptr in _op_RfidMACtagProcess_svar function. */
#undef my_id
#undef my_node_id
#undef tx_data_rate
#undef slot_genisligi
#undef terminalAddress
#undef destinationAddress
#undef timeOutEndEvh
#undef timerCancelStat
#undef came_select
#undef came_query
#undef reply_state
#undef arbitrate_state
#undef rtrn_rcv_pkt
#undef header
#undef man_number
#undef object_class
#undef serial_number
#undef came_ack
#undef came_query_rep
#undef random_Qvalue
#undef came_read_req
#undef read_Complated
#undef came_query_adj
#undef Qvalue
#undef Qrange
#undef came_kill
#undef temp_Qvalue
#undef reply_mod
#undef hasta_2_kimlik
#undef hasta_2_yas
#undef came_packet
#undef reply_send
#undef reply_was_send
#undef random_RN16
#undef ack_mod
#undef EDAddress
#undef SerialNumber
#undef patientID
#undef tagID
#undef newRN16evh
#undef newreply_mod
#undef tag_log
#undef age
#undef gender
#undef height_cm
#undef weight_kg

#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

VosT_Obtype
_op_RfidMACtagProcess_init (int * init_block_ptr)
	{
	VosT_Obtype obtype = OPC_NIL;
	FIN_MT (_op_RfidMACtagProcess_init (init_block_ptr))

	obtype = Vos_Define_Object_Prstate ("proc state vars (RfidMACtagProcess)",
		sizeof (RfidMACtagProcess_state));
	*init_block_ptr = 0;

	FRET (obtype)
	}

VosT_Address
_op_RfidMACtagProcess_alloc (VosT_Obtype obtype, int init_block)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	RfidMACtagProcess_state * ptr;
	FIN_MT (_op_RfidMACtagProcess_alloc (obtype))

	ptr = (RfidMACtagProcess_state *)Vos_Alloc_Object (obtype);
	if (ptr != OPC_NIL)
		{
		ptr->_op_current_block = init_block;
#if defined (OPD_ALLOW_ODB)
		ptr->_op_current_state = "RfidMACtagProcess [init enter execs]";
#endif
		}
	FRET ((VosT_Address)ptr)
	}



void
_op_RfidMACtagProcess_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	RfidMACtagProcess_state		*prs_ptr;

	FIN_MT (_op_RfidMACtagProcess_svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (RfidMACtagProcess_state *)gen_ptr;

	if (strcmp ("my_id" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->my_id);
		FOUT
		}
	if (strcmp ("my_node_id" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->my_node_id);
		FOUT
		}
	if (strcmp ("tx_data_rate" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->tx_data_rate);
		FOUT
		}
	if (strcmp ("slot_genisligi" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->slot_genisligi);
		FOUT
		}
	if (strcmp ("terminalAddress" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->terminalAddress);
		FOUT
		}
	if (strcmp ("destinationAddress" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->destinationAddress);
		FOUT
		}
	if (strcmp ("timeOutEndEvh" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->timeOutEndEvh);
		FOUT
		}
	if (strcmp ("timerCancelStat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->timerCancelStat);
		FOUT
		}
	if (strcmp ("came_select" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->came_select);
		FOUT
		}
	if (strcmp ("came_query" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->came_query);
		FOUT
		}
	if (strcmp ("reply_state" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->reply_state);
		FOUT
		}
	if (strcmp ("arbitrate_state" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->arbitrate_state);
		FOUT
		}
	if (strcmp ("rtrn_rcv_pkt" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->rtrn_rcv_pkt);
		FOUT
		}
	if (strcmp ("header" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->header);
		FOUT
		}
	if (strcmp ("man_number" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->man_number);
		FOUT
		}
	if (strcmp ("object_class" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->object_class);
		FOUT
		}
	if (strcmp ("serial_number" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->serial_number);
		FOUT
		}
	if (strcmp ("came_ack" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->came_ack);
		FOUT
		}
	if (strcmp ("came_query_rep" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->came_query_rep);
		FOUT
		}
	if (strcmp ("random_Qvalue" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->random_Qvalue);
		FOUT
		}
	if (strcmp ("came_read_req" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->came_read_req);
		FOUT
		}
	if (strcmp ("read_Complated" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->read_Complated);
		FOUT
		}
	if (strcmp ("came_query_adj" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->came_query_adj);
		FOUT
		}
	if (strcmp ("Qvalue" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->Qvalue);
		FOUT
		}
	if (strcmp ("Qrange" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->Qrange);
		FOUT
		}
	if (strcmp ("came_kill" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->came_kill);
		FOUT
		}
	if (strcmp ("temp_Qvalue" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->temp_Qvalue);
		FOUT
		}
	if (strcmp ("reply_mod" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->reply_mod);
		FOUT
		}
	if (strcmp ("hasta_2_kimlik" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->hasta_2_kimlik);
		FOUT
		}
	if (strcmp ("hasta_2_yas" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->hasta_2_yas);
		FOUT
		}
	if (strcmp ("came_packet" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->came_packet);
		FOUT
		}
	if (strcmp ("reply_send" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->reply_send);
		FOUT
		}
	if (strcmp ("reply_was_send" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->reply_was_send);
		FOUT
		}
	if (strcmp ("random_RN16" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->random_RN16);
		FOUT
		}
	if (strcmp ("ack_mod" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->ack_mod);
		FOUT
		}
	if (strcmp ("EDAddress" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->EDAddress);
		FOUT
		}
	if (strcmp ("SerialNumber" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->SerialNumber);
		FOUT
		}
	if (strcmp ("patientID" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->patientID);
		FOUT
		}
	if (strcmp ("tagID" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->tagID);
		FOUT
		}
	if (strcmp ("newRN16evh" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->newRN16evh);
		FOUT
		}
	if (strcmp ("newreply_mod" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->newreply_mod);
		FOUT
		}
	if (strcmp ("tag_log" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->tag_log);
		FOUT
		}
	if (strcmp ("age" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->age);
		FOUT
		}
	if (strcmp ("gender" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->gender);
		FOUT
		}
	if (strcmp ("height_cm" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->height_cm);
		FOUT
		}
	if (strcmp ("weight_kg" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->weight_kg);
		FOUT
		}
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

