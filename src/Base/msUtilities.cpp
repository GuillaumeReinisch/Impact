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


#include "msUtilities.h"

 
double Gamma(double x) 
{ 
   double pi=3.14159265;
   
  if( x==0.5 )	 return sqrt(pi);
  if( x== 1 )	 return 1.;
  if( x==1.5 )	 return sqrt(pi)/2.;
  if( x==2. )	 return 1.;
  if( x==2.5 )	 return 3*sqrt(pi)/4.;
  if( x==3 )	 return 2.;
  if( x==3.5 )	 return 15*sqrt(pi)/8.;
  if( x==4 )	 return 6.;
  if( x > 3 )    return((x-1)*Gamma(x-1));
 // impact::LOGGER_WRITE_ALONE(impact::msLogger::FATAL, "Pb with the Gamma function, integer or half integer positive number must be providen"
   //                                 , "double Gamma(double x)");
  return -1;
 }
 
 
namespace impact
{
 
namespace output
{
 

 std::string printSepLine()
 {return( "---------------------------------------------------------------------------\n" );
  }
 
 std::string printYellow( std::string inp )
 {
  std::stringstream out;
  out<<"\033[1;33m"<<inp<<"\033[0m";
  return out.str();
  }
 std::string printRed( std::string inp )
 {
  std::stringstream out;
  out<<"\033[1;31m"<<inp<<"\033[0m";
  return out.str();
  }
 std::string printGreen( std::string inp )
 {
  std::stringstream out;
  out<<"\033[1;32m"<<inp<<"\033[0m";
  return out.str();
  }
  
 std::ostream& printHeader(std::ostream& out,std::string name)
 {
  out<<output::printSepLine()
     <<output::printYellow("\t"+name+"\n")
     <<output::printSepLine(); 
  return out;
  }
 }


}
