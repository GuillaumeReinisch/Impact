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
 *
 * \author G. Reinisch, based, some times ago, on code of Peter Selinger, updated by Norman Hardy for the dynamic part.
 * \see http://cap-lore.com/MathPhys/DynamJavaDemo/ for the original source
 */

#ifndef msMECHSYSTEM_H
#define msMECHSYSTEM_H

#include <msMotion.h>
#include <msKineticOperator.h>
#include <msSamplerLinear.h>
#include <msIntegrandLagrangian.h>

//#include <boost/numeric/odeint.hpp>

namespace impact {
    
    namespace atomism {
        
        class msSolverLagrangian;
        
        /*! \class msLagrangian
         * \brief Base class for classical mechanical system. Use a Lagrangian representation, introduce
         * the concept of PES, Kinetic energy, generalized coordinates and the associated equations of motion.
         *
         * This is the base class of all the motions involving the motions of ponctual
         * masses. The system statistic is studied using the Lagrangien
         * formalism. It introduces the concept of kinetic energy (msKineticOperator), potential energy
         * (msScalarFunction) and generalized coordinates (msGeneralizedCoordinates).
         * The sampling of the configurational space is realized by msSampler derived classes.
         * For dynamic solver, see msSolverLagrangian derived classes.
         *
         * \section lagrangePMA Physics, Mathematics, Algorithms
         *
         * The Lagrangian of an atomic systems of generalized coordinates \f$\mathbf{q}\f$ and
         * generalized velocities \f$\mathbf{qp}\f$ is given by:
         * \f[
         L(\mathbf{q},\mathbf{\dot{q}})=T(\mathbf{q},\mathbf{qp})+V(\mathbf{q})
         \f]
         * where  \f$ V(\mathbf{q}) \f$ is the electronic PES and \f$ T(\mathbf{q},\mathbf{qp}})\f$
         * the kinetic energy, given by the relation:
         * \f[
         T(\mathbf{q},\mathbf{qp})=\frac{1}{2}\sum_{at} m_{at}\left(\sum_{i}
         \frac{\partial \vec{R}_{at}}{\partial q_i}qp_i\right)^2 \\
         =\frac{1}{2}\mathbf{qp^T}\mathbf{\underline{A}}
         \left(\mathbf{q}\right)\mathbf{qp}
         \f]
         * The matrix \f$ \underline{A} \f$  is called the kinetic matrix, its elements are
         * defined by:
         * \f[
         A_{ij}=\frac{1}{2}\sum_{at} m_{at} \frac{\partial \vec{R}_{at}}{\partial q_i}.
         \frac{\partial \vec{R}_{at}}{\partial q_j}
         \f]
         *
         * The theorem of Aston and Eidinoff allows a computation of the partition function
         * through a configurational integral involving the determinant of the kinetic matrix
         * and the PES:
         * \f[
         Q(T)=\frac{1}{\delta}\left(2\pi k_BT\right)^{n/2} \int |\underline{A}(\mathbf{q})|^{1/2}
         \exp\left(-V(\mathbf{q})/k_B T\right)d\mathbf{q}
         \f]
         * where \f$ k_B \f$ is  the Boltzmann constant, \f$ T \f$ the temperature,
         \f$ n \f$ the dimension of \f$ \mathbf{q} \f$, and \f$ \delta \f$  the symmetry
         * number of  the motion  (see  the work  of  Fernandez-Ramos \e et \e al ).
         *
         * Applying the inverse Laplace tranform (see work of Robertson \e et \e al ),
         * the DOS can be obtained:
         * \f[
         g(E)=\frac{1}{\delta \left(N_{av}\right)^{n/2}\Gamma(n/2) } \left(\frac{2\pi}{h^2}\right)^{n/2}
         \int |\underline{A}(\mathbf{q})|^{1/2}\left(E-V(\mathbf{q})\right)^{n/2-1}d\mathbf{q}
         \f]
         * where \f$ N_{av} \f$ is the Avogadro number, \f$ h \f$ the planck constant and \f$ \Gamma \f$
         * the gamma function;  \f$ E \f$ and \f$ V \f$ being expressed using molar quantity.
         *
         *
         * For more details on the configurational integration formulation see ref @ref Aston_JCP_1935 \n
         * For more details on the computation of the kinetic matrix see ref @ref Reinisch_JCP2010. \n
         * For more details on the symmetry number see ref @ref Fernandez_TCA_2007. \n
         * For more details on how this apply to the computation of reaction rate see ref  @ref Robertson_JCP_2000.
         *
         */
        class msLagrangian : public msMotion
        {
            friend class msSolverLagrangian;
            
        protected:
            
            class msLagrangianParams : public msMotionParams
            {
                static msRegistrar Registrar;
                static msTreeMapper* createInstance() { return new msLagrangianParams; }
                
