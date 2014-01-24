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

#include "msUnitsManager.h"
#include <msPhysicalInterface.h>

namespace impact{
    
    bool msUnitsManager::isUnitsManagerRegisteredInPython=0;
    msRegistrar msUnitsManager::Registrar( "msUnitsManager" , msUnitsManager::createInstance );
    bool msUnit::isUnitRegisteredInPython=0;
    msRegistrar msUnit::Registrar( "msUnit" , msUnit::createInstance );
    
    std::map<msUnit::uTypeVar, std::string> msUnit::MapTypeVar;
    
    std::ostream& operator<<(std::ostream& out,const msUnit& unit)
    {
        return( unit.print(out) );
    }
    
    void msUnit::registryInPython() {
        
#if USE_PYTHON
        msRegister::registryInPython();
        
        if( ! msUnit::isUnitRegisteredInPython ) {
            
            using namespace boost::python;
            
            class_<msUnit,bases<msTreeMapper>, boost::shared_ptr<msUnit> >(
                                                                           "msUnit","define the unit of a number" ,
                                                                           no_init )
            .def( "New", &msUnit::New , "Create a new object")
            .staticmethod("New")
            .def( "getStr" , &msUnit::getStr ,
                 "return the label of the unit" )
            .def( "setDimFrom" , &msUnit::setDimFrom,
                 "set the dimension from another msUnits object"   )
            .def( "set" , (void (msUnit::*)(std::string) )&msUnit::set,
                 "set the unit from a string representation. arg2: value"   )
            .def( "getStringVariableType", &msUnit::getStringVariableType,
                 "return the string representation of the given unitTypeVar. arg1: variable type")
            .staticmethod("getStringVariableType")
            .def(self_ns::str(self_ns::self));
            enum_<uTypeVar>("unitTypeVar")
            .value("vNone",         vNone)
            .value("vLength",       vLength)
            .value("vAngle", 		vAngle)
            .value("vEnergy",       vEnergy)
            .value("vMass", 		vMass)
            .value("vQuantity", 	vQuantity)
            .value("vTime", 		vTime)
            .value("vTemperature", 	vTemperature)
            .value("vVolume", 	    vVolume)
            .value("vPressure", 	vPressure)
            .value("vVelocity", 	vVelocity)
            .value("vMomentOfInertia",vMomentOfInertia)
            .value("vAngularVelocity",vAngularVelocity)
            .value("vLinearMomentum", vLinearMomentum)
            .value("vAngularMomentum",vAngularMomentum)
            .value("vKineticEnergy",  vKineticEnergy)
            .value("vDensity",        vDensity)
            .value("vQuantityDensity",        vQuantityDensity)    
            .value("vConcentration",  vConcentration)
            .value("vInverseTime",vInverseTime)
            .value("vEnergyByQuantity",vEnergyByQuantity)
            .value("vEntropyByQuantity",vEntropyByQuantity)
            .export_values();
            msUnit::isUnitRegisteredInPython = 1 ;
        }
#endif
    }
    
    void msUnit::set(string line) {
        
        Expression=line;
        if( (line=="none") || (line=="") ){ setNull();
            return;
        }
        
        using namespace std;
        setNull();
        std::vector< std::string > strs0;
        boost::split(strs0,line, boost::is_any_of("."));
		
        for( size_t i=0; i<strs0.size(); i++)
        {
            std::vector< std::string > strs;
            boost::split(strs,strs0[i], boost::is_any_of("^"));
            int v=1;
            if( strs.size()>1 )
            {try { boost::lexical_cast <int> (strs[1] );
            }
                catch(...)
                {   BOOST_THROW_EXCEPTION(msError( "Can not get the power of the unit '"+strs[0]+"' (token='"+strs0[i]
                                                +"', exposant:'"+strs[1]+"')",
                                                "void msUnit::set(string line)","object has no id") );
                }
                v=atoi(strs[1].c_str());
            }
            if( msUnitsManager::MapStrLength.find( strs[0] ) != msUnitsManager::MapStrLength.end() )
                Length=pair<uLength,int>( msUnitsManager::MapStrLength[strs[0]].first , v );
            else
                if( msUnitsManager::MapStrEnergy.find( strs[0] ) != msUnitsManager::MapStrEnergy.end() )
                    Energy=pair<uEnergy,int>( msUnitsManager::MapStrEnergy[ strs[0]].first , v );
                else
                    if( msUnitsManager::MapStrQuantity.find( strs[0] ) != msUnitsManager::MapStrQuantity.end() )
                        Quantity=pair<uQuantity,int>( msUnitsManager::MapStrQuantity[ strs[0]].first , v );
                    else
                        if( msUnitsManager::MapStrMass.find( strs[0] ) != msUnitsManager::MapStrMass.end() )
                            Mass=pair<uMass,int>( msUnitsManager::MapStrMass[ strs[0]].first , v );
                        else
                            if( msUnitsManager::MapStrTime.find( strs[0] )  != msUnitsManager::MapStrTime.end() )
                                Time=pair<uTime,int>( msUnitsManager::MapStrTime[ strs[0]].first , v );
                            else
                                if( msUnitsManager::MapStrTemperature.find( strs[0] ) != msUnitsManager::MapStrTemperature.end() )
                                    Temperature=pair<uTemperature,int>( msUnitsManager::MapStrTemperature[ strs[0]].first ,v );
                                else
                                    if( msUnitsManager::MapStrPressure.find( strs[0] ) != msUnitsManager::MapStrPressure.end() )
                                        Pressure=pair<uPressure,int>( msUnitsManager::MapStrPressure[ strs[0]].first ,v );
                                    else
                                        if( msUnitsManager::MapStrAngle.find( strs[0] ) != msUnitsManager::MapStrAngle.end() )
                                            Angle=pair<uAngle,int>( msUnitsManager::MapStrAngle[ strs[0]].first , v );
                                        else if(strs[0]!="")
                                            BOOST_THROW_EXCEPTION( msError("symbol "+strs[0]+" not known"
                                                                           ,"msUnit::msUnit(string line)",getFullId()) );
        }
    }
    
