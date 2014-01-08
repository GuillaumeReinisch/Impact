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

//! \file msSystem.h definition of the class msSystem, msSystemParams, and ElementIterator

#ifndef MSSYSTEM_H
#define MSSYSTEM_H
#include <msEntity.h>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/iterator/iterator_facade.hpp>


namespace impact {
    
    namespace atomism {
        
        typedef std::vector<Vector3d> msVector3dContainer;
        
        class msKineticOperator;
        class msJacobianOfDispl;
        
        /** \class msSystem
         *
         * \brief A container for entities, computes the absolute positions of their elements
         *
         * The msSystem class manages the entities (msEntity) and elements (msElement),
         * it knows about all the masses involved in the problem. It is constructed by
         * sequentially adding entity (addEntity()).\n
         * The purpose of the class is to compute the absolute coordinates of the elements
         * at the current values of the degrees of freedom (see computeCartCoordinates()).
         * It is designed to work in close colaboration with the kinetic operator
         * class (msKineticOperator) to compute the kinetic work associated to a displacment
         * of the generalized coordinates.
         * In this context, msSystem is able to account for conservation of linear and angular
         * momentum (thus reducing by 5 or 6 the number of generalized coordinates to consider
         * to describe the problem). The typical computational cycle follows the sequence:
         *     - 1) the kinetic operator set the dynamic DoF values corresponding to the
         * position q0 before displacment
         *     - 2) the kinetic operator asks msSystem to compute the element's coordinates
         * -> stored in Coors0
         *     - 3) the kinetic operator updates the generlized coordinates at q0 + dq and
         * update the dynamic DoFs values.
         *     - 4) the kinetic operator asks msSystem to compute the element's coordinates
         * -> stored in Coors1
         *     - 5) For isolated system: the kinetic operator asks msSystem to apply to
         * Coors1 an overall transformation that ensures the conservation of linear and angular
         * momentum from Coors0
         * (functions  annihil_dP(const msVector3dContainer&, msVector3dContainer&, Eigen::Vector3d*)
         and annihil_dJ(const msVector3dContainer&, msVector3dContainer&, Eigen::Matrix3d* Mat)).
         * The computation of the overall transformation is described in \ref pmaSystem.
         *
         * Besides this central task, msSystem provides numerous functions to
         * study the geometric variables: separations, angles, center of mass, etc.
         * It is used for instance to deal with 3D visualization (see for
         * instance atomism::msVisualizerAtom). \n
         *
         * \section pmaSystem Physics, Mathematics, Algorithm
         *
         * Providing that the entities are able to update the coordinates of their elements w/
         * the current values of the DoFs, the computational task of msSystem is to ensure
         * conservation of total angular and linear momentum during a displacment.
         *  That is to say, we want to annihil them; trivial overall rotation/translation
         * can be applied to fix them to desired values after annihilation.
         * In this case the DoF need only to describe the internal geometry.
         *
         * The starting arguments are an   initial   structure   \f$\vec R_{at}(\mathbf{q^0})\f$, and
         * a virtual final \f$\vec R_{at}(\mathbf{q^1=q0+dq})\f$ structure,
         * both constructed directly from the internal coordinates:
         * the displacments defined by
         * the two structures does not conserve angular and linear momentum
         * (that's why the final structure is called virtual).\n
         * The objective is to find an overall translation operator
         * \f$\hat T\f$ and an overall rotation \f$\hat R\f$ that applied to
         *  \f$\vec R_{at}(\mathbf{q^1})\f$ generates the real final structure.
         *
         * The determination of the translation operator is trivial: this is the
         * opposite of the translation between the center of mass of
         * \f$\vec R_{at}(\mathbf{q^1})\f$ and \f$\vec R_{at}(\mathbf{q^0})\f$.
         * We note the geometry translated \f$ \vec R_{at}^{'}(\mathbf{q^1})\f$.
         *
         * The deterimation of the overall rotation operator to apply to
         * \f$\vec R_{at}^{'}(\mathbf{q^1})\f$ is obtained sequencially, by transforming
         * \f$\vec R_{at}^{'}(\mathbf{q^1})\f$ using a counter rotation around the
         * the axis defined by the angular momentum generated by the displacment
         * from \f$\vec R_{at}(\mathbf{q^0})\f$ to \f$\vec R_{at}^{'}(\mathbf{q^1})\f$.
         * The newly obtained structure replace \f$\vec R_{at}^{'}(\mathbf{q^1})\f$,
         * and the procedure is repeated until the angular momentum residue
         * is below a fixed treshold.
         *
         * The structure \f$\vec R_{at}^{'}(\mathbf{q^1})\f$ obtained is finaly:
         *      - 1) rotated by \f$ \theta = |\vec J| / I_{\vec J} \f$,
         * where \f$ \vec J\f$ is the desired angular momentum and
         *  \f$ I_{\vec J} \f$ is the moment of inertia of
         * \f$\vec R_{at}^{'}(\mathbf{q^1})\f$  aroud \f$\vec J\f$
         *      - 2) translated by  \f$  \vec T = \vec P / m \f$, where
         * \f$  \vec P \f$ is the desired linear momentum.
         *
         */
        class msSystem : public msPhysicalInterface
        {
            friend class msKineticOperator;
            friend class msJacobianOfDispl;
            
