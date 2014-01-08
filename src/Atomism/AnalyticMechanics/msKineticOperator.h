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


#ifndef MSKINETICOPERATOR_H
#define MSKINETICOPERATOR_H

#include <msLinearScalarFunction.h>
#include <msSystem.h>


namespace impact {
    
    namespace atomism {
        
        /*! \class msDynamicDof
         * \brief Describes a degree of freedom that is dependant of a scalar variable via a scalar function
         *
         * This class is used by a kinetic operator (msKineticOperator) to define the dependance of 
         * a degree of freedom of an entity (msEntity) with respect to generalized coordinates (msGeneraliedCoordinates).
         * See msKineticOperator.
         */
        class msDynamicDof : public msTreeMapper
        {
            friend class msKineticOperator;
            
        private:
            //! @Name from msRegistrer
            //@{
            static msRegistrar Registrar;
            static msRegister* createInstance() { return new msDynamicDof; }
            static bool isDynamicDofRegisteredInPython;
            //@}
            
        protected:
            
            void initialize(){msTreeMapper::initialize();
                declareChild<msScalarFunction>( Function , msScalarFunction::New(), "Function" );
                declareChild<msEntity>( Entity , msEntity::New(),   "Entity" );
                declareAttribute( Indice, "Indice");
            }
            
            void update(){
                msTreeMapper::update();
            }
            
            void registryInPython();
            
        public:
	  
	    msDynamicDof():msTreeMapper() {
                constructVar("msDynamicDof","DynamicDof","dynamic variable");
            }
            
            static boost::shared_ptr<msDynamicDof> New(){
                
                boost::shared_ptr<msDynamicDof> T( new msDynamicDof );
                LOGGER_ENTER_FUNCTION_DBG("msDynamicDof::New(boost::shared_ptr<msUnitsManager> units)",T->getFullId());
                T->initialize(); T->update();
                LOGGER_EXIT_FUNCTION();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msDynamicDof> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            void set(boost::shared_ptr<msScalarFunction> function,
                     boost::shared_ptr<msEntity> entity, size_t i);
            
            std::ostream& print(std::ostream& out) const {
                
                out<<" Degree of freedom no. "<<Indice<<" for the entity of id '"<<Entity->getId()
                <<"'; scalar function:"<<Function->getType();
                return out;
            }
            
        private:
            
            msChild<msScalarFunction>   Function;
            msChild<msEntity>           Entity;
            size_t Indice;
        };
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        class msKineticOperator;
        
        class msJacobianOfDispl
        {
            friend class msSystem;
            friend class msKineticOperator;
            
        public:
            
            msJacobianOfDispl();
            
            void initialize( msKineticOperator* kinetic );
            
            void compute( const msUnitsManager& units, bool isolated );
            
            void resize(size_t noOfat,size_t noOfdof);
            
            const Eigen::Vector3d& operator()(int elem,int qi);
            
            void setJacColumn( const msGeneralizedCoordinates::UnfreezIt q , size_t ind, const msUnitsManager& units );
            
            void setDisplacment( const msVector3dContainer& coors0,
                                const msVector3dContainer& coors1,
                                size_t ind, double conv );
            
            std::ostream& print(std::ostream& out) const;
            
        private:
            
            boost::numeric::ublas::matrix<Eigen::Vector3d> Displ_Elem_qi;
            
            msKineticOperator* 	      KineticOperator;
            msSystem* 	 	      System;
            Eigen::Vector3d*     	      OverallTrans;
            Eigen::Matrix3d*     	      OverallRot;
            msGeneralizedCoordinates*       GeneralizedCoordinates;
            
            size_t NoOfAtoms, NoOfDofs;
            
            msVector3dContainer 	      Coors0;
            Eigen::Vector3d 		      CenterOfMass0;
            msVectorOfDofs 		      Dofs0;
            
