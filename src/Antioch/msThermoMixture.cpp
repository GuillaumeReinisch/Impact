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


#include "msThermoMixture.h"

namespace impact {
    
    namespace antioch {
        
      
        bool msThermoMixture::isThermoMixtureRegisteredInPython = 0;
        msRegistrar msThermoMixture::Registrar("msThermoMixture", msThermoMixture::createInstance);
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msThermoMixture::registryInPython() {
            
#if USE_PYTHON
            
            msPhysicalInterface::registryInPython();
            
            if( ! msThermoMixture::isThermoMixtureRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msThermoMixture,bases<msChemicalMixture>,boost::shared_ptr<msThermoMixture> >
                ("msThermoMixture",
                 "Object describing a chemical mixture with thermo description",no_init
                 )
                .def( "New", &msThermoMixture::New ,
                     "Create a new object.")
                .staticmethod("New")
		.def("parseFromCanteraMechanismXml",&msThermoMixture::parseFromCanteraMechanismXml,
		    "fill the mixture with the species contains in a xml mechanism file of Cantera. arg2: path of the file")
		.def("parseFromCeaAscii",&msThermoMixture::parseFromCeaAscii,
		    "fill the mixture with the species contains in a ASCII CEA mechanism file. arg2: path of the file")
		.def("Cps",&msThermoMixture::Cps,
		    "return the calorific capacities (cts P) for all species in a list")
		.def("Cvs",&msThermoMixture::Cvs,
		    "return the calorific capacities (cts V) for all species in a list")
		.def("Ss",&msThermoMixture::Ss,
		    "return the entropies for all species  in a list")
		.def("Hs",&msThermoMixture::Hs,
		    "return the enthalpies for all species in a list")
		.def("CpTot",&msThermoMixture::CpTot,
		    "return the calorific capacity (cts P) of the gas")
		.def("CvTot",&msThermoMixture::CvTot,
		    "return the calorific capacity (cts V) of the gas")
		.def("STot",&msThermoMixture::STot,
		    "return the entropy of the gas")
		.def("HTot",&msThermoMixture::HTot,
		    "return the enthalpy of the gas")	
		.def("Cp",&msThermoMixture::Cp,
		    "return the calorific capacity (cts P) of a specie. arg2: index of the specie")
		.def("Cv",&msThermoMixture::Cv,
		    "return the calorific capacity (cts V) of a specie. arg2: index of the specie")		
		.def("S",&msThermoMixture::S,
		    "return the entropy of a specie. arg2: index of the specie")
		.def("H",&msThermoMixture::H,
		    "return the enthalpy of a specie. arg2: index of the specie")
		;
                
                msThermoMixture::isThermoMixtureRegisteredInPython = 1;
                }
#endif
	}
	
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msThermoMixture::parseFromCanteraMechanismXml(string path) {
	  
	    LOGGER_ENTER_FUNCTION("boost::shared_ptr<msTreeMapper> msThermoMixture::parseFromCanteraMechanismXml(string path) ",getFullId()); 
	    using namespace boost::property_tree::detail;
	    
	    ifstream theFile(path.c_str());
	   
	    if( ! theFile.is_open() ) {
	     
	        msException e("can not open the file "+path,
		  	  "boost::shared_ptr<msTreeMapper> msThermoMixture::parseFromCanteraMechanismXml(string path)",
			  getFullId());
	        IMPACT_THROW_EXCEPTION(e);
	    }
	    
	    LOGGER_WRITE(msLogger::DEBUG,"file '"+path+ "' open." );
	   
            vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
            buffer.push_back('\0');

	    rapidxml::xml_document<> Document;
            Document.parse<0>(&buffer[0]);
            rapidxml::xml_node<>* RootNode = Document.first_node();
	    
	    if( RootNode == 0 ) {
	      
	        msException e("can not get the root node of the xml file",
		          "boost::shared_ptr<msTreeMapper> msThermoMixture::parseFromCanteraMechanismXml(string path)",
			  getFullId());
		IMPACT_THROW_EXCEPTION(e);
	    }
	    rapidxml::xml_node<>* speciesNode = RootNode->first_node("speciesData");
	    
	    if( speciesNode == 0 ) {
	      
	        msException e("can not get the 'speciesData' node in the root node",
		          "boost::shared_ptr<msTreeMapper> msThermoMixture::parseFromCanteraMechanismXml(string path)",
			  getFullId());
		IMPACT_THROW_EXCEPTION(e);
	    }
	    
	    for(rapidxml::xml_node<>* xmlSpecie = speciesNode->first_node();
		xmlSpecie != xmlSpecie->last_node();
	        xmlSpecie  = xmlSpecie->next_sibling()) {
	      
	        rapidxml::xml_attribute<>* attName = xmlSpecie->first_attribute("name");
	    
	        if( attName == 0 ) {
	      
	            msException e("can not get the 'name' attribute of the specie",
		              "boost::shared_ptr<msTreeMapper> msThermoMixture::parseFromCanteraMechanismXml(string path)",
			       getFullId());
		    IMPACT_THROW_EXCEPTION(e);
	        }
	        string name = attName->value();
		
		boost::shared_ptr<msMolecule> species = msMolecule::New();
		species->setId(name);
		
	        LOGGER_WRITE(msLogger::DEBUG,"Parse specie "+name);
		
		LOGGER_WRITE(msLogger::DEBUG,"Parse atomic composition");
	        rapidxml::xml_node<>* atomArray = xmlSpecie->first_node("atomArray");
		
		if( atomArray == 0 ) {
	      
	            msException e("can not get the 'atomArray' node in the specie node",
		              "boost::shared_ptr<msTreeMapper> msThermoMixture::parseFromCanteraMechanismXml(string path)",
			      getFullId());
		    IMPACT_THROW_EXCEPTION(e);
	        }
	        
	        string composition = atomArray->value();
		LOGGER_WRITE(msLogger::DEBUG,"Composition map: "+composition);
		
		vector<string> elements;
		fillVecFromString<string>(composition,elements," ");
		
		for( size_t i = 0 ; i<elements.size();i++) {
		  
		    vector<string> Elem_Coef;
		    fillVecFromString<string>(elements[i],Elem_Coef,":");
		    int n = 0;
		    try{
		        n = boost::lexical_cast<int>(Elem_Coef[1]);
		    }
		    catch(...){
		        msException e("can not convert the string '"+Elem_Coef[1]+"' in an integer for the compisition map",
		                  "boost::shared_ptr<msTreeMapper> msThermoMixture::parseFromCanteraMechanismXml(string path)",
			           getFullId());
		        IMPACT_THROW_EXCEPTION(e);
		    }
		    for( size_t j =0; j<n; j++){
		      
		        species->addElement(atomism::NewAtom(Elem_Coef[0]));
		    }
		}
		
		LOGGER_WRITE(msLogger::DEBUG,"Parse thermo ");
		boost::shared_ptr<msMotionCEA> td = msMotionCEA::New();
		
		rapidxml::xml_node<>* thermo = xmlSpecie->first_node("thermo");
		
		if( thermo == 0 ) {
	      
	            msException e("can not get the 'thermo' node in the specie node",
		              "boost::shared_ptr<msTreeMapper> msThermoMixture::parseFromCanteraMechanismXml(string path)",
			      getFullId());
		    IMPACT_THROW_EXCEPTION(e);
	        }
	        
		for(rapidxml::xml_node<>* xmlNasa = thermo->first_node();
		xmlNasa != thermo->last_node();
	        xmlNasa  = thermo->next_sibling()) {
		  
		    rapidxml::xml_node<>* floatArray = xmlSpecie->first_node("floatArray");
		    
		    if( floatArray == 0 ) {
	      
	                msException e("can not get the 'floatArray' node in the Nasa node",
		                  "boost::shared_ptr<msTreeMapper> msThermoMixture::parseFromCanteraMechanismXml(string path)",
			          getFullId());
		        IMPACT_THROW_EXCEPTION(e);
	            }
		    vector<double> coefs;
		    fillVecFromString<double>(string(floatArray->value()),coefs,",");
		    coefs.insert(coefs.insert(coefs.begin(),0),0);
		    coefs.insert(coefs.begin()+7,0);
		    td->addCoeffsFit(coefs);
	        }
	        species->setMotion(td);
	    }
            LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msThermoMixture::parseFromCanteraMechanismXml(string path)");
	    return mySharedPtr();
	  
	}

        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msThermoMixture::parseFromCeaAscii(string path) {
	   
	    IMPACT_LOGIN();
	    
	    ifstream in(path.c_str());
	    
	    Antioch::skip_comment_lines(in, '#');
  
	    std::string name;
	    unsigned int n_int;
	    std::vector<double> coeffs;
	    double h_form, val;
	    boost::shared_ptr<msMotion::msMotionParams> param = msMotion::msMotionParams::New();
	    boost::shared_ptr<msTreeMapper> elementsContainer = msTreeMapper::New();
	    
	    std::vector<boost::shared_ptr<atomism::msAtom> > availAtoms;
	    
	    addChild(param,"Param_CEA");
	    addChild(elementsContainer,"ElementsContainer");
	    
	    msChildren<msElement>::iterator it;
	    
	    while (in.good()) {
	      
	        in >> name;   // Species Name
	        in >> n_int;  // Number of T intervals: [200-1000], [1000-6000], ([6000-20000])
	        in >> h_form; // Formation Enthalpy @ 298.15 K
	        
	        if(!in.good()) break;
		
                LOGGER_WRITE(msLogger::DEBUG,"Parse specie "+name);
		
		boost::shared_ptr<msMolecule> species = msMolecule::New();
		species->setId(name);
		
		LOGGER_WRITE(msLogger::DEBUG,"Create atoms "+name);
		vector<boost::shared_ptr<atomism::msAtom> > Atoms;
		size_t s0 = availAtoms.size();
		
		IMPACT_TRY( [&]() { atomism::getListAtomsFromName(name, availAtoms,Atoms,
		                                                  elementsContainer,getUnits());
		});
		
		msChildren<msElement>::iterator it;
		
		for_each( Atoms.begin(), Atoms.end() , 
		         [&](boost::shared_ptr<atomism::msAtom> atom){ species->addElement(atom);
		});
		
		LOGGER_WRITE(msLogger::DEBUG,"Construct thermodynamic data");
		
		boost::shared_ptr<msMotionCEA> td = msMotionCEA::New();		
		td->setParameters(param);
		
	        for (unsigned int interval=0; interval<n_int; interval++) {
		    coeffs.clear();
	            for (unsigned int n=0; n<10; n++) in >> val, coeffs.push_back(val);
	            td->addCoeffsFit(coeffs);
	        }
	        
	        td->setUnits(getUnits());
	        species->setMotion(td);
		species->setUnits(getUnits());
		
	        msChemicalMixture::addEntity(species);
            } // end while
            
            size_t i=0;
            for_each( availAtoms.begin(), availAtoms.end() , [&](boost::shared_ptr<atomism::msAtom> atom){  
		  
		    elementsContainer->addChild(atom,"Elements_"+output::getString(i));
		    i++;
		});
	    
            LOGGER_WRITE(msLogger::DEBUG,"create antioch calculator");	 
	    IMPACT_TRY( [&]() {  update();} );
	    IMPACT_LOGOUT();
	    return mySharedPtr();
	}		    
    }
}