        public:
            
            /*!\class msSystemParams
             * \brief Parameters of msMotion: MaxStep, ResEkin.
             *
             * The parameters defined are:
             *     - MaxStep : Max. steps for J annihilation
             *     - ResEkin : Relative residue for Ekin convergence
             */
            class msSystemParams : public msParamsManager
            {
                static msRegistrar Registrar;
                static msRegister* createInstance() { return new msSystemParams; }
                
            public:
                void initialize() { msParamsManager::initialize();
                    addParameter( "MaxStep" , "500" ,   "Max. steps for J annihilation" , msUnit() );
                    addParameter( "ResEkin" , "1e-15" , "Relative residue for Ekin convergence" , msUnit() );
                }
                
                static boost::shared_ptr<msSystemParams> New()
                { boost::shared_ptr<msSystemParams> T( new msSystemParams );
                    T->initialize(); T->update();
                    return T;
                }
                
                msSystemParams() : msParamsManager() { }
            };
            
            /*!\class ElementIterator
             * \brief Iterate sequentially over the entitie's elements
             *
             * The iterator loop over the entities, and for each of them, loop over its elements;
             * in the same order in witch they have been pushed back.
             */
            template<class Value,class Value2>
            class ElementIterator : public boost::iterator_facade<ElementIterator<Value,Value2>,
            Value,
            boost::forward_traversal_tag>
            {
            public:
                
                ElementIterator()          : Element(0), System(0), IndEntity(0) , IndElement(-1) {};
                ElementIterator(Value2* s) : Element(0), System(s), IndEntity(0) , IndElement(-1) { increment(); };
                
                explicit ElementIterator(Value* v) : Element(v) {};
                
                Value& dereference() const {return *Element;}
                
                boost::shared_ptr<msEntity> getEntity() const { return System->Entities[IndEntity].getSharedPtr(); };
                
            private:
                
                friend class boost::iterator_core_access;
                
                void increment() {
                    
                    if(  (System==0) || ( (IndEntity >= int(System->noOfEntities()) - 1)
                                         &&(IndElement >= int(System->Entities[IndEntity]->noOfElements()) - 1) )) {
                        Element=0; return;
                    }
                    if( IndElement == int(System->Entities[IndEntity]->noOfElements()) - 1 ) {
                        IndElement=-1; IndEntity++;
                    }
                    IndElement++;
                    Element=( System->Entities[IndEntity]->beginElements() + IndElement )->get();
                }
                
                void advance(size_t n){ for(size_t i=0;i<n;i++) increment(); }
                
                bool equal(ElementIterator<Value,Value2> const& other) const { return this->Element == other.Element; }
                
            protected:
                
                Value* Element;
                Value2* System;
                int IndEntity;
                int IndElement;
            };
            
