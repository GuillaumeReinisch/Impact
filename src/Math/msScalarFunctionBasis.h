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


#ifndef MSSCALARFUNCTIONBASISSET_H
#define MSSCALARFUNCTIONBASISSET_H
#include <msScalarFunction.h>
#include <msSampler.h>
#include <msPolynomial.h>

namespace impact {
    
    
    namespace math{
        
        /*! \class msScalarFunctionBasis
         *
         * \brief Object to create scalar orthonormalize basis function of a 
         * particular (templated) type to represent a scalar field.
         *
         * Still in development, use it carefully.
         */
        template< class Template = msPolynomial>
        class msScalarFunctionBasis : public msTreeMapper
        {
        private:
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isScalarFunctionBasisRegisteredInPython;
            static msRegister* createInstance() { return new msScalarFunctionBasis; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void update()  {msTreeMapper::update();
            };

        public:
	  
	    msScalarFunctionBasis() : msTreeMapper(){
                constructVar("msScalarFunctionBasisSet",
                             "ScalarFunctionBasisSet",
                             "scalar function basis set");
            }
            
            static boost::shared_ptr<msScalarFunctionBasis> New(){
                
                boost::shared_ptr<msScalarFunctionBasis> T( new msScalarFunctionBasis );
                LOGGER_ENTER_FUNCTION_DBG("msScalarFunctionBasisSet::New()",T->getFullId());
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                
                boost::shared_ptr<msScalarFunctionBasis> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            void initialize()  {  msTreeMapper::initialize();
                declareChildren<Template>(Functions,"Functions");
                declareChild<msSampler>(Sampler,msSampler::New(),"Sampler");
            };
            
            void addBasisFunction(boost::shared_ptr<Template> fct) {
                
                if( !fct->hasParent() ) fct->setAffiliation( mySharedPtr() );
                addElementToChildren<Template>(Functions,fct);
            };
            
            
            void orthonormalize( msSampler& sampler );
            
            void orthonormalTest( msSampler& sampler, ostream& out );
            
            void projection( msScalarFunction& fct,  msSampler& sampler, vector<double>& coefs) const;
            
            ostream& print(ostream& out) const;
            
            size_t noOfFunctions(){ return Functions.size(); }
            
            boost::shared_ptr<Template> getFunction(size_t i) {return Functions[i].getSharedPtr();}
            
        private:
            
            msChildren<Template>  	Functions;
            msChild<msSampler> 	Sampler;
        };
        
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        template<class T>
        bool msScalarFunctionBasis<T>::isScalarFunctionBasisRegisteredInPython=0;
        
        template<class T>
        msRegistrar msScalarFunctionBasis<T>::Registrar( "msScalarFunctionBasis" , msScalarFunctionBasis::createInstance );
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        template<class T> inline
        std::ostream& operator<<(std::ostream& out,const msScalarFunctionBasis<T>& basis) {
            
            return( basis.print(out) );
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        template<class T> inline
        void msScalarFunctionBasis<T>::registryInPython()
        {
#if USE_PYTHON
            msTreeMapper::registryInPython();
            
            if(  ! msScalarFunctionBasis<T>::isScalarFunctionBasisRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msScalarFunctionBasis,bases<msTreeMapper>,boost::shared_ptr<msScalarFunctionBasis> >
                ("msScalarFunctionBasis",  "Basis for scalar function",no_init
                 )
                .def( "New",&msScalarFunctionBasis::New ,"Return a shared pointer to a new object" )
                .staticmethod("New")
                .def( "addBasisFunction", &msScalarFunctionBasis::addBasisFunction ,"add a basis function. arg2: basis function" )
                .def( "getFunction", &msScalarFunctionBasis::getFunction ,"return a basis function. arg2: rank of the basis function" )
                .def(self_ns::str(self_ns::self));
                msScalarFunctionBasis::isScalarFunctionBasisRegisteredInPython= 1 ;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        template<class T> inline
        ostream& msScalarFunctionBasis<T>::print(ostream& out) const {
            
            msTreeMapper::print(out);
            output::printHeader(out,"Scalar function basis set");
            if(Functions.size()>0)
                out<<"Type of basis function:"<<Functions[0]->getType()<<endl;
            else out<<"No basis function defined, you can use 'addBasisFunction'"<<endl;
            
            for( size_t i=0;i<Functions.size();++i)
                (Functions[i]->abstract(out))<<endl;
            return out;
        }
	    
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        template<class T>
        void msScalarFunctionBasis<T>::orthonormalTest(  msSampler& sampler ,
                                                       ostream& out ) {
            
            for(int j=0;j<Functions.size();j++) {
                
                for(int i=0;i<j;i++) {
                    double scal= sampler.scalarProduct(*(Functions[i]),*(Functions[j]) );
                    out<<i<<" scal "<<j<<":"<<scal<<";";
                }
                double norme=sampler.scalarProduct(*(Functions[j]),*(Functions[j]));
                out<<j<<" scal "<<j<<":"<<norme<<endl;
            };
        }
	    
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        template<class T> 
        void msScalarFunctionBasis<T>::orthonormalize(  msSampler& sampler  ) {
            
            LOGGER_ENTER_FUNCTION("void msScalarFunctionBasis<T>::orthonormalize(  msSampler& sampler  )",getFullId());
            for(int j=0;j<Functions.size();j++) {
                LOGGER_WRITE(msLogger::INFO,"Orthonormalize function "+output::getString<int>(j)+"/"+output::getString<int>(Functions.size()));
                for(int i=0;i<j;i++) 
                    (*(Functions[j])).add( *(Functions[i]) , - sampler.scalarProduct( *(Functions[i]),
                                                                                     *(Functions[j]) ) );
                *(Functions[j]) /= pow(sampler.scalarProduct(*(Functions[j]),*(Functions[j])),0.5);
            };
            LOGGER_EXIT_FUNCTION();
        }  
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        template<class T> 
        void  msScalarFunctionBasis<T>::projection( msScalarFunction& fct,  
                                                   msSampler& sampler,
                                                   vector<double>& coefs) const {
            
            coefs.resize(Functions.size(),0);  
            for(int j=0;j<Functions.size();j++) 
                coefs[j] = sampler.scalarProduct(fct,*Functions[j]);
        }
        
    }
    
}
#endif // MSSCALARFUNCTIONBASISSET_H
