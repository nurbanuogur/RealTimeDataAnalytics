/*
 * Fuzzy.h
 *
 *  Created on: Dec 26, 2016
 *      Author: wsan
 */

#ifndef FUZZY_H_
#define FUZZY_H_




#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <iostream>

#include <cmath>

#include <cstring>



using namespace std;
 
struct InOutNames *LinkedlistInputs;

struct InOutNames *LinkedlistOutput;

#define MAXNAME 10

#define UPPER_LIMIT 50
 

struct InOutNames{

   char

	   name[MAXNAME];///////name string
 
   int value;

   struct MemberFunction *membership_functions;

   struct InOutNames *next;

   };

struct MemberFunction{

   char name[MAXNAME];

   int value;

   int FirstColumn;

   int LastColumn;

   float LinguisticVlaue1;

   float LinguisticVlaue2;

   struct MemberFunction *next;

   };

struct TypeRule{

   struct TypeRuleElement *if_side;

   struct TypeRuleElement *then_side;

   struct TypeRule *next;

   };

struct TypeRuleElement{

   int *value;

   struct TypeRuleElement *next;

   };

struct TypeRule *Rule_Base;





float TrapezoidComputeArea(struct MemberFunction *mf)



{  float run_1,run_2,area,top;

   float base;

   base=mf->LastColumn - mf->FirstColumn;

   run_1=mf->value / mf->LinguisticVlaue1;

   run_2=mf->value / mf->LinguisticVlaue2;

   top=base - run_1 - run_2;

   area=mf->value*(base+top)/2;

   return(area);

}





void EvaluationRule()

{  struct TypeRule *rule;

   struct TypeRuleElement *ip;     

   struct TypeRuleElement *tp;     

   int strength;

   int nomatch=0;                   

   for(rule=Rule_Base;rule!=NULL;rule=rule->next)

   {  strength=UPPER_LIMIT;



	  for(ip=rule->if_side;ip!=NULL;ip=ip->next){

             strength=min(strength,*(ip->value));
 
	  }



      for(tp=rule->then_side;tp!=NULL;tp=tp->next)

      {  *(tp->value)=max(strength,*(tp->value));       



	   if(strength>0)nomatch=1;                       

      }                                                 

   }

   if(nomatch==0)printf("NO MATCHING RULES FOUND!\n");  

}                                        /* END RULE EVALUATION */

void Defuzzification()

{  struct InOutNames *so;

   struct MemberFunction *mf;

   int sum_of_products;

   int sum_of_areas;

   int area, centroid;

   for(so=LinkedlistOutput;so!=NULL;so=so->next)

   {  sum_of_products=0;

      sum_of_areas=0;

      for(mf=so->membership_functions;mf!=NULL;mf=mf->next)

      {

		 area=TrapezoidComputeArea(mf);

         centroid=mf->FirstColumn+(mf->LastColumn-mf->FirstColumn)/2;

         sum_of_products+=area*centroid;

         sum_of_areas+=area;

      }

      if(sum_of_areas==0)

      {  printf("Sum of Areas = 0, will cause div error\n");

         printf("Sum of Products= %d\n",sum_of_products);

         so->value=0;

         return;

      }

      so->value=sum_of_products/sum_of_areas;

   }

}       /* END DEFUZZIFICATION */       /* END   TRAPEZOID */



void initialize()

