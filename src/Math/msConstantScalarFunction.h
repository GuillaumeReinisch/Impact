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


#ifndef MSCONSTANTSCALARFUNCTION_H
#define MSCONSTANTSCALARFUNCTION_H


#include <msScalarFunction.h>

namespace impact {
    
    
    namespace math{
        
        
        class msConstantScalarFunction : public msScalarFunction
        {
            
            static msRegistrar Registrar;
            static bool  isConstantScalarFunctionRegisteredInPython;
            static msRegister* createInstance() { return new msConstantScalarFunction; }
            
        protected:
            
            void registryInPython();
            
            void initialize()  {
                msScalarFunction::initialize();
                msTreeMapper::declareAttribute(Value,"Value");
            };
            
            void update(){  msScalarFunction::update(); }
            
        public:
            
            msConstantScalarFunction() : msScalarFunction(){
                
                constructVar("msConstantScalarFunction","msConstantScalarFunction","constant scalar function");
                Value=0;
            }
            
            static boost::shared_ptr<msConstantScalarFunction> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msConstantScalarFunction> msConstantScalarFunction::New()","");
                LOGGER_WRITE(msLogger::DEBUG, "New msConstantScalarFunction object")
                
                boost::shared_ptr<msConstantScalarFunction> T( new msConstantScalarFunction );
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msConstantScalarFunction> msConstantScalarFunction::New()");
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                
                boost::shared_ptr<msConstantScalarFunction> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            boost::shared_ptr<msTreeMapper> setValue(double v) {
                
                Value=v;
                return mySharedPtr();
            }
            
            double evaluate();
            
            std::string getLabel(){ return ("Constant scalar function"); }
            
            ostream&  print(ostream& out) const {
                
                msScalarFunction::print(out);
                output::printHeader(out,"Constant scalar function")<<"\n Value="<<Value<<endl;
                return out;
            }
            
        private:
            
            double Value;
        };
        
    }
}

#endif