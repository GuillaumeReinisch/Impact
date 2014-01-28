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


#include "msKineticOperator.h"

namespace impact {
    
    namespace atomism {
        
        bool msDynamicDof::isDynamicDofRegisteredInPython = 0;
        msRegistrar msDynamicDof::Registrar("msDynamicDof", msDynamicDof::createInstance);
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        
        void msDynamicDof::registryInPython() {
            
#if USE_PYTHON
            if( ! msDynamicDof::isDynamicDofRegisteredInPython ) {
                
                using namespace boost::python;
                msTreeMapper::registryInPython();
                
                class_< msDynamicDof, bases<msTreeMapper>, boost::shared_ptr<msDynamicDof> >(
                                                                                             "msDynamicDof", "Dynamic degree of freedom definition" ,
                                                                                             no_init )
                .def( "New", &msDynamicDof::New , "Create a new object.")
                .staticmethod("New");
                msDynamicDof::isDynamicDofRegisteredInPython=1;
            }
#endif
            
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msDynamicDof::set(boost::shared_ptr<msScalarFunction> function,
                               boost::shared_ptr<msEntity> entity, size_t i) {
            
            LOGGER_ENTER_FUNCTION("void msDynamicDof::set(boost::shared_ptr<msScalarFunction> function,boost::shared_ptr<msEntity> entity, size_t i) ",
                                  getFullId());
            
            if( i >= entity->noOfDof() )
                IMPACT_THROW_EXCEPTION( msException("the indice of the Dof is out of range",
                                               "void msDynamicDof::set(boost::shared_ptr<msScalarFunction> function,boost::shared_ptr<msEntity> entity, size_t i)",
                                               getFullId()
                                               ));
            
            msTreeMapper::update(Function,function);
            msTreeMapper::update(Entity,entity);
            Indice = i;
            LOGGER_EXIT_FUNCTION();
        }
        
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        
        bool msKineticOperator::isKineticOperatorRegisteredInPython=0;
        msRegistrar msKineticOperator::Registrar("msKineticOperator", msKineticOperator::createInstance);
        msRegistrar msKineticOperator::msKineticOperatorParams::Registrar("msKineticOperatorParams", msKineticOperator::msKineticOperatorParams::createInstance);
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msKineticOperator::registryInPython() {
            
#if USE_PYTHON
            msTreeMapper::registryInPython();
            
            if( ! msKineticOperator::isKineticOperatorRegisteredInPython ) {
                
                using namespace boost::python;
                class_<msKineticOperator,bases<msPhysicalInterface>,boost::shared_ptr<msKineticOperator> >
                ("msKineticOperator",
                 "Operator defining the kinetic part of the equations of motion",
                 no_init )
                .def( "New", &msKineticOperator::New ,
                     "Create a new object. arg2: msUnitsManager object")
                .staticmethod("New")
                .def( "setCoordinates" , (boost::shared_ptr<msTreeMapper>(msKineticOperator::*)(boost::shared_ptr<msGeneralizedCoordinates>) )
                     &msKineticOperator::setCoordinates,
                     "Set the coordinates. arg2: generalized coordinates")
                .def( "setSystem" , (boost::shared_ptr<msTreeMapper>(msKineticOperator::*)(boost::shared_ptr<msSystem>) )
                     &msKineticOperator::setSystem,
                     "Set the system. arg2: system ")
                .def( "set" , (boost::shared_ptr<msTreeMapper>(msKineticOperator::*)(boost::shared_ptr<msGeneralizedCoordinates>,boost::shared_ptr<msSystem>) )&msKineticOperator::set,
                     "Set the kinetic operator: coordinates and system. arg2: generalized coordinates , arg3: system")
                .def( "setDynamicDofs", &msKineticOperator::setDynamicDofs,
                     "set the dynamic DoF w/ generalized coordinate")
                .def( "move" , &msKineticOperator::move,
                     "move the system w/ a coordinate. Arg1: coordinate")
                .def( "kineticFunction" ,  (double(msKineticOperator::*)() ) &msKineticOperator::kineticFunction,
                     "return the kinetic function at the current configuration")
                .def( "getKMatrixElement", &msKineticOperator::getKMatrixElement,
                     "return a KMatrix element of the last one computed. arg2: column index (on active Dof only);arg3: row index (on active Dof only).")
                .def( "setDynamicDofs" ,  &msKineticOperator::setDynamicDofs ,
                     "Set the dynamic degrees of freedom w/ current coordinates. Does not update the cartesian coordinates of the system.")
                .def( "addDynamicDof" , (boost::shared_ptr<msTreeMapper>(msKineticOperator::*)(boost::shared_ptr<msEntity> , size_t , boost::shared_ptr<msScalarFunction>) )
                     &msKineticOperator::addDynamicDof,
                     "Link a degree of freedom to the generalized coordinates. arg2: entity in the system, arg3: indice of the dof in the entity, arg3: scalar function")
                
                .def("addDynamicDofLinear",&msKineticOperator::addDynamicDofLinear,
                     "add a dynamic DoF with the scalar function f(q)=q. arg2: entity in the system, arg3: indice of the dof in the entity, arg3: coordinate q. ")
                .def( "getSystem" ,  &msKineticOperator::getSystem, "return the system")
                .def( "getCoordinates" ,( boost::shared_ptr<msGeneralizedCoordinates>(msKineticOperator::*)()) &msKineticOperator::getCoordinates,
                     "return the generalized coordinates") ;
                
                msKineticOperator::isKineticOperatorRegisteredInPython=1;
            }
#endif
        }
        
        ostream& operator<<(ostream& out,const msKineticOperator& op) { op.print(out); return out;  }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        /*
         msKineticOperator::msKineticOperator(const msKineticOperator& rhs) : msPhysicalInterface()
         {
         constructVar("msKineticOperator","KineticOperator","kinetic_operator");
         initialize();
         *this=rhs;
         }
         
         msKineticOperator& msKineticOperator::operator=(const msKineticOperator& original)
         {
         msPhysicalInterface::operator=(original);
         return *this;
         }
         */
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msKineticOperator::set(  boost::shared_ptr<msGeneralizedCoordinates> coords ,
                                                               boost::shared_ptr<msSystem> system  ) {
            setCoordinates(coords);return setSystem(system);
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msKineticOperator::setCoordinates(  boost::shared_ptr<msGeneralizedCoordinates> coords ) {
            
            LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msTreeMapper> msKineticOperator::setCoordinates(  boost::shared_ptr<msGeneralizedCoordinates> coords )"
                                      ,getFullId());
            msTreeMapper::update(GeneralizedCoordinates,coords);
            if( !GeneralizedCoordinates->hasParent()) GeneralizedCoordinates->setAffiliation(mySharedPtr());
            LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msKineticOperator::setCoordinates(  boost::shared_ptr<msGeneralizedCoordinates> coords )");
            return mySharedPtr();
        }
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msKineticOperator::setSystem(  boost::shared_ptr<msSystem> system ) {
            
            LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msTreeMapper> msKineticOperator::setSystem(  boost::shared_ptr<msSystem> system )"
                                      ,getFullId());
            if( !system->hasParent()) system->setAffiliation(mySharedPtr());
            msTreeMapper::update(System,system);
            LOGGER_EXIT_FUNCTION();
            return mySharedPtr();
        }
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msKineticOperator::addDynamicDof( std::string id , size_t indice ,
                                                                         boost::shared_ptr<msScalarFunction> fct) {
            
            LOGGER_ENTER_FUNCTION_DBG("void msKineticOperator::addDynamicDof(...)",getFullId());
            if( ( !checkTest(msKineticOperator::system       ))
               ||( !checkTest( msKineticOperator::coordinates )) )
            {LOGGER_EXIT_FUNCTION();  return mySharedPtr();}
            boost::shared_ptr<msEntity> entity = System->getEntity(id);
            if( entity == boost::shared_ptr<msEntity>())
            {LOGGER_EXIT_FUNCTION(); return mySharedPtr();}
            if( indice >= entity->noOfDof() ) {
                LOGGER_WRITE(msLogger::ERROR,"Degree of freedom indice providen out of bound for the entity");
                LOGGER_EXIT_FUNCTION();return mySharedPtr();
            }
            boost::shared_ptr<msDynamicDof> var= msDynamicDof::New();
            
            fct->setAffiliation( var );
            var->set(fct , entity , indice);
            var->setAffiliation(  mySharedPtr() );
            msTreeMapper::addElementToChildren<msDynamicDof>(DynamicDofs,var);
            LOGGER_EXIT_FUNCTION();
            return mySharedPtr();
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msKineticOperator::addDynamicDof( boost::shared_ptr<msEntity> entity , size_t indice ,
                                                                         boost::shared_ptr<msScalarFunction> fct ) {
            LOGGER_ENTER_FUNCTION_DBG("void msKineticOperator::addDynamicDof(msEntity& entity , size_t indice , boost::shared_ptr<msScalarFunction> fct)",
                                      getFullId());
            
            if( !checkTest(msKineticOperator::system )     )
                IMPACT_THROW_EXCEPTION( msException("Problem with the system definition",
                                               "void msKineticOperator::addDynamicDof(msEntity& entity , size_t indice , boost::shared_ptr<msScalarFunction> fct)",
                                               getFullId()
                                               ));
            if( !checkTest( msKineticOperator::coordinates ) )
                IMPACT_THROW_EXCEPTION( msException("Problem with the coordinates definition",
                                               "void msKineticOperator::addDynamicDof(msEntity& entity , size_t indice , boost::shared_ptr<msScalarFunction> fct)",
                                               getFullId()
                                               ));
            if( entity == boost::shared_ptr<msEntity>())
                IMPACT_THROW_EXCEPTION( msException("the entity is void",
                                               "void msKineticOperator::addDynamicDof(msEntity& entity , size_t indice , boost::shared_ptr<msScalarFunction> fct)",
                                               getFullId()
                                               ));
            if( indice >= entity->noOfDof() )
                IMPACT_THROW_EXCEPTION( msException("the indice of the Dof is out of range",
                                               "void msKineticOperator::addDynamicDof(msEntity& entity , size_t indice , boost::shared_ptr<msScalarFunction> fct)",
                                               getFullId()
                                               ));
            
            boost::shared_ptr<msDynamicDof> var = msDynamicDof::New();
            fct->setAffiliation( var );
            var->set(fct , entity , indice);
            var->setAffiliation(  mySharedPtr() );
            msTreeMapper::addElementToChildren<msDynamicDof>(DynamicDofs,var);
            LOGGER_EXIT_FUNCTION();
            return mySharedPtr();
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msKineticOperator::addDynamicDofLinear( boost::shared_ptr<msEntity> entity , size_t indice ,
                                                                               boost::shared_ptr<msScalarVariable> q ) {
            
            boost::shared_ptr<math::msLinearScalarFunction> linearFct = math::msLinearScalarFunction::New();
            linearFct->setYUnit(q->getUnitStr());
            boost::shared_ptr<msGeneralizedCoordinates> gcoors  = msGeneralizedCoordinates::New();
            gcoors->setUnits(getUnits());
            gcoors->addVar(q);
            linearFct->setCoordinates(gcoors);
            linearFct->setCoefsIndice(0,1,0);
            return addDynamicDof( entity, indice, linearFct );
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msKineticOperator::setDynamicDofs() {
            
            LOGGER_ENTER_FUNCTION_DBG("void msKineticOperator::setDynamicDofs()",getFullId());
            
            msChildren<msDynamicDof>::iterator it=DynamicDofs.begin();
            
            for(;it!=DynamicDofs.end();it++)
                (*it)->Entity->Dof( (*it)->Indice )= (*it)->Function->evaluate() ;
            
            LOGGER_EXIT_FUNCTION();
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        /*
         void msKineticOperator::computeDofsVariation(msScalarVariable& qi) {
         
         LOGGER_ENTER_FUNCTION("void msKineticOperator::computeDofVariation(msScalarVariable& qi)",this->getFullId());
         vector<double> v(dDofs.size());
         
         qi.Value += qi.dq;    setDynamicDofs();   System->getValueOfDofs(dDofs);
         LOGGER_WRITE(msLogger::DEBUG,"dof values at q0: "+output::getString<double>(dDofs));
         qi.Value -= qi.dq;    setDynamicDofs();   System->getValueOfDofs(v);
         LOGGER_WRITE(msLogger::DEBUG,"dof values at q0+dq: "+output::getString<double>(v));
         
         for(size_t j=0;j<v.size();j++) dDofs[j]-=v[j];
         
         LOGGER_EXIT_FUNCTION();
         }
         */
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        msKineticWork msKineticOperator::move(size_t i) {
            
            LOGGER_ENTER_FUNCTION("msKineticWork msKineticOperator::move(msScalarVariable& q)",
                                  getFullId());
            
            msKineticWork work; stringstream out;
            msScalarVariable& q = *(GeneralizedCoordinates->operator[](i));
            
            setDynamicDofs();   	System->computeCartCoordinates(Coors0);
            
            q.increment();
            setDynamicDofs();	System->computeCartCoordinates(Coors1);
            
            
            if( System->isIsolated() ) {
                
                LOGGER_WRITE(msLogger::DEBUG,"Annihil Ptot");
                System->annihil_dP(Coors0,Coors1,&( System->OverallTranslation) );
                LOGGER_WRITE(msLogger::DEBUG,"Annihil Jtot");
                work = System->annihil_dJ(Coors0,Coors1,&( System->OverallRotation) );
                out.str("");
                out<<"Updated overall rotation operator: \n"<<System->OverallRotation<<endl;
                LOGGER_WRITE( msLogger::DEBUG,out.str() );
            }
            
            System->computeCartCoordinates();   //System->computeCartCoordinates(Coors1);
            System->getPositions(Coors1);
            
            work = System->kineticWork(Coors0,Coors1);
            out.str("");out<<"Displacments done; kinetic work: dEc="<<work.dEkin<<"; dJ="<<work.dJ.norm()<<"; dP="<<work.dP.norm();
            LOGGER_WRITE( msLogger::INFO,out.str() );
            
            LOGGER_EXIT_FUNCTION();
            return work;
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        const msKineticOperator::msKineticMatrix& msKineticOperator::computeKMat(const msUnitsManager& units) {
            
            LOGGER_ENTER_FUNCTION("const msKineticMatrix& msKineticOperator::computeKMat(const msUnitsManager* units)",
                                  this->getFullId());
            stringstream s;
            msUnitsManager tmpUnits = units;
            tmpUnits.setAngle(msUnit::Radian);
            int N = GeneralizedCoordinates->noOfActive();
            
            if (N==0 ){
                msException e("Empty coordinates are associated to the kinetic operator",
                          "const msKineticOperator::msKineticMatrix& msKineticOperator::computeKMat(const msUnitsManager& units)",
                          getFullId()
                          );
                IMPACT_THROW_EXCEPTION(e);
            }

            KMatrix->resize(N,N);
            (*KMatrix)=Eigen::MatrixXd::Zero(N,N);
            
            if (DynamicDofs.size()==0){
                LOGGER_EXIT_FUNCTION2("const msKineticMatrix& msKineticOperator::computeKMat(const msUnitsManager* units)");
               return *KMatrix;
            }
            try{ JacobianOfDispl->initialize( this );
            }
            catch( msException& e ) {
                e.addContext("Initialization of the Jacobian of displacement failed (const msKineticOperator::msKineticMatrix& msKineticOperator::computeKMat(const msUnitsManager& units))");
                throw e;
            }
            
            try{ JacobianOfDispl->compute(tmpUnits,System->isIsolated());
            }
            catch( msException& e ) {
                e.addContext("Computation of the Jacobian of displacement failed (const msKineticOperator::msKineticMatrix& msKineticOperator::computeKMat(const msUnitsManager& units))");
                throw e;
            }
            
            LOGGER_WRITE(msLogger::INFO,"Displacments computed, Now sum mat * dRat/dqi * dRat/dqj" );
            
            msGeneralizedCoordinates::UnfreezIt qi = GeneralizedCoordinates->beginUnfreezed();
            msGeneralizedCoordinates::UnfreezIt qj = qi;
            msSystem::ElementItr  elem = System->beginElements();
            
            double convLength = getUnits()->convert(*System,msUnit::vLength,1);
            
            qi=GeneralizedCoordinates->beginUnfreezed();
            
            boost::shared_ptr<msGeneralizedCoordinates> gc=GeneralizedCoordinates.getSharedPtr();
            
            for(int i=0; qi!=gc->endUnfreezed();qi++,i++){
                qj = qi;
                for(int j=i; qj!=gc->endUnfreezed();qj++,j++) {
                    
                    stringstream s; s<<"KMat["<<i<<","<<j<<"]=";
                    elem = System->beginElements();
                    for(int ielem=0 ; elem!=System->endElements() ; elem++ , ielem++){
                        
                        (*KMatrix)(i,j) += (*elem).getMass(tmpUnits.getMass())
                        * (*JacobianOfDispl)(ielem,i).dot( (*JacobianOfDispl)(ielem,j));
                        s<<(*KMatrix)(i,j)<<"+";
                        if( i!=j )   (*KMatrix)(j,i) = (*KMatrix)(i,j) ;
                    }
                    LOGGER_WRITE(msLogger::DEBUG , s.str() );
                }
            }
            s<<"KMatrix:\n"<<(*KMatrix);
            LOGGER_WRITE(msLogger::DEBUG,s.str() );
            LOGGER_EXIT_FUNCTION2("const msKineticMatrix& msKineticOperator::computeKMat(const msUnitsManager* units)");
            return *KMatrix;
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        double msKineticOperator::getKMatrixElement(size_t i,size_t j) {
            
            if( (i>=GeneralizedCoordinates->noOfActive()) ||
               (j>=GeneralizedCoordinates->noOfActive()) ) {
                
                stringstream out;
                out<<"The number of active degree of freedom is "<<GeneralizedCoordinates->noOfActive()
                <<" and you provided one index out of range (i="<<i<<", j="<<j<<").";
                msException e(out.str(),"double msKineticOperator::getKMatrixElement(size_t i,size_t j)",getFullId() );
                IMPACT_THROW_EXCEPTION(e);
            }
            if( KMatrix->size() != GeneralizedCoordinates->noOfActive() ) {
                
                stringstream out;
                out<<"Unconsistent size of the last KMatrix computed with number of active Dof, try recompute the KMatrix";
                msException e(out.str(),"double msKineticOperator::getKMatrixElement(size_t i,size_t j)",getFullId() );
                IMPACT_THROW_EXCEPTION(e);
            }
            return (*KMatrix)(i,j);
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        double msKineticOperator::kineticFunction(const msUnitsManager& units) {
            
            try{ computeKMat(units);
            }
            catch( msException & e ) {
                e.addContext("computation of KMatrix failed (double msKineticOperator::kineticFunction(const msUnitsManager& units) )");
                throw;
            }
            
            return KMatrix->determinant();
        };
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        std::ostream& msKineticOperator::print(std::ostream& out) const {
            
            msPhysicalInterface::print(out);
            out<<output::printSepLine()
            <<output::printYellow("\tKinetic Operator \n")
            <<output::printSepLine();
            
            if(checkTest(msKineticOperator::system))
                out<<"Id of the attached system:  "<<System->getId()<<"; type: "<<System->getType()
                <<";\n\nnumber of dynamic degrees of freedom defined: "<<DynamicDofs.size()<<std::endl;
            else out<<" no system defined"<<endl;
            
            msChildren<msDynamicDof>::const_iterator it = DynamicDofs.begin();
            out<<setw(20)<<"No."<<setw(20)<<"Id Entity"<<setw(20)<<"Dof number"<<setw(20)<<"Id scalar fct\n";
            
            for(int i=0 ; it != DynamicDofs.end() ; ++it, ++i )
                out<<setw(20)<<i<<setw(20)<<(*it)->Entity->getId()<<setw(20)<<(*it)->Indice<<setw(20)<<(*it)->Function->getId()<<endl;
            
            
            if(checkTest(msKineticOperator::coordinates)) {
                
                out<<"\nId of the generalized coordinates:  "<<GeneralizedCoordinates->getId()<<"; type: "<<GeneralizedCoordinates->getType()<<endl<<"\t";
                GeneralizedCoordinates->getUnits()->abstract(out);
                out<<"\n\tList of unfreezed coordinates:\n";
                msGeneralizedCoordinates::UnfreezIt qi = GeneralizedCoordinates->beginUnfreezed();
                
                for(; qi!=GeneralizedCoordinates->endUnfreezed(); qi++) {
                    out<<"\t\t"; (*qi).abstract(out); out<<endl;}
                
            }
            else out<<" no coordinates defined"<<endl;
            out<<"Jacobian of displacments: ";JacobianOfDispl->print(out); out<<endl;
            if( GeneralizedCoordinates->noOfActive() < 10 )
                out<<"\n\nThe last kinetic matrix computed is \n"<<*KMatrix<<endl;
            
            return out;
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        bool msKineticOperator::checkTest(TestType test) const {
            
            LOGGER_ENTER_FUNCTION_DBG("bool msKineticOperator::checkTest(TestType test) ",getFullId());
            
            switch( test ) {
                case system:
                    if( System.isNull() ) {
                        LOGGER_WRITE(msLogger::ERROR, "System needs to be initialized");
                        break;
                    }
                case coordinates:
                    if( GeneralizedCoordinates.isNull()  ) {
                        LOGGER_WRITE(msLogger::ERROR, "Generalized coordinates need to be initialized");
                        break;
                    }
            }
            LOGGER_EXIT_FUNCTION();
            return 1;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        // msJacobianOfDispl
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        ostream& operator<<(ostream& out,const msJacobianOfDispl& jac) { jac.print(out); return out;  }
        
        msJacobianOfDispl::msJacobianOfDispl(){ NoOfAtoms=NoOfDofs=0; };
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msJacobianOfDispl::resize(size_t noOfat,size_t noOfdof) {
            
            if( (noOfat==NoOfAtoms) && (noOfdof==NoOfDofs) )  return;
            
            Displ_Elem_qi.resize(NoOfAtoms=noOfat,NoOfDofs=noOfdof);
        }
        
        //-------------------------------------------------------------------------------
        
        const Eigen::Vector3d& msJacobianOfDispl::operator()(int elem,int qi) {
            
            return Displ_Elem_qi(elem,qi);
        };
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        std::ostream& msJacobianOfDispl::print(std::ostream& out) const {
            
            out<<"Jacobian of displacments of "<<NoOfAtoms<<" elements with respect to "<<NoOfDofs<<" degrees of freedom."<<endl;
            return out;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msJacobianOfDispl::initialize( msKineticOperator* kinetic ) {
            
            LOGGER_ENTER_FUNCTION("void msJacobianOfDispl::initialize( msKineticOperator* kinetic )",kinetic->getFullId()+"JacobianOfDispl");
            
            KineticOperator        = kinetic;
            System 		= kinetic->System.get();
            GeneralizedCoordinates = KineticOperator->GeneralizedCoordinates.get();
            OverallTrans	        = &(System->OverallTranslation);
            OverallRot  	        = &(System->OverallRotation);
            
            msGeneralizedCoordinates::UnfreezIt qi = GeneralizedCoordinates->beginUnfreezed();
            
            for(int i=0;qi!=GeneralizedCoordinates->endUnfreezed();++qi,++i)
                
                if( (! (*qi).isHomogeneousWith("Angstrom")) && (!(*qi).isHomogeneousWith("Degree")) )
                    
                    IMPACT_THROW_EXCEPTION( msException("Variable are neither of type angle or length"
                                                   ,"void msJacobianOfDispl::initialize( msKineticOperator* kinetic )",KineticOperator->GeneralizedCoordinates->getFullId()) );
            
            
            
            
            KineticOperator->setDynamicDofs();
            System->getValueOfDofs(Dofs0);
            LOGGER_WRITE(msLogger::INFO, "dofs value at q0 : " + output::getString<double>(Dofs0) );
            
            System->computeCartCoordinates(Coors0);
            CenterOfMass0 = System->getCenterOfMass();
            
            resize(System->noOfElements(), GeneralizedCoordinates->noOfActive());
            
            LOGGER_EXIT_FUNCTION();
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msJacobianOfDispl::compute( const msUnitsManager& units, bool isolated ) {
            
            LOGGER_ENTER_FUNCTION("msJacobianOfDispl::compute(...)",KineticOperator->getFullId()+"JacobianOfDispl");
            
            msGeneralizedCoordinates::UnfreezIt qi = GeneralizedCoordinates->beginUnfreezed();
            msKineticWork work;stringstream out;
            
            if( isolated )
                LOGGER_WRITE(msLogger::INFO,"System isolated: annihil overall motions contribution");
            
            for(int i=0;qi!=GeneralizedCoordinates->endUnfreezed();++qi,++i) {
                
                qi->setValue( qi->getValue() + qi->getdq());
                KineticOperator->setDynamicDofs();
                
                bool isNull = System->getDeltaDofs(dDofs,Dofs0);
                out.str("");out<<"dofs variation for d"<<(*qi).getId()<<" = "<<(*qi).getdq()<<" : "<<output::getString<double>(dDofs);
                LOGGER_WRITE(msLogger::INFO,out.str());
                
                if( !isNull ) {
                    
                    try{ System->computeCartCoordinates(Coors1);
                    }
                    catch( msException& e ) {
                        
                        e.addContext( "Computation of cartesian coordinates failed (void msJacobianOfDispl::compute( const msUnitsManager& units, bool isolated ))" );
                        throw e;
                    }
                    
                    if( isolated ) {
                        try{
                            LOGGER_WRITE(msLogger::DEBUG,"Annihil Ptot");
                            System->annihil_dP(Coors0,Coors1,0);
                            LOGGER_WRITE(msLogger::DEBUG,"Annihil Jtot");
                            work = System->annihil_dJ(Coors0,Coors1,0);
                        }
                        catch(msException& e){ e.addContext("can not annihil overall motions (void msJacobianOfDispl::compute( const msUnitsManager& units, bool isolated ))");
                            throw e;
                        }
                    }
                    
                    
                    if( msLogger::isDebug() ){
                        
                        LOGGER_WRITE(msLogger::DEBUG,"Initial atomic positions:");
                        LOGGER_HEADER3COLUMNS(msLogger::DEBUG,"x",System->getUnits()->getLengthStr(),
                                              "y",System->getUnits()->getLengthStr(),
                                              "z",System->getUnits()->getLengthStr()
                                              );
                        for(size_t i =0;i<Coors1.size();i++){
                            LOGGER_WRITE3COLUMNS(msLogger::DEBUG,Coors0[i][0],Coors0[i][1],Coors0[i][2]);
                        }
                        LOGGER_WRITE(msLogger::DEBUG,"New atomic positions:");
                        LOGGER_HEADER3COLUMNS(msLogger::DEBUG,"x",System->getUnits()->getLengthStr(),
                                              "y",System->getUnits()->getLengthStr(),
                                              "z",System->getUnits()->getLengthStr()
                                              );
                        for(size_t i =0;i<Coors1.size();i++){
                            LOGGER_WRITE3COLUMNS(msLogger::DEBUG,Coors1[i][0],Coors1[i][1],Coors1[i][2]);
                        }
                    }
                    
                    out.str("");out<<"Displacments done; kinetic work: dEc="<<work.dEkin<<"; dJ="<<work.dJ.norm()<<"; dP="<<work.dP.norm();
                    LOGGER_WRITE(msLogger::INFO,out.str());
                }
                else{
                    
                    LOGGER_WRITE(msLogger::ERROR, "The variation of the generalized coordinate "+output::getString<int>(i)+" did not generate displacment in the system");
                    Coors1 = Coors0 ;
                }
                setJacColumn( qi,i,units);
                qi->setValue( qi->getValue() - qi->getdq());
            }
            
            msSystem::ElementItr elem = System->beginElements();
            for( int ielem=0; elem!= System->endElements() ; elem++ , ielem++ )
                (*elem).setPosition( Coors0[ielem] );
            
            LOGGER_EXIT_FUNCTION();
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msJacobianOfDispl::setJacColumn( const msGeneralizedCoordinates::UnfreezIt qi, size_t ind,
                                             const msUnitsManager& units ) {
            
            LOGGER_ENTER_FUNCTION("void msJacobianOfDispl::setJacColumn(...)",KineticOperator->getFullId()+".JacobianOfDispl");
            double dqi = GeneralizedCoordinates->getdq(qi.toIterator(), units);
            LOGGER_WRITE(msLogger::DEBUG, "dq= "+ output::getString<double>(dqi));	 
            
            stringstream s;
            for(size_t i=0;i<NoOfAtoms;i++) {
                
                Displ_Elem_qi(i,ind) = units.convert(System->getUnits()->getLength(),1) * ( Coors1[i] - Coors0[i] ) / dqi; 
                s<<"Jac[qi="<<ind<<"][elem="<<i<<"]="<<Displ_Elem_qi(i,ind)[0]<<" "<<Displ_Elem_qi(i,ind)[1]<<" "<<Displ_Elem_qi(i,ind)[2];
                LOGGER_WRITE(msLogger::DEBUG, s.str()) ;s.str("");	
            }
            
            LOGGER_EXIT_FUNCTION();	
        } 
    }
}

