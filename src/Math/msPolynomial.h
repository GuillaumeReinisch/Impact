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


#ifndef MSPOLYNOME_H
#define MSPOLYNOME_H

#include <msScalarFunction.h>

namespace impact {
    
    
    namespace math{
        
        
        class msPolynomial : public msScalarFunction
        {
        private:
            
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isPolynomialRegisteredInPython;
            static msRegister* createInstance() { return new msPolynomial; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void update()  {msScalarFunction::update();
            };
            
            typedef std::map< std::vector<int> , double >::iterator monomeIterator;
            typedef std::map< std::vector<int> , double >::const_iterator const_monomeIterator;
            
        public:
            
	    msPolynomial() : msScalarFunction(){
                constructVar("msPolynomial","Polynomial","polynome");
            }
            
            static boost::shared_ptr<msPolynomial> New(){
                
                boost::shared_ptr<msPolynomial> T( new msPolynomial );
                LOGGER_ENTER_FUNCTION_DBG("msScalarFunction::New(boost::shared_ptr<msGeneralizedCoordinates> unit)",T->getFullId());
                T->initialize();
		T->update();
                LOGGER_EXIT_FUNCTION();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msPolynomial> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            void initialize()  {  msScalarFunction::initialize();
                declareAttribute< std::map< std::vector<int> , double > >(Monomes,"Monomes");
            };
            
        public:
            
            boost::shared_ptr<msTreeMapper> setExpression(string str);
            boost::shared_ptr<msTreeMapper> addMonomes(string str);
            boost::shared_ptr<msTreeMapper> addMonome(vector<int> exposants,double a);
            
            double evaluate();
            /*
             msPolynomial operator+(const msPolynomial& rhs) const;
             msPolynomial operator-(const msPolynomial& rhs) const;
             msPolynomial operator*(double a) const;
             msPolynomial operator/(double a) const;
             */
            msPolynomial& operator+=(const msPolynomial& rhs);
            msPolynomial& add(const msPolynomial& rhs,double coef);
            msPolynomial& operator-=(const msPolynomial& rhs);
            msPolynomial& operator*=(double a);
            msPolynomial& operator/=(double a);
            
            ostream& print(ostream& out) const;
            
            ostream& abstract(std::ostream& out) const;
            
        private:
            
            std::map< std::vector<int> , double >  Monomes;
        };
        
    }
}
#endif // MSPOLYNOME_H
