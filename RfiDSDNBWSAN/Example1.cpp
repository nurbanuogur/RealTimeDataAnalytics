#include <stdio.h>
 


int main(int argc, char *argv[])
{
     
  int example[5] = {1,2,3,4,5};
char output[5];
output[5]={};
int i;
for (i = 0 ; i < 5 ; ++i)
{
    output[i] = example[i] + '0';
}
   
    printf("%s\n",output);
   // printf("%s\n",buffer);
 
	return 0;
}
