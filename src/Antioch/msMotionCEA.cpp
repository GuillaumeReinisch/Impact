/*
 <one line to give the program's name and a brief idea of what it does.>
 Copyright (C) <year>  <name of author>
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */

#include "msMotionCEA.h"

namespace impact {
    
    namespace antioch {
        
        bool msMotionCEA::isMotionCEARegisteredInPython=0;
        
        msRegistrar msMotionCEA::Registrar("msMotionCEA", msMotionCEA::createInstance);
        
        
        ostream& operator<<(ostream& out,const msMotionCEA& rotor) {
            
            rotor.print(out);
            return out;
        }
        
        
        void msMotionCEA::registryInPython() {
            
#if USE_PYTHON
            msMotion::registryInPython();
            
            if( ! msMotionCEA::isMotionCEARegisteredInPython )
            {
                using namespace boost::python;
                
                class_< msMotionCEA, bases<msMotion>, boost::shared_ptr<msMotionCEA> >(
                       "msMotionCEA", "A motion described by CEA polynoms" ,
                        no_init )
                .def( "New", &msMotionCEA::New , "Create a new object.")
                .staticmethod("New");
		
		msTreeMapper::finalizeDeclarationForPython<msMotionCEA>("msMotionCEA");
		
                msMotionCEA::isMotionCEARegisteredInPython=1;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
	void msMotionCEA::parseFromAscii( string filename , string nameTarget) {
	      
	    IMPACT_LOGIN();
	    ifstream in(filename.c_str());
	    Antioch::skip_comment_lines(in, '#');

            std::string name;
            double      h_form, val;
	    bool        found = 0;
            size_t      n_int;
            std::vector<double> coeffs;
		
            while ( (in.good()) && (!found)) {
		  
	       in >> name >> n_int >> h_form; 
	       coeffs.clear();
	       for (unsigned int interval=0; interval<n_int; interval++) {
	           for (unsigned int n=0; n<10; n++) in >> val, coeffs.push_back(val);
	                  
	           if(name == nameTarget){ addCoeffsFit(coeffs);found = 1; }
	       }
	    }
	    IMPACT_LOGOUT();
	}

        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
	vector<double> msMotionCEA::getCoeffsOfFit(size_t i) {
	      
	    IMPACT_LOGIN();
	    IMPACT_EXCEPT_IF([&](){return i>3;},"index out of bounds: 3 fits maximum");
	    if(i==0) return CoeffsFit_200_1000;
	    if(i==1) return CoeffsFit_1000_6000;
	    if(i==2) return CoeffsFit_6000_20000;
	    IMPACT_LOGOUT();
	    return vector<double>();
	};

        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
         
	vector<double> msMotionCEA::getAllCoeffsOfFit() {
	      
	    std::vector<double> coefs = CoeffsFit_200_1000;
	    coefs.insert(coefs.end(), CoeffsFit_1000_6000.begin(),  CoeffsFit_1000_6000.end());
	    coefs.insert(coefs.end(), CoeffsFit_6000_20000.begin(), CoeffsFit_6000_20000.end());
	    return coefs;
	};

        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
         
	void msMotionCEA::addCoeffsFit(vector<double>& fit) {
	    
	    IMPACT_LOGIN();
	    int n=nFit;
	    IMPACT_EXCEPT_IF([&](){return n>=3;}, "CEA fit support 3 fits maximum" );
		
	    if( nFit==0 ) CoeffsFit_200_1000   = fit;
	    if( nFit==1 ) CoeffsFit_1000_6000  = fit;
	    if( nFit==2 ) CoeffsFit_6000_20000 = fit;
	    nFit++;
	    IMPACT_LOGOUT();
	}
	  
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msMotionCEA::H(double T)   { 
	      
	    IMPACT_LOGIN();
	    IMPACT_TRY([&](){ update();});
	    
	    Antioch::TempCache<double> cache(T);
	    const double* a = 0;
	    IMPACT_TRY([&](){ a = Calculator->coefficients( Calculator->interval(cache.T) );});
	    
	    double  H_on_RT =  -a[0]/cache.T2 + a[1]*cache.lnT/cache.T + a[2] + a[3]*cache.T/2.0 + a[4]*cache.T2/3.0 + a[5]*cache.T3/4.0 + a[6]*cache.T4/5.0 + a[8]/cache.T;
	    IMPACT_LOGOUT();
	    return H_on_RT * T * R;
	}
	  
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msMotionCEA::S(double T) { 
	      
	    IMPACT_LOGIN();
	    IMPACT_TRY([&](){ update();});
	    
	    Antioch::TempCache<double> cache(T);
	    const double* a = 0;
	    IMPACT_TRY([&](){ a = Calculator->coefficients( Calculator->interval(cache.T) );});
	    
	    double  S_on_R    =  -a[0]/cache.T2/2.0 - a[1]/cache.T + a[2]*cache.lnT + a[3]*cache.T + a[4]*cache.T2/2.0 + a[5]*cache.T3/3.0 + a[6]*cache.T4/4.0 + a[9];
            IMPACT_LOGOUT();
	    return S_on_R * R;
	}
	  
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msMotionCEA::Cv(double T) { 
	      
	    IMPACT_LOGIN();
	    IMPACT_TRY([&](){ update();});
	    
	    Antioch::TempCache<double> cache(T);
	    const double* a = 0;
	    IMPACT_TRY([&](){ a = Calculator->coefficients( Calculator->interval(cache.T) );});
	    
	    double  Cp_on_R =  a[0]/cache.T2 + a[1]/cache.T + a[2] + a[3]*cache.T + a[4]*cache.T2 + a[5]*cache.T3 + a[6]*cache.T4;
            IMPACT_LOGOUT();
  	    return Cp_on_R * R - R;
        }
	  
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        	
        ostream& msMotionCEA::print(ostream& out) const  {
            
            msMotion::print(out);
            output::printHeader(out,"CEA Motion");
            
            return out;
        }
    }
}


