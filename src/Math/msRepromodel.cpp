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


#include "msRepromodel.h"


namespace impact {
    
    
    namespace math{
        
        
        bool msPolynomialBasis::isPolynomialBasisRegisteredInPython=0;
        msRegistrar msPolynomialBasis::Registrar("msPolynomialBasis", msPolynomialBasis::createInstance);
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        std::ostream& operator<<(std::ostream& out,const msPolynomialBasis& fct)
        {
            fct.print(out);
            return out;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msPolynomialBasis::registryInPython()
        {
#if USE_PYTHON
            msScalarFunctionBasis<msPolynomial>::registryInPython();
            
            if( ! msPolynomialBasis::isPolynomialBasisRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msPolynomialBasis,bases< msScalarFunctionBasis<msPolynomial> >,boost::shared_ptr<msPolynomialBasis> >
                ("msPolynomialBasis",
                 "polynomial basis set",no_init
                 )
                .def( "New",&msPolynomialBasis::New ,"Return a shared pointer to a new object" )
                .staticmethod("New");
                ;
                msPolynomialBasis::isPolynomialBasisRegisteredInPython=1;
            }
#endif
        }
        
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        std::ostream& operator<<(std::ostream& out,const msRepromodel& fct)
        {
            fct.print(out);
            return out;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        bool msRepromodel::isRepromodelRegisteredInPython=0;
        msRegistrar msRepromodel::Registrar("msRepromodel", msRepromodel::createInstance);
        
        void msRepromodel::registryInPython()
        {
#if USE_PYTHON
            msScalarFunction::registryInPython();
            
            if( ! msRepromodel::isRepromodelRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msRepromodel,bases<msScalarFunction>,boost::shared_ptr<msRepromodel> >
                ("msRepromodel",
                 "multidimensional scalar function fit using repromodeling",no_init
                 )
                .def( "New",&msRepromodel::New ,"Return a shared pointer to a new object" )
                .staticmethod("New")
                .def("setBasisSet",&msRepromodel::setBasisSet,"set the polynomial basis set. arg2: maximum degrees of the polynoms")
                .def("loadData",&msRepromodel::loadData,
                     "load the fitting database. Each line of the file contains: the coordinate values and the field value, separated by one or more blank. arg2: file name")
                .def("addData",&msRepromodel::addData,"add a data to the fitting database. arg2: coordinate values, arg3: field value")
                .def("getSampler",&msRepromodel::getSampler,"return the sampler")
                .def("getBasisSet",&msRepromodel::getBasisSet,"return the basis set")
                .def("getField",&msRepromodel::getField,"return the scalar field")
                .def("fit",&msRepromodel::fit,"fit the scalar field")
                .def(self_ns::str(self_ns::self));
                ;
                msRepromodel::isRepromodelRegisteredInPython=1;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msRepromodel::setBasisSet(size_t degree){
            
            int nm=1;int NbVar=getCoordinates()->noOfDim();
            vector<int> exposants(NbVar,0);
            boost::shared_ptr<msPolynomial> pol=msPolynomial::New();
	    pol->setYUnit(getYUnit().getStr());
            pol->setCoordinates(getCoordinates());
            pol->addMonome(exposants,1);
            BasisSet->addBasisFunction(pol);
            
            for(int i=0; i<NbVar;i++){
                
                vector<int> exposants(NbVar,0); exposants[i]=1;
                boost::shared_ptr<msPolynomial> pol=msPolynomial::New();
		pol->setYUnit(getYUnit().getStr());
                pol->setCoordinates(getCoordinates());
                pol->addMonome(exposants,1);
                BasisSet->addBasisFunction(pol);
            }
            if(degree==1){ BasisSet->orthonormalize(*Sampler);
                return mySharedPtr();
            }
            for(int i=0;i<NbVar;i++)
                for(int j=i;j<NbVar;j++) {
                    
                    vector<int> exposants(NbVar,0);
                    exposants[i]++;
                    exposants[j]++;
                    boost::shared_ptr<msPolynomial> pol=msPolynomial::New();
		    pol->setYUnit(getYUnit().getStr());
                    pol->setCoordinates(getCoordinates());
                    pol->addMonome(exposants,1);
                    BasisSet->addBasisFunction(pol);
                }
            
            if(degree==2){ BasisSet->orthonormalize(*Sampler);
                return mySharedPtr();
            }
            
            for(int i=0;i<NbVar;i++)
                for(int j=i;j<NbVar;j++)
                    for(int k=j;k<NbVar;k++){
                        
                        vector<int> exposants(NbVar,0);
                        exposants[i]++;exposants[j]++;exposants[k]++;
                        boost::shared_ptr<msPolynomial> pol=msPolynomial::New();
			pol->setYUnit(getYUnit().getStr());
                        pol->setCoordinates(getCoordinates());
                        pol->addMonome(exposants,1);
                        BasisSet->addBasisFunction(pol);
                    }
            if(degree==3){ BasisSet->orthonormalize(*Sampler);
                return mySharedPtr();
            }
            
            for(int i=0;i<NbVar;i++)
                for(int j=i;j<NbVar;j++)
                    for(int k=j;k<NbVar;k++)
                        for(int l=k;l<NbVar;l++){
                            vector<int> exposants(NbVar,0);
                            exposants[i]++;exposants[j]++;exposants[k]++;exposants[l]++;
                            boost::shared_ptr<msPolynomial> pol=msPolynomial::New();
			    pol->setYUnit(getYUnit().getStr());
                            pol->setCoordinates(getCoordinates());
                            pol->addMonome(exposants,1);
                            BasisSet->addBasisFunction(pol);
                        }
            if(degree==4){ BasisSet->orthonormalize(*Sampler);
                return mySharedPtr();
            }
            
            for(int i=0;i<NbVar;i++)
                for(int j=i;j<NbVar;j++)
                    for(int k=j;k<NbVar;k++)
                        for(int l=k;l<NbVar;l++)
                            for(int m=l;m<NbVar;m++){
                                vector<int> exposants(NbVar,0);
                                exposants[i]++;exposants[j]++;exposants[k]++;exposants[l]++;exposants[m]++;
                                boost::shared_ptr<msPolynomial> pol=msPolynomial::New();
				pol->setYUnit(getYUnit().getStr());
                                pol->setCoordinates(getCoordinates());
                                pol->addMonome(exposants,1);
                                BasisSet->addBasisFunction(pol);
                            }
            if(degree==5){ BasisSet->orthonormalize(*Sampler);
                return mySharedPtr();
            }
            
            for(int i=0;i<NbVar;i++)
                for(int j=i;j<NbVar;j++)
                    for(int k=j;k<NbVar;k++)
                        for(int l=k;l<NbVar;l++)
                            for(int m=l;m<NbVar;m++)
                                for(int n=l;n<NbVar;n++){
                                    vector<int> exposants(NbVar,0);
                                    exposants[i]++;exposants[j]++;exposants[k]++;exposants[l]++;exposants[m]++;exposants[n]++;
                                    boost::shared_ptr<msPolynomial> pol=msPolynomial::New();
				    pol->setYUnit(getYUnit().getStr());
                                    pol->setCoordinates(getCoordinates());
                                    pol->addMonome(exposants,1);
                                    BasisSet->addBasisFunction(pol);
                                }
            if(degree==6){ BasisSet->orthonormalize(*Sampler);
                return mySharedPtr();
            }
            /*
             int NbVar=Coordinates->noOfDim();
             for( size_t order=0;order<=degree;++order) {
             
             vector<int> exposants(NbVar,0);
             boost::shared_ptr<msPolynomial> pol=msPolynomial::New(getYUnit().getStr());
             exposants[0]=order;
             bool done=0;
             cout<<"order="<<order<<endl;
             while( !done ){
             boost::shared_ptr<msPolynomial> pol=msPolynomial::New(getYUnit().getStr());
             pol->setCoordinates(getCoordinates());
             pol->addMonome(exposants,1);
             cout<<exposants<<" "<<exposants.size()<<endl;;
             BasisSet->addBasisFunction(pol);
             done=1;
             for(int i=exposants.size()-2;i>=0;i--){
             cout<<"\ti "<<exposants[i]<<endl;
             if(exposants[i]>0){ exposants[i+1]++;exposants[i]--;done=0;i=0;}
             }
             }
             }*/
            return mySharedPtr();
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msRepromodel::fit(){
            
            vector<double> coefs;
            try{ BasisSet->projection(*ScalarField,*Sampler,coefs);
            }
            catch(msException& e)
            {
                e.addContext("Can not realize the projection of the field on the polynomial basis (void msRepromodel::fit())"); 
            }
            Fit = msPolynomial::New();
	    Fit->setYUnit(getYUnit().getStr());
            Fit->setCoordinates(getCoordinates()); 
            
            for(size_t i=0;i<BasisSet->noOfFunctions();++i)
                Fit->add(*(BasisSet->getFunction(i)),coefs[i]);
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msRepromodel::addData( vector<double> coors, double value){
            
            ScalarField->addDirac(coors,value);
            ScalarField->setCoordinates(getCoordinates());
            //Sampler->addSample(coors);
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msRepromodel::loadData(string file) {
            
            ifstream input(file.c_str());
            
            char line[500];  bool eof=0;
            input.getline(line,500);
            vector<double> coors(getCoordinates()->noOfDim(),0);
            
            while(!eof){
                double value;
                for( size_t i=0; (i<getCoordinates()->noOfDim()) && (!eof); i++) {
                    input>>coors[i];
                    eof=input.eof();
                }
                if( !eof) {
                    input>>value;
                    ScalarField->addDirac(coors,value);
                }
            }
            input.close();
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        ostream& msRepromodel::print(ostream& out) const{
            
            msScalarFunction::print(out);
            output::printHeader(out,"Repromodel");
            
            out<<"Basis set contains "<<BasisSet->noOfFunctions()<<" polynomials \n";
            
            if(Fit != boost::shared_ptr<msPolynomial>()){
                out<<"Fitted polynomial: ";Fit->abstract(out)<<endl;
            }
            else out<<"Polynomial not fitted yet."<<endl;
            
            return out;
        }
        
    }
}