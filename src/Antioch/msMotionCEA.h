/*
 <one line to give the library's name and an idea of what it does.>
 Copyright (C) 2012  Guillaume <email>
 
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


#ifndef MSMOTIONCEA_H
#define MSMOTIONCEA_H

#include "antioch/chemical_mixture.h"
#include "antioch/cea_evaluator.h"
#include "antioch/cea_mixture.h"
#include "antioch/cea_thermo.h"
/*
#include "antioch/input_utils.h"
#include "antioch/cea_curve_fit.h"
#include "antioch/temp_cache.h"*/
#include <msMotion.h>


namespace impact {
    
    namespace antioch {
        
        using namespace impact::atomism;
        /** \brief Base class for a reaction
         *
         * A reaction is described by a set of coefficients and a name
         *
         */
        class msMotionCEA : public msMotion
        {
            
        private:
            
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isMotionCEARegisteredInPython;
            static msRegister* createInstance() { return new msMotionCEA; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void update() {
                
                LOGGER_ENTER_FUNCTION_DBG("void msMotionCEA::update()",getFullId());
                msMotion::update();
		updateCalculator();
                LOGGER_EXIT_FUNCTION2("void msMotionCEA::update()");
            }
            
        public:
            
	    msMotionCEA(): msMotion(), nFit(0) {
                
                constructVar("msMotionCEA","MotionCEA","motion CEA");
            }
            
            static boost::shared_ptr<msMotionCEA> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msMotionCEA> msMotionCEA::New()","");
                boost::shared_ptr<msMotionCEA> T( new msMotionCEA() );
                T->initialize();
                T->setParameters( msMotionParams::New() );
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msMotionCEA> msMotionCEA::New()");
                return T;
            }
            
            void initialize() {
	      
                LOGGER_ENTER_FUNCTION_DBG("void msMotionCEA::initialize()","");
                
                msMotion::initialize();
		
		declareAttribute(CoeffsFit_200_1000,"CoeffsFit_200_1000");
		declareAttribute(CoeffsFit_1000_6000,"CoeffsFit_1000_6000");
		declareAttribute(CoeffsFit_6000_20000,"CoeffsFit_6000_20000");
		
                LOGGER_EXIT_FUNCTION2("void msMotionCEA::initialize()");
            }
            
	    void parseFromAscii( string filename , string nameTarget) {
	      
	        ifstream in(filename.c_str());
	      
	        Antioch::skip_comment_lines(in, '#');

                std::string name;
                unsigned int n_int;
                std::vector<double> coeffs;
                double h_form, val;
		bool found = 0;
		
                while ( (in.good()) && (!found)) {
		  
		      in >> name;   // Species Name
		      in >> n_int;  // Number of T intervals: [200-1000], [1000-6000], ([6000-20000])
		      in >> h_form; // Formation Enthalpy @ 298.15 K

		      coeffs.clear();
		      for (unsigned int interval=0; interval<n_int; interval++) {
			
	                  for (unsigned int n=0; n<10; n++)  {
		              in >> val, coeffs.push_back(val);
	                  } 
	                  if(name==nameTarget) addCoeffsFit(coeffs);
	              }
		      if(name==nameTarget) found = 1;
		}
	    }
	    
	    vector<double> getCoeffsOfFit(size_t i) {
	      
	        if(i==0) return CoeffsFit_200_1000;
	        if(i==1) return CoeffsFit_1000_6000;
		if(i==2) return CoeffsFit_6000_20000;
		
		msError e("index out of bounds: 3 fits maximum","vector<double> msMotionCEA::getCoeffsOfFit(size_t i)",
			  getFullId());
		          
		BOOST_THROW_EXCEPTION(e);
		return vector<double>();
	    };
	    
	    vector<double> getAllCoeffsOfFit() {
	      
	        std::vector<double> coefs;
		for( size_t i=0;i<CoeffsFit_200_1000.size();i++) 
		  coefs.push_back(CoeffsFit_200_1000[i]);
		for( size_t i=0;i<CoeffsFit_1000_6000.size();i++) 
		  coefs.push_back(CoeffsFit_1000_6000[i]);
		for( size_t i=0;i<CoeffsFit_6000_20000.size();i++) 
		  coefs.push_back(CoeffsFit_6000_20000[i]);
		
		return coefs;
	    };
	    
	    //! @name msMotion  methods
            //@{
	    
