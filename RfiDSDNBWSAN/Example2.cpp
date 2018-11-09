/* sprintf example */
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

int main ()
{
  char buffer [50];
  int n, a=5, b=3;
  n=sprintf (buffer, "%d plus %d is %d", a, b, a+b);
  printf ("[%s] is a string %d chars long\n",buffer,n);
  
    int DijkPath[5] = {1,2,3,4,5};
 char err_str[300];
  for (int i = 0; i<5; ++i){
 
	 //cout << DijkPath[i]<< " ";

	 sprintf (err_str, " %d  =>  %d => %d  => %d  =>  %d  ", DijkPath[0], DijkPath[1], DijkPath[2], DijkPath[3], DijkPath[4]);
}
  
   printf ("[%s] is a path %d chars long\n",err_str,n);
  return 0;
}

