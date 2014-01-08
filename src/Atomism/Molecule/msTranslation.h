/*
 <one line to give the program's name and a brief idea of what it does.>
 Copyright (C) <2011>  <G.Reinisch>
 
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

/*!
 * \file c_trans.h
 * \brief Overall translational degrees of freedom
 * \author G.Reinisch
 * \date 2011
 */

#ifndef msTranslation_H
#define msTranslation_H

#include <msMotion.h>
#include <msSystem.h>

namespace impact {
    
    namespace atomism {
        
        /*! \class msTranslation
         * \brief Model to account for the overall 3D translational motion of the system
         *
         * Model for thr overall translational degree of freedom. e.g. motion of a free particule with a mass ZMat->Mass();
         * Q=2\pi*ZMat->Mass()*k_bT/h^2)^1.5*V/Cstes->Nav;
         * The translational partition function has a volumetric unit, to be properly linked with the entropy and other statistic values
         * the SI is always used; whatever the Units attribut.
         * Compute for a pressure of P=1.01325E+05 pascal (1 atm)
         * The basic parameters is the Z-Matrix of the system, note that no copy of the pointer is realized
         *
         * The eigenstates are obtained by:
         * E=\frac{\hb^2\pi^2}{2ZMat->Mass()L^2}(n1^2+n2^2+n3^2)
         * \todo implement the quantum framework of msQMotion
         */
        class msTranslation : public msMotion
        {
            
        private:
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool        isTranslationRegisteredInPython;
            static msRegister* createInstance() { return new msTranslation; }
            //@}
            
        protected:
            
            void registryInPython(){
#if USE_PYTHON
                msMotion::registryInPython();
                
                if( ! msTranslation::isTranslationRegisteredInPython ) {
                    using namespace boost::python;
                    class_< msTranslation, bases<msMotion>, boost::shared_ptr<msTranslation> >(
                                                                                               "msTranslation", "An overall translation motion" ,
                                                                                               no_init )
                    .def( "New", &msTranslation::New , "Create a new object. arg2: msUnitsManager object")
                    .staticmethod("New")
                    .def( "setSystem" , &msTranslation::setSystem,"set the system of reference" )
                    .def(self_ns::str(self_ns::self));
                    
                    msTranslation::isTranslationRegisteredInPython=1;
                }
#endif
            };
            
        public:
            
            void initialize(){ msMotion::initialize();
                declareChild<msSystem>( System, msSystem::New() , "System" );
            }
            
            void update(){ msMotion::update();
            }
            
            static boost::shared_ptr<msTranslation> New( boost::shared_ptr<msUnitsManager> units ) {
                
                boost::shared_ptr<msTranslation> T( new msTranslation );
                T->initialize(); T->setUnits(units);
                T->setParameters( msMotionParams::New() );
                T->update();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msTranslation> clone = New( msUnitsManager::New() );
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            msTranslation() : msMotion() { constructVar("msTranslation","Translation","translation");};
            
            boost::shared_ptr<msTreeMapper> setSystem( boost::shared_ptr<msSystem> sys ){   msTreeMapper::update(System,sys);
                if(!sys->hasParent())  sys->setAffiliation(mySharedPtr());
                return mySharedPtr();
            };
            
            double Q(double T) { LOGGER_ENTER_FUNCTION("double msTranslation::Q(double T)",getFullId());
                if( System.getSharedPtr() == boost::shared_ptr<msSystem>() )
                    
                    BOOST_THROW_EXCEPTION(msError( "The system is not defined, please use the function 'setSystem'",
                                                  "void msTranslation::Q(double T)",getFullId()));
                
                double P=1.01325E+05;double R=8.3145;
                double V=R*T/P;
                double m=SiUnits.convert(System->getUnits()->getMass(), System->getMass() );
                
                double r = pow(2 * csts::pi * m * csts::kb * T/ (csts::h*csts::h),1.5) * V / csts::Nav;
                LOGGER_EXIT_FUNCTION();
                return r * Mult / Sym;
            }
            
            double dlogQ_dT_Vcst(double T) { return ( 3./(2.*T) );
            }
            
            /*   double S(double T) {  return ( getUnits()->convert(msUnit::J_mol,csts::R) * ( log(Q(T)) + 1 + 3/2 ) );
             }*/
            
            ostream& print(ostream& out) const { msMotion::print(out);
                output::printHeader(out,"Translation");
                if( System.getSharedPtr() == boost::shared_ptr<msSystem>() ) out<<"You need to define the system"<<endl;
                else out<<"\nTranslation of a body of mass "<<getUnits()->convert(System->getUnits()->getMass(), System->getMass() )<<" "<<getUnits()->getMassStr()<<endl;
                return out;
            };
            
        private:
            
            msChild< msSystem > System;
        };
        
        ostream& operator<<(ostream& out,const msTranslation& rotor);
    }
}
#endif // msTranslation_H
