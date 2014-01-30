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
 * \file msPhysicalInterface.h
 * \brief definition of the class msPhysicalInterface
 * \author G.Reinisch
 * \date 2011
 */

#ifndef msPhysicalInterface_H
#define msPhysicalInterface_H

#include <msTreeMapper.h>
#include <msParamsManager.h>
#include <msUnitsManager.h>
#include <boost/graph/graph_concepts.hpp>

namespace impact{
    
    using namespace std;
    
    namespace csts
    {
        extern  double h;   	// J s
        extern  double hb;      // J s
        extern  double kb;	// J/K
        extern  double pi;
        extern  double R; 	// J/mol
        extern  double Nav;
    }
    
    /*! \class msPhysicalInterface
     *
     *  \brief Provides the ability to manage units and parameters
     *
     * The objects derived from msPhysicalInterface are associated to a units manager
     * (see msUnitsManager) and a parameters manager (see msParamsManager).
     * The purpose of msPhysicalInterface is to ensure that data and managers
     * are always coherent.
     *
     * The units manager defines the units used to interpret data.
     * There are two rules:
     *      - the input data providen in *set* type of functions are interpreted
     * using the units manager
     *      - the output are always printed within the units manager system
     *
     * Each time the state of the units manager is changed an update function is called
     * to allow updating the data members, see updateUnits(msUnitsManager&,msUnitsManager&).
     * The physical data members declared (i.e. associated to a unit) are automatically
     * updated, see declarePhysicalVariable(const msUnit&,double*).
     *
     * The parameters manager contains the parameters used by the object (see
     * msParamsManager). Here also, an update function is called when the
     * parameters are changed, see updateParameters().
     *
     * \section derivePhysical Deriving from msPhysicalInterface
     *
     * Deriving from msPhysicalInterface usually requires a fairly small development
     * effort. There is three steps:
     *     - in the derived msTreeMapper::initialize method, declare the data members
     * you want to be always consistent with the units (called physical  data) using the method
     * declarePhysicalVariable(const msUnit&,double*).
     *     - if you have some members that are related to the units manager but can not
     * be declared as physical data, define the msPhysicalInterface::updateUnits()
     * method to define the expacted behavior tu update them when units are changed.
     * In this case do not forget to call the updateUnits() method of the parent class.
     *     - if you have some members that are related to the parameters value (e.g. the size
     * of a vector), derived the msPhysicalInterface::updateParameters()
     * method to define the expacted behavior (e.g. resize the vector).
     * In this case do not forget to call the updateParameters() method of the parent
     * class.
     *     - create the units manager and parameters manager (if a specific one has been
     * derived, see msParamsManager) in the *New* constructor. If you can, try to ask
     * for a msUnitsManager object as parameter of *New*, to ensure that all data will
     * be appropriatly interpreted.
     *
     * A typical example of msPhysicalInterface derived class would be:
     * ~~~~~~~~~~~~~{.cpp}
     #include <msPhysicalInterface.h>
     #include <anObjectThatDependsOnTheUnits.h>
     #include <anObjectThatDependsOnTheParameters.h>
     
     class myClass : public msPhysicalInterface
     {
     ////--------------------------------------
     // // The parameters
     ////--------------------------------------
     
     class myClassParams : public msParamsManager   // the container of parameters
     {
     static msRegistrar Registrar;
     static msRegister* createInstance() { return new myClassParams; }
	 
     public:
     
     void initialize() {
     
     msParamsManager::initialize();
	 addParameter( "aParam" ,   "500" ,   "some comments" , msUnit("kJ/mol") );
	 addParameter( "bParam" ,  "1e-15" , "some comments"  , msUnit("s") );
     }
     
     static boost::shared_ptr<myClassParams> New(){  // This is always the same, just change aClassParams by your class name
     
     boost::shared_ptr<myClassParams> T( new myClassParams );
	 T->initialize();
	 T->update();
	 return T;
     }
     
     myClassParams() : aParentClassWithParams() { }
     };
     
     ////--------------------------------------
     // // The members
     ////--------------------------------------
     
     double theBigMacEnergeticValue;
     
     double theSpeedOfLight;
     
     anObjectThatDependsOnTheUnits ObjectDependantOfUnits;
     
     anObjectThatDependsOnTheParameters ObjectDependantOfParameters;
     
     ////--------------------------------------
     // // The constructors
     ////--------------------------------------
     
     void initialize() {
     
     msPhysicalInterface::initialize();
     
     theBigMacEnergeticValue = 500;
     declarePhysicalVariable(msUnit("kcal/mol"), theBigMacEnergeticValue );
     
     theSpeedOfLight = 299 792 458;
     declarePhysicalVariable(msUnit("m.s^-1"),   theSpeedOfLight );
     }
     
     static boost::shared_ptr<myClass> New( boost::shared_ptr<msUnitsManager> units ){
     
     boost::shared_ptr<myClass> T( new myClass );
     T->initialize();
     T->setParameters( aClassParams::New() ); // T will be registrated as a linked object to the parameters manager
     T->setUnits(units); 		      // T will be registrated as a linked object to the units manager
     T->update();
     return T;
     }
     
     ////--------------------------------------
     // // The updators
     ////--------------------------------------
     
     void updateUnits(msUnitsManager& Old,msUnitsManager& New){
     
     msPhysicalInterface::updateUnits(Old,New);
     ObjectDependantOfUnits->updateFromUnitChange(Old,New);
     }
     
     void updateParameters(){
     
     msPhysicalInterface::updateParameters();
     ObjectDependantOfParameters->updateFromParamsChange();
     }
     
     ////--------------------------------------
     // // The printer
     ////--------------------------------------
     
     std::ostream& print(std::ostream& out) const {
     
     msPhysicalInterface::print(out);
     output::printHeader(out,"myClass");
     
     out<<"The speed of light in my current units system is: "<<theSpeedOfLight<<" "<<getUnits()->getString(msUnit::Velocity)<<endl;
     out<<"The energetic value of a Big Mac in my current units system is: "<<theBigMacEnergeticValue<<" "<<getUnits()->getString(msUnit::Energy)<<endl;
     
     return out;
     };
     };
     * ~~~~~~~~~~~~~
     *
     *
     */
    class msPhysicalInterface : public msTreeMapper //, public boost::enable_shared_from_this<msPhysicalInterface>
    {
        friend class msUnitsManager;
        friend class msParamsManager;
        
