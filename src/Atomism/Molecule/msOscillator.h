/*
 Copyright [yyyy] [name of copyright owner]
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 
 */
/*!
 * \file c_anhoscill.h
 * \brief Defines the class msOscillator
 * \author G.Reinisch
 */
#ifndef msOscillator_H
#define msOscillator_H

#include <msMotion.h>
#include <msQuantumInterface.h>



namespace impact {
    
    namespace atomism {
        
        class msOscillatorAssembly;
        
        
        /** \class msOscillator
         *
         * \brief Small amplitude vibrations modeled by a perturbative approach
         *
         * Virtual class for the description of coupled anharmonic small amplitude oscillators
         * by perturbative approaches (Dunham expansion) at the order 1 (msHarmonicOscillator) or
         * 2 (msAnharmonicOscillatorP2).
         * To be used, the oscillators are included in an assembly of oscillator manager
         * (msOscillatorsAssembly).
         * The msOscillator derived classes store normal mode frequency, anharmonic coefficients, 
         * and define the energy spectrum.
         *
         * \section PMA Physics, Mathematics, Algorithms
         *
         * The energy spectrum of an oscillator (indexed *) coupled at any order by Dunham
         * expansion to others is given by:
         \f[
         E(\mathbf{n})= G_0 + \omega(n_*+\frac{1}{2})
         + \sum_{i} \nu_{*i}(n_*+\frac{1}{2})(n_i+\frac{1}{2})
         + \sum_{ij} \nu_{*ij}(n_*+\frac{1}{2})(n_i+\frac{1}{2})(n_j+\frac{1}{2})
         +  ...
         \f]
         * where \f$ G_0 \f$ is a constant correction, \f$ \omega_* \f$ is the normal mode frequency
         * of the oscillator, \f$ \nu_{*j} \f$ the anharmonic coefficient between the oscillator and
         * the oscillator \f$ i \f$, \f$ \nu_{*j} \f$ the anharmonic coefficient between the oscillator
         * and the oscillators \f$ i \f$ and \f$ j \f$, and \f$ \mathbf{n} \f$ the occupancy numbers.
         * The anharmonic constants are computed by \e ab \e initio quantum chemistry codes.
         */
        class msOscillator : public msTreeMapper
        {
        private:
            friend class msOscillatorsAssembly;
            
            //! @Name from msRegistrer
            //@{
            static msRegistrar Registrar;
            static bool  isOscillatorRegisteredInPython;
            static msRegister* createInstance() { return new msOscillator; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void initialize(){ msTreeMapper::initialize();
                msTreeMapper::declareChild<msUnit>(FrequencyUnit,msUnit::New(),"FrequencyUnit");
            }
            
            void update(){ msTreeMapper::update();
            }
            
            /** @name Virtual functions
             * These functions have to be defined when the class is derived
             */
            //@{
            virtual double Energy( const vector<int>& inds ) {
                BOOST_THROW_EXCEPTION( msError("This method needs to be implemented and called by the derived classes"
                                               ,"double msOscillator::Energy( const vector<int>& inds )",  getFullId()) );
            }
            virtual double ZPE() const {
                BOOST_THROW_EXCEPTION( msError("This method needs to be implemented and called by the derived classes"
                                               ,"double msOscillator::ZPE()",  getFullId()) );
            }
            //@}
            
        public:
	  
	    msOscillator() : msTreeMapper() {
                constructVar("msOscillator","Oscillator","oscillator");
            }
            
            static boost::shared_ptr<msOscillator> New(){
                
                boost::shared_ptr<msOscillator> T( new msOscillator );
                T->initialize();
                T->update();
                return T;
            }
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msOscillator> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            size_t getIndice(){ return Indice; }
            /*
             boost::shared_ptr<msTreeMapper> setUnit(msUnitsManager& units) {
             
             FrequencyUnit->reset(units);
             return  mySharedPtr();
             }*/
            
            //! \brief return the energy of a particular state
            double Energy( const vector<int>& inds,msUnitsManager& units)  { return units.convert( *FrequencyUnit , Energy(inds) );
            };
            
            //! return the ZPE
            double ZPE(msUnitsManager& units) const { return units.convert(*FrequencyUnit,ZPE());
            };
            
            /** @name Virtual functions
             * These functions have to be defined when the class is derived
             */
            //@{
            //! return the maximum occupency number
            virtual int nvmax(double Eu , const vector<int>& nv , msUnitsManager& units ) {
                BOOST_THROW_EXCEPTION( msError("This method needs to be implemented and called by the derived classes"
                                               ,"virtual int nvmax(double Eu , const vector<int>& nv ) const",  getFullId()) );
            };
            
            //! return 0 if the state is bounded, 1 if not
            virtual bool CheckDeriv( const vector<int>& inds ) {
                BOOST_THROW_EXCEPTION( msError("This method needs to be implemented and called by the derived classes"
                                               ,"virtual bool CheckDeriv(vector<int>& inds=0)",  getFullId()) );
            };
            
            /** \brief A (small) string to describe the coupling
             */
            virtual ostream& print(ostream& out) const;
            virtual ostream& abstract(ostream& out) const {out<<"Unit: "<<FrequencyUnit->getStr();
                return out;
            };
            
            virtual bool isHarmonic(){ return 1; };
            //@}
        protected:
            
            msChild<msUnit> FrequencyUnit;
            
            size_t Indice;		//!< Reference indice of this motion
            
        };
        
        
        ostream& operator<<(ostream&,const msOscillator&);
        