    msUnit& msUnit::set(uTypeVar typeVar, const msUnitsManager& unitsystem) {
        
        setNull();
        switch(typeVar){
            case vNone:
                break;
            case vLength:
                Length=pair<	  uLength,int>( 	unitsystem.Length , 1 );
                break;
            case vEnergy:
                Energy=pair<	  uEnergy,int>( 	unitsystem.Energy , 1 );
                break;
            case vMass:
                Mass=pair<	  uMass,int>( 	     	unitsystem.Mass ,   1 );
                break;
            case vQuantity:
                Quantity = pair< uQuantity,int>( unitsystem.Quantity,1 );
                break;
            case vTemperature:
                Temperature=pair< uTemperature,int>( 	unitsystem.Temperature , 1 );
                break;
            case vTime:
                Time=pair<	  uTime,int>( 		unitsystem.Time ,   1 );
                break;
            case vAngle:
                Angle=pair<	  uAngle,int>( 		unitsystem.Angle ,  1 );
                break;
            case vVolume:
                Length =pair<	uLength,int>( 		unitsystem.Length ,  3 );
                break;
            case vPressure:
                Pressure=pair<	  uPressure,int>( 		unitsystem.Pressure ,  1 );
                break;
            case vVelocity:
                Length=pair<	  uLength,int>( 	unitsystem.Length , 1 );
                Time=pair<	  uTime,int>( 		unitsystem.Time ,  -1 );
                break;
            case vAngularVelocity:
                Angle=pair<	  uAngle,int>( 		unitsystem.Angle ,  1 );
                Time=pair<	  uTime,int>( 		unitsystem.Time ,  -1 );
                break;
            case vMomentOfInertia:
                Mass=pair<	  uMass,int>( 	     	unitsystem.Mass ,   1 );
                Length=pair<	  uLength,int>( 	unitsystem.Length , 2 );
                break;
            case vLinearMomentum:
                Mass=pair<	  uMass,int>( 	     	unitsystem.Mass ,   1 );
                Length=pair<	  uLength,int>( 	unitsystem.Length , 2 );
                Time=pair<	  uTime,int>( 		unitsystem.Time ,  -2 );
                break;
            case vAngularMomentum:
                Mass=pair<	  uMass,int>( 	     	unitsystem.Mass ,   1 );
                Length=pair<	  uLength,int>( 	unitsystem.Length , 2 );
                Angle=pair<	  uAngle,int>( 		unitsystem.Angle ,  1 );
                Time=pair<	  uTime,int>( 		unitsystem.Time ,  -1 );
                break;
            case vKineticEnergy:
                Mass=pair<	  uMass,int>( 	     	unitsystem.Mass ,   1 );
                Length=pair<	  uLength,int>( 	unitsystem.Length , 2 );
                Time=pair<	  uTime,int>( 		unitsystem.Time ,  -2 );
                break;
            case vDensity:
                Mass=pair<	  uMass,int>( 	     	unitsystem.Mass ,   1 );
                Length=pair<	  uLength,int>( 	unitsystem.Length , -3 );
                break;
            case vQuantityDensity:
                Quantity=pair<	  uQuantity,int>(      	unitsystem.Quantity ,1 );
                Length=pair<	  uLength,int>( 	unitsystem.Length , -3 );
                break;
            case vConcentration:
                Quantity=pair<	  uQuantity,int>( 	     unitsystem.Quantity ,   1 );
                Length=pair<	  uLength,int>( 	unitsystem.Length , -3 );
                break;
            case vInverseTime:
                Time=pair<	  uTime,int>( 	     	unitsystem.Time ,   -1 );
                break;
            case vEnergyByQuantity:
                Energy=pair<	  uEnergy,int>( 	     	unitsystem.Energy,   1 );
                Quantity = pair< uQuantity,int>( unitsystem.Quantity, -1 );
                break;
            case vEntropyByQuantity:
                Energy=pair<	  uEnergy,int>( 	     	unitsystem.Energy,   1 );
                Quantity = pair< uQuantity,int>( unitsystem.Quantity, -1 );
                Temperature=pair< uTemperature,int>( 	unitsystem.Temperature , -1 );
                break;
            default:
                BOOST_THROW_EXCEPTION(msError("The variable type passed to the function is not defined, you should defined it.",
                                              "void msUnit::set(uTypeVar typeVar, const msUnitsManager& unitsystem)",
                                              getFullId())
                                      );
        }
        return *this;
    }
    
    std::ostream& msUnit::print(std::ostream& out) const {
        
        out<<"Units selected: "<<getStr()<<std::endl;
        return out;
    }
    
    bool msUnit::isConsistent(const msUnit& unit) const {
        
        return( (Length.second      == unit.Length.second) &&
               ( Time.second        == unit.Time.second) &&
               ( Energy.second      == unit.Energy.second) &&
               ( Mass.second        == unit.Mass.second) &&
               ( Angle.second       == unit.Angle.second) &&
               ( Temperature.second == unit.Temperature.second) &&
               ( Pressure.second    == unit.Pressure.second) &&
               ( Quantity.second    == unit.Quantity.second));
    }
    
