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


#include "msReaction.h"

namespace impact {
    
    namespace antioch {
        
        
        bool msReaction::isReactionRegisteredInPython = 0;
        msRegistrar msReaction::Registrar("msReaction", msReaction::createInstance);
        
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msReaction::registryInPython() {
            
#if USE_PYTHON
            
            msPhysicalInterface::registryInPython();
            
            if( ! msReaction::isReactionRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msReaction,bases<msPhysicalInterface>,boost::shared_ptr<msReaction> >
                ("msReaction",
                 "Object describing a reaction",no_init
                 )
                .def( "New", &msReaction::New ,
                     "Create a new object.")
                .staticmethod("New");/*
                .def("getCoefficients",&msReaction::getCoefficients,
                     "return the coefficients value")
                .def("getCoefficientsName",&msReaction::getCoefficientsName,
                     "return the name of the coefficients")
                .def("getCoefficient",&msReaction::getCoefficient,
                     "return the value of a coefficient. arg2: name of the coefficient")
                .def("setCoefficient",&msReaction::setCoefficient,
                     "set the value of a coefficient. arg2: name of the coefficient");*/
                
                msReaction::isReactionRegisteredInPython = 1;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        /*
        boost::shared_ptr<msTreeMapper> msReaction::addCoefficient(std::string name, double& value, double coefConversion,msUnit::uTypeVar u){
            
            Coefficients[name] = value * coefConversion;
            ptrOnCoefficientsInCalculator.push_back(&value);
            msUnit unit;
            unit.set( u, *(getUnits()) );
            declarePhysicalVariable( unit , &(Coefficients[name]) );
            return mySharedPtr();
        }
        */
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        /*
        std::vector<double> msReaction::getCoefficients() const {
            
            std::vector<double> coefs;
            
            for(map<std::string,double>::const_iterator it=Coefficients.begin();
                it!=Coefficients.end();
                ++it)
                coefs.push_back((*it).second);
            
            return coefs;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        std::vector<string> msReaction::getCoefficientsName() const {
            
            std::vector<string> names;
            
            for(map<std::string,double>::const_iterator it=Coefficients.begin();
                it!=Coefficients.end();
                ++it)
                names.push_back((*it).first);
            
            return names;
        }
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        
        double msReaction::getCoefficient(std::string id) const {
            
            if( Coefficients.find(id) == Coefficients.end()){
                msError e("The coefficient of name "+id+" does not exist",
                          "double msReaction::getCoefficient(std::string id) const ",
                          getFullId());
                BOOST_THROW_EXCEPTION(e);
            }
            map<std::string,double>::const_iterator it = Coefficients.find(id);
            return (*it).second;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msReaction::setCoefficient(std::string id,double v) {
            
            if( Coefficients.find(id) == Coefficients.end()){
                msError e("The coefficient of name "+id+" does not exist",
                          "double msReaction::getCoefficient(std::string id) const ",
                          getFullId());
                BOOST_THROW_EXCEPTION(e);
            }
            Coefficients[id] = v;
            return mySharedPtr();
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        std::ostream& msReaction::print(std::ostream& out) const {
            
            msPhysicalInterface::print(out);
            output::printHeader(out,"Reaction");
            
            out<<"Coefficients defined:"<<endl;
            int i=0;
            for(map<std::string,double>::const_iterator it=Coefficients.begin();
                it!=Coefficients.end();
                ++it,++i)
                out<<(*it).first<<": "<<(*it).second<<" (cantera: "
                <<*(ptrOnCoefficientsInCalculator[i])<<");";
            
            return out<<endl;
        }
        */
    }
}
