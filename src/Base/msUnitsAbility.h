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
 * \file msUnitsAbility.h
 * \brief definition of the class msUnitsAbility
 * \author G.Reinisch
 * \date 2011
 */

#ifndef msUnitsAbility_H
#define msUnitsAbility_H

#include <msUnitsManager.h>

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
    
    /*! \class msUnitsAbility
     *
     *  \brief Derive your object from this class if you want units management ability for your class.
     *
     *
     */
    class msUnitsAbility  //, public boost::enable_shared_from_this<msUnitsAbility>
    {
        public:
        //! @name construction
        //@{
        msUnitsAbility() {
		}
        
        void initialize(boost::shared_ptr<msTreeMapper> object) {
            
	        object->declareChild<msUnitsManager>(   Units, msUnitsManager::New(), "Units");
        }
        
        //@}
        virtual ~msUnitsAbility(){};
        
        void setUnits(boost::shared_ptr<msUnitsManager> units){
            
            msUnitsManager old;
            old.set( Units->getExpression() );
            
            msTreeMapper::update<msUnitsManager>( Units , units);
            units->addObjectUsingMe( boost::static_pointer_cast<msUnitsAbility>( mySharedPtr() ) );
            
            updateUnits(old,*units);
        }
        
        //! return the units manager
        boost::shared_ptr<msUnitsManager> getUnits()  const     { return Units.getSharedPtr();}
        
        const msUnitsManager& getUnits_const() const { return *Units; }
        
        
        protected:
        
       	static msUnitsManager SiUnits;	//!< The SI units
        
        /** \brief declare the physical variables (i.e. associated to units) automatically upated when units change
         *
         * When data members are associated to unit, you should register them using this function in
         * the derived msTreeMapper::initialize() method.
         * \param unit unit of the data at the time it is declared
         * \param ptr pointer to the value of the variable
         */
        void declarePhysicalVariable(const msUnit& unit,double* ptr){
            
            PtrOnPhysicalVariables.push_back( pair<double* , msUnit>(ptr,unit) );
        }

        
        void clearPhysicalVariables(){
            PtrOnPhysicalVariables.resize(0);
        }
        
        /** \brief This function is called if the Units attribute is modified
         *
         * By default the function updates the values of the physical parameters
         * declared using the method declarePhysicalVariable(const msUnit& ,double* )
         * Reimplement it if you want additional operations, but don't forget to
         * call the method of the parent class.
         */
        virtual void updateUnits(msUnitsManager& Old,msUnitsManager& New) {
            
            LOGGER_ENTER_FUNCTION_DBG("void msPhysicalInterface::updateUnits(msUnitsManager& Old,msUnitsManager& New)", getFullId());
            LOGGER_WRITE(msLogger::DEBUG, "update the value of the physical variables declared")
            
            vector< pair< double* , msUnit > >::iterator it = PtrOnPhysicalVariables.begin();
            
            for(;it!=PtrOnPhysicalVariables.end();++it){
                
                *((*it).first) *= New.convert( (*it).second, 1 );
                (*it).second.reset(New);
            }
            
            LOGGER_EXIT_FUNCTION2("void msPhysicalInterface::updateUnits(msUnitsManager& Old,msUnitsManager& New)");
        }

        
        
        private:
        
        msChild<msUnitsManager> Units;		//!< Units used by the class
        
        vector< pair< double* , msUnit > > PtrOnPhysicalVariables;
        
    };
    
    
}

#endif // msUnitsAbility_H
