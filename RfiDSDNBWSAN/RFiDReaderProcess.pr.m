MIL_3_Tfile_Hdr_ 145A 140A modeler 9 5A2E6EB1 5BBBA337 46 HBI-JCGS160 Mohammed@Hussein 0 0 none none 0 0 none 119FE8A7 3544 0 0 0 0 0 0 1e80 8                                                                                                                                                                                                                                                                                                                                                                                  ��g�      @   D   H      #�  3   3$  3(  3,  38  3<  3@  #�           	   begsim intrpt             ����      doc file            	nd_module      endsim intrpt             ����      failure intrpts            disabled      intrpt interval         ԲI�%��}����      priority              ����      recovery intrpts            disabled      subqueue                     count    ���   
   ����   
      list   	���   
          
      super priority             ����          6   Objid	\my_id;       Objid	\my_node_id;       int	\terminalAddress;       int	\destinationAddress;       Evhandle	\timeOutEndEvh;       Stathandle	\timerCancelStat;       int	\timerCancelCount;       OpT_Int64	\came_serial_number;       Packet *	\copy_ack_pkt;       int	\rn_value;       Boolean	\read_Mod;       Boolean	\read_Complated;       int	\TagID;       int	\readComplatedTagCount;       Boolean	\readFromTagComplated;       int	\TotalTagCount;       int	\Query_rep_count;       Boolean	\kill_status;       Evhandle	\New_session;       Boolean	\readTotalTagCount;       int	\patientID;       FILE*	\Reader_log;       Boolean	\RFID_log;       	int	\age;       int	\gender;       int	\height_cm;       int	\weight_kg;          int		current_intrpt_type;    Objid 	tx_id, comp_id, tx_ch_id;      #include <math.h>       !#define EPSILON    0.000000000001       //RfidReaderMac Connectivity   #define TX_OUT_STRM		(0)   #define RX_IN_STRM		(1)   "#define STRM_FROM_READER_TO_ED (2)   #define SINK_OUT_STRM	(1)       K///// define the code for op_intrpt_schedule_self() function to make recall   #define Timeout_code     (3)   #define New_session_code (6)       // define the link conditions       .#define STOP 				(readTotalTagCount==OPC_TRUE)   �#define KILL 				(kill_status==OPC_TRUE) && (current_intrpt_type == OPC_INTRPT_SELF) && ((op_intrpt_code () == New_session_code))    c#define PktReceive			(current_intrpt_type == OPC_INTRPT_STRM) && (op_intrpt_strm () == RX_IN_STRM)    (#define wAITfORACK 			(packetWasSent==1)   +#define qUEUEiSeMPTY 		((op_subq_empty(0)))   e#define Timeout 			(current_intrpt_type == OPC_INTRPT_SELF) && ((op_intrpt_code () == Timeout_code))        �           )static void  ReaderRFID_log_file_init() {   Objid selfID;   Objid fatherID;   char folder_path[120];   char log_name[132];   char name [20];   //"Coordinator.NIntel_process";   7//strncpy(log_name, "Coordinator.NIntel_process", 132);   FIN (TAG_log_file_init ());   selfID = op_id_self();   "fatherID = op_topo_parent(selfID);   Bop_ima_obj_attr_get (fatherID, "Log File Directory", folder_path);   .op_ima_obj_attr_get (fatherID, "name", &name);       Fif (prg_path_name_is_dir (folder_path) == PrgC_Path_Name_Is_Not_Dir) {   		char msg[128];		   g		sprintf (msg, " \"%s\" is not valid directory name. The output will not be logged.\n", folder_path);    '		/* Display an appropriate warning.	*/   F		op_prg_odb_print_major ("Warning from Tag process: ", msg, OPC_NIL);   	}	   Jsprintf (log_name, "%s%s%s.ak", folder_path, name,"_ReaderRFID_process");    ,printf ("Log file name: %s \n\n", log_name);   !Reader_log = fopen(log_name,"w");   jfprintf (Reader_log, "|-----------------------------------------------------------------------------|\n");   Pfprintf (Reader_log, "| NODE NAME          : %Reader RFID info (2018)\n", name);   FOUT;   }   //    static void ReaderSelect()   {   Packet*	select_pkt;   FIN (ReaderSelect());     if (RFID_log) {   	int selectRFID =1;   6	select_pkt= op_pk_create_fmt("select_packet_format");   2	op_pk_nfd_set (select_pkt, "Target", selectRFID);   &	op_pk_send (select_pkt, TX_OUT_STRM);   I	fprintf(Reader_log,"Reader select RFID Tag with No: %d**\n",selectRFID);   	}   FOUT;   }   static void ReaderQuery()   {   Packet*	query_pkt;   FIN (ReaderQuery());   if (RFID_log) {    int query=3;    4 query_pkt= op_pk_create_fmt("query_packet_format");   (   op_pk_nfd_set (query_pkt, "Q",query);   (   op_pk_nfd_set (query_pkt, "Sel", 10);   '   op_pk_send (query_pkt, TX_OUT_STRM);   L   fprintf(Reader_log,"Reader query_packet with query value: %d**\n",query);       			    }   FOUT;   }       static void Query_rep(){   Packet*	query_rep_pkt;   Packet* query_adj_pkt;   FIN (Query_rep());   if (RFID_log) {   // waiting time for the ack    if (Query_rep_count>5)   	{   >	query_adj_pkt= op_pk_create_fmt("queryAdjust_packet_format");   )	op_pk_nfd_set (query_adj_pkt, "UpDn",2);   )	op_pk_send (query_adj_pkt, TX_OUT_STRM);   	Query_rep_count=0;   #	// so much print for this sentence   A	//fprintf(Reader_log,"Reader queryAdjust_packet_format Q>5 \n");   	}   else    	{   ;	query_rep_pkt= op_pk_create_fmt("queryRep_packet_format");   +    //op_pk_nfd_set (query_pkt, "Sel", 10);   ,    op_pk_send (query_rep_pkt, TX_OUT_STRM);       Query_rep_count++;   #	// so much print for this sentence   >	//fprintf(Reader_log,"Reader queryRep_packet_format Q<5 \n");   	}       }       FOUT;   }       static void Reader_kill(){   Packet* kill_pkt;   FIN (Reader_kill());   if (RFID_log) {   *kill_pkt= op_pk_create_fmt("kill_packet");   #op_pk_send (kill_pkt, TX_OUT_STRM);   kill_status=OPC_TRUE;   INew_session=op_intrpt_schedule_self ((op_sim_time()+1),New_session_code);   K//New_session=op_intrpt_schedule_self ((op_sim_time()+2),New_session_code);   }   FOUT;   }       j// when the packet is received the ACK packet will be create and reply to each valid type of these packets   E// type of the packet are: RN16,EPC , XPC,CRC16, and the System Data.       %static void Reader_Received_Packet(){       &Packet*	packet_came; 	Packet*	ack_pkt;   ,Packet*	read_req_packet;Packet* receive_epc;   4Packet* read_value_pkt; Packet* copy_read_value_pkt;       char	pk_name[50];       FIN (Reader_Received_Packet());       if (RFID_log) {       int packet_format=0;        3if((packet_came = op_pk_get (RX_IN_STRM))!=OPC_NIL)    {	   ) // obtains the name of the packet format   $ 	op_pk_format(packet_came,pk_name);       I	if (strcmp("rfid_RN16_tag_packet_format",pk_name)==0)  	packet_format=1;   C	if (strcmp("rfid_epc_send_packet",pk_name)==0) 			packet_format=2;   I	if (strcmp("read_reply_tag_packet_format",pk_name)==0) 	packet_format=3;           switch (packet_format) {	   	 case 1:{   .		op_pk_nfd_get(packet_came, "RN", &rn_value);   6		ack_pkt= op_pk_create_fmt("rfid_ack_packet_format");   1        op_pk_nfd_set (ack_pkt, "RN16",rn_value);   l		fprintf(Reader_log,"after receives RN than Reader sent rfid_ack_packet_format  with RN:%d **\n",rn_value);   %		//copy_ack_pkt=op_pk_copy(ack_pkt);   /        //op_pk_nfd_set (query_pkt, "Sel", 10);   *        op_pk_send (ack_pkt, TX_OUT_STRM);   		}  break;   	case 2:	{   q		op_pk_nfd_get_pkt(packet_came, "EPC_number", &receive_epc);// take the bayload and get SN value from inside it    D		op_pk_nfd_get (receive_epc, "Serial Number", &came_serial_number);   K		fprintf(Reader_log,"Reader receives Seri no: %d**\n",came_serial_number);   h		fprintf(Reader_log,"after Reader receives EPC_number than Reader sent ACK  with RN:%d **\n",rn_value);   :		read_req_packet= op_pk_create_fmt("read_packet_format");   1		op_pk_nfd_set (read_req_packet, "RN",rn_value);   ,		op_pk_send (read_req_packet, TX_OUT_STRM);   	    		}  break;   	case 3:	{           	   1		   op_pk_nfd_get(packet_came, "TagID", &TagID);   		      9		   op_pk_nfd_get(packet_came, "PatientID", &patientID);   -		   op_pk_nfd_get(packet_came, "Age", &age);   3		   op_pk_nfd_get(packet_came, "Gender", &gender);   9		   op_pk_nfd_get(packet_came, "Height_cm", &height_cm);   9		   op_pk_nfd_get(packet_came, "Weight_kg", &weight_kg);   		      ^ //**************************create packet for sending to ED *******************************//   -		   // create new packet and insert values     ;		   read_value_pkt= op_pk_create_fmt("read_value_packet");   3		   op_pk_nfd_set (read_value_pkt, "TagID",TagID);   		      ;		   op_pk_nfd_set (read_value_pkt, "PatientID",patientID);   /		   op_pk_nfd_set (read_value_pkt, "Age",age);   5		   op_pk_nfd_set (read_value_pkt, "Gender",gender);   ;		   op_pk_nfd_set (read_value_pkt, "Height_cm",height_cm);   ;		   op_pk_nfd_set (read_value_pkt, "Weight_kg",weight_kg);   		      4		   copy_read_value_pkt=op_pk_copy(read_value_pkt);   0		   op_pk_send (read_value_pkt, SINK_OUT_STRM);   0		   //sending the rfid info to the end deveice    >		   op_pk_send (copy_read_value_pkt, STRM_FROM_READER_TO_ED);   e		   fprintf(Reader_log,"ReaderID=%d.  sending the rfid info to the end deveice  :+-+-\n",patientID);   		   readComplatedTagCount++;   		  // read_Mod=OPC_FALSE;   f	printf("readComplatedTagCount=%d TotalTagCount=%d   \n",readComplatedTagCount,readTotalTagCount);	      -		   if(readComplatedTagCount==TotalTagCount)   !			   readTotalTagCount=OPC_TRUE;   			   else   O			   timeOutEndEvh=op_intrpt_schedule_self ((op_sim_time()+.01),Timeout_code);   Q		    //timeOutEndEvh=op_intrpt_schedule_self ((op_sim_time()+1),tIMEoUTINTcODE);   		   }  break;   		}   	}   }   FOUT;   }                                         �   Z          
   init   
       
      	read_Mod=OPC_TRUE;   	readComplatedTagCount=0;   	//TotalTagCount=2;   	Query_rep_count=0;   	kill_status=OPC_FALSE;   	readTotalTagCount=OPC_FALSE;   B	/* obtain self object ID of the surrounding processor or queue */   	my_id = op_id_self();   6	/* obtain object ID of the parent object (node) */			   ,	my_node_id = op_topo_parent (my_id);							   C	op_ima_obj_attr_get (my_node_id, "MAC Address", &terminalAddress);   ?	op_ima_obj_attr_get (my_node_id, "MaxNoRfid", &TotalTagCount);   O	//op_ima_obj_attr_get (my_node_id, "DestinationAddress", &destinationAddress);   *	/* get the RFID activation of the node */   >	op_ima_obj_attr_get (my_node_id, "RFID Active", &RFID_log);		   +	if (RFID_log) ReaderRFID_log_file_init();	   !	/* Transmitter veri hizini al */   I	tx_id =  op_topo_assoc (my_id, OPC_TOPO_ASSOC_OUT, OPC_OBJTYPE_RATX, 0);   6	comp_id = op_topo_child (tx_id, OPC_OBJTYPE_COMP, 0);   ;	tx_ch_id = op_topo_child (comp_id, OPC_OBJTYPE_RATXCH, 0);   >//	op_ima_obj_attr_get (tx_ch_id, "data rate", &tx_data_rate);   
                     
   ����   
          pr_state         �  J          
   Query   
       
      ReaderQuery();   
       
      )	current_intrpt_type = op_intrpt_type ();   
       
   ����   
          pr_state         �   �          
   Select   
       
      ReaderSelect();   
                     
   ����   
          pr_state         �  �          
   wait   
       
      J timeOutEndEvh=op_intrpt_schedule_self ((op_sim_time()+0.1),Timeout_code);   
       
      (current_intrpt_type = op_intrpt_type ();   
           ����             pr_state         Z  �          
   Rcv_pkt   
       
      Reader_Received_Packet();   
       
      *//current_intrpt_type = op_intrpt_type ();   
       
   ����   
          pr_state        J  �          
   	Query_Rep   
       
      Query_rep();    
       
       
       
   ����   
          pr_state         Z   �          
   Stop   
       
      Reader_kill();   
       
      (current_intrpt_type = op_intrpt_type ();   
           ����             pr_state         	             �   �      �   `   �   �   �   �          
   tr_41   
       ����          ����          
    ����   
          ����                       pr_transition               �        �   �   �   �   �  3          
   tr_44   
       ����          ����          
    ����   
          ����                       pr_transition               �  �      �  _   �  �          
   tr_46   
       ����          ����          
    ����   
          ����                       pr_transition               �  �      �  �   �  �   d  �          
   tr_53   
       
   
PktReceive   
       ����          
    ����   
          ����                       pr_transition               �  �      b  �   �  �   �  �          
   tr_54   
       
   default   
       ����          
    ����   
          ����                       pr_transition                �      �  �    �  I  �          
   tr_56   
       
   Timeout   
       ����          
    ����   
          ����                       pr_transition              >  �     K  �    �   �  �          
   tr_57   
       
����   
       ����          
    ����   
          ����                       pr_transition               R  P      U  �   8  H   U   �          
   tr_58   
       
   STOP   
       ����          
    ����   
          ����                       pr_transition               m   �      Y   �   b   �   �   g          
   tr_63   
       
   KILL   
       ����          
    ����   
          ����                       pr_transition         
                                    