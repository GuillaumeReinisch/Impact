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
 * \file msParamsManager.h
 * \brief define the msParamsManager classe
 * \author G.Reinisch
 * \date 2013
 */

#ifndef msPARAMETERS_H
#define msPARAMETERS_H

#include <msTreeMapper.h>
#include <msUnitsManager.h>

namespace impact
{
    class msPhysicalInterface;
    
    using namespace std;
    
    /*! \class msParam
     *
     * \brief Define one parameter of a parameter manager
     *
     * \see msParamsManager
     */
    class msParam : public msTreeMapper
    {
        
        friend class msParamsManager;
        
    protected:
        /** @Name from msRegister
         *///@{
        static msRegister* createInstance(){ return new msParam(); }
        static msRegistrar Registrar;
        static bool isParamRegisteredInPython;
        //@}
        
        void initialize()
        {
            msTreeMapper::initialize();
            declareAttribute(Value,"Value");
            declareAttribute(Abstract,"Abstract");
            declareChild<msUnit>(Unit,msUnit::New(),"Unit");
	    }
	    
        void registryInPython();
	    
    public:
        
        msParam() : msTreeMapper() {constructVar("msParam","Parameter","parameter");};
       
	      
        static boost::shared_ptr<msParam> New()
        {boost::shared_ptr<msParam> T( new msParam );
            LOGGER_ENTER_FUNCTION_DBG("msParam::New()",T->getFullId());
            T->initialize();T->update();
            LOGGER_EXIT_FUNCTION();
            return T;
        }
        virtual boost::shared_ptr<msTreeMapper> clone() {
            boost::shared_ptr<msParam> clone = New();
            clone->msTreeMapper::operator=(*this);
            return clone;
        }
        
        void set( string n, string v, string a, const msUnit& u) {
            setId(n); Value=v; Abstract=a;
            (*Unit).set(u.getStr());
        }
        
        std::string getValue(){return Value;}
        std::string getUnit(){return Unit->getStr();}
        std::string getAbstract(){return Abstract;}
        
    private:
        
        std::string Value, Abstract;
        msChild<msUnit> Unit;
        
        
    };
    
    /*! \class msParamsManager
     *
     * \brief Manager of parameters used for numerical computations
     *
     * The msParamsManager class is a container for parameters used by the classes
     * deriving from msPhysicalInterface. Its objective is to simply, to organize,
     * and to make safer the declaration, the use, and the modification of parameter's
     * value. In particular, each time the parameters are changed, the updateParameters()
     * method of the related physical objects is called to allow updating some members.
     *
     * The key idea behind msParamsManager is to be derived for each msPhysicalInterface
     * derived class that use parameters. Hence, the parameters manager of one object
     * contains the parameters defined for it and for all its parent classes.
     *
     * \section derivingParam  Deriving from msParamsManager
     *
     * Lets look at this following example to illustrate the points, it is commented just after:
     * ~~~~~~~~~~~~~{.cpp}
     #include <msPhysicalInterface.h>
     
     class myClass : public aParentClassWithParams
     {
     class ParamsOfaClassParams : public ParamsOfaParentClassWithParams
     {
     static msRegistrar Registrar;
     static msRegister* createInstance() { return new ParamsOfaClassParams; }
	 
     public:
     
     void initialize() {
     
     ParamsOfaParentClassWithParams::initialize();
	 addParameter( "aParam" ,   "500" ,   "some comments" , msUnit("kJ/mol") );
	 addParameter( "baParam" ,  "1e-15" , "some comments" , msUnit("s") );
     }
     
     static boost::shared_ptr<ParamsOfaClassParams> New(){  // This is always the same, just change aClassParams by your class name
     
     boost::shared_ptr<ParamsOfaClassParams> T( new ParamsOfaClassParams );
	 T->initialize();
	 T->update();
	 return T;
     }
     
     ParamsOfaClassParams() : ParamsOfaParentClassWithParams() { }
     };
     
     static boost::shared_ptr<myClass> New( boost::shared_ptr<msUnitsManager> units ){
     
     boost::shared_ptr<myClass> T( new myClass );
     T->initialize();
     T->setParameters( ParamsOfaClassParams::New() ); // T will be registrated as a linked object to the aClassParams object
     T->setUnits(units);
     T->update();
     return T;
     }
     };
     * ~~~~~~~~~~~~~
     *
     * There are two important points:
     *       - the creation of the *aClassParams* shoudl be meaningfull: only the initialize()
     * method will change depending of the context (the *500* and *1e-15*  are default
     * values). Do not forget to call the parent's initialize() method. Parameters are
     * associated to units, to ensure safety when their value will be query.
     *       - *myClass* in the New constructor, you have to instantiate the *aClassParams*
     * parameters. See msPhysicalInterface for more information.
     *
     
     * \section usingParam  Using msParamsManager
     *
     * Just interact with msParamsManager using the *query* and set *methods*.
     * Do not forget to implement the method msPhysicalInterface::updateParameters() in the
     * associated object if some of its attributes depend on the parameters manager.
     */
    class msParamsManager : public msTreeMapper
    {
        friend class msPhysicalInterface;
    public:
        
