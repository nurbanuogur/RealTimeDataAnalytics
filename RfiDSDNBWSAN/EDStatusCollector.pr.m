MIL_3_Tfile_Hdr_ 145A 140A modeler 9 59F62629 59F62629 1 DESKTOP-JCGS160 Mohammed@Hussein 0 0 none none 0 0 none B16065CA 2BF2 0 0 0 0 0 0 1e80 8                                                                                                                                                                                                                                                                                                                                                                               ��g�      @   D   H    �  Y   �   �  $f  (  (  )�  )�  M           	   begsim intrpt         
   ����   
   doc file            	nd_module      endsim intrpt             ����      failure intrpts            disabled      intrpt interval         ԲI�%��}����      priority              ����      recovery intrpts            disabled      subqueue         
            count    ���   
   ����   
      list   	���   
          
   
   super priority             ����            begsim intrpt      begsim intrpt����   ����           ����          ����          ����                        double	\current_energy_level;       int	\a;       double	\updated_rssi;       double	\updated_rssi_src;       double	\updated_rssi_dst;       int	\intrpt_code;       int	\intrpt_type;       int	\intrpt_stream_indx;       double	\neighbour_list[15][2];       int	\no_neighbours;       +neighbour_list_struct	\neighbour_listS[15];       FILE*	\Slog_file;       Boolean	\enable_log;       int	\counterPassedThreshold;          Packet*		pkptr;   //Objid self_id;   !       #include <iostream>   #include <cstdlib>   #include "headers/Node.h"   using namespace std;   #define OUT_STRM   4   #define SendStss   210       // NDP neighbour list indeces   #define node_address_index		0   #define node_rssi_index    		1           Q#define		SEND_STATUS		(intrpt_type == OPC_INTRPT_SELF && intrpt_code == SendStss)   8#define		RECEIVE_STATUS	(intrpt_type == OPC_INTRPT_STRM)       !//struct for neighbour list array   /*   typedef struct {   int node_address;   double node_rssi;   } neighbour_list_struct;       // status info structure   typedef struct {   double energy_level;   int neighbours_no;   int parent_address;   (neighbour_list_struct nighbourList [10];   } SDNWSAN_status_values;   */   //functinos used   -static void  statusCollector_log_file_init();      .static void  statusCollector_log_file_init() {   Objid selfID;   Objid fatherID;   char folder_path[120];   char log_name[132];   char name [20];   //"Coordinator.NIntel_process";   7//strncpy(log_name, "Coordinator.NIntel_process", 132);   FIN (wpan_log_file_init ());   selfID = op_id_self();   "fatherID = op_topo_parent(selfID);   Bop_ima_obj_attr_get (fatherID, "Log File Directory", folder_path);   .op_ima_obj_attr_get (fatherID, "name", &name);       Fif (prg_path_name_is_dir (folder_path) == PrgC_Path_Name_Is_Not_Dir) {   		char msg[128];		   g		sprintf (msg, " \"%s\" is not valid directory name. The output will not be logged.\n", folder_path);    '		/* Display an appropriate warning.	*/   W		op_prg_odb_print_major ("Warning from network intelligence process: ", msg, OPC_NIL);   	}	   Gsprintf (log_name, "%s%s%s.ak", folder_path, name,"_StatusCollector");    ,printf ("Log file name: %s \n\n", log_name);    Slog_file = fopen(log_name,"w");   ifprintf (Slog_file, "|-----------------------------------------------------------------------------|\n");   Ifprintf (Slog_file, "| NODE NAME          : %sStatusCollector \n", name);   FOUT;   }                                  	  J            
   WAIT   
                     
       intrpt_code = op_intrpt_code ();    intrpt_type = op_intrpt_type ();       $if (intrpt_type == OPC_INTRPT_STRM){   'intrpt_stream_indx = op_intrpt_strm();}   
           ����             pr_state         Z            
   INIT   
       
      "a = 0; counterPassedThreshold = 0;   current_energy_level = 0;   updated_rssi = 0;   updated_rssi_src = -1;   updated_rssi_dst = -1;       //self_id   = op_id_self ();   '//parent_id = op_topo_parent (self_id);   Top_ima_obj_attr_get (op_topo_parent (op_id_self ()), "Enable Logging", &enable_log);       if (enable_log)   !	statusCollector_log_file_init();   
                     
   ����   
          pr_state        J   Z          
   BL_rcv   
       
   /       5//   here we get node statisics from incoming packet    &pkptr = op_pk_get (op_intrpt_strm ());       if (intrpt_stream_indx==0)   {//    =op_pk_nfd_get (pkptr, "energy_level", &current_energy_level);   a//cout << " this packets come from Battery process to collector " << current_energy_level <<endl;   }   !else if (intrpt_stream_indx==1) {   M//cout<<"SNR values inside packets come from MAC process to collector"<<endl;   #SDNWSAN_status_values *tempSvalues;   =op_pk_nfd_get_ptr (pkptr, "Stts_info", (void**)&tempSvalues);   9//op_pk_nfd_get (pkptr, "no_neighbours", &no_neighbours);   +no_neighbours = tempSvalues->neighbours_no;   //if(no_neighbours<11)   $for (int i=0; i<no_neighbours; i++){   M	neighbour_listS[i].node_address = tempSvalues->nighbourList[i].node_address;   ;	neighbour_listS[i].SNR = tempSvalues->nighbourList[i].SNR;   	}   op_prg_mem_free(tempSvalues);   }   op_pk_destroy(pkptr);   if (enable_log)   6	fprintf (Slog_file, "neighbour list at this time\n");   ,//	printf ("neighbour list at this time\n");   &	for (int i=0; i<no_neighbours; i++) {   	if (enable_log)   ~		fprintf (Slog_file, "%d. node address: %d of rssi of: %f  \n", i,  neighbour_listS[i].node_address, neighbour_listS[i].SNR);   s//		printf ("%d. node address: %d of rssi of: %f \n", i,  neighbour_listS[i].node_address, neighbour_listS[i].SNR);   		}   if (enable_log)   Z	fprintf (Slog_file, "*****************end of neighbour list*************************\n");   Z//	printf ("************************end of neighbour list***************************\n");	   							           if (a == 0)   	{   	a = 1;   	//send one time only    	//op_pk_send(pkptr,OUT_STRM);   	   :	//op_intrpt_schedule_self ((op_sim_time()+1.0),SendStss);   M	op_intrpt_schedule_self ((op_sim_time() + op_dist_uniform (1.0)), SendStss);       	}   
                     
   ����   
          pr_state        �            
   sendStss   
       
   %           $	SDNWSAN_status_values *tempSvalues;   *	pkptr = op_pk_create_fmt ("status_repo");   [	tempSvalues = (SDNWSAN_status_values *) op_prg_mem_alloc (sizeof (SDNWSAN_status_values));   ,	tempSvalues->neighbours_no = no_neighbours;   2	tempSvalues->energy_level = current_energy_level;   	   A	//printf("Node Status BT level=%f\n",tempSvalues->energy_level);   	//if(no_neighbours<11)   %	for (int i=0; i<no_neighbours; i++){   N		tempSvalues->nighbourList[i].node_address = neighbour_listS[i].node_address;   :		if (neighbour_listS[i].SNR<=1) neighbour_listS[i].SNR=1;   <		tempSvalues->nighbourList[i].SNR = neighbour_listS[i].SNR;   		}   						   ~	op_pk_nfd_set_ptr (pkptr, "Stts_info", tempSvalues, op_prg_mem_copy_create, op_prg_mem_free, sizeof (SDNWSAN_status_values));   	//op_pk_print (pkptr);   :	//setup the sending status acourding to threshold value 2   >	if (current_energy_level >= 2) 		op_pk_send (pkptr,OUT_STRM);   A	else if (current_energy_level < 2 && counterPassedThreshold < 4)   			{   A			counterPassedThreshold++;// counter try after passed threshold    			op_pk_send (pkptr, OUT_STRM);   			   			}   	//op_pk_destroy(pkptr);       9	//op_intrpt_schedule_self ((op_sim_time()+10),SendStss);   	   	if (a < 3)   		{   M	op_intrpt_schedule_self ((op_sim_time() + op_dist_uniform (2.0)), SendStss);   	a++;   	} else    O		op_intrpt_schedule_self ((op_sim_time() + op_dist_uniform (25.0)), SendStss);   (	//send the status every several seconds   
                     
   ����   
          pr_state                  	   �        s    <            
   tr_12   
       ����          ����          
@����   
          ����                       pr_transition         	        �     ;   �  ;   i          
   tr_2   
       
   RECEIVE_STATUS   
       ����          
    ����   
          ����                       pr_transition            	  a   �     X   q  X   �          
   tr_3   
       ����          ����          
    ����   
          ����                       pr_transition         	     �   �     c    �            
   tr_4   
       
   SEND_STATUS   
       ����          
    ����   
          ����                       pr_transition            	  �       �    a            
   tr_5   
       ����          ����          
    ����   
          ����                       pr_transition                   End-to-End Delay (seconds)          FEnd-to-end delay of packets received by the traffic sink in this node.   Traffic Sink    bucket/default total/sample mean   linear        ԲI�%��}   Traffic Received (bits)          <Traffic received (in bits) by the traffic sink in this node.   Traffic Sink   bucket/default total/sum   linear        ԲI�%��}   Traffic Received (bits/sec)          @Traffic received (in bits/sec) by the traffic sink in this node.   Traffic Sink   bucket/default total/sum_time   linear        ԲI�%��}   Traffic Received (packets)          ?Traffic received (in packets) by the traffic sink in this node.   Traffic Sink   bucket/default total/sum   linear        ԲI�%��}   Traffic Received (packets/sec)          CTraffic received (in packets/sec) by the traffic sink in this node.   Traffic Sink   bucket/default total/sum_time   linear        ԲI�%��}      End-to-End Delay (seconds)          GEnd-to-end delay of packets received by traffic sinks across all nodes.   Traffic Sink    bucket/default total/sample mean   linear        ԲI�%��}   Traffic Received (bits)          ATraffic received (in bits) by the traffic sinks across all nodes.   Traffic Sink   bucket/default total/sum   linear        ԲI�%��}   Traffic Received (bits/sec)          ETraffic received (in bits/sec) by the traffic sinks across all nodes.   Traffic Sink   bucket/default total/sum_time   linear        ԲI�%��}   Traffic Received (packets)          DTraffic received (in packets) by the traffic sinks across all nodes.   Traffic Sink   bucket/default total/sum   linear        ԲI�%��}   Traffic Received (packets/sec)          HTraffic received (in packets/sec) by the traffic sinks across all nodes.   Traffic Sink   bucket/default total/sum_time   linear        ԲI�%��}                  General Process Description:    ----------------------------    zThe sink process model accepts packets from any number of sources and discards them regardless of their content or format.       ICI Interfaces:    --------------    None        Packet Formats:    ---------------    None        Statistic Wires:    ----------------    None        Process Registry:    -----------------    Not Applicable       Restrictions:    -------------    None        