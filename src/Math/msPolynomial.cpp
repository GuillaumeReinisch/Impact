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


#include <msPolynomial.h>

namespace impact {
    
    
    namespace math{
        
        
        bool msPolynomial::isPolynomialRegisteredInPython=0;
        msRegistrar msPolynomial::Registrar( "msPolynomial" , msPolynomial::createInstance );
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        std::ostream& operator<<(std::ostream& out,const msPolynomial& pol) {
            
            return( pol.print(out) );
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msPolynomial::registryInPython()
        {
#if USE_PYTHON
            msScalarFunction::registryInPython();
            
            if(  ! msPolynomial::isPolynomialRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msPolynomial,bases<msScalarFunction>,boost::shared_ptr<msPolynomial> >
                ("msPolynomial",  "A polynome scalar function",no_init
                 )
                .def( "New",&msPolynomial::New ,"Return a shared pointer to a new object" )
                .staticmethod("New")
                .def( "setExpression", &msPolynomial::setExpression , "set the expression of the polynome" )
                .def( "addMonomes", &msPolynomial::addMonomes , "add monomes to the polynome" )
                //    .def(self - self)
                //    .def(self + self)
                .def(self_ns::str(self_ns::self));
                msPolynomial::isPolynomialRegisteredInPython = 1 ;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msPolynomial::setExpression(string str){
            
            lexical_cast(str,Monomes);
            return mySharedPtr();
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msPolynomial::addMonomes(string str){
            
            map< vector<int>, double> monomes;
            lexical_cast(str,monomes);
            Monomes.insert(monomes.begin(),monomes.end());
            return mySharedPtr();
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msPolynomial::addMonome(vector<int> exposants,double a){
            
            if(Monomes.find( exposants )!= Monomes.end() )
                Monomes[exposants]+=a;
            else
                Monomes[exposants]=a;
            return mySharedPtr();
        }
        
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msPolynomial::evaluate(){
            
            const_monomeIterator it = Monomes.begin();
            double r=0;
            for( ;it!=Monomes.end();++it) {
                
                double a=(*it).second;
                if( (*it).first.size() != getCoordinates()->noOfDim() )
                    BOOST_THROW_EXCEPTION(msError("number of variables in the polynome different to the number of coordinates",
                                                  "double msPolynomial::evaluate() const",getFullId())
                                          );
                for( size_t i=0; i<(*it).first.size();++i){
                    
                    a*=pow( (*getCoordinates())[i]->getValue() , (*it).first[i]);
                }
                r+=a;
            }
            return r;
        }
        /*
         //-------------------------------------------------------------------------------------------------
         //-------------------------------------------------------------------------------------------------
         
         msPolynomial msPolynomial::operator+(const msPolynomial& rhs) const {
         
         const_monomeIterator it = rhs.Monomes.begin();
         boost::shared_ptr<msPolynomial> polynome = msPolynomial::New("");
         
         polynome->msTreeMapper::operator=(*this);
         
         for( ;it!=rhs.Monomes.end();++it) {
         cout<<(*it).second<<endl;
         if(polynome->Monomes.find( (*it).first )!=polynome->Monomes.end() )
         polynome->Monomes[ (*it).first ] += (*it).second;
         else
         polynome->Monomes[ (*it).first ]  = (*it).second;
         }
         //print(cout);
         cout<<endl;
         return *polynome;
         }
         
         //-------------------------------------------------------------------------------------------------
         //-------------------------------------------------------------------------------------------------
         
         msPolynomial msPolynomial::operator-(const msPolynomial& rhs) const {
         
         const_monomeIterator it = rhs.Monomes.begin();
         msPolynomial polynome;
         polynome.msTreeMapper::operator=(*this);
         
         for( ;it!=rhs.Monomes.end();++it) {
         
         if(polynome.Monomes.find( (*it).first )!=polynome.Monomes.end() )
         polynome.Monomes[ (*it).first ] -= (*it).second;
         else
         polynome.Monomes[ (*it).first ]  = -(*it).second;
         }
         return polynome;
         }
         
         //-------------------------------------------------------------------------------------------------
         //-------------------------------------------------------------------------------------------------
         
         msPolynomial msPolynomial::operator*(double a) const {
         
         msPolynomial polynome;
         polynome.msTreeMapper::operator=(*this);
         monomeIterator it = polynome.Monomes.begin();
         
         for( ;it!=polynome.Monomes.end();++it) (*it).second *= a;
         return polynome;
         }
         
         //-------------------------------------------------------------------------------------------------
         //-------------------------------------------------------------------------------------------------
         
         msPolynomial msPolynomial::operator/(double a) const{
         
         msPolynomial polynome;
         polynome.msTreeMapper::operator=(*this);
         monomeIterator it = polynome.Monomes.begin();
         
         for( ;it!=polynome.Monomes.end();++it) (*it).second /= a;
         return polynome;
         }
         
         //-------------------------------------------------------------------------------------------------
         //-------------------------------------------------------------------------------------------------
         */
        msPolynomial& msPolynomial::operator+=(const msPolynomial& rhs){
            
            const_monomeIterator it = rhs.Monomes.begin();
            
            for( ;it!=rhs.Monomes.end();++it) {
                
                if( Monomes.find( (*it).first ) !=rhs.Monomes.end() )
                    Monomes[ (*it).first ] += (*it).second;
                else
                    Monomes[ (*it).first ]  = (*it).second;
            }
            return *this;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        msPolynomial& msPolynomial::add(const msPolynomial& rhs,double coef){
            
            const_monomeIterator it = rhs.Monomes.begin();
            for( ;it!=rhs.Monomes.end();++it) {
                if( Monomes.find( (*it).first )!=rhs.Monomes.end() )
                    Monomes[ (*it).first ] += (*it).second * coef;
                else Monomes[ (*it).first ]  = (*it).second * coef;
            }
            return *this;
        }
        
        
        msPolynomial& msPolynomial::operator-=(const msPolynomial& rhs){
            
            const_monomeIterator it = rhs.Monomes.begin();
            cout<<"msPolynomial& msPolynomial::operator-=(const msPolynomial& rhs) "<<rhs.Monomes.size()<<endl;
            for( ;it!=rhs.Monomes.end();++it) {
                //cout<<"azazaz"<<endl;
                if( Monomes.find( (*it).first )!=rhs.Monomes.end() ){
                    
                    Monomes[ (*it).first ] -= (*it).second;
                    cout<<"monome ";
                    impact::operator<<(cout,(*it).first)<<" exists; ";
                    cout<<Monomes[ (*it).first ]<<endl;
                }
                else{
                    Monomes[ (*it).first ]  = -(*it).second;
                    impact::operator<<(cout,(*it).first)<<" don't exist; ";
                    cout<<Monomes[ (*it).first ]<<endl;
                }
            }
            return *this;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        msPolynomial& msPolynomial::operator*=(double a){
            
            monomeIterator it = Monomes.begin();
            
            for( ;it!=Monomes.end();++it) (*it).second *= a;  
            return *this;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        msPolynomial& msPolynomial::operator/=(double a){
            
            monomeIterator it = Monomes.begin();
            
            for( ;it!=Monomes.end();++it) (*it).second /= a;  
            return *this;
        }
        
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        ostream& msPolynomial::print(ostream& out) const{
            
            msScalarFunction::print(out);
            output::printHeader(out,"Polynomial");
            abstract(out);
            out<<endl;
            return out;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        ostream& msPolynomial::abstract(std::ostream& out) const{
            
            const_monomeIterator it = Monomes.begin();
            
            for( ;it!=Monomes.end();++it) {
                
                if( (*it).first.size() != getCoordinates()->noOfDim() ){
                    out<<"number of variables in the polynome different to the number of coordinates"<<endl;
                    return out;}
                if((*it).second<0)  out<<(*it).second<<"*[";
                if((*it).second>=0) out<<"+"<<(*it).second<<"*[";
                for(size_t i=0; i<(*it).first.size();i++) {
                    
                    out<<(*it).first[i];
                    if(i<(*it).first.size()-1) out<<",";
                }
                out<<"]";
            }
            return out;
        }
	}
}