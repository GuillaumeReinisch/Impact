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

#include "msZMat.h"

namespace impact {
    
    namespace atomism {
        
        bool msZMat::isZMatRegisteredInPython=0;
        msRegistrar msZMat::Registrar("msZMat", msZMat::createInstance);
        
        
        void msZMat::registryInPython() {
            
#if USE_PYTHON
            msEntity::registryInPython();
            if( ! msZMat::isZMatRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msZMat,bases<msEntity>,boost::shared_ptr<msZMat> >
                ("msZMat",
                 "Define a container of elements of type 'z-matrix'",no_init)
                .def( "New", &msZMat::New , "Create a new object. Arg1: msUnitsManager object")
                .staticmethod("New")
                .def( "set" , &msZMat::set, "Set the Z-Matrix from a string")
                .def( "setFromCartesians", &msZMat::setFromCartesians,
                     "set an the zmat using cartesian coordinates using an automatic chemical connecty scheme from cartesian coordinates. arg2: list of the atom's names, arg3: list of x coordinates, arg3: list of y coordinates, arg4: list of z coordinates.")
                .def( "setCenterOfGravity" , (void(msZMat::*)(double,double,double)) &msZMat::setCenterOfMass, "Set the center of gravity (dofs [nDof-6 to nDof-4]). Arg 1,2,3: x,y,z")
                .def( "setRndEulerAngles" ,  &msZMat::setRndEulerAngles , "Set the euler angle of the global orientation randomly (dofs [nDof-3 to nDof-1]).")
                .def( "setEulerAngles" ,  &msZMat::setEulerAngles ,
                     "Set the euler angle of the global orientation (dofs [nDof-3 to nDof-1]). Arg1: rotation around y, Arg2: rotation around z, Arg3: rotation around the the vector obtained from (1,0,0) ")
                .def( "setOutputFormat", &msZMat::setOutputFormat ,"Set the output format of the ZMat (Internal, XYZ or Formal")
                .def( "getString", &msZMat::getString , "return the string representation")
                .def( "separation" , &msZMat::separation,
                     "return the separation between two atoms. arg2: indice of the first atom, arg3: indice of the second atom");
                enum_<msZMat::outputType>("outputType")
                .value("Internal", Internal)
                .value("XYZ", XYZ)
                .value("Formal", Formal)
                .export_values();
                
                msZMat::isZMatRegisteredInPython=1;
            }
#endif
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msZMat::set(std::string line) {
            
            LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msTreeMapper> msZMat::set(std::string line)",getFullId());
            
            IndCoors.clear();
            msEntity::reset();
            
            std::vector<string> strs;
            fillVecFromString<std::string>(line,strs," ");
            std::vector<string>::iterator it=strs.begin();
            enum var   { atom , indiceL , indiceA, indiceD , valueL, valueA, valueD };
            var type=atom;
            int indatom=0;
            reserveDofs(100);
            
            for(it=strs.begin(); it!=strs.end(); ++it)
                
                if( (*it!=" ") &&  (*it!="\n") && (*it!="") ) {
                    
                    int vartype=type;
                    try {
                        switch(vartype) {
                                
                            case atom : {
                                LOGGER_WRITE(msLogger::DEBUG,"Add atom of name "+(*it));
				boost::shared_ptr<msAtom> newatom = NewAtom(*it);
				newatom->setUnits(getUnits());
                                addElement( newatom );
                                type=indiceL;
                                indatom++;
			    }
                                break;
                            case indiceL :
                                IndCoors.push_back(boost::lexical_cast<int>(it->c_str()));
                                type=valueL;
                                break;
                            case indiceA :
                                IndCoors.push_back(boost::lexical_cast<int>(it->c_str()));
                                type=valueA;
                                break;
                            case indiceD :
                                IndCoors.push_back(boost::lexical_cast<int>(it->c_str()));
                                type=valueD;
                                break;
                            case valueL :
                                addDof( boost::lexical_cast<double>(it->c_str()), msUnit( getUnits()->getLengthStr() ) );
                                type=indiceA;
                                break;
                            case valueA :
                                addDof(boost::lexical_cast<double>(it->c_str()) , msUnit( getUnits()->getAngleStr()  ) );
                                type=indiceD;
                                break;
                            case valueD :
                                addDof(boost::lexical_cast<double>(it->c_str()) , msUnit( getUnits()->getAngleStr()  ) );
                                type=atom;
                                break;
                        }
                        if( (indatom==1) && (type==indiceL) ) type=atom;
                        if( (indatom==2) && (type==indiceA) ) type=atom;
                        if( (indatom==3) && (type==indiceD) ) type=atom;
                    }
                    catch (...) {
                        
                        msError e("Error while parsing the '"+string(it->c_str())+"' for the atom number "+output::getString<int>(indatom),
                                  "boost::shared_ptr<msTreeMapper> msZMat::set(std::string line)",
                                  getFullId() );
                        BOOST_THROW_EXCEPTION(e);
                        
                    }
                    
                }
            
            addDof(0,msUnit( getUnits()->getLengthStr() ));
            addDof(0,msUnit( getUnits()->getLengthStr() ));
            addDof(0,msUnit( getUnits()->getLengthStr() ));
            
            addDof(0,msUnit( getUnits()->getAngleStr()  ));
            addDof(0,msUnit( getUnits()->getAngleStr()  ));
            addDof(0,msUnit( getUnits()->getAngleStr()  ));
            setCenterOfMass(0,0,0);
            setEulerAngles(0,0,0);
            LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msZMat::set(std::string line)");
            return mySharedPtr();
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msZMat::updatePositions() {
            
            LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msTreeMapper> msZMat::updatePositions()",getFullId());
            
            Vector3d axis2, axis, v1, v2, v3 , vperp;
            Matrix3d Rot,rotx, roty, rotz, rot;
            
            double 	l, angle, dihedre;
            int 		ind, indatom;
            msChildren<msElement>::iterator it=msEntity::beginElements();
            InversedPlan.clear();
            indatom=0;
            double ToRadian = 1 / getUnits()->convert( msUnit::Radian, 1 );
            double eps=1e-5;
            
            LOGGER_WRITE(msLogger::DEBUG,"Dofs values:" + msTreeMapper::getAttributeInString("Dofs") );
            
            for(;it!=msEntity::endElements();++it) {
                
	        double convLength = 1./getUnits()->convert(*(*it),msUnit::vLength,1);
		
                switch(indatom) {
                        
                    case 0 : (*it)->setPosition(0,0,0);      break;
                    case 1 : (*it)->setPosition(convLength * Dof(0),0,0); break;
                    case 2 :
                        ind 	= IndCoors[1];
                        l 	        = Dof(1);
                        angle 	= Dof(2) * ToRadian;
                        try{
                            (*it)->setPosition( getPosition(ind) +
						convLength * (1-2*ind) * (Eigen::Vector3d()<<l*cos(angle), l*sin(angle) , 0).finished() );
                        }
                        catch( msError& e ) {
                            
                            e.addContext("Z-Matrix unable to update positions (void msZMat::updatePositions())");
                            throw e;
                        }
                        
                        if(   ( fmod(angle,2*csts::pi) > csts::pi)
                           && ( fmod(angle,2*csts::pi) < 2*csts::pi) )
                            InversedPlan.push_back(msPlan(2,IndCoors[1],IndCoors[2]));
                        
                        break;
                    default :
                        ind	= getFirstIndiceOfAtom(indatom);
                        v1	= getSeparation(IndCoors[ind],IndCoors[ind+1]);
                        v2	= getSeparation(IndCoors[ind],IndCoors[ind+2]);
                        angle	= Dof(ind+1) * ToRadian;
                        dihedre	= Dof(ind+2) * ToRadian;
                        
                        if(  ( fmod(angle,csts::pi) < eps)
                           && ( fmod(angle,csts::pi) >-eps) ) {
                            
                            try{ if(  fmod(angle, 2*csts::pi) < eps )
                                ((*it)->setPosition( getPosition(IndCoors[ind]) - convLength * v1 * Dof(ind) / v1.norm() ));
                                
                                if(  fmod(angle+csts::pi,2*csts::pi) < eps )
                                    ((*it)->setPosition( getPosition(IndCoors[ind]) + convLength * v1 * Dof(ind) / v1.norm() ));
                            }
                            catch( msError& e ) {
                                
                                e.addContext("Z-Matrix unable to update positions (void msZMat::updatePositions())");
                                throw e;
                            }
                        }
                        
                        if(  v1.dot(v2)/v1.norm()/v2.norm()  == 1 ) {
                            v1[0]+=eps;v1[1]+=eps;v1[2]+=eps;
                            break;
                        }
                        if( (angle >csts::pi) && (angle<2*csts::pi) )  InversedPlan.push_back(msPlan(indatom,ind,IndCoors[ind+1]));
                        
                        if( isPlanInversed( IndCoors[ind+2], IndCoors[ind+1], IndCoors[ind] ) )  dihedre -= csts::pi;
                        
                        axis = v1; axis.normalize();
                        vperp= v1.cross(v2);vperp.normalize();
                        axis2 = AngleAxisd(dihedre,axis) * vperp;
                        v3 = ( AngleAxisd(angle,axis2) * axis) * Dof(ind);
                        if( msLogger::isDebug() ){
                            
                            double bend=acos( (v3).dot(v1)/(v3.norm()*v1.norm()));
                            double ratio1=fabs( max(angle,bend) / min(angle,bend) );
                            double ratio2=max(Dof(ind),v3.norm())/min(Dof(ind),v3.norm());
                            
                            if( (ratio1>1.0001) || (ratio2>1.0001) )
                                BOOST_THROW_EXCEPTION( msError("Unconsistent recalculated internal Dof ("+output::getString<double>(ratio1)
                                                               +" "+output::getString<double>(ratio2)
                                                               ,"void msZMat::updatePositions()",getFullId()) );
                        }
                        
                        try{
                            (*it)->setPosition( getPosition(IndCoors[ind]) + convLength * v3  );
                        }
                        catch( msError& e ) {
                            
                            e.addContext("Z-Matrix unable to update positions (void msZMat::updatePositions())");
                            throw e;
                        }
                        break;
                }
                LOGGER_WRITE(msLogger::DEBUG,"Coordinates of atom (element unit): " 
		             + output::getString<double>((*it)->getPosition()[0])+" "
                             + output::getString<double>((*it)->getPosition()[1])+" "
                             + output::getString<double>((*it)->getPosition()[2]));
                indatom++;
            }
            
            stringstream a;
            a<<GlobalOrientation[0]<<endl;
            
            LOGGER_WRITE(msLogger::DEBUG,"Apply overall rotation matrix defined by "+output::getString<double>(GlobalOrientation[0])+" "
                         +output::getString<double>(GlobalOrientation[1])+" "
                         +output::getString<double>(GlobalOrientation[2]) );
            
            roty = AngleAxisd(GlobalOrientation[0] * ToRadian, Vector3d::UnitY() );
            rotz = AngleAxisd(GlobalOrientation[1] * ToRadian, Vector3d::UnitZ() );
            rot  = AngleAxisd(GlobalOrientation[2] * ToRadian, rotz*roty*Vector3d::UnitX() );
            
            rot=rot*rotz*roty;
            if( (rot(0,0)!=rot(0,0)) || (rot(0,1)!=rot(0,1)) || (rot(0,2)!=rot(0,2)) ||
               (rot(1,0)!=rot(1,0)) || (rot(1,1)!=rot(1,1)) || (rot(1,2)!=rot(1,2)) ||
               (rot(2,0)!=rot(2,0)) || (rot(2,1)!=rot(2,1)) || (rot(2,2)!=rot(2,2)) ){
                
                throw msError("Rotation Matrix contains a nan","void msZMat::updatePositions()",getFullId());
            }
            
            for( it = beginElements(); it != endElements(); ++it)  (*it)->applyTransformation(rot);
            
            LOGGER_WRITE(msLogger::DEBUG,"Set center of mass");
            
            try{
                translateOriginAtCenterOfMass(CenterOfGravity);
            }
            catch( msError& e ) {
                
                e.addContext("can not set the center of mass (void msZMat::updatePositions())");
                throw e;
            }
            LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msZMat::updatePositions()");
            return mySharedPtr();
        }
        
        //-----------------------------------------------------------------------------
        
        void msZMat::setCenterOfMass( double x, double y, double z ) {
            
            CenterOfGravity<<x,y,z;
            Dof(noOfDof()-6)=x;
            Dof(noOfDof()-5)=y;
            Dof(noOfDof()-4)=z;
            updatePositions();
        };
        
        //-----------------------------------------------------------------------------
        
        void msZMat::setRndEulerAngles() {
            
            double alpha = getUnits()->convert( msUnit::Degree,  ( (double) rand() /(RAND_MAX))*360 );
            double beta  = getUnits()->convert( msUnit::Degree,  ( (double) rand() /(RAND_MAX))*360 );
            double gamma = getUnits()->convert( msUnit::Degree,  ( (double) rand() /(RAND_MAX))*360 );
            
            setEulerAngles(alpha,beta,gamma);
        }
        
        //-----------------------------------------------------------------------------
        
        void msZMat::setEulerAngles(double x,double y,double z) {
            
            GlobalOrientation<<x,y,z;
            Dof(noOfDof()-3)=x;
            Dof(noOfDof()-2)=y;
            Dof(noOfDof()-1)=z;
            updatePositions();
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msZMat::setChemConnectivity() {
            
            LOGGER_ENTER_FUNCTION_DBG("void msZMat::setChemConnectivity()",getFullId());
            IndCoors.clear();
            msChildren<msElement>::iterator it=beginElements()+1;
            int indatom=1;
            for(;it!=endElements();++it,indatom++) {
                
                if(indatom>=1) IndCoors.push_back(PrevClosest(indatom,indatom));
                if(indatom>=2) IndCoors.push_back(PrevClosest(IndCoors.back(),
                                                              indatom,IndCoors.back()));
                if(indatom>=3) IndCoors.push_back(PrevClosest(IndCoors.back(),
                                                              indatom,*(IndCoors.end()-2),
                                                              IndCoors.back()));
            }
            stringstream out;
            out<<"Indices of the connectivity: ";
            for(size_t i=0 ;i<IndCoors.size();i++)
                out<<IndCoors[i]<<" ";
            LOGGER_WRITE(msLogger::INFO,out.str());
            
            LOGGER_EXIT_FUNCTION2("void msZMat::setChemConnectivity()");
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        msAtom* msZMat::getAtom(size_t atom) const {
            
            LOGGER_ENTER_FUNCTION_DBG("msAtom* msZMat::getAtom(size_t atom) const",getFullId());
            
            if( atom> noOfElements() ) {
                
                msError e("Indice out of bounds; number of atoms: "+output::getString<int>(noOfElements()) ,
                          "msAtom* msZMat::getAtom(size_t atom) const",
                          getFullId() );
                BOOST_THROW_EXCEPTION(e);
            }
            LOGGER_EXIT_FUNCTION2("msAtom* msZMat::getAtom(size_t atom) const");
            return (msAtom*)(getElement(atom).get());
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        size_t msZMat::getFirstIndiceOfAtom(size_t atom) const {
            
            if( atom > noOfElements() )
            {LOGGER_ENTER_FUNCTION_DBG("int msZMat::getFirstIndiceOfAtom(int atom) const",getFullId());
                LOGGER_WRITE(msLogger::ERROR,"indice out of bound ("+output::getString<size_t>(atom)+")");
                LOGGER_EXIT_FUNCTION();
                return 0;
            }
            int r=0;
            switch(atom)
            {
                case 0 : r =-1; break;
                case 1 : r = 0; break;
                case 2 : r = 1; break;
                case 3 : r = 3; break;
                default : r = 3+(atom-3)*3;break;
            }
            return r;
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        size_t msZMat::PrevClosest(int at,int maxAt,int noAt,int noAt1)
        {
            double dmin=100000;int indAt=-1;
            
            for( int i=0 ; i<maxAt ; i++ ) {
                
                double d = ( getPosition(at)-getPosition(i) ).norm();
                if((d<dmin)&&(i!=noAt)&&(i!=noAt1)) { dmin = d; indAt= i;}
            }
            return(indAt);
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msZMat::setFromCartesians(vector<std::string> names, vector<double> xs, vector<double> ys, vector<double> zs) {
            
            LOGGER_ENTER_FUNCTION_DBG("void msZMat::setFromCartesian(vector<std::string> names, vector<double> xs, vector<double> ys, vector<double> zs)",getFullId());
            
            if( !((names.size()==xs.size()) && (names.size()==ys.size()) && (names.size()==zs.size()) )) {
                
                msError e("unconsistant size of the vectors given",
                          "void msZMat::setFromCartesian(vector<std::string> names, vector<double> xs, vector<double> ys, vector<double> zs)",
                          getFullId());
                
                BOOST_THROW_EXCEPTION(e);
            }
            
            IndCoors.clear();
            msEntity::reset();
            reserveDofs(100);
            
            for(int at=0;at<names.size();at++) {
                
                LOGGER_WRITE(msLogger::DEBUG,"Add atom of name "+names[at]);
                boost::shared_ptr<msAtom> atom;
                try{
                    addElement( atom = NewAtom( names[at] ) );
                }
                catch(msError& e){
                    
                    e.addContext("    boost::shared_ptr<msTreeMapper> msZMat::setFromCartesians(vector<std::string> names, vector<double> xs, vector<double> ys, vector<double> zs)");
                    BOOST_THROW_EXCEPTION(e);
                }
                catch(std::exception& e0) {
                    msError e("unexpected error while adding new atom"+string(e0.what()),
                              "void msZMat::setFromCartesian(vector<std::string> names, vector<double> xs, vector<double> ys, vector<double> zs)",
                              getFullId());
                    
                    BOOST_THROW_EXCEPTION(e);
                }
                LOGGER_WRITE(msLogger::DEBUG,"Set position of x="+output::getString<double>(xs[at])
                             + " y="+output::getString<double>(ys[at])
                             + " z="+output::getString<double>(zs[at]));
                atom->setPosition(xs[at],ys[at],zs[at]);
            }
            setChemConnectivity();
            setZmatFromCartesian();
            
            LOGGER_EXIT_FUNCTION2("void msZMat::setFromCartesian(vector<std::string> names, vector<double> xs, vector<double> ys, vector<double> zs)");
            return mySharedPtr();
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        
        void msZMat::setZmatFromCartesian() {
            
            LOGGER_ENTER_FUNCTION_DBG("void msZMat::setZmatFromCartesian()",getFullId());
            
            int n = noOfElements();
            reserveDofs(100);
            LOGGER_WRITE(msLogger::DEBUG,"Number of atom:"+output::getString<int>(n));
            stringstream out;
            out<<"Atom number 0 ; easy: no DoF";
            LOGGER_WRITE(msLogger::DEBUG,out.str());
            if( n >= 2 ) {
                int firstInd=getFirstIndiceOfAtom(1);
                stringstream out;
                out<<"Atom number 1 ; ref. first indice in DoFs: "<<firstInd<<"; atom indice:"<<IndCoors[firstInd];
                LOGGER_WRITE(msLogger::DEBUG,out.str());
                addDof( ( getPosition(IndCoors[firstInd]) - getPosition(1) ).norm() , msUnit( getUnits()->getLengthStr() ));
            }
            
            if( n >= 3 ) {
                
                int firstInd=getFirstIndiceOfAtom(2);
                stringstream out;
                out<<"Atom number 2 ; ref. first indice in DoFs: "<<firstInd<<"; atom indices:"<<IndCoors[firstInd]
                <<","<<IndCoors[firstInd+1];
                LOGGER_WRITE(msLogger::DEBUG,out.str());
                addDof( (getPosition(IndCoors[firstInd]) - getPosition(2) ).norm() , msUnit( getUnits()->getLengthStr() ));
                Vector3d v0=getPosition(2);
                Vector3d v1=getPosition(IndCoors[firstInd]);
                Vector3d v2=getPosition(IndCoors[firstInd+1]); ;
                
                Vector3d a=v0-v1;Vector3d b=v2-v1;
                double cosa = a.dot(b) / ( a.norm() * b.norm() );
                addDof( getUnits()->convert(msUnit::Radian,acos(cosa)) , msUnit( getUnits()->getAngleStr() ));
            }
            
            for(int at=3;at<n;at++) {
                
                int firstInd=getFirstIndiceOfAtom(at);
                stringstream out;
                out<<"Atom number "<<at<<"; ref. first indice in DoFs: "<<firstInd<<"; atom indices:"<<IndCoors[firstInd]
                <<","<<IndCoors[firstInd+1]<<","<<IndCoors[firstInd+2];
                LOGGER_WRITE(msLogger::DEBUG,out.str());
                addDof( ( getPosition(IndCoors[firstInd]) - getPosition(at) ).norm() , msUnit( getUnits()->getLengthStr() ));
                
                Vector3d v0 = getPosition(at);
                Vector3d v1 = getPosition(IndCoors[firstInd]);
                Vector3d v2=  getPosition(IndCoors[firstInd+1]);
                Vector3d a=v0-v1;Vector3d b=v2-v1;
                double cosa=a.dot(b)/(a.norm()*b.norm());
                addDof( getUnits()->convert(msUnit::Radian,acos(cosa)) , msUnit( getUnits()->getAngleStr() ));
                
                Vector3d v3 = getPosition(IndCoors[firstInd+2]);
                Vector3d c = v3-v2;
                a.normalize(); b.normalize(); c.normalize();
                double l=csts::pi+atan2((a*b.norm()).dot(b.cross(c)),(a.cross(b)).dot(b.cross(c)));
                if( l>csts::pi ) l-=2*csts::pi;
                addDof( getUnits()->convert(msUnit::Radian,-l) , msUnit( getUnits()->getAngleStr() ));
            }
            LOGGER_WRITE(msLogger::DEBUG,"Done with internal coordinates");
            addDof(0,msUnit( getUnits()->getLengthStr() ));
            addDof(0,msUnit( getUnits()->getLengthStr() ));
            addDof(0,msUnit( getUnits()->getLengthStr() ));
            
            addDof(0,msUnit( getUnits()->getAngleStr()  ));
            addDof(0,msUnit( getUnits()->getAngleStr()  ));
            addDof(0,msUnit( getUnits()->getAngleStr()  ));
            setCenterOfMass(0,0,0);
            setEulerAngles(0,0,0);
            LOGGER_EXIT_FUNCTION2("void msZMat::setZmatFromCartesian()");
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        double msZMat::separation(size_t i,size_t j) {
            
            if( (i>=noOfElements()) || (j>=noOfElements()) )
                BOOST_THROW_EXCEPTION( msError("atom's indice out of bound",
                                               "double msZMat::separation(size_t i,size_t j)",getFullId()) );
            
            Vector3d v0 = getPosition(i) - getPosition(j);
            return v0.norm();
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        bool msZMat::isPlanInversed(int i,int j,int k) {
            
            bool ok=0;
            
            for(size_t ip=0;ip<InversedPlan.size();ip++) {
                /*cout<<i<<" "<<j<<" "<<k<<endl;
                 cout<<InversedPlan[ip].i<<" "<<InversedPlan[ip].j<<" "<<InversedPlan[ip].k<<endl;*/
                if((i==InversedPlan[ip].i)&&(j==InversedPlan[ip].j)&&(k==InversedPlan[ip].k))
                {ok=1;}
                if((i==InversedPlan[ip].i)&&(k==InversedPlan[ip].j)&&(j==InversedPlan[ip].k))
                {ok=1;}
                if((j==InversedPlan[ip].i)&&(i==InversedPlan[ip].j)&&(k==InversedPlan[ip].k))
                {ok=1;}
                if((j==InversedPlan[ip].i)&&(k==InversedPlan[ip].j)&&(i==InversedPlan[ip].k))
                {ok=1;}
                if((k==InversedPlan[ip].i)&&(i==InversedPlan[ip].j)&&(j==InversedPlan[ip].k))
                {ok=1;}
                if((k==InversedPlan[ip].i)&&(j==InversedPlan[ip].j)&&(i==InversedPlan[ip].k))
                {ok=1;}
                if(ok)
                {ip=InversedPlan.size();}
                
            }
            return ok;
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msZMat::setOutputFormat(string type)  {
            
            PrintType=Internal;
            if(type=="XYZ") PrintType=XYZ;
            if(type=="Formal")PrintType=Formal;
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        size_t msZMat::noOfElectrons() const {
            
            int n=0;
            for(size_t i=0;i<noOfElements();i++)  n+=getAtom(i)->AtomicNb;
            return n;
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        ostream&  msZMat::writeFormal(ostream& out) const {
            
            for(size_t i=0;i<noOfElements();i++) {
                
                int firstInd=getFirstIndiceOfAtom(i);
                out<<getAtom(i)->getName();
                if( i>=1 ) out<<" "<<IndCoors[firstInd]+1<<" r"<<(i);
                if( i>=2 ) out<<" "<<IndCoors[firstInd+1]+1<<" a"<<(i-1);
                if( i>2  ) out<<" "<<IndCoors[firstInd+2]+1<<" d"<<(i-2);
                out<<endl;
            }
            out<<endl;
            for(size_t i=0;i<noOfElements();i++) {
                
                int firstInd=getFirstIndiceOfAtom(i);
                if( i>=1 ) out<<"r"<<(i)<<" = "<<Dof(firstInd)<<endl;
                if( i>=2 ) out<<"a"<<(i-1)<<" = "<<Dof(firstInd+1)<<endl;
                if( i>2  ) out<<"d"<<(i-2)<<" = "<<Dof(firstInd+2)<<endl;
            }
            return out;
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        std::string msZMat::getString() const {
            
            std::stringstream out;
            
            for(size_t i=0;i<noOfElements();i++) {
                
                int firstInd=getFirstIndiceOfAtom(i);
                
                out<<getAtom(i)->getName();
                if( i>=1 ) out<<"  "<<IndCoors[firstInd]<<"  "<<Dof(firstInd);
                if( i>=2 ) out<<"  "<<IndCoors[firstInd+1]<<"  "<<Dof(firstInd+1);
                if( i>2 )  out<<"  "<<IndCoors[firstInd+2]<<"  "<<Dof(firstInd+2);
                out<<endl;
            }
            return out.str();
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        ostream&  msZMat::print(ostream& out) const {
            
            msEntity::print(out);
            output::printHeader(out,"Z-Matrix");
            
            if( noOfElements()== 0 ) {
                
                out<<"Z-Matrix empty, use the method 'set' to define it"<<endl;
                return out;
            }
            
            if(PrintType==Formal) writeFormal(out);
            
            if(PrintType==Internal) {
                
                for(size_t i=0;i<noOfElements();i++) {
                    
                    int firstInd=getFirstIndiceOfAtom(i);
                    
                    out<<getAtom(i)->getName();
                    if( i>=1 ) out<<"  "<<IndCoors[firstInd]<<"  "<<Dof(firstInd);
                    if( i>=2 ) out<<"  "<<IndCoors[firstInd+1]<<"  "<<Dof(firstInd+1);
                    if( i>2 )  out<<"  "<<IndCoors[firstInd+2]<<"  "<<Dof(firstInd+2);
                    out<<endl;
                }
            }
            out<<"Properties of atoms : \n";
            size_t width=25;
            std::string mass="Mass["+getUnits()->getMassStr()+"]";
            std::string hf="Hf["+getUnits()->getEnergyStr()+"]";
            std::string vdw="vdW radius["+getUnits()->getLengthStr()+"]";
            
            out<<std::setw(width)<<"Atomic Nb"<<std::setw(width)<<mass<<std::setw(width)<<hf<<std::setw(width)<<vdw<<"\n";
            
            for(size_t i=0;i<noOfElements();i++)
                out<<std::setw(width)<<getAtom(i)->AtomicNb<<std::setw(width)<<getAtom(i)->getMass(getUnits()->getMass())
                <<std::setw(width)<<getAtom(i)->getHf(getUnits()->getEnergy())
                <<std::setw(width)<<getAtom(i)->getVdWRadius(getUnits()->getLength())<<"\n";
            
            
            return out;
        }
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        /*
         size_t msZMat::getAtomByIndice(size_t indice) {
         
         LOGGER_ENTER_FUNCTION("int msZMat::getAtomByIndice(int indice)",getFullId());
         
         if( indice > IndCoors.size() ) {
         
         LOGGER_WRITE(msLogger::ERROR,"indice out of bound ("+output::getString<size_t>(indice)+")");
         LOGGER_EXIT_FUNCTION();
         return -1;
         }
         
         switch(indice) {
         
         case 0:  return 1; break;
         case 1:  return 2; break;
         case 2:  return 2; break;
         default: return 2+int(indice/3); break;
         }
         LOGGER_EXIT_FUNCTION();
         return -1;
         }
         
         bool msZMat::areLinked(int at,int targ) {
         
         bool r1=1;bool r2=1;bool r3=1;
         if(at<targ){return 0;}
         if(at==targ){return 1;}
         
         int ind=getFirstIndiceOfAtom(at);
         r1=r1 & areLinked(IndCoors[ind],targ);
         r2=r2 & areLinked(IndCoors[ind+1],targ);
         r3=r3 & areLinked(IndCoors[ind+2],targ);
         
         if(r1||r2||r3){return(1);}
         else{return 0;}
         }
         
         msZMat::varType msZMat::getTypeOfDof(size_t indice) {
         
         LOGGER_ENTER_FUNCTION("msZMat::varType msZMat::getTypeOfDof(size_t indice)",getFullId());
         
         varType result;
         
         if( indice == 0 ) result=Length;
         if( indice == 1 ) result=Length;
         if( indice == 2 ) result=Angle;
         
         if( (indice>=3)
         && (std::size_t(indice)<IndCoors.size()) ) {
         
         int r=indice/3;
         int reste=indice-3*r;
         if(reste==0) result=Length;
         if(reste==1) result=Angle;
         if(reste==2) result=Dihedre;
         }
         
         if(  indice>=IndCoors.size() )
         {
         LOGGER_WRITE(msLogger::ERROR,"indice out of bound");
         }
         
         LOGGER_EXIT_FUNCTION();
         return result;
         }
         
         int msZMat::isBondedByCovalent(int i,int j) {
         
         if(i==j) return 0;
         msAtom* a1=getAtom(i);
         msAtom* a2=getAtom(j);
         double l=getSeparation(i,j).getNorme();
         int order=0;
         for( int k=0; k<3 ; k++) {
         
         double covlength = getCovalentLength(k,*a1,*a2);
         if(l<covlength) order=k;
         }
         return order;
         }
         
         void msZMat::updateBondStructure()  {
         
         int order; int i1=0 ; int i2=0 ;
         
         Structure3d->resize(Elements.size());
         msChildren<msElement>::iterator it1=Elements.beginElements();
         msChildren<msElement>::iterator it2=Elements.beginElements();
         
         for( ; it1 != Elements.end(); it1++,i1++ )
         
         for( it2=it1 ,i2=i1 ; it2 != Elements.end() ; it2++,i2++)
         
         if( (order=isBondedByCovalent(i1,i2))>0 ) {
         
         boost::shared_ptr<ms3dTube> bond1=boost::shared_ptr<ms3dTube>(new ms3dTube);
         boost::shared_ptr<ms3dTube> bond2=boost::shared_ptr<ms3dTube>(new ms3dTube);
         
         bond1->setGeometry((*it1)->getPosition(),
         (*it1)->getPosition()+getSeparation(i1,i2)/2.,
         0.05*isBondedByCovalent(i1,i2));
         bond1->setColor(getAtom(i1)->Color);
         Structure3d->add(bond1);
         
         bond2->setGeometry((*it1)->getPosition()+getSeparation(i1,i2)/2.,
         (*it2)->getPosition(),
         0.05*isBondedByCovalent(i1,i2));
         bond2->setColor(getAtom(i2)->Color);
         Structure3d->add(bond2);
         }
         
         
         }
         
         */
    }
}
