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


#ifndef MSUTILITIES_H
#define MSUTILITIES_H

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <istream>
#include <fstream>
#include <sstream>
#include <errno.h>
#include <vector>
#include <string>
#include <map>
#include <math.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/exception/all.hpp>
#include <boost/shared_ptr.hpp>

//#include <msLogger.h>
#include <Eigen/Dense>



namespace impact
{
typedef Eigen::VectorXd vector_type;
typedef Eigen::MatrixXd matrix_type;
typedef Eigen::Vector3d vector_type3d;
typedef Eigen::Matrix3d matrix_type3d;
 
 }

template<class T>
void fillVecFromString(std::string str,std::vector<T>& vec,std::string sep)
{
 vec.clear();vec.resize(0);
 std::vector<std::string> strs;
 
 boost::split(strs, str , boost::is_any_of(sep));
 
 for( size_t i=0; i<strs.size(); i++)
    {
     if( strs[i]!="" ) {
     try { vec.push_back(boost::lexical_cast <T> (strs[i] )); 
           }
           catch(boost::bad_lexical_cast &e) 
               {  
                std::cout<<"Can not convert the string in the desired type ("<<strs[i] 
                         <<") in : void fillVecFromString<T>(std::vector<T>& vec,std::string strs)"<<std::endl;
                return;
                 }
           }
     }
 }

double Gamma(double x);
namespace impact
{

namespace output
{
 template<class T>
 std::string getString(std::vector<T>& elem) {
    
    std::stringstream out;out<<"[";
    typename std::vector< T >::iterator iter;

    for(iter=elem.begin(); iter!=elem.end(); ++iter) out<<*iter<<" " ; 

    out<<"]";return out.str();
    }

 template<class T>
 std::string getString(const T& elem) {

   std::stringstream out;out<<elem;return out.str();
   }

 std::string printYellow( std::string inp );
 std::string printGreen( std::string inp  );
 std::string printRed( std::string inp  );
 std::string printSepLine();
 std::ostream& printHeader(std::ostream& out,std::string name); 
 }
}


namespace bnu = boost::numeric::ublas;
 
int determinant_sign(const bnu::permutation_matrix<std::size_t>& pm);
 
double determinant( bnu::matrix<double>& m );

#endif // MSUTILITIES_H
