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
 * \file msUnitsManager.h
 * \brief define the msUnit and msUnitsManager classes
 * \author G.Reinisch
 * \date 2011
 */

#ifndef msUNITS_H
#define msUNITS_H

#include <msTreeMapper.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
//#include <boost/regex.hpp>
#include<boost/algorithm/string/replace.hpp>


namespace impact{
    
    using namespace std;
    
    class msPhysicalInterface;
    
    class msUnitsManager;
    
    /*! \class msUnit
     *
     * \brief Store the unit of a variable
     *
     * msUnit represent a unit as maps between primitive unit-symbols and a power.
     * The primitive types are of type *Length*, *Angle*, *Energy*, *Mass*,
     * *Time*, *Pressure*, *Quantity* and *Temperature*.
     *
     * The primitive units are used to define several composed variable types
     * (concentration, velocity, entropy, ..., see msUnit::uTypeVar ) by combining
     * different primitive unit type with appropriate power.
     *
     * To define a unit, one can either:
     *    - use an explicit string representation \n
     * The string representation is in the format \n
     *       *StrA^i.StrB^j.StrC^k ...* \n
     * where *StrA*, *StrB*, *StrC* ... are the string representation of some primitive unit symbols and
     * *i*, *j*, *k*,... the associated power.
     * See msUnitsManager::initMaps() to look at the string representation of the available symbols.
     *
     * example:
     * ~~~~~~~~~~~~~{.py}
     from libimpact import *
     import itertools
     
     unit = msUnit.New()
     unit.set( "mol^1.m^-3" )
     * ~~~~~~~~~~~~~
     *
     *    - a particular variable type (msUnit::uTypeVar) and units manager (msUnitsManager)
     *
     * example:
     * ~~~~~~~~~~~~~{.py}
     from libimpact import *
     import itertools
     
     unitMngr = msUnitsManager.new("mol m s kcal")
     unit = msUnit.New()
     unit.set( unitTypeVar.vConcentration, unitMngr )
     #i.e. unit.set( mol^1.m^-3 )
     * ~~~~~~~~~~~~~
     *
     * The list of available variable type are presented in the enum msUnit::uTypeVar.
     *
     * \section newUnit  Defining a new primitive unit symbol
     *
     * The definition of a new symbol of a primitive unit is achieved in two steps:
     *    - by defining the symbol's name in the appropriate enum list:
     * msUnit::uLength, msUnit::uEnergy, msUnit::uQuantity, msUnit::uMass,
     * msUnit::uTime, msUnit::uTemperature, msUnit::uAngle, msUnit::uPressure.
     *    - by defining the conversion factor to the corresponding SI unit. This is
     * done in the msUnitsManager::initMaps() method.
     *
     * \section newVarType  Defining a new composed variable type
     *
     * You need to:
     *     - complete the enum list msUnit::uTypeVar with a symbol for your new variable type.
     * Don't forget to indicate the decomposition associated in the doxygen documentation of your symbol.
     *     - provide a string representation of your symbol in the msUnitsManager::initMaps() method by
     * adding the element in the  msUnit::MapTypeVar attribute.
     *     - complete the function msUnit::set(uTypeVar typeVar, const msUnitsManager& unitsystem)
     * that define the decomposition on the primitive units.
     *     - export the type in python by completing the enum list in the msUnit::registryInPython()
     * method.
     *
     */
    class msUnit : public msTreeMapper
    {
        friend class msUnitsManager;
        friend class msParamsManager;
        
        //! @name Registration, see msRegister
        //@{
        static msRegister* createInstance() {  return new msUnit; }
        static msRegistrar Registrar;
        static bool isUnitRegisteredInPython;
        
        /* \brief register msUnit in python. 
         *
         * If a new composed variable type is defined, this method needs to be completed
         * by exporting the new enum symbol in python with the desired name.
         */
        void registryInPython();
        //@}
    public:

