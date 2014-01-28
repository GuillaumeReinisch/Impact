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


#include "msSolverLagrangian.h"
namespace impact {
    
    namespace atomism {
        
        bool msSolverLagrangian::isSolverLagrangianRegisteredInPython=0;
        msRegistrar msSolverLagrangian::Registrar("msSolverLagrangian", msSolverLagrangian::createInstance);
        
        ostream& operator<<(ostream& out,const msSolverLagrangian& solver) {  solver.print(out); return out; }
        
        
        void msSolverLagrangian::registryInPython()
        {
#if USE_PYTHON
            msSolver::registryInPython();
            
            if( ! msSolverLagrangian::isSolverLagrangianRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msSolverLagrangian,bases<msSolver>,boost::shared_ptr<msSolverLagrangian> >
                ("msSolverLagrangian",
                 "Dynamic solver for mechanical systems. ",no_init )
                .def( "New",     &msSolverLagrangian::New ,     "Create a new object")
                .staticmethod("New")
                .def("run",    &msSolverLagrangian::run,    "run the dynamic." )
                .def("step",   (boost::shared_ptr<msTreeMapper>(msSolverLagrangian::*)() )
                     &msSolverLagrangian::step,    "step in time" )
                .def("setLagrangian",&msSolverLagrangian::setLagrangian,
                     "set the Lagrangian. arg2: Lagrangian")
                .def("getLagrangian",&msSolverLagrangian::getLagrangian,
                     "return the Lagrangian")
                .def("initializeVelocities_random",&msSolverLagrangian::initializeVelocities_random,
                     "Initialize the velocities randomly on a target kinetic energy. arg2: kinetic energy.");
                
                msSolverLagrangian::isSolverLagrangianRegisteredInPython=1;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msSolverLagrangian::resize(int n) {
            
            if( n == Ndof ) return;
            
            q.resize(n);    qp.resize(n);   Epsilon.resize(n); Epsilonp.resize(n);
            K_0.resize(n);  K_1.resize(n);  K_2.resize(n);     K_3.resize(n);
            Kp_0.resize(n); Kp_1.resize(n); Kp_2.resize(n);    Kp_3.resize(n);
            rhs.resize(n);
            A.resize(n,n);
            q=qp=Epsilon=Epsilonp=K_0=K_1=K_2=K_3=Kp_0=Kp_1=Kp_2=Kp_3=rhs=vector_type::Zero(n);
            A=matrix_type::Zero(n,n);
            Ndof=n;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        ostream& msSolverLagrangian::printState(ostream& out) const {
            
            size_t d=20;
            string v1=output::getString<double>(Lagrangian->getUnits()->convert(SiUnits.getEnergy(),Et));
            string v2=output::getString<double>(Lagrangian->getUnits()->convert(SiUnits.getEnergy(),Ep));
            string v3=output::getString<double>(Lagrangian->getUnits()->convert(SiUnits.getEnergy(),Ek));
            
            out<<std::setw(d)<<t_current<<std::setw(d)<<v1<<std::setw(d)<<v2<<std::setw(d)<<v3<<std::setw(d);
            
            msGeneralizedCoordinates::UnfreezIt qi = Lagrangian->GeneralizedCoordinates->beginUnfreezed();
            for(size_t i=0 ; qi!=Lagrangian->GeneralizedCoordinates->endUnfreezed(); ++qi,++i) {
                
                string v=output::getString<double>(Lagrangian->GeneralizedCoordinates->getValue(qi.toIterator(), *(Lagrangian->getUnits()) ));
                out<<std::setw(d)<<v<<std::setw(d)<<qp[i];
            }
            
            out<<"\n";
            return out;
        }
        
        ostream& msSolverLagrangian::printHeader(ostream& out) const {
            
            size_t d=20;
            out<<"Initial state prepared, all in SI units:\n"<<std::setw(d)<<"t"<<std::setw(d)<<"Etot"<<std::setw(d)<<"Epot"<<std::setw(d)<<"Ekin";
            for(size_t i=0 ; i < Ndof; i++){
                
                string qi="q"+output::getString<int>(i);string qpi="qp"+output::getString<int>(i);
                out<<std::setw(d)<<qi<<std::setw(d)<<qpi;
            }
            out<<endl;
            return out;
        }
        
        void msSolverLagrangian::printError( msSolverLagrangian::ErrorSolver error ) const {
            
            switch( error )
            {
                case AijError:
                    stringstream s;s<<"Problem with the computation of A: may be unadapted numerical derivative step size, you should increase it.";
                    for(int i=0;i<Ndof;i++){s<<"Epsilonp["<<i<<"]="<<Epsilonp[i];}
                    LOGGER_WRITE(msLogger::ERROR,s.str());
                    break;
            }
        }
        
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msSolverLagrangian::initializeVelocities_random(  double E)
        {
            LOGGER_ENTER_FUNCTION("boost::shared_ptr<msTreeMapper> msSolverLagrangian::initializeVelocities_random(  double E)",getFullId())
            
            Lagrangian->GeneralizedCoordinates->getUnfreezedValues(SiUnits,q);
            
            double Edisp = Lagrangian->SiUnits.convert( Lagrangian->getUnits()->getEnergy(), E) ;
            size_t nDof  = Lagrangian->GeneralizedCoordinates->noOfActive();
            
            Lagrangian->KineticOperator->computeKMat( Lagrangian->SiUnits );
            stringstream out;out<<"Edisp in SI: "<<Edisp<<" Kmat in Si: \n"<<Lagrangian->KineticOperator->getKMatrix();
            LOGGER_WRITE( msLogger::INFO , out.str() );
            
            qp = vector_type::Zero(nDof);
            
            for(size_t i=0 ; i < nDof; i++) {
                
                qp[i] = - sqrt( 2*(Edisp/nDof ) / ( Lagrangian->KineticOperator->getKMatrix()(i,i) ) );
                LOGGER_WRITE(msLogger::INFO,"qp"+output::getString<int>(i)+"="+output::getString<double>(qp[i])+" in SI");
            }
            Lagrangian->qp = qp;
            Lagrangian->updateCoorsAndVelocitiesFromSi(Lagrangian->q,Lagrangian->qp);
            LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msSolverLagrangian::initializeVelocities_random(  double E)");
            return mySharedPtr();
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msSolverLagrangian::run() {
            
            LOGGER_ENTER_FUNCTION("boost::shared_ptr<msTreeMapper> msSolverLagrangian::run()",getFullId())
            
            resize( Lagrangian->GeneralizedCoordinates->noOfActive() );
            Lagrangian->GeneralizedCoordinates->getUnfreezedValues( SiUnits , q );
            
            //setRnd_qp(*Lagrangian , E);
            msGeneralizedCoordinates::UnfreezIt qi = Lagrangian->GeneralizedCoordinates->beginUnfreezed();
            msGeneralizedCoordinates::UnfreezIt qpi = Lagrangian->getGeneralizedVelocities()->beginUnfreezed();
            
            for(size_t i=0 ; qi!=Lagrangian->GeneralizedCoordinates->endUnfreezed(); ++qi,++i) {
            
                Epsilon[i]  = Lagrangian->GeneralizedCoordinates->getdq(qi.toIterator(), SiUnits);
            }
            for(size_t i=0 ; qpi!=Lagrangian->getGeneralizedVelocities()->endUnfreezed(); ++qpi,++i) {
                
                Epsilonp[i] = Lagrangian->getGeneralizedVelocities()->getdq(qpi.toIterator(), SiUnits);
            }
            
            Lagrangian->computeAll(q,qp,Et,Ep,Ek,L);
            
            LOGGER_WRITE(msLogger::INFO,"Number of active Dof: "+output::getString<int>(Ndof));
            printHeader(cout);
            printState(cout);
            
            t_current = 0;
            t_tot  = msPhysicalInterface::getParameters()->getValue<double>("timeEnd", Lagrangian->getUnits());
            t_tot *= SiUnits.convert(Lagrangian->getUnits()->getTime(),1);
            int n = msPhysicalInterface::getParameters()->getValue<int>("nstep", Lagrangian->getUnits());
            double dt=t_tot/n;
            LOGGER_WRITE(msLogger::INFO,"End simulation time: "+output::getString<double>(t_tot)+ "s");
            LOGGER_WRITE(msLogger::INFO,"Step simulation time: "+output::getString<double>(dt)+ "s");
            
            ofstream outfile("vmd.xyz");
            
            for(int i=0;i<n;i++) {
                
                step(dt);
                // KineticWork = Lagrangian->moveSystemTo(q);
                Lagrangian->computeAll(q,qp,Et,Ep,Ek,L);
                printState(cout);
                
                if(i%20==0)
                {
                    Lagrangian->getKinetic()->getSystem()->writeXYZ(outfile,output::getString<double>(i*dt));
                }
            }
            outfile.close();
            
            for(size_t i=0 ; qi!=Lagrangian->GeneralizedCoordinates->endUnfreezed(); ++qi,++i)
                Lagrangian->qp[i] = qp[i];
            
            LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msSolverLagrangian::run()");
            return mySharedPtr();
        }

        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msSolverLagrangian::step() {
            
            LOGGER_ENTER_FUNCTION("boost::shared_ptr<msTreeMapper> msSolverLagrangian::run()",getFullId())
            
            double t_tot  = msPhysicalInterface::getParameters()->getValue<double>("timeEnd", Lagrangian->getUnits());
            t_tot *= SiUnits.convert(Lagrangian->getUnits()->getTime(),1);
            int n = msPhysicalInterface::getParameters()->getValue<int>("nstep", Lagrangian->getUnits());
            double dt=t_tot/n;
            step(dt);
            
            LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msSolverLagrangian::run()");
            return mySharedPtr();
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msSolverLagrangian::step(double dt) {
            
            vector_type qtmp(  Ndof );
            vector_type qptmp( Ndof );
            vector_type qpp(   Ndof );
            
            try { calc_qpp(q, qp, qpp);
            }
            catch (msException& e) {
                e.addContext("void msSolverLagrangian::step(double dt)");
                IMPACT_THROW_EXCEPTION(e);
            }
            
            for( int i=0; i<Ndof; i++) {
                
                K_0[i]  = dt * qp[i];
                Kp_0[i] = dt * qpp[i];
            }
            
            // calculate K1
            for( int i=0; i<Ndof; i++) {
                
                qtmp[i]  = q[i]  + 0.5 * K_0[i];
                qptmp[i] = qp[i] + 0.5 * Kp_0[i];
            }
            try { calc_qpp(qtmp, qptmp, qpp);
            }
            catch (msException& e) {
                e.addContext("void msSolverLagrangian::step(double dt) : calculation of K1");
                IMPACT_THROW_EXCEPTION(e);
            }
            
            for(int i=0; i<Ndof; i++) {
                
                K_1[i]  = dt * qptmp[i];
                Kp_1[i] = dt * qpp[i];
            }
            // calculate K2
            for( int i=0; i<Ndof; i++){
                
                qtmp[i]  = q[i]  + 0.5 * K_1[i];
                qptmp[i] = qp[i] + 0.5 * Kp_1[i];
            }
            
            try { calc_qpp(qtmp, qptmp, qpp);
            }
            catch (msException& e) {
                e.addContext("void msSolverLagrangian::step(double dt) : calculation of K2");
                IMPACT_THROW_EXCEPTION(e);
            }
            for (int i=0; i<Ndof; i++) {
                
                K_2[i]  = dt * qptmp[i];
                Kp_2[i] = dt * qpp[i];
            }
            // calculate K3
            for (int i=0; i<Ndof; i++) {
                
                qtmp[i]  = q[i] + K_2[i];
                qptmp[i] = qp[i] + Kp_2[i];
            }
            
            try { calc_qpp(qtmp, qptmp, qpp);
            }
            catch (msException& e) {
                e.addContext("void msSolverLagrangian::step(double dt) : : calculation of K3");
                IMPACT_THROW_EXCEPTION(e);
            }
            
            for( int i=0; i<Ndof; i++) {
                
                K_3[i]  = dt * qptmp[i];
                Kp_3[i] = dt * qpp[i];
            }
            // Advance step
            for( int i=0; i<Ndof; i++) {
                
                q[i]  += (  K_0[i] + 2*K_1[i]  + 2*K_2[i]  + K_3[i] ) / 6 ;
                qp[i] += ( Kp_0[i]+ 2*Kp_1[i] + 2*Kp_2[i] + Kp_3[i] ) / 6 ;
            }
            
            //q2Coordinfo();
            t_current += dt;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msSolverLagrangian::diffq(int i, vector_type& q_, vector_type& qp_)
        {
            double L0, L1;
            
            q_[i] +=  Epsilon[i];
            L1     =  Lagrangian->L(q_, qp_);
            q_[i] -=  2*Epsilon[i];
            L0     =  Lagrangian->L(q_, qp_);
            q_[i] +=  Epsilon[i];
            
            return (L1-L0)/(2*Epsilon[i]);
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msSolverLagrangian::diffpp(int i, int j, vector_type& q_, vector_type& qp_)
        {
            double L00, L01, L10, L11;
            
            double epsilonp_i=Epsilonp[i]; double epsilonp_j=Epsilonp[j];
            
            Lagrangian->GeneralizedCoordinates->setUnfreezedValues(SiUnits,q_);
            Lagrangian->KineticOperator->computeKMat( SiUnits );
            
            qp_[i] += epsilonp_i;
            qp_[j] += epsilonp_j;
            L11     = Lagrangian->L(q_, qp_, 0); qp_[j] -= 2*epsilonp_j;
            L10     = Lagrangian->L(q_, qp_, 0); qp_[i] -= 2*epsilonp_i;
            L00     = Lagrangian->L(q_, qp_, 0); qp_[j] += 2*epsilonp_j;
            L01     = Lagrangian->L(q_, qp_, 0); qp_[j] -= epsilonp_j;
            qp_[i] += epsilonp_i;
            
            return ((L11-L01)/(2*epsilonp_i)-(L10-L00)/(2*epsilonp_i))/(2*epsilonp_j);
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msSolverLagrangian::diffqp(int i, int j, vector_type& q_, vector_type& qp_) {
            
            double L00, L01, L10, L11;
            
            q_[i]  += Epsilon[i];
            qp_[j] += Epsilonp[j];
            Lagrangian->GeneralizedCoordinates->setUnfreezedValues(SiUnits,q_);
            Lagrangian->KineticOperator->computeKMat( SiUnits );
            L11    = Lagrangian->L(q_, qp_,0);
            
            qp_[j] -= 2*Epsilonp[j];
            L10    = Lagrangian->L(q_, qp_,0);
            
            q_[i]  -= 2*Epsilon[i];
            Lagrangian->GeneralizedCoordinates->setUnfreezedValues(SiUnits,q_);
            Lagrangian->KineticOperator->computeKMat( SiUnits );
            L00    = Lagrangian->L(q_, qp_,0);
            
            qp_[j] += 2*Epsilonp[j];
            L01    = Lagrangian->L(q_, qp_,0);
            
            qp_[j] -= Epsilonp[j];
            q_[i]  += Epsilon[i];
            
            return ((L11-L01)/(2*Epsilon[i])-(L10-L00)/(2*Epsilon[i]))/(2*Epsilonp[j]);
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msSolverLagrangian::calc_qpp(vector_type& q_, vector_type& qp_, vector_type& qpp_)
        {
            // Calculate system of equations
            for (int i=0; i<Ndof; i++) {
                
                rhs[i] = diffq(i,q_,qp_);
                for (int j=0; j<Ndof; j++) {
                    
                    A(i,j) = diffpp(j,i,q_,qp_);
                    
                    if(A(i,j)==0){
                        
                        msException e("The matrix element A[i][j] is zero",
                                  "bool msSolverLagrangian::calc_qpp(vector_type& q_, vector_type& qp_, vector_type& qpp_)",
                                  getFullId() );
                        IMPACT_THROW_EXCEPTION(e);
                    }
                    // calculate B
                    rhs[i] -= diffqp(j,i,q_,qp_) * qp_[j];
                }
            }
            // Solve A * q'' = rhs
            try{
                solve(A,rhs,qpp_);
            }
            catch(msException& e){
                
                e.addContext("bool msSolverLagrangian::calc_qpp(vector_type& q_, vector_type& qp_, vector_type& qpp_)");
                IMPACT_THROW_EXCEPTION(e);
            }
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msSolverLagrangian::solve(matrix_type& A, vector_type& v, vector_type& w ) {
            
            int* r = new int[Ndof];  // indexing rows and columns for quick reordering
            int* c = new int[Ndof];
            int  n = 0;              // n is the total number of unconstrained rows
            
            for (int i=0; i<Ndof; i++) { c[n]=i; r[n]=i; n++;
            }
            
            for (int j=0; j<n-1; j++) {  // make j'th column zero from j+1'st row
                
                for (int i=j+1; i<n; i++) {  // make Aij zero
                    
                    if (fabs(A( r[j] , c[j] ) )<fabs(A(r[i],c[j]) )) {
                        int tmp;
                        tmp=r[i]; r[i]=r[j]; r[j]=tmp;
                    }
                    
                    if (A(r[i],c[j]) != 0.0) {
                        double m = A(r[i] , c[j]) / A( r[j] , c[j]);
                        
                        for (int k=j; k<n; k++)  A(r[i] , c[k]) -= m*A( r[j] , c[k]);
                        
                        v[r[i]] -= m*v[r[j]];
                    }
                }
            }
            // calculate result vector
            
            for (int j=n-1; j>=0; j--) {
                
                double b=v[r[j]];
                for (int k=j+1; k<n; k++)  b-=w[c[k]]*A( r[j] , c[k]);
                
                w[c[j]] = b / A( r[j] , c[j]);
            }
            
            delete[] r;delete[] c;
        }
        
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        std::ostream& msSolverLagrangian::print(std::ostream& out) const {
            
            msTreeMapper::print(out);
            output::printHeader(out,"msSolverLagrangian");
            getParameters()->print(out);
            return out;
        }
        
    }
}
