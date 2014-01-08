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


#ifndef MSELEMENT_H
#define MSELEMENT_H

#include <msPhysicalInterface.h>
#include <Eigen/Dense>
#include <msUnitsManager.h>

namespace impact {
    
    namespace atomism {
        
        using namespace Eigen;
        
        /*! \class msElement
         * \brief Represents an indivisible mass
         * 
         * The class msElement represents an indivisible mass;
         * it stores its position and its mass. msElement can be derived to 
         * define additional specific properties (see msAtom classes for instance).
         * The position of the elements is usually computed by a msEntity
         * container using the degrees of freedom defined in the entity.
         *
         * One important utilisation of msElement is in the definition 
         * of entities (msEntity) to study the equations of motion
         * of mechanical systems (msSystem) using analytical mechanics (msLagrangian).
         */
        class msElement : public msPhysicalInterface
        {
            friend class msEntity;
            friend class msSystem;
            
        private:
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isElementRegisteredInPython;
            static msRegister* createInstance() { return new msElement; }
            //@}
        public:
            
	  
            msElement() : msPhysicalInterface() {
                constructVar("msElement","Element","element");
                Position<<0,0,0;
            };
	    
            static boost::shared_ptr<msElement> New()
            {boost::shared_ptr<msElement> T( new msElement );
                T->initialize();
                return T;
            }
            
            static boost::shared_ptr<msElement> New(msUnit& massUnit)
            {boost::shared_ptr<msElement> T( new msElement );
                T->initialize();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msElement> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
        protected:
            
            
            void initialize()  {
                msPhysicalInterface::initialize();
                // declareAttribute(Mass,"Mass");
            };
            
            void registryInPython();
            
            
            void setMass(double m, msUnit& unit) {
                
                Mass = m; MassUnit = unit;
            };
            
        public:
            
            /*! \brief return the mass of the element
             *
             * \param unit mass unit desired
             */
            virtual double getMass(msUnit::uMass unit) const {return Mass;};
            
	    double getMass() const {return getMass(getUnits()->getMass()); }
            /*! \brief return the position of the element
             */
            Vector3d getPosition() const {return Position;}
            
            /*! \brief return the X-position of the element
             */
            double getX() const {return Position[0];}
            
            /*! \brief return the Y-position of the element
             */
            double getY() const {return Position[1];}
            
            /*! \brief return the Z-position of the element
             */
            double getZ() const {return Position[2];}
            
            /*! \brief set the position of the element
             *
             * This function is used by msEntity and msSystem classes
             * using the degrees of freedom defined by the entities.
             */
            bool setPosition(double x,double y,double z) {
                
                if( (x!=x) || (y!=y) || (z!=z) ){
                    
                    Position<<0,0,0;
                    throw msError("Nan providen to msElement" , "bool msElement::setPosition(double x,double y,double z)" , getFullId());
                }
                Position<<x,y,z; return 1;
            }
            
            /*! \brief set the position of the element
             *
             * This function is used by msEntity and msSystem classes
             * using the degrees of freedom defined by the entities.
             */
            bool setPosition(Vector3d pos) { return setPosition(pos[0],pos[1],pos[2]); };
            
            //! \brief Translate the element
            void applyTranslation(const Vector3d& v) { if( (v[0]!=v[0]) || (v[1]!=v[1]) || (v[2]!=v[2]) )
                throw msError("Nan providen to msElement" , "void msElement::applyTranslation(const Vector3d& v)" , getFullId());
                Position+=v; 
            }
            
            //! \brief Apply a transformation to the position 
            void applyTransformation(const Matrix3d& mat){Position=mat*Position;};
            
        private:
            
            msUnit MassUnit;
            
            Vector3d Position;
            
            double Mass;
            
        };
        
    }
}

#endif // MSELEMENT_H