    bool msUnit::isConsistent(uTypeVar unit) const {
        
        msUnit v;
        v.set(unit,msUnitsManager());
        return( isConsistent(v));
    }
    
    bool msUnit::isNoneUnit() const{
        
        return( (Length.second==0) &&
               (Time.second==0) &&
               (Energy.second==0) &&
               (Mass.second==0) &&
               (Angle.second==0) &&
               (Temperature.second==0) &&
               (Pressure.second==0) &&
               (Quantity.second==0));
        
    }
    
    
    string msUnit::getStr() const {
        
        if( isNoneUnit() ){return "none";}
        stringstream r;
        if(Length.second!=0) r<<msUnitsManager::getStr(Length.first)<<"^"<<Length.second<<".";
        if(Time.second!=0) r<<msUnitsManager::getStr(Time.first)<<"^"<<Time.second<<".";
        if(Energy.second!=0) r<<msUnitsManager::getStr(Energy.first)<<"^"<<Energy.second<<".";
        if(Mass.second!=0) r<<msUnitsManager::getStr(Mass.first)<<"^"<<Mass.second<<".";
        if(Angle.second!=0) r<<msUnitsManager::getStr(Angle.first)<<"^"<<Angle.second<<".";
        if(Temperature.second!=0) r<<msUnitsManager::getStr(Temperature.first)<<"^"<<Temperature.second<<".";
        if(Pressure.second!=0) r<<msUnitsManager::getStr(Pressure.first)<<"^"<<Pressure.second<<".";
        if(Quantity.second!=0) r<<msUnitsManager::getStr(Quantity.first)<<"^"<<Quantity.second;
        return r.str();
    }
    
    std::string msUnit::getStringVariableType(msUnit::uTypeVar typevar) {
        
        if( msUnit::MapTypeVar.find(typevar)==msUnit::MapTypeVar.end())
            BOOST_THROW_EXCEPTION(
                                  msError("The 'typevar' is not included in the msUnit::MapTypeVar to get the string label, you need to complet the msUnit::MapTypeVar list","string msUnit::getStringVariableType(msUnit::uTypeVar typevar)"
                                          ,"")
                                  );
        
        return msUnit::MapTypeVar[typevar];
    }
    
    void msUnit::setNull(){
        
        Length=pair<uLength,int>();       Energy=pair<uEnergy,int>();
        Quantity=pair<uQuantity,int>();   Mass=pair<uMass,int>();
        Time=pair<uTime,int>();           Temperature=pair<uTemperature,int>();
        Pressure = pair<uPressure,int>(); Angle=pair<uAngle,int>();
    }
    
    void msUnit::setDimFrom(const msUnit& u){
        
        Length.second=u.Length.second; Energy.second=u.Energy.second;
        Quantity.second=u.Quantity.second; Mass.second=u.Mass.second;
        Time.second=u.Time.second; Temperature.second=u.Temperature.second;
        Pressure.second=u.Pressure.second; Angle.second=u.Angle.second;
    }
    
    msUnit& msUnit::reset(const msUnitsManager& u)
    { Length.first = u.Length;     Energy.first = u.Energy ;
        Quantity.first = u.Quantity; Mass.first = u.Mass ;
        Time.first = u.Time;         Temperature.first = u.Temperature ;
        Pressure.first = u.Pressure; Angle.first = u.Angle;
        return *this;
    }
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    double msUnitsManager::Convert(msUnit::uEnergy source,double e,msUnit::uEnergy dest){
        
        if( source==dest ) return e;
        double f = MapStrEnergy[ getStr(dest) ].second;
        double f1 = MapStrEnergy[ getStr(source) ].second;
        return e*f1/f;
    }
    
    double msUnitsManager::Convert(msUnit::uLength source,double e,msUnit::uLength dest){
        
        if( source==dest ) return e;
        double f = MapStrLength[ getStr(dest) ].second;
        double f1 = MapStrLength[ getStr(source) ].second;
        return e*f1/f;
    }
    
    double msUnitsManager::Convert(msUnit::uMass source,double e,msUnit::uMass dest) {
        
        if( source==dest ) return e;
        double f = MapStrMass[ getStr(dest) ].second;
        double f1 = MapStrMass[ getStr(source) ].second;
        return e*f1/f;
    }
    
    double msUnitsManager::Convert(msUnit::uQuantity source,double e,msUnit::uQuantity dest) {
        
        if( source==dest ) return e;
        double f = MapStrQuantity[ getStr(dest) ].second;
        double f1 = MapStrQuantity[ getStr(source) ].second;
        return e*f1/f;
    }
    
    double msUnitsManager::Convert(msUnit::uTime source,double e,msUnit::uTime dest) {
        
        if( source==dest ) return e;
        double f = MapStrTime[ getStr(dest) ].second;
        double f1 = MapStrTime[ getStr(source) ].second;
        return e*f1/f;
    }
    
    double msUnitsManager::Convert(msUnit::uTemperature source,double e,msUnit::uTemperature dest) {
        
        if( source==dest ) return e;
        double f = MapStrMass[ getStr(dest) ].second;
        double f1 = MapStrMass[ getStr(source) ].second;
        return e*f1/f;
    }
    
    double msUnitsManager::Convert(msUnit::uPressure source,double e,msUnit::uPressure dest) {
        
        if( source==dest ) return e;
        double f = MapStrPressure[ getStr(dest) ].second;
        double f1 = MapStrPressure[ getStr(source) ].second;
        return e*f1/f;
    }
    
