/* Process model C form file: RFiDReaderProcess.pr.c */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char RFiDReaderProcess_pr_c [] = "MIL_3_Tfile_Hdr_ 145A 30A op_runsim 7 5BBC9DC5 5BBC9DC5 1 HBI-JCGS160 Mohammed@Hussein 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1e80 8                                                                                                                                                                                                                                                                                                                                                                                           ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

#include <math.h>

#define EPSILON    0.000000000001

//RfidReaderMac Connectivity
#define TX_OUT_STRM		(0)
#define RX_IN_STRM		(1)
#define STRM_FROM_READER_TO_ED (2)
#define SINK_OUT_STRM	(1)

///// define the code for op_intrpt_schedule_self() function to make recall
#define Timeout_code     (3)
#define New_session_code (6)

// define the link conditions

#define STOP 				(readTotalTagCount==OPC_TRUE)
#define KILL 				(kill_status==OPC_TRUE) && (current_intrpt_type == OPC_INTRPT_SELF) && ((op_intrpt_code () == New_session_code)) 
#define PktReceive			(current_intrpt_type == OPC_INTRPT_STRM) && (op_intrpt_strm () == RX_IN_STRM) 
#define wAITfORACK 			(packetWasSent==1)
#define qUEUEiSeMPTY 		((op_subq_empty(0)))
#define Timeout 			(current_intrpt_type == OPC_INTRPT_SELF) && ((op_intrpt_code () == Timeout_code)) 
 

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
	int	                    		terminalAddress                                 ;
	int	                    		destinationAddress                              ;
	Evhandle	               		timeOutEndEvh                                   ;
	Stathandle	             		timerCancelStat                                 ;
	int	                    		timerCancelCount                                ;
	OpT_Int64	              		came_serial_number                              ;
	Packet *	               		copy_ack_pkt                                    ;
	int	                    		rn_value                                        ;
	Boolean	                		read_Mod                                        ;
	Boolean	                		read_Complated                                  ;
	int	                    		TagID                                           ;
	int	                    		readComplatedTagCount                           ;
	Boolean	                		readFromTagComplated                            ;
	int	                    		TotalTagCount                                   ;
	int	                    		Query_rep_count                                 ;
	Boolean	                		kill_status                                     ;
	Evhandle	               		New_session                                     ;
	Boolean	                		readTotalTagCount                               ;
	int	                    		patientID                                       ;
	FILE*	                  		Reader_log                                      ;
	Boolean	                		RFID_log                                        ;
	int	                    		age                                             ;
	int	                    		gender                                          ;
	int	                    		height_cm                                       ;
	int	                    		weight_kg                                       ;
	} RFiDReaderProcess_state;

#define pr_state_ptr            		((RFiDReaderProcess_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))
#define my_id                   		pr_state_ptr->my_id
#define my_node_id              		pr_state_ptr->my_node_id
#define terminalAddress         		pr_state_ptr->terminalAddress
#define destinationAddress      		pr_state_ptr->destinationAddress
#define timeOutEndEvh           		pr_state_ptr->timeOutEndEvh
#define timerCancelStat         		pr_state_ptr->timerCancelStat
#define timerCancelCount        		pr_state_ptr->timerCancelCount
#define came_serial_number      		pr_state_ptr->came_serial_number
#define copy_ack_pkt            		pr_state_ptr->copy_ack_pkt
#define rn_value                		pr_state_ptr->rn_value
#define read_Mod                		pr_state_ptr->read_Mod
#define read_Complated          		pr_state_ptr->read_Complated
#define TagID                   		pr_state_ptr->TagID
#define readComplatedTagCount   		pr_state_ptr->readComplatedTagCount
#define readFromTagComplated    		pr_state_ptr->readFromTagComplated
#define TotalTagCount           		pr_state_ptr->TotalTagCount
#define Query_rep_count         		pr_state_ptr->Query_rep_count
#define kill_status             		pr_state_ptr->kill_status
#define New_session             		pr_state_ptr->New_session
#define readTotalTagCount       		pr_state_ptr->readTotalTagCount
#define patientID               		pr_state_ptr->patientID
#define Reader_log              		pr_state_ptr->Reader_log
#define RFID_log                		pr_state_ptr->RFID_log
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
#define FIN_PREAMBLE_DEC	RFiDReaderProcess_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((RFiDReaderProcess_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


/* Function Block */

