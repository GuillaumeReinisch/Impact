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

#include "msOscillatorsAssembly.h"
namespace impact {
    
    namespace atomism {
        
        bool msOscillatorsAssembly::isOscillatorsAssemblyRegisteredInPython = 0;
        
        msRegistrar msOscillatorsAssembly::Registrar("msOscillatorsAssembly", msOscillatorsAssembly::createInstance);
        
        
        ostream& operator<<(ostream& out,const msOscillatorsAssembly& oscil) {
            
            oscil.print(out);
            return out;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msOscillatorsAssembly::registryInPython() {
            
#if USE_PYTHON
            msMotion::registryInPython();
            msQuantumInterface::registryInPython();
            
            if( ! msOscillatorsAssembly::isOscillatorsAssemblyRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msOscillatorsAssembly,bases<msMotion,msQuantumInterface>,boost::shared_ptr<msOscillatorsAssembly> >
                ("msOscillatorsAssembly",
                 "Study an assembly of oscillators: quantum small amplitude coupled motions.",no_init )
                .def( "New", &msOscillatorsAssembly::New , "Create a new object. arg2: msUnitsManager object")
                .staticmethod("New")
                .def("addOscillator" ,  &msOscillatorsAssembly::addOscillator , "Add an oscillator to the assembly. arg2: oscillator to add")
                .def("generateAnharmonicBoundedState" ,  &msOscillatorsAssembly::generateAnharmonicBoundedState ,
                     "Generate a random bounded state below a target energy for the anharmonic oscillators. arg2: energy")
                .def(self_ns::str(self_ns::self));
                
                msOscillatorsAssembly::isOscillatorsAssemblyRegisteredInPython=1;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msOscillatorsAssembly::addOscillator(boost::shared_ptr<msOscillator> oscill) {
            
            try{
                if( !oscill->hasParent() ) oscill->setAffiliation(mySharedPtr());
                
                if( oscill->isHarmonic() ){
                    boost::shared_ptr<msHarmonicOscillator> ho=boost::static_pointer_cast<msHarmonicOscillator>(oscill);
                    msTreeMapper::addElementToChildren<msHarmonicOscillator>(HarmOscillators,ho);
                }
                else{ msTreeMapper::addElementToChildren<msOscillator>(AnhOscillators,oscill);
                    Occups.push_back(0);
                    oscill->Indice=Occups.size()-1;
                }
                Oscillators.push_back(oscill); nDoF++; updateParameters();
            }
            catch(msException& e) {e.addContext("Can not add the oscillator to the assembly (boost::shared_ptr<msTreeMapper> msOscillatorsAssembly::addOscillator(boost::shared_ptr<msOscillator> oscill))");
            }
            return mySharedPtr();
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msOscillatorsAssembly::updateParameters(){
            
            nEigenLimit = 0;
            try{
                msMotion::updateParameters();
                getParameters()->queryIntValue("DCLim", DCLim );
                getParameters()->queryIntValue("nGaussHerm", nGaussHerm);
                getParameters()->queryIntValue("nEigenLimit", nEigenLimit);
                getParameters()->queryIntValue("nMaxTrialsWL", nMaxTrialsWL);
            }
            catch(msException& e)
            {
                e.addContext("can't get the parameters, parameter object has been corrupted (void msOscillatorsAssembly::updateParameters()).");
                IMPACT_THROW_EXCEPTION( e );
            }
            int n = max(nDoF,1);
            DensOfStates->set(0,    getEmax(), dE, getUnits()->getEnergyStr(), getUnits()->getEnergyStr()+"^-1" );
            SumOfStates->set(0,     getEmax(), dE, getUnits()->getEnergyStr(), "" );
            DensOfStates_HO->set(0, getEmax(), dE, getUnits()->getEnergyStr(), "" );
            DensOfStates_AO->set(0, getEmax(), dE, getUnits()->getEnergyStr(), "" );
            EigenValues = vector_type::Zero(nEigenLimit);
            g.resize(nEigenLimit);
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        double msOscillatorsAssembly::ZPE() const {
            
            double e=0; boost::shared_ptr<msOscillator> oscill;
            
            for(const_iteratorOscillator it = Oscillators.begin(); it!=Oscillators.end(); ++it)
                
                if( (oscill  = (*it).lock()) ) e += oscill->ZPE(*getUnits());
            
            return e;
        }
        
        
        double msOscillatorsAssembly::ZPE_AO() const {
            
            double e=0;
            for(const_itAnhOscillator it = AnhOscillators.begin(); it!=AnhOscillators.end(); ++it)
                
                e += (*it)->ZPE(*getUnits());
            
            return e;
        }
        
        double msOscillatorsAssembly::ZPE_HO() const {
            
            double e=0;
            for(const_itHarmOscillator it = HarmOscillators.begin(); it!=HarmOscillators.end(); ++it)
                
                e += (*it)->ZPE(*getUnits());
            
            return e;
        }
        
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        double msOscillatorsAssembly::energyAO(const vector<int>& inds) const {
            
            double e=0; boost::shared_ptr<msOscillator> oscill;
            
            for(const_iteratorOscillator it = Oscillators.begin(); it!=Oscillators.end(); ++it)
                
                if( (oscill  = (*it).lock()) ) e += oscill->Energy(inds,*getUnits());
            
            return e;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msOscillatorsAssembly::computeDOS()
        {
            DensOfStates->reset(0);
            DensOfStates_AO->reset(0);
            DensOfStates_HO->reset(0);
            
            if( AnhOscillators.size()>0  ){  computeDOS_AOpart();
                TranslateForZPE(*DensOfStates_AO,ZPE_AO());
            }
            
            if( HarmOscillators.size()>0 ){ computeDOS_HOpart();
                TranslateForZPE(*DensOfStates_HO,  ZPE_HO());
            }
            ConvolutionDos();
            
            fillSOSfromDOS();
        }
        
        
        
        void msOscillatorsAssembly::TranslateForZPE(msVectorFit1d& dos,  double ZPE ) {
            
            int zpe=ZPE/dE + 0.5;
            for( int j=dos.size() -1; j>=zpe; j--) dos[j]=dos[j-zpe];
            
            dos[zpe]=1./dE;
            for( int j=zpe-1; j>=0; j--) dos[j]=0;
        }
        
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msOscillatorsAssembly::ConvolutionDos()
        {
            LOGGER_ENTER_FUNCTION( "void msOscillatorsAssembly::ConvolutionDos()" , getFullId());
            
            if( AnhOscillators.size() == 0 )  { DensOfStates->vector<double>::operator=(*DensOfStates_HO);
                LOGGER_EXIT_FUNCTION();
                return;
            }
            
            if( HarmOscillators.size() == 0 ) { DensOfStates->vector<double>::operator=(*DensOfStates_AO);
                LOGGER_EXIT_FUNCTION();
                return;
            }
            
            DensOfStates->set(0, getEmax(), dE, getUnits()->getEnergyStr(),getUnits()->getEnergyStr()+"^-1");
            
            for( double Evis=0; Evis < getEmax(); Evis+=dE ) {
                
                int iVis = Evis/dE ;  (*DensOfStates)[iVis] = 0;
                
                for( double e=0 ; e<=Evis ; e+=dE)  {
                    
                    int ieA = e/dE + 0.5;  int ieB = (Evis-e)/dE + 0.5;
                    
                    if( (ieA<int(DensOfStates_AO->size())) && (ieB<int(DensOfStates_HO->size())) )
                        
                        (*DensOfStates)[iVis] += (*DensOfStates_AO)[ieA]*dE * (*DensOfStates_HO)[ieB]*dE ;
                }
            }
            
            for( size_t i=0 ; i<  (*DensOfStates).size(); i++) (*DensOfStates)[i] /= dE;
            
            LOGGER_EXIT_FUNCTION();
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msOscillatorsAssembly::computeDOS_HOpart()
        {
            LOGGER_ENTER_FUNCTION("void msOscillatorsAssembly::computeDOS_HOpart(vector<msAnhOscill*>* HAO)",getFullId());
            LOGGER_WRITE(msLogger::INFO,"Use the Beyer Swinehart algorithm ");
            
            int M = msMotion::getEmax()/dE;
            
            vector<int> R; R.resize(HarmOscillators.size(),0); (*DensOfStates_HO)[0]=1;
            
            for(size_t i=0; i<HarmOscillators.size(); i++)  R[i] = HarmOscillators[i]->getFrequency(*getUnits()) / dE + 0.5 ;
            
            for(size_t i=0; i<HarmOscillators.size(); i++)  for(int N=0; N<M-R[i]; N++)  (*DensOfStates_HO)[ R[i]+N ] += (*DensOfStates_HO)[N];
            
            for(size_t i=0; i<  (*DensOfStates_HO).size(); i++)  (*DensOfStates_HO)[i]/=dE;
            
            LOGGER_EXIT_FUNCTION();
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msOscillatorsAssembly::computeDOS_AOpart()
        {
            LOGGER_ENTER_FUNCTION("void msOscillatorsAssembly::computeDOS_AOpart(vector<msAnhOscill*>* HAO)",getFullId());
            getParameters();
            if( AnhOscillators.size() < 2){ LOGGER_WRITE(msLogger::ERROR,"At least 2 anharmonic oscillators are needed");
                LOGGER_EXIT_FUNCTION();
                return;
            }
            if( (AnhOscillators.size() > 6) || (int(AnhOscillators.size()) > DCLim))
                computeDOS_WangLandau();
            else computeDOS_Direct();
            
            LOGGER_EXIT_FUNCTION();
        }
        
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msOscillatorsAssembly::computeDOS_Direct() {
            
            LOGGER_ENTER_FUNCTION("void msOscillatorsAssembly::compDOS_Direct()", getFullId() );
            
            DensOfStates_AO->set(0, getEmax(), dE, getUnits()->getEnergyStr(),getUnits()->getEnergyStr()+"^-1");
            
            int count=0;
            
            if( AnhOscillators.size() == 2 ) 	count=compDOSd_2();
            if( AnhOscillators.size() == 3 ) 	count=compDOSd_3();
            if( AnhOscillators.size() == 4 ) 	count=compDOSd_4();
            if( AnhOscillators.size() == 5 ) 	count=compDOSd_5();
            if( AnhOscillators.size() == 6 ) 	count=compDOSd_6();
            if( AnhOscillators.size()  > 6 )  	LOGGER_WRITE(msLogger::ERROR,"The direct count is implemented for nDoF<7");
            
            LOGGER_WRITE(msLogger::INFO,"Number of states counted:"+output::getString<int>(count));
            if(count<nEigenLimit)    isDiscrete=1;
            if(count>=nEigenLimit){  isDiscrete=0; EigenValues.resize(0); g.resize(0);
                LOGGER_WRITE(msLogger::INFO,"Number of states > limit providen, direct count desactivated");
            }
            LOGGER_EXIT_FUNCTION();
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msOscillatorsAssembly::computeDOS_WangLandau()
        {
            LOGGER_ENTER_FUNCTION("void msOscillatorsAssembly::compDOS_WangLandau()",getFullId());
            
            if( AnhOscillators.size() < 2){ LOGGER_WRITE(msLogger::ERROR,"At least 2 anharmonic oscillators are needed");
                LOGGER_EXIT_FUNCTION();
                return;
            }
            
            //-------Initialize parameters and vectors----------------------------------------------
            /*
             if(rndType==0) { idum=-1147483647; iv.resize(32,0); iy=0;}
             if(rndType==1) { srand ( 11 ); }
             if(rndType==2) { const gsl_rng_type * T; gsl_rng_env_setup();
             T = gsl_rng_default; rnd_gsl = gsl_rng_alloc (T);
             }*/
            
            int ngrains   = getEmax()/dE;
            int MaxTrials = nMaxTrialsWL * ngrains;  int iters = 21;  double ln_f=1;  double p = min(1.0/nDoF,0.25);
            
            vector<int> 	nv;		nv.resize(nDoF);
            vector<int> 	nvold; 		nvold.resize(nDoF);
            vector<double> H; 		H.resize(ngrains+1,0);
            vector<double> g;  		g.resize(ngrains+1,0);
            
            vector<double> OmegaFtot; 	OmegaFtot.resize(ngrains+1,0);
            vector<double> Ev; 		Ev.resize(ngrains+1,0);
            
            LOGGER_WRITE(msLogger::INFO,"coupled DoF:"+output::getString<int>(AnhOscillators.size()));
            
            // if( ReactMotion==0 ) LOGGER(msLogger::INFO,"No reactional motion coupled");
            // if( ReactMotion!=0 ) LOGGER(msLogger::INFO,"Reactional motion coupled");
            
            //-----------------------------------------------------------------------------------
            
            generateAnharmonicBoundedState(getEmax());
            LOGGER_WRITE(msLogger::INFO,"Bounded state generated");
            
            double zpe=ZPE();  double enrg=energyAO(Occups)-zpe;  int nold=enrg/dE+0.5;  double test=0;
            
            int Nrej,Nacc,Nout;
            for( int i=0; i<nDoF; i++)  nvold[i]=Occups[i];
            
            for( int i=0; i<iters; i++){
                
                Nrej = 0;  Nacc = 0;  Nout = 0;  for(int j=0; j<ngrains; j++) H[i]=0;
                
                for(int j=0; j<MaxTrials; j++) {
                    
                    for( size_t k=0; k<AnhOscillators.size();k++) {
                        
                        test=Ran1();
                        if(test  <=  p     )  nv[ AnhOscillators[k]->getIndice() ] = max(0,nvold[ AnhOscillators[k]->getIndice() ]-1);
                        else
                            if( test <=  2.0*p )  nv[ AnhOscillators[k]->getIndice() ] = nvold[ AnhOscillators[k]->getIndice() ] + 1;
                            else                  nv[ AnhOscillators[k]->getIndice() ] = nvold[ AnhOscillators[k]->getIndice() ];
                    }
                    
                    int nnew = (energyAO( nv)-zpe) / dE + 0.5;   bool isBounded=0;
                    
                    if( (nnew >= 0 ) && (nnew <= ngrains) )  isBounded = 1;/*isStateBounded(&nv);*/
                    
                    if(!isBounded )  Nout++; 	    			//rejected: tried to step out energy range
                    
                    if( isBounded ) { double acc=exp(g[nold]-g[nnew]);     // acceptance probability
                        
                        if( Ran1() < acc ) {			// accept for a move
                            
                            nold=nnew;  Nacc++;
                            for(int l=0; l<nDoF; l++) nvold[l]=nv[l];
                        }
                        else  Nrej++;  			// rejected: sits tight
                    }
                    H[nold]++; g[nold] += ln_f;
                    
                    /* if( (ReactMotion!=0) && (i==iters-1) ){ OmegaFtot[nold] += ReactMotion->OmegaF(&nvold);
                     Ev[nold]  	 += energyAO(&nvold);
                     }*/
                }
                ln_f/=2.;
                
                LOGGER_WRITE(msLogger::INFO,"Step" + output::getString<int>(i)+"/"+output::getString<int>(iters)
                             + " ; State accepted:"+output::getString<double>(100.*Nacc/MaxTrials)
                             + " % ; State rejected:"+output::getString<double>(100.*Nout/MaxTrials)+" %");
            }
            
            //------Finalize DOS------------------------------------------------------------------
            
            DensOfStates_AO->set(0, getEmax(), dE, getUnits()->getEnergyStr(),getUnits()->getEnergyStr()+"^-1");
            
            for(int i=0;i<ngrains; i++) (*DensOfStates_AO)[i] = exp( g[i] - g[0] )/dE;
            
            isDiscrete=0;/*
                          LOGGER_WRITE(msLogger::INFO, " Qaho(300K)="+output::getString<double>(QfromDOS_AHO(300))
                          + "; Qaho(1500K)="+output::getString<double>(QfromDOS_AHO(1500)) );*/
            
            LOGGER_EXIT_FUNCTION();
            return;
            
            //-----Finalize semi classical part (if any)-------------------------------------------
            /*
             for( int i=0 ; i<ngrains; i++)  if( H[i]>0 ) OmegaFtot[i]/=H[i]; Ev[i]/=H[i];
             
             for( int i=0 ; i<ngrains; i++)
             {
             (*SumOfReacTraj)[i]=0.0;  double eng= i*dE;
             
             for( int j=0; j<=i ; j++) if( (H[j]>0) && (eng>=Ev[j]))
             (*SumOfReacTraj)[i] +=  (*DensOfStates_AO)[j] *  ReactMotion->TransProb( eng - Ev[j] , OmegaFtot[j] );
             }
             
             (*DensOfReacTraj)[0]=0;
             for(int i=1;i<ngrains; i++) (*DensOfReacTraj)[i] = *SumOfReacTraj)[i]-(*SumOfReacTraj)[i-1];
             
             
             if(rndType==2) { gsl_rng_free (rnd_gsl); }*/
            
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msOscillatorsAssembly::generateAnharmonicBoundedState(double emax) {
            
            LOGGER_ENTER_FUNCTION("void msOscillatorsAssembly::GenerateBoundedState(double emax)",getFullId());
            bool isBounded=0;   int nstart=0;
            
            while( !isBounded ) {
                
                double Estart=Ran1()*emax;
                for(int k=0; k<Occups.size(); k++) Occups[k]=0;
                for(size_t l=0; l<Occups.size(); l++) {
                    
                    double Eu=Estart-( energyAO(Occups) - ZPE() );
                    if(Eu!=0) {
                        nstart = AnhOscillators[l]->nvmax(Eu, Occups, *getUnits() );
                        Occups[ AnhOscillators[l]->getIndice() ] = nstart*double(rand()) / double(RAND_MAX);
                    }
                }
                
                isBounded = isStateBounded(Occups);
            }
            
            LOGGER_EXIT_FUNCTION();
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        bool msOscillatorsAssembly::isStateBounded(const vector<int>& nv) const {
            
            for(size_t i=0;i< AnhOscillators.size(); i++)  if( AnhOscillators[i]->CheckDeriv(nv) ) return 0;
            
            return 1;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        /*
         double msOscillatorsAssembly::Q_HO(double T){
         
         boost::shared_ptr<msHarmonicOscillator> ho = msHarmonicOscillator::New(getUnits);
         double q = 1;
         
         for(size_t i=0; i<HarmOscillators.size(); i++) {
         
         ho->set( HarmOscillators[i]->getFrequency( *getUnits() ) );
         q *= ho->Q(T);
         }
         
         return q;
         }*/
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        double msOscillatorsAssembly::Qho(double T)
        {/*
          boost::shared_ptr<msHarmonicOscillator> ho = msHarmonicOscillator::New( getUnits()->getEnergyStr() );
          boost::shared_ptr<msOscillator> oscill;
          double q = 1;
          
          for(iteratorOscillator it = Oscillators.begin(); it!=Oscillators.end(); ++it)
          
          if( oscill  = (*it).lock()){
          ho->set( oscill->getFrequency( *getUnits() ) , *getUnits() );
          q *= ho->Q( T , getUnits()->convert(msUnit::J_mol,csts::R) );
          }
          
          return q;*/
            return 0;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        double msOscillatorsAssembly::Q(double T)
        {
            LOGGER_ENTER_FUNCTION("double msOscillatorsAssembly::Q(double T)",getFullId());
            
            //-------------compute Q for the HO oscillators ------- //
            double q=1;
            
            if( HarmOscillators.size() > 0 ) {
                
                boost::shared_ptr<msHarmonicOscillator> ho = msHarmonicOscillator::New( );
                ho->FrequencyUnit->set( getUnits()->getEnergyStr() );
                for(size_t i=0; i<HarmOscillators.size(); i++){
                    
                    ho->set( HarmOscillators[i]->getFrequency( *getUnits() ) , *getUnits() );
                    q *= ho->Q( T, getUnits()->convert(msUnit::J_mol,csts::R) );
                }
                if( AnhOscillators.size() == 0 ) { LOGGER_EXIT_FUNCTION(); return q;}
            }
            
            //-------------compute Q for the AHO oscillators ------- //
            
            if( DensOfStates_AO->size() != 0 )  LOGGER_WRITE(msLogger::INFO, "DOS/eigenvalues already computed, no re-computation");
            if( DensOfStates_AO->size() == 0 ) {
                
                LOGGER_WRITE(msLogger::INFO, "Anharmonic DOS not computed yet, compute DOS for the anharmonic oscillators");
                computeDOS_AOpart();
            }
            /*
             if(isDiscrete) {
             
             LOGGER_WRITE(msLogger::INFO, "Use the energy eigenvalues to compute the AHO partition function");
             LOGGER_EXIT_FUNCTION();
             return q *= msQuantumInterface::Q( T , getUnits()->convert(msUnit::J_mol,csts::R) );
             }*/
            
            LOGGER_WRITE(msLogger::INFO, "Use the DOS to compute the AHO partition function");
            
            q *= msMotion::QfromDOS( *DensOfStates_AO , T );
            LOGGER_EXIT_FUNCTION();
            return q;
        }
        
        
        
        bool msOscillatorsAssembly::isQuantum()
        {
            return isDiscrete;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        double msOscillatorsAssembly::Ran1()
        {
            double ran1=0;
            /* boost::random::uniform_int_distribution<> dist(0, 2147483647 );
             return dist(gen) / 2147483647 ;*/
            /*if( rndType==0 )
             {
             int IA=16807;int IM=2147483647; double AM=1./IM; int IQ=127773;
             int IR=2836;int NTAB=32;int NDIV=1+(IM-1)/NTAB;
             double EPS=2.3e-16;double RNMX=1.0-EPS;
             
             if( (idum<=0) || (iy==0) )
             {
             idum=max(-idum,1);
             
             for(int j=NTAB+8;j>=1;j--){ int k=idum/IQ;
             idum=IA*(idum-k*IQ) - IR*k;
             if(idum<0)  idum+=IM;
             if(j<=NTAB) iv[j]=idum;
             }
             iy=iv[1];
             }
             int k=idum/IQ;  idum=IA*(idum-k*IQ) - IR*k;
             if(idum<0) idum+=IM;
             int j=1+iy/NDIV;  iy=iv[j];  iv[j]=idum; double T=AM*iy;
             ran1=min(T,RNMX);
             }
             if( rndType==1 ) ran1=rand()/double(RAND_MAX);
             if( rndType==2 )*/
            //ran1=gsl_rng_uniform(rnd_gsl);
            
            return ran1;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        double msOscillatorsAssembly::compDOSd_2()
        {
            double Eu=getEmax();double tp;
            for(int i=0;i<nDoF;i++) Occups[i]=0;
            int n1,n2;
            n1=AnhOscillators[0]->nvmax(Eu,Occups,*getUnits());
            int count=0;
            
            for(int i=0; i<n1 ; i++)
            {
                Occups[ AnhOscillators[0]->getIndice() ]=i; for(int dof=1;dof<nDoF;dof++){Occups[dof]=0;}
                Eu = getEmax() - ( energyAO(Occups)-ZPE() );  n2 = AnhOscillators[1]->nvmax(Eu,Occups,*getUnits());
                
                for( int j=0;j<n2; j++)
                {
                    Occups[ AnhOscillators[1]->getIndice() ]=j;
                    tp=energyAO(Occups)-ZPE();
                    
                    if( (tp<=getEmax()-dE) && (isStateBounded(Occups)) )
                    {
                        if(count<nEigenLimit){EigenValues[count]=tp;g[count]=1;}
                        (*DensOfStates_AO)[ int(tp/dE+0.5) ] += 1/dE;
                        count++;
                    }
                    /* if(ReactMotion!=0)
                     for(int e=0;e<Emax/dE;e++) (*SumOfReacTraj)[e]+=ReactMotion->TransProb(e*dE);
                     */
                }
            }
            return count;
        }
        
        double msOscillatorsAssembly::compDOSd_3()
        {
            double Eu=getEmax();double tp;
            for(int i=0;i<nDoF;i++) Occups[i]=0;
            int n1,n2,n3;
            n1=AnhOscillators[0]->nvmax(Eu,Occups,*getUnits());
            int count=0;
            for(int i=0; i<n1 ; i++)
            {
                Occups[ AnhOscillators[0]->getIndice() ]=i; for(int dof=1;dof<nDoF;dof++){Occups[dof]=0;}
                Eu = getEmax() - ( energyAO(Occups)-ZPE() );  n2 = AnhOscillators[1]->nvmax(Eu,Occups,*getUnits());
                for( int j=0;j<n2; j++)
                {
                    Occups[ AnhOscillators[1]->getIndice() ]=j; for(int dof=2;dof<nDoF;dof++){Occups[dof]=0;}
                    Eu = getEmax() - ( energyAO(Occups)-ZPE() ); n3 = AnhOscillators[2]->nvmax(Eu,Occups,*getUnits());
                    for(int k=0;k<n3; k++)
                    {
                        Occups[ AnhOscillators[2]->getIndice() ]=k;
                        tp=energyAO(Occups)-ZPE();
                        if( (tp<=getEmax()-dE) && (isStateBounded(Occups)) )
                        {
                            if(count<nEigenLimit){ EigenValues[count]=tp;g[count]=1; }
                            
                            (*DensOfStates_AO)[ int(tp/dE+0.5) ] += 1/dE;
                            
                            count++;
                        }
                        /*   if(ReactMotion!=0)
                         for(int e=0;e<getEmax()/dE;e++) (*SumOfReacTraj)[e]+=ReactMotion->TransProb(e*dE);*/
                        
                    }
                }
            }
            return count;
        }
        
        double msOscillatorsAssembly::compDOSd_4()
        {
            double Eu=getEmax();double tp;
            for(int i=0;i<nDoF;i++) Occups[i]=0;
            int n1,n2,n3,n4;
            n1=AnhOscillators[0]->nvmax(Eu,Occups,*getUnits());
            int count=0;
            
            for(int i=0; i<n1 ; i++)
            {
                Occups[ AnhOscillators[0]->getIndice() ]=i; for(int dof=1;dof<nDoF;dof++){Occups[dof]=0;}
                Eu = getEmax() - ( energyAO(Occups)-ZPE() );  n2 = AnhOscillators[1]->nvmax(Eu,Occups,*getUnits());
                
                for( int j=0;j<n2; j++)
                {
                    Occups[ AnhOscillators[1]->getIndice() ]=j; for(int dof=2;dof<nDoF;dof++){Occups[dof]=0;}
                    Eu = getEmax() - ( energyAO(Occups)-ZPE() ); n3 = AnhOscillators[2]->nvmax(Eu,Occups,*getUnits());
                    
                    for(int k=0;k<n3; k++)
                    {
                        Occups[ AnhOscillators[2]->getIndice() ]=k; for(int dof=3;dof<nDoF;dof++){Occups[dof]=0;}
                        Eu = getEmax() - ( energyAO(Occups)-ZPE() ); n4 = AnhOscillators[3]->nvmax(Eu,Occups,*getUnits());
                        
                        for(int l=0;l<n4; l++)
                        {
                            Occups[ AnhOscillators[3]->getIndice() ]=l;
                            tp=energyAO(Occups)-ZPE();
                            
                            if( (tp<=getEmax()-dE) && (isStateBounded(Occups)) )
                            {
                                if(count<nEigenLimit){EigenValues[count]=tp;g[count]=1;}
                                (*DensOfStates_AO)[ int(tp/dE+0.5) ] += 1/dE;
                                count++;
                            }
                            /*   if(ReactMotion!=0)
                             for(int e=0;e<getEmax()/dE;e++){ (*SumOfReacTraj)[e]+=ReactMotion->TransProb(e*dE);}*/
                            
                        }
                    }
                }
            }
            return count;
        }
        
        double msOscillatorsAssembly::compDOSd_5()
        {
            double Eu=getEmax();double tp;
            for(int i=0;i<nDoF;i++) Occups[i]=0;
            int n1,n2,n3,n4,n5;
            n1=AnhOscillators[0]->nvmax(Eu,Occups,*getUnits());
            int count=0;
            
            for(int i=0; i<n1 ; i++)
            {
                Occups[ AnhOscillators[0]->getIndice() ]=i;
                for(int dof=1;dof<nDoF;dof++){Occups[dof]=0;}
                Eu = getEmax() - ( energyAO(Occups)-ZPE() );  n2 = AnhOscillators[1]->nvmax(Eu,Occups,*getUnits());
                
                for( int j=0;j<n2; j++)
                {
                    Occups[ AnhOscillators[1]->getIndice() ]=j;
                    for(int dof=2;dof<nDoF;dof++){Occups[dof]=0;}
                    Eu = getEmax() - ( energyAO(Occups)-ZPE() ); n3 = AnhOscillators[2]->nvmax(Eu,Occups,*getUnits());
                    
                    for(int k=0;k<n3; k++)
                    {
                        Occups[ AnhOscillators[2]->getIndice() ]=k;
                        for(int dof=3;dof<nDoF;dof++){Occups[dof]=0;}
                        Eu = getEmax() - ( energyAO(Occups)-ZPE() ); n4 = AnhOscillators[3]->nvmax(Eu,Occups,*getUnits());
                        
                        for(int l=0;l<n4; l++)
                        {
                            Occups[ AnhOscillators[3]->getIndice() ]=l;
                            for(int dof=4;dof<nDoF;dof++){Occups[dof]=0;}
                            Eu = getEmax() - ( energyAO(Occups)-ZPE() ); n5 = AnhOscillators[4]->nvmax(Eu,Occups,*getUnits());
                            
                            for(int m=0;m<n5; m++)
                            {
                                Occups[ AnhOscillators[4]->getIndice() ]=m;
                                tp=energyAO(Occups)-ZPE();
                                
                                if( (tp<=getEmax()-dE) && (isStateBounded(Occups)) )
                                {
                                    if(count<nEigenLimit){EigenValues[count]=tp;g[count]=1;}
                                    (*DensOfStates_AO)[ int(tp/dE+0.5) ] += 1/dE;
                                    count++;
                                }
                                /*    if(ReactMotion!=0)
                                 for(int e=0;e<getEmax()/dE;e++) (*SumOfReacTraj)[e]+=ReactMotion->TransProb(e*dE);
                                 */
                            }
                        }
                    }
                }
            }
            return count;
        }
        
        
        double msOscillatorsAssembly::compDOSd_6()
        {
            double Eu=getEmax();double tp;
            for(int i=0;i<nDoF;i++) Occups[i]=0;
            int n1,n2,n3,n4,n5,n6;
            n1=AnhOscillators[0]->nvmax(Eu,Occups,*getUnits());
            int count=0;
            
            for(int i=0; i<n1 ; i++)
            {
                Occups[ AnhOscillators[0]->getIndice() ]=i; for(int dof=1;dof<nDoF;dof++){Occups[dof]=0;}
                Eu = getEmax() - ( energyAO(Occups)-ZPE() );  n2 = AnhOscillators[1]->nvmax(Eu,Occups,*getUnits());
                
                for( int j=0;j<n2; j++)
                {
                    Occups[ AnhOscillators[1]->getIndice() ]=j; for(int dof=2;dof<nDoF;dof++){Occups[dof]=0;}
                    Eu = getEmax() - ( energyAO(Occups)-ZPE() ); n3 = AnhOscillators[2]->nvmax(Eu,Occups,*getUnits());
                    
                    for(int k=0;k<n3; k++)
                    {
                        Occups[ AnhOscillators[2]->getIndice() ]=k; for(int dof=3;dof<nDoF;dof++){Occups[dof]=0;}
                        Eu = getEmax() - ( energyAO(Occups)-ZPE() ); n4 = AnhOscillators[3]->nvmax(Eu,Occups,*getUnits());
                        
                        for(int l=0;l<n4; l++)
                        {
                            Occups[ AnhOscillators[3]->getIndice() ]=l; for(int dof=4;dof<nDoF;dof++){Occups[dof]=0;}
                            Eu = getEmax() - ( energyAO(Occups)-ZPE() ); n5 = AnhOscillators[4]->nvmax(Eu,Occups,*getUnits());
                            
                            for(int m=0;m<n5; m++)
                            {
                                Occups[ AnhOscillators[4]->getIndice() ]=m; for(int dof=5;dof<nDoF;dof++){Occups[dof]=0;}
                                Eu = getEmax() - ( energyAO(Occups)-ZPE() ); n6 = AnhOscillators[5]->nvmax(Eu,Occups,*getUnits());
                                
                                for(int n=0;n<n6; n++)
                                {
                                    Occups[ AnhOscillators[5]->getIndice() ]=n;
                                    
                                    tp=energyAO(Occups)-ZPE();
                                    if( (tp<=getEmax()-dE) && (isStateBounded(Occups)) )
                                    {
                                        if(count<nEigenLimit){EigenValues[count]=tp;g[count]=1;}
                                        (*DensOfStates_AO)[ int(tp/dE+0.5) ] += 1/dE;
                                        count++;
                                    }
                                    /*  if(ReactMotion!=0)
                                     for(int e=0;e<getEmax()/dE;e++){ (*SumOfReacTraj)[e]+=ReactMotion->TransProb(e*dE);}*/
                                    
                                }
                            }
                        }
                    }
                }
            }
            return count;
        }
        
        //-------------------------------------------------------------------------------------------------
        //------------------------------------------------------------------------------------------------- 
        
        ostream& msOscillatorsAssembly::print(ostream& out) const  {
            
            msMotion::print(out);
            output::printHeader(out,"Oscillators assembly");
            
            out<<"List of harmonic oscillators:\n";
            int i=0;
            for( const_itHarmOscillator it=HarmOscillators.begin();it!=HarmOscillators.end();++it,++i)
                out<<i<<": Frequency: "<<(*it)->getFrequency(*getUnits())<<" "<<getUnits()->getEnergyStr()<<endl;
            
            out<<"\nList of anharmonic oscillators:\n";
            for( const_itAnhOscillator it=AnhOscillators.begin();it!=AnhOscillators.end();++it,++i){
                out<<i<<" Indice="<<(*it)->Indice<<";";(*it)->abstract(out);out<<endl;
            }
            
            out<<"Current configuration for anharmonic oscillators: ";
            
            for(size_t i=0; i<Occups.size();i++ ) out<<Occups[i]<<" ";
            out<<"; energy="<<energyAO(Occups)<<" "<<getUnits()->getEnergyStr()<<endl;
            out<<"ZPE="<<ZPE()<<endl; 
            return out;
        }
        
    }
    
}