    double msUnitsManager::Convert(msUnit::uAngle source,double e,msUnit::uAngle dest) {
        
        if( source==dest ) return e;
        double f = MapStrAngle[ getStr(dest) ].second;
        double f1 = MapStrAngle[ getStr(source) ].second;
        return e*f1/f;
    }
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    std::ostream& operator<<(std::ostream& out,const msUnitsManager& unit) {
        
        return( unit.print(out) );
    }
    
    
    void msUnitsManager::registryInPython() {
        
#if USE_PYTHON
        msTreeMapper::registryInPython();
        
        if( ! msUnitsManager::isUnitsManagerRegisteredInPython ) {
            
            using namespace boost::python;
            class_<msUnitsManager, bases<msTreeMapper>, boost::shared_ptr<msUnitsManager> >
            ( "msUnitsManager",
             "Define an unit system. It is used by msPhysicalInterface derived objects to define units of inputs and outputs.",
             no_init )
            .def( "New", &msUnitsManager::New , "Create a new object")
            .staticmethod("New")
            .def( "setSI" , 		&msUnitsManager::setSI ,
                 "set SI units" )
            .def( "set" , 		&msUnitsManager::set,
                 "set from a string containing the key strings of units" )
            .def( "getStrFrom" , 	&msUnitsManager::getStrFrom,
                 "return the string description of the units system" )
            .def( "convert" ,		(double(msUnitsManager::*)( std::string , double) const )
				 &msUnitsManager::convert,
                 "convert a number in msUnit to this system units. arg2: units of the value, arg3: value " )
            .def( "convert" ,		(double(msUnitsManager::*)( const msUnit& , double) const )
				 &msUnitsManager::convert,
                 "convert a number in msUnit to this system units " )
            .def( "convert" ,		(double(msUnitsManager::*)( const msUnit& , double) const )
				 &msUnitsManager::convert,
                 "convert a number in msUnit to this system units " )
            /*   .def( "convert" ,		(double(msUnitsManager::*)(const msPhysicalInterface&,msUnit::uTypeVar,double)
             &msUnitsManager::convert,
             "convert a number obtained from a msPhysicalInterface derived object to this system units. arg2: msPhysicalInterface object , arg3: type of variable, arg4: value " )*/
            .def( "printUnityMap" , 	&msUnitsManager::printUnityMap,
                 "print the map of conversion factors" )
            .def( "listWhoUsingMe", 	&msUnitsManager::listWhoUsingMe,
                 "list the object using this as units system" )
            .def( "getStr" ,		(string (msUnitsManager::*)(msUnit::uTypeVar) const )
                 &msUnitsManager::getStr,
                 "return the string representation of a particular variable type, arg2: variable type")
            .def( "getEnergyStr" , 	&msUnitsManager::getEnergyStr,
                 "return the unit of energy in a string" )
            .def( "getLengthStr" , 	&msUnitsManager::getLengthStr,
                 "return the unit of length in a string" )
            .def( "getTimeStr" , 	&msUnitsManager::getTimeStr,
                 "return the unit of time in a string" )
            .def( "getQuantityStr" , 	&msUnitsManager::getQuantityStr,
                 "return the unit of time in a string" )
            .def( "getMassStr" , 	&msUnitsManager::getMassStr,
                 "return the unit of mass in a string" )
            .def( "getAngleStr" , 	&msUnitsManager::getAngleStr,
                 "return the unit of angle in a string" )
            .def( "getTempStr" , 	&msUnitsManager::getTempStr,
                 "return the unit of temperature in a string" )
            .def( "getPressureStr" , 	&msUnitsManager::getPressureStr,
                 "return the unit of pressure in a string" )
            .def( "getInertiaStr" , 	&msUnitsManager::getInertiaStr,
                 "return the unit of moment of inertia in a string" )
	    .def( "getObjectsUsingMe" , &msUnitsManager::getObjectsUsingMe,
                 "return the list of objects (of type msPhysicalInterface) using the unit system.")
            .def( "listUnitsAvailable" , &msUnitsManager::listUnitsAvailable,
                 "return the list (strings) of units available for a particular variable type. arg2: variable type")
            .staticmethod("listUnitsAvailable")
            .def(self_ns::str(self_ns::self));
            
            msUnitsManager::isUnitsManagerRegisteredInPython = 1 ;
        }
#endif
    }
    
    
    map<string,pair<msUnit::uLength,double> > msUnitsManager::MapStrLength;
    map<string,pair<msUnit::uEnergy,double> > msUnitsManager::MapStrEnergy;
    map<string,pair<msUnit::uQuantity,double> > msUnitsManager::MapStrQuantity;
    map<string,pair<msUnit::uMass,double> > msUnitsManager::MapStrMass;
    map<string,pair<msUnit::uTime,double> > msUnitsManager::MapStrTime;
    map<string,pair<msUnit::uTemperature,double> > msUnitsManager::MapStrTemperature;
    map<string,pair<msUnit::uPressure,double> > msUnitsManager::MapStrPressure;
    map<string,pair<msUnit::uAngle,double> > msUnitsManager::MapStrAngle;
    
    bool msUnitsManager::_is_init=0;//= msUnitsManager::initMaps();
    
    string msUnitsManager::getStr(msUnit::uLength u) {
        
        map< string , pair<msUnit::uLength,double> >::iterator it;
        for( it = MapStrLength.begin() ; it!=MapStrLength.end(); ++it)
            if(it->second.first == u){return it->first;}
        return "unknown_length";
    }
    
    string msUnitsManager::getStr(msUnit::uEnergy u) {
        
        map< string , pair<msUnit::uEnergy,double> >::const_iterator it;
        for( it = MapStrEnergy.begin() ; it!=MapStrEnergy.end(); it++ )
            if(it->second.first == u){return it->first;}
        return "unknown_energy";
    }
    
    string msUnitsManager::getStr(msUnit::uTime u)  {
        
        map< string , pair<msUnit::uTime,double> >::iterator it;
        for( it = MapStrTime.begin() ; it!=MapStrTime.end(); it++)
            if(it->second.first == u){return it->first;}
        return "unknown_time";
    }
    
    string msUnitsManager::getStr(msUnit::uMass u)  {
        
        map< string , pair<msUnit::uMass,double> >::iterator it;
        for( it = MapStrMass.begin() ; it!=MapStrMass.end(); it++)
            if(it->second.first == u){return it->first;}
        return "unknown_mass";
    }
    
    string msUnitsManager::getStr(msUnit::uAngle u) {
        
        map< string , pair<msUnit::uAngle,double> >::iterator it;
        for( it = MapStrAngle.begin() ; it!=MapStrAngle.end(); it++)
            if(it->second.first == u){return it->first;}
        return "unknown_angle";
    }
    
    string msUnitsManager::getStr(msUnit::uTemperature u)  {
        
        map< string , pair<msUnit::uTemperature,double> >::iterator it;
        for( it = MapStrTemperature.begin() ; it!=MapStrTemperature.end(); it++)
            if(it->second.first == u){return it->first;}
        return "unknown_temperature";
    }
    
    string msUnitsManager::getStr(msUnit::uPressure u)  {
        
        map< string , pair<msUnit::uPressure,double> >::iterator it;
        for( it = MapStrPressure.begin() ; it!=MapStrPressure.end(); it++)
            if(it->second.first == u){return it->first;}
        
        return "[unknown_pressure (rank given = " +output::getString<int>(u)+")]";
    }
    
    string msUnitsManager::getStr(msUnit::uQuantity u)  {
        
        map< string , pair<msUnit::uQuantity,double> >::iterator it;
        for( it = MapStrQuantity.begin() ; it!=MapStrQuantity.end(); it++)
            if(it->second.first == u){return it->first;}
        return "unknown_quantity";
    }
 	
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    void msUnitsManager::initExpr(string line) {
        
        std::vector<std::string> strs;
        boost::split(*&strs,line, boost::is_any_of(" "));
        
        for( size_t i=0; i<strs.size(); i++) {
            
            if(MapStrLength.find(strs[i])!=MapStrLength.end())
                
                Length=MapStrLength[ strs[i] ].first;
            else
                if(MapStrQuantity.find(strs[i])!=MapStrQuantity.end())
                    
                    Quantity=MapStrQuantity[ strs[i] ].first;
                else
                    if(MapStrEnergy.find(strs[i])!=MapStrEnergy.end())
                        
                        Energy=MapStrEnergy[ strs[i] ].first;
                    else
                        if(MapStrMass.find(strs[i])!=MapStrMass.end())
                            
                            Mass=MapStrMass[ strs[i] ].first;
                        else
                            if(MapStrTime.find(strs[i])!=MapStrTime.end())
                                
                                Time=MapStrTime[ strs[i] ].first;
                            else
                                if(MapStrTemperature.find(strs[i])!=MapStrTemperature.end())
                                    
                                    Temperature=MapStrTemperature[ strs[i] ].first;
                                else
                                    if(MapStrPressure.find(strs[i])!=MapStrPressure.end())
                                        
                                        Pressure=MapStrPressure[ strs[i] ].first;
                                    else
                                        if(MapStrAngle.find(strs[i])!=MapStrAngle.end())
                                            
                                            Angle=MapStrAngle[ strs[i] ].first;
                                        else
                                            if(strs[i]!="")
                                                BOOST_THROW_EXCEPTION( msError("symbol "+strs[i]+" not known (from line: "+line+")"
                                                                               ,"void msUnitsManager::initExpr(string line)",getFullId())
                                                                      );
        }
        updateExpr();
    }
	
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    void msUnitsManager::set(string line) {
        
        msUnitsManager unitOld; unitOld.initExpr(Expr);
        initExpr(line);
        updateObjectsUsingMe(unitOld,*this);
    }
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    void msUnitsManager::setSI(){
        
        Length=msUnit::m;
        Energy=msUnit::J;
        Time=msUnit::s;
        Mass=msUnit::kg;
        Angle=msUnit::Radian;
        Temperature=msUnit::K;
        Quantity=msUnit::entity;
        Pressure = msUnit::Pa;
        updateExpr();
    }
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    void msUnitsManager::updateExpr() {
        
        Expr=getEnergyStr()+" "+
        getLengthStr()+" "+
        getMassStr()+" "+
        getQuantityStr()+" "+
        getAngleStr()+" "+
        getTempStr()+" "+
        getPressureStr()+" "+
        getTimeStr();
    }
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    void msUnitsManager::updateObjectsUsingMe( msUnitsManager& Old, msUnitsManager& New ) {
        
        LOGGER_ENTER_FUNCTION_DBG("void msUnitsManager::updateObjectsUsingMe( msUnitsManager& Old, msUnitsManager& New )", getFullId());
        LOGGER_WRITE(msLogger::DEBUG, "call the 'updateUnits' function of the "+output::getString<int>(LinkedObjects.size())+" objects using this")
        
        std::vector< boost::weak_ptr<msPhysicalInterface> >::iterator it;
        
        for(it=LinkedObjects.begin(); it!= LinkedObjects.end() ; ++it){
            
            if ( boost::shared_ptr<msPhysicalInterface> ptr = (*it).lock() )
                ptr->updateUnits(Old,New);
        }
        
        LOGGER_EXIT_FUNCTION2("void msUnitsManager::updateObjectsUsingMe( msUnitsManager& Old, msUnitsManager& New )");
    }
    