        //! @name From msRegister
        //@{
        static msRegistrar Registrar;
        static bool  isInterfaceRegisteredInPython;
        static msRegister* createInstance() { return new msPhysicalInterface; }
        //@}
    public:
      
        class msPhysicalVariable {
	  
	   friend class msPhysicalInterface;
	   
	public:
	     msPhysicalVariable(string id,double* value,const msUnit& unit){ 
	        
	        Name=id; Value=value; Unit.set(unit.getStr()); 
	    }
	     
	     string getName(){return Name;}
	     double getValue(){return *Value;}
	     msUnit getUnit(){return Unit;}
	     
	     static void registryInPython() { 
	       
#if USE_PYTHON
             using namespace boost::python;
             class_<msPhysicalVariable,boost::shared_ptr<msPhysicalVariable> >
                  ("msPhysicalVariable",
             "Defines a physical variable: union of value, id and unit ",
             no_init)
            .def( "getUnit", &msPhysicalVariable::getUnit ,
                 "return the unit." )
            .def( "getValue",&msPhysicalVariable::getValue ,
                 "return the value." )
            .def( "getName", &msPhysicalVariable::getName  ,
                 "return the name" );
	    
	    registerVector<msPhysicalVariable>("Vector_msPhysicalVariable","Vector of object deriving from msPhysicalVariable");
	    registerMapStringKey<msPhysicalVariable>("msPhysicalVariable");
#endif 
            }   
             
	private:
	  
	     void reset(msUnitsManager& New){ 
	       
	       (*Value) *= New.convert(Unit, 1 );
               Unit.reset(New);
	     }
	     string Name;
	     double* Value;
	     msUnit Unit;
	};
	
    protected:
        
        void registryInPython();
        
