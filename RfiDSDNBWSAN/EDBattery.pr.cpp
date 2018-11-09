/* Process model C++ form file: EDBattery.pr.cpp */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char EDBattery_pr_cpp [] = "MIL_3_Tfile_Hdr_ 145A 30A op_runsim 7 5A2F81D7 5A2F81D7 1 DESKTOP-JCGS160 Mohammed@Hussein 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1e80 8                                                                                                                                                                                                                                                                                                                                                                                       ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

/* Include files					*/
#include "headers\wpan_math.h"
#include "headers\wpan_struct.h"
#include "headers\wpan_params.h"
#include <stdio.h>
#include <string.h>

/* Codes for remote self interrupts	*/
#define PACKET_TX_CODE 101
#define PACKET_RX_CODE 102
#define END_OF_SLEEP_PERIOD 103
#define OUT_STRM 0


/* The same value should be in the wpan_mac Module for remote process	*/
#define END_OF_CAP_PERIOD_CODE 105
#define END_OF_ACTIVE_PERIOD_CODE 104


/* Special attribute values			*/
#define milli 0.001
#define micro 0.000001

/* Structures		*/
typedef struct {

	Objid own_id;
	Objid parent_id;
	double power_supply; /*in Volt*/
	double initial_energy; /*in Joule*/
	double current_rx_mA;
	double current_tx_mA;
	double current_idle_microA;
	double current_sleep_microA;

	double current_energy; /*in Joule*/	
	
	char Device_Mode[20];
	Boolean enable;
	
} wpan_battery_attributes;


typedef struct {

	Stathandle remaining_energy;
	Stathandle consumed_energy;
	Stathandle consumed_energy_Watt;
	//Stathandle remaining_energy_ratio;
	//Stathandle consumed_energy_ratio;	

} wpan_battery_statistics;


