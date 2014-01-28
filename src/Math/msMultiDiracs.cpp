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


#include "msMultiDiracs.h"


namespace impact
{
    
    namespace math {
        
        
        bool msMultiDiracs::isMultiDiracRegisteredInPython = 0;
        
        msRegistrar msMultiDiracs::Registrar("msMultiDiracs", msMultiDiracs::createInstance);
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msMultiDiracs::registryInPython(){
            
#if USE_PYTHON
            if( ! msMultiDiracs::isMultiDiracRegisteredInPython ) {
                
                using namespace boost::python;
                msTreeMapper::registryInPython();
                
                class_< msMultiDiracs, bases<msTreeMapper>, boost::shared_ptr<msMultiDiracs> >(
                                                                                                     "msMultiDiracs", "step function" , init<>() )
                .def( "New", &msMultiDiracs::New , "Create a new object.")
                .staticmethod("New")
                .def("addDirac", &msMultiDiracs::addDirac,
                     "Add a dirac function. arg2: vector of values; arg3: id to the diracs function appended.")
                .def("setSampler",&msMultiDiracs::setSampler,
                     "Set the explicit sampler. arg2: sampler")
                .def("getDiracsFunctions",&msMultiDiracs::getDiracsFunctions,
                     "return the diracs functions in a vector.")
                .def("getSampler",&msMultiDiracs::getSampler,
                     "return the sampler.")
                .def("getCoordinates",&msMultiDiracs::getCoordinates,
                     "return the coordinates.")
                .def("getValue",&msMultiDiracs::getValue,
                     "return the value of a dirac function for current index of the sample. arg2: index of the dirac function")
		 .def("getValueById",&msMultiDiracs::getValueById,
                     "return the value of a dirac function for current index of the sample. arg2: id of the dirac function")
                .def("getValues",&msMultiDiracs::getValues,
                     "return the values of all the Diracs function for the current sample of the sampler in a vector");
                msMultiDiracs::isMultiDiracRegisteredInPython=1;
            }
#endif
        };
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msMultiDiracs::setSampler(boost::shared_ptr<msSamplerExplicit> sampler) {
            
            LOGGER_ENTER_FUNCTION("boost::shared_ptr<msTreeMapper> msMultiDiracs::setSampler(boost::shared_ptr<msSamplerExplicit> sampler)",
                                  getFullId());
            
            msTreeMapper::update(Sampler,sampler);
            
            if(!Sampler->hasParent())  Sampler->setAffiliation(mySharedPtr());
            
            boost::shared_ptr<msGeneralizedCoordinates> coors = msGeneralizedCoordinates::New();
            
            for(size_t i = 0;i<Sampler->noOfDim(); i++) {
                
                boost::shared_ptr<msScalarVariable> var = msScalarVariable::New();
                stringstream name;
                name<<"q"<<i;
                var->setId(name.str());
                coors->addVar(var);
            }
            coors->setAffiliation(mySharedPtr());
            Sampler->setCoordinates(coors);
            Sampler->setCoordinatesRange();
            msTreeMapper::update(Coordinates,coors);
            
            LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msMultiDiracs::setSampler(boost::shared_ptr<msSamplerExplicit> sampler)");
            
            return mySharedPtr();
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msMultiDiracs::addDirac(vector<double> values, string name) {
            
            LOGGER_ENTER_FUNCTION("boost::shared_ptr<msTreeMapper> msMultiDiracs::addDirac(vector<double> values, string name)",
                                  getFullId());
            
            if(!Sampler.getSharedPtr()) {
                
                msException e("You have to set the sampler before adding dirac function",
                          "boost::shared_ptr<msTreeMapper> msMultiDiracs::addDirac(vector<double> values, string name)",
                          getFullId());
                IMPACT_THROW_EXCEPTION(e);
            }
            if(values.size()!=Sampler->noOfSamples()){
                
                stringstream out;
                out<<"The size of the vector of values provided ("<<values.size()
                <<") does not match the number of sample defined in the sampler"
                <<" ("<<Sampler->noOfSamples()<<").";
                
                msException e(out.str(),
                          "boost::shared_ptr<msTreeMapper> msMultiDiracs::addDirac(vector<double> values, string name)",
                          getFullId());
                IMPACT_THROW_EXCEPTION(e);
            }

            boost::shared_ptr<msDiracs> dirac=msDiracs::New();
            
            dirac->setId(name);
            dirac->setAffiliation(mySharedPtr());
            dirac->setCoordinates(Coordinates.getSharedPtr());
            dirac->setDiracSampler(Sampler.getSharedPtr());
            dirac->setValues(values);
            
            msTreeMapper::addElementToChildren<msDiracs>(Diracs,dirac);
            
            Sampler->begin();
            
            LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msMultiDiracs::addDirac(vector<double> values, string name)");
            return mySharedPtr();
        }
    }
}

