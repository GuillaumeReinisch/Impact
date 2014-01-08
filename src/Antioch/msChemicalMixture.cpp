/*
 <one line to give the library's name and an idea of what it does.>
 Copyright (C) 2012  Guillaume <email>
 
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


#include "msChemicalMixture.h"

namespace impact {
    
    namespace antioch {
        
      
        bool msChemicalMixture::isPhaseRegisteredInPython = 0;
        msRegistrar msChemicalMixture::Registrar("msChemicalMixture", msChemicalMixture::createInstance);
        
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msChemicalMixture::registryInPython() {
            
#if USE_PYTHON
            
            msPhysicalInterface::registryInPython();
            
            if( ! msChemicalMixture::isPhaseRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msChemicalMixture,bases<msPhysicalInterface>,boost::shared_ptr<msChemicalMixture> >
                ("msChemicalMixture",
                 "Object describing a Phase",no_init
                 )
                .def( "New", &msChemicalMixture::New ,
                     "Create a new object.")
                .staticmethod("New")
                .def("noOfConstituents", &msChemicalMixture::noOfConstituents,
                     "return the number of constituents")
		.def("getConstituentFromId", &msChemicalMixture::getConstituentFromId,
                     "return a constituent of particular id. arg2: id.")	
               /* .def("getSpeciesNames",&msChemicalMixture::getSpeciesNames,
                     "return the specie's names in a list")*/
                .def("getMassFractions",&msChemicalMixture::getMassFractions,
                     "return the mass fraction of the constituent in a list")
		.def("getMassFractionByName",&msChemicalMixture::getMassFractionByName,
		     "return the mass fractions of a specie. arg2: specie's name")
                .def("getMoleFractions",&msChemicalMixture::getMoleFractions,
                     "return the mole fraction of the constituent in a list")
		.def("getMoleFractionByName",&msChemicalMixture::getMoleFractionByName,
		     "return the mole fractions of a specie. arg2: specie's name")
                ;
                msChemicalMixture::isPhaseRegisteredInPython = 1;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        std::ostream& msChemicalMixture::print(std::ostream& out) const {
            
            msPhysicalInterface::print(out);
            output::printHeader(out,"Phase");
            
            return out<<endl;
        }
        
    }
}
