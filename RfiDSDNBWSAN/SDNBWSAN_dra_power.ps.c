/* abj_dra_power.ps.c */                                                       

#include "opnet.h"
#include "dra.h"
#include <math.h>


/***** constants *****/

#define C					3.0E+08								/* speed of light (m/s) */
#define SIXTEEN_PI_SQ		(16.0 * VOSC_NA_PI * VOSC_NA_PI)	/* 16 times pi-squared */
#define	dc					(4 * VOSC_NA_PI * lambda)


/***** pipeline procedure *****/

#if defined (__cplusplus)
extern "C"
#endif

void SDNBWSAN_dra_power_mt (OP_SIM_CONTEXT_ARG_OPT_COMMA Packet * pkptr)
{
	double				   	prop_distance, rcvd_power, path_loss;
	double				   	tx_power, tx_base_freq, tx_bandwidth, tx_center_freq;
	double				   	lambda, rx_ant_gain, tx_ant_gain;
	Objid				   	rx_ch_obid;
	double				   	in_band_tx_power, band_max, band_min;
	double				   	rx_base_freq, rx_bandwidth;
	DraT_Rxch_State_Info*	rxch_state_ptr;
	Objid					rx_objid, RX_ID, TX_ID;
	double*					rx_power_threshold;
	int						signal_locked = OPC_FALSE;
	

	/* Compute the average power in Watts of the signal associated with a transmitted packet.				*/
	FIN_MT (SDNBWSAN_dra_power_mt (pkptr));

	/* If the incoming packet is mine, then	the channel match should be set to ignore.	*/
	RX_ID	= op_topo_parent (op_topo_parent (op_topo_parent (op_td_get_int (pkptr, OPC_TDA_RA_RX_CH_OBJID))));
	TX_ID	= op_topo_parent (op_topo_parent (op_topo_parent (op_td_get_int (pkptr, OPC_TDA_RA_TX_CH_OBJID))));

	if (RX_ID == TX_ID)	{ op_td_set_int (pkptr, OPC_TDA_RA_MATCH_STATUS, OPC_TDA_RA_MATCH_NOISE);}

	/* If the incoming packet is 'valid', it may cause the receiver to lock onto it.						*/
	/* However, if the receiving node is disabled, then	the channel match should be set to noise.			*/
	if (op_td_get_int (pkptr, OPC_TDA_RA_MATCH_STATUS) == OPC_TDA_RA_MATCH_VALID)
	{
		if (op_td_is_set (pkptr, OPC_TDA_RA_ND_FAIL))
		{
			/* The receiving node is disabled. Change the channel match status to noise.					*/
			op_td_set_int (pkptr, OPC_TDA_RA_MATCH_STATUS, OPC_TDA_RA_MATCH_NOISE);
		}
		else
		{
			/* The receiving node is enabled.  Get the address of the receiver channel.						*/
			rx_ch_obid = op_td_get_int (pkptr, OPC_TDA_RA_RX_CH_OBJID);
	
			/* Access receiver channels state information.	*/
			rxch_state_ptr = (DraT_Rxch_State_Info *) op_ima_obj_state_get (rx_ch_obid);
			
			/* If the receiver channel is already locked, the packet will now be considered to be noise.	*/
			/* This prevents simultaneous reception of multiple	valid packets on any given radio channel.	*/
			if (rxch_state_ptr->signal_lock)
			{
				op_td_set_int (pkptr, OPC_TDA_RA_MATCH_STATUS, OPC_TDA_RA_MATCH_NOISE);
			}
			else
			{
				/* Otherwise, the receiver channel will become locked until the packet reception ends.		*/
				rxch_state_ptr->signal_lock = OPC_TRUE;
				signal_locked = OPC_TRUE;
			}
		}
	}

	/* Get power allotted to transmitter channel. */
	tx_power = op_td_get_dbl (pkptr, OPC_TDA_RA_TX_POWER);

	/* Get transmission frequency in Hz. */
	tx_base_freq	= op_td_get_dbl (pkptr, OPC_TDA_RA_TX_FREQ);
	tx_bandwidth	= op_td_get_dbl (pkptr, OPC_TDA_RA_TX_BW);
	tx_center_freq	= tx_base_freq + (tx_bandwidth / 2.0);

	/* Calculate wavelength (in meters). */
	lambda = C / tx_center_freq;

	/* Get distance between transmitter and receiver (in meters). */
	prop_distance = op_td_get_dbl (pkptr, OPC_TDA_RA_START_DIST);

	/* When using TMM, the TDA OPC_TDA_RA_RCVD_POWER will already have a raw value for the path loss. */
	if (op_td_is_set (pkptr, OPC_TDA_RA_RCVD_POWER))	{ path_loss = op_td_get_dbl (pkptr, OPC_TDA_RA_RCVD_POWER);}

	else
	{	if (prop_distance > 0.0)
		{
			if (prop_distance < dc)		// Use two-ray ground model...The antenna lengths were selected as wavelength...
			{
				path_loss = (lambda * lambda * lambda * lambda) / (prop_distance * prop_distance * prop_distance * prop_distance);
			}
			else						// Use Free Space model...
			{
				path_loss = (lambda * lambda) / (SIXTEEN_PI_SQ * prop_distance * prop_distance);
			}
		}		
		else
		{
			path_loss = 1.0;
		}
	}

	/* Determine the receiver bandwidth and base frequency. */
	rx_base_freq = op_td_get_dbl (pkptr, OPC_TDA_RA_RX_FREQ);
	rx_bandwidth = op_td_get_dbl (pkptr, OPC_TDA_RA_RX_BW);

	/* Use these values to determine the band overlap with the transmitter.													*/
	/* Note that if there were no overlap at all, the packet would already have been filtered by the channel match stage.	*/

	/* The base of the overlap band is the highest base frequency. */
	if (rx_base_freq > tx_base_freq)					{ band_min = rx_base_freq;}

	else												{ band_min = tx_base_freq;}
	
	/* The top of the overlap band is the lowest end frequency. */
	if (rx_base_freq + rx_bandwidth > tx_base_freq + tx_bandwidth)	{ band_max = tx_base_freq + tx_bandwidth;}

	else															{ band_max = rx_base_freq + rx_bandwidth;}

	/* Compute the amount of in-band transmitter power. */
	in_band_tx_power = tx_power * (band_max - band_min) / tx_bandwidth;

	/* Get antenna gains (watt). */
	tx_ant_gain = pow (10.0, op_td_get_dbl (pkptr, OPC_TDA_RA_TX_GAIN) / 10.0);
	rx_ant_gain = pow (10.0, op_td_get_dbl (pkptr, OPC_TDA_RA_RX_GAIN) / 10.0);

	/* Calculate received power level. */
	rcvd_power = in_band_tx_power * tx_ant_gain * path_loss * rx_ant_gain;

	/* Assign the received power level (in Watts) to the packet transmission data attribute.	*/
	op_td_set_dbl (pkptr, OPC_TDA_RA_RCVD_POWER, rcvd_power);

	/* Control the packet for the own coverage area...*/
	if (op_td_get_int (pkptr, OPC_TDA_RA_MATCH_STATUS) == OPC_TDA_RA_MATCH_VALID)
	{
		if (op_td_is_set (pkptr, OPC_TDA_RA_ND_FAIL) == OPC_FALSE)
		{
			/* If the source of the packet is out of the coverage area...*/
			/* If the rcvd power is less than the reciver power threshold the packet must be marked as noise */
			rx_objid = op_topo_parent (op_topo_parent (rx_ch_obid));
			rx_power_threshold = (double*) op_ima_obj_state_get (rx_objid);
			//rx_power_threshold = op_ima_obj_state_get (rx_objid);
//			printf ("\n\tNode %d 'sc_dra_power_mt' Report: RSSI: %2.5f/%2.5f", (int)op_topo_parent(rx_objid), (log10(op_td_get_dbl (pkptr, OPC_TDA_RA_RCVD_POWER)*1000) * 10), (log10((*rx_power_threshold)*1000) * 10));

			if (rcvd_power < (*rx_power_threshold))
			{
				op_td_set_int (pkptr, OPC_TDA_RA_MATCH_STATUS, OPC_TDA_RA_MATCH_NOISE);

				if (signal_locked == OPC_TRUE)	{ rxch_state_ptr->signal_lock = OPC_FALSE;}

//				printf (" - Not Valid\n");
			}
//			else	{ printf ("\n");}
		}
	}
	FOUT
}