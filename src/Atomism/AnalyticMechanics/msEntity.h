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

//! \file msEntity.h definition of the class msEntity, msVectorOfDofs, and msVectorOfPtrDof and msKineticWork

#ifndef MSENTITY_H
#define MSENTITY_H
#include <msElement.h>
#include <msGeneralizedCoordinates.h>

namespace impact {
    
    namespace atomism {
        
        class msEntity;
        
        class msKineticOperator;
        
        class msJacobianOfDispl;
        
        class msVectorOfDofs : public  vector< double>
        {
        public:
            
            msVectorOfDofs() : vector<double>(){};
            
            msVectorOfDofs(int s) : vector<double>(s){};
            
            void operator+=(const msVectorOfDofs& rhs) {
                
                for(size_t i=0;i<min(size(),rhs.size());++i)
                    (*this)[i]+=rhs[i];
            }
            
            void operator-=(const msVectorOfDofs& rhs) {
                
                for(size_t i=0;i<min(size(),rhs.size());++i)
                    (*this)[i]-=rhs[i];
            }
            std::string getDofsStr(){
                
                stringstream s;
                for(size_t i=0;i<size();++i)  s<<(*this)[i]<<" ";
                return s.str();
            }
        };
        
        class msKineticWork
        {
        public:
            msKineticWork(){dEkin=0;};
            Eigen::Vector3d dP;
            Eigen::Vector3d dJ;
            double dEkin;
        };
        
        std::ostream& operator<<(std::ostream& out,const msKineticWork& inter);
        