        /*! @name Primitive unit symbols, 
         *
         * See the msUnitsManager::initMap() method for the definition of the 
         * string representations and conversion factors.
         */
        //@{
        /*! symbols defined for the Length primitive unit.
         */
        enum uLength {
            Angstrom , Bohr , nm , m , NO_OF_LENGTH_MAX
        };
        /*! symbols defined for the Energy primitive unit.
         */
        enum uEnergy {
            J , kJ , cal , kcal , hartree , cm_1 , eV,
            J_mol , kJ_mol , cal_mol , kcal_mol, J_kmol , kJ_kmol , cal_kmol , kcal_kmol, NO_OF_ENERGY_MAX
        };
        /*! symbols defined for the Quantity primitive unit.
         */
        enum uQuantity {
            mol , kmol, entity , NO_OF_QUANTITY_MAX
        };
        /*! symbols defined for the Mass primitive unit.
         */
        enum uMass {
            amu , g , kg , g_mol , kg_mol, NO_OF_MASS_MAX
        };
        /*! symbols defined for the Time primitive unit.
         */
        enum uTime {
            ps , ns , micros , s, min, NO_OF_TIME_MAX
        };
        /*! symbols defined for the Temperature primitive unit.
         */
        enum uTemperature {
            K , C , NO_OF_TEMPERATURE_MAX
        };
        /*! symbols defined for the Angle primitive unit.
         */
        enum uAngle {
            Degree , Radian, NO_OF_ANGLE_MAX
        };
        /*! symbols defined for the Pressure primitive unit.
         */
        enum uPressure {
            Pa , NO_OF_PRESSURE_MAX
        };
        //@}
        
        /*! enum list of the defined composed variables and their decomposition on
         * the primitive types.
         *
         * If you ad a new symbol you need to complete: 
         * (i) the msUnit::set(uTypeVar typeVar, const msUnitsManager& unitsystem) method, and
         * (ii) the msUnit::regiterInPython() method.
         * (iii) the msUnitsManager::initMaps() method by providing a string representation of the symbol.
         */
        enum uTypeVar {
            vNone,              //!< no unit
            vLength,            //!< uLenght^1
            vAngle,             //!< uAngle^1
            vEnergy ,           //!< uEnergy^1
            vMass,              //!< uMass^1
            vQuantity,          //!< uQuantity^1
            vTime,              //!< uTime^1
            vTemperature,       //!< uTemperature^1
            vPressure,          //!< uPressure^1
            vVolume,            //!< uLength^3
            vVelocity,          //!< uLength^1.uTime^-1
            vAngularVelocity,   //!< uAngle^1.uTime^-1
	    vMomentOfInertia,   //!< uMass^1.uLength^2
            vLinearMomentum,    //!< uMass^1.uLength^2.uTime^-2
            vAngularMomentum,   //!< uMass^1.uLength^2.uAngle^-2
            vKineticEnergy,     //!< uMass^1.uLength^2.uTime^-2
            vDensity,           //!< uMass^1.uLength^-3
	    vQuantityDensity,   //!< uQuantity^1.uLength^-3
            vConcentration,     //!< uQuantity^1.uLength^-3
            vInverseTime,       //!< uTime^-1
            vEnergyByQuantity,  //!< uEnergy^1.uQuantity^-1
            vEntropyByQuantity  //!< uEnergy^1.uQuantity^-1.uTemperature^-1
        };
        
        static map< msUnit::uTypeVar , std::string > MapTypeVar;
        
        //! return the string label of a uTypeVar
        static string getStringVariableType(msUnit::uTypeVar);
        
        
    protected:
        //! @name Tree structure definition, see msTreeMapper
        //@{
        void initialize() {
            
            msTreeMapper::initialize();
            declareAttribute(Expression,"Expression");
        }
        void update(){
            msTreeMapper::update();
            set(Expression);
        }
        //@}
    public:
        
        static boost::shared_ptr<msUnit> New() {
            
            LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msUnit> msUnits::New()","");
            LOGGER_WRITE(msLogger::DEBUG, "New msUnits object")
            boost::shared_ptr<msUnit> T( new msUnit() );
            T->initialize(); T->update();
            LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msUnit> msUnits::New()");
            return T;
        }
        
        virtual boost::shared_ptr<msTreeMapper> clone() {
            boost::shared_ptr<msUnit> clone = New();
            clone->msTreeMapper::operator=(*this);
            return clone;
        }
        
        //! set a 'no unit' state
        msUnit() : msTreeMapper() { constructVar("msUnit","Unit","unit");  setNull();  };
        
        //! set from a string, see @ref string
        msUnit(string expr): msTreeMapper(){ constructVar("msUnit","Unit","unit"); set(expr); };
        
        //! set a 'no unit' state
        void setNull();
        
        //! set from a string, see @ref string
        void set(string line);
        