        /** \class msHarmonicOscillator
         *
         * \brief  Harmonic oscillator
         *
         * This class uses a first order devellopment of the energie to express the eigenvalues:
         \f[
         E(\mathbf{n})= G_0 + \omega(n_*+\frac{1}{2})
         \f]
         * where \f$ G_0 \f$ is a constant correction, \f$ \omega_* \f$ is the normal mode frequency
         * of the oscillator, \f$ \nu_{*j} \f$ and \f$ \mathbf{n} \f$ the occupency numbers.
         */
        class msHarmonicOscillator : public msOscillator
        {
            
        private:
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool        isHarmonicOscillatorRegisteredInPython;
            static msRegister* createInstance() { return new msHarmonicOscillator; }
            //@}
            
            
        protected:
            
            void registryInPython();
            
            void initialize(){ msOscillator::initialize();
                msTreeMapper::declareAttribute( Frequency , "Frequency" );
            }
            
            void update(){ msOscillator::update();
            }
            
            double Frequency;
            
        public:
            
	    msHarmonicOscillator() : msOscillator() {
                Frequency=0 ;
                constructVar("msHarmonicOscillator","HarmonicOscillator","harmonic oscillator");
            }
            
            static boost::shared_ptr<msHarmonicOscillator> New( ){
                
                boost::shared_ptr<msHarmonicOscillator> T( new msHarmonicOscillator );
                T->initialize();
                T->update();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msHarmonicOscillator> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            boost::shared_ptr<msTreeMapper> set(double f, msUnitsManager& units) {
                
                Frequency = f / units.convert( *FrequencyUnit , 1);
                return  mySharedPtr();
            }
            
            boost::shared_ptr<msTreeMapper> set(double f) {
                
                Frequency = f ;
                return  mySharedPtr();
            }
            
            double getFrequency(msUnitsManager& units) { return  units.convert( *FrequencyUnit, Frequency ); }
            
            
            double Q(double T,double R) {  return 1./(1-exp(- Frequency /(R*T) ) ) * exp(-Frequency/2 / (R*T) ); }
            
            virtual ostream& print(ostream& out) const {  msOscillator::print(out);
                output::printHeader(out,"msHarmonicOscillator");
                abstract(out);
                return out;
            } ;
            
            virtual ostream& abstract(ostream& out) const{msOscillator::abstract(out);out<<"; Fundamental frequency: "<<Frequency;
                return out;
            }
            double ZPE(msUnitsManager& units) const { return units.convert(*FrequencyUnit,ZPE());
            };
            
        protected:
            
            double Energy(int ind)  { return Frequency*( ind +0.5 );
            }
            
            virtual double Energy(const vector<int>& inds ) { return Frequency*( inds[Indice] +0.5 );
            }
            
            virtual double ZPE() const  { return 0.5*Frequency;
            }
            
            int nvmax(double Eu, const vector<int>& nv, msUnitsManager& units)  { return Eu/units.convert(*FrequencyUnit,Frequency)+1;
            }
            
            bool CheckDeriv(vector<int>* nv=0)  { return 0; }
        };
        
        
        /** \class msAnharmonicOscillatorP2
         *
         * \brief Anharmonic oscillator coupled at the order 2
         *
         * This class uses a second order devellopment of the energie:
         \f[
         E(\mathbf{n})= G_0 + \omega(n_*+\frac{1}{2})
         + \sum_{i} \nu_{*i}(n_*+\frac{1}{2})(n_i+\frac{1}{2})
         \f]
         * where \f$ G_0 \f$ is a constant correction, \f$ \omega_* \f$ is the normal mode frequency
         * , \f$ \nu_{*j} \f$ the anharmonic coefficient with
         * the oscillator \f$ i \f$, and \f$ \mathbf{n} \f$ the occupency numbers.
         */
        class msAnharmonicOscillatorP2 : public msHarmonicOscillator
        {
            
        private:
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool        isAnharmonicOscillatorP2RegisteredInPython;
            static msRegister* createInstance() { return new msAnharmonicOscillatorP2; }
            //@}
            
        protected:
            
            void initialize(){ msHarmonicOscillator::initialize();
                msTreeMapper::declareAttribute( AnhCstesP2 , "AnhCstesP2" );
            }
            
            void update(){ msHarmonicOscillator::update(); }
            