{  int a, b, c, d ;

   struct InOutNames *outptr;

   struct MemberFunction *top_mf;

   struct MemberFunction *MemFunPtr;

   struct InOutNames *InputOutputPtr;

   struct TypeRule *ruleptr;

   struct TypeRuleElement *ifptr;

   struct TypeRuleElement *thenptr;

   InputOutputPtr=NULL;

   ruleptr=NULL;

   ifptr=NULL;

   thenptr=NULL;

   char labelBattery[10] = "Battery";

   char labelSNR[10] = "SNR";

   char labelConnCost[10] = "ConnCost";

   const int numberofInput =3;
   const int numberofInputBt =4;

   int InputSNR[numberofInput][4] = {
   	{0, 1, 10, 20 },
    {10, 20, 20, 40 },
	{20, 40, 40, 50 }
 
	  };

   char* InputSNRname[]={"L","M","H"};

   //int InputBattery[numberofInputBt][4] = {
   int InputBattery[numberofInput][4] = {
		{0, 1, 2, 3 },
		{2, 3, 3, 4 },
		{3, 4, 4, 5 }
		//{4, 5, 5, 6 }
		

		};

  //  char* InputBatteryname[]={"L","M","A","H"};
   char* InputBatteryname[]={"L","M","H"};
	const int numberofCost=6;

	int OutputCost[numberofCost][4] = {

		{1, 5, 5, 10 },
		{5, 10, 10, 15 },
		{10, 15, 15, 20 },
		{15, 20, 20, 25 },
		{20, 25, 25, 30 },
		{25, 30, 30, 35 }

	};

char* OutputCostName[]={"VL","L","M","A","H","VH"};
const int numberofrules=9;

char* Rules[numberofrules][3]={
	{"L","L","VH"},
	{"L","M","A"},
	{"L","H","L"},
	{"M", "L", "H"},
	{"M", "M", "A"},
	{"M", "H", "L"},
	{"H", "L", "H"},
	{"H", "M", "M"},
	{"H", "H", "VL"}

};
/*
const int numberofrules=12;

char* Rules[numberofrules][3]={
	{"L","L","VH"},
	{"L","M","A"},
	{"L","A","M"},
	{"L","H","L"},
	{"M", "L", "H"},
	{"M", "M", "M"},
	{"M", "A", "M"},
	{"M", "H", "VL"},
	{"H", "L", "H"},
	{"H", "M", "L"},
	{"H", "A", "VL"},
	{"H", "H", "VL"}
 
	{"L","L","VH"},
	{"L","M","A"},
	{"L","A","M"},
	{"L","H","L"},
	{"M", "L", "H"},
	{"M", "M", "A"},
	{"M", "A", "M"},
	{"M", "H", "VL"},
	{"H", "L", "H"},
	{"H", "M", "A"},
	{"H", "A", "M"},
	{"H", "H", "VL"}

};
*/
      //////////////////////////Input1//////////////////////////////////

// READ THE FIRST FUZZY SET (ANTECEDENT); INITIALIZE STRUCTURES

    InputOutputPtr=(struct InOutNames *)calloc(1,sizeof(struct InOutNames));

	//InputOutputPtr=new InOutNames;

   LinkedlistInputs=InputOutputPtr;                  /* Anchor to top of inputs */

   sprintf(InputOutputPtr->name,"%s",labelSNR);

   int col=0;

   MemFunPtr=NULL;// x = number of columns in the row

  for (int row = 0; row < numberofInput; row++)

   {

	   if(MemFunPtr==NULL)                    /* first time thru only */

      {	   MemFunPtr=(struct MemberFunction *)calloc(1,sizeof(struct MemberFunction));

		  //MemFunPtr=new  MemberFunction;

         top_mf=MemFunPtr;

         InputOutputPtr->membership_functions=MemFunPtr;

      }

      else

      {  for(MemFunPtr=top_mf;MemFunPtr->next;MemFunPtr=MemFunPtr->next); /* spin to last */

         //MemFunPtr->next=new MemberFunction;

			MemFunPtr->next=(struct MemberFunction *)calloc(1,sizeof(struct MemberFunction));

         MemFunPtr=MemFunPtr->next;

      }



      sprintf(MemFunPtr->name,"%s",InputSNRname[row]);    /* membership name, V H etc */

        a=InputSNR[row ][col ];   
		b=InputSNR[row ][col+1 ]; 
		c=InputSNR[row ][col+2 ];
		d=InputSNR[row ][col+3 ]; 

	 

	  MemFunPtr->FirstColumn=a;                  

      MemFunPtr->LastColumn=d;                  



	  if(b-a>0) MemFunPtr->LinguisticVlaue1=UPPER_LIMIT/(b-a);     

      else

      { cout<<"Error in input"<<InputSNRname[row];

         exit(1);

      }

      if(d-c>0) MemFunPtr->LinguisticVlaue2=UPPER_LIMIT/(d-c);      

      else

      { cout<<"Error in input"<<InputSNRname[row];

         exit(1);

      }

	  	col=0;

   }// end for



     //////////////////////////Input 2//////////////////////////////////

/* READ THE SECOND FUZZY SET (ANTECEDENT); INITIALIZE STRUCTURES */

   InputOutputPtr->next=(struct InOutNames *)calloc(1,sizeof(struct InOutNames));

   InputOutputPtr=InputOutputPtr->next;

	sprintf(InputOutputPtr->name,"%s",labelBattery);/* into struct InOutNames.name */

    MemFunPtr=NULL;

   col=0;
 
   for (int row = 0; row < numberofInputBt; row++)

   {  if(MemFunPtr==NULL)                    /* first time thru only */

      {  MemFunPtr=(struct MemberFunction *)calloc(1,sizeof(struct MemberFunction));

         top_mf=MemFunPtr;

         InputOutputPtr->membership_functions=MemFunPtr;

      }

      else

      {  for(MemFunPtr=top_mf;MemFunPtr->next;MemFunPtr=MemFunPtr->next); /* spin to last */

         MemFunPtr->next=(struct MemberFunction *)calloc(1,sizeof(struct MemberFunction));

         MemFunPtr=MemFunPtr->next;

      }



	    sprintf(MemFunPtr->name,"%s",InputBatteryname[row]);

        a=InputBattery[row ][col ];

		b=InputBattery[row ][col+1 ];

		c=InputBattery[row ][col+2 ];

		d=InputBattery[row ][col+3 ];



	  MemFunPtr->FirstColumn=a;                   

      MemFunPtr->LastColumn=d;                   

      if(b-a>0) MemFunPtr->LinguisticVlaue1=UPPER_LIMIT/(b-a);      

      else

      {  cout<<"Error in input"<<InputBatteryname[row];

         exit(1);

      }

      if(d-c>0) MemFunPtr->LinguisticVlaue2=UPPER_LIMIT/(d-c);     /* right slope */

      else

      {  cout<<"Error  membership element"<<InputBatteryname[row];

         exit(1);

      }

	  col=0;

   }// end for



    //////////////////////////output//////////////////////////////////



   // READ THE THIRD FUZZY SET (CONSEQUENCE); INITIALIZE STRUCTURES

   InputOutputPtr=(struct InOutNames *)calloc(1,sizeof(struct InOutNames));

   LinkedlistOutput=InputOutputPtr;                  /* Anchor output structure */

   sprintf(InputOutputPtr->name,"%s",labelConnCost);

   MemFunPtr=NULL;



	col=0;

   for (int row = 0; row < numberofCost; row++)

   {

	   if(MemFunPtr==NULL)

      {  MemFunPtr=(struct MemberFunction *)calloc(1,sizeof(struct MemberFunction));

         top_mf=MemFunPtr;

         InputOutputPtr->membership_functions=MemFunPtr;

      }

      else

      {  for(MemFunPtr=top_mf;MemFunPtr->next;MemFunPtr=MemFunPtr->next);

         MemFunPtr->next=(struct MemberFunction *)calloc(1,sizeof(struct MemberFunction));

         MemFunPtr=MemFunPtr->next;

      }

      sprintf(MemFunPtr->name,"%s",OutputCostName[row]);

        a=OutputCost[row][col];

		b=OutputCost[row][col+1];

		c=OutputCost[row][col+2];

		d=OutputCost[row][col+3];



	  MemFunPtr->FirstColumn=a;                   
 

      if(b-a>0) MemFunPtr->LinguisticVlaue1=UPPER_LIMIT/(b-a);     

      else

      { cout<<"Error in input"<<OutputCostName[row];

         exit(1);

      }



      if(d-c>0) MemFunPtr->LinguisticVlaue2=UPPER_LIMIT/(d-c);     

      else

      {  cout<<"Error in input"<<OutputCostName[row];

         exit(1);

      }



	    col=0;

   }//end   for





/* READ RULES FILE; INITIALIZE STRUCTURES */

   InputOutputPtr=NULL;

   outptr=NULL;



   ruleptr=(struct TypeRule *)calloc(1,sizeof(struct TypeRule));

   if(InputOutputPtr==NULL)Rule_Base=ruleptr;     /* first time thru, anchor */

   col=0;

  for (int row = 0; row < numberofrules; row++)

   {

	 InputOutputPtr=LinkedlistInputs;               /* points to Rssi */

      for(MemFunPtr=InputOutputPtr->membership_functions;MemFunPtr!=NULL;MemFunPtr=MemFunPtr->next)

      {  if((strcmp(MemFunPtr->name,Rules[row ][col]))==0)

		 {  ifptr=(struct TypeRuleElement *)

                  calloc(1,sizeof(struct TypeRuleElement));

        ruleptr->if_side=ifptr;      /* points to Rssi */

        ifptr->value=&MemFunPtr->value;  /* needs address here */

        ifptr->next=(struct TypeRuleElement *)

                  calloc(1,sizeof(struct TypeRuleElement));

        ifptr=ifptr->next;

        break;                       /* match found */

          }

      }

      InputOutputPtr=InputOutputPtr->next;                 /* points to Battery */

      for(MemFunPtr=InputOutputPtr->membership_functions;MemFunPtr!=NULL;MemFunPtr=MemFunPtr->next)

      {  if((strcmp(MemFunPtr->name,Rules[row][col+1]))==0)

     {  ifptr->value=&MemFunPtr->value;  /* needs address here */

        break;                       /* match found */

          }

      }

      if(outptr==NULL)outptr=LinkedlistOutput;/* point then stuff to output */

      for(MemFunPtr=outptr->membership_functions;MemFunPtr!=NULL;MemFunPtr=MemFunPtr->next)

      {  if((strcmp(MemFunPtr->name,Rules[row][col+2]))==0)

         {  thenptr=(struct TypeRuleElement *)

                calloc(1,sizeof(struct TypeRuleElement));

        ruleptr->then_side=thenptr;

            thenptr->value=&MemFunPtr->value; /* needs address here */

        break;                        /* match found */

         }

      }

      ruleptr->next=(struct TypeRule *)calloc(1,sizeof(struct TypeRule));

      ruleptr=ruleptr->next;

   col=0;

  }  // end of for              /* END WHILE READING RULES FILE */

}                                        /* END INITIALIZE */

 int  GetFuzzyOutputs()                     
{  struct InOutNames *InputOutputPtr;
 
   int hbi=0;
   for(InputOutputPtr=LinkedlistInputs;InputOutputPtr!=NULL;InputOutputPtr=InputOutputPtr->next)
   { // printf("%s: Value= %d\n",InputOutputPtr->name,InputOutputPtr->value);
    //  printf("\n");
   }
   
   for(InputOutputPtr=LinkedlistOutput;InputOutputPtr!=NULL;InputOutputPtr=InputOutputPtr->next)
   { // printf("%s: crisp Value= %d\n",InputOutputPtr->name,InputOutputPtr->value);
		hbi=InputOutputPtr->value;
   }
   return hbi;
}