#if !defined (VOSD_NO_FIN)
enum { _op_block_origin = __LINE__ + 2};
#endif



static void  ReaderRFID_log_file_init() {
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
sprintf (log_name, "%s%s%s.ak", folder_path, name,"_ReaderRFID_process"); 
printf ("Log file name: %s \n\n", log_name);
Reader_log = fopen(log_name,"w");
fprintf (Reader_log, "|-----------------------------------------------------------------------------|\n");
fprintf (Reader_log, "| NODE NAME          : %Reader RFID info (2018)\n", name);
FOUT;
}
// 
static void ReaderSelect()
{
Packet*	select_pkt;
FIN (ReaderSelect());
  if (RFID_log) {
	int selectRFID =1;
	select_pkt= op_pk_create_fmt("select_packet_format");
	op_pk_nfd_set (select_pkt, "Target", selectRFID);
	op_pk_send (select_pkt, TX_OUT_STRM);
	fprintf(Reader_log,"Reader select RFID Tag with No: %d**\n",selectRFID);
	}
FOUT;
}
static void ReaderQuery()
{
Packet*	query_pkt;
FIN (ReaderQuery());
if (RFID_log) {
 int query=3; 
 query_pkt= op_pk_create_fmt("query_packet_format");
   op_pk_nfd_set (query_pkt, "Q",query);
   op_pk_nfd_set (query_pkt, "Sel", 10);
   op_pk_send (query_pkt, TX_OUT_STRM);
   fprintf(Reader_log,"Reader query_packet with query value: %d**\n",query);
 
			
 }
FOUT;
}

static void Query_rep(){
Packet*	query_rep_pkt;
Packet* query_adj_pkt;
FIN (Query_rep());
if (RFID_log) {
// waiting time for the ack 
if (Query_rep_count>5)
	{
	query_adj_pkt= op_pk_create_fmt("queryAdjust_packet_format");
	op_pk_nfd_set (query_adj_pkt, "UpDn",2);
	op_pk_send (query_adj_pkt, TX_OUT_STRM);
	Query_rep_count=0;
	// so much print for this sentence
	//fprintf(Reader_log,"Reader queryAdjust_packet_format Q>5 \n");
	}
else 
	{
	query_rep_pkt= op_pk_create_fmt("queryRep_packet_format");
    //op_pk_nfd_set (query_pkt, "Sel", 10);
    op_pk_send (query_rep_pkt, TX_OUT_STRM);
    Query_rep_count++;
	// so much print for this sentence
	//fprintf(Reader_log,"Reader queryRep_packet_format Q<5 \n");
	}

}

FOUT;
}

static void Reader_kill(){
Packet* kill_pkt;
FIN (Reader_kill());
if (RFID_log) {
kill_pkt= op_pk_create_fmt("kill_packet");
op_pk_send (kill_pkt, TX_OUT_STRM);
kill_status=OPC_TRUE;
New_session=op_intrpt_schedule_self ((op_sim_time()+1),New_session_code);
//New_session=op_intrpt_schedule_self ((op_sim_time()+2),New_session_code);
}
FOUT;
}

// when the packet is received the ACK packet will be create and reply to each valid type of these packets
// type of the packet are: RN16,EPC , XPC,CRC16, and the System Data.