            msVector3dContainer 	      Coors1;
            Eigen::Vector3d 		      CenterOfMass1;
            msVectorOfDofs		      dDofs;
        };
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        /*! \class msKineticOperator
         * \brief Describes the kinetic operator used to define the equations of motion in
         * the Lagrangian formalism.
         *
         * The kinetic operator describes how the masses in the system move with respect
         * to the generalized coordinates. It is a container of dynamic Degrees Of Freedom (msDynamicDof):
         * they link a DoF in an entity to generalized coordinates (msGeneralizedCoordinates)
         * via a scalar function (msScalarFunction).
         
        * See below for an example with the molecule H2 to define the kinetic operator of
        * the vibration.
        * ~~~~~~~~~~~~~{.py}
        from libimpact import *
        
        # definition of the units system used by the objects
        units = msUnitsManager.New("Angstrom Degree amu kcal/mol ps")
         
        # definition of the entity (here zmat type, the separation H-H is the first DoF of the entity)
        zmat = msZMat.New().setUnits(units).setId("zmat").set("H H 0 0.74191")
         
        # create a mechanical system
         
        # definition of the generalized coordinate
        gcoors = msGeneralizedCoordinates.New().setUnits(units).
        q0     = msScalarVariable.New("Angstrom").set(0.7414, 0.1, 5, 0.01, 0.1).setId("q0")
        gcoors.addVar(q0)
         
        # definition of the system: it contains the zmat entity
        system = msSystem.New().setUnits(units).addEntity(zmat)
         
        # definition of the kinetic operator: operate on system using gcoors
        kinop = msKineticOperator.New().setUnits(units).set(gcoors,system)
        
        # we link the separation H-H in the z-matrix (DOF number 0 of zmat) by the scalar function f(q0)=q0.
        kinop.addDynamicDof( zmat, 0, msParser.New("Angstrom").setCoordinates(gcoors).setExpression("q0").setId("f=q0") )
        print kinop
        * ~~~~~~~~~~~~~
        */
        class msKineticOperator : public msPhysicalInterface
        {
            friend class msJacobianOfDispl;
            
            class msKineticOperatorParams : public msParamsManager
            {
                static msRegistrar Registrar;
                static msRegister* createInstance() { return new msKineticOperatorParams; }
                
