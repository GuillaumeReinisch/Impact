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


#ifndef MSParameterSpace_H
#define MSParameterSpace_H

#include <msScalarFunction.h>

namespace impact {
    
    namespace queso {
        
        /*! \brief A parameter of model
         *
         * A parameter is a scalar variable (msScalarVariable) that contains some
         * parameter-related features.
         *
         * For now, pointers to 'double' values can be added, their value are updated
         * each time the msModelParameter::setValue method is called. This feature 
         * can only be used from C++.
         */
        class msModelParameter : public msScalarVariable
        {
            
        protected:
            
            static msRegistrar Registrar;
            static bool  isModelParameterRegisteredInPython;
            static msRegister* createInstance() { return new msModelParameter; }
            
            void registryInPython();
            
        public:
            
            msModelParameter() : msScalarVariable() {
                
                constructVar("msModelParameter","ModelParameter","model parameter");
            };
            
            void update()  {
                
                msScalarVariable::update();
            };
            
            void initialize() {
                
                msScalarVariable::initialize();
            };
            
            static boost::shared_ptr<msModelParameter> New(){
                
                boost::shared_ptr<msModelParameter> T( new msModelParameter );
                T->initialize();
                T->update();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msModelParameter> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            /*! \brief set the value of the parameter and update the references on values providen
             *
             * In addition to set the parameter's value, the method update the value of the references
             * provided by using the method msModelParameter::addRefOnValue .
             * \param v value
             */
            boost::shared_ptr<msTreeMapper> setValue(double v){
                
                msScalarVariable::setValue(v);
                
                for(size_t i=0; i<PtrOnValues.size(); i++) {
                    
                    *(PtrOnValues[i]) = v;
                }
                return mySharedPtr();
            }
            
            boost::shared_ptr<msTreeMapper> addRefOnValue(double & ref) {
                
                PtrOnValues.push_back(&ref);
                return mySharedPtr();
            }
        private:
            
            vector<double*> PtrOnValues;
        };
        
        /*! \brief A container for model parameters
         *
         * The class msParameterSpace is a msGeneralizedCoordinates
         * object (container of scalar variable) that
         * have additional parameter-related method. see msGeneralizedCoordinates.
         *
         * The container can be filled by msModelParameter object that
         * are msScalarVariable, again with additional parameter-related
         * functions.
         */
        class msParameterSpace : public msGeneralizedCoordinates
        {
            
        protected:
            
            static msRegistrar Registrar;
            static bool  isParameterSpaceRegisteredInPython;
            static msRegister* createInstance() { return new msParameterSpace; }
            
            void registryInPython();
            
        public:
            
            msParameterSpace();
            
            void update()  {
                
                msGeneralizedCoordinates::update();
            };
            
            void initialize() {
                
                msGeneralizedCoordinates::initialize();
            };
            
            static boost::shared_ptr<msParameterSpace> New(){
                
                boost::shared_ptr<msParameterSpace> T( new msParameterSpace );
                T->initialize();
                T->update();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msParameterSpace> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
        private:
            
        };
    }
    
}
#endif // MSDIRACS_H
