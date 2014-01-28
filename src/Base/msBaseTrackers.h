/*
 <one line to give the program's name and a brief idea of what it does.>
 Copyright (C) <year>  <name of author>
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */

/*!
 * \file c_scalfunction.h
 * \brief Defines the class msScalarFunction
 * \author G.Reinisch
 * \date 2011
 */
#ifndef msBASETRACKERS_H
#define msBASETRACKERS_H

#include <msTracker.h>

namespace impact
{
    class msTrackerScalarFunction : public msTracker
    {
        
    private:
        
        //!\name from msRegister
        //@{
        static msRegistrar Registrar;
        static bool  isTrackerScalarFunctionRegisteredInPython;
        static msRegister* createInstance() { return new msTrackerScalarFunction; }
        //@}
        
    protected:
        
        void registryInPython();
        
        void update() {
            
            LOGGER_ENTER_FUNCTION_DBG("void msTrackerScalarFunction::update()",getFullId());
            msTracker::update();
            
            ScalarFunction = boost::static_pointer_cast<msScalarFunction>(getObject());
            
            LOGGER_EXIT_FUNCTION2("void msTrackerScalarFunction::update()");
        }
        
    public:
        
	    msTrackerScalarFunction(): msTracker() {
            
            constructVar("msTrackerScalarFunction","TrackerScalarFunction","TrackerScalarFunction");
        }
        
        void initialize() {
            
            LOGGER_ENTER_FUNCTION_DBG("void msTrackerScalarFunction::initialize()","");
            msTracker::initialize();
            setPropertyName("scalar function value");
            LOGGER_EXIT_FUNCTION2("void msTrackerScalarFunction::initialize()");
        }
        
        static boost::shared_ptr<msTrackerScalarFunction> New(){
            
            LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msTrackerScalarFunction> msTrackerScalarFunction::New()","");
            boost::shared_ptr<msTrackerScalarFunction> T( new msTrackerScalarFunction );
            T->initialize();
            T->update();
            LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msTrackerScalarFunction> msTrackerScalarFunction::New()");
            return T;
        }
        
        /*! \brief set the msScalarFunction obect tracked.
         *
         * \param object msScalarFunction object
         */
        boost::shared_ptr<msTreeMapper> setTrackedObject(boost::shared_ptr<msScalarFunction> object) {
            
            msTracker::setTrackedObject(object);
            ScalarFunction = object;
            getProperty(0)->setYUnit(ScalarFunction->getYUnit().getStr());
            return mySharedPtr();
        };
        
        /*! \brief append the mass fraction of the constituent to the tracker.
         *
         * \param timeIndicator time indicator
         */
        boost::shared_ptr<msTreeMapper> addValue(double timeIndicator) {
            
            if( !ScalarFunction ){
                
                msException e("The scalar function object has not been set",
                          "boost::shared_ptr<msTreeMapper> msTrackerScalarFunction::addValue(double timeIndicator)" ,
                          getFullId());
                IMPACT_THROW_EXCEPTION(e);
            }
            msTracker::addValue( timeIndicator, ScalarFunction->evaluate() );
            return mySharedPtr();
        }
        
        std::ostream& print(std::ostream& out) const ;
        
    private:
        
        boost::shared_ptr<msScalarFunction> ScalarFunction;
        
        
    };
    
}
#endif 
