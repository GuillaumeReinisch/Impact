/*
 <one line to give the library's name and an idea of what it does.>
 Copyright (C) 2013  Guillaume <email>
 
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


#ifndef MSSOLVERLAGRANGIAN_H
#define MSSOLVERLAGRANGIAN_H

#include<msLagrangian.h>
#include<msBaseTrackers.h>
#include<msSolver.h>
#include<math.h>


namespace impact {
    
    namespace atomism {
        
        /** \class msSolverLagrangian
         *  \brief Compute the dynamic of a Lagrangian motion
         *
         * The system of ODE of the Lagrangian (see msLagragian) is solved using the 4 order
         * Runge Kunta (based on Peter Selinger code and updated by Norman Hardy).
         * Based on ideas and code by Peter Lynch, Met Eireann, Glasnevin Hill, Dublin. \n
         * The Lagrangian equations of motion are
         * \f[
         \[ \frac{d}{dt}\frac{\partial L}{\partial{qp}_i} =
         \frac{\partial L}{\partial q_i} \]
         \f]
         * or equivalently
         * \f[
         \[ \sum_j\frac{\partial^2 L}{\partial {qp}_i\partial{qp}_j}
         \qpp_j+\sum_j\frac{\partial^2 L}{\partial {qp}_i\partial
         q_j} {qp}_j=\frac{\partial L}{\partial q_i}.
         \]
         \f]
         * Here we assume that \f$ L \f$ has no explicit time dependency, although it would be
         * easy to add that feature. There is one equation for each unconstrained coordinate
         * \f$ i \f$. The above is a system of linear equations  \f$ A.qpp+B.qp=v \f$, where
         \f[
         \[  A_{ij}=\frac{\partial^2 L}{\partial {qp}_i\partial {qp}_j},\
         B_{ij}=\frac{\partial^2 L}{\partial {qp}_i\partial q_j},\
         v_i=\frac{\partial L}{\partial q_i}.
         \]
         \f]
         * Here, \f$ i \f$ ranges over unconstrained coordinates, and \f$ j \f$ over all
         * coordinates. At a given instant, \f$ q_j \f$ and \f$ qp_j \f$ are known, and
         * so is \f$ qpp_j \f$ for any constrained coordinate \f$ j \f$. The unknowns
         * are \f$ qpp_i \f$, where \f$ i \f$ is unconstrained. \n
         * Issues can arise when dynamic of a system described by a Z-Matrice in which
         * a bending and dihedral angles of a same atom are defined as coordinates. Indeed,
         * when the system is close to a bending angle=0 or 180 degrees the determinant
         * of the K-Matrice become null. The ODEs are hard to solve close to these points.
         */
        class msSolverLagrangian : public msSolver
        {
        protected:
            
            enum ErrorSolver{ AijError, };
            
            class msSolverLagrangianParams : public msSolverParams
            {
                static msRegistrar Registrar;
                static msTreeMapper* createInstance() { return new msSolverLagrangianParams; }
                
            public:
                
                void initialize()
                {   msSolverParams::initialize();
                    
                    addParameter("timeEnd","1","elementary time step, fraction of the Lagragian's time unit",msUnit(""));
                    addParameter( "noOfStep", "100" , "number of measure" , msUnit("") );
                }
                
                static boost::shared_ptr<msSolverLagrangianParams> New(){
                    
                    boost::shared_ptr<msSolverLagrangianParams> T( new msSolverLagrangianParams );
                    T->initialize();
                    T->update();
                    return T;
                }
            };
        private:
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isSolverLagrangianRegisteredInPython;
            static msRegister* createInstance() { return new msSolverLagrangian; }
            //@}
        protected:
            
            
            void registryInPython();
            
            void initialize(){
                msSolver::initialize();
                msTreeMapper::declareChild<msLagrangian>( Lagrangian, msLagrangian::New(), "Lagrangian");
            }
            
            void update(){msTreeMapper::update();
            }
            
        public:
            
	        msSolverLagrangian() : msSolver() { constructVar("msSolverLagrangian","SolverLagrangian","lagrangian solver");
                Ndof=0; t_current=0; t_tot=100; SiUnits.setSI();
            }
            
            static boost::shared_ptr<msSolverLagrangian> New() {
                boost::shared_ptr<msSolverLagrangian> T( new msSolverLagrangian );
                T->initialize();
                T->setParameters( msSolverLagrangianParams::New() );
                T->update();
                return T;
            }
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msSolverLagrangian> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            /*! \brief run the simulation
             */
            boost::shared_ptr<msTreeMapper> run();
            
            /*! \brief set the Lagrangian
             *
             * \param L Lagrangian
             */
            boost::shared_ptr<msTreeMapper> setLagrangian(boost::shared_ptr<msLagrangian> L){
                
                msTreeMapper::update(Lagrangian,L);
                return mySharedPtr();
            }

            /*! \brief return the Lagrangian
             */
            boost::shared_ptr<msLagrangian> getLagrangian(){
                
                return Lagrangian.getSharedPtr();
            }
            
            /*! \brief initialize the generalized velocities randomly
             *
             * \param E kinetic energy targeted, in the Lagrangian units
             */
            boost::shared_ptr<msTreeMapper> initializeVelocities_random( double E);
            
            boost::shared_ptr<msTreeMapper> step();
            
            std::ostream& print(std::ostream& out) const;
            
            
        private:
            
            msChild<msLagrangian>            Lagrangian;
            //msChildren<msCoordinateTracker>  Coordinates;
            msChild<msTrackerScalarFunction> TotalEnergies;
            msChild<msTrackerScalarFunction> KineticEnergies;
            msChild<msTrackerScalarFunction> PotentialEnergies;
            msChild<msTrackerScalarFunction> Action;
            
            vector<double> dofs;
            
            void resize(int n);
            
            ostream& printState(ostream& ) const;
            ostream& printHeader(ostream& ) const;
            void printError(ErrorSolver error ) const;
            
            void step(double dt);
            
            msUnitsManager SiUnits;
            //!\name Registred in the tree
            //@{
            
            double dEk;
            double t_tot;
            
            double Ek,Ep,Et,L;
            //@}
            
            //! @name Variables used by the classe for the calculations NOTE: everything is in SI units
            //@{
            double t_current;
            vector_type q;
            vector_type qp;
            
            vector_type Epsilon;
            vector_type Epsilonp;
            
            vector_type K_0;
            vector_type K_1;
            vector_type K_2;
            vector_type K_3;
            vector_type Kp_0;
            vector_type Kp_1;
            vector_type Kp_2;
            vector_type Kp_3;
            
            vector_type rhs;
            matrix_type A;
            //@}
            double diffq(int i, vector_type& q_, vector_type& qp_);
            double diffpp(int i, int j, vector_type& q_, vector_type& qp_);
            double diffqp(int i, int j, vector_type& q_, vector_type& qp_);
            
            void calc_qpp(vector_type& q_,vector_type& qp_, vector_type& qpp_);
            void solve(matrix_type& A, vector_type& v, vector_type& w);
            
            int Ndof;
            
        };
        
    }
}
#endif // MSSOLVERLAGRANGIAN_H
