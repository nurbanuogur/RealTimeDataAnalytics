MIL_3_Tfile_Hdr_ 145A 140A modeler 9 5A5F2228 5BCB8879 4A HBI-JCGS160 Mohammed@Hussein 0 0 none none 0 0 none 9A459613 41DB 0 0 0 0 0 0 1e80 8                                                                                                                                                                                                                                                                                                                                                                                  ��g�      @   D   H      '  ?�  ?�  ?�  ?�  ?�  ?�  ?�  &�           	   begsim intrpt             ����      doc file            	nd_module      endsim intrpt             ����      failure intrpts            disabled      intrpt interval         ԲI�%��}����      priority              ����      recovery intrpts            disabled      subqueue                     count    ���   
   ����   
      list   	���   
          
      super priority             ����          \   Objid	\my_id;       Objid	\my_node_id;       double	\tx_data_rate;       double	\slot_genisligi;       int	\terminalAddress;       int	\destinationAddress;       Evhandle	\timeOutEndEvh;       Stathandle	\timerCancelStat;       Boolean	\came_select;       Boolean	\came_query;       Boolean	\reply_state;       Boolean	\arbitrate_state;       Boolean	\rtrn_rcv_pkt;       int	\header;       int	\man_number;       int	\object_class;       OpT_Int64	\serial_number;       Boolean	\came_ack;       Boolean	\came_query_rep;       int	\random_Qvalue;       Boolean	\came_read_req;       Boolean	\read_Complated;       Boolean	\came_query_adj;       int	\Qvalue;       int	\Qrange;       Boolean	\came_kill;       int	\temp_Qvalue;       Boolean	\reply_mod;       int	\hasta_2_kimlik;       int	\hasta_2_yas;       Packet *	\came_packet;       Evhandle	\reply_send;       Boolean	\reply_was_send;       int	\random_RN16;       Boolean	\ack_mod;       int	\EDAddress;       int	\SerialNumber;       int	\patientID;       int	\tagID;       Evhandle	\newRN16evh;       Boolean	\newreply_mod;       FILE*	\tag_log;       	int	\age;       int	\gender;       int	\height_cm;       int	\weight_kg;              //Packet*	came_packet;           !Objid 	tx_id, comp_id, tx_ch_id;    int		current_intrpt_type;          #include <math.h>   #include <stdio.h>   #include <stdlib.h>       !#define EPSILON    0.000000000001           #define TX_OUT_STRM		(0)   #define SINK_OUT_STRM	(1)   #define RX_IN_STRM		(1)       &#define SELECT (came_select==OPC_TRUE)   v#define QUERY (came_query==OPC_TRUE || came_query_rep==OPC_TRUE || came_query_adj==OPC_TRUE) && (reply_mod==OPC_FALSE)   '//#define QUERY_REP (came_query_rep==1)   %#define REPLY (reply_state==OPC_TRUE)   -#define ARBITRATE (arbitrate_state==OPC_TRUE)   -#define RTRN_RCV_PKT (rtrn_rcv_pkt==OPC_TRUE)   R#define ACK (came_ack==OPC_TRUE || came_read_req==OPC_TRUE) && (ack_mod==OPC_TRUE)   )#define NEW_SESSION (came_kill==OPC_TRUE)   (#define NEWRPLY (newreply_mod==OPC_TRUE)       #define tIMEoUTINTcODE	(1)   #define newRN16INTcODE (1)   #define replysendintcode (1)       c#define ReceivePktRX	(current_intrpt_type == OPC_INTRPT_STRM) && (op_intrpt_strm () == RX_IN_STRM)    %#define wAITfORACK (packetWasSent==1)   +#define qUEUEiSeMPTY 		((op_subq_empty(0)))   f#define tIMEoUT 			(current_intrpt_type == OPC_INTRPT_SELF) && ((op_intrpt_code () == tIMEoUTINTcODE))   �   "static void Receive_packet(void) {       char pk_format[50];       FIN(Receive_packet ());   3if((came_packet = op_pk_get (RX_IN_STRM))!=OPC_NIL)   {	   $op_pk_format(came_packet,pk_format);   =//fprintf (tag_log, "Tag packet Type -> %s    \n",pk_format);       4	 if (strcmp("query_packet_format",pk_format)==0) {	   		 came_query=OPC_TRUE;   V		 fprintf (tag_log, "|t = %f ->  receive pkt :query_packet_format \n",op_sim_time());   		 }    ;	 else if (strcmp("rfid_ack_packet_format",pk_format)==0) {   		came_ack=OPC_TRUE;     X		fprintf (tag_log, "|t = %f ->  receive pkt :rfid_ack_packet_format \n",op_sim_time());   		}    :	 else if (strcmp("queryRep_packet_format",pk_format)==0){   @		came_query_rep=OPC_TRUE; // here is the first most called case   $		// so much print for this sentence   Z		//fprintf (tag_log, "|t = %f ->  receive pkt :queryRep_packet_format \n",op_sim_time());   		 }    7	 else if (strcmp("read_packet_format",pk_format)==0)	{   		came_read_req=OPC_TRUE;   S		fprintf (tag_log, "|t = %f -> receive pkt :read_packet_format \n",op_sim_time());   		 }    =	 else if (strcmp("queryAdjust_packet_format",pk_format)==0){   @		came_query_adj=OPC_TRUE;// here is the second most called case   $		// so much print for this sentence   \		//fprintf (tag_log, "|t = %f -> receive pkt :queryAdjust_packet_format \n",op_sim_time());   		}    0	 else if (strcmp("kill_packet",pk_format)==0) {   		came_kill=OPC_TRUE;   L		fprintf (tag_log, "|t = %f -> receive pkt :kill_packet \n",op_sim_time());   		 }    	}       if(ack_mod==OPC_TRUE)   	{   6	 if (came_ack==OPC_FALSE && came_read_req==OPC_FALSE)   		 {   L		 newRN16evh=op_intrpt_schedule_self ((op_sim_time()+0.05),newRN16INTcODE);   K		 //newRN16evh=op_intrpt_schedule_self ((op_sim_time()+1),newRN16INTcODE);   		 if (op_ev_valid(newRN16evh))   				{   					newreply_mod=OPC_TRUE;   					op_ev_cancel(newRN16evh);   				}   		 }   	 }   	   FOUT;   }       static void  QueryTag() {   FIN (QueryTag ());   if(came_query==OPC_TRUE)   {   )op_pk_nfd_get(came_packet, "Q", &Qvalue);   temp_Qvalue=Qvalue;   Qrange=pow(2,Qvalue)-1;   ://random_Qvalue_temp=op_dist_load("uniform_int",Qrange,0);   4//random_Qvalue=op_dist_outcome(random_Qvalue_temp);   -//random_Qvalue2=op_dist_exponential(Qrange);   srand(time(NULL)/tagID);   random_Qvalue=rand()%Qrange;   _fprintf (tag_log,"%d. Tag has update the Q=rand()%(Q^2-1) value : %d**\n",tagID,random_Qvalue);   came_query=OPC_FALSE;   }   "else if (came_query_adj==OPC_TRUE)   	{int UpDn_value;   1	op_pk_nfd_get(came_packet, "UpDn", &UpDn_value);   	if (UpDn_value==2)   		{   		Qvalue--;   		if (Qvalue==0)   			{   			Qvalue=temp_Qvalue;   			}   7		Qrange=pow(2,Qvalue)-1; 	    srand(time(NULL)/tagID);   !	    random_Qvalue=rand()%Qrange;   Q	    fprintf (tag_log,"%d. Label take a new  value: %d**\n",tagID,random_Qvalue);   		}   	    came_query_adj=OPC_FALSE;   	}   	   !else if(came_query_rep==OPC_TRUE)   	{   	random_Qvalue=random_Qvalue-1;   N	fprintf (tag_log,"%d. label random_Qvalue flaw: %d**\n",tagID,random_Qvalue);   	came_query_rep=OPC_FALSE;   	}           FOUT;   }           static void  ReplyTag() {   Packet *RN16_pkt;       FIN (ReplyTag ());   %// create rfid_RN16_tag_packet_format   =   RN16_pkt= op_pk_create_fmt("rfid_RN16_tag_packet_format");      srand(time(NULL)/tagID);      random_RN16=rand();   ; //printf( "******** op_pk_print ReplyTag() *********\n" );   	//		op_pk_print (RN16_pkt);   .   op_pk_nfd_set (RN16_pkt, "RN",random_RN16);   q   fprintf (tag_log,"%d. rfid_RN16_tag_packet_format: Label RN16 another random_RN16: %d**\n",tagID,random_RN16);       &   op_pk_send (RN16_pkt, TX_OUT_STRM);      reply_state=OPC_FALSE;      reply_mod=OPC_TRUE;      ack_mod=OPC_TRUE;      newreply_mod=OPC_FALSE;      //reply_was_send=OPC_TRUE;       FOUT;   }           static void  AckTag() {   int came_RN16;   char pk_format[50];   >Packet*	epc_pkt; Packet* send_epc_pkt; Packet* read_tag_reply;   FIN (AckTag ());        	if (came_ack==OPC_TRUE)   		{   :		//printf( "******** op_pk_print AckTag() *********\n" );   			//op_pk_print (came_packet);   		       &		op_pk_format(came_packet,pk_format);   6		if (strcmp("rfid_ack_packet_format",pk_format)==0) {   2			op_pk_nfd_get(came_packet, "RN16", &came_RN16);   			came_ack=OPC_FALSE;   			if (random_RN16==came_RN16)   			{   2			epc_pkt = op_pk_create_fmt ("rfid_epc_packet");   -			op_pk_nfd_set (epc_pkt, "Header", header);   5			op_pk_nfd_set (epc_pkt, "Man Number",man_number);    9			op_pk_nfd_set (epc_pkt, "Object Class", object_class);   :			op_pk_nfd_set (epc_pkt, "Serial Number", SerialNumber);   <			send_epc_pkt = op_pk_create_fmt ("rfid_epc_send_packet");   ;			op_pk_nfd_set_pkt (send_epc_pkt, "EPC_number", epc_pkt);   *			op_pk_send (send_epc_pkt, TX_OUT_STRM);   <			fprintf(tag_log,"%d.ACK send Label of EPC : **\n",tagID);       			}   			}   		}   !	else if(came_read_req==OPC_TRUE)   		{	// from AckTag() function   J// here we get the information of the RFID tag then send it to RFID Reader   D		read_tag_reply= op_pk_create_fmt ("read_reply_tag_packet_format");   1		op_pk_nfd_set (read_tag_reply, "TagID", tagID);   9		op_pk_nfd_set (read_tag_reply, "PatientID", patientID);   .		op_pk_nfd_set (read_tag_reply, "Age", age);    4		op_pk_nfd_set (read_tag_reply, "Gender", gender);    :		op_pk_nfd_set (read_tag_reply, "Height_cm", height_cm);    :		op_pk_nfd_set (read_tag_reply, "Weight_kg", weight_kg);    		   +		op_pk_send (read_tag_reply, TX_OUT_STRM);   		came_read_req=OPC_FALSE;   		read_Complated=OPC_TRUE;   y		fprintf(tag_log,"**%d. label reading data has sent to the reader packet (read_reply_tag_packet_format)**\n",EDAddress);   		ack_mod=OPC_FALSE;   		}   	   FOUT;   }       "static void  TAG_log_file_init() {   Objid selfID;   Objid fatherID;   char folder_path[120];   char log_name[132];   char name [20];   //"Coordinator.NIntel_process";   7//strncpy(log_name, "Coordinator.NIntel_process", 132);   FIN (TAG_log_file_init ());   selfID = op_id_self();   "fatherID = op_topo_parent(selfID);   Bop_ima_obj_attr_get (fatherID, "Log File Directory", folder_path);   .op_ima_obj_attr_get (fatherID, "name", &name);       Fif (prg_path_name_is_dir (folder_path) == PrgC_Path_Name_Is_Not_Dir) {   		char msg[128];		   g		sprintf (msg, " \"%s\" is not valid directory name. The output will not be logged.\n", folder_path);    '		/* Display an appropriate warning.	*/   F		op_prg_odb_print_major ("Warning from Tag process: ", msg, OPC_NIL);   	}	   Csprintf (log_name, "%s%s%s.ak", folder_path, name,"_Tag_process");    ,printf ("Log file name: %s \n\n", log_name);   tag_log = fopen(log_name,"w");   gfprintf (tag_log, "|-----------------------------------------------------------------------------|\n");   ?fprintf (tag_log, "| NODE NAME          : %Tag info \n", name);   FOUT;   }                                         Z   Z          
   init   
       
   4   .TAG_log_file_init();// this only for log print       	Qvalue=0;   	Qrange=0;   //OPC_TRUE     /*   #came_select=OPC_FALSE;//OPC_FALSE     came_query=OPC_FALSE;   reply_state=OPC_FALSE;   reply_mod=OPC_FALSE;   came_ack=OPC_FALSE;   came_read_req=OPC_FALSE;   came_query_rep=OPC_FALSE;   arbitrate_state=OPC_FALSE;   rtrn_rcv_pkt=OPC_FALSE;   read_Complated=OPC_FALSE;   came_query_adj=OPC_FALSE;   came_kill=OPC_FALSE;   */   =//https://www.binaryhexconverter.com/hex-to-decimal-converter   header=0x10; //=16   man_number=0x01F1C07;//=2038791   object_class=0x0101B3;//=65971   //serial_number=0x000836FFE;   //hasta_2_kimlik=1111111111;   //hasta_2_yas=1990;   reply_was_send=OPC_FALSE;   ack_mod=OPC_FALSE;   newreply_mod=OPC_FALSE;           	my_id = op_id_self();   				   %	my_node_id = op_topo_parent (my_id);   								   5	//op_ima_obj_attr_get (my_node_id, "Ad", &tag_name);   -    //printf("Du ngum Adi: %s**\n",tag_name);   <	op_ima_obj_attr_get (my_node_id, "ED Address", &EDAddress);   3	op_ima_obj_attr_get (my_node_id, "TagID", &tagID);   B	op_ima_obj_attr_get (my_node_id, "Serial Number", &SerialNumber);   ;	op_ima_obj_attr_get (my_node_id, "PatientID", &patientID);   /	op_ima_obj_attr_get (my_node_id, "Age", &age);   5	op_ima_obj_attr_get (my_node_id, "Gender", &gender);   ;	op_ima_obj_attr_get (my_node_id, "Height_cm", &height_cm);   :op_ima_obj_attr_get (my_node_id, "Weight_kg", &weight_kg);   				       !	/* Transmitter veri hizini al */   I	tx_id =  op_topo_assoc (my_id, OPC_TOPO_ASSOC_OUT, OPC_OBJTYPE_RATX, 0);   6	comp_id = op_topo_child (tx_id, OPC_OBJTYPE_COMP, 0);   ;	tx_ch_id = op_topo_child (comp_id, OPC_OBJTYPE_RATXCH, 0);   >	//op_ima_obj_attr_get (tx_ch_id, "data rate", &tx_data_rate);   
       
      char	pk_format[50];   int 	Target;   char SL;   3if((came_packet = op_pk_get (RX_IN_STRM))!=OPC_NIL)   {	   &	op_pk_format(came_packet,pk_format);	   1	if (strcmp("select_packet_format",pk_format)==0)   		{   		came_select=1;   A		op_pk_nfd_get (came_packet, "Target", &Target);// select packet   		if(Target==1)   		SL=1;   a		fprintf(tag_log,"at (init): The Tag receives select Packet with a Target=%d   **\n",EDAddress);   		}   	   }   
       
    ����   
          pr_state         �   �          
   Ready   
       
       
       
      (current_intrpt_type = op_intrpt_type ();   
       
    ����   
          pr_state                    
   Pkt_rcv   
       
      Receive_packet ();   
       
      rtrn_rcv_pkt=OPC_FALSE;   
       
   ����   
          pr_state        �  �          
   Reply   
       
      ReplyTag();   
       
      //rtrn_rcv_pkt=OPC_TRUE;   (current_intrpt_type = op_intrpt_type ();   
       
    ����   
          pr_state         Z  �          
   	arbitrate   
       
      arbitrate_state=OPC_FALSE;   
       
      (current_intrpt_type = op_intrpt_type ();   
           ����             pr_state          �          
   Query   
       
      QueryTag();   
       
      if(random_Qvalue==0)   	reply_state=OPC_TRUE;   else    	arbitrate_state=OPC_TRUE;       
       
   ����   
          pr_state        �   �          
   ACK   
       
      	AckTag();   
                     
   ����   
          pr_state                      �   h      r   X   �   �          
   tr_39   
       
   SELECT   
       ����          
    ����   
          ����                       pr_transition               s   ,      r   T   �   .   X      [   D          
   tr_40   
       
   default   
       ����          
    ����   
          ����                       pr_transition               �   K      �   �   �   ;   �   D   �   �          
   tr_41   
       
   default   
       ����          
    ����   
          ����                       pr_transition               �   �      �   �   �   �              
   tr_43   
       
   ReceivePktRX   
       ����          
    ����   
          ����                       pr_transition              �  W     �  �  �  W  "            
   tr_49   
       
   ReceivePktRX   
       ����          
    ����   
          ����                       pr_transition                 �        �     �   �   �          
   tr_57   
       
   default   
       ����          
    ����   
          ����                       pr_transition                G         /  E    x          
   tr_58   
       
   QUERY   
       
����   
       
    ����   
       
   ����   
                    pr_transition               �  �      �  �   �  �   o  �          
   tr_60   
       
   	ARBITRATE   
       ����          
    ����   
          ����                       pr_transition               �  F      m  �   �  Y              
   tr_61   
       
   ReceivePktRX   
       ����          
    ����   
          ����                       pr_transition      	        c  �     &  �  u  �  �  �          
   tr_62   
       
   REPLY   
       ����          
    ����   
          ����                       pr_transition      
        [   �          T   �  {   �          
   tr_63   
       
   ACK   
       ����          
    ����   
          ����                       pr_transition                 ^     w   �  -   y   �   �          
   tr_64   
       ����          ����          
    ����   
          ����                       pr_transition               _  �      J  �   +  y   r  r   a  �          
   tr_65   
       
   default   
       ����          
    ����   
          ����                       pr_transition               s   �           O     Z   i          
   tr_66   
       
   NEW_SESSION   
       ����          
    ����   
          ����                       pr_transition              �  A     #    �    �  7  �  �          
   tr_14   
       
   NEWRPLY   
       ����          
    ����   
          ����                       pr_transition                                             