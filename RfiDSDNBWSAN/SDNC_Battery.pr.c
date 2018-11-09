/* Process model C form file: SDNC_Battery.pr.c */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char SDNC_Battery_pr_c [] = "MIL_3_Tfile_Hdr_ 145A 30A op_runsim 7 5A2F81E1 5A2F81E1 1 DESKTOP-JCGS160 Mohammed@Hussein 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1e80 8                                                                                                                                                                                                                                                                                                                                                                                       ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

/* Include files					*/
#include "headers\wpan_math.h"
#include "headers\wpan_struct.h"
#include "headers\wpan_params.h"


/* Codes for remote self interrupts	*/
#define PACKET_TX_CODE 101
#define PACKET_RX_CODE 102
#define END_OF_SLEEP_PERIOD 103


/* The same value should be in the wpan_mac Module for remote process	*/
#define END_OF_CAP_PERIOD_CODE 105
#define END_OF_ACTIVE_PERIOD_CODE 104


/* Special attribute values			*/
#define milli 0.001
#define micro 0.000001

/* Structures		*/
typedef struct {

	double power_supply; /*in Volt*/
	double initial_energy; /*in Joule*/
	double current_rx_mA;
	double current_tx_mA;
	double current_idle_microA;
	double current_sleep_microA;

	double current_energy; /*in Joule*/	
	
} wpan_battery_attributes;


typedef struct {

	Stathandle remaining_energy;
	Stathandle consumed_energy;
	Stathandle consumed_energy_Watt;
	Stathandle remaining_energy_Watt;
	//Stathandle remaining_energy_ratio;
	//Stathandle consumed_energy_ratio;	

} wpan_battery_statistics;


typedef struct {	

	Stathandle consumed_energy;	
} wpan_global_battery_statistics;


typedef struct {

	Boolean is_idle;
	Boolean is_sleep;
	
	double last_idle_time;
	double sleeping_time;
} wpan_node_activity;

/* Function prototypes.				*/
static void wpan_battery_init (void);
static void wpan_battery_update (void);



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
	Objid	                  		self_id                                         ;
	Objid	                  		parent_id                                       ;
	char	                   		device_name[20]                                 ;
	int	                    		my_address                                      ;
	wpan_battery_attributes			battery                                         ;
	wpan_battery_statistics			statistics                                      ;
	wpan_node_activity	     		activity                                        ;
	wpan_global_battery_statistics			statisticsG                                     ;
	} SDNC_Battery_state;

#define pr_state_ptr            		((SDNC_Battery_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))
#define self_id                 		pr_state_ptr->self_id
#define parent_id               		pr_state_ptr->parent_id
#define device_name             		pr_state_ptr->device_name
#define my_address              		pr_state_ptr->my_address
#define battery                 		pr_state_ptr->battery
#define statistics              		pr_state_ptr->statistics
#define activity                		pr_state_ptr->activity
#define statisticsG             		pr_state_ptr->statisticsG

