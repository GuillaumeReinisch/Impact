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

#include "msLagrangian.h"


namespace impact {
    
    namespace atomism {
        
        bool msLagrangian::isLagrangianRegisteredInPython=0;
        msRegistrar msLagrangian::Registrar("msLagrangian", msLagrangian::createInstance);
        
        ostream& operator<<(ostream& out,const msLagrangian& motion) {  motion.print(out); return out; }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        
        void msLagrangian::registryInPython()
        {
#if USE_PYTHON
            msMotion::registryInPython();
            
            if( ! msLagrangian::isLagrangianRegisteredInPython )
            {
                using namespace boost::python;
                
                class_<msLagrangian,bases<msMotion>,boost::shared_ptr<msLagrangian> >
                ("msLagrangian",
                 "Defines a mechanical system. ",no_init )
                .def( "New", &msLagrangian::New , "Create a new object")
                .staticmethod("New")
                .def("setEqOfMotion" ,  &msLagrangian::setEqOfMotion , "defines the system. Arg1: kinetic operator; Arg2: Potential energy" )
                .def("E" ,  (double(msLagrangian::*)() ) &msLagrangian::E , "Total energy at the current state" )
                .def("T" ,  (double(msLagrangian::*)() ) &msLagrangian::T , "Kinetic energy at the current state" )
                .def("U" ,  (double(msLagrangian::*)() ) &msLagrangian::U , "Potential energy at the current state" )
                .def("L" ,  (double(msLagrangian::*)() ) &msLagrangian::L , "Action at the current state" )
                .def("getGeneralizedCoordinates" ,  &msLagrangian::getGeneralizedCoordinates , "return the generalized coordinates" )
                .def("getGeneralizedVelocities" ,  &msLagrangian::getGeneralizedVelocities, "return the generalized velocities" )
                .def("getKineticOperator" ,  &msLagrangian::getKineticOperator , "return the kinetic operator" )
                .def("getSampler" ,  &msLagrangian::getSampler , "return the sampler" )
                .def(self_ns::str(self_ns::self));
                msLagrangian::isLagrangianRegisteredInPython=1;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msLagrangian::initialize(){
            
            msMotion::initialize();
            
            try{
                declareChild<msScalarFunction>( Epot, msScalarFunction::New() , "Epot" );
                declareChild<msGeneralizedCoordinates>(GeneralizedCoordinates,msGeneralizedCoordinates::New(), "GeneralizedCoordinates");
                declareChild<msKineticOperator>(KineticOperator, msKineticOperator::New(), "KineticOperator" );
                declareChild<msSampler>(Sampler,boost::static_pointer_cast<msSampler>(msSamplerLinear::New()), "Sampler" );
                GeneralizedCoordinates->setUnits(getUnits());
                GeneralizedVelocities = msGeneralizedCoordinates::New();
                GeneralizedVelocities->setUnits(getUnits());
            }
            catch(msException& e){
                
                e.addContext("void msLagrangian::initialize()");
                throw e;
            }
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msLagrangian::update(){
            
            msMotion::update();
            
            try{
                q.resize(GeneralizedCoordinates->noOfActive());
                qp.resize(GeneralizedCoordinates->noOfActive());
                Sampler->setCoordinates(GeneralizedCoordinates.getSharedPtr());
                initializeGeneralizedVelocities();
            }
            catch(msException& e){
                
                e.addContext("void msLagrangian::update()");
                throw e;
            }
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper>  msLagrangian::setEqOfMotion( boost::shared_ptr<msKineticOperator> kin ,
                                                                     boost::shared_ptr<msScalarFunction> epot ) {
            
            LOGGER_ENTER_FUNCTION_DBG("void msLagrangian::defineEqOfMotion(...)",getFullId());
            msTreeMapper::update(KineticOperator,kin);
            msTreeMapper::update(Epot,epot);
            
            if( Epot->getCoordinates()->noOfDim() == 0){
                
                Epot->setCoordinates(getGeneralizedCoordinates());
            }
            if( KineticOperator->getCoordinates()->noOfDim() == 0 ){
                    
                KineticOperator->setCoordinates(getGeneralizedCoordinates());
            }
            
            if(    (Epot->getCoordinates().get() != KineticOperator->getCoordinates().get()) ) {
                
                IMPACT_THROW_EXCEPTION( msException("The coordinates of the PES and kinetic operators have to point on the same object"
                                               ,"boost::shared_ptr<msTreeMapper>  msLagrangian::setEqOfMotion( boost::shared_ptr<msKineticOperator> kin , boost::shared_ptr<msScalarFunction> epot )"
                                               ,getFullId() ));
            }
            if(!kin->hasParent())  kin->setAffiliation(mySharedPtr());
            if(!epot->hasParent()) epot->setAffiliation(mySharedPtr());
            
            msTreeMapper::update(GeneralizedCoordinates,KineticOperator->getCoordinates());
            
            if(!GeneralizedCoordinates->hasParent()) GeneralizedCoordinates->setAffiliation(mySharedPtr());
            
            update();
            LOGGER_EXIT_FUNCTION();
            return mySharedPtr();
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msLagrangian::initializeGeneralizedVelocities() {
            
            GeneralizedVelocities->clear();
            GeneralizedVelocities->setUnits(getGeneralizedCoordinates()->getUnits());
            
            if( GeneralizedCoordinates->noOfActive() == 0) return;
            if( KineticOperator->getCoordinates()->noOfActive() == 0) return;
            
            double Edisp = SiUnits.convert( getUnits()->getEnergy(), Emax) ;
            double dE    = SiUnits.convert( getUnits()->getEnergy(), dE) ;
            size_t nDof  = GeneralizedCoordinates->noOfActive();
            
            KineticOperator->computeKMat( SiUnits );
            
            msGeneralizedCoordinates::UnfreezIt qi = GeneralizedCoordinates->beginUnfreezed();
            for(size_t i=0; qi!=GeneralizedCoordinates->endUnfreezed(); qi++, i++) {
                
                double pmax = sqrt( 2*(Edisp/nDof ) / ( KineticOperator->getKMatrix()(i,i) ) );
                double dp =   sqrt( 2*(dE/nDof )    / ( KineticOperator->getKMatrix()(i,i) ) );
                
                boost::shared_ptr<msScalarVariable> var = msScalarVariable::New();
                var->set(0,0,0,0,0,qi->getUnit()->getStr()+"."+getUnits()->getTimeStr()+"^-1");
                var->setMax(pmax,SiUnits);
                var->setDq(pmax/1000.,SiUnits);
                var->setdq(dp/100.,SiUnits);
                var->setId(qi->getId()+"_p");
                GeneralizedVelocities->addVar(var);
            }
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msLagrangian::DOS(double E) {
            
            LOGGER_ENTER_FUNCTION("double msLagrangian::DOS(double E)",getFullId());
            LOGGER_WRITE(msLogger::INFO,"Sample the generalized coordinates space");
            
            boost::shared_ptr<msDOSIntegrand> DOSfct = msDOSIntegrand::New();
            DOSfct->setSystemAndEnergy(this,E);
            DOSfct->setYUnit("J^-1");
            DOSfct->setCoordinates(GeneralizedCoordinates.getSharedPtr());
            double DOS = 0;
            
            try{ DOS =  Sampler->sample( *DOSfct , SiUnits ).Integral;
            }
            catch( msException& e ) {
                e.addContext("double msLagrangian::DOS(double E) ");
                throw;
            }
            
            DOS *= 1/getUnits()->convert(msUnit::J,1);
            LOGGER_EXIT_FUNCTION2("double msLagrangian::DOS(double E)");
            return DOS * Mult / Sym;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msLagrangian::Q(double T) {
            
            LOGGER_ENTER_FUNCTION("double msLagrangian::Q(double T)",getFullId());
            LOGGER_WRITE(msLogger::INFO,"Sample the generalized coordinates space");
            
            double Q = 0;
            
            boost::shared_ptr<msQIntegrand> Qfct = msQIntegrand::New();
            Qfct->setSystemAndTemperature(this,T);
            Qfct->setYUnit("");
            Qfct->setCoordinates(GeneralizedCoordinates.getSharedPtr());
            
            LOGGER_WRITE(msLogger::INFO,"Start sampling");
            try{ Q =  Sampler->sample( *Qfct , SiUnits ).Integral;
            }
            catch( msException& e ) {
                e.addContext("double msLagrangian::Q(double T)");
                throw;
            }
            LOGGER_EXIT_FUNCTION2("double msLagrangian::Q(double T)");
            return Q * Mult / Sym;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msLagrangian::averageByBoltzmann( msObservable& observable , double T )  {
            
            LOGGER_ENTER_FUNCTION("double msLagrangian::averageByBoltzmann( msObservable& observable , double T )",getFullId());
            LOGGER_WRITE(msLogger::INFO,"Sample the generalized coordinates space");
            
            double average = 0;
            boost::shared_ptr<msQbyObservable> observe = msQbyObservable::New();
            observe->setSystemAndTemperature( this, T );
            observe->setObservable(&observable);
            observe->setYUnit("");
            observe->setCoordinates(GeneralizedCoordinates.getSharedPtr());
            LOGGER_WRITE(msLogger::INFO,"Start sampling");
            
            try{
                average = Sampler->sample(*observe).Integral / ( observe->sumQ * GeneralizedCoordinates->getVolumeDq() ) ;
            }
            catch( msException& e ) {
                
                e.addContext("double msLagrangian::averageByBoltzmann( msObservable& observable , double T )");
                IMPACT_THROW_EXCEPTION(e);
            }
            LOGGER_EXIT_FUNCTION2("double msLagrangian::averageByBoltzmann( msObservable& observable , double T )");
            return average * Mult / Sym;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msLagrangian::E() {
            
            GeneralizedCoordinates->getUnfreezedValues(SiUnits,q);
            GeneralizedVelocities->getUnfreezedValues(SiUnits,qp);
            
            return getUnits()->convert(SiUnits.getEnergy() ,E( q , qp));
        }

        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msLagrangian::U() {
               
            GeneralizedCoordinates->getUnfreezedValues(SiUnits,q);
            
            return getUnits()->convert(SiUnits.getEnergy() , U( q ));
        }

        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
               
        double msLagrangian::T() {
            
            GeneralizedCoordinates->getUnfreezedValues(SiUnits,q);
            GeneralizedVelocities->getUnfreezedValues(SiUnits,qp);
            
            return getUnits()->convert(SiUnits.getEnergy(),T( q , qp));
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
               
        double msLagrangian::L() {
            
            GeneralizedCoordinates->getUnfreezedValues(SiUnits,q);
            GeneralizedVelocities->getUnfreezedValues(SiUnits,qp);
            
            return getUnits()->convert(SiUnits.getEnergy(),L( q , qp));
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msLagrangian::T(vector_type q_, vector_type qp_) {
            
            try{
                GeneralizedCoordinates->setUnfreezedValues(SiUnits,q=q_);
                GeneralizedVelocities->setUnfreezedValues(SiUnits,qp=qp_);
                
                KineticOperator->computeKMat( SiUnits );
            }
            catch(msException& e) {
                e.addContext("double msLagrangian::T(vector_type q_, vector_type qp_)");
                IMPACT_THROW_EXCEPTION(e);
            }
            vector_type tmp = KineticOperator->getKMatrix()*qp_;
            
            return 0.5*tmp.dot(qp_);
        }
        
        //-------------------------------------------------------------------------------------------------
        
        double msLagrangian::U(vector_type q_) {
            
            GeneralizedCoordinates->setUnfreezedValues(SiUnits,q_);
            
            return  SiUnits.convert( Epot->getYUnit(), Epot->evaluate());
        }
        
        //-------------------------------------------------------------------------------------------------
        
        double msLagrangian::L(vector_type q_, vector_type qp_) { return T(q_, qp_)-U(q_);  //J/mol
        }
        
        //-------------------------------------------------------------------------------------------------
        
        double msLagrangian::L(vector_type q_, vector_type qp_,bool compkmat) {
            
            if(compkmat) return L(q_,qp_);
            
            vector_type tmp = KineticOperator->getKMatrix()*qp_;
            double L = 0.5*tmp.dot(qp_) + U(q_);
            return L;
        }
        
        //-------------------------------------------------------------------------------------------------
        
        double msLagrangian::E(vector_type q_, vector_type qp_)  {
            
            return T(q_, qp_) + U(q_);    //J/mol
        }
        
        //-------------------------------------------------------------------------------------------------
        
        void msLagrangian::computeAll(vector_type q_, vector_type qp_,double& E,double& Ep,double& Ek,double& L) {
            
            Ek=T(q_, qp_);
            Ep=U(q_);
            E=Ek+Ep;
            L=Ek-Ep;
        }
        
        //-------------------------------------------------------------------------------------------------
        
        void msLagrangian::updateCoorsAndVelocitiesFromSi(vector_type& q, vector_type& qp) {
            
            GeneralizedCoordinates->setUnfreezedValues(SiUnits,q);
            GeneralizedVelocities->setUnfreezedValues(SiUnits,qp);
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        bool msLagrangian::sanityCheck() {
            
            if( GeneralizedCoordinates != Epot->getCoordinates() ) {
                
                msException e("The PES's coordinates and the Lagrangian's coordinates must point to the same object",
                          "virtual bool msLagrangian::sanityCheck()",
                          getFullId());
                IMPACT_THROW_EXCEPTION(e);
            }
            if( GeneralizedCoordinates != KineticOperator->getCoordinates() ) {
                
                msException e("The kinetic operator's coordinates and the Lagrangian's coordinates must point to the same object",
                          "virtual bool msLagrangian::sanityCheck()",
                          getFullId());
                IMPACT_THROW_EXCEPTION(e);
            }
            if( ! Epot->getYUnit().isConsistent(msUnit::vEnergy)) {
                
                msException e("The unit of the potentiale energy is not consistent with energy",
                          "double msLagrangian::U()",
                          getFullId());
                IMPACT_THROW_EXCEPTION(e);
            };
            
            return 1;
        };
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        std::ostream& msLagrangian::print(std::ostream& out) const {
            
            msMotion::print(out);
            output::printHeader(out,"Lagrangian");
            
            out<<"Potential energy: "<< Epot->getFullId() <<endl;
            out<<"Kinetic operator: "<< KineticOperator->getFullId() <<endl;
            out<<"\nId of the generalized coordinates:  "<<GeneralizedCoordinates->getId()<<"; type: "<<GeneralizedCoordinates->getType()<<endl<<"\t";
            GeneralizedCoordinates->getUnits()->abstract(out);
            out<<"\n\tList of unfreezed coordinates:\n";
            msGeneralizedCoordinates::UnfreezIt qi = GeneralizedCoordinates->beginUnfreezed();
            
            for(; qi!=GeneralizedCoordinates->endUnfreezed(); qi++) {
                out<<"\t\t"; (*qi).abstract(out); out<<endl;}
            
            // out<<"\nCurrent state in SI:\nq:\n"<<q<<"\nqp: \n"<<qp<<endl;
            return out;
        }
        
        
        
    }
    
}