            /*!\class DofIterator
             * \brief Iterate sequentially over the degrees of freedom
             *
             * The iterator loop over the entities, and for each of them, loop over its degrees of freedom;
             * in the same order in witch they have been pushed back.
             */
            template<class Value,class Value2>
            class DegreeOfFreedomIterator : public boost::iterator_facade<DegreeOfFreedomIterator<Value,Value2>,
            Value,
            boost::forward_traversal_tag>
            {
            public:
                
                DegreeOfFreedomIterator()          : Dof(0), System(0), IndEntity(0) , IndDof(-1) {};
                DegreeOfFreedomIterator(Value2* s) : Dof(0), System(s), IndEntity(0) , IndDof(-1) { increment(); };
                
                explicit DegreeOfFreedomIterator(Value* v) : Dof(v) {};
                
                Value& dereference() const {return *Dof;}
                
                boost::shared_ptr<msEntity> getEntity() const { return System->Entities[IndEntity].getSharedPtr(); };
                
            private:
                
                friend class boost::iterator_core_access;
                
                void increment() {
                    
                    if(  (System==0) || ( (IndEntity >= int(System->noOfEntities()) - 1)
                                         &&(IndDof >= int(System->Entities[IndEntity]->noOfDof()) - 1) )) {
                        Dof=0; return;
                    }
                    if( IndDof == int(System->Entities[IndEntity]->noOfDof()) - 1 ) {
                        IndDof=-1; IndEntity++;
                    }
                    IndDof++;
                    Dof = & System->Entities[IndEntity]->Dofs[IndDof];
                }
                
                void advance(size_t n){ for(size_t i=0;i<n;i++) increment(); }
                
                bool equal(DegreeOfFreedomIterator<Value,Value2> const& other) const { return this->Dof == other.Dof; }
                
            protected:
                
                Value* Dof;
                Value2* System;
                int IndEntity;
                int IndDof;
            };
            
        private:
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool        isSystemRegisteredInPython;
            static msRegister* createInstance() { return new msSystem; }
            //@}
            
        protected:
            
            void registryInPython();
            
        public:
            
            //! \copydoc ElementIterator
            typedef ElementIterator<msElement,msSystem>                     ElementItr;
            
            //! \copydoc ElementIterator, const version
            typedef ElementIterator<const msElement,const msSystem> 	    const_ElementItr;
            
            //! \copydoc DegreeOfFreedomIterator
            typedef DegreeOfFreedomIterator<double,msSystem>                DofItr;
            
            //! \copydoc DegreeOfFreedomIterator, const version
            typedef DegreeOfFreedomIterator<const double,const msSystem>    const_DofItr;
            
            //! @name construction
            //@{
            msSystem() : msPhysicalInterface() {
                constructVar("msSystem","System","system");
                nDoF = 0;
            }
            
            void initialize()  {
                msPhysicalInterface::initialize();
                msTreeMapper::declareChildren<msEntity>(Entities,"Entities");
            };
            
            void update(){
                msPhysicalInterface::update();
            }
            