            std::vector<double> AnhCstesP2;	//!< Anharmonic constants
            
            
        public:
            
	    msAnharmonicOscillatorP2() : msHarmonicOscillator() {
                constructVar("msAnharmonicOscillatorP2",
                             "AnharmonicOscillatorP2",
                             "anharmonic oscillator 2");
            }
            
            void registryInPython();
            
            static boost::shared_ptr<msAnharmonicOscillatorP2> New( ){
                
                boost::shared_ptr<msAnharmonicOscillatorP2> T( new msAnharmonicOscillatorP2 );
                T->initialize();
		T->update();
                return T;
            }
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msAnharmonicOscillatorP2> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            //!\name set anharmonicity
            //@{
            boost::shared_ptr<msTreeMapper> set(double f, string anharlists, msUnitsManager& units) {
                
                fillVecFromString(anharlists,AnhCstesP2," "); return set(f,AnhCstesP2,units);
            }
            
            boost::shared_ptr<msTreeMapper> set(double f, vector<double> anharlists, msUnitsManager& units) {
                
                msHarmonicOscillator::set(f,units);
                for(size_t i=0; i< anharlists.size(); i++ ) AnhCstesP2[i] = anharlists[i] / units.convert( *FrequencyUnit , 1);
                return  mySharedPtr();
            }
            
            boost::shared_ptr<msTreeMapper> set(double f, string anharlists) {
                
                fillVecFromString(anharlists,AnhCstesP2," "); return set(f,AnhCstesP2);
            }
            
            boost::shared_ptr<msTreeMapper> set(double f, vector<double> anharlists) {
                
                msHarmonicOscillator::set(f); AnhCstesP2 = anharlists; return  mySharedPtr();
            }
            //@}
            
            virtual ostream& print(ostream& out) const {  msHarmonicOscillator::print(out);
                output::printHeader(out,"msAnharmonicOscillatorP2");
                abstract(out);
                return out;
            } ;
            
            virtual ostream& abstract(ostream& out) const{ msHarmonicOscillator::abstract(out);out<<"; 2nd order coupling constants: ";
                for(size_t j=0; j<AnhCstesP2.size(); j++) out<<AnhCstesP2[j]<<" ";
                return out;
            }
            
            bool isHarmonic(){return 0;};
            
            
            int nvmax(double Eu,const vector<int>& nv,msUnitsManager& units)  {
                
                Eu/=units.convert(*FrequencyUnit,1);
                if( Eu<0 )    return -1;
                int nvmax=0;  double sum=0.;  double wAndSum = Frequency +sum;
                
                if( AnhCstesP2[Indice] != 0 ) {
                    
                    sum=0;
                    for(size_t i=0;i<AnhCstesP2.size();i++)  if( i!=size_t(Indice) ) sum += ( nv[i] + 0.5 )*AnhCstesP2[i];
                    
                    double zpe = AnhCstesP2[Indice]/4. + Frequency/2. + sum/2.;
                    
                    double vd = - wAndSum / ( 2.0 * AnhCstesP2[Indice] ) - 0.5;
                    double D  = - pow(wAndSum,2) / ( 4. * AnhCstesP2[Indice] ) -zpe ;
                    double vmax = vd*( 1. - sqrt(1-Eu/D) );
                    
                    nvmax = int(vmax);
                    
                    if( (wAndSum>0) && (AnhCstesP2[Indice]<0) && (Eu > D) )  nvmax = int(vd);
                    if(  wAndSum<0 ) nvmax=-1;
                }
                else{ nvmax=Eu/Frequency; }
                
                return nvmax;  
            }       
            
            bool CheckDeriv(const vector<int>& nv) 
            {
                double sum = 0.0;
                
                for(size_t i=0; i<AnhCstesP2.size(); i++) sum += AnhCstesP2[i]*( nv[i] + 0.5); 
                
                double deriv= Frequency + (nv[Indice]+0.5)*AnhCstesP2[Indice] + sum;
                
                if( deriv<=0 ) return 1; 
                return 0;   
            }
            
        protected:
            
            double Energy(const vector<int>& inds)  {
                
                double e = msHarmonicOscillator::Energy(inds) ; //+  AnhCstesP2[Indice] * pow( (inds[Indice] + 0.5) , 2 );
                //note: the first 0.5 is to account for the double counting (i,j) and (j,i)
                
                for(size_t j=Indice; j<AnhCstesP2.size(); j++) e +=   AnhCstesP2[j]*( inds[Indice] + 0.5) * ( inds[j] + 0.5 );
                
                return e;
            }
            
            double ZPE() const {
                
                double eng = msHarmonicOscillator::ZPE() ;//+ AnhCstesP2[Indice]/4.;
                for(size_t j=Indice; j<AnhCstesP2.size(); j++)  eng +=  0.25 * AnhCstesP2[j];
                
                return eng;
            }
            
        };
        
    }
    
}
#endif // msOscillator_H
