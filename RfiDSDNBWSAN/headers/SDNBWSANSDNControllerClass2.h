/*
 * SDNBWSANSDNControllerClass2.h
 *
 *  Created on: Dec 26, 2016
 *      Author: wsan
 */

#include "SDNBWSANSDNControllerClass.h"

using namespace std;
 
SDNBWSANSDNControllerClass::SDNBWSANSDNControllerClass(){

// initialize the Node status pointers  
	  nsHead=NULL;
	  nsCurr=NULL;
	  nsTemp=NULL;

}
 void SDNBWSANSDNControllerClass::CreateApplication(int ApplicationID,int  ApplicationSrc){
	CombinedFlowTables*  App=new CombinedFlowTables;
	App[0].Application.ApplicationID=ApplicationID;
	App[0].Application.ApplicationSRC=ApplicationSrc;
	Applications.push_back(App);
 }
 vector<FlowEntery*>  SDNBWSANSDNControllerClass::CreatePathEntry(vector<int> DijkPath,int ApplicationID  ){
		 
 	 ApplicationPath* Pathp = new ApplicationPath;

 	  for(int i=0; i<DijkPath.size();i++){
 	    FlowEntery* fEntery = new FlowEntery;
	 	fEntery->nodeAddress = DijkPath[i];
		fEntery->ApplicationID=ApplicationID;
		
		if (i == 0){
			fEntery->srcAdrMatchRule = DijkPath[i];
			fEntery->action=Forward;
		}
		else{
			fEntery->srcAdrMatchRule = DijkPath[i-1];
			fEntery->action=Forward;
		}
		//fEntery->action=Drop;
	if (i == DijkPath.size()-1)
			fEntery->NextHop = DijkPath[i];
		else
			fEntery->NextHop=DijkPath[i+1];
 	    Pathp->Nodes.push_back(fEntery);
		 }
	 for(int k=0;k<Applications.size();k++)
	 if(Applications[k]->Application.ApplicationID==ApplicationID)// if it is exist
	 {
		 Applications[k]->Application.Path=NULL;
		 Applications[k]->Application.Path=Pathp; // update the application's path
	 } 
 

return 	 Pathp->Nodes;
 }
 
vector<int> SDNBWSANSDNControllerClass::GetSrcApplication(int nodeAddress ){
 
vector<int> source;  source.clear();
	 for(int j=0;j<Applications.size();j++){
		cout<<"from class function appliction ID="<< Applications[j]->Application.ApplicationID<<endl;
		cout<<"from class function appliction SRC="<< Applications[j]->Application.ApplicationSRC<<endl;
		
		for(int i=0;i<Applications[j]->Application.Path->Nodes.size();i++){
			if (nodeAddress==Applications[j]->Application.Path->Nodes[i]->nodeAddress){
				source.push_back(Applications[j]->Application.ApplicationSRC);
				cout<<"nodeAddress loss battery ="<<Applications[j]->Application.Path->Nodes[i]->nodeAddress<<endl;
				}
			//cout<<"class appliction Path node address="<< Applications[j]->Application.Path->Nodes[i]->nodeAddress <<endl;
			//cout<<"class appliction Path NextHop="<<	Applications[j]->Application.Path->Nodes[i]->NextHop <<endl;
			//cout<<"class appliction Path action="<<		Applications[j]->Application.Path->Nodes[i]->action <<endl;
 	  }
  
   }
   return source;
}

int SDNBWSANSDNControllerClass::GetApplicationSize(){

   return Applications.size();
}

vector<AppStatus> SDNBWSANSDNControllerClass::GetApplicationStatus(){
	AppStatus element;
	vector<AppStatus> AppResult;  AppResult.clear();

	 for(int i=0;i<Applications.size();i++){
		 int counter=0;
	 for(int j=0;j<Applications.size();j++){
		 if(Applications[i]->Application.ApplicationSRC==Applications[j]->Application.ApplicationSRC) {
				if(i>j)	break;
				counter++;
			}
	 }
	 
	 if(counter) 
	 {
		element.NodeSrc =Applications[i]->Application.ApplicationSRC;
		element.Counter =counter;
		AppResult.push_back(element);
	 }		 
	 
	 }
   return AppResult;
}

