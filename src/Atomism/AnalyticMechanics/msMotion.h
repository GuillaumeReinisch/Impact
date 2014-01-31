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

#ifndef msMOTION_H
#define msMOTION_H

#include <msEntity.h>
#include <msVectorFit1d.h>
#include <msPhysicalInterface.h>

namespace impact {
    
    namespace atomism {
        
        /*!\class msMotion
         * \brief Base class for every type of motion. Introduces the concept of partition function and DOS.
         *
         * This virtual class defines all the commun attributes and functions related to a motion
         * for a statitical analysis. Everything here is based on Density Of States (DOS) and partition function.
         *
         * \section virtualMotion Most important virtual functions
         *
         * To derive a new type of motion from msMotion, there is only one virtual method that
         * need to be redefined:
         *	- void DOS(double E) : Density Of State of the motion at energy E
         *
         * The partition function can be computed from this method. However, it is
         * often possible to compute the partition function faster by direct method,
         * in this case you may redefine the function :
         *      - double Q(double T) : partition function of the motion at temperature T
         *
         * In the same vein, you may consider the reimplementation of:
         *      - double computeDOS() : compute the DOS from 0 to nDoF*Emax
         *
         * Also, if the entropy or the other TD functions have analytical expressions, you
         * can overide them.\n
         * The function Q(double T) is used by the class to compute all the thermodynamic properties
         * in the canonical ensemble (enthalpy, entropy ... ). The function DOS(double T) is
         * used to compute microcanonical properties.
         *
         * \section motionPMA Physics, Mathematics, Algorithms
         *
         * The partition function can be directly computed from the DOS by:
         \f[
         Q(T)= \int_{E=0}^\infty DOS(E) \exp(- E / (RT) ) dE
         \f]
         *
         * The thermodynamic functions are computed using standart relations from the partition
         * function. The following expressions are expressed by molar quantity.
         *
         * The entropy of the motion is given by :
         \f[
         S(T)=R \left( ln(Q) + T \left( \frac{\partial ln(Q)}{dT} \right)_{V} \right)
         \f]
         * The thermic energy is given by :
         \f[
         E_{Therm}(T) = RT^2 \left( \frac{\partial ln(Q)}{dT} \right)_{V}
         \f]
         *
         * The volumetric calorific capacity is given by :
         \f[
         C_v(T) = RT^2 \left( \frac{\partial E_{Therm}}{dT} \right)_{V}
         \f]
         *
         * see also: http://en.wikipedia.org/wiki/Partition_function_%28statistical_mechanics%29.
         *
         * The different parameters used for the calculations are described in the msMotion::msMotionParams class.
         */
        class msMotion : public msPhysicalInterface
        {
            public:
            
            /*!\class msMotionParams
             * \brief Parameters of msMotion: dT, dE, Emax, dE_dos, Tmin, Tmax, DeltaT.
             *
             * The parameters defined are:
             *     - dT : delta temperature for derivation (like in dQ(T)/dT)
             *     - dE : energy grain
             *     - dE_dos : energy interval for DOS calculation, see msMotion::computeDOS()
             *     - Tmin : minimum temperature considered (not yet used in msMotion)
             *     - Tmax : maximum temperature considered (not yet used in msMotion)
             *     - DeltaT : temperature step (not yet used in msMotion)
             */
            class msMotionParams : public msParamsManager
            {
                
                static msRegistrar Registrar;
                static msTreeMapper* createInstance() { return new msMotionParams; }
                
                public:
                
                void initialize() {
                    
                    msParamsManager::initialize();
                    addParameter("dT","0.1","delta temperature for derivation ",msUnit("K"));
                    addParameter("dE","0.05","energy grain ",msUnit("kcal/mol"));
                    addParameter("Emax","20","maximum energy considered by DoF",msUnit("kcal/mol"));
                    addParameter("dE_dos","2","energy interval for DOS calculation",msUnit("kcal/mol"));
                    addParameter("Tmin","100","minimum temperature",msUnit("K"));
                    addParameter("Tmax","3000","maximum temperature",msUnit("K"));
                    addParameter("DeltaT","100","temperature step",msUnit("K"));
                }
                
                static boost::shared_ptr<msMotionParams> New(){
                    
                    boost::shared_ptr<msMotionParams> T( new msMotionParams );
                    T->initialize(); T->update();
                    return T;
                }
                
                msMotionParams() : msParamsManager() {}
            };
            
            private:
            //! @Name from msRegistrer
            //@{
            static msRegistrar Registrar;
            static bool  isMotionRegisteredInPython;
            static msRegister* createInstance() { return new msMotion; }
            //@}
            
