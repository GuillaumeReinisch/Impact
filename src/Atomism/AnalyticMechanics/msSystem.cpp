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


#include "msSystem.h"

namespace impact {
    
    namespace atomism {
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        // msSystem
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        bool msSystem::isSystemRegisteredInPython=0;
        
        msRegistrar msSystem::Registrar("msSystem", msSystem::createInstance);
        
        std::ostream& operator<<(std::ostream& out,const msSystem& system)
        {
            system.print(out);
            return out;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msSystem::registryInPython()
        {
#if USE_PYTHON
            msPhysicalInterface::registryInPython();
            
            if( ! msSystem::isSystemRegisteredInPython ) {
                
                using namespace boost::python;
                class_<msSystem,bases<msPhysicalInterface>,boost::shared_ptr<msSystem> >
                ("msSystem",
                 "A container for entities. It manages the DoF and their variation.",no_init
                 )
                .def( "New", &msSystem::New , "Create a new object. Arg1: msUnitsManager object")
                .staticmethod("New")
                .def( "computeCartCoordinates", (void (msSystem::*)()) &msSystem::computeCartCoordinates , "compute the positions  of the elements")
                .def( "minSeparation", &msSystem::minSeparation , "return the separation between the two elements being the closest")
                .def( "addEntity" , &msSystem::addEntity , "Add an entity to the system")
                .def( "getEntity" , (boost::shared_ptr<msEntity>(msSystem::*)(size_t) const) &msSystem::getEntity,
                     "return the entity of a particular indice. arg2 : indice of the entity (int)")
                .def( "getEntity" , (boost::shared_ptr<msEntity>(msSystem::*)(std::string) const) &msSystem::getEntity,
                     "return the entity of a particular id. arg2 : id of the entity (string)")
                .def( "separation" ,  &msSystem::separation,
                     "return the separation between 2 elements. arg2, arg3 : indice of the 2 elements")
                .def( "angle" ,  &msSystem::angle,
                     "return the angle between 3 elements. arg2, arg3, arg4 : indice of the 3 elements")
                .def( "dihedral" ,  &msSystem::dihedral,
                     "return the dihedral angle between 4 elements. arg2, arg3, arg4, arg5 : indice of the 4 elements")
                .def(self_ns::str(self_ns::self));
                msSystem::isSystemRegisteredInPython=1;
            }
#endif
        }
        
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msSystem::addEntity(boost::shared_ptr<msEntity> entity)
        {
            if( EntitiesMap.find(entity->getId()) != EntitiesMap.end() )
            {LOGGER_ENTER_FUNCTION("void msSystem::addEntity(boost::shared_ptr<msEntity> entity)",getFullId());
                LOGGER_WRITE(msLogger::ERROR,"entity of the same id already exists");
                LOGGER_EXIT_FUNCTION();
                return mySharedPtr();
            }
            if( !entity->hasParent()) entity->setAffiliation(mySharedPtr());
            
            OverallTranslation=Eigen::Vector3d::Zero();
            OverallRotation=Eigen::Matrix3d::Zero();
            OverallRotation(0,0)=1;OverallRotation(1,1)=1;OverallRotation(2,2)=1;
            entity->setOverallOperators(&OverallTranslation,&OverallRotation);
            
            msTreeMapper::addElementToChildren<msEntity>(Entities,entity);
            EntitiesMap[entity->getId()]=entity;
            
            return mySharedPtr();
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msSystem::computeCartCoordinates()
        {
            LOGGER_ENTER_FUNCTION_DBG("void msSystem::computeCartCoordinates(Vector3dsContainer& vec)",getFullId());
            
            msChildren<msEntity>::iterator it=Entities.begin();
            
            try{
                for(;it!=Entities.end();++it)  (*it)->computeCartCoordinates();
            }
            catch( msException& e ) {
                
	        e.addContext("void msSystem::computeCartCoordinates()");
                IMPACT_THROW_EXCEPTION(e);
            }
            
            if( msLogger::isDebug() ){
                
                ElementItr elem = beginElements();
                
                for( int ielem=0; elem!= endElements() ; elem++ , ielem++ )
		  
                    if( (*elem).getPosition() != (*elem).getPosition() )
		      
                        throw msException("Nan in an element's coordinate located", "void msSystem::computeCartCoordinates()",getFullId());
            }
            LOGGER_EXIT_FUNCTION();
        }
        //-------------------------------------------------------------------------------------------------
        
        void msSystem::computeCartCoordinates(msVector3dContainer& vec) {
            
            computeCartCoordinates();
            ElementItr it=beginElements();
            vec.resize(noOfElements());
	    
	    double convLength = 1;
	    
            for(int i=0; it!=endElements();++it,++i) {
	      
	        convLength =  getUnits()->convert(*it,msUnit::vLength,1);
		
                vec[i]= convLength * (*it).getPosition();
            }
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double  msSystem::minSeparation() {
            
            double min=1e10;
            msVector3dContainer vec;
            computeCartCoordinates(vec);
            for( size_t i=0; i< vec.size(); ++i)
                for( size_t j=i+1; j< vec.size(); ++j)
                    if( (vec[i]-vec[j]).norm() < min ) min = (vec[i]-vec[j]).norm();
            return min;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msSystem::separation(size_t i,size_t j) {
            
            if( (i>=noOfElements()) || (j>=noOfElements()) )
                IMPACT_THROW_EXCEPTION( msException("atom's indice out of bound",
                                               "double msSystem::separation(size_t i,size_t j)", getFullId()) );
            
            return ( (getPosition(i)-getPosition(j)).norm() );
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msSystem::angle(size_t i,size_t j,size_t k){
            
            if( (i>=noOfElements()) || (j>=noOfElements()) || (k>=noOfElements()) )
                IMPACT_THROW_EXCEPTION( msException("atom's indice out of bound",
                                               "double msSystem::angle(size_t i,size_t j,size_t k)", getFullId()) );
            Vector3d a0 = getPosition(j) - getPosition(i);
            Vector3d a1 = getPosition(j) - getPosition(k);
            
            return ( getUnits()->convert(msUnit::Radian, acos(a0.dot(a1)/ (a0.norm()*a1.norm())) ) );
        }
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msSystem::dihedral(size_t i,size_t j,size_t k,size_t l){
            
            if( (i>=noOfElements()) || (j>=noOfElements()) )
                IMPACT_THROW_EXCEPTION( msException("atom's indice out of bound",
                                               "double msSystem::separation(size_t i,size_t j)", getFullId()) );
            
            Vector3d v0 = getPosition(i);
            Vector3d v1 = getPosition(j);
            Vector3d v2 = getPosition(k);
            Vector3d a=v0-v1;Vector3d b=v2-v1;
            Vector3d v3 = getPosition(l);
            Vector3d c = v3-v2;
            a.normalize(); b.normalize(); c.normalize();
            double dih = csts::pi+atan2((a*b.norm()).dot(b.cross(c)),(a.cross(b)).dot(b.cross(c)));
            if( dih>csts::pi ) dih-=2*csts::pi;
            
            return ( getUnits()->convert(msUnit::Radian,-dih) );
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        size_t msSystem::noOfElements() const
        {
            int n=0; msChildren<msEntity>::const_iterator it=Entities.begin();
            
            for(;it!=Entities.end();++it)
                n += (*it)->noOfElements();
            
            return n;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        size_t msSystem::noOfDof()const {
            
            int sum = 0;
            for(size_t i=0;i<Entities.size();i++) sum +=Entities[i]->noOfDof();
            return sum;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        size_t msSystem::noOfEntities() const { return Entities.size();
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msSystem::getValueOfDofs(vector<double>& dof) const
        {
            LOGGER_ENTER_FUNCTION_DBG("void msSystem::getValueOfDofs(vector<double>& dof) const",getFullId());
            
            dof.resize(noOfDof());
            
            const_DofItr it = beginDofs();
            for( int i=0;it!=endDofs();++it,++i)  dof[i]=*(it);
            
            LOGGER_EXIT_FUNCTION();
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        bool msSystem::getDeltaDofs( msVectorOfDofs& result, msVectorOfDofs& Dof0 ) {
            
            LOGGER_ENTER_FUNCTION("bool msSystem::getDeltaDofs( Vector3dsContainer& result,Vector3dsContainer& Dof0 )",getFullId());
            
            result.resize(noOfDof());
            if( Dof0.size()  != result.size()) {
                LOGGER_WRITE(msLogger::ERROR,"The argument Dof0 has a wrong size");
                LOGGER_EXIT_FUNCTION();
                return 1;
            }
            bool isNull=1;
            DofItr it = beginDofs();
            for( int i=0;it!=endDofs();++it,++i) {
                
                result[i]=*(it)-Dof0[i];
                if( (isNull) && (result[i]!=0) ) isNull=0;
            }
            if(isNull) LOGGER_WRITE(msLogger::WARNING,"A null variation of the Dofs has been generated");
            LOGGER_EXIT_FUNCTION();
            return isNull;
        }
        
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        Vector3d msSystem::getPosition(size_t i) {
            
            ElementItr it=beginElements();
            return( (*(it+=i)).getPosition());
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msSystem::getPositions(msVector3dContainer& coors) const
        {
            coors.resize(noOfElements());
            
            const_ElementItr elem = beginElements();
            
            for( int ielem=0; elem!= endElements() ; elem++ , ielem++ )
                
                coors[ielem]=(*elem).getPosition()*getUnits()->convert( elem.getEntity()->getUnits()->getLength(), 1);
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msSystem::writeXYZ(ostream& file,string comment) const
        {
            const_ElementItr elem = beginElements();
            file<<noOfElements()<<endl;
            file<<comment<<endl;
            double c = getUnits()->convert( elem.getEntity()->getUnits()->getLength(), 1);
            
            for( int ielem=0; elem!= endElements() ; elem++ , ielem++ )
                
                file<<(*elem).getId()<<" "<<c*(*elem).getPosition()[0]<<" "<<(*elem).getPosition()[1]<<" "<<(*elem).getPosition()[2]<<endl;
            
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        boost::shared_ptr<msEntity> msSystem::getEntity(size_t i) const
        {
            if( i>= Entities.size() ) {
                IMPACT_THROW_EXCEPTION( msException("indice out of bounds"
                                               ,"boost::shared_ptr<msEntity> msSystem::getEntity(size_t i) const ",getFullId()) );
            }
            return Entities[i].getSharedPtr();
        }
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        boost::shared_ptr<msEntity> msSystem::getEntity(std::string id) const
        {
            if( EntitiesMap.find(id) == EntitiesMap.end() ){
                IMPACT_THROW_EXCEPTION( msException("entity of the given id does not exist"
                                               ,"boost::shared_ptr<msEntity> msSystem::getEntity(std::string id) const",getFullId()) );
                return boost::shared_ptr<msEntity>();
            }
            return EntitiesMap.at(id);
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        msKineticWork msSystem::kineticWork(const msVector3dContainer& Coor0,
                                            const msVector3dContainer& Coor1) const
        {
            msKineticWork KinWork;  double mtot=0;
            KinWork.dJ=Eigen::Vector3d::Zero();
            KinWork.dP=Eigen::Vector3d::Zero();
            KinWork.dEkin=0;
            const_ElementItr it=beginElements();
            it=beginElements();
            for(int i=0; it!=endElements();++it,++i) {
                
                double m=(*it).getMass(getUnits()->getMass());
                KinWork.dP    += ( Coor1[i]-Coor0[i] ) * m;
                mtot += m;
                KinWork.dJ    += m * Coor0[i].cross(Coor1[i]-Coor0[i]);
                KinWork.dEkin += 0.5 * m * ( Coor1[i] - Coor0[i] ).dot( Coor1[i] - Coor0[i]);
            }
            
            if(KinWork.dJ!=KinWork.dJ) {
                
                stringstream s;s<<"Probem with atom's displacment\n";
                it=beginElements();
                for(int i=0; it!=endElements();++it,++i) {
                    
                    double m=(*it).getMass(getUnits()->getMass());
                    s<<"mass="<<m<<" coor0="<<Coor0[i][0]<<" "<<Coor0[i][1]<<Coor0[i][2]<<"          "
                    <<" coor1="<<Coor1[i][0]<<" "<<Coor1[i][1]<<Coor1[i][2]<<"          "
                    <<" displ="<<(Coor1[i]-Coor0[i])[0]<<" "<<(Coor1[i]-Coor0[i])[1]<<(Coor1[i]-Coor0[i])[2]<<endl;
                }
                IMPACT_THROW_EXCEPTION( msException(s.str()
                                               ,"msKineticWork msSystem::kineticWork(const msVector3dContainer& Coor0, const msVector3dContainer& Coor1) const"
                                               ,getFullId() ) );
            }
            return KinWork;
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        double msSystem::getMass() const {
            
            double mtot=0;
            const_ElementItr it=beginElements();
            for(;it!=endElements();++it) mtot += (*it).getMass(getUnits()->getMass());
            
            return mtot;
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        Vector3d msSystem::getCenterOfMass() const {
            
            LOGGER_ENTER_FUNCTION_DBG("Vector3d msSystem::getCenterOfMass() const",getFullId());
            
            const_ElementItr it=beginElements(); Vector3d cdg(0,0,0); double mtot=0;
            
            for(;it!=endElements();++it){
                
                double m = (*it).getMass(getUnits()->getMass());  mtot+=m;
                cdg += (*it).getPosition() * m;
            }
            cdg/=mtot;
            LOGGER_EXIT_FUNCTION();
            return cdg;
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        Vector3d msSystem::getDeltaCenterOfMass(const msVector3dContainer& coors1,
                                                const msVector3dContainer& coors0 )
        const {
            
            LOGGER_ENTER_FUNCTION("Vector3d msSystem::getDeltaCenterOfMass(...) const",getFullId());
            if( (coors1.size() != noOfElements())
               ||(coors1.size() != noOfElements()) ) {
                
                IMPACT_THROW_EXCEPTION( msException("size of the coor vector not compatible with the number of elements"
                                               ,"Vector3d msSystem::getDeltaCenterOfMass(const msVector3dContainer& coors1, const msVector3dContainer& coors0 ) const"
                                               ,getFullId() ) );
            }
            Vector3d cdg0,cdg1;
            cdg0=Vector3d::Zero(); cdg1=Vector3d::Zero();
            double m ; double mtot=0;
            const_ElementItr it=beginElements();
            for(int i=0;it!=endElements();++it,++i){
                
                m     = (*it).getMass(getUnits()->getMass());
                mtot += m;
                cdg0  += coors0[i] * m; cdg1  += coors1[i] * m;
            }
            LOGGER_EXIT_FUNCTION();
            return (cdg1-cdg0)/mtot;
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msSystem::computeEigenInertia(Eigen::Matrix3d& evect,Eigen::Vector3d& eval)const {
            
            evect= Eigen::Matrix3d::Zero();
            eval = Eigen::Vector3d::Zero();
            if(noOfElements()==0) return;
            Eigen::Matrix3d Inertia=Matrix3d::Zero();
            
            double A,B,C,D,E,F;A=B=C=D=E=F=0; double x,y,z,m;
            
            const_ElementItr it=beginElements();
            
            for(;it!=endElements();++it) {
	      
                double convLength = getUnits()->convert((*it).getUnits()->getLength(),1);
                m=(*it).getMass(getUnits()->getMass());
                x=(*it).getPosition()[0] * convLength;
                y=(*it).getPosition()[1] * convLength;
                z=(*it).getPosition()[2] * convLength;
                A+=m*(y*y+z*z);B+=m*(x*x+z*z);C+=m*(y*y+x*x);
                D-=m*(z*y);E-=m*(x*z);F-=m*(x*y);
            }
            Inertia<<A,F,E,F,B,D,E,D,C;
            Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver(Inertia);
            evect =  eigensolver.eigenvectors() ;
            eval =  eigensolver.eigenvalues();
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msSystem::annihil_dP(const msVector3dContainer& Coors0,
                                  msVector3dContainer& Coors1,
                                  Vector3d* trans
                                  )
        {Vector3d transDefault = Vector3d::Zero();
            
            transDefault=getDeltaCenterOfMass(Coors1,Coors0);//-getCenterOfMass(Coors0);
            ElementItr elem = beginElements();
            
            for( int ielem=0; elem!= endElements() ; elem++ , ielem++ ) {
                
                Coors1[ielem] -= transDefault;
                (*elem).setPosition( Coors1[ielem] );
            }
            
            msKineticWork work=kineticWork(Coors0,Coors1);
            if( work.dP.norm() >1e-10) {
                
                IMPACT_THROW_EXCEPTION(msException("the total linear momentum has not be annihiled",
                                              "void msSystem::annihil_dP(const msVector3dContainer& Coors0, msVector3dContainer& Coors1, Vector3d* trans)",
                                              getFullId()));
            }
            if( trans!=0 ) *trans=transDefault;
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        msKineticWork msSystem::annihil_dJ(const msVector3dContainer& Coors0,
                                           msVector3dContainer& Coors1,
                                           Matrix3d* Mat
                                           )
        {
            LOGGER_ENTER_FUNCTION("void msSystem::annihil_dJ(...)",getFullId());
            
            Matrix3d matDefault = Matrix3d::Zero();
            if( Mat==0 ) Mat=&matDefault;
            
            double dteta=0.01;  double eps=1e-10;
            Matrix3d MatRot = Matrix3d::Zero();
            
            msKineticWork Work  = kineticWork(Coors0,Coors1);
            msKineticWork Work0 = Work;
            
            int MaxStep;  getParameters()->queryIntValue("MaxStep",MaxStep);
            double res;   getParameters()->queryDoubleValue("ResEkin",res,*getUnits());
            double error = 1;
            
            ElementItr it=beginElements(); int n_it=0;
            LOGGER_WRITE(msLogger::INFO,"maximum number of step: "+output::getString<int>(MaxStep)+" residue: "+output::getString<double>(res));
            LOGGER_HEADER4COLUMNS(msLogger::INFO,"it","",
                                  "dEc"," "/*getStringUnit(strunit::dKinEner,this)*/
                                  ,"dJ"," "/*getStringUnit(strunit::dJ,this)*/
                                  ,"dP"," "/*getStringUnit(strunit::dP,this)*/);
            LOGGER_WRITE4COLUMNS(msLogger::INFO,n_it,Work.dEkin,Work.dJ.norm(),Work.dP.norm());
            
            while( (Work0.dJ.norm()>eps) && (Work0.dEkin>eps) && (error>res) && (n_it<MaxStep) ) {
                
                Vector3d axe=Work0.dJ;
                axe.normalize();
                MatRot = AngleAxisd(dteta/180*3.14,axe);
                
                Eigen::Vector3d dJ_ext=Eigen::Vector3d::Zero();
                it=beginElements();
                for(int i=0;it!=endElements();++it,++i) dJ_ext += Coors1[i].cross( MatRot*Coors1[i] - Coors1[i] )
                    * (*it).getMass(getUnits()->getMass());
                
                double teta = dteta * Work0.dJ.norm() / dJ_ext.norm();
                if( Work0.dJ.dot(dJ_ext) > 0 ) teta=-teta;
                
                MatRot = AngleAxisd(teta/180*3.14,axe);
                
                if( Mat!=0 ) (*Mat)= MatRot * (*Mat); 
                
                it=beginElements();
                for(int i=0;it!=endElements();++it,++i) Coors1[i] = MatRot * Coors1[i]; 
                
                Work=kineticWork(Coors0,Coors1);
                
                LOGGER_WRITE4COLUMNS(msLogger::INFO,n_it,Work.dEkin,Work.dJ.norm(),Work.dP.norm());
                
                error = fabs( (Work.dEkin - Work0.dEkin)/Work0.dEkin );
                Work0=Work; n_it++;
            }
            
            if( n_it>=MaxStep ) LOGGER_WRITE(msLogger::ERROR,"can not annihil overall rotation, if dEc is very small may be ok");
            if( Work.dEkin != Work.dEkin) LOGGER_WRITE(msLogger::ERROR,"non algebric number for the kinetic energy, return 0");
            /*
             if( Mat!=0 )
             { 
             LOGGER_WRITE(msLogger::INFO,"rotation matrix updated");
             (*Mat)*=MatRot; 
             }*/
            LOGGER_EXIT_FUNCTION();
            return Work;
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        ostream&  msSystem::print(ostream& out) const
        {
            msPhysicalInterface::print(out);
            output::printHeader(out,"msSystem");
            
            out<<"Number of entities: "<<noOfEntities()<<endl<<endl;
            
            Eigen::Matrix3d evect;Eigen::Vector3d eval; computeEigenInertia(evect,eval);
            
            out<<"Total mass: "<<getMass()<<" "<<getUnits()->getMassStr()
            <<"; principal mmt of inertia:"<<eval[0]<<", "<<eval[1]<<" and "<<eval[2]<<" "<<getUnits()->getLengthStr()<<"^2"<<getUnits()->getMassStr()<<endl;
            
            msChildren<msEntity>::const_iterator it=Entities.begin();
            for(;it!=Entities.end();++it)
                out<<"Entity "<<(*it)->getId()<<" of type "<<(*it)->getType()
                <<";\n\tnumber of degrees of freedom: "<<(*it)->noOfDof()
                <<"\n\tnumber of elements: "<<(*it)->noOfElements()
                <<endl;
            return out;
        }
    }
    
}
