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


#include "msEntity.h"

namespace impact {
    
    namespace atomism {
        
        bool msEntity::isEntityRegisteredInPython=0;
        msRegistrar msEntity::Registrar("msEntity", msEntity::createInstance);
        
        void msEntity::registryInPython()
        {
#if USE_PYTHON
            
            msPhysicalInterface::registryInPython();
            
            if( ! msEntity::isEntityRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msEntity,bases<msPhysicalInterface>,boost::shared_ptr<msEntity> >
                ("msEntity",
                 "Define a container of elements, and link the elements position to the generalized coordinates",no_init
                 )
                .def( "New", &msEntity::New ,
                     "Create a new object. Arg1: msUnitsManager object")
                .staticmethod("New")
                .def( "addElement" , &msEntity::addElement,
                     "Add an element to the entity")
                .def( "setDof" ,  &msEntity::setDof,
                     "set the value of a Dof. arg1: indice of the dof (int), arg2: value (double).")
                .def( "noOfDof" ,  &msEntity::noOfDof,
                     "return the number of degree of freedom")
                .def( "computeCartCoordinates", &msEntity::computeCartCoordinates,
                     "compute the cartesian coordinates")
                .def( "getDofs", &msEntity::getDofs,
                     "return the list of the Dof values")
                .def( "getMass", &msEntity::getMass,
                     "return the mass of the entity")
                .def( "getEigenInertia", &msEntity::getEigenInertia,
                     "return a vector of double of the eigen moment of inertia");
                
                msEntity::isEntityRegisteredInPython=1;
            }
#endif
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msEntity::addElement(boost::shared_ptr<msElement> elem) {
            
            if(!elem->hasParent()) elem->setAffiliation(mySharedPtr());
	    
            msTreeMapper::addElementToChildren<msElement>(Elements,elem);
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msEntity::reserveDofs( size_t n ) {
            
            Dofs.reserve(n);
            Dofs.clear();
        }
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msEntity::reset() {
            
            LOGGER_ENTER_FUNCTION_DBG("void msEntity::reset() ",getFullId());
            Dofs.resize(0);
            msPhysicalInterface::clearPhysicalVariables();
            msTreeMapper::clearChildren(Elements);
            LOGGER_EXIT_FUNCTION();
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msEntity::addDof( double value, const msUnit& unit) {
            
            LOGGER_ENTER_FUNCTION_DBG("void msEntity::addDof(double value) "+output::getString<double>(value),getFullId());
            
            if( Dofs.size() == Dofs.capacity() - 1)
                BOOST_THROW_EXCEPTION(msError("The number of Dof added exceed the capacity reserved",
                                              "void msEntity::addDof( double value, const msUnit& unit)",getFullId())
                                      );
            Dofs.push_back(value);
            
            declarePhysicalVariable( unit , &(Dofs.back()) );
            LOGGER_EXIT_FUNCTION2("void msEntity::addDof(double value) "+output::getString<double>(value));
        }
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        boost::shared_ptr<msElement> msEntity::getElement(size_t i) const
        {
            if( i >=Elements.size() ) {
                
                LOGGER_ENTER_FUNCTION("Vector3d msEntity::getElement(size_t indice)",getFullId());
                LOGGER_WRITE(msLogger::ERROR,"indice out of bounds");
                LOGGER_EXIT_FUNCTION();
                return(boost::shared_ptr<msElement>() );
            }
            return Elements[i].getSharedPtr();
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        Vector3d msEntity::getPosition(size_t indice) const {
            if( indice >=Elements.size() ) {
                stringstream out;
                out<<"Indice out of bounds ("<<indice<<">"<<Elements.size()<<")";
                BOOST_THROW_EXCEPTION( msError(out.str(),
                                               "Vector3d msEntity::getPosition(size_t indice) const",
                                               getFullId())
                                      );
            }
            return Elements[indice]->getPosition();
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msEntity::getPositions(vector<Vector3d>& coors) const
        {
            coors.resize(0);
            msChildren<msElement>::const_iterator it=Elements.begin();
            for(;it!=Elements.end();++it)      coors.push_back((*it)->getPosition());
            
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        Vector3d msEntity::getSeparation(int i0,int i1)
        {
            return( Elements[i1]->getPosition()-Elements[i0]->getPosition());
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msEntity::setPositions(const vector<Vector3d>& coors)
        {
            if( coors.size()!=Elements.size() ) {
                
                BOOST_THROW_EXCEPTION( msError("Size of 'coors not' consistant",
                                               "void msEntity::setPositions(const vector<Vector3d>& coors)",
                                               getFullId())
                                      );
            }
            msChildren<msElement>::iterator it=Elements.begin();
            vector<Vector3d>::const_iterator itcoors=coors.begin();
            
            for(;it!=Elements.end() && itcoors!=coors.end();++it,++itcoors)
                (*it)->setPosition((*itcoors));
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        double msEntity::getMass() const
        {
            msChildren<msElement>::const_iterator it=Elements.begin();
            double m=0;
            for(;it!=Elements.end();++it)
            {
                m+=(*it)->getMass(getUnits()->getMass());
            }
            return m;
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msEntity::computeCartCoordinates()
        {
            LOGGER_ENTER_FUNCTION_DBG("void msEntity::computeCartCoordinates()",
                                      getFullId());
            updatePositions();
            msChildren<msElement>::const_iterator it=Elements.begin();
            
            if(OverallRotation!=0){
                
                stringstream out;out<<"Apply overall rotation correction:\n"<<*OverallRotation<<endl;
                LOGGER_WRITE(msLogger::DEBUG,out.str());
                applyTransformation(*OverallRotation);
            }
            
            if(OverallTranslation!=0) applyTranslation(*OverallTranslation);
            
            LOGGER_EXIT_FUNCTION2("void msEntity::computeCartCoordinates()");
        };
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msEntity::translateOriginAtCenterOfMass(const Vector3d& pt)
        {
            /*if( (pt[0]!=pt[0]) || (pt[1]!=pt[1]) || (pt[2]!=pt[2]) )
             throw msError() << msErrorInfo("Nan providen")<<boost::errinfo_api_function("msEntity::setCenterOfMass");*/
            
            msChildren<msElement>::iterator it=Elements.begin();
            Vector3d cdg(0,0,0); double mtot=0;
            
            for(;it!=Elements.end();++it) {
                
                double m = (*it)->getMass(getUnits()->getMass());
                mtot += m;
                cdg += (*it)->getPosition() * m ;
            }
            applyTranslation(pt-cdg/mtot);
            
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msEntity::applyTranslation(const Vector3d& v)
        {
            msChildren<msElement>::iterator it=Elements.begin();
            for(;it!=Elements.end();++it)  (*it)->applyTranslation(v);
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msEntity::applyTransformation(const Eigen::Matrix3d& mat)
        {
            msChildren<msElement>::iterator it=Elements.begin();
            for(;it!=Elements.end();++it)  (*it)->applyTransformation(mat);
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        vector<double> msEntity::getEigenInertia() const {
            
            Eigen::Vector3d eval;
            Eigen::Matrix3d evect;
            computeEigenInertia(evect, eval);
            std::vector<double> evals;
            evals.push_back(eval[0]); evals.push_back(eval[1]); evals.push_back(eval[2]);
            return evals;
        }
        
        void msEntity::computeEigenInertia(Eigen::Matrix3d& evect,Eigen::Vector3d& eval) const {
            
            evect=Eigen::Matrix3d::Zero();
            eval=Eigen::Vector3d::Zero();
            
            if(Elements.size()==0) return;
            
            Eigen::Matrix3d Inertia=Matrix3d::Zero();
            
            double A,B,C,D,E,F;A=B=C=D=E=F=0; double x,y,z,m;
            
            msChildren<msElement>::const_iterator it=Elements.begin();
            
            for(;it!=Elements.end();++it)
            {
                m=(*it)->getMass(getUnits()->getMass());
                x=(*it)->getPosition()[0];
                y=(*it)->getPosition()[1];
                z=(*it)->getPosition()[2];
                A+=m*(y*y+z*z);B+=m*(x*x+z*z);C+=m*(y*y+x*x);
                D-=m*(z*y);E-=m*(x*z);F-=m*(x*y);
            }
            Inertia<<A,F,E,F,B,D,E,D,C;
            Eigen::EigenSolver<Eigen::Matrix3d> eigensolver(Inertia);
            for( size_t i=0; i<3 ;i++ ){
                
                eval[i]= eigensolver.eigenvalues()[i].real();
                for( size_t j=0; j<3 ; j++ ) evect.col(i)[j] = eigensolver.eigenvectors().col(i)[j].real();
            }
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        double& msEntity::Dof(size_t i)
        {
            if( i>Dofs.size() )
            {
                LOGGER_ENTER_FUNCTION("double& msEntity::Dof(size_t i) ",getFullId());
                LOGGER_WRITE(msLogger::ERROR,"Indice out of bounds");
                LOGGER_EXIT_FUNCTION();
                
                return Dofs[0];
            }
            return Dofs[i];
        }
        
        //-----------------------------------------------------------------------------
        
        const double& msEntity::Dof(size_t i) const
        {
            if( i>Dofs.size() )
            {
                LOGGER_ENTER_FUNCTION("double& msEntity::Dof(size_t i) ",getFullId());
                LOGGER_WRITE(msLogger::ERROR,"Indice out of bounds");
                LOGGER_EXIT_FUNCTION();
                return Dofs[0];
            }
            return Dofs[i];
        };
        
        //-----------------------------------------------------------------------------
        
        std::vector<double> msEntity::getDofs() const {
            
            return Dofs;
        }
        
        //-----------------------------------------------------------------------------
        
        void msEntity::setDof(size_t i,double d)
        {
            if( i>Dofs.size() )
            {LOGGER_ENTER_FUNCTION("void msEntity::setDof(size_t i,double d) ",getFullId());
                LOGGER_WRITE(msLogger::ERROR,"Indice out of bounds");
                LOGGER_EXIT_FUNCTION();
                return;
            }
            Dofs[i]=d;
            updatePositions();
        };
        
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        std::ostream& msEntity::print(std::ostream& out) const
        {
            msPhysicalInterface::print(out);
            output::printHeader(out,"Entity");
            
            msChildren<msElement>::const_iterator it=Elements.begin();
            out<<"Number of elements "<<Elements.size()<<endl;
            for(;it!=Elements.end();++it)
            {
                out<<(*it)->getId()<<" "<<(*it)->getPosition()[0]<<" "
                <<(*it)->getPosition()[1]<<" "
                <<(*it)->getPosition()[2]<<" ";
                out<<endl;
            }
            
            out<<"\nTotal mass: "<<getMass()<<" "<<getUnits()->getMassStr()<<". \n Principal moment of inertia: ";
            Vector3d EVal;Matrix3d EVect;
            computeEigenInertia(EVect,EVal);
            out<<"eigenvalues:\n"<<EVal<<"\n eigenvectors: \n"<<EVect<<endl;
            return out; 
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
    }
}