        /** \class msEntity
         *
         * \brief A container for elements, computes the element's positions from the degrees of freedom
         *
         * The msEntity class manages elements (msElement), its purpose is to compute
         * the position of the elements from degrees of freedom (DoF).
         * This classes is virtual as the transformation between DoF to cartesian coordinates
         * is problem specific.
         * The derivation is however straigthforward, in particular because the msEntity derived
         * classes do not have to deal with global orientation.
         *
         * \section deriveEntity Deriving from msEntity
         *
         * Two steps are required to define a new entity: (i) defining a setter, and (ii) defining
         * a the virtual function updatePositions(). We take the example of a 2D chain of pendulum
         * to illustrate the points.  Because the class derived from msRegister, msTreeMapper,
         * and msPhysicalInterface, the base structure looks like this:
         * ~~~~~~~~~~~~~{.cpp}
         
         #include <msEntity.h>
         
         class myPendulum : public msEntity
         {
         
         private:
         
         static msRegistrar   Registrar;
         static bool  	   isPendulumRegisteredInPython;
         static msRegister*   createInstance() { return new myPendulum; }
         
         vector<double*>      Lengths;   //! points to the lengths DoF
         vector<double*>      Angles;    //! points to the angles DoF
         
         protected:
         
         void registryInPython();
         
         void initialize()     { msEntity::initialize(); }
         
         void update()         {
         msEntity::update(); Lengths.clear(); Angles.clear();
         
         for( size_t i=0 ; i<noOfDof(); ++i){
         
         if( (i%2) == 0 ) Lengths.push_back( &(Dof[i]) );
         else 	     Angles.push_back(  &(Dof[i]) );
         }
         }
         
         myPendulum() : msEntity() { constructVar("myPendulum","Pendulum","pendulum"); }
         
         public:
         
         static boost::shared_ptr<myPendulum> New( boost::shared_ptr<msUnitsManager> units ){
         
         boost::shared_ptr<myPendulum> T( new myPendulum );
         T->initialize(); T->setUnits(units); T->update();
         return T;
         }
         
         virtual boost::shared_ptr<msTreeMapper> clone() {
         boost::shared_ptr<msZMat> clone = New(getUnits());
         clone->msTreeMapper::operator=(*this);
         return clone;
         }
         
         std::ostream& print(std::ostream& out) const {
         
         msEntity::print(out);
         output::printHeader(out,"myPendulum");
         return( out<<"A description of the pendulum."<<endl );
         };
         };
         * ~~~~~~~~~~~~~
         * This example shows an implementation of the function update (msTreeMapper::update()).
         * The degrees of freedom of the class myPendulum are actually the degrees of freedom of the
         * pendulum  (as a physical object), and are already managed by msEntity:
         * no need to take care of them for declaration or update.
         * However, as a sugar trick, we want to organise the DoF between the two
         * vectors:
         *    - Lengths will store the length of the strings, we define that the length of the
         * string 'j' will be the Dof[2*j]
         *    - Angles their angle with the vertical,  we define that the angle of the
         * string 'j' will be the Dof[2*j+1]
         *
         * Those data does not represent additional information for the object structure, this is
         * just a reorganization; that's why we do not declare them to the tree mapper.
         *
         * \subsection constructEntity  Defining a setter
         *
         * The construction of msEntity derived classes, as it should be for every classes deriving
         * from msRegister, should not be initialize in the default constructor (except if the
         * construction is very light). We recommand to define the construction in a setter type
         * of method. \n
         *
         * The objectives of the setter is to declare the Dof and the elements, in our example:
         *
         * ~~~~~~~~~~~~~{.cpp}
         boost::shared_ptr<msTreeMapper> myPendulum::setStringsAndMasses(const std::vector<double>& lengthOfStrings,
         const std::vector<double>& masses) {
         
         clearEntity();
         reserveDofs(100);
         
         if( lengthOfStrings.size() != masses.size() )
         BOOST_THROW_EXCEPTION( msError("Unconsistent size between strings and masses: should be the same",
         "void myPendulum::setStringsAndMasses",
         getFullId())
         );
         
         for( size_t i=0; i<lengthOfStrings.size(); ++i){
         
         addElement( msElement::New( masses[i] , msUnit( getUnits()->getMassStr() )) );
         
         addDof( lengthOfStrings[i] , msUnit( getUnits()->getLengthStr() )) ); // add the length
         addDof( 0 		 , msUnit( getUnits()->getAngleStr()  )) ); // add the angle, init at 0
         if( i>=49 )
         BOOST_THROW_EXCEPTION( msError("number of masses limited at maximum 50",
         "void myPendulum::setStringsAndMasses",
         getFullId())
         );
         
         }
         
         update();
         
         return mySharedPtr();
         }
         * ~~~~~~~~~~~~~
         *
         * Note that you don't have to take care of units change for the Dof, this is handled
         * by the msEntity class.
         *
         * \subsection updateEntity  Computing the position of elements
         *
         *
         * The computation of the cartesian coordinates is defined in the updatePositions()
         * method. For the pendulum, each angle reference the angle between the associated string
         * and the vertical. The method becomes:
         * ~~~~~~~~~~~~~{.cpp}
         boost::shared_ptr<msTreeMapper> myPendulum::updatePositions() {
         
         ElementItr it=msEntity::begin();
         
         Vector3d lastPos(0,0,0); Vector3d x(1,0,0) ; Vector3d y(0,1,0);
         
         for(int i=0;it!=msEntity::end();++it,++i) {
         
         double l = *(Lengths[i]);
         double a =  getUnits()->ConvertTo(msUnit::Radian, *(Angles[i]) );
         
         (*it)->setPosition( lastPos += l * cos(a) * x + l * sin(a) * y
         )
         }
         return mySharedPtr();
         }
         * ~~~~~~~~~~~~~
         *
         * \subsection morePendulum  What do I need more to see my pendulum penduling ?
         *
         * You need to define the PES and you are done; the PES definition of the pendulum
         * is described in deriveScalarFunction. You would be able to do it in python using
         * the msParser object but it would be more painfull and less efficient.
         */
        class msEntity : public msPhysicalInterface
        {
            friend class msSystem;
            friend class DofIterator;
            
