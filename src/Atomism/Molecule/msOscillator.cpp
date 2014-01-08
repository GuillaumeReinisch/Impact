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

#include "msOscillator.h"

namespace impact {
    
    namespace atomism {
        
        bool msOscillator::isOscillatorRegisteredInPython = 0;
        
        msRegistrar msOscillator::Registrar("msOscillator", msOscillator::createInstance);
        
        
        ostream& operator<<(ostream& out,const msOscillator& oscil) {
            
            oscil.print(out);
            return out;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msOscillator::registryInPython() {
            
#if USE_PYTHON
            msTreeMapper::registryInPython();
            
            if( ! msOscillator::isOscillatorRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_< msOscillator, bases<msTreeMapper>, boost::shared_ptr<msOscillator> >(
                                                                                             "msOscillator", "An oscillator: a coupled small amplitude quantum motion" ,
                                                                                             no_init )
                .def( "New", &msOscillator::New , "Create a new object. arg2: string of frequency's unit")
                .staticmethod("New")
                //    .def( "setUnit" , &msOscillator::setUnit, "Set the unit used for frequencies/energies. arg2: units" )
                .def(self_ns::str(self_ns::self));
                
                msOscillator::isOscillatorRegisteredInPython=1;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        ostream&  msOscillator::print(ostream& out) const {
            
            msTreeMapper::print(out);
            output::printHeader(out,"msOscillator");
            abstract(out);
            return out;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        bool msHarmonicOscillator::isHarmonicOscillatorRegisteredInPython=0;
        
        msRegistrar msHarmonicOscillator::Registrar("msHarmonicOscillator", msHarmonicOscillator::createInstance);
        
        void msHarmonicOscillator::registryInPython() {
            
#if USE_PYTHON
            msOscillator::registryInPython();
            
            if( ! msHarmonicOscillator::isHarmonicOscillatorRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_< msHarmonicOscillator, bases<msOscillator> , boost::shared_ptr<msHarmonicOscillator> >(
                                                                                                              "msHarmonicOscillator", "An harmonic oscillator, not coupled to other motions" ,
                                                                                                              no_init )
                .def( "New", &msHarmonicOscillator::New , "Create a new object. arg2: string of frequency's unit")
                .staticmethod("New")
                .def( "Q" , &msHarmonicOscillator::Q , "Partition function. arg2: Tmemperature, arg 3: R, same units as frequency")
                .def( "set" ,   (boost::shared_ptr<msTreeMapper> (msHarmonicOscillator::*)(double) ) &msHarmonicOscillator::set
                     , "Set the frequency of the oscillator. arg2: frequency" )
                .def( "set" , (boost::shared_ptr<msTreeMapper> (msHarmonicOscillator::*)(double,msUnitsManager& units) ) &msHarmonicOscillator::set
                     , "Set the frequency of the oscillator. arg2: frequency, arg3: units" )
                .def( "getFrequency" , &msHarmonicOscillator::getFrequency, "Return the frequency in particular unit. arg2: units" )
                .def(self_ns::str(self_ns::self));
                
                msHarmonicOscillator::isHarmonicOscillatorRegisteredInPython=1;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        bool msAnharmonicOscillatorP2::isAnharmonicOscillatorP2RegisteredInPython=0;
        
        msRegistrar msAnharmonicOscillatorP2::Registrar("msAnharmonicOscillatorP2", msAnharmonicOscillatorP2::createInstance);
        
        void msAnharmonicOscillatorP2::registryInPython() {
            
#if USE_PYTHON
            msOscillator::registryInPython();
            
            if( ! msAnharmonicOscillatorP2::isAnharmonicOscillatorP2RegisteredInPython ) {
                
                using namespace boost::python;
                
                class_< msAnharmonicOscillatorP2, bases<msOscillator> , boost::shared_ptr<msAnharmonicOscillatorP2> >(
                                                                                                                      "msAnharmonicOscillatorP2", "An anharmonic oscillator analyzed by VPT2" ,
                                                                                                                      no_init )
                .def( "New", &msAnharmonicOscillatorP2::New , "Create a new object. arg2: string of frequency's unit")
                .staticmethod("New")
                .def( "set" ,   (boost::shared_ptr<msTreeMapper> (msAnharmonicOscillatorP2::*)(double,string) )
                     &msAnharmonicOscillatorP2::set
                     , "Set the frequency of the oscillator. arg2: frequency, arg3: list of anharmonic constants at the second order" )
                .def( "set" , (boost::shared_ptr<msTreeMapper> (msAnharmonicOscillatorP2::*)(double,string,msUnitsManager& units) )
                     &msAnharmonicOscillatorP2::set
                     , "Set the frequency of the oscillator. arg2: frequency, arg3: list of anharmonic constants at the second order, arg4: units" )
                .def(self_ns::str(self_ns::self));
                
                msAnharmonicOscillatorP2::isAnharmonicOscillatorP2RegisteredInPython=1;
            }
#endif
        }
        
        
    }
}
