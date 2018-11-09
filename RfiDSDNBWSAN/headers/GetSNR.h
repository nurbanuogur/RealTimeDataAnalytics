/*
 * SNR.h
 *
 *  Created on: April 18, 2017
 *      Author: wsan
 */

#ifndef GetSNR_H_
#define GetSNR_H_

 
#include <stdio.h>
#include <iostream>

 
using namespace std;
  
int GetSNR(int input1)
{ 
 int SNRcost=0;
	if (input1<=1)  			 input1=1;
		 
printf(" ^^^^^^^^^^^^^^^ SNR input is = %d    \n",input1 );
  //here for only SNR the MAX vlaues are nearest to dest 
  // so dijkstra will select the min values in this case
 SNRcost=(int) (50 -input1);

	   return SNRcost;


}
 

#endif /* GetSNR_H_ */