            public:
                static boost::shared_ptr<msLagrangianParams> New() {
                    
                    boost::shared_ptr<msLagrangianParams> T( new msLagrangianParams );
                    T->initialize(); T->update();
                    return T;
                }
                msLagrangianParams() : msMotionParams() {}
            };
            
            
        private:
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isLagrangianRegisteredInPython;
            static msRegister* createInstance() { return new msLagrangian; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void initialize();
            
            void update();
            
            void updateUnits(msUnitsManager& Old,msUnitsManager& New) {
                
                msMotion::updateUnits(Old,New);
                initializeGeneralizedVelocities();
            }
            
        public:
            
            
            static boost::shared_ptr<msLagrangian> New() {
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msLagrangian> New()","");
                LOGGER_WRITE(msLogger::DEBUG, "New msLagrangian object")
                
                boost::shared_ptr<msLagrangian> T( new msLagrangian );
                T->initialize();
                T->setParameters( msLagrangianParams::New() );
                T->update();
                
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msLagrangian> New()");
                return T;
            }
            virtual boost::shared_ptr<msTreeMapper> clone() {
                
                boost::shared_ptr<msLagrangian> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            msLagrangian() : msMotion() {
                
                constructVar("msLagrangian","Lagrangian","lagrangian");
                Mult=Sym=1; OriginEnergy=0;
            }
            /*! \brief set the kineic operator and the potential energy
             *
             * The object have to share the same generalized coordinates.
             * If the two objects have different non void coordinates an exception is raised.
             * If one of the object has a void coordinates object, its coordinates obaject is set to the
             * other argument coordinates.
             * If both 'kin' adn 'epot' coordinates are void the the kinetic operator's coordinates container
             * is set to the one of the potential energy.
             *
             * \param kin kinetic operator
             * \param epot potential energy
             */
            boost::shared_ptr<msTreeMapper> setEqOfMotion( boost::shared_ptr<msKineticOperator> kin ,
                                                          boost::shared_ptr<msScalarFunction> epot
                                                          );
            
            double E();  		//!< the total energy for the current coordinates define by q and qp
            
            double U();  		//!< the potential energy at the current configuration
            
            double T();   		//!< the kinetic energy at the current state
            
            double L();   		//!< action at the current state
            
            double KinFct(); 	//!< the kinetic energy at the current configuration
            
            double DOS(double E);
            
            double Q(double T);
            
            /*double scanMinOfPes();
             double getOriginEnergy() const;*/
            
            void setOriginEnergy(double e) { OriginEnergy=e; }
            
            //! return the kinetic operator
            boost::shared_ptr<msKineticOperator> getKinetic() 	const {
                return KineticOperator.getSharedPtr();}
            
            //! return the sampler used for configurational space integration
            boost::shared_ptr<impact::msSampler> getSampler()	const {
                return Sampler.getSharedPtr();}
            
            //! return the generalized coordinates
            boost::shared_ptr<msGeneralizedCoordinates> getGeneralizedCoordinates() const {
                return GeneralizedCoordinates.getSharedPtr();
            }
            
            //! return the generalized coordinates
            boost::shared_ptr<msGeneralizedCoordinates> getGeneralizedVelocities() const {
                return GeneralizedVelocities;
            }

            //! return the potential energy
            boost::shared_ptr<msScalarFunction> getEpot() const {
                return Epot.getSharedPtr();
            }

            //! return the potential energy
            boost::shared_ptr<msScalarFunction> getLagrangian() const {
                return Epot.getSharedPtr();
            }
            
            //! return the kinetic operator
            boost::shared_ptr<msKineticOperator> getKineticOperator() 	        const {return KineticOperator.getSharedPtr();}
            
            //boost::shared_ptr<msTreeMapper> setRnd_qp(double E);
            
            
            /** \brief Statistic value of an observable wrt to the temperature
             *
             * Average configuration dependant properties on configurational space using Boltzmann PDF wrt to the temperature.  of an observable
             * \param Observable : pointer to the obervable functionnoid.
             * \param T : temperature [K]
             */
            double averageByBoltzmann( msObservable& observable , double T );
            
            std::ostream& print(std::ostream& out) const;
            
            
            virtual bool sanityCheck();
            
        private:
            
            void initializeGeneralizedVelocities();
            
            msChild<msScalarFunction> Epot;   				//!<  Potential energy surface
            
            msChild<msGeneralizedCoordinates> GeneralizedCoordinates; 	//!<  Generalized coordinates
            
            msChild<msKineticOperator> KineticOperator; 	//!<  Kinetic operator
            
            msChild<msSampler> Sampler; 					//!<  Sample the configurational space
            
            boost::shared_ptr<msGeneralizedCoordinates> GeneralizedVelocities; 	//!<  Generalized velocities
            
            vector_type q;
            vector_type qp;
            
        protected:
            
            double OriginEnergy;
            
            /** @name Functions for the Lagrangian evaluation
             *
             * These functions compute the different energy (kinetic potential, action).
             * NOTE: They use SI units and not the 'Units' attribut to avoid conversion
             * q=positions, qp=velocities
             */
            //@{
            
            double T(vector_type q, vector_type qp); //!< \brief the kinetic energy
            
            double L(vector_type q, vector_type qp, bool compkmat); //!< \brief the kinetic energy
            
            double U(vector_type q); 		 //!< \brief the potential energy
            
            double E(vector_type q, vector_type qp); //!< \brief the total energy
            
            double L(vector_type q, vector_type qp);//!< \brief Action
            
            void computeAll(vector_type q, vector_type qp,double& E,double& Ep,double& Ek,double& L);
            
            void updateCoorsAndVelocitiesFromSi(vector_type& q, vector_type& qp);
            
            //boost::shared_ptr<msTreeMapper> setRnd_qp(double E, vector_type& qp);
            //@}
            
            boost::shared_ptr<msUnitsManager> UserUnits;  //!< The user defined units
            
            double MinOfPes;
        };
        
    }
    
}
//}ATOMIMS

#endif // msMECHSYSTEM_H