            double Q(double T){
	      
                msError e("The computation of Q for a msMotionCEA need to be implemented",
			  "vector<double> msMotionCEA::Q(double T)",
			   getFullId());
		          
		BOOST_THROW_EXCEPTION(e);
		
                return QfromDOS(T);
            };
            
	    
            void computeDOS() {
	      
	        msError e("The computation of the DOS for a msMotionCEA need to be implemented",
			  "vector<double> msMotionCEA::computeDOS()",
			   getFullId());
		          
		BOOST_THROW_EXCEPTION(e);
            };;
            
            
            //! Density of states at energy E
            virtual double DOS( double E ){
                
	        msError e("The computation of the DOS for a msMotionCEA need to be implemented",
			  "vector<double> msMotionCEA::DOS( double E)",
			   getFullId());
		          
		BOOST_THROW_EXCEPTION(e);
		
            };

            double H(double T)   { 
	      
	        updateCalculator();
		Antioch::TempCache<double> cache(T);
		const double* a = Calculator->coefficients( Calculator->interval(cache.T) );
		double  H_on_RT    =  -a[0]/cache.T2 + a[1]*cache.lnT/cache.T + a[2] + a[3]*cache.T/2.0 + a[4]*cache.T2/3.0 + a[5]*cache.T3/4.0 + a[6]*cache.T4/5.0 + a[8]/cache.T;
 
		return H_on_RT * T * getUnits()->convert( "J.mol^-1.K^-1",  csts::R );
	        };
	    
            double S(double T) { 
	      
	        updateCalculator();
	        Antioch::TempCache<double> cache(T);
		const double* a = Calculator->coefficients( Calculator->interval(cache.T) );
		double  S_on_R    =  -a[0]/cache.T2/2.0 - a[1]/cache.T + a[2]*cache.lnT + a[3]*cache.T + a[4]*cache.T2/2.0 + a[5]*cache.T3/3.0 + a[6]*cache.T4/4.0 + a[9];
 
		return S_on_R * getUnits()->convert( "J.mol^-1.K^-1",  csts::R );
	        };
	     
            
	    double Cv(double T) { 
	      
	        updateCalculator();
	        Antioch::TempCache<double> cache(T);
		const double* a = Calculator->coefficients( Calculator->interval(cache.T) );
		double  Cp_on_R =  a[0]/cache.T2 + a[1]/cache.T + a[2] + a[3]*cache.T + a[4]*cache.T2 + a[5]*cache.T3 + a[6]*cache.T4;
		double R = getUnits()->convert( "J.mol^-1.K^-1",  csts::R );
		return Cp_on_R * R - R;
	        };  		
            
	    //@}
	    
            std::ostream& print(std::ostream& out) const ;
	   
	    void addCoeffsFit(vector<double>& fit) {
	    
	        if( nFit==0 ) CoeffsFit_200_1000 = fit;
	        if( nFit==1 ) CoeffsFit_1000_6000 = fit;
	        if( nFit==2 ) CoeffsFit_6000_20000 = fit;
	        if( nFit>=3 ) {
	      
	            msError e("CEA fit support 3 fits maximum",
			      "add msMotionCEA:: addCoeffsFit(vector<double>& fit) ",
		  	      getFullId());
		    BOOST_THROW_EXCEPTION(e);
	        }
		nFit++;
	  }
	  
	protected:
	   	  
	  vector<double> getFit(double T) {
	    
	      if( (T<200) || (T>20000) ) {
	      
	          msError e("CEA fit only for T>200 and T<20000 (T="+output::getString<double>(T)+")",
			    "vector<double>& msMotionCEA:: getFit(double T) ",
			    getFullId());
		  BOOST_THROW_EXCEPTION(e);
	      }
	      if( T>=200  && T < 1000 )    return CoeffsFit_200_1000;
	      if( T>=1000 && T < 6000 )    return CoeffsFit_1000_6000;
	      if( T>=6000 && T <= 20000 )  return CoeffsFit_6000_20000;
	  }
	  	    
        private:
            
	  vector<double> CoeffsFit_200_1000;
	  vector<double> CoeffsFit_1000_6000;
	  vector<double> CoeffsFit_6000_20000;
	  
	  boost::shared_ptr<Antioch::CEACurveFit<double> > Calculator;
	  
	  void updateCalculator() {
	    Calculator = boost::shared_ptr<Antioch::CEACurveFit<double> >(
		             new Antioch::CEACurveFit<double>(getAllCoeffsOfFit())
									      );
	  }
	  size_t nFit;
	  
	};
        
    }
}

#endif // PA0DCSTSTP_H