void GetInputs(int input1,int input2)         /* NEW */
{

 struct InOutNames *InputOutputPtr;

   InputOutputPtr=LinkedlistInputs;

   InputOutputPtr->value=input1;

   InputOutputPtr=InputOutputPtr->next;

   InputOutputPtr->value=input2;

}

 void MembershipComputeDegree(struct MemberFunction *mf,int input){



  int Delta1, Delta2;

   Delta1=input - mf->FirstColumn;

   Delta2=mf->LastColumn - input;

   if((Delta1<=0)||(Delta2<=0))mf->value=0;

   else

   {

 // here where the value become 0 error hbi

	  mf->value=min((mf->LinguisticVlaue1*Delta1),(mf->LinguisticVlaue2*Delta2));

	  mf->value=min(mf->value,UPPER_LIMIT);
   }

}  // end of        /* END DEGREE OF MEMBERSHIP */

void Fuzzification()

{  struct InOutNames *si;

   struct MemberFunction *mf;

   for(si=LinkedlistInputs;si!=NULL;si=si->next)

      for(mf=si->membership_functions;mf!=NULL;mf=mf->next)

     MembershipComputeDegree(mf,si->value);

}                                        /* END FUZZIFICATION */


int fuzzy(int input1, int input2)
{
	if (input1<=1) 			 input1=1;
	if (input2<=1) 			 input2=1;
				 
 //printf("fff inputs to Fuzzy are SNR = %d Battery=%d  \n",input1,input2);
		initialize();

	   GetInputs(input1,input2);
 
	   Fuzzification();

	   EvaluationRule();

	   Defuzzification();

	   return GetFuzzyOutputs();


}
 

#endif /* FUZZY_H_ */
