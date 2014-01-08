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


/*!
 * \file c_motion.h
 * \brief Base class for every type of motion
 * \author G.Reinisch
 * \date 2011
 */

#ifndef msMOTIONSCONTAINER_H
#define msMOTIONSCONTAINER_H

#include <msMotion.h>
#include <msRigidRotor.h>

namespace impact {
    
    namespace atomism {
        
        /*!\class msMotionsContainer
         * \brief Base class for every type of motion. Introduces the concept of partition function and DOS.
         *
         */
        class msMotionsContainer : public msMotion
        {
            
            /*! \class msMotionsContainerParams
             * \brief Parameters for msMotionsContainer
             *
             * No more defined than msMotionParams.
             */
            class msMotionsContainerParams : public msMotionParams
            {
                static msRegistrar Registrar;
                static msTreeMapper* createInstance() { return new msMotionsContainerParams; }
                
            public:
                
                void initialize() { msMotionParams::initialize();
                }
                
                static boost::shared_ptr<msMotionsContainerParams> New() {
                    
                    boost::shared_ptr<msMotionsContainerParams> T( new msMotionsContainerParams );
                    T->initialize(); T->update(); return T;
                }
                
                msMotionsContainerParams() : msMotionParams() {}
            };
            
        private:
            //! @Name from msRegistrer
            //@{
            static msRegistrar Registrar;
            static bool  isMotionsContainerdRegisteredInPython;
            static msRegister* createInstance() { return new msMotionsContainer; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void initialize(){
                
                msMotion::initialize();
                try{
                    declareChildren<msMotion>(Motions,"Motions");
                    VibDensOfStates = msVectorFit1d::New();
                    VibSumOfStates  = msVectorFit1d::New();
                }
                catch(msError& e){
                    
                    e.addContext("void msMotionsContainer::initialize()");
                    BOOST_THROW_EXCEPTION(e);
                }
            }
            
            void update(){
                
                msMotion::update();
                VibDensOfStates->set(0,Emax,dE,getUnits()->getEnergyStr(),getUnits()->getEnergyStr()+"^-1");
                VibSumOfStates->set(0,Emax,dE,getUnits()->getEnergyStr(),"");
                
                try{ boost::shared_ptr<msTreeMapper> ptr = getChild("Rotor");
                    declareChild<msRigidRotor>(Rotor,boost::static_pointer_cast<msRigidRotor>(ptr),"Rotor");
                }
                catch(msError& e){;}
            }
            
            
        public:
            
            msMotionsContainer() : msMotion() {
                constructVar("msMotionsContainer","MotionsContainer","motions_container");
            }
            
            static boost::shared_ptr<msMotionsContainer> New(){
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msMotionsContainer> New()","");
                LOGGER_WRITE(msLogger::DEBUG, "New msMotionsContainer object")
                
                boost::shared_ptr<msMotionsContainer> T( new msMotionsContainer );
                T->initialize();
                T->setParameters( msMotionsContainerParams::New() );
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msMotionsContainer> New()");
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                
                boost::shared_ptr<msMotionsContainer> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            
            boost::shared_ptr<msTreeMapper> addMotion(boost::shared_ptr<msMotion> motion);
            
            boost::shared_ptr<msTreeMapper> setRotor(boost::shared_ptr<msRigidRotor> rotor);
            
            double Q(double T);
            
            void computeDOS();
            
            
            double DOS( double E ) {
                
                if(DensOfStates->size() == 0 ) computeDOS();
                return (*DensOfStates)(E);
            };
            
            std::ostream& print(std::ostream& out) const;
            
        private:
            
            msChildren<msMotion> Motions;
            msChild<msRigidRotor> Rotor;
            
            void ConvolVibWithRot();
            
            //! Last VibDensOfStates vector computed; deprecated: use DOS() instead
            boost::shared_ptr<msVectorFit1d> VibDensOfStates;
            
            //! Last VibSumOfStates vector computed; deprecated: use SOS() instead
            boost::shared_ptr<msVectorFit1d> VibSumOfStates; 
            
            void throwUnitsException(string method){
                
                msError e("The motions container and the included motion have to share the same units system",
                          method,getFullId());
                BOOST_THROW_EXCEPTION(e);
            }
            
        };
        ostream& operator<<(ostream&,const msMotionsContainer&);
    }
}
#endif // msMOTION_H
