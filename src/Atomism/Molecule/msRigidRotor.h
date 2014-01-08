/*
 <one line to give the program's name and a brief idea of what it does.>
 Copyright (C) <2011>  <Reinisch>
 
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
 * \file msRigidRotor.h
 * \brief Independant 3D rigid rotor model
 * \author G.Reinisch
 * \date 2011
 */

#ifndef msRIGROTOR3D_H
#define msRIGROTOR3D_H

#include <msMotion.h>
#include <msSystem.h>
#include <msQuantumInterface.h>

namespace impact {
    
    namespace atomism {
        
        /*! \class msRigidRotor
         * \brief Rigid rotor model for overall rotational motion
         *
         * The DOS and partition function obtained using standart statistical relations.
         * The class introduces the concept of overall rotation quantum number \f$ J \f$ , although the
         * energy spectrum w/ \f$ J \f$ is not known in this general case ( but in the case of
         * msLinearRigidRotor and msSymmetricRigidRotor2D it is).
         *
         * \section rotorPMA Physics, Mathematics, Algorithms
         *
         * Standart relations are used to compute the classical partition function,
         * based on the principal moment of inertia \f$ I_i \f$ of the molecule:
         * \f[
         q_r=(\pi.I)^{0.5}\left((8\pi^2k_bT)/(h*h)\right)^{1.5}/\sigma
         \f]
         where \f$ I= \Pi_i I_i \f$ , \f$ k_b \f$ the Boltzmann factor, \f$ h \f$ the
         * planck constant and \f$ \sigma \f$ is the symmetry number.
         * These moment are calculated by the class msSystem.
         * see reference @ref Fernandez_TCA_2007 for a through discussion about the
         * symmetry number.
         *
         * \todo find a way to approximate the spectrum w/ J for the general case see http://en.wikipedia.org/wiki/Rotational_spectroscopy
         */
        class msRigidRotor : public  msMotion , public msQuantumInterface
        {
            
        protected:
	  
            /*! \class msRigidRotorParams
             * \brief Parameters for rigid rotor computations
             *
	     * No more defined than msMotionParams.
	     */
            class msRigidRotorParams : public msMotionParams
            {
                static msRegistrar Registrar;
                static msTreeMapper* createInstance() { return new msRigidRotorParams; }
                
            public:
                
                void initialize() { msMotionParams::initialize();
                }
                
                static boost::shared_ptr<msRigidRotorParams> New() {
		  
		    boost::shared_ptr<msRigidRotorParams> T( new msRigidRotorParams );
                    T->initialize(); T->update(); return T;
                }
                
                msRigidRotorParams() : msMotionParams() {}
            };
            
            
        private:
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool        isRigidRotordRegisteredInPython;
            static msRegister* createInstance() { return new msRigidRotor; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void updateParameters();
            
        public:
            
            void initialize(){ 
	      msMotion::initialize();
	      declareAttribute<double>(I1,"I1");
	      declareAttribute<double>(I2,"I2");
	      declareAttribute<double>(I2,"I3");
	      }
            
            void update(){ 
	      
	      msMotion::update(); 
	      msQuantumInterface::R = getUnits()->convert(msUnit::J_mol,csts::R);
	      declarePhysicalVariable(getUnits()->getUnit(msUnit::vMomentOfInertia), &I1);
	      declarePhysicalVariable(getUnits()->getUnit(msUnit::vMomentOfInertia), &I2);
	      declarePhysicalVariable(getUnits()->getUnit(msUnit::vMomentOfInertia), &I3);
            }
            
            static boost::shared_ptr<msRigidRotor> New( ) {
                
                boost::shared_ptr<msRigidRotor> T( new msRigidRotor );
                T->initialize(); 
                T->setParameters( msRigidRotorParams::New() );
                T->update();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msRigidRotor> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            msRigidRotor() : msMotion() , msQuantumInterface(),I1(-1),I2(-1),I3(-1) { constructVar("msRigidRotor","RigidRotor","rigid rotor");
            }
            
            boost::shared_ptr<msTreeMapper> setMmtOfInertia( boost::shared_ptr<msSystem> syst );
            
            double Q(double T);
            
            double ZPE(){ return msQuantumInterface::ZPE();
            };
            
            
            void ComputeDOS(){ computeEigenValues();  fillDOS(*DensOfStates);   fillSOSfromDOS();
            };
            
            virtual double Qj(size_t J, double T) {
                BOOST_THROW_EXCEPTION( msError("This method needs to be implemented"
                                               ,"virtual double Q(size_t J,double T)",  getFullId()) );
            };
            
            virtual void DOSj(size_t J, double E){
                BOOST_THROW_EXCEPTION( msError("This method needs to be implemented"
                                               ,"virtual double DOS(size_t J,double E)",getFullId()) );
            };
            
            virtual void computeDOSj(size_t J){
                BOOST_THROW_EXCEPTION( msError("This method needs to be implemented"
                                               ,"virtual double computeDOS(size_t J)",  getFullId()) );
            };
            
            double getI1(){return I1;}
            double getI2(){return I2;}
            double getI3(){return I3;}
            
            ostream& print(ostream& out) const;
            
            //   void getEigenInertia(double& I1, double& I2, double& I3) const;
            
        protected:
            
            
            double I1;
            
            double I2;
            
            double I3;
            
        };
        
        ostream& operator<<(ostream& out,const msRigidRotor& rotor);
    }
}

#endif // msRIGROTOR3D_H
