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


#ifndef MSARRHENIUS_H
#define MSARRHENIUS_H

#include <msKineticsType.h>
//#include <antioch/arrhenius_rate.h>

namespace impact {
    
    namespace antioch {
        
        class msArrhenius : public msKineticsType
        {
            
        private:
            
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool        isArrheniusRegisteredInPython;
            static msRegister* createInstance() { return new msArrhenius; }
            //@}
            
        protected:
            
            void registryInPython();
            
	    void initialize() { 
	      
	        msKineticsType::initialize();
	        //msKineticsType::setCalculator( boost::shared_ptr<Antioch::ArrheniusRate<double> >(new Antioch::ArrheniusRate<double>()) );
		
		msTreeMapper::declareAttribute(A,"A");
		msTreeMapper::declareAttribute(Ea,"Ea");
		
		msUnit unitA( getUnits()->getStr(msUnit::vInverseTime));
		msUnit unitEa(getUnits()->getStr(msUnit::vEnergyByQuantity));
		msPhysicalInterface::declarePhysicalVariable(unitA,&A);
		msPhysicalInterface::declarePhysicalVariable(unitEa,&Ea);
	    }
            
            void update() {
	        
	        msKineticsType::update();
		
	        setEa(Ea);
		setA(A);
		//getCalculator<Antioch::ArrheniusRate>()->set_rscale(1);
	    }
	    
        public:
            
            msArrhenius() : msKineticsType() {
	        
	        constructVar("msArrhenius","Arrhenius","Arrhenius model for reaction rate"); 
	        A=0;Ea=0;
	    };
            
            static boost::shared_ptr<msArrhenius> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msArrhenius> msArrhenius::New()","");
                boost::shared_ptr<msArrhenius> T( new msArrhenius );
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msArrhenius> msArrhenius::New()");
                return T;
            }
                        
            double getA() { return A;
            };
            
            double getEa() { return Ea;
            };
	    
            boost::shared_ptr<msTreeMapper> setA(double value) {
	      
	        A = value;
	        stringstream unitA;
	        unitA<<"s^-1";
		unitA<<"m^"<<3*(getOrder()-1)<<"mol^-"<<getOrder()-1;
	       // getCalculator<ArrheniusRate>()->set_Cf( getUnits().convertTo(A, unitA.str()) );
	        
	        return mySharedPtr();
	    }

	    
	    boost::shared_ptr<msTreeMapper> setEa(double value) {
	      
	        Ea = value;
		//getCalculator<ArrheniusRate>()->set_Ea( getUnits().convertTo(Ea, "J.mol^-1"));
	        
	        return mySharedPtr();
	    }
	    
	    
	    virtual map<string,double> getCoefficients(){
	      
	      map<string,double> coefs;
	      coefs["A"]  = A;
	      coefs["Ea"] = Ea;
	      return coefs;
	    }
	    
	    virtual boost::shared_ptr<msTreeMapper> setCoefficient(string name,double value) {
	      
	      if     (name=="A")     setA(value);
	      else if(name=="Ea")    setEa(value);
	      else if(name=="Order") setOrder(value);
	      else {
	          msError e("The coeficient "+name+" is not known from msArrhenius",
		          "virtual boost::shared_ptr<msTreeMapper> msArrhenius::setCoefficient(string name,double value)",
			  getFullId());
		  BOOST_THROW_EXCEPTION(e);
	      }
	      return mySharedPtr();
	    }
	 
	    virtual bool sanityCheck(){
	        
	        msKineticsType::sanityCheck();
		
		stringstream unitA;
	        unitA<<"s^-1";
		unitA<<"m^"<<3*(getOrder()-1)<<"mol^-"<<getOrder()-1;
		/*
	        if( getUnits()->convertTo(Ea, "J.mol^-1") != getCalculator<Antioch::ArrheniusRate>()->get_Ea()){
		  
		  msError e("Calculator's activation energy and msArrhenius::Ea not consistent",
		            "",getFullId());
		  BOOST_THROW_EXCEPTION(e); 
		}
		
	        if( getUnits()->convertTo(A, unitA.str()) != getCalculator<Antioch::ArrheniusRate>()->get_Cf()){
		  
		  msError e("Calculator's pre-exp factor and msArrhenius::A not consistent",
		            "",getFullId());
		  BOOST_THROW_EXCEPTION(e); 
		}	        
	       */
	      return 1;
	    };
	    
	private:
	  
	   double A;
	   
	   double Ea;
	   
        };
    }
    
}

#endif