        /** \brief Set the unit from a variable type and a units manager
         *
         * see msUnit::uTypeVar for the proposed decomposition of 
         * the different variable type.
         * If a new msUnit::uTypeVar is defined, this method needs to be completed 
         * accordingly.
         *
         * \param typeVar type of the variable
         * \param unitsystem units system desired
         */
        msUnit& set(uTypeVar typeVar, const msUnitsManager& unitsystem);
        
        //! equal the powers of the unit to those of a given msUnit object
        void setDimFrom(const msUnit& u);
        
        //! return the string representation
        string getStr() const;
        
        //! return true if the object does not have unit
        bool isNoneUnit() const;
        
        //! return true if *this* is homogeneous with the given unit object
        bool isConsistent(const msUnit& unit) const;

        //! return true if *this* is homogeneous with the given unit object
        bool isConsistent(uTypeVar typeVar) const;
        
        std::ostream& print(std::ostream& out) const;
        
        msUnit& operator=(const msUnit& rhs){ msTreeMapper::operator=(rhs); Expression=rhs.Expression; return *this; }
        
        //! keep the power, but change the symbols to those define in the given unitsystem
        msUnit& reset(const msUnitsManager& unitsystem);
        
    private:
        
        std::string Expression;
        
        pair<uLength,int>         Length;
        pair<uEnergy,int>         Energy;
        pair<uQuantity,int>       Quantity;
        pair<uMass,int>           Mass;
        pair<uTime,int>           Time;
        pair<uTemperature,int>    Temperature;
        pair<uAngle,int>          Angle;
        pair<uPressure,int>       Pressure;
        
    };
    ostream& operator<<(ostream&,const msUnit&);
    
    
    /*! \class msUnitsManager
     *
     * \brief Manager of units system, used to handle conversions
     *
     *
     * The msUnitsManager class defines the units system used by the objects deriving
     * from msPhysicalInterface.
     * Its objective is to simply and clarify the use of unit system; in particular
     * the default behavior of msPhysicalInterface derived objects is to interpret inputs
     * and provide outputs using  the units system defined by the units manager.
     * The class provides also a lot of conversion functions. \n
     * The msPhysicalInterface objects are referenced in their msUnitsManager member
     * in the 'LinkedObjects' attribute.
     * Each time the units system is changed, the msUnitsManager class calls the
     * method msPhysicalInterface::updateUnits with the old and new units system.
     * This allows to implement the proper behavior to ensure that the
     * msPhysicalInterface object is always up to date with respect to its units
     * manager state.
     *
     *
     * \section newUnit2  Defining new units
     *
     * As we explained in the msUnit documentation, the definition of a new symbol
     * of unit is achieved in two steps:
     *     - defining the symbol's id in the appropriate enum list.
     * We have shown how to define the *BigMac/mol* symbol unit (as the enum
     * element *BigMac_mol*, see @ref newUnit).
     *     - defining the conversion factor to the associated SI unit.
     * To do so, you need to add an element to the corresponding map which associates
     * to the symbol's name :
     *          - a name (string representation)
     *          - a conversion factor with respect to the SI unit.
     * For  our BigMac/mol  unit, this is achievd by adding this line into the
     * method initMaps:
     * ~~~~~~~~~~~~~{.cpp}
     MapStrEnergy["BigMac/mol"] = pair<msUnit::uEnergy,double>( msUnit::BigMac_mol , 495 * 4186.7951  / 6.02214129e23 );
     * ~~~~~~~~~~~~~
     * (495 kcal in the energetic value of a Big Mac). In the same way Length, Angle, Temperature, Time and Mass
     * units can be completed.
     */
    class msUnitsManager : public msTreeMapper
    {
        friend class msPhysicalInterface;
        friend class msUnit;
	
        static msRegister* createInstance() { return new msUnitsManager; }
        static msRegistrar Registrar;
        static bool isUnitsManagerRegisteredInPython;
        void registryInPython();
        
    public:
        
        
        static double Convert(msUnit::uEnergy source,double v,msUnit::uEnergy dest);
        static double Convert(msUnit::uMass source  ,double v,msUnit::uMass dest);
        static double Convert(msUnit::uQuantity source  ,double v,msUnit::uQuantity dest);
        static double Convert(msUnit::uLength source,double v,msUnit::uLength dest);
        static double Convert(msUnit::uTime source,double v,msUnit::uTime dest);
        static double Convert(msUnit::uTemperature source,double v,msUnit::uTemperature dest);
        static double Convert(msUnit::uAngle source,double v,msUnit::uAngle dest);
        static double Convert(msUnit::uPressure source,double v,msUnit::uPressure dest);
        
        static string getStr(msUnit::uLength);
        static string getStr(msUnit::uEnergy);
        static string getStr(msUnit::uTime);
        static string getStr(msUnit::uMass);
        static string getStr(msUnit::uAngle);
        static string getStr(msUnit::uTemperature);
        static string getStr(msUnit::uQuantity);
        static string getStr(msUnit::uPressure);
        
        static map<string,pair<msUnit::uLength,double> >        MapStrLength;
        static map<string,pair<msUnit::uEnergy,double> >        MapStrEnergy;
        static map<string,pair<msUnit::uQuantity,double> >      MapStrQuantity;
        static map<string,pair<msUnit::uMass,double> >          MapStrMass;
        static map<string,pair<msUnit::uTime,double> >          MapStrTime;
        static map<string,pair<msUnit::uTemperature,double> >   MapStrTemperature;
        static map<string,pair<msUnit::uAngle,double> >         MapStrAngle;
        static map<string,pair<msUnit::uPressure,double> >      MapStrPressure;
        
        static bool _is_init;
        
        static std::vector<std::string> listUnitsAvailable(msUnit::uTypeVar v);
        
        void printUnityMap();
        
        /*! \brief initialize the conversion factors and string representation of unit's symbols
         *
         *  Below are indicated the string representations and convertion factors of the primitive unit.
         *  If you add a new primitive or composed unit, you have to complete the method accordingly.
         *
         *  primitive type      |   string representation   | Conversion |
         *  :----:              |   :----:                  |  :----:    |
         *  uLength::m          |   "m"                     |  1         |
         *  uLength::Angstrom   |   "Angsrom"               |  1e-10     |
         *  uLength::Bohr       |   "Bohr"                  |  5.2918e-11 |
         *  uLength::nm         |   "nm"                    |  1e-9      |
         *  uAngle::Radian      |   "Radian"                |  1         |
         *  uAngle::Degree      |   "Degree"                |  3.14159265/180.         |
         *  uEnergy::J          |   "J"                     |  1         |
         *  uEnergy::kJ         |   "kJ"                    |  1e3       |
         *  uEnergy::cal        |   "cal"                   |  4.1867951 |
         *  uEnergy::kcal       |   "kcal"                  |  4186.7951 |
         *  uEnergy::hartree    |   "hartree"               |  4.35974434e-18 |
         *  uEnergy::cm_1       |   "cm-1"                  |  1.98630e-23 |
         *  uEnergy::ev         |   "ev"                    |  1.60217657e-19 |
         *  uQuantity::entity   |   "entity"                |  1         |
         *  uQuantity::mol      |   "mol"                   |  6.02214129e23         |
         *  uQuantity::kmol     |   "kmol"                  |  6.02214129e26        |
         *  uMass::kg           |   "kg"                    |  1.         |
         *  uMass::amu          |   "amu"                   |  1.6605402E-27         |
         *  uMass::g            |   "g"                     |  1E-3           |
         *  uTime::s            |   "s"                     |  1.       |
         *  uTime::ps           |   "ps"                    |  1.e-12       |
         *  uTime::ns           |   "ns"                    |  1.e-9       |
         *  uTime::micros       |   "micros"                |  1.e-6       |
         *  uTime::min          |   "min"                   |  60       |
         *  uTemperature::K      |   "K"                    |  1.       |
         *  uPressure::Pa       |   "Pa"                    |  1.       |
         */
        static bool initMaps() {
            
            msUnit::MapTypeVar[msUnit::vNone] =             "None";
            msUnit::MapTypeVar[msUnit::vLength] =           "Length";
            msUnit::MapTypeVar[msUnit::vAngle] =            "Angle";
            msUnit::MapTypeVar[msUnit::vEnergy] =           "Energy";
            msUnit::MapTypeVar[msUnit::vMass] =             "Mass";
            msUnit::MapTypeVar[msUnit::vQuantity] =         "Quantity";
            msUnit::MapTypeVar[msUnit::vTime] =             "Time";
            msUnit::MapTypeVar[msUnit::vTemperature] =      "Temperature";
            msUnit::MapTypeVar[msUnit::vPressure] =         "Pressure";
            msUnit::MapTypeVar[msUnit::vVelocity] =         "Velocity";
            msUnit::MapTypeVar[msUnit::vMomentOfInertia] =  "MomentOfInertia";
            msUnit::MapTypeVar[msUnit::vAngularVelocity] =  "AngularVelocity";
            msUnit::MapTypeVar[msUnit::vLinearMomentum] =   "LinearMomentum";
            msUnit::MapTypeVar[msUnit::vAngularMomentum] =  "AngularMomentum";
            msUnit::MapTypeVar[msUnit::vKineticEnergy] =    "KineticEnergy";
            msUnit::MapTypeVar[msUnit::vDensity] =          "Density";
            msUnit::MapTypeVar[msUnit::vQuantityDensity] =  "vQuantityDensity";
            msUnit::MapTypeVar[msUnit::vVolume] =           "Volume";
            msUnit::MapTypeVar[msUnit::vConcentration] =    "Concentration";
            msUnit::MapTypeVar[msUnit::vInverseTime] =      "InverseTime";
            msUnit::MapTypeVar[msUnit::vEnergyByQuantity] = "EnergyByQuantity";
            msUnit::MapTypeVar[msUnit::vEntropyByQuantity]= "EntropyByQuantity";
            
            //----------------------------------------------------------------------------------
            
            MapStrLength["Angstrom"]    =pair<msUnit::uLength,double>(         msUnit::Angstrom ,
                        1e-10           	);
            MapStrLength["Bohr"]		=pair<msUnit::uLength,double>(             msUnit::Bohr ,
                        5.2918e-11      	);
            MapStrLength["nm"]          =pair<msUnit::uLength,double>(               msUnit::nm ,
                        1e-9            	);
            MapStrLength["m"]           =pair<msUnit::uLength,double>(                msUnit::m ,
                        1.              	);
            //----------------------------------------------------------------------------------
            
            MapStrEnergy["J"]           =pair<msUnit::uEnergy,double >(               msUnit::J ,
                        1.              	);
            MapStrEnergy["kJ"]          =pair<msUnit::uEnergy,double >(              msUnit::kJ ,
                        1e3             	);
            MapStrEnergy["cal"]         =pair<msUnit::uEnergy,double>(              msUnit::cal ,
                        4.1867951          	);
            MapStrEnergy["kcal"]		=pair<msUnit::uEnergy,double>(             msUnit::kcal ,
                        4186.7951         	);
            MapStrEnergy["hartree"]     =pair<msUnit::uEnergy,double>(          msUnit::hartree ,
                        4.35974434e-18  	);
            MapStrEnergy["cm-1"]		=pair<msUnit::uEnergy,double>(             msUnit::cm_1 ,
                        1.98630e-23     	);
            MapStrEnergy["eV"]          =pair<msUnit::uEnergy,double>(               msUnit::eV ,
                        1.60217657e-19	     	);
            MapStrEnergy["J/mol"]       =pair<msUnit::uEnergy,double >(           msUnit::J_mol ,
                        1./ 6.02214129e23       );
            MapStrEnergy["kJ/mol"]      =pair<msUnit::uEnergy,double >(          msUnit::kJ_mol ,
                        1e3 / 6.02214129e23     );
            MapStrEnergy["cal/mol"]     =pair<msUnit::uEnergy,double>(          msUnit::cal_mol ,
                        4.1867951  / 6.02214129e23 );
            MapStrEnergy["kcal/mol"]	=pair<msUnit::uEnergy,double>(         msUnit::kcal_mol ,
                        4186.7951  / 6.02214129e23 );
            MapStrEnergy["J/kmol"]      =pair<msUnit::uEnergy,double >(          msUnit::J_kmol ,
                        1.e-3/ 6.02214129e23       );
            MapStrEnergy["kJ/kmol"]     =pair<msUnit::uEnergy,double >(         msUnit::kJ_kmol ,
                        1 / 6.02214129e23     );
            MapStrEnergy["cal/kmol"]	=pair<msUnit::uEnergy,double>(         msUnit::cal_kmol ,
                        4.1867951 *1e-3 / 6.02214129e23 );
            MapStrEnergy["kcal/kmol"]	=pair<msUnit::uEnergy,double>(        msUnit::kcal_kmol ,
                        4.186  / 6.02214129e23 );
            //----------------------------------------------------------------------------------
            
            MapStrQuantity["mol"]		=pair<msUnit::uQuantity,double>(            msUnit::mol ,
                        6.02214129e23   	);
            MapStrQuantity["kmol"]		=pair<msUnit::uQuantity,double>(            msUnit::kmol ,
                                                                        6.02214129e26   	);
            MapStrQuantity["entity"]	=pair<msUnit::uQuantity,double>(         msUnit::entity ,
                        1.              	);
            //----------------------------------------------------------------------------------
            
            MapStrMass["amu"]           =pair<msUnit::uMass,double>(                msUnit::amu ,
                        1.6605402E-27   	);
            MapStrMass["g"]             =pair<msUnit::uMass,double>(                  msUnit::g ,
                        1e-3            	);
            MapStrMass["kg"]            =pair<msUnit::uMass,double>(                 msUnit::kg ,
                        1.              	);
            MapStrMass["g/mol"]         =pair<msUnit::uMass,double>(              msUnit::g_mol ,
                        1e-3 / 6.02214129e23    );
            MapStrMass["kg/mol"]		=pair<msUnit::uMass,double>(             msUnit::kg_mol ,
                        1.   / 6.02214129e23    );
            //----------------------------------------------------------------------------------
            
            MapStrTime["ps"]            =pair<msUnit::uTime,double>(                 msUnit::ps ,
                        1e-12           	);
            MapStrTime["ns"]            =pair<msUnit::uTime,double>(                 msUnit::ns ,
                        1e-9            	);
            MapStrTime["micros"]		=pair<msUnit::uTime,double>(             msUnit::micros ,
                        1e-6            	);
            MapStrTime["s"]             =pair<msUnit::uTime,double>(                  msUnit::s ,
                        1.              	);
            MapStrTime["min"]           =pair<msUnit::uTime,double>(                  msUnit::min ,
                        60.              	);
            //----------------------------------------------------------------------------------
            
            MapStrTemperature["K"]      =pair<msUnit::uTemperature,double>(           msUnit::K ,
                        1.              	);
            //----------------------------------------------------------------------------------
            
            MapStrPressure["Pa"]        =pair<msUnit::uPressure,double>(           msUnit::Pa ,
                        1.              	);
            //----------------------------------------------------------------------------------
            
            MapStrAngle["Degree"]       =pair<msUnit::uAngle,double>(            msUnit::Degree ,
                        3.14159265/180. 	);
            MapStrAngle["Radian"]       =pair<msUnit::uAngle,double>(            msUnit::Radian ,
                        1.      	) ;
            return 1;
        }
    protected:
        
        void initialize() {
            
            msTreeMapper::initialize();
            msTreeMapper::declareAttribute(Expr,"Expr");
        }
        
        void update(){
            
            msTreeMapper::update(); set(Expr);
        }
    public:
        
        static boost::shared_ptr<msUnitsManager> New() {
            
            LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msUnitsManager> msUnitsManager::New()","");
            LOGGER_WRITE(msLogger::DEBUG, "New msUnitsManager object")
            
            boost::shared_ptr<msUnitsManager> T( new msUnitsManager );
            T->initialize();
            T->update();
            
            LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msUnitsManager> msUnitsManager::New()");
            return T;
        }
        virtual boost::shared_ptr<msTreeMapper> clone() {
            boost::shared_ptr<msUnitsManager> clone = New();
            clone->msTreeMapper::operator=(*this);
            return clone;
        }
        
        msUnitsManager() : msTreeMapper() {
            
            if(!_is_init) initMaps();
            _is_init=1;
            constructVar("msUnitsManager","Units","units");  setSI();
        };
        
        msUnitsManager(std::string line) : msTreeMapper() {
            
            if(!_is_init) initMaps();
            _is_init=1;
            constructVar("msUnitsManager","Units","units");
            setSI();
            set(line);
        };
        
        void set( string line );
        
        void setSI();
        
        string getExpression() { return Expr; }
        
        std::vector<boost::shared_ptr<msPhysicalInterface> > getObjectsUsingMe() const;
        
        
        std::ostream& print(std::ostream& out) const;
        
        std::ostream& abstract(std::ostream& out) const;
        
        void listWhoUsingMe();

        msUnit::uEnergy getEnergy() const 	{ return Energy; }
        msUnit::uMass 	getMass() const 	{ return Mass; }
        msUnit::uLength getLength() const 	{ return Length; }
        msUnit::uAngle 	getAngle() const 	{ return Angle; }
        msUnit::uTime 	getTime() const 	{ return Time; }
        msUnit::uTemperature 	getTemperature() const 	{ return Temperature; }
        msUnit::uPressure 	getPressure()    const 	{ return Pressure; }
        msUnit::uQuantity 	getQuantity()    const 	{ return Quantity; }
 
	msUnit getUnit(msUnit::uTypeVar v) { return msUnit(getStr(v)); } 
	
        void 	setEnergy( msUnit::uEnergy v)  		{ Energy = v; }
        void 	setMass(msUnit::uMass v)  		{ Mass 	 = v; }
        void 	setLength(msUnit::uLength v)  		{ Length = v; }
        void 	setAngle(msUnit::uAngle v)  		{ Angle  = v; }
        void 	setTime(msUnit::uTime v)  		{ Time   = v; }
        void 	setTemperature(msUnit::uTemperature v)  	{ Temperature = v; }
        void	setPressure( msUnit::uPressure v)  		{ Pressure = v; }
        void 	setQuantity( msUnit::uQuantity v)  		{ Quantity = v; }
        