            static boost::shared_ptr<msSystem> New() {
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msSystem> msSystem::New()","");
                LOGGER_WRITE(msLogger::DEBUG, "New msSystem object")
                
                boost::shared_ptr<msSystem> T( new msSystem );
                T->initialize();
                T->setParameters( msSystemParams::New() );
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msSystem> msSystem::New()");
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                
                boost::shared_ptr<msSystem> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            //! Add an entity to the system
            boost::shared_ptr<msTreeMapper> addEntity(boost::shared_ptr<msEntity> entity);
            //@}
            
            /** \brief compute the cartesian coordinates of the elements, and update their positions (msElement::Position)
             *
             * The function calls the msEntity::updatePositions() for all entities.
             */
            void computeCartCoordinates();
            
            //! \copydoc computeCartCoordinates(). Also fill 'vec' by the coordinates.
            void computeCartCoordinates(msVector3dContainer& vec);
            
            //! @name Analyze the system's geometry
            //@{
            size_t  noOfDof() const;       //!< Number of DoF included in the system
            
            size_t  noOfElements() const;  //!< Number of elements included in the system
            
            size_t  noOfEntities() const;  //!< Number of entities included in the system
            
            //! fill 'dof' by the values of the DoF
            void getValueOfDofs(vector<double>& dof) const;
            
            //! get the position of the element number i, throw an exception is i > noOfElements()
            Vector3d getPosition(size_t i);
            
            //! fill coors with the elements position
            void getPositions( msVector3dContainer& coors) const;
            
            //! return the minimum separation between two elements
            double minSeparation();
            
            //! return the separation between the elements i, j; throw an exception if one of them out of range
            double separation(size_t i,size_t j);
            
            //! return the angle between the elements i, j, k; throw an exception if one of them out of range
            double angle(size_t i,size_t j,size_t k);
            
            //! return the dihedral angle between the elements i, j, k, l; throw an exception if one of them out of range
            double dihedral(size_t i,size_t j,size_t k,size_t l);
            
            //! write am XYZ file of the structure
            void writeXYZ(ostream& file,string comment) const ;
            
            //! return the total mass of the system
            double getMass() const ;
            
            //! return the center of mass of the system
            Vector3d getCenterOfMass() const ;
            
            //! compute the principal moment of inertia.  'eigenvectors' store the directions, and 'eigenvalues' the norms
            void computeEigenInertia(Matrix3d& eigenvectors,Vector3d& eigenvalues) const;
            //@}
            
            //! @name get/naviguate in entities, elements and degrees of freedom
            //@{
            //! return the entity number i, throw an exception is i > noOfEntities()
            boost::shared_ptr<msEntity> getEntity(size_t i) const;
            
            //! return the entity of id 'id', throw an exception if it does not exist
            boost::shared_ptr<msEntity> getEntity(std::string id) const;
            
            //! return an element iterator on the first element
            ElementItr       beginElements()        { ElementItr it(this);  return it;  }
            
            //! return a const element iterator on the first element
            const_ElementItr beginElements() const  { const_ElementItr it(this); return it;  }
            
            //! return the element iterator just after the last element
            ElementItr       endElements()          { return ElementItr();}
            
            //! return the const element iterator just after the last element
            const_ElementItr endElements()   const  { return const_ElementItr();}
            
            //! return an Dof iterator on the first Dof
            DofItr           beginDofs()            { DofItr it(this);  return it;  }
            
            //! return a Dof const iterator on the first Dof
            const_DofItr    beginDofs() const      { const_DofItr it(this); return it;  }
            
            //! return the Dof iterator just after the last Dof
            DofItr           endDofs()              { return DofItr(); }
            
            //! return a Dof const iterator just after the last Dof
            const_DofItr    endDofs() const        { return const_DofItr(); }
            
            //@}
            //! return 1 if the system is isolated, 0 else
            bool isIsolated(){return 1;}
            
            /** \brief return the kinetic work generated by displacments from Coors0 to Coors1
             *
             * The cartesian coordinates vector contains the elements position (as defined in ElementIterator).
             * \param Coors0 Cartesian coordinates of the initial structure
             * \param Coors1 Cartesian coordinates of the virtual final structure
             */
            msKineticWork kineticWork(const msVector3dContainer& Coor0,
                                      const msVector3dContainer& Coor1) const;
            
            std::ostream& print(std::ostream& out) const;
            
            
        protected:
            
            /** \brief Apply a translation to 'Coors1' so that no linear momentum is generated by the displacments from Coors0 to Coors1
             *
             * The cartesian coordinates vector contains the elements position (from the first element of the first entity
             * to the last element of the last entity, as the ElementItr scan the structure).
             * \param Coors0 Cartesian coordinates of the initial structure \f$\vec R_{at}(\mathbf{q^0})\f$
             * \param Coors1 Cartesian coordinates of the virtual final structure \f$\vec R_{at}(\mathbf{q^1})\f$
             * \param trans if not zero stores the translation applied
             */
            void annihil_dP(const msVector3dContainer& Coors0,
                            msVector3dContainer& Coors1,
                            Eigen::Vector3d* trans=0);
            
            /** \brief Apply a rotation to 'Coors1' so that no angular momentum is generated by the displacments from Coors0 to Coors1
             *
             * The cartesian coordinates vector contains the elements position (as defined in ElementIterator).
             * \param Coors0 Cartesian coordinates of the initial structure \f$\vec R_{at}(\mathbf{q^0})\f$
             * \param Coors1 Cartesian coordinates of the virtual final structure \f$\vec R_{at}(\mathbf{q^1})\f$
             * \param Mat if not zero stores the rotation matrix applied
             */    
            msKineticWork annihil_dJ(const msVector3dContainer& Coors0, 
                                     msVector3dContainer& Coors1,
                                     Eigen::Matrix3d* Mat=0);
            
            /** \brief return the center of mass displacment between Coors0 to Coors1
             * 
             * The cartesian coordinates vector contains the elements position (as defined in ElementIterator).
             * \param Coors0 Cartesian coordinates of the initial structure 
             * \param Coors1 Cartesian coordinates of the virtual final structure 
             */    
            Vector3d getDeltaCenterOfMass(const msVector3dContainer& coors1,
                                          const msVector3dContainer& coors0 ) const;
            
            
        private:
            
            Matrix3d OverallRotation;
            
            Vector3d OverallTranslation; 
            
            bool getDeltaDofs( msVectorOfDofs& result, msVectorOfDofs& coor0);
            
            int nDoF;          
            
            msVector3dContainer Coors0,Coors1;
            
            msChildren<msEntity>  Entities;
            
            std::map< std::string , boost::shared_ptr<msEntity> > EntitiesMap;
        };
        
        ostream& operator<<(ostream&,const msSystem&);
    }
}

#endif // MSSYSTEM_H
