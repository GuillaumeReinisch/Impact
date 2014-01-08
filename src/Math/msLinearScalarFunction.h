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


#ifndef MSLINEARFCT_H
#define MSLINEARFCT_H

#include <msScalarFunction.h>

namespace impact {
    
    
    namespace math{
        
        
        class msLinearScalarFunction : public msScalarFunction
        {
            
            static msRegistrar Registrar;
            static bool  isLinearScalarFunctionRegisteredInPython;
            static msRegister* createInstance() { return new msLinearScalarFunction; }
            
        protected:
            
            void registryInPython();
            
            void initialize()  {
                msScalarFunction::initialize();
                msTreeMapper::declareAttribute(a,"a");
                msTreeMapper::declareAttribute(b,"b");
                msTreeMapper::declareAttribute(Indice,"Indice");
            };
            
            void update(){  msScalarFunction::update(); }
            
        public:
            
            msLinearScalarFunction() : msScalarFunction(){
                
                constructVar("msLinearScalarFunction","msLinearScalarFunction","linear scalar function");
            }
            
            static boost::shared_ptr<msLinearScalarFunction> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msLinearScalarFunction> msLinearScalarFunction::New()","");
                LOGGER_WRITE(msLogger::DEBUG, "New msLinearScalarFunction object")
                
                boost::shared_ptr<msLinearScalarFunction> T( new msLinearScalarFunction );
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msLinearScalarFunction> msLinearScalarFunction::New()");
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                
                boost::shared_ptr<msLinearScalarFunction> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            boost::shared_ptr<msTreeMapper> setCoefsIndice(double a0, double a1,int i);
            
            double evaluate();
            
            std::string getLabel(){ return ("Linear scalar function"); }
            
            ostream&  print(ostream& out) const {  msScalarFunction::print(out);
                output::printHeader(out,"Linear scalar function")<<"\n a="<<a<<", b="<<b<<endl;
                return out;
            }
            
        private:
            
            double a; double b;
            int Indice;
        };
        
    }
}
#endif