        typedef std::map< std::string , msParam >::iterator ParamsMapIt;
        
        enum status_query { SUCESS , WRONG_TYPE , NOT_FOUND , ERROR};
        
        /** @Name from msRegister
         *///@{
        static msRegister* createInstance(){ return new msParamsManager(); }
        static msRegistrar Registrar;
        static bool isParamsManagerRegisteredInPython;
        void registryInPython();
        //@}
        
    protected:
        
        void update(){msTreeMapper::update();}
        
    public:
        
        //! @name construction
        //@{
        msParamsManager() : msTreeMapper() {constructVar("msParamsManager","Parameters","numerical parameters");
        };
        
        
        void initialize() {
            
            msTreeMapper::initialize();
            msTreeMapper::declareChildren<msParam>(Parameters,"Parameters");
        }
        
        static boost::shared_ptr<msParamsManager> New() {
            
            boost::shared_ptr<msParamsManager> T( new msParamsManager );
            LOGGER_ENTER_FUNCTION_DBG("msParamsManager::New()",T->getFullId());
            T->initialize(); T->update();
            LOGGER_EXIT_FUNCTION();
            return T;
        }
        
        virtual boost::shared_ptr<msTreeMapper> clone() {
            boost::shared_ptr<msParamsManager> clone = New();
            clone->msTreeMapper::operator=(*this);
            return clone;
        }
        //@}
        /** \brief add a parameter to the container
         *
         * \param id name of the parameter
         * \param val default value
         * \param abstract abstract
         * \param unit unit
         */
        void addParameter(string id,string val,string abstract,msUnit unit);
        
        std::ostream& print(std::ostream& out) const;
        
        std::vector< boost::shared_ptr<msParam> > getParameters();
        
        vector< boost::shared_ptr<msParam> >::iterator begin();
        
        vector< boost::shared_ptr<msParam> >::iterator end();
        
        
        template<class T>
        T getValue(string name,boost::shared_ptr<msUnitsManager> units) const {
            
            T value;
            msChildren<msParam>::const_iterator it=Parameters.find(name);
            
            if( it == Parameters.end() ) {
                
                msException e("The parameter queried ("+name+") does not exist",
                          "template<class T> T msParamsManager::getValue(string name,boost::shared_ptr<msUnitsManager> units) const",
                          getFullId());
                IMPACT_THROW_EXCEPTION(e);
            }
               
            try{ value  = boost::lexical_cast <T> ( (*it)->getValue() );
                 value *= units->convert( *((*it)->Unit) , 1 );
                 }
            catch(msException& e) {
                   
                e.addContext("template<class T> T msParamsManager::getValue(string name,boost::shared_ptr<msUnitsManager> units) const");
                IMPACT_THROW_EXCEPTION(e);
               }
            catch(boost::bad_lexical_cast &e0) {
                    
                msException e("Can not convert the parameter queried (string value: "+name+")",
                          "template<class T> T msParamsManager::getValue(string name,boost::shared_ptr<msUnitsManager> units) const",
                          getFullId());
                IMPACT_THROW_EXCEPTION(e);
            }
            
            return value;
        };
        
        //! @name query value
        //@{
        //! query the string value of the parameter *name*, if units are associated to the parameter it throws an exception (use queryDoubleValue() method instead)
        status_query queryStringValue(string name, string& val);
        
        //! query the value of the integer parameter *name*, if units are associated to the parameter it throws an exception (use queryDoubleValue() method instead)
        status_query queryIntValue(string name, int& val);
        
        //! query the value of the double parameter *name* in the units system *units*
        status_query queryDoubleValue(string name , double& val, const msUnitsManager& units );
        //@}
        //! @name set already defined parameters, convert the value using the units manager providen
        //@{
        //! set the parameter *name* from a string *value*, if the lexical cast fails throw an exception
        boost::shared_ptr<msTreeMapper> setParameter(string name , string value ,  const msUnitsManager& unit);
        
        //! set the parameter *name* from a double *value*
        boost::shared_ptr<msTreeMapper> setParameter(string name , double value ,  const msUnitsManager& unit);
        
        //! set the parameter *name* from a double *value*, if unit is not homogeneous with the initial one, it throws an exception
        boost::shared_ptr<msTreeMapper> setParameter(string name , double value ,  string unit);
        //@}
        
        void listWhoUsingMe();       //!< List msPhysicalInterface objects using the parameters manager  
        
    private:
        
        void updateObjectsUsingMe();
        
        void addObjectUsingMe(boost::shared_ptr<msPhysicalInterface> obj);
        
        std::vector<boost::weak_ptr<msPhysicalInterface> > LinkedObjects;
        
        boost::shared_ptr<msUnitsManager> Units;	
        
        msChildren<msParam> Parameters;
        
        std::vector<boost::shared_ptr<msParam> > ParametersForIteration;
    };
    
    
    
    std::ostream& operator<<(std::ostream& out,const msParamsManager& inter);
}
#endif // msPARAMETERS_H
