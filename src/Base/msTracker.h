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


#ifndef MSTRACKER
#define MSTRACKER


#include <msVectorFit1d.h>

namespace impact{
    
    /*! \class msTracker
     *
     * \brief Virtual base class for object tracking the property of an object
     * with respect to (virtual) time.
     *
     * The msTracker class allows to store the property (double) of an msTreeMapper
     * derived object over a virtual time in a msVectorFit1d scalar function.
     *
     * Derived classes need to implement the msTracker::addValue method, and may
     * overide the msTracker::setTrackedObject method.
     */
    class msTracker : public msTreeMapper
    {
        
    private:
        
        //!\name from msRegister
        //@{
        static msRegistrar Registrar;
        static bool  isTrackerRegisteredInPython;
        static msRegister* createInstance() { return new msTracker; }
        //@}
        
    protected:
        
        void registryInPython();
        
        void update() {
            
            LOGGER_ENTER_FUNCTION_DBG("void msTracker::update()",getFullId());
            msTreeMapper::update();
            LOGGER_EXIT_FUNCTION2("void msTracker::update()");
        }
        
    public:
        
        msTracker(): msTreeMapper() {
            
            constructVar("msTracker","Tracker","Tracker");
        }
        
        void initialize() {
            
            LOGGER_ENTER_FUNCTION_DBG("void msTracker::initialize()","");
            msTreeMapper::initialize();
            declareAttribute(PropertyName,"PropertyName");
            declareChild<msTreeMapper>(Object,msTreeMapper::New(),"Object");
            declareChild<msScalarVariable>(TimeVariable,msScalarVariable::New(),"TimeVariable");
            declareChildren<msVectorFit1d>(Properties,"Properties");
            LOGGER_EXIT_FUNCTION2("void msTracker::initialize()");
        }
        
        static boost::shared_ptr<msTracker> New(){
            
            LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msTracker> msTracker::New()","");
            boost::shared_ptr<msTracker> T( new msTracker );
            T->initialize();
            T->update();
            LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msTracker> msTracker::New()");
            return T;
        }
        
        /*! \brief set the tracked object
         *
         * May be overiden if the some manipulation of the tracked object is needed.
         * \param object tracked object
         */
        virtual boost::shared_ptr<msTreeMapper> setTrackedObject(boost::shared_ptr<msTreeMapper> object) {
            
            msTreeMapper::update(Object,object);
            return mySharedPtr();
        };
        
        /*! \brief add the value of the tracked property to the vector list
         *
         * Derived class needs to add the property targeted of the tracked object in
         * the member Values.
         * \param timeIndicator an indicator of time
         */
        virtual boost::shared_ptr<msTreeMapper> addValues(double timeIndicator) {
            
            msError e("This method need to be overiden by derived class",
                      "boost::shared_ptr<msTreeMapper> msTracker::addValues(double timeIndicator)",
                      getFullId());
            
            BOOST_THROW_EXCEPTION(e);
            
            return mySharedPtr();
        }
        
        virtual boost::shared_ptr<msTreeMapper> clear() {
            
            TimeVariable->setMin(0);
            TimeVariable->setMax(0);
            TimeVariable->setDq(0);
            
            msChildren<msVectorFit1d>::iterator it= Properties.begin();
            for(;it!=Properties.end();++it) (*it)->clear();
            
            return mySharedPtr();
        }
        
        
        boost::shared_ptr<msTreeMapper> newProperty(string name) {
            
            boost::shared_ptr<msVectorFit1d> fct = msVectorFit1d::New();
            fct->setVariable(TimeVariable.getSharedPtr(),"");
            fct->setId(name);
            return mySharedPtr();
        }
        
        std::string getPropertyName() const { return PropertyName;
        }
        
        boost::shared_ptr<msTreeMapper> getObject() const { return Object.getSharedPtr();
        }

        boost::shared_ptr<msScalarVariable> getVariable() const { return TimeVariable.getSharedPtr();
        }
        
        boost::shared_ptr<msVectorFit1d> getProperty(size_t i) const {
            
            return Properties[i].getSharedPtr();
        }
        
        std::ostream& print(std::ostream& out) const ;
        
        
    protected:
        
        boost::shared_ptr<msTreeMapper> addValue(double timeIndicator,double value) {
            
            if(Properties.size()>1) {
                
                msError e("This method should be called only if one property is defined, and not more.",
                          "boost::shared_ptr<msTreeMapper> msTracker::addValue(double timeIndicator,double value)",getFullId());
                BOOST_THROW_EXCEPTION(e);
            }
            updateTimeVariable(timeIndicator);
            Properties[0]->push_back(value);
            
            return mySharedPtr();
        }
 
        boost::shared_ptr<msTreeMapper> addValues(double timeIndicator,vector<double> values) {
            
            if(Properties.size()>values.size()) {
                
                msError e("The argument 'value' does not contains enough elements.",
                          "boost::shared_ptr<msTreeMapper> msTracker::addValues(double timeIndicator,vector<double> value)",getFullId());
                BOOST_THROW_EXCEPTION(e);
            }
            updateTimeVariable(timeIndicator);
            msChildren<msVectorFit1d>::iterator it = Properties.begin();
            for( size_t i=0;it!=Properties.end();++it,++i) (*it)->push_back(values[i]);
            
            return mySharedPtr();
        }
        
        boost::shared_ptr<msTreeMapper> updateTimeVariable(double timeIndicator) {
            
            if(TimeVariable->getMax()==0){
                TimeVariable->setMin(timeIndicator);
                TimeVariable->setMax( timeIndicator );
            }
            if(TimeVariable->getDq()==0){
                TimeVariable->setDq( timeIndicator - TimeVariable->getMin());
            }
            if(TimeVariable->getDq()>0){
                TimeVariable->setMax( TimeVariable->getMax() + TimeVariable->getDq() );
            }
            return mySharedPtr();
        }
        
        boost::shared_ptr<msTreeMapper> setPropertyName(std::string name){
            
            PropertyName=name;
            return mySharedPtr();
        }
        
        
    private:
        
        //!\name attributs and children
        //@{
        
        std::string PropertyName;
        
        msChild<msTreeMapper> Object;
        msChild<msScalarVariable> TimeVariable;
        msChildren<msVectorFit1d> Properties;
        //@}
        
    };
    
    
    
}

#endif // PA0DCSTSTP_H