typedef struct {	

	Stathandle consumed_energy_Watt;
	Stathandle consumed_energy_Joule;	
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
static void send_status(void);


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
class EDBattery_state
	{
	private:
		/* Internal state tracking for FSM */
		FSM_SYS_STATE

	public:
		EDBattery_state (void);

		/* Destructor contains Termination Block */
		~EDBattery_state (void);

		/* State Variables */
		wpan_battery_attributes			battery                                         ;
		wpan_battery_statistics			statistics                                      ;
		wpan_global_battery_statistics			statisticsG                                     ;
		wpan_node_activity	     		activity                                        ;
		int	                    		send_counter                                    ;

		/* FSM code */
		void EDBattery (OP_SIM_CONTEXT_ARG_OPT);
		/* Diagnostic Block */
		void _op_EDBattery_diag (OP_SIM_CONTEXT_ARG_OPT);

#if defined (VOSD_NEW_BAD_ALLOC)
		void * operator new (size_t) throw (VOSD_BAD_ALLOC);
#else
		void * operator new (size_t);
#endif
		void operator delete (void *);

		/* Memory management */
		static VosT_Obtype obtype;
	};

VosT_Obtype EDBattery_state::obtype = (VosT_Obtype)OPC_NIL;

#define pr_state_ptr            		((EDBattery_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))
#define battery                 		pr_state_ptr->battery
#define statistics              		pr_state_ptr->statistics
#define statisticsG             		pr_state_ptr->statisticsG
#define activity                		pr_state_ptr->activity
#define send_counter            		pr_state_ptr->send_counter

/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	EDBattery_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((EDBattery_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


/* Function Block */

#if !defined (VOSD_NO_FIN)
enum { _op_block_origin = __LINE__ + 2};
#endif

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
//	wpan_battery_attributes battery;
//	wpan_battery_statistics statistics;
//	wpan_global_battery_statistics statisticsG;
//	wpan_node_activity activity;
static void wpan_battery_init() {

	Objid current_draw_comp_id; 
	Objid current_draw_id; 		
	
	// get the ID of this module 
	battery.own_id = op_id_self ();
	
	// get the ID of the node 
	battery.parent_id = op_topo_parent (battery.own_id);	
	
	// get the value to check if this node is PAN Coordinator or not 
	op_ima_obj_attr_get (battery.parent_id, "Device Mode", &battery.Device_Mode);
	
	op_ima_obj_attr_get (battery.own_id, "Power Supply", &battery.power_supply);
	op_ima_obj_attr_get (battery.own_id, "Initial Energy", &battery.initial_energy);
	
	op_ima_obj_attr_get (battery.own_id, "Current Draw", &current_draw_id);	
	current_draw_comp_id = op_topo_child (current_draw_id, OPC_OBJTYPE_GENERIC, 0);
	
	op_ima_obj_attr_get (current_draw_comp_id, "Receive Mode", &battery.current_rx_mA);
	op_ima_obj_attr_get (current_draw_comp_id, "Transmission Mode", &battery.current_tx_mA);
	op_ima_obj_attr_get (current_draw_comp_id, "Idle Mode", &battery.current_idle_microA);
	op_ima_obj_attr_get (current_draw_comp_id, "Sleep Mode", &battery.current_sleep_microA);
	
	battery.current_energy = battery.initial_energy;
	
	// register the statistics that will be maintained by this model 
	statistics.remaining_energy	= op_stat_reg ("Battery.Remaining Energy (Joule)", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	statistics.consumed_energy	= op_stat_reg ("Battery.Consumed Energy (Joule)", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	statistics.consumed_energy_Watt	= op_stat_reg ("Battery.Consumed Energy (Watt)", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
	
	statisticsG.consumed_energy_Joule	= op_stat_reg ("Battery.Consumed Energy (Joule)", OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
	//statisticsG.consumed_energy_Watt	= op_stat_reg ("Battery.Consumed Energy (Watt)", OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
	op_stat_write(statistics.remaining_energy,battery.current_energy);
	op_stat_write(statistics.consumed_energy,0.0);
	op_stat_write(statistics.consumed_energy_Watt,0.0);
	
	op_stat_write(statisticsG.consumed_energy_Joule,0.0);
	//op_stat_write(statisticsG.consumed_energy_Watt,0.0);
	
	send_status();
	
	
	
	activity.is_idle = OPC_TRUE;
	activity.is_sleep = OPC_FALSE;
	activity.last_idle_time = 0.0;
	activity.sleeping_time = 0.0;
}


/********* function send packet to stutus collector **********/

static void send_status() {

Packet* pkptr;
pkptr = op_pk_create_fmt ("status_repo");
//op_pk_nfd_set (pkptr, "energy_level",(battery.current_energy/battery.initial_energy) * 100); 

//printf("from BT itself BT level=%f\n",battery.current_energy);

op_pk_nfd_set (pkptr, "energy_level",battery.current_energy); 
op_pk_send (pkptr, OUT_STRM);
	
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
	
	if (op_intrpt_type() == OPC_INTRPT_REMOTE) {
		
	  switch (op_intrpt_code()) {		
		case PACKET_TX_CODE :
		{			
			//get the ICI information associated to the remote interrupt
			iciptr = op_intrpt_ici();
			
			op_ici_attr_get(iciptr, "Packet Size", &pksize);
			op_ici_attr_get(iciptr, "WPAN DATA RATE", &wpan_data_rate);
			
			op_ici_destroy(iciptr);
			
			// compute the packet size of the transmitted packet 
			tx_time =(double)( pksize/wpan_data_rate);
			
			// compute the consumed energy when transmitting a packet 
			consumed_energy=(double)( (battery.current_tx_mA * milli) * tx_time * battery.power_supply);

			// compute the time spent by the node in idle state 
			idle_duration = op_sim_time()-activity.last_idle_time;
			
			// update the consumed energy with the one of in idle state 
			consumed_energy= consumed_energy +(double)((battery.current_idle_microA * micro) * idle_duration * battery.power_supply);
			
			// update the current energy level 
			battery.current_energy = battery.current_energy - consumed_energy;
			//if ( battery.current_energy < 0.0001)
			//	op_sim_end("bettery finished", "battery level passed dead level","","");
			// update the time when the node enters the idle state. we add tx_time, because the remote process is generated at the time to start transmission
			activity.last_idle_time = op_sim_time()+tx_time;
			
			// update the statistics 			
			op_stat_write(statistics.remaining_energy,battery.current_energy);
			op_stat_write(statistics.consumed_energy,battery.initial_energy-battery.current_energy);
			op_stat_write(statistics.consumed_energy_Watt,consumed_energy);
			op_stat_write(statisticsG.consumed_energy_Joule,battery.initial_energy-battery.current_energy);
		//	op_stat_write(statisticsG.consumed_energy_Watt,consumed_energy);
			
			break;
		}
		
		case PACKET_RX_CODE :
		{
			// get the ICI information associated to the remote interrupt 
			iciptr=op_intrpt_ici();
			
			op_ici_attr_get(iciptr, "Packet Size",&pksize);
			op_ici_attr_get(iciptr, "WPAN DATA RATE",&wpan_data_rate);
			
			op_ici_destroy(iciptr);
	 
			// compute the packet size of the transmitted packet 
			rx_time =(double) (pksize/wpan_data_rate);

			// compute the consumed energy when receiving a packet 
			consumed_energy= (double) ((battery.current_rx_mA * milli) * rx_time * battery.power_supply);
			
			// compute the time spent by the node in idle state 
			idle_duration = op_sim_time()-activity.last_idle_time;
			
			// update the consumed energy with the one of in idle state 
			consumed_energy= consumed_energy +(double)((battery.current_idle_microA * micro) * idle_duration * battery.power_supply);
			
			// update the current energy level 
			battery.current_energy = battery.current_energy - consumed_energy;
			//if ( battery.current_energy < 0.0001)
				//op_sim_end("bettery finished", "battery level passed dead level","","");
			// update the time when the node enters the idle state 
			activity.last_idle_time = op_sim_time();
			
			// update the statistics 
			op_stat_write(statistics.remaining_energy, battery.current_energy);
			op_stat_write(statistics.consumed_energy, battery.initial_energy-battery.current_energy);
			op_stat_write(statistics.consumed_energy_Watt,consumed_energy);
			op_stat_write(statisticsG.consumed_energy_Joule,battery.initial_energy-battery.current_energy);
		//	op_stat_write(statisticsG.consumed_energy_Watt,consumed_energy);
			
			break;
		}
		
		case END_OF_SLEEP_PERIOD :
		{
			// compute the time spent by the node in sleep state 
			sleep_duration = op_sim_time()-activity.sleeping_time;
			
			// energy consumed during sleeping mode 
			consumed_energy=(double)( (battery.current_sleep_microA * micro) * sleep_duration * battery.power_supply);
			
			//printf ("END_OF_SLEEP_PERIOD: current_sleep_microA = %f, time in the sleep period = %f , consumed_energy = %f mJoule\n", battery.current_sleep_microA, sleep_duration, consumed_energy*1000);
			
			// update the current energy level 
			battery.current_energy = battery.current_energy - consumed_energy;
			//if ( battery.current_energy < 0.0001)
			//	op_sim_end("bettery finished", "battery level passed dead level","","");
			op_stat_write(statistics.remaining_energy, battery.current_energy);
			op_stat_write(statistics.consumed_energy, battery.initial_energy-battery.current_energy);
			op_stat_write(statistics.consumed_energy_Watt,consumed_energy);	
			op_stat_write(statisticsG.consumed_energy_Joule,battery.initial_energy-battery.current_energy);
			//op_stat_write(statisticsG.consumed_energy_Watt,consumed_energy);

			activity.last_idle_time = op_sim_time();
			activity.is_idle = OPC_TRUE;
			activity.is_sleep = OPC_FALSE;				
			
			break;		
		}
		
		case END_OF_ACTIVE_PERIOD_CODE :
		{
			// compute the time spent by the node in idle state 
			idle_duration = op_sim_time()-activity.last_idle_time;
			
			// update the consumed energy with the one of in idle state 
			consumed_energy=(double)( (battery.current_idle_microA * micro) * idle_duration * battery.power_supply);			
				
			// update the current energy level 
			battery.current_energy = battery.current_energy - consumed_energy;
			//if ( battery.current_energy < 0.0001)
			//	op_sim_end("bettery finished", "battery level passed dead level","","");
			op_stat_write(statistics.remaining_energy,battery.current_energy);
			op_stat_write(statistics.consumed_energy,battery.initial_energy-battery.current_energy);
			op_stat_write(statistics.consumed_energy_Watt,consumed_energy);
			op_stat_write(statisticsG.consumed_energy_Joule,battery.initial_energy-battery.current_energy);
			//op_stat_write(statisticsG.consumed_energy_Watt,consumed_energy);
			
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
#undef battery
#undef statistics
#undef statisticsG
#undef activity
#undef send_counter

/* Access from C kernel using C linkage */
extern "C"
{
	VosT_Obtype _op_EDBattery_init (int * init_block_ptr);
	VosT_Address _op_EDBattery_alloc (VosT_Obtype, int);
	void EDBattery (OP_SIM_CONTEXT_ARG_OPT)
		{
		((EDBattery_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))->EDBattery (OP_SIM_CONTEXT_PTR_OPT);
		}

	void _op_EDBattery_svar (void *, const char *, void **);

	void _op_EDBattery_diag (OP_SIM_CONTEXT_ARG_OPT)
		{
		((EDBattery_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))->_op_EDBattery_diag (OP_SIM_CONTEXT_PTR_OPT);
		}

	void _op_EDBattery_terminate (OP_SIM_CONTEXT_ARG_OPT)
		{
		/* The destructor is the Termination Block */
		delete (EDBattery_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr);
		}


} /* end of 'extern "C"' */




/* Process model interrupt handling procedure */


void
EDBattery_state::EDBattery (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (EDBattery_state::EDBattery ());
	try
		{


		FSM_ENTER_NO_VARS ("EDBattery")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (dissipation) enter executives **/
			FSM_STATE_ENTER_UNFORCED (0, "dissipation", state0_enter_exec, "EDBattery [dissipation enter execs]")
				FSM_PROFILE_SECTION_IN ("EDBattery [dissipation enter execs]", state0_enter_exec)
				{
				wpan_battery_update();
				
				 send_status();
				 /*
				if (send_counter % 600 == 0)
					{
					send_status();
					send_counter+= 9;// this will send 7 times status for 3 hours 
					}
				 
				*/
				}
				FSM_PROFILE_SECTION_OUT (state0_enter_exec)

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (1,"EDBattery")


			/** state (dissipation) exit executives **/
			FSM_STATE_EXIT_UNFORCED (0, "dissipation", "EDBattery [dissipation exit execs]")


			/** state (dissipation) transition processing **/
			FSM_TRANSIT_FORCE (0, state0_enter_exec, ;, "default", "", "dissipation", "dissipation", "transit", "EDBattery [dissipation -> dissipation : default / ]")
				/*---------------------------------------------------------*/



			/** state (init) enter executives **/
			FSM_STATE_ENTER_FORCED_NOLABEL (1, "init", "EDBattery [init enter execs]")
				FSM_PROFILE_SECTION_IN ("EDBattery [init enter execs]", state1_enter_exec)
				{
				wpan_battery_init();
				send_counter = 0;
				}
				FSM_PROFILE_SECTION_OUT (state1_enter_exec)

			/** state (init) exit executives **/
			FSM_STATE_EXIT_FORCED (1, "init", "EDBattery [init exit execs]")


			/** state (init) transition processing **/
			FSM_TRANSIT_FORCE (0, state0_enter_exec, ;, "default", "", "init", "dissipation", "tr_12", "EDBattery [init -> dissipation : default / ]")
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (1,"EDBattery")
		}
	catch (...)
		{
		Vos_Error_Print (VOSC_ERROR_ABORT,
			(const char *)VOSC_NIL,
			"Unhandled C++ exception in process model (EDBattery)",
			(const char *)VOSC_NIL, (const char *)VOSC_NIL);
		}
	}




void
EDBattery_state::_op_EDBattery_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}

void
EDBattery_state::operator delete (void* ptr)
	{
	FIN (EDBattery_state::operator delete (ptr));
	Vos_Poolmem_Dealloc (ptr);
	FOUT
	}

EDBattery_state::~EDBattery_state (void)
	{

	FIN (EDBattery_state::~EDBattery_state ())


	/* No Termination Block */


	FOUT
	}


#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

void *
EDBattery_state::operator new (size_t)
#if defined (VOSD_NEW_BAD_ALLOC)
		throw (VOSD_BAD_ALLOC)
#endif
	{
	void * new_ptr;

	FIN_MT (EDBattery_state::operator new ());

	new_ptr = Vos_Alloc_Object (EDBattery_state::obtype);
#if defined (VOSD_NEW_BAD_ALLOC)
	if (new_ptr == VOSC_NIL) throw VOSD_BAD_ALLOC();
#endif
	FRET (new_ptr)
	}

/* State constructor initializes FSM handling */
/* by setting the initial state to the first */
/* block of code to enter. */

EDBattery_state::EDBattery_state (void) :
		_op_current_block (2)
	{
#if defined (OPD_ALLOW_ODB)
		_op_current_state = "EDBattery [init enter execs]";
#endif
	}

VosT_Obtype
_op_EDBattery_init (int * init_block_ptr)
	{
	FIN_MT (_op_EDBattery_init (init_block_ptr))

	EDBattery_state::obtype = Vos_Define_Object_Prstate ("proc state vars (EDBattery)",
		sizeof (EDBattery_state));
	*init_block_ptr = 2;

	FRET (EDBattery_state::obtype)
	}

VosT_Address
_op_EDBattery_alloc (VosT_Obtype, int)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	EDBattery_state * ptr;
	FIN_MT (_op_EDBattery_alloc ())

	/* New instance will have FSM handling initialized */
#if defined (VOSD_NEW_BAD_ALLOC)
	try {
		ptr = new EDBattery_state;
	} catch (const VOSD_BAD_ALLOC &) {
		ptr = VOSC_NIL;
	}
#else
	ptr = new EDBattery_state;
#endif
	FRET ((VosT_Address)ptr)
	}



void
_op_EDBattery_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	EDBattery_state		*prs_ptr;

	FIN_MT (_op_EDBattery_svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (EDBattery_state *)gen_ptr;

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
	if (strcmp ("statisticsG" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->statisticsG);
		FOUT
		}
	if (strcmp ("activity" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->activity);
		FOUT
		}
	if (strcmp ("send_counter" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->send_counter);
		FOUT
		}
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