        //! @name return a string representation
        //@{
        string getStr(msUnit::uTypeVar typevar) const;
        string getStr(std::string typevar) const;
        
        string getStrFrom(const msUnit&) const;
        string getEnergyStr() const;
        string getLengthStr() const;
        string getTimeStr()   const;
        string getMassStr()   const;
        string getAngleStr()  const;
        string getTempStr()   const;
        string getPressureStr()   const;
        string getQuantityStr() const;
        string getInertiaStr() const;
        //@}
        
        /** @name Convert a value specified by a unit 'source' to the current unit system
         */
        //@{
        double convert(const msUnit& source , double a) 	const;
        double convert(std::string source, double a) 		const;
        double convert(const msPhysicalInterface& source,
                       msUnit::uTypeVar, double a) 		const;
        
        double convert(msUnit::uEnergy source,double v) 	const;
        double convert(msUnit::uMass source,double v) 		const;
        double convert(msUnit::uLength source,double v) 	const;
        double convert(msUnit::uTime source,double v) 		const;
        double convert(msUnit::uTemperature source,double v) 	const;
        double convert(msUnit::uPressure source,double v) 	const;
        double convert(msUnit::uAngle source,double v) 		const;
        double convert(msUnit::uQuantity source,double v) 	const;
        //@}
        /** @name Convert a value from the current unit system to the specified units 'target'
         */
        //@{  
        double convertTo(double v,msUnit::uEnergy target)	const 	{ return v/convert(target,1); 
        }
        double convertTo(double v,msUnit::uMass target) 	const   { return v/convert(target,1); 
        }
        double convertTo(double v,msUnit::uLength target) 	const 	{ return v/convert(target,1); 
        }
        double convertTo(double v,msUnit::uTime target) 	const   { return v/convert(target,1); 
        }  
        double convertTo(double v,msUnit::uTemperature target) 	const 	{ return v/convert(target,1); 
        }
        double convertTo(double v,msUnit::uPressure target) 	const 	{ return v/convert(target,1);
        }
        double convertTo(double v,msUnit::uAngle target) 	const  	{ return v/convert(target,1);
        }
        double convertTo(double v,msUnit::uQuantity target) 	const   { return v/convert(target,1); 
        }
        double convertTo(double v,const msUnit& target) 	const   { return v/convert(target,1); 
        }
        //@}      
        
	private:
        
	          
        //! @name Attributs
        //@{
        msUnit::uLength Length;
        msUnit::uEnergy Energy;
        msUnit::uQuantity Quantity;
        msUnit::uMass Mass;
        msUnit::uTime Time;
        msUnit::uTemperature Temperature;
        msUnit::uPressure Pressure;
        msUnit::uAngle Angle;
        //@}
	
        string Expr; 			//!< Store the expression value defined in a node
        
        void updateObjectsUsingMe(msUnitsManager& Old, msUnitsManager& New);
        
        void addObjectUsingMe(boost::shared_ptr<msPhysicalInterface> obj);
        
        std::vector<boost::weak_ptr<msPhysicalInterface> > LinkedObjects;
        
        void updateExpr();
        void initExpr(string line);
    };
    
    ostream& operator<<(ostream&,const msUnitsManager&);
    
}
#endif // msUNITS_H
