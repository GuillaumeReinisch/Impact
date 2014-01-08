/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2013  Guillaume <email>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "msUtilitiesBayes.h"
using namespace std;

void AnalyseHisto(vector<double>* values,double& CImin,double &CImax,double& mean)
{
  vector<double> histo;
  int nDiv= values->size();//10;
  histo.resize(nDiv+1);
  
  double min=(*values)[0];
  double max=min;
      
  for(size_t i = 0; i < values->size(); ++i)
         {
	  if((*values)[i]<min) { min=(*values)[i];}
	  if((*values)[i]>max) { max=(*values)[i];}
	  }
 
  if(min==max) { CImin=CImax=mean=min; return;} 
  
  for(int i = 0; i < nDiv; ++i) { histo[i] = 0;}
      
  for(size_t i = 0; i < values->size(); ++i)
     {
      int ind= nDiv*((*values)[i]-min)/(max-min)+0.5;
      histo[ind]++;	 
      }
       
  double sum=0;
  bool done5=0; bool done50=0;bool done95=0;
      
  for(int i = 0; i < nDiv; ++i) {
    
    sum+=histo[i];
	  
    double v=(min+(i-0.5)*(max-min)/nDiv);
	  
    if( (sum/values->size()>=0.05) && (!done5) )  {done5=1;CImin=v;}
    if( (sum/values->size()>=0.5) && (!done50) )  {done50=1;mean=v;}
    if( (sum/values->size()>=0.95) && (!done95) ) {done95=1;CImax=v;i = nDiv;}
  }  
  if (!done95) { CImax=min+(nDiv-0.5)*(max-min)/nDiv; }
 }