    //-------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    std::vector<boost::shared_ptr<msPhysicalInterface> > msUnitsManager::getObjectsUsingMe() const {
      
        std::vector< boost::weak_ptr<msPhysicalInterface> >::const_iterator it;
        std::vector< boost::shared_ptr<msPhysicalInterface> > vec;
	
        for(it=LinkedObjects.begin(); it!= LinkedObjects.end() ; ++it){
            
            if ( boost::shared_ptr<msPhysicalInterface> ptr = (*it).lock() )
                vec.push_back(ptr);
        }
        return vec;
    }
    
    //-------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    void msUnitsManager::addObjectUsingMe(boost::shared_ptr<msPhysicalInterface> obj) {
        
        std::vector< boost::weak_ptr<msPhysicalInterface> >::iterator it;
        
        for(it=LinkedObjects.begin(); it!= LinkedObjects.end() ; ++it){
            
            if ( boost::shared_ptr<msPhysicalInterface> ptr = (*it).lock() )
                if( ptr == obj ) return;
            
        }
        LinkedObjects.push_back( obj );
    }
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    std::vector<std::string> msUnitsManager::listUnitsAvailable(msUnit::uTypeVar v) {
        
        std::vector<std::string> result;
        
        if( v == msUnit::vLength ){
            
            map<string,pair<msUnit::uLength,double> >::iterator it;
            for( it = MapStrLength.begin();it!=MapStrLength.end();++it) {
                
                result.push_back( (*it).first );
            }
        }
        
        if( v == msUnit::vEnergy ){
            
            map<string,pair<msUnit::uEnergy,double> >::iterator it;
            for( it = MapStrEnergy.begin();it!=MapStrEnergy.end();++it) {
                
                result.push_back( (*it).first );
            }
        }
        if( v == msUnit::vMass ){
            
            map<string,pair<msUnit::uMass,double> >::iterator it;
            for( it = MapStrMass.begin();it!=MapStrMass.end();++it) {
                
                result.push_back( (*it).first );
            }
        }
        if( v == msUnit::vQuantity ){
            
            map<string,pair<msUnit::uQuantity,double> >::iterator it;
            for( it = MapStrQuantity.begin();it!=MapStrQuantity.end();++it) {
                
                result.push_back( (*it).first );
            }
        }
        if( v == msUnit::vAngle ){
            
            map<string,pair<msUnit::uAngle,double> >::iterator it;
            for( it = MapStrAngle.begin();it!=MapStrAngle.end();++it) {
                
                result.push_back( (*it).first );
            }
        }
        if( v == msUnit::vPressure ){
            
            map<string,pair<msUnit::uPressure,double> >::iterator it;
            for( it = MapStrPressure.begin();it!=MapStrPressure.end();++it) {
                
                result.push_back( (*it).first );
            }
        }
        if( v == msUnit::vTime ){
            
            map<string,pair<msUnit::uTime,double> >::iterator it;
            for( it = MapStrTime.begin();it!=MapStrTime.end();++it) {
                
                result.push_back( (*it).first );
            }
        }
        /*
         * msUnit u;
         msUnitsManager umangr;
         
         int nEner,nLength, nMass, nAngle, nTime , nPressure , nQuantity= 1;
         u.set(v,umangr);
         
         if( u.Energy.second > 0 )   nEner =     msUnit::NO_OF_ENERGY_MAX;
         if( u.Length.second > 0 )   nLength =   msUnit::NO_OF_LENGTH_MAX;
         if( u.Mass.second > 0 )     nMass =     msUnit::NO_OF_MASS_MAX;
         if( u.Quantity.second > 0 ) nQuantity = msUnit::NO_OF_QUANTITY_MAX;
         if( u.Angle.second > 0 )    nAngle =    msUnit::NO_OF_ANGLE_MAX;
         if( u.Pressure.second > 0 ) nPressure = msUnit::NO_OF_PRESSURE_MAX;
         if( u.Time.second > 0 )     nTime =     msUnit::NO_OF_TIME_MAX;
         
         msLogger::stop();
         
         for( size_t iEner=0;iEner<nEner;iEner++)
         for( size_t iLength=0;iLength<nLength;iLength++)
         for( size_t iMass=0;iMass<nMass;iMass++)
         for( size_t iQuantity=0;iQuantity<nQuantity;iQuantity++)
         for( size_t iAngle=0;iAngle<nAngle;iAngle++)
         for( size_t iPressure=0;iPressure<nPressure;iPressure++)
         for( size_t iTime=0;iTime<nTime;iTime++){
         
         std::string s = getStr( static_cast<msUnit::uEnergy>(iEner) ) + " "
         + getStr( static_cast<msUnit::uLength>(iLength) ) + " "
         + getStr( static_cast<msUnit::uMass>(iMass) ) + " "
         + getStr( static_cast<msUnit::uQuantity>(iQuantity) ) + " "
         + getStr( static_cast<msUnit::uAngle>(iAngle) ) + " "
         + getStr( static_cast<msUnit::uPressure>(iPressure) ) + " "
         + getStr( static_cast<msUnit::uTime>(iTime) );
         try {
         umangr.set(s);
         }
         catch (msError& e){
         
         e.addContext("std::vector<std::string> msUnitsManager::listUnitsAvailable(msUnit::uTypeVar v)");
         throw e;
         }
         
         try{u.set(v,umangr);}
         catch(msError& e){
         e.addContext("indice enum: "+output::getString<int>(v));
         throw e;
         }
         s = u.getStr();
         if ( std::find( result.begin(),result.end(),s) == result.end() )
         result.push_back(u.getStr());
         }
         */
        //msLogger::restart();
        return result;
    }
    
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    void msUnitsManager::listWhoUsingMe() {
        
        std::vector< boost::weak_ptr<msPhysicalInterface> >::iterator it;
        
        for(it=LinkedObjects.begin(); it!= LinkedObjects.end() ; ++it){
            
            if ( boost::shared_ptr<msPhysicalInterface> ptr = (*it).lock() )
                std::cout<<ptr->getFullId()<<std::endl;
        }
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    string msUnitsManager::getStrFrom(const msUnit& u0) const {
        
        string a=getEnergyStr()+"."+getLengthStr()+"."+getMassStr()+"."+
        getQuantityStr()+"."+getTimeStr()+"."+getTempStr()+"."+getPressureStr()+"."+getAngleStr();
        msUnit u;u.set(a);
        u.setDimFrom(u0);
        return u.getStr();
    }
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    /*
     string msUnitsManager::getStr(std::string typevar) const {
     
     if( msUnit::MapTypeVar.find(typevar) == msUnit::MapTypeVar.end() ) {
     
     std::stringstream out;
     out<<"The variable type '"+typevar+"' is not known. Available type :";
     for( std::map<string,msUnit::uTypeVar>::iterator it = msUnit::MapTypeVar.begin();
     it!=msUnit::MapTypeVar.end();it++ )
     out<<it->first<<", ";
     out<<". You can complete the variable type list by adding in the enum type msUnit::TypeVar"
     <<" and the map msUnit::MapTypeVar";
     BOOST_THROW_EXCEPTION(
     msError(out.str(),"string msUnitsManager::getStr(std::string typevar) const"
     ,getFullId())
     );
     }
     
     return getStr(msUnit::MapTypeVar[typevar]);
     }
     */
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    string msUnitsManager::getStr(msUnit::uTypeVar typevar) const {
        
        msUnit u;u.set(typevar,*this);
        return u.getStr();
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    string msUnitsManager::getEnergyStr() const 	{ return getStr(Energy);
    }
    string msUnitsManager::getQuantityStr() const 	{ return getStr(Quantity);
    }
    string msUnitsManager::getLengthStr() const 	{ return getStr(Length);
    }
    string msUnitsManager::getTimeStr() const 	{ return getStr(Time);
    }
    string msUnitsManager::getMassStr() const 	{ return getStr(Mass);
    }
    string msUnitsManager::getAngleStr() const 	{ return getStr(Angle);
    }
    string msUnitsManager::getTempStr() const 	{ return getStr(Temperature);
    }
    string msUnitsManager::getPressureStr() const 	{ return getStr(Pressure);
    }
    string msUnitsManager::getInertiaStr() const 	{ return getMassStr()+"."+getLengthStr()+"^2";
    }
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    std::ostream& msUnitsManager::print(std::ostream& out) const {
        
        msTreeMapper::print(out);
        out<<output::printSepLine()<<output::printYellow("\tUnits manager\n")<<output::printSepLine()<<std::endl;
        
        abstract(out);
        return out;
    }
    
    std::ostream& msUnitsManager::abstract(std::ostream& out) const {
        
        out<<"Energy: "<<getEnergyStr()<<"; "
        <<"Length: "<<getLengthStr()<<"; "
        <<"Mass: "<<getMassStr()<<"; "
        <<"Time: "<<getTimeStr()<<"; "
        <<"Temperature: "<<getTempStr()<<"; "
        <<"Pressure: "<<getPressureStr()<<"; "
        <<"Quantity: "<<getQuantityStr()<<"; "
        <<"Angle: "<<getAngleStr()<<"."<<endl;
        return out;
    }
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    double msUnitsManager::convert(msUnit::uEnergy source,double e) const {
        
        if( source==Energy ) return e;
        double f = MapStrEnergy[ getEnergyStr() ].second;
        double f1 = MapStrEnergy[ getStr(source) ].second;
        return e*f1/f;
    }
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    double msUnitsManager::convert(msUnit::uQuantity source,double e) const {
        
        if( source==Quantity ) return e;
        double f = MapStrQuantity[ getQuantityStr() ].second;
        double f1 = MapStrQuantity[ getStr(source) ].second;
        return e*f1/f;
    }
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    double msUnitsManager::convert(msUnit::uMass source,double e) const {
        
        if( source==Mass ) return e;
        double f = MapStrMass[ getMassStr() ].second;
        double f1 = MapStrMass[ getStr(source) ].second;
        return e*f1/f;
    }
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    double msUnitsManager::convert(msUnit::uLength source,double e) const {
        
        if( source==Length ) return e;
        double f  = MapStrLength[ getLengthStr() ].second;
        double f1 = MapStrLength[ getStr(source) ].second;
        return e*f1/f;
    }
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    double msUnitsManager::convert(msUnit::uTime source,double e) const {
        
        if( source==Time ) return e;
        double f  = MapStrTime[ getTimeStr() ].second;
        double f1 = MapStrTime[ getStr(source) ].second;
        return e*f1/f;
    }
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    double msUnitsManager::convert(msUnit::uTemperature source,double e) const {
        
        if( source==Temperature ) return e;
        double f = MapStrTemperature[ getTempStr() ].second;
        double f1 = MapStrTemperature[ getStr(source) ].second;
        return e*f1/f;
    }
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    double msUnitsManager::convert(msUnit::uPressure source,double e) const {
        
        if( source==Pressure ) return e;
        double f = MapStrPressure[ getPressureStr() ].second;
        double f1 = MapStrPressure[ getStr(source) ].second;
        return e*f1/f;
    }
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    double msUnitsManager::convert(msUnit::uAngle source,double e) const {
        
        if( source==Angle ) return e;
        double f = MapStrAngle[ getAngleStr() ].second;
        double f1 = MapStrAngle[ getStr(source) ].second;
        return e*f1/f;
    }
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    double msUnitsManager::convert(const msUnit& u , double v) const {
        
        v = v * pow( convert(u.Length.first , 1) , u.Length.second )
        * pow( convert(u.Energy.first , 1) , u.Energy.second )
        * pow( convert(u.Quantity.first , 1) , u.Quantity.second )
        * pow( convert(u.Mass.first , 1) , u.Mass.second )
        * pow( convert(u.Time.first , 1) , u.Time.second )
        * pow( convert(u.Temperature.first , 1) , u.Temperature.second )
        * pow( convert(u.Pressure.first , 1) , u.Pressure.second )
        * pow( convert(u.Angle.first , 1) , u.Angle.second );
        
        return v;
    }
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    double msUnitsManager::convert(const msPhysicalInterface& source,
                                   msUnit::uTypeVar type, double v) const {
        
        if(source.getUnits().get() == this ) return 1;
        msUnit u;
        u.set(type, *(source.getUnits()) );
        return convert(u , v) ;
    }
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    double msUnitsManager::convert(std::string unitstr , double v) const {
        
        msUnit unit;
        try{ unit.set(unitstr);
        }
        catch(msError& e){
            
            e.addContext("can not convert the string into unit (double msUnitsManager::convert(std::string unitstr , double v) const)");
            throw e;
        }
        
        return convert(unit,v);
    }
    
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    
    void msUnitsManager::printUnityMap() {
        
        cout<<"convertion factor to SI units"<<setprecision(5)<<endl;;
        map< string , pair<msUnit::uLength,double> >::iterator it;
        for( it = MapStrLength.begin(); it!= MapStrLength.end(); ++it)     		cout<<setw(15)<<it->first;
        cout<<endl;
        for( it = MapStrLength.begin(); it!= MapStrLength.end(); ++it)     		cout<<setw(15)<<it->second.second;
        cout<<endl<<endl;
        map< string , pair<msUnit::uEnergy,double> >::iterator it2;
        for( it2 = MapStrEnergy.begin(); it2!= MapStrEnergy.end(); ++it2)    		cout<<setw(15)<<it2->first;
        cout<<endl;
        for( it2 = MapStrEnergy.begin(); it2!= MapStrEnergy.end(); ++it2)     		cout<<setw(15)<<it2->second.second;
        cout<<endl<<endl;
        map< string , pair<msUnit::uTime,double> >::iterator it3;
        for( it3 = MapStrTime.begin(); it3!= MapStrTime.end(); ++it3)     		cout<<setw(15)<<it3->first;
        cout<<endl;
        for( it3 = MapStrTime.begin(); it3!= MapStrTime.end(); ++it3)     		cout<<setw(15)<<it3->second.second;
        cout<<endl<<endl;
        map< string , pair<msUnit::uMass,double> >::iterator it4;
        for( it4 = MapStrMass.begin(); it4!= MapStrMass.end(); ++it4)     		cout<<setw(15)<<it4->first;
        cout<<endl;
        for( it4 = MapStrMass.begin(); it4!= MapStrMass.end(); ++it4)     		cout<<setw(15)<<it4->second.second;
        cout<<endl<<endl;
        map< string , pair<msUnit::uQuantity,double> >::iterator it5;
        for( it5 = MapStrQuantity.begin(); it5!= MapStrQuantity.end(); ++it5)     	cout<<setw(15)<<it5->first;
        cout<<endl;
        for( it5 = MapStrQuantity.begin(); it5!= MapStrQuantity.end(); ++it5)     	cout<<setw(15)<<it5->second.second;
        cout<<endl<<endl;
        map< string , pair<msUnit::uAngle,double> >::iterator it6;
        for( it6 = MapStrAngle.begin(); it6!= MapStrAngle.end(); ++it6)     		cout<<setw(15)<<it6->first;
        cout<<endl;
        for( it6 = MapStrAngle.begin(); it6!= MapStrAngle.end(); ++it6)     		cout<<setw(15)<<it6->second.second;
        cout<<endl<<endl;
        map< string , pair<msUnit::uTemperature,double> >::iterator it7;
        for( it7 = MapStrTemperature.begin(); it7!= MapStrTemperature.end(); ++it7)    cout<<setw(15)<<it7->first;
        cout<<endl;
        for( it7 = MapStrTemperature.begin(); it7!= MapStrTemperature.end(); ++it7)    cout<<setw(15)<<it7->second.second;
        cout<<endl<<endl;
        map< string , pair<msUnit::uPressure,double> >::iterator it8;
        for( it8 = MapStrPressure.begin(); it8!= MapStrPressure.end(); ++it8)    cout<<setw(15)<<it8->first;
        cout<<endl;
        for( it8 = MapStrPressure.begin(); it8!= MapStrPressure.end(); ++it8)    cout<<setw(15)<<it8->second.second;
        cout<<endl<<endl;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    /*
     void msUnitsManager::listWhoUsingMe()
     {
     std::cout<<"Parameters manager '"<<getId()<<"' used in:"<<std::endl;
     std::vector< boost::weak_ptr<msPhysicalInterface> >::iterator it;
     for(it=LinkedObjects.begin(); it!= LinkedObjects.end() ; ++it) {
     
     if ( boost::shared_ptr<msPhysicalInterface> ptr = (*it).lock() )
     std::cout<<ptr->getId()<<std::endl;
     }
     }*/
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    
}