            public:
                msKineticOperatorParams() : msParamsManager() {
                    /* addParameter( "MaxStep" , "25" ,   "Max. steps for J annihilation" , msUnit() );
                     addParameter( "ResEkin" , "1e-5" , "Max. steps for J annihilation" , msUnit() );*/
                }
            };
        private:
            //! @Name from msRegistrer
            //@{
            static msRegistrar Registrar;
            static bool  isKineticOperatorRegisteredInPython;
            static msRegister* createInstance() { return new msKineticOperator; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void initialize(){
                
                msPhysicalInterface::initialize();
                declareChildren<msDynamicDof>( DynamicDofs,       "DynamicDofs" );
                declareChild<msSystem>( System , msSystem::New(), "System" );
                declareChild<msGeneralizedCoordinates>( GeneralizedCoordinates ,
                                                       msGeneralizedCoordinates::New(),
                                                       "GeneralizedCoordinates" );
            }
            void update(){
                
                msPhysicalInterface::update();JacobianOfDispl.reset(new msJacobianOfDispl);
                KMatrix.reset(new msKineticMatrix(GeneralizedCoordinates->noOfActive(),
                                                  GeneralizedCoordinates->noOfActive()));
            }
            
        public:
            
	    msKineticOperator():msPhysicalInterface() {
                
                constructVar("msKineticOperator","KineticOperator","kinetic operator");
            }
            
            enum TestType { system , coordinates };
            
            typedef Eigen::MatrixXd msKineticMatrix;
            
            static boost::shared_ptr<msKineticOperator> New( ){
                
                boost::shared_ptr<msKineticOperator> T( new msKineticOperator );
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msKineticOperator> msKineticOperator::New()",T->getFullId());
                LOGGER_WRITE(msLogger::DEBUG, "New msKineticOperator object")
                T->initialize();
                T->setParameters( boost::shared_ptr<msParamsManager> (new msKineticOperatorParams()) );
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msKineticOperator> msKineticOperator::New()");
                return T;
            }
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msKineticOperator> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            /* msKineticOperator(const msKineticOperator& rhs);
             
             msKineticOperator& operator=(const msKineticOperator& rhs);*/
            
            /*! \brief define the kinetic operator
             *
             * \param coords generalized coordinates
             * \param system system
             */
            boost::shared_ptr<msTreeMapper> set( boost::shared_ptr<msGeneralizedCoordinates> coords ,
                                                boost::shared_ptr<msSystem> system );

            /*! \brief set the generalized coordinates
             *
             * \param coords generalized coordinates
             */
            boost::shared_ptr<msTreeMapper> setCoordinates( boost::shared_ptr<msGeneralizedCoordinates> coords);
            
            /*! \brief set the system
             *
             * \param system system
             */
            boost::shared_ptr<msTreeMapper> setSystem( boost::shared_ptr<msSystem> system );
            
            //! deprecated
            boost::shared_ptr<msTreeMapper> addDynamicDof( std::string id , size_t indice ,
                                                          boost::shared_ptr<msScalarFunction> fct);
            
            /*! \brief add a dynamic degree of freedom
             *
             * Define the dynamic DoF 'i' of the 'entity' as dynamic and defined by the scalar function 'fct'
             * \param entity entity containing the target Dof
             * \param index index of the Dof targeted in 'entity'
             * \param fct scalar function defining the Dof value w/ generalized coordinates
             */
            boost::shared_ptr<msTreeMapper> addDynamicDof( boost::shared_ptr<msEntity> entity , size_t index ,
                                                          boost::shared_ptr<msScalarFunction> fct);
            
            /*! \brief add a dynamic degree of freedom represented by a linear scalar function
             *
             * Define the dynamic DoF 'i' of the 'entity' as dynamic and defined by a linear scalar function
             * of a variable 'q' (f(q)=q)
             * \param entity entity containing the target Dof
             * \param index index of the Dof targeted in 'entity'
             * \param q scalar variable
             */
            boost::shared_ptr<msTreeMapper> addDynamicDofLinear( boost::shared_ptr<msEntity> entity , size_t indice ,
                                                                boost::shared_ptr<msScalarVariable> q );
            
            //! set the value of the dynamic Dof in the entites w/ current value of the generalized coordinates
            void setDynamicDofs();
            
            //! set the position of the elements in the system w/ current value of the Dof
            void setPositions(){System->computeCartCoordinates();};
            
            msKineticWork move(  size_t i );
            
            double kineticFunction(){ return kineticFunction(*getUnits()); };
            
            double kineticFunction(const msUnitsManager& units);
            
            const msKineticMatrix&   computeKMat() { return computeKMat(*getUnits());}
            
            const msKineticMatrix&   computeKMat(const msUnitsManager& units);
            
            //    void computeDofsVariation( msScalarVariable& qi );
            
            const msKineticMatrix& getKMatrix() { return *KMatrix; }
            
            /*! \brief return the elment (i,j) of the last KMatrix computed
	     * 
	     * \param i index of the coordinates for the column; freezed coordinates not accounted
	     * \param j index of the coordinates for the row; freezed coordinates not accounted
            */
            double getKMatrixElement(size_t i,size_t j);
	    
            const boost::shared_ptr<msGeneralizedCoordinates> getCoordinates() const {
                return GeneralizedCoordinates.getSharedPtr(); }
            
            boost::shared_ptr<msGeneralizedCoordinates>       getCoordinates() {
                return GeneralizedCoordinates.getSharedPtr(); }
            
            boost::shared_ptr<msSystem> getSystem(){ return System.getSharedPtr();}
            
            std::ostream& print(std::ostream& out) const;
            
        private:
            
            void registerMembers();
            
            msChildren<msDynamicDof> 		   DynamicDofs;
            msChild<msSystem> 		   System;
            msChild<msGeneralizedCoordinates>    GeneralizedCoordinates;
            
            
            boost::shared_ptr<msKineticMatrix>   KMatrix;
            boost::shared_ptr<msJacobianOfDispl> JacobianOfDispl;
            
            msVectorOfDofs dDofs,Dofs0;
            msVector3dContainer Coors0,Coors1;
            
            bool checkTest(TestType test) const;    
        };
        
        ostream& operator<<(ostream&,const msKineticOperator&);
        ostream& operator<<(ostream&,const msJacobianOfDispl&);
    }
}
#endif // MSKINETICOPERATOR_H
