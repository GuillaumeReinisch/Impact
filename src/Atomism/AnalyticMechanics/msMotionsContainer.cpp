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

#include "msMotionsContainer.h"

namespace impact {
    
    namespace atomism {
        
        bool msMotionsContainer::isMotionsContainerdRegisteredInPython=0;
        
        msRegistrar msMotionsContainer::Registrar("msMotionsContainer", msMotionsContainer::createInstance);
        
        
        ostream& operator<<(ostream& out,const msMotionsContainer& rotor) {
            
            rotor.print(out);
            return out;
        }
        
        
        void msMotionsContainer::registryInPython() {
            
#if USE_PYTHON
            msMotion::registryInPython();
            
            if( ! msMotionsContainer::isMotionsContainerdRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_< msMotionsContainer, bases<msMotion>, boost::shared_ptr<msMotionsContainer> >(
                                                                                                     "msMotionsContainer", "A container for multiple motions",
                                                                                                     init<>() )
                .def( "New", &msMotionsContainer::New , "Create a new object.")
                .staticmethod("New")
                .def( "addMotion" , &msMotionsContainer::addMotion,
                     "Add a motion to the container. arg2: motion" )
                .def( "setRotor" , &msMotionsContainer::setRotor,
                     "Set the motion describing the overall rotation. arg2: rotor" );
                
                msTreeMapper::finalizeDeclarationForPython<msMotionsContainer>("msMotionsContainer");
                
                msMotionsContainer::isMotionsContainerdRegisteredInPython=1;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msMotionsContainer::addMotion( boost::shared_ptr<msMotion> motion) {
            
            LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msTreeMapper> msMotionsContainer::addMotion( boost::shared_ptr<msMotion> motion",
                                      getFullId());
            if(!motion->hasParent()) motion->setAffiliation(mySharedPtr());
            msTreeMapper::addElementToChildren<msMotion>(Motions,motion);
            LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msMotionsContainer::addMotion( boost::shared_ptr<msMotion> motion");
            return mySharedPtr();
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msMotionsContainer::setRotor( boost::shared_ptr<msRigidRotor> rotor) {
            
            LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msTreeMapper> msMotionsContainer::setRotor( boost::shared_ptr<msRigidRotor> rotor)",
                                      getFullId());
            if(!rotor->hasParent()) rotor->setAffiliation(mySharedPtr());
            declareChild<msRigidRotor>(Rotor,rotor,"Rotor");
            LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msMotionsContainer::setRotor( boost::shared_ptr<msRigidRotor> rotor)");
            return mySharedPtr();
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msMotionsContainer::Q(double T) {
            
            msChildren<msMotion>::iterator motion = Motions.begin();
            
            double Q=1; double q;
            for( ; motion != Motions.end() ; ++motion) {
                
                q=(*motion)->Q(T);
                Q*=q;
                LOGGER_WRITE(msLogger::DEBUG,"Partition function of motion " +(*motion)->getId()+":"+
                             output::getString<double>(q));
            }/*
              if( Rotor.getSharedPtr() ){
              q = Rotor->Q(T);
              LOGGER_WRITE(msLogger::DEBUG,"Partition function of the rotor: "+ output::getString<double>(q));
              }*/
            return Q*q;
        };
	    
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msMotionsContainer::computeDOS() {
            
            LOGGER_ENTER_FUNCTION_DBG("void msMotionsContainer::computeDOS()",
                                      getFullId());
            
            vector<double> EV,g;
            
            int     M    = Emax/dE + 1;
            
            LOGGER_WRITE(msLogger::DEBUG,"Stein-Rabinovitch algo. for the DOS computation using dE=" +
                         output::getString<double>(dE) + getUnits()->getEnergyStr() +
                         " and Emax="+output::getString<double>(Emax));
            
            vector<double> T(M,0);
            T[1]  = 1;
            vector<double> AT(M,0);
            AT[1] = 1;
            
            //--------------Copy the first activated motion in T and AT------//
            
            msChildren<msMotion>::iterator motion = Motions.begin();
            
            if( (*motion)->getUnits() != getUnits() ) throwUnitsException("void msMotionsContainer::computeDOS()");
            
            LOGGER_WRITE(msLogger::DEBUG,"Init DOS with motion " +(*motion)->getId() );
            
            (*motion)->FillEvAndG(EV,g);
            
            for( size_t k=0; k<EV.size() ; k++ ) {
                
                int Rk = 0.5 + ( EV[k]-EV[0] )/dE * getUnits()->convert( (*motion)->getUnits()->getEnergy() , 1 ) + 1;
                T[Rk] = AT[Rk] = g[k];
            }
            
            //--------------Computation of the convolution product-----------//
            
            for( ++motion; motion != Motions.end() ; ++motion) {
                
                if( (*motion)->isActivated() ) {
                    
                    if( (*motion)->getUnits() != getUnits() ) throwUnitsException("void msMotionsContainer::computeDOS()");
                    
                    (*motion)->FillEvAndG(EV,g);
                    
                    LOGGER_WRITE(msLogger::DEBUG,"DOS Convolution with the motion " +(*motion)->getId() );
                    
                    for( size_t k=1; k<EV.size() ; k++ )  {
                        
                        int Rk = 0.5 + ( EV[k]-EV[0] )/dE * getUnits()->convert( (*motion)->getUnits()->getEnergy() , 1 );
                        
                        for( int I = 1 ; I< M - Rk ; I++)  AT[Rk+I] += g[k]*T[I];
                    }
                    for( int I = 1 ; (I < M) && (EV.size()>0) ; I++)  T[I] = AT[I];
                    
                    LOGGER_WRITE(msLogger::DEBUG,"Done with " +(*motion)->getId() );
                }
            }
            (*VibDensOfStates)[0] = T[1]/dE ;
            (*VibSumOfStates)[0]  = T[1] ;
            
            for( int i=1 ; i<M-1 ; i++){
                
                (*VibDensOfStates)[i] = T[i+1]/dE;
                (*VibSumOfStates)[i]  = (*VibSumOfStates)[i-1] + (*VibDensOfStates)[i]*dE;
            }
            /*
            if( (Rotor.getSharedPtr()) && (Rotor->isActivated()) ) { ConvolVibWithRot(); }
            else{
                if( !Rotor.getSharedPtr() ) LOGGER_WRITE(msLogger::DEBUG,"No overall rotation defined");
                
                if( (Rotor.getSharedPtr() ) && ( !Rotor->isActivated() ) )
                    LOGGER_WRITE(msLogger::DEBUG,"Overall rotation defined in the molecule");
	            
                (*DensOfStates)[0] = T[1]/dE ;
                (*SumOfStates)[0]  = T[1] ;
                
                for( int i=1 ; i<M-1 ; i++){
                    (*DensOfStates)[i] = T[i+1]/dE;
                    (*SumOfStates)[i] = (*SumOfStates)[i-1] + (*DensOfStates)[i]*dE;
                }
            }*/
            LOGGER_EXIT_FUNCTION2("void msMotionsContainer::computeDOS()");
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msMotionsContainer::ConvolVibWithRot() {
            
            LOGGER_ENTER_FUNCTION_DBG("void msMotionsContainer::ConvolVibWithRot()",
                                      getFullId());
            
            int     M    = Emax/dE + 1;
            vector<double> EV,g;
            vector<double> T(M,0);
            vector<double> AT(M,0);
            
            T[1] = AT[1] = (*VibSumOfStates)[0];
            for( int i=2 ; i<M ; i++) AT[i]=T[i]=(*VibDensOfStates)[i-1]*dE;
            
            if( Rotor.getSharedPtr() ) {
                
                Rotor->FillEvAndG(EV,g);
                LOGGER_WRITE(msLogger::DEBUG,"DOS Convolution with the rotor");
                
                for( size_t k=1; k<EV.size() ; k++ ) {
                    
                    int Rk = 0.5 + ( EV[k]-EV[0] )/dE * getUnits()->convert( Rotor->getUnits()->getEnergy() , 1 );
                    
                    for( int I = 1 ; I< M - Rk ; I++)  AT[Rk+I] += g[k]*T[I]; 
                }        
                for( int I = 1 ; (I < M) && (EV.size()>0) ; I++)  { T[I] = AT[I];}
            }
            (*DensOfStates)[0] = T[1]/dE ; 
            (*SumOfStates)[0]  = T[1] ;
            
            for( int i=1 ; i<M-1 ; i++){ 
                
                (*DensOfStates)[i] = T[i+1]/dE; 
                (*SumOfStates)[i] = (*SumOfStates)[i-1] + (*DensOfStates)[i]*dE; 
            }         
            
            LOGGER_EXIT_FUNCTION2("void msMotionsContainer::ConvolVibWithRot()");
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        ostream& msMotionsContainer::print(ostream& out) const  {
            
            msMotion::print(out);
            output::printHeader(out,"Motion container");
            return out;
        }
    }
}