static void Reader_Received_Packet(){

Packet*	packet_came; 	Packet*	ack_pkt;
Packet*	read_req_packet;Packet* receive_epc;
Packet* read_value_pkt; Packet* copy_read_value_pkt;

char	pk_name[50];

FIN (Reader_Received_Packet());

if (RFID_log) {

int packet_format=0; 

if((packet_came = op_pk_get (RX_IN_STRM))!=OPC_NIL)
 {	
 // obtains the name of the packet format
 	op_pk_format(packet_came,pk_name);

	if (strcmp("rfid_RN16_tag_packet_format",pk_name)==0)  	packet_format=1;
	if (strcmp("rfid_epc_send_packet",pk_name)==0) 			packet_format=2;
	if (strcmp("read_reply_tag_packet_format",pk_name)==0) 	packet_format=3;


switch (packet_format) {	
 case 1:{
		op_pk_nfd_get(packet_came, "RN", &rn_value);
		ack_pkt= op_pk_create_fmt("rfid_ack_packet_format");
        op_pk_nfd_set (ack_pkt, "RN16",rn_value);
		fprintf(Reader_log,"after receives RN than Reader sent rfid_ack_packet_format  with RN:%d **\n",rn_value);
		//copy_ack_pkt=op_pk_copy(ack_pkt);
        //op_pk_nfd_set (query_pkt, "Sel", 10);
        op_pk_send (ack_pkt, TX_OUT_STRM);
		}  break;
case 2:	{
		op_pk_nfd_get_pkt(packet_came, "EPC_number", &receive_epc);// take the bayload and get SN value from inside it 
		op_pk_nfd_get (receive_epc, "Serial Number", &came_serial_number);
		fprintf(Reader_log,"Reader receives Seri no: %d**\n",came_serial_number);
		fprintf(Reader_log,"after Reader receives EPC_number than Reader sent ACK  with RN:%d **\n",rn_value);
		read_req_packet= op_pk_create_fmt("read_packet_format");
		op_pk_nfd_set (read_req_packet, "RN",rn_value);
		op_pk_send (read_req_packet, TX_OUT_STRM);
	 
		}  break;
case 3:	{
 

	
		   op_pk_nfd_get(packet_came, "TagID", &TagID);
		   
		   op_pk_nfd_get(packet_came, "PatientID", &patientID);
		   op_pk_nfd_get(packet_came, "Age", &age);
		   op_pk_nfd_get(packet_came, "Gender", &gender);
		   op_pk_nfd_get(packet_came, "Height_cm", &height_cm);
		   op_pk_nfd_get(packet_came, "Weight_kg", &weight_kg);
		   
 //**************************create packet for sending to ED *******************************//
		   // create new packet and insert values  
		   read_value_pkt= op_pk_create_fmt("read_value_packet");
		   op_pk_nfd_set (read_value_pkt, "TagID",TagID);
		   
		   op_pk_nfd_set (read_value_pkt, "PatientID",patientID);
		   op_pk_nfd_set (read_value_pkt, "Age",age);
		   op_pk_nfd_set (read_value_pkt, "Gender",gender);
		   op_pk_nfd_set (read_value_pkt, "Height_cm",height_cm);
		   op_pk_nfd_set (read_value_pkt, "Weight_kg",weight_kg);
		   
		   copy_read_value_pkt=op_pk_copy(read_value_pkt);
		   op_pk_send (read_value_pkt, SINK_OUT_STRM);
		   //sending the rfid info to the end deveice 
		   op_pk_send (copy_read_value_pkt, STRM_FROM_READER_TO_ED);
		   fprintf(Reader_log,"ReaderID=%d.  sending the rfid info to the end deveice  :+-+-\n",patientID);
		   readComplatedTagCount++;
		  // read_Mod=OPC_FALSE;
	printf("readComplatedTagCount=%d TotalTagCount=%d   \n",readComplatedTagCount,readTotalTagCount);	   
		   if(readComplatedTagCount==TotalTagCount)
			   readTotalTagCount=OPC_TRUE;
		   else
			   timeOutEndEvh=op_intrpt_schedule_self ((op_sim_time()+.01),Timeout_code);
		    //timeOutEndEvh=op_intrpt_schedule_self ((op_sim_time()+1),tIMEoUTINTcODE);
		   }  break;
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

#if defined (__cplusplus)
extern "C" {
#endif
	void RFiDReaderProcess (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Obtype _op_RFiDReaderProcess_init (int * init_block_ptr);
	void _op_RFiDReaderProcess_diag (OP_SIM_CONTEXT_ARG_OPT);
	void _op_RFiDReaderProcess_terminate (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Address _op_RFiDReaderProcess_alloc (VosT_Obtype, int);
	void _op_RFiDReaderProcess_svar (void *, const char *, void **);


#if defined (__cplusplus)
} /* end of 'extern "C"' */
#endif




/* Process model interrupt handling procedure */


void
RFiDReaderProcess (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (RFiDReaderProcess ());

		{
		/* Temporary Variables */
		int		current_intrpt_type;
		Objid 	tx_id, comp_id, tx_ch_id;
		/* End of Temporary Variables */


		FSM_ENTER ("RFiDReaderProcess")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (init) enter executives **/
			FSM_STATE_ENTER_FORCED (0, "init", state0_enter_exec, "RFiDReaderProcess [init enter execs]")
				FSM_PROFILE_SECTION_IN ("RFiDReaderProcess [init enter execs]", state0_enter_exec)
				{
					read_Mod=OPC_TRUE;
					readComplatedTagCount=0;
					//TotalTagCount=2;
					Query_rep_count=0;
					kill_status=OPC_FALSE;
					readTotalTagCount=OPC_FALSE;
					/* obtain self object ID of the surrounding processor or queue */
					my_id = op_id_self();
					/* obtain object ID of the parent object (node) */			
					my_node_id = op_topo_parent (my_id);							
					op_ima_obj_attr_get (my_node_id, "MAC Address", &terminalAddress);
					op_ima_obj_attr_get (my_node_id, "MaxNoRfid", &TotalTagCount);
					//op_ima_obj_attr_get (my_node_id, "DestinationAddress", &destinationAddress);
					/* get the RFID activation of the node */
					op_ima_obj_attr_get (my_node_id, "RFID Active", &RFID_log);		
					if (RFID_log) ReaderRFID_log_file_init();	
					/* Transmitter veri hizini al */
					tx_id =  op_topo_assoc (my_id, OPC_TOPO_ASSOC_OUT, OPC_OBJTYPE_RATX, 0);
					comp_id = op_topo_child (tx_id, OPC_OBJTYPE_COMP, 0);
					tx_ch_id = op_topo_child (comp_id, OPC_OBJTYPE_RATXCH, 0);
				//	op_ima_obj_attr_get (tx_ch_id, "data rate", &tx_data_rate);
				}
				FSM_PROFILE_SECTION_OUT (state0_enter_exec)

			/** state (init) exit executives **/
			FSM_STATE_EXIT_FORCED (0, "init", "RFiDReaderProcess [init exit execs]")


			/** state (init) transition processing **/
			FSM_TRANSIT_FORCE (2, state2_enter_exec, ;, "default", "", "init", "Select", "tr_41", "RFiDReaderProcess [init -> Select : default / ]")
				/*---------------------------------------------------------*/



			/** state (Query) enter executives **/
			FSM_STATE_ENTER_FORCED (1, "Query", state1_enter_exec, "RFiDReaderProcess [Query enter execs]")
				FSM_PROFILE_SECTION_IN ("RFiDReaderProcess [Query enter execs]", state1_enter_exec)
				{
				ReaderQuery();
				}
				FSM_PROFILE_SECTION_OUT (state1_enter_exec)

			/** state (Query) exit executives **/
			FSM_STATE_EXIT_FORCED (1, "Query", "RFiDReaderProcess [Query exit execs]")
				FSM_PROFILE_SECTION_IN ("RFiDReaderProcess [Query exit execs]", state1_exit_exec)
				{
					current_intrpt_type = op_intrpt_type ();
				}
				FSM_PROFILE_SECTION_OUT (state1_exit_exec)


			/** state (Query) transition processing **/
			FSM_TRANSIT_FORCE (3, state3_enter_exec, ;, "default", "", "Query", "wait", "tr_46", "RFiDReaderProcess [Query -> wait : default / ]")
				/*---------------------------------------------------------*/



			/** state (Select) enter executives **/
			FSM_STATE_ENTER_FORCED (2, "Select", state2_enter_exec, "RFiDReaderProcess [Select enter execs]")
				FSM_PROFILE_SECTION_IN ("RFiDReaderProcess [Select enter execs]", state2_enter_exec)
				{
				ReaderSelect();
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** state (Select) exit executives **/
			FSM_STATE_EXIT_FORCED (2, "Select", "RFiDReaderProcess [Select exit execs]")


			/** state (Select) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "Select", "Query", "tr_44", "RFiDReaderProcess [Select -> Query : default / ]")
				/*---------------------------------------------------------*/



			/** state (wait) enter executives **/
			FSM_STATE_ENTER_UNFORCED (3, "wait", state3_enter_exec, "RFiDReaderProcess [wait enter execs]")
				FSM_PROFILE_SECTION_IN ("RFiDReaderProcess [wait enter execs]", state3_enter_exec)
				{
				 timeOutEndEvh=op_intrpt_schedule_self ((op_sim_time()+0.1),Timeout_code);
				}
				FSM_PROFILE_SECTION_OUT (state3_enter_exec)

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (7,"RFiDReaderProcess")


			/** state (wait) exit executives **/
			FSM_STATE_EXIT_UNFORCED (3, "wait", "RFiDReaderProcess [wait exit execs]")
				FSM_PROFILE_SECTION_IN ("RFiDReaderProcess [wait exit execs]", state3_exit_exec)
				{
				current_intrpt_type = op_intrpt_type ();
				}
				FSM_PROFILE_SECTION_OUT (state3_exit_exec)


			/** state (wait) transition processing **/
			FSM_PROFILE_SECTION_IN ("RFiDReaderProcess [wait trans conditions]", state3_trans_conds)
			FSM_INIT_COND (PktReceive)
			FSM_TEST_COND (Timeout)
			FSM_TEST_LOGIC ("wait")
			FSM_PROFILE_SECTION_OUT (state3_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 4, state4_enter_exec, ;, "PktReceive", "", "wait", "Rcv_pkt", "tr_53", "RFiDReaderProcess [wait -> Rcv_pkt : PktReceive / ]")
				FSM_CASE_TRANSIT (1, 5, state5_enter_exec, ;, "Timeout", "", "wait", "Query_Rep", "tr_56", "RFiDReaderProcess [wait -> Query_Rep : Timeout / ]")
				}
				/*---------------------------------------------------------*/



			/** state (Rcv_pkt) enter executives **/
			FSM_STATE_ENTER_FORCED (4, "Rcv_pkt", state4_enter_exec, "RFiDReaderProcess [Rcv_pkt enter execs]")
				FSM_PROFILE_SECTION_IN ("RFiDReaderProcess [Rcv_pkt enter execs]", state4_enter_exec)
				{
				Reader_Received_Packet();
				}
				FSM_PROFILE_SECTION_OUT (state4_enter_exec)

			/** state (Rcv_pkt) exit executives **/
			FSM_STATE_EXIT_FORCED (4, "Rcv_pkt", "RFiDReaderProcess [Rcv_pkt exit execs]")
				FSM_PROFILE_SECTION_IN ("RFiDReaderProcess [Rcv_pkt exit execs]", state4_exit_exec)
				{
				//current_intrpt_type = op_intrpt_type ();
				}
				FSM_PROFILE_SECTION_OUT (state4_exit_exec)


			/** state (Rcv_pkt) transition processing **/
			FSM_PROFILE_SECTION_IN ("RFiDReaderProcess [Rcv_pkt trans conditions]", state4_trans_conds)
			FSM_INIT_COND (STOP)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("Rcv_pkt")
			FSM_PROFILE_SECTION_OUT (state4_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 6, state6_enter_exec, ;, "STOP", "", "Rcv_pkt", "Stop", "tr_58", "RFiDReaderProcess [Rcv_pkt -> Stop : STOP / ]")
				FSM_CASE_TRANSIT (1, 3, state3_enter_exec, ;, "default", "", "Rcv_pkt", "wait", "tr_54", "RFiDReaderProcess [Rcv_pkt -> wait : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (Query_Rep) enter executives **/
			FSM_STATE_ENTER_FORCED (5, "Query_Rep", state5_enter_exec, "RFiDReaderProcess [Query_Rep enter execs]")
				FSM_PROFILE_SECTION_IN ("RFiDReaderProcess [Query_Rep enter execs]", state5_enter_exec)
				{
				Query_rep(); 
				}
				FSM_PROFILE_SECTION_OUT (state5_enter_exec)

			/** state (Query_Rep) exit executives **/
			FSM_STATE_EXIT_FORCED (5, "Query_Rep", "RFiDReaderProcess [Query_Rep exit execs]")


			/** state (Query_Rep) transition processing **/
			FSM_TRANSIT_FORCE (3, state3_enter_exec, ;, "default", "", "Query_Rep", "wait", "tr_57", "RFiDReaderProcess [Query_Rep -> wait : default / ]")
				/*---------------------------------------------------------*/



			/** state (Stop) enter executives **/
			FSM_STATE_ENTER_UNFORCED (6, "Stop", state6_enter_exec, "RFiDReaderProcess [Stop enter execs]")
				FSM_PROFILE_SECTION_IN ("RFiDReaderProcess [Stop enter execs]", state6_enter_exec)
				{
				Reader_kill();
				}
				FSM_PROFILE_SECTION_OUT (state6_enter_exec)

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (13,"RFiDReaderProcess")


			/** state (Stop) exit executives **/
			FSM_STATE_EXIT_UNFORCED (6, "Stop", "RFiDReaderProcess [Stop exit execs]")
				FSM_PROFILE_SECTION_IN ("RFiDReaderProcess [Stop exit execs]", state6_exit_exec)
				{
				current_intrpt_type = op_intrpt_type ();
				}
				FSM_PROFILE_SECTION_OUT (state6_exit_exec)


			/** state (Stop) transition processing **/
			FSM_TRANSIT_ONLY ((KILL), 0, state0_enter_exec, ;, Stop, "KILL", "", "Stop", "init", "tr_63", "RFiDReaderProcess [Stop -> init : KILL / ]")
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (0,"RFiDReaderProcess")
		}
	}




void
_op_RFiDReaderProcess_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}




void
_op_RFiDReaderProcess_terminate (OP_SIM_CONTEXT_ARG_OPT)
	{

	FIN_MT (_op_RFiDReaderProcess_terminate ())


	/* No Termination Block */

	Vos_Poolmem_Dealloc (pr_state_ptr);

	FOUT
	}


/* Undefine shortcuts to state variables to avoid */
/* syntax error in direct access to fields of */
/* local variable prs_ptr in _op_RFiDReaderProcess_svar function. */
#undef my_id
#undef my_node_id
#undef terminalAddress
#undef destinationAddress
#undef timeOutEndEvh
#undef timerCancelStat
#undef timerCancelCount
#undef came_serial_number
#undef copy_ack_pkt
#undef rn_value
#undef read_Mod
#undef read_Complated
#undef TagID
#undef readComplatedTagCount
#undef readFromTagComplated
#undef TotalTagCount
#undef Query_rep_count
#undef kill_status
#undef New_session
#undef readTotalTagCount
#undef patientID
#undef Reader_log
#undef RFID_log
#undef age
#undef gender
#undef height_cm
#undef weight_kg

#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

VosT_Obtype
_op_RFiDReaderProcess_init (int * init_block_ptr)
	{
	VosT_Obtype obtype = OPC_NIL;
	FIN_MT (_op_RFiDReaderProcess_init (init_block_ptr))

	obtype = Vos_Define_Object_Prstate ("proc state vars (RFiDReaderProcess)",
		sizeof (RFiDReaderProcess_state));
	*init_block_ptr = 0;

	FRET (obtype)
	}

VosT_Address
_op_RFiDReaderProcess_alloc (VosT_Obtype obtype, int init_block)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	RFiDReaderProcess_state * ptr;
	FIN_MT (_op_RFiDReaderProcess_alloc (obtype))

	ptr = (RFiDReaderProcess_state *)Vos_Alloc_Object (obtype);
	if (ptr != OPC_NIL)
		{
		ptr->_op_current_block = init_block;
#if defined (OPD_ALLOW_ODB)
		ptr->_op_current_state = "RFiDReaderProcess [init enter execs]";
#endif
		}
	FRET ((VosT_Address)ptr)
	}



void
_op_RFiDReaderProcess_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	RFiDReaderProcess_state		*prs_ptr;

	FIN_MT (_op_RFiDReaderProcess_svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (RFiDReaderProcess_state *)gen_ptr;

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
	if (strcmp ("timerCancelCount" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->timerCancelCount);
		FOUT
		}
	if (strcmp ("came_serial_number" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->came_serial_number);
		FOUT
		}
	if (strcmp ("copy_ack_pkt" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->copy_ack_pkt);
		FOUT
		}
	if (strcmp ("rn_value" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->rn_value);
		FOUT
		}
	if (strcmp ("read_Mod" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->read_Mod);
		FOUT
		}
	if (strcmp ("read_Complated" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->read_Complated);
		FOUT
		}
	if (strcmp ("TagID" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->TagID);
		FOUT
		}
	if (strcmp ("readComplatedTagCount" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->readComplatedTagCount);
		FOUT
		}
	if (strcmp ("readFromTagComplated" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->readFromTagComplated);
		FOUT
		}
	if (strcmp ("TotalTagCount" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->TotalTagCount);
		FOUT
		}
	if (strcmp ("Query_rep_count" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->Query_rep_count);
		FOUT
		}
	if (strcmp ("kill_status" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->kill_status);
		FOUT
		}
	if (strcmp ("New_session" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->New_session);
		FOUT
		}
	if (strcmp ("readTotalTagCount" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->readTotalTagCount);
		FOUT
		}
	if (strcmp ("patientID" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->patientID);
		FOUT
		}
	if (strcmp ("Reader_log" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->Reader_log);
		FOUT
		}
	if (strcmp ("RFID_log" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->RFID_log);
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