/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	SDNC_Battery_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((SDNC_Battery_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


/* Function Block */

#if !defined (VOSD_NO_FIN)
enum { _op_block_origin = __LINE__ + 2};
#endif

/********************************************************************************* 
				IEEE 802.15.4/ZigBee OPNET Simulation Model
**********************************************************************************

Author: Petr Jurcik <petr@isep.ipp.pt> <jurcikp@control.felk.cvut.cz>
Author: Anis Koubaa <aska@isep.ipp.pt>

Organization:
	IPP-HURRAY Research Group
	CISTER/ISEP
	Polytechnic Institute of Porto, Portugal

Organization:
	Department of Control Engineering
	Faculty of Electrical Engineering
	Czech Technical University in Prague, Czech Republic
 
Copyright (c) 2007 - 2009 

This file is part of IEEE 802.15.4/ZigBee OPNET Simulation Model for OPNET Modeler. IEEE 802.15.4/ZigBee OPNET Simulation Model can be used, copied and modified under the next licenses:
 
 - GPL - GNU General Public License
 
Code can be modified and re-distributed under any combination of the above listed licenses. If a contributor does not agree with some of the licenses, he/she can delete appropriate line. If you delete all lines, you are not allowed to distribute source code and/or binaries utilizing code.


**********************************************************************************
                           GNU General Public License  

IEEE 802.15.4/ZigBee OPNET Simulation Model is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later version.
 
IEEE 802.15.4/ZigBee OPNET Simulation Model is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with IEEE 802.15.4/ZigBee OPNET Simulation Model; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 

*********************************************************************************/

/*********************************************************************************
 * The battery module computes the consumed and remaining energy levels.
 * The default values of the current draws are set to those of the MICAz mote.
**********************************************************************************/

/*--------------------------------------------------------------------------------
 * Function:	wpan_battery_init
 *
 * Description:	- initialize the process
 *				- read the attributes and set the global variables
 *
 * No parameters
 *--------------------------------------------------------------------------------*/

static void wpan_battery_init() {

	Objid current_draw_comp_id; 
	Objid current_draw_id; 	
	
	/* Stack tracing enrty point */
	FIN(wpan_battery_init);
	
	/* get the ID of this module */
	self_id = op_id_self ();
	
	/* get the ID of the node */
	parent_id = op_topo_parent (self_id);	
	
	/* get the value to check if this node is PAN Coordinator or not */
	//op_ima_obj_attr_get (parent_id, "Device Mode", &battery.Device_Mode);
	
	op_ima_obj_attr_get (parent_id, "name", &device_name);
	
	/* get the MAC address of the node */
	op_ima_obj_attr_get (parent_id, "MAC Address", &my_address);
	
	op_ima_obj_attr_get (self_id, "Power Supply", &battery.power_supply);
	op_ima_obj_attr_get (self_id, "Initial Energy", &battery.initial_energy);
	
	op_ima_obj_attr_get (self_id, "Current Draw", &current_draw_id);	
	current_draw_comp_id = op_topo_child (current_draw_id, OPC_OBJTYPE_GENERIC, 0);
	
	op_ima_obj_attr_get (current_draw_comp_id, "Receive Mode", &battery.current_rx_mA);
	op_ima_obj_attr_get (current_draw_comp_id, "Transmission Mode", &battery.current_tx_mA);
	op_ima_obj_attr_get (current_draw_comp_id, "Idle Mode", &battery.current_idle_microA);
	op_ima_obj_attr_get (current_draw_comp_id, "Sleep Mode", &battery.current_sleep_microA);
	
	battery.current_energy = battery.initial_energy;
	
	/* register the statistics that will be maintained by this model */
	statistics.remaining_energy	= op_stat_reg ("Battery.Remaining Energy (Joule)", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	statistics.remaining_energy_Watt	= op_stat_reg ("Battery.Remaining Energy (Watt)", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	statistics.consumed_energy	= op_stat_reg ("Battery.Consumed Energy (Joule)", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	statistics.consumed_energy_Watt	= op_stat_reg ("Battery.Consumed Energy (Watt)", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	statisticsG.consumed_energy	= op_stat_reg ("Battery.Consumed Energy (Joule)", OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
	
	op_stat_write(statistics.remaining_energy,battery.current_energy);
	op_stat_write(statistics.remaining_energy_Watt,battery.current_energy);
	
	
	op_stat_write(statistics.consumed_energy,0.0);
	op_stat_write(statistics.consumed_energy_Watt,0.0);

	op_stat_write(statisticsG.consumed_energy,0.0);
	
	activity.is_idle = OPC_TRUE;
	activity.is_sleep = OPC_FALSE;
	activity.last_idle_time = 0.0;
	activity.sleeping_time = 0.0;
	
	/* Stack tracing exit point */
	FOUT;
}


/*--------------------------------------------------------------------------------
 * Function:	wpan_battery_update
 *
 * Description:	compute the energy consumed and update enregy level
 *              ---------> consumed energy (JOULE) = current * time * voltage  = (AMPERE * SEC * VOLT) <--------------
 *
 * No parameters
 *--------------------------------------------------------------------------------*/

static void wpan_battery_update() {

	Ici * iciptr;
	double tx_time;
	double rx_time;
	double pksize;
	double wpan_data_rate;
	double consumed_energy;
	double idle_duration;
	double sleep_duration;
	
	/* Stack tracing enrty point */
	FIN(wpan_battery_update);
	
	if (op_intrpt_type() == OPC_INTRPT_REMOTE) {
		
	  switch (op_intrpt_code()) {		
		case PACKET_TX_CODE :
		{			
			/* get the ICI information associated to the remote interrupt */
			iciptr = op_intrpt_ici();
			
			op_ici_attr_get(iciptr, "Packet Size", &pksize);
			op_ici_attr_get(iciptr, "WPAN DATA RATE", &wpan_data_rate);
			
			op_ici_destroy(iciptr);
			
			/* compute the packet size of the transmitted packet */
			tx_time = pksize/wpan_data_rate;
			
			/* compute the consumed energy when transmitting a packet */
			consumed_energy= (battery.current_tx_mA * milli) * tx_time * battery.power_supply;

			/* compute the time spent by the node in idle state */
			idle_duration = op_sim_time()-activity.last_idle_time;
			
			/* update the consumed energy with the one of in idle state */
			consumed_energy= consumed_energy +(battery.current_idle_microA * micro) * idle_duration * battery.power_supply;
			
			/* update the current energy level */
			battery.current_energy = battery.current_energy - consumed_energy;
			
			/* update the time when the node enters the idle state. we add tx_time, because the remote process is generated at the time to start transmission */
			activity.last_idle_time = op_sim_time()+tx_time;
			
			/* update the statistics */			
			op_stat_write(statistics.remaining_energy,battery.current_energy);
			op_stat_write(statistics.remaining_energy_Watt,battery.current_energy);
			
			op_stat_write(statistics.consumed_energy,battery.initial_energy-battery.current_energy);
			
			//op_stat_write(statistics.consumed_energy_Watt,battery.current_energy);
			op_stat_write(statistics.consumed_energy_Watt,consumed_energy);
			
			op_stat_write(statisticsG.consumed_energy,consumed_energy);
			
			break;
		}
		
		case PACKET_RX_CODE :
		{
			/* get the ICI information associated to the remote interrupt */
			iciptr=op_intrpt_ici();
			
			op_ici_attr_get(iciptr, "Packet Size", &pksize);
			op_ici_attr_get(iciptr, "WPAN DATA RATE", &wpan_data_rate);
			
			op_ici_destroy(iciptr);
			
			
			/* compute the packet size of the transmitted packet */
			rx_time = pksize/wpan_data_rate;

			/* compute the consumed energy when receiving a packet */
			consumed_energy= (battery.current_rx_mA * milli) * rx_time * battery.power_supply;
			
			/* compute the time spent by the node in idle state */
			idle_duration = op_sim_time()-activity.last_idle_time;
			
			/* update the consumed energy with the one of in idle state */
			consumed_energy= consumed_energy +(battery.current_idle_microA * micro) * idle_duration * battery.power_supply;
			
			/* update the current energy level */
			battery.current_energy = battery.current_energy - consumed_energy;
			
			/* update the time when the node enters the idle state */
			activity.last_idle_time = op_sim_time();
			
			/* update the statistics */
			op_stat_write(statistics.remaining_energy, battery.current_energy);
			op_stat_write(statistics.remaining_energy_Watt,battery.current_energy);
			
			op_stat_write(statistics.consumed_energy, battery.initial_energy-battery.current_energy);
			//op_stat_write(statistics.consumed_energy_Watt, battery.initial_energy-battery.current_energy);
			op_stat_write(statistics.consumed_energy_Watt,consumed_energy);
			op_stat_write(statisticsG.consumed_energy, consumed_energy);
			
			break;
		}
		
		case END_OF_SLEEP_PERIOD :
		{
			/* compute the time spent by the node in sleep state */
			sleep_duration = op_sim_time()-activity.sleeping_time;
			
			/* energy consumed during sleeping mode */
			consumed_energy= (battery.current_sleep_microA * micro) * sleep_duration * battery.power_supply;
			
			//printf (" [%s (#%d)] t=%f  -> END_OF_SLEEP_PERIOD: current_sleep_microA = %f, time in the sleep period = %f , consumed_energy = %f mJoule\n", device_name, my_address, op_sim_time(), battery.current_sleep_microA, sleep_duration, consumed_energy*1000);
			
			/* update the current energy level */
			battery.current_energy = battery.current_energy - consumed_energy;
			
			op_stat_write(statistics.remaining_energy, battery.current_energy);
			op_stat_write(statistics.remaining_energy_Watt,battery.current_energy);
			
			op_stat_write(statistics.consumed_energy, battery.initial_energy-battery.current_energy);
			//op_stat_write(statistics.consumed_energy_Watt, battery.initial_energy-battery.current_energy);	
			op_stat_write(statistics.consumed_energy_Watt,consumed_energy);
			op_stat_write(statisticsG.consumed_energy, consumed_energy);

			activity.last_idle_time = op_sim_time();
			activity.is_idle = OPC_TRUE;
			activity.is_sleep = OPC_FALSE;				
			
			break;		
		}
		
		case END_OF_ACTIVE_PERIOD_CODE :
		{
			/* compute the time spent by the node in idle state */
			idle_duration = op_sim_time()-activity.last_idle_time;
			
			/* update the consumed energy with the one of in idle state */
			consumed_energy= (battery.current_idle_microA * micro) * idle_duration * battery.power_supply;			
				
			/* update the current energy level */
			battery.current_energy = battery.current_energy - consumed_energy;
			
			op_stat_write(statistics.remaining_energy,battery.current_energy);
			op_stat_write(statistics.remaining_energy_Watt,battery.current_energy);
			
			op_stat_write(statistics.consumed_energy,battery.initial_energy-battery.current_energy);
			//op_stat_write(statistics.consumed_energy_Watt, battery.initial_energy-battery.current_energy);
			op_stat_write(statistics.consumed_energy_Watt,consumed_energy);
			op_stat_write(statisticsG.consumed_energy,consumed_energy);
			
			activity.sleeping_time = op_sim_time();
			activity.is_idle = OPC_FALSE;
			activity.is_sleep = OPC_TRUE;
			
			break;		
		}
		
		default :
		{
		};		
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

#if defined (__cplusplus)
extern "C" {
#endif
	void SDNC_Battery (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Obtype _op_SDNC_Battery_init (int * init_block_ptr);
	void _op_SDNC_Battery_diag (OP_SIM_CONTEXT_ARG_OPT);
	void _op_SDNC_Battery_terminate (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Address _op_SDNC_Battery_alloc (VosT_Obtype, int);
	void _op_SDNC_Battery_svar (void *, const char *, void **);


#if defined (__cplusplus)
} /* end of 'extern "C"' */
#endif




/* Process model interrupt handling procedure */


void
SDNC_Battery (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (SDNC_Battery ());

		{


		FSM_ENTER_NO_VARS ("SDNC_Battery")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (dissipation) enter executives **/
			FSM_STATE_ENTER_UNFORCED (0, "dissipation", state0_enter_exec, "SDNC_Battery [dissipation enter execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_Battery [dissipation enter execs]", state0_enter_exec)
				{
				wpan_battery_update();
				}
				FSM_PROFILE_SECTION_OUT (state0_enter_exec)

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (1,"SDNC_Battery")


			/** state (dissipation) exit executives **/
			FSM_STATE_EXIT_UNFORCED (0, "dissipation", "SDNC_Battery [dissipation exit execs]")


			/** state (dissipation) transition processing **/
			FSM_TRANSIT_FORCE (0, state0_enter_exec, ;, "default", "", "dissipation", "dissipation", "transit", "SDNC_Battery [dissipation -> dissipation : default / ]")
				/*---------------------------------------------------------*/



			/** state (init) enter executives **/
			FSM_STATE_ENTER_FORCED_NOLABEL (1, "init", "SDNC_Battery [init enter execs]")
				FSM_PROFILE_SECTION_IN ("SDNC_Battery [init enter execs]", state1_enter_exec)
				{
				wpan_battery_init();
				}
				FSM_PROFILE_SECTION_OUT (state1_enter_exec)

			/** state (init) exit executives **/
			FSM_STATE_EXIT_FORCED (1, "init", "SDNC_Battery [init exit execs]")


			/** state (init) transition processing **/
			FSM_TRANSIT_FORCE (0, state0_enter_exec, ;, "default", "", "init", "dissipation", "tr_12", "SDNC_Battery [init -> dissipation : default / ]")
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (1,"SDNC_Battery")
		}
	}




void
_op_SDNC_Battery_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}




void
_op_SDNC_Battery_terminate (OP_SIM_CONTEXT_ARG_OPT)
	{

	FIN_MT (_op_SDNC_Battery_terminate ())


	/* No Termination Block */

	Vos_Poolmem_Dealloc (pr_state_ptr);

	FOUT
	}


/* Undefine shortcuts to state variables to avoid */
/* syntax error in direct access to fields of */
/* local variable prs_ptr in _op_SDNC_Battery_svar function. */
#undef self_id
#undef parent_id
#undef device_name
#undef my_address
#undef battery
#undef statistics
#undef activity
#undef statisticsG

#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

VosT_Obtype
_op_SDNC_Battery_init (int * init_block_ptr)
	{
	VosT_Obtype obtype = OPC_NIL;
	FIN_MT (_op_SDNC_Battery_init (init_block_ptr))

	obtype = Vos_Define_Object_Prstate ("proc state vars (SDNC_Battery)",
		sizeof (SDNC_Battery_state));
	*init_block_ptr = 2;

	FRET (obtype)
	}

VosT_Address
_op_SDNC_Battery_alloc (VosT_Obtype obtype, int init_block)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	SDNC_Battery_state * ptr;
	FIN_MT (_op_SDNC_Battery_alloc (obtype))

	ptr = (SDNC_Battery_state *)Vos_Alloc_Object (obtype);
	if (ptr != OPC_NIL)
		{
		ptr->_op_current_block = init_block;
#if defined (OPD_ALLOW_ODB)
		ptr->_op_current_state = "SDNC_Battery [init enter execs]";
#endif
		}
	FRET ((VosT_Address)ptr)
	}



void
_op_SDNC_Battery_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	SDNC_Battery_state		*prs_ptr;

	FIN_MT (_op_SDNC_Battery_svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (SDNC_Battery_state *)gen_ptr;

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
	if (strcmp ("battery" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->battery);
		FOUT
		}
	if (strcmp ("statistics" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->statistics);
		FOUT
		}
	if (strcmp ("activity" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->activity);
		FOUT
		}
	if (strcmp ("statisticsG" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->statisticsG);
		FOUT
		}
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