            protected:
            
            void registryInPython();
            
            void initialize(){
                
	        IMPACT_LOGIN();
                msPhysicalInterface::initialize();
		
		msTreeMapper::declareAttribute( Sym , "Sym" );
                msTreeMapper::declareAttribute( Mult , "Mult");
                msTreeMapper::declareAttribute( Activated , "Activated" );
                DensOfStates = msVectorFit1d::New();
                SumOfStates  = msVectorFit1d::New();
		
		IMPACT_LOGOUT();
            }
            
            void update(){
                IMPACT_LOGIN();
                msPhysicalInterface::update();
		IMPACT_LOGOUT();
                //msMotion::updateParameters();
            }
            
            void updateUnits(msUnitsManager& Old,msUnitsManager& New) {
                
                msPhysicalInterface::updateUnits(Old,New);
                initLocalParameters();
            }
            public:
            
            msMotion() : msPhysicalInterface() , Sym(1), Mult(1), Activated(1), nDoF(0) {
                constructVar("msMotion","Motion","virtual_motion");
            }
            
            static boost::shared_ptr<msMotion> New(){
                
                IMPACT_LOGIN_STATIC();
                boost::shared_ptr<msMotion> T( new msMotion );
                T->initialize();
                T->setParameters( msMotionParams::New() );
                T->update();
                IMPACT_LOGOUT();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                
                boost::shared_ptr<msMotion> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            //! @name Important virtual functions
            //@{
            //! Partition function at temperature T
            virtual double Q(double T);
            
            //! Compute the DOS from 0 to Emax every dE_dos
            virtual void computeDOS();
            
            
            //! Density of states at energy E
            virtual double DOS( double E ){
                IMPACT_THROW_EXCEPTION( msException("msMotion does not implement DOS"
                                               ,"virtual double DOS(double E)",getFullId()) );
            };
            
            //  virtual double ZPE() {return 0;} //!<\brief Zero point energy
            //@}
            
            bool isActivated() const {return Activated; }
            
            //! \brief return the number of degrees of freedom described by the motion
            size_t noOfDof() { return nDoF; };
            
            //! \brief return the symmetry number of the motion
            size_t noOfSymmetry() { return Sym; };
            
            //! \brief return the multiplicity of the motion
            size_t noOfMultiplicity() { return Mult; };
            
            double SOS( double E );		//!< Sum of states at energy E
            
            double QfromDOS(double T); 	//!< Partition function from the density of state at temperature T
            
            virtual double Eav(double T); 		//!< Thermal energy at temperature T
            
            virtual double S(double T);	//!< Entropy at temperature T
            
            virtual double Cv(double T);		//!< Calorific capacity at cst volume at temperature T
            
            virtual double Cp(double T);		//!< Calorific capacity at cst pressure at temperature T
            
            //! Partition function at temperature T for a particular DOS, the first variable of the function DOS is interpreted as energy.
            double QfromDOS( msScalarFunction& DOS , double T );
            
            
            //! \brief Return the partition function w/ temperature
            boost::shared_ptr<msVectorFit1d> getQ() {
                
                boost::shared_ptr<msVectorFit1d> Qfct = msVectorFit1d::New();
                Qfct->set(Tmin,Tmax,DeltaT,getUnits()->getTempStr(),"");
                
                for(size_t i=1;i<Qfct->size(); i++)
                (*Qfct)[i] = Q(Tmin+i*DeltaT);
                
                return Qfct;
            };
            
            //! \brief Return the partition function computed from the DOS w/ temperature
            boost::shared_ptr<msVectorFit1d> getQfromDOS() {
                
                boost::shared_ptr<msVectorFit1d> Qfct = msVectorFit1d::New();
                Qfct->set(Tmin,Tmax,DeltaT,getUnits()->getTempStr(),"");
                
                for(size_t i=1;i<Qfct->size(); i++)
                (*Qfct)[i] = QfromDOS(Tmin+i*DeltaT);
                
                return Qfct;
            };
            
            //! \brief Return the entropy w/ temperature
            boost::shared_ptr<msVectorFit1d> getS() {
                
                boost::shared_ptr<msVectorFit1d> fct = msVectorFit1d::New();
                fct->set(Tmin,Tmax,DeltaT,getUnits()->getTempStr(),getUnits()->getStr(msUnit::vEntropyByQuantity));
                fct->getCoordinates()->getVariable(0)->setId("T");
                for(size_t i=1;i<fct->size(); i++)
                (*fct)[i] = S(Tmin+i*DeltaT);
                
                return fct;
            };
            
            //! \brief Return the Cp w/ temperature
            boost::shared_ptr<msVectorFit1d> getCp() {
                
                boost::shared_ptr<msVectorFit1d> fct = msVectorFit1d::New();
                fct->set(Tmin,Tmax,DeltaT,getUnits()->getTempStr(),getUnits()->getStr(msUnit::vEntropyByQuantity));
                fct->getCoordinates()->getVariable(0)->setId("T");
                for(size_t i=1;i<fct->size(); i++)
                (*fct)[i] = Cp(Tmin+i*DeltaT);
                
                return fct;
            };
            
            //! Return the last DOS computed (from 0 to Emax*nDoF)
            boost::shared_ptr<msVectorFit1d> getDOS() { return DensOfStates; };
            
            //! Return the last SOS computed (from 0 to Emax*nDoF)
            boost::shared_ptr<msVectorFit1d> getSOS() { return SumOfStates; };
            
            
            std::ostream& print(std::ostream& out) const;
            
            //! Return the total maximum energy (Emax*nDoF)
            double getEmax(){ return nDoF*Emax; }
            
            //! Set the symmetry number
            boost::shared_ptr<msTreeMapper> setSymmetryNumber(size_t sym) { Sym=sym; return mySharedPtr(); }
            
            //! Set the multiplicity
            boost::shared_ptr<msTreeMapper> setMuliplicity(size_t mult)   { Mult=mult; return mySharedPtr(); }
            
            /** \brief Pseudo classical spectrum computed from the DOS
             *
             * The vector eigenvalues is EigenValues[i]=i*dE, and g[i]=DOS(i*dE).
             * This method should be reimplemented for quantum motion to have exact (and faster) results.
             * \param EigenValues the list of eigenvalues to fill
             * \param g the list of degenrancy of each eigenvalues
             */
            virtual void FillEvAndG(vector<double>& EigenValues,vector<double>& g);
            
            protected:
            
            size_t Sym;	//!< Symmetry number, interpretation can depend of the derived class
            
            int Mult;         //!< Multiplicity: multiplicity of the configurational domain integration,
            
            /** \brief Derivative of log(Q(T)) at constant volume
             *
             * This function requiered as the translational partition function is volume dependant
             * \param T temperature in Kelvin
             * \return partition function
             */
            virtual double dlogQ_dT_Vcst(double T);
            
            
            void fillSOSfromDOS();    //!< Fill DensOfStates from SumOfStates
            
            bool Activated;		//!< Motion not taken into account by a msMolec class if 0
            
            int nDoF;			//!< Number of degrees Of freedom
            
            void updateParameters(); 	//!< Update the frequently used parameters
            
            //! @name msMotionParams converted to the good units, do not change their values in derived classes. see msMotionParams.
            //@{
            double dE,Emax,dE_dos,dT,Tmin,Tmax,DeltaT;
            //@}
            
            //! Last DensOfStates vector computed; deprecated: use DOS() instead
            boost::shared_ptr<msVectorFit1d> DensOfStates;
            
            //! Last SumOfStates vector computed; deprecated: use SOS() instead
            boost::shared_ptr<msVectorFit1d> SumOfStates;
            
            private:
            
            //! Energy coordinates, used in DOS and SOS scalar function for instance
            boost::shared_ptr<msScalarVariable> EnergyCoordinate;
            
            //! Temperature coordinates, used in Q() for instance
            boost::shared_ptr<msScalarVariable> TemperatureCoordinate;
            
            bool initLocalParameters() {
                
                try{
                    msPhysicalInterface::getParameters()->queryDoubleValue("dE", dE, *getUnits() );
                    msPhysicalInterface::getParameters()->queryDoubleValue("Emax", Emax, *getUnits() );
                    msPhysicalInterface::getParameters()->queryDoubleValue("dT", dT, *getUnits() );
                    msPhysicalInterface::getParameters()->queryDoubleValue("dE_dos", dE_dos, *getUnits() );
                    msPhysicalInterface::getParameters()->queryDoubleValue("Tmin", Tmin, *getUnits() );
                    msPhysicalInterface::getParameters()->queryDoubleValue("Tmax", Tmax, *getUnits() );
                    msPhysicalInterface::getParameters()->queryDoubleValue("DeltaT", DeltaT, *getUnits() );
		            DensOfStates->set(0, Emax, dE, getUnits()->getEnergyStr(), getUnits()->getEnergyStr()+"^-1" );
                    SumOfStates->set(0,Emax,dE, getUnits()->getEnergyStr(), "" );
                }
                catch( msException& e) {
                    
                }
                return 1;
            }
        };
        ostream& operator<<(ostream&,const msMotion&);
    }
}
#endif // msMOTION_H