        private:
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isEntityRegisteredInPython;
            static msRegister* createInstance() { return new msEntity; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void update() { msPhysicalInterface::update();
            }
            
        public:
            
            //! Iterate sequentially over the elements
            typedef msChildren<msElement>::iterator 	    	ElementItr;
            
            //! Iterate sequentially over the elements. const version
            typedef msChildren<msElement>::const_iterator 	const_ElementItr;
            
            //! @name construction
            //@{
            msEntity() : msPhysicalInterface() {
                
                constructVar("msEntity","Entity","entity");
                OverallRotation=0;OverallTranslation=0;
            }
            
            void initialize() {
                
                msPhysicalInterface::initialize();
                declareAttribute(Dofs,"Dofs");
                declareChildren<msElement>(Elements,"Elements");
            }
            
            static boost::shared_ptr<msEntity> New(){
                
                boost::shared_ptr<msEntity> T( new msEntity );
                LOGGER_ENTER_FUNCTION_DBG("msEntity::New(boost::shared_ptr<msUnitsManager> units)",T->getFullId());
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msEntity> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            //! add an element
            boost::shared_ptr<msTreeMapper> addElement(boost::shared_ptr<msElement> elem);
            
            /** \bried add a degree of freedom
             *
             * Before calling this function you should have called reserve() to ensure
             * a correct behavior. You don't have to deal with units modification
             * for the Dof: this is handled by msEntity.
             */
            void addDof(double value, const msUnit& unit);
            //@}
            
            //! @name Dof and elements
            //@{
            //! set the Dof number 'i' to a value 'd'; return an exception if i > noOfDof()
            void setDof(size_t i,double d);
            
            //! return a reference on the Dof number 'i'; return an exception if i > noOfDof()
            double& Dof(size_t i);
            
            //! return a const reference on the Dof number 'i'; return an exception if i > noOfDof()
            const double& Dof(size_t i) const;
            
            //! return a copy of the Dofs vector value.
            std::vector<double> getDofs() const;
            
            //! return the element 'i'; return an exception if i > noOfDof()
            boost::shared_ptr<msElement> getElement(size_t i) const;
            
            //! return the element iterator pointing on  the first Element
            ElementItr       beginElements()        { return Elements.begin();  };
            
            //! return the element iterator pointing on  the first Element, const version
            const_ElementItr beginElements() const  { return Elements.begin();  };
            
            //! return the element iterator just after the last element
            ElementItr       endElements()          { return Elements.end(); }
            
            //! return the element iterator just after the last element, const version
            const_ElementItr endElements()   const  { return Elements.end(); }
            
            //! number of elements
            size_t noOfElements() const {return Elements.size();};
            
            //! number of Dof
            size_t noOfDof()      const {return Dofs.size();}
            
            //@}
            
            //! return the separation between Elements 'i' and 'j'; throw an exception if one of them is out of range
            Vector3d getSeparation(int i0,int i1);
            
            //! return the position of the element 'i'; return an exception if i > noOfDof()
            Vector3d getPosition(size_t i) const;
            
            //! fill 'coors' by the cartesian coordinates of the elements
            void getPositions(vector<Vector3d>& coors) const;
            
            //! compute the cartesian coordinates (i.e. updatePositions() + apply overall transformation if necessary )
            void computeCartCoordinates();
            
            //! return the mass of the entity
            double getMass() const;
            
            //! return a vector of 3 double of the eigen moment of inertia
            vector<double> getEigenInertia() const;
            
            //! compute the principal moment of inertia.  'eigenvectors' store the directions, and 'eigenvalues' the norms
            void computeEigenInertia(Matrix3d& eigenvectors,Vector3d& eigenvalues) const;    
            
            std::ostream& print(std::ostream& out) const;
            
        protected:
            
            void reserveDofs( size_t n );
            
            void reset();
            
            virtual boost::shared_ptr<msTreeMapper> updatePositions(){return mySharedPtr();}; 
            
            void translateOriginAtCenterOfMass(const Vector3d& pt);
            
        private:
            
            //!< vector to store the DoF of the structure
            msVectorOfDofs Dofs;	  
            
            void setPositions(const vector<Vector3d>& coors);
            
            void applyTranslation(const Vector3d& v);
            
            void applyTransformation(const Matrix3d& mat);
            
            const Vector3d* OverallTranslation;
            
            const Matrix3d* OverallRotation;
            
            msChildren<msElement> Elements;
            
            void setOverallOperators(const Vector3d* const trans,const Matrix3d* const rot)
            {
                OverallTranslation=trans; 
                OverallRotation=rot; 
            };	     
        };
        
    }
}
#endif // MSENTITY_H
