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
                
                IMPACT_LOGIN();
                msMotion::update();
		Calculator = boost::shared_ptr<Antioch::CEACurveFit<double> >(
		             new Antioch::CEACurveFit<double>(getAllCoeffsOfFit()));
                IMPACT_LOGOUT();
            }
            
        public:
            
	    msMotionCEA(): msMotion(), nFit(0) {
                
                constructVar("msMotionCEA","MotionCEA","motion CEA");
		R =  csts::R; 
            }
            
            static boost::shared_ptr<msMotionCEA> New(){
                
                IMPACT_LOGIN_STATIC();
                boost::shared_ptr<msMotionCEA> T( new msMotionCEA() );
                T->initialize();
                T->setParameters( msMotionParams::New() );
                T->update();
                IMPACT_LOGOUT();
                return T;
            }
            
            void initialize() {
	      
                IMPACT_LOGIN();
                msMotion::initialize();
		declareAttribute(CoeffsFit_200_1000  ,"CoeffsFit_200_1000");
		declareAttribute(CoeffsFit_1000_6000 ,"CoeffsFit_1000_6000");
		declareAttribute(CoeffsFit_6000_20000,"CoeffsFit_6000_20000");
		declarePhysicalVariable( msUnit("J.mol^-1.K^-1"), &R, "R");
		IMPACT_LOGOUT();
            }
            
	    void parseFromAscii( string filename , string nameTarget);
	    
	    vector<double> getCoeffsOfFit(size_t i);
	    
	    vector<double> getAllCoeffsOfFit(); 
	    
	    void addCoeffsFit(vector<double>& fit);
	    
	    //! @name msMotion  methods
            //@{
	    
            double Q(double T){ IMPACT_THROW_NOT_IMPLEMENTED(); }            
	    
            void computeDOS() { IMPACT_THROW_NOT_IMPLEMENTED(); }         
            
            //! Density of states at energy E
            virtual double DOS( double E ){IMPACT_THROW_NOT_IMPLEMENTED(); }      

            double H(double T);
	    
            double S(double T);
            
	    double Cv(double T);  		
            
	    //@}
	    
            std::ostream& print(std::ostream& out) const ;
	  
	protected:
	   	  
	  vector<double> getFit(double T) {
	    
	      IMPACT_LOGIN();
	      IMPACT_EXCEPT_IF( [&](){return (T<200) || (T>20000);}, "CEA fit only for T>200 and T<20000" );
		
	      if( T>=200  && T < 1000 )    return CoeffsFit_200_1000;
	      if( T>=1000 && T < 6000 )    return CoeffsFit_1000_6000;
	      if( T>=6000 && T <= 20000 )  return CoeffsFit_6000_20000;
	      IMPACT_LOGOUT();
	  }
	  	    
        private:
            
	  //! @name Attributs & children
          //@{
	  vector<double> CoeffsFit_200_1000;
	  vector<double> CoeffsFit_1000_6000;
	  vector<double> CoeffsFit_6000_20000;
	  //@}
	  boost::shared_ptr<Antioch::CEACurveFit<double> > Calculator;
	  
	  size_t nFit;
	  double R; //! physical variable, always in the this->getUnits() unit
	};
        
    }
}

#endif // PA0DCSTSTP_H
