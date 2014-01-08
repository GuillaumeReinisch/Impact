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


#ifndef MSREPROMODEL_H
#define MSREPROMODEL_H

#include <msScalarFunctionBasis.h>
#include <msDiracs.h>
#include <msPolynomial.h>
#include <msScalarFunction.h>

namespace impact {
    
    
    namespace math{
        
        
        class msPolynomialBasis : public msScalarFunctionBasis<msPolynomial>
        {
        private:
            static msRegistrar Registrar;
            static bool  isPolynomialBasisRegisteredInPython;
            static msRegister* createInstance() { return new msPolynomialBasis; }
        protected:
            void registryInPython();
            void update()  {msScalarFunctionBasis<msPolynomial>::update();
            };

        public:
	  
	    msPolynomialBasis() : msScalarFunctionBasis<msPolynomial>(){
                constructVar("msPolynomialBasis","PolynomialBasis","polynomial basis");
            }
            
            static boost::shared_ptr<msPolynomialBasis> New(){
                boost::shared_ptr<msPolynomialBasis> T( new msPolynomialBasis );
                T->initialize(); T->update();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msPolynomialBasis> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            void initialize()  {  msScalarFunctionBasis<msPolynomial>::initialize();
            };
            
        };
        
        /*
         *
         */
        class msRepromodel : public msScalarFunction
        {
            
        private:
            
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isRepromodelRegisteredInPython;
            static msRegister* createInstance() { return new msRepromodel; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void update()  {msScalarFunction::update();
                ScalarField->setDiracSampler(Sampler.getSharedPtr());
            };
            
            msRepromodel():msScalarFunction(){
                constructVar("msRepromodel","Repromodel","repromodel");
            }
            
        public:
            
            static boost::shared_ptr<msRepromodel> New(){
                
                boost::shared_ptr<msRepromodel> T( new msRepromodel );
                T->initialize(); 
		T->update();
                return T;
            }
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msScalarFunction> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            void initialize()  {  msScalarFunction::initialize();
                declareChild< msPolynomialBasis >(BasisSet,
                                                  msPolynomialBasis::New(),
                                                  "BasisSet");
                declareChild<msSamplerExplicit >(Sampler,
                                                 msSamplerExplicit::New(),
                                                 "Sampler");
                declareChild<msDiracs >(ScalarField,
                                        msDiracs::New(),
                                        "ScalarField");
                ScalarField->setYUnit(getYUnit().getStr());
            };
            
            boost::shared_ptr<msTreeMapper> setBasisSet(size_t degree);
            
            void addData( vector<double> coors, double field );
            
            void loadData(string file);
            
            void fit();
            
            double evaluate(){ return Fit->evaluate();
            }
            boost::shared_ptr<msSamplerExplicit> getSampler()	{return Sampler.getSharedPtr();};
            
            boost::shared_ptr<msPolynomialBasis> getBasisSet()	{return BasisSet.getSharedPtr();};
            
            boost::shared_ptr<msDiracs> getField()			{return ScalarField.getSharedPtr();};
            
            ostream& print(ostream& out) const;
        private:
            
            msChild<msPolynomialBasis >        BasisSet;
            msChild<msSamplerExplicit >        Sampler;
            msChild<msDiracs>		       ScalarField;  
            boost::shared_ptr<msPolynomial>    Fit;
        };
        
        std::ostream& operator<<(std::ostream& out,const msRepromodel& fct);
        
        
    }
}
#endif // MSREPROMODEL_H
