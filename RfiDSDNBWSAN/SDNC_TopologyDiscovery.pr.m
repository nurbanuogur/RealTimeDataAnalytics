MIL_3_Tfile_Hdr_ 145A 140A modeler 9 59F62698 59F62698 1 DESKTOP-JCGS160 Mohammed@Hussein 0 0 none none 0 0 none 65BAFBE 3C79 0 0 0 0 0 0 1e80 8                                                                                                                                                                                                                                                                                                                                                                                ��g�      @   D   H    �  �  8�  8�  8�  8�  8�  :q  :u  �           	   begsim intrpt         
   ����   
   doc file            	nd_module      endsim intrpt             ����      failure intrpts            disabled      intrpt interval         ԲI�%��}����      priority              ����      recovery intrpts            disabled      subqueue         
            count    ���   
   ����   
      list   	���   
          
   
   super priority             ����            begsim intrpt      begsim intrpt����   ����           ����          ����          ����                        int	\total_no_nodes;       int	\intrpt_code;       int	\intrpt_type;       int	\intrpt_strm;       ,startTimeSoBo_list*	\startTimeSoBo_list_ptr;       *vector<SDNWSAN_status_values*>	\node_list;       FILE*	\topoLog;          Packet*		pkptr;   *   #include <stdio.h>   #include <vector>   #include <headers/node.h>   #define MAC_TO_TDP			106   #define DEFAULT_SOBO		0   #define NOT_DEFAULT_SOBO	1   #define TDP_OUT_INDEX		0   #define CHECK_STRUCT		22    #define BASE_START_TIME		0.03072   #define NOT_DEFINED			-1       using namespace std;   !//struct for neighbour list array   //typedef struct {   //int node_address;   //double SNR;   //} neighbour_list_struct;       // status info structure   //typedef struct {   //double energy_level;   //int neighbours_no;   //int nodeAddress;   //int parentAddress;   *//neighbour_list_struct nighbourList [10];   //} SDNWSAN_status_values;       typedef struct {   int nodes_no_to_be_set;   int nodeAddress[15];   double startTime[15];   int so[15];   int bo[15];   } startTimeSoBo_list;               ?#define INCOMING_PACKET 	(op_intrpt_type () == OPC_INTRPT_STRM)   \#define INCOMING_ndp_N_LIST 	(intrpt_type == OPC_INTRPT_REMOTE && intrpt_code == MAC_TO_TDP)   s#define CHECK_NODES_STRUCT_TO_SET_NET_PARAMS 	(op_intrpt_type () == OPC_INTRPT_SELF && intrpt_code == CHECK_STRUCT)       $static void topoDis_log_file_init();      &static void  topoDis_log_file_init() {   Objid selfID;   Objid fatherID;   char folder_path[120];   char log_name[132];   char name [20];   //"Coordinator.NIntel_process";   7//strncpy(log_name, "Coordinator.NIntel_process", 132);   FIN (topoDis_log_file_init ());   selfID = op_id_self();   "fatherID = op_topo_parent(selfID);   Bop_ima_obj_attr_get (fatherID, "Log File Directory", folder_path);   .op_ima_obj_attr_get (fatherID, "name", &name);       Fif (prg_path_name_is_dir (folder_path) == PrgC_Path_Name_Is_Not_Dir) {   		char msg[128];		   g		sprintf (msg, " \"%s\" is not valid directory name. The output will not be logged.\n", folder_path);    '		/* Display an appropriate warning.	*/   W		op_prg_odb_print_major ("Warning from network intelligence process: ", msg, OPC_NIL);   	}	   Esprintf (log_name, "%s%s%s.ak", folder_path, name,"_topoDisModule");    +printf ("Log file name: %s \n\n", topoLog);   topoLog = fopen(log_name,"w");   gfprintf (topoLog, "|-----------------------------------------------------------------------------|\n");   Efprintf (topoLog, "| NODE NAME          : %stopoDisModule \n", name);   FOUT;   }                                  	              
   idle   
                     
       intrpt_type = op_intrpt_type ();   #if (intrpt_type == OPC_INTRPT_STRM)   !	intrpt_strm = op_intrpt_strm ();   else   !	intrpt_code = op_intrpt_code ();   
           ����             pr_state         �            
   INIT   
       
      total_no_nodes = 0;   ;op_intrpt_schedule_self (op_sim_time() + 16, CHECK_STRUCT);   0startTimeSoBo_list_ptr = new startTimeSoBo_list;   topoDis_log_file_init();   
                     
   ����   
          pr_state        �            
   inPkt   
       
   5   ^//op_intrpt_schedule_self (op_sim_time() + 5, CHECK_STRUCT); // check topology after 5 seconds   int source_address;   