        void update() {
            
            msTreeMapper::update();
            Units->addObjectUsingMe(      boost::static_pointer_cast<msPhysicalInterface>( mySharedPtr() ) );
            Parameters->addObjectUsingMe( boost::static_pointer_cast<msPhysicalInterface>( mySharedPtr() ) );
            Parameters->setAffiliation(   mySharedPtr() );
	    updateParameters();
        }
        
    public:
        //! @name construction
        //@{
        msPhysicalInterface() : msTreeMapper() {
            
            constructVar("msPhysicalInterface","PhysicalInterface","physical interface");
		}
        
        void initialize() {
            
	    msTreeMapper::initialize();
            msTreeMapper::declareChild<msUnitsManager>(   Units, msUnitsManager::New(), "Units");
            msTreeMapper::declareChild<msParamsManager>( Parameters, msParamsManager::New(), "Parameters");
        }
        
        msPhysicalInterface& operator=(const msPhysicalInterface& rhs) {
            
            msTreeMapper::operator=(rhs);
            return *this;
        }
        //@}
        virtual ~msPhysicalInterface(){};
        
        std::ostream& print(std::ostream& out) const ;
        
        /*! \brief set the parameters manager
         * Set one parameter of the parameters manager. 
         * The units used are those defined in the units manager.
         *
         * \param key id of the parameter
         * \param value string representation of the value
         */
        void setParameter(string key, string value);
        
        //! \brief set the parameters manager
         void setParameters(boost::shared_ptr<msParamsManager> param);
        
        //! \brief set the units
        void setUnits(boost::shared_ptr<msUnitsManager> units);
        
        //! return the parameters  manager
        boost::shared_ptr<msParamsManager> getParameters() const{ return Parameters.getSharedPtr(); }
        
        //! return the units manager
        boost::shared_ptr<msUnitsManager> getUnits()  const     { return Units.getSharedPtr();}
        
        
        //! set the children physical objects to the units system msPhysicalInterface::getUnits()
        boost::shared_ptr<msTreeMapper> homogenizeUnits();
        
	/*! \brief return the physical variables 
	 * 
	 * This is for python as 
	 */ 
        vector<boost::shared_ptr<msPhysicalVariable> > getPhysicalVariables() const;
	
        const msUnitsManager& getUnits_const() const { return *Units; }
        
        
    protected:
        
       	msUnitsManager SiUnits;	//!< The SI units
        
        /** \brief declare the physical variables (i.e. associated to units) automatically upated when units change
         *
         * When data members are associated to unit, you should register them using this function in
         * the derived msTreeMapper::initialize() method.
         * \param unit unit of the data at the time it is declared
         * \param ptr pointer to the value of the variable
	 * \param id id 
         */
        void declarePhysicalVariable(const msUnit& unit,double* ptr,string id="");
        
	void declarePhysicalVariable(double* ptr,double value,const msUnit& unit,string id="");
	
        void clearPhysicalVariables(){PtrOnPhysicalVariables.clear();}
        
        /** \brief This function is called if the Parameters attribute has been modified
         *
         * By default, it does nothing. Reimplement it if you want additional operations.
         */
        virtual void updateParameters(){ /*update();*/ };
        
        /** \brief This function is called if the Units attribute is modified
         *
         * By default the function updates the values of the physical parameters
         * declared using the method declarePhysicalVariable(const msUnit& ,double* )
         * Reimplement it if you want additional operations, but don't forget to 
         * call the method of the parent class.
         */
        virtual void updateUnits(msUnitsManager& Old,msUnitsManager& New);
        
        
    private:
        
        msChild<msUnitsManager> Units;		//!< Units used by the class
        
        msChild<msParamsManager> Parameters;	//!< Parameters used by the class
        
        map< double* , msUnit > PtrOnPhysicalVariables;
	
	map<string,boost::shared_ptr<msPhysicalVariable> > PhysicalVariables;
        
    };
    
    //std::string getStringUnit(strunit::UnitType type, msPhysicalInterface* obj);
    
    std::ostream& operator<<(std::ostream& out,const msPhysicalInterface& inter);
    
}

#endif // msPhysicalInterface_H