int i = 0;   int a = -1;   Packet* command_frame;   RSDNWSAN_status_values* SDNWSAN_status_values_ptr;// = new SDNWSAN_status_values ;    &pkptr = op_pk_get (op_intrpt_strm ());   9op_pk_nfd_get (pkptr, "Source Address", &source_address);           	   Oif (op_pk_nfd_get_pkt (pkptr, "MSDU", &command_frame) == OPC_COMPCODE_SUCCESS )   	{   v		if (op_pk_nfd_get_ptr (command_frame, "Command Payload", (void**)&SDNWSAN_status_values_ptr)!= OPC_COMPCODE_FAILURE)   			{   			   \			//printf("----t = %f ->---------  this is TDP  ----------------------\n", op_sim_time());   			   			   �			//printf ("********************* statuses of current incoming node %d *************************\n", SDNWSAN_status_values_ptr->nodeAddress);		   X			//printf (" The energy level is		  %f \n", SDNWSAN_status_values_ptr->energy_level);	   ]			//printf (" Number of neighbours is 	  %d \n", SDNWSAN_status_values_ptr->neighbours_no);	   V			//printf (" Parent address is 	  %d \n", SDNWSAN_status_values_ptr->parentAddress);   d			//printf ("*******************  The neighbour node list of this node **********************\n");	   G			//for (int l=0; l < SDNWSAN_status_values_ptr->neighbours_no; l++) {   �				//printf ("%d. node address: %d of SNR of: %f  \n", l+1,  SDNWSAN_status_values_ptr->nighbourList[l].node_address, SDNWSAN_status_values_ptr->nighbourList[l].SNR);   			//	}   /			// if the source is not in the list added it   			while (i < total_no_nodes) {   5			if (node_list[i++]->nodeAddress == source_address)   				a = i - 1;// src is here   			}    /			if (a == -1) {// if the src not exist add it   D				a = total_no_nodes++;// get the new index for src in vector list   4				node_list.push_back(SDNWSAN_status_values_ptr);	   				}   			else   				{       J			node_list[a]->neighbours_no = SDNWSAN_status_values_ptr->neighbours_no;   H			node_list[a]->energy_level = SDNWSAN_status_values_ptr->energy_level;   J			node_list[a]->parentAddress = SDNWSAN_status_values_ptr->parentAddress;   C			for (int j=0; j<SDNWSAN_status_values_ptr->neighbours_no; j++) {   i				node_list[a]->nighbourList[j].node_address = SDNWSAN_status_values_ptr->nighbourList[j].node_address;   W				node_list[a]->nighbourList[j].SNR = SDNWSAN_status_values_ptr->nighbourList[j].SNR;   				}   			}   			}   		op_pk_destroy(command_frame);   		}       op_pk_destroy(pkptr);       
       
      /*   Z			printf("----t = %f ->---------  this is TDP  ----------------------\n", op_sim_time());   L			printf("-------------------  Total node list  ----------------------\n");   e			printf("-------------------  Total number of nodes %d  ----------------------\n", total_no_nodes);   			   			   +			for (int k = 0; k < total_no_nodes; k++)   				{   y			printf ("********************* statuses of node address %d *************************\n", node_list[k]->nodeAddress);		   I			printf (" The energy level is		  %f \n", node_list[k]->energy_level);	   N			printf (" Number of neighbours is 	  %d \n", node_list[k]->neighbours_no);	   G			printf (" Parent address is 	  %d \n", node_list[k]->parentAddress);   b			printf ("*******************  The neighbour node list of this node **********************\n");	   6			for (int l=0; l<node_list[k]->neighbours_no; l++) {   �				printf ("%d. node address: %d of rssi of: %f  \n", l+1,  node_list[k]->nighbourList[l].node_address, node_list[k]->nighbourList[l].SNR);   				}   			}       */   
       
   ����   
          pr_state           Z          
   	iciFrmMac   
       
   +   Ici* iciptr;   int sdnc_no_neighbours;   int *n_addresses;       iciptr = op_intrpt_ici ();   Dop_ici_attr_get (iciptr, "sdnc_neighbours_no", &sdnc_no_neighbours);   4op_ici_attr_get (iciptr, "addresses", &n_addresses);   G//startTimeSoBo_list* startTimeSoBo_list_ptr = new startTimeSoBo_list;	   g//printf ("-----t = %f------ this is sdnc tdp neighbour list --------------------- \n", op_sim_time());   lfprintf (topoLog, "--t = %f------- this is sdnc tdp neighbour list --------------------|\n", op_sim_time());   C//printf ("number of neighbour nodes:) %d \n", sdnc_no_neighbours);   //printf ("addresses are: ");   Kfprintf (topoLog, "number of neighbour nodes:) %d \n", sdnc_no_neighbours);   %fprintf (topoLog, "addresses are: ");   ,for (int i = 0; i < sdnc_no_neighbours; i++)   #//printf ("%d - ", n_addresses[i]);   Q//printf ("\n-------------------------------------------------------------- \n");   ,for (int i = 0; i < sdnc_no_neighbours; i++)   +fprintf (topoLog, "%d - ", n_addresses[i]);   Yfprintf (topoLog, "\n-------------------------------------------------------------- \n");       #double startTime = BASE_START_TIME;   B		startTimeSoBo_list_ptr->nodes_no_to_be_set = sdnc_no_neighbours;   		Packet* TDPpkt;   &		TDPpkt = op_pk_create_fmt("TDPpkt");   .		for (int i = 0; i < sdnc_no_neighbours; i++)   			{   <				startTimeSoBo_list_ptr->nodeAddress[i] = n_addresses[i];   5				startTimeSoBo_list_ptr->startTime[i] = startTime;   0				startTime = startTime + BASE_START_TIME; //    			}   .		for (int i = 0; i < sdnc_no_neighbours; i++)   			{   `				fprintf (topoLog, "%d. node address: %d \n", i + 1, startTimeSoBo_list_ptr->nodeAddress[i]);   a				fprintf (topoLog, "%d. node start time: %f \n", i + 1, startTimeSoBo_list_ptr->startTime[i]);   			}           8fprintf (topoLog, " sending tdp packet to SDNC mac \n");       �		op_pk_nfd_set_ptr (TDPpkt, "startTimeSoBo_list", startTimeSoBo_list_ptr, op_prg_mem_copy_create, op_prg_mem_free, sizeof (startTimeSoBo_list));   1		op_pk_nfd_set(TDPpkt, "TD_code", DEFAULT_SOBO);   $		op_pk_send(TDPpkt, TDP_OUT_INDEX);   
                     
   ����   
          pr_state         �  �          
   checkStruct   
       
   @   >//op_intrpt_schedule_self (op_sim_time() + 300, CHECK_STRUCT);   Ofprintf(topoLog, "\n\n intterupt for checking network structure updates \n\n");   'int startTimeSoBo_list_ptr_counter = 0;   #double startTime = BASE_START_TIME;   P//int no_nodes_with_children = 0; // we may need it or will be calculated in mac       (for (int i = 0; i < total_no_nodes; i++)   	{   6		if (node_list[i]->parentAddress == 0)// SDNC address   		{   ,		Boolean node_with_child_found = OPC_FALSE;   		int outerLoopBreak = 0;   7		for (int j = 0; j < node_list[i]->neighbours_no; j++)   			{   :				//if (node_list[i]->nighbourList[j].node_address == 0)   				//	continue;   
				//else   -					for (int k = 0; k < total_no_nodes; k++)   )					{// if the node is SDNC nighbourList   R						if (node_list[k]->nodeAddress == node_list[i]->nighbourList[j].node_address)   							{   h								if (node_list[i]->nodeAddress == node_list[k]->parentAddress )// or it is SDNC nighbourList node   V									{// here is the problem when we have router with no child in the SDNC cluster   V									 // in this case, if this node selected in the route then app traffic droped    j										startTimeSoBo_list_ptr->nodeAddress[startTimeSoBo_list_ptr_counter] = node_list[i]->nodeAddress;   X										startTimeSoBo_list_ptr->startTime[startTimeSoBo_list_ptr_counter] = startTime;   I										startTimeSoBo_list_ptr->so[startTimeSoBo_list_ptr_counter] = 1;   K										startTimeSoBo_list_ptr->bo[startTimeSoBo_list_ptr_counter++] = 1;   +										node_with_child_found = OPC_TRUE;   2										startTime = startTime + BASE_START_TIME;   										outerLoopBreak = 1;   										break;   
									}   							}   					}   				if (outerLoopBreak == 1)   					break;   			}   		if (!node_with_child_found)   			{   d				startTimeSoBo_list_ptr->nodeAddress[startTimeSoBo_list_ptr_counter] = node_list[i]->nodeAddress;   T				startTimeSoBo_list_ptr->startTime[startTimeSoBo_list_ptr_counter] = NOT_DEFINED;   M				startTimeSoBo_list_ptr->so[startTimeSoBo_list_ptr_counter] = NOT_DEFINED;   O				startTimeSoBo_list_ptr->bo[startTimeSoBo_list_ptr_counter++] = NOT_DEFINED;   			}   		}   	}       LstartTimeSoBo_list_ptr->nodes_no_to_be_set = startTimeSoBo_list_ptr_counter;       |	fprintf (topoLog, "t = %f -> number of nodes to be set: %d \n", op_sim_time(), startTimeSoBo_list_ptr->nodes_no_to_be_set);   F		for (int i = 0; i < startTimeSoBo_list_ptr->nodes_no_to_be_set; i++)   			{   `				fprintf (topoLog, "%d. node address: %d \n", i + 1, startTimeSoBo_list_ptr->nodeAddress[i]);   a				fprintf (topoLog, "%d. node start time: %f \n", i + 1, startTimeSoBo_list_ptr->startTime[i]);   M				fprintf (topoLog, "%d. so: %d \n", i + 1, startTimeSoBo_list_ptr->so[i]);   M				fprintf (topoLog, "%d. bo: %d \n", i + 1, startTimeSoBo_list_ptr->bo[i]);   			}       Packet* TDPpkt;   $TDPpkt = op_pk_create_fmt("TDPpkt");   �op_pk_nfd_set_ptr (TDPpkt, "startTimeSoBo_list", startTimeSoBo_list_ptr, op_prg_mem_copy_create, op_prg_mem_free, sizeof (startTimeSoBo_list));   3op_pk_nfd_set(TDPpkt, "TD_code", NOT_DEFAULT_SOBO);   "op_pk_send(TDPpkt, TDP_OUT_INDEX);   
                     
   ����   
          pr_state                	   	  @  Q       )  @  G  "  !          
       
       
   default   
       ����              ����             ����                       pr_transition            	   �        �                 
   tr_12   
       ����          ����          
@����   
          ����                       pr_transition         	     �  %     &    �            
   tr_2   
       
   INCOMING_PACKET   
       ����          
    ����   
          ����                       pr_transition            	  �   �     �   �      �          
   tr_3   
       ����          ����          
    ����   
          ����                       pr_transition         	      �   �      �   �   �   g          
   tr_4   
       
   INCOMING_ndp_N_LIST   
       ����          
    ����   
          ����                       pr_transition            	     �        p     �          
   tr_5   
       ����          ����          
    ����   
          ����                       pr_transition         	      x  H      �     �  l          
   tr_6   
       
   $CHECK_NODES_STRUCT_TO_SET_NET_PARAMS   
       ����          
    ����   
          ����                       pr_transition            	   �  O      �  t    )          
   tr_7   
       ����          ����          
    ����   
          ����                       pr_transition                                       General Process Description:    ----------------------------    zThe sink process model accepts packets from any number of sources and discards them regardless of their content or format.       ICI Interfaces:    --------------    None        Packet Formats:    ---------------    None        Statistic Wires:    ----------------    None        Process Registry:    -----------------    Not Applicable       Restrictions:    -------------    None        