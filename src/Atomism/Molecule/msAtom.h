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
 * \file atom.h
 * \brief Definition of the classes msAtom, msVarGeom, msBond, msAngle and msDihedre
 * \author G.Reinisch
 * \date 2011
 */

#ifndef msATOM_H
#define msATOM_H

#include <msElement.h>
#include <msPhysicalInterface.h>

namespace impact {
    
    namespace atomism {
        
        std::string getAtomName(int atomicNumb);
	
	class msAtom;
	void getListAtomsFromName(string name,vector<boost::shared_ptr<msAtom> >& result);
	
        using namespace std;
        
        /*! \class msAtom
         * \brief Virtual base class for atom objects
         *
         * Atoms are elements associated to:
         *    - an atomic number
         *    - Enthalpy of formation at 298K
         *    - VdW radius
         *    - Covalent bond length
         *
         * Future work as to be done to link with the blue obelisk database to
         * obtain the properties.
         *
         * Defining a new atom is easy and fast: look at the msAtom.h file.
         * You can find some properties:
         *    - for covalent bond lengths: http://en.wikipedia.org/wiki/Covalent_radius
         *    - for enthalpies of formation : http://www.wiredchemist.com/chemistry/data/enthalpies
         */
        class msAtom : public msElement
        {
            
        protected:
	  
            static msRegistrar Registrar;
            static bool  isAtomRegisteredInPython;
            static msRegister* createInstance() { return new msAtom; }
            void registryInPython();
            
            void update() { msElement::update(); }
            
            void initialize() {
                msElement::initialize();
                declareAttribute(Color,"Color");
            }
           
            
            msAtom(int an, double m ,double hf , double vdw, double cr1, double cr2, double cr3, string color) : msElement() ,
            AtomicNb(an) , Mass(m) , HfExp298(hf) , VdWRadius(vdw) , CovalentRadius1(cr1) ,
            CovalentRadius2(cr2) , CovalentRadius3(cr2) , Color(color)  {
                
                constructVar("msAtom","Atom","atom");
                VariableAttributs.Id=getAtomName(an);
            }
            
        public:
            
	    msAtom() : msElement() , AtomicNb(0) , Mass(0) , HfExp298(0) ,
            VdWRadius(0) , CovalentRadius1(0) , CovalentRadius2(0) , CovalentRadius3(0)
            {
                constructVar("msAtom","Atom","atom");
            };
            
            static boost::shared_ptr<msAtom> New(){
                
                boost::shared_ptr<msAtom> T( new msAtom );
                LOGGER_ENTER_FUNCTION_DBG("msAtom::New()",T->getFullId());
                T->initialize(); T->update();
                LOGGER_EXIT_FUNCTION();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                
                boost::shared_ptr<msAtom> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            virtual string getName() const {return "noname";}
            
            
            const int AtomicNb; 	//!< Atomic number
            
            const double Mass; 		//!<  mass in amu
            
            const double HfExp298;	//!< Experimental formation enthalpy at 298K in kcal/mol
            
            const double VdWRadius;	//!< Van der Walls radius in Angstrom
            
            const double CovalentRadius1; //!< covalent radius in Angstrom, simple bond
            
            const double CovalentRadius2; //!< covalent radius in Angstrom, double bond
            
            const double CovalentRadius3; //!< covalent radius in Angstrom, triple bond
            
            string Color; 		//!< rgb color; ex: red = "255 0 0"
            
            double getMass(msUnit::uMass unit) const {
                return msUnitsManager::Convert(msUnit::amu,Mass,unit);
            };
	    
            double getHf(msUnit::uEnergy unit) const {
                return msUnitsManager::Convert(msUnit::kcal_mol,HfExp298,unit);
            };
	    
            double getVdWRadius(msUnit::uLength unit) const {
                return msUnitsManager::Convert(msUnit::Angstrom,VdWRadius,unit);
            };
            
        };
        
        double getCovalentLength(int order,const msAtom& a,const msAtom& b);
        
        boost::shared_ptr<msAtom> NewAtom(std::string type);
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        /*! \class msARatom
         * \brief definition of the argon atom
         */
        class msARatom : public msAtom
        {
        protected:
            static bool isARatomRegisteredInPython;
            static msRegistrar Registrar;
            static msRegister* createInstance() { return new msARatom; }
            void registryInPython();
            
            msARatom() : msAtom(18,39.948,0,1.06,1.06,-1,-1,"0 255 255") {constructVar("msARatom","ARatom","argon");};
            
        public:
            static boost::shared_ptr<msARatom> New() {
                
                boost::shared_ptr<msARatom> T( new msARatom );
                LOGGER_ENTER_FUNCTION_DBG("msARatom::New()",T->getFullId());
                T->initialize(); T->update();
                LOGGER_EXIT_FUNCTION();
                return T;
            }
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msARatom> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            string getName() const {return "AR";}
        };
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        /*! \class msHatom
         * \brief definition of the hydrogen atom
         */
        class msHatom : public msAtom
        {
        protected:
            static bool isHatomRegisteredInPython;
            static msRegistrar Registrar;
            static msRegister* createInstance() { return new msHatom; }
            void registryInPython();
            
            msHatom() : msAtom(1,1.00783,52.070,0.45,0.32,-1,-1,"255 255 255") {constructVar("msHatom","Hatom","hydrogen");};
            
        public:
            static boost::shared_ptr<msHatom> New() {
                
                boost::shared_ptr<msHatom> T( new msHatom );LOGGER_ENTER_FUNCTION_DBG("msHatom::New()",T->getFullId());
                T->initialize(); T->update();
                LOGGER_EXIT_FUNCTION();
                return T;
            }
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msHatom> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            string getName() const {return "H";}
        };
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        /*! \class msBatom
         * \brief definition of the boron atom
         */
        class msBatom : public msAtom
        {
        protected:
            static bool isBatomRegisteredInPython;
            static msRegistrar Registrar;
            static msRegister* createInstance() { return new msBatom; }
            void registryInPython();
            
            msBatom() : msAtom(5,11.00931,134.930,0.5,0.85,0.78,0.73,"255 181 181") {constructVar("msBatom","Batom","boron");};
        public:
            static boost::shared_ptr<msBatom>  New() {
                
                boost::shared_ptr<msBatom> T( new msBatom );LOGGER_ENTER_FUNCTION_DBG("msBatom::New()",T->getFullId());
                T->initialize(); T->update();
                LOGGER_EXIT_FUNCTION();
                return T;}
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msBatom> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            string getName() const {return "B";}
        };
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        /*! \class msCatom
         * \brief definition of the carbon atom
         */
        class msCatom : public msAtom
        {
        protected:
            static bool isCatomRegisteredInPython;
            static msRegistrar Registrar;
            static msRegister* createInstance() { return new msCatom; }
            void registryInPython();
            
            msCatom() : msAtom(6,12,171.290,0.55,0.75,0.67,0.60,"144 144 144"){constructVar("msCatom","Catom","carbon");};
        public:
            static boost::shared_ptr<msCatom>  New() {
                
                boost::shared_ptr<msCatom> T( new msCatom );LOGGER_ENTER_FUNCTION_DBG("msCatom::New()",T->getFullId());
                T->initialize(); T->update();
                LOGGER_EXIT_FUNCTION();
                return T;}
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msCatom> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            string getName() const {return "C";}
        };
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        /*! \class msOatom
         * \brief definition of the oxygen atom
         */
        class msOatom : public msAtom
        {
        protected:
            static bool isOatomRegisteredInPython;
            static msRegistrar Registrar;
            static msRegister* createInstance() { return new msOatom; }
            void registryInPython();
            
            msOatom() : msAtom(8,15.9994,59.53,0.7,0.6,0.57,0.53,"255 13 13") {constructVar("msOatom","Oatom","oxygen");};
        public:
            static boost::shared_ptr<msOatom>  New() {
                
                boost::shared_ptr<msOatom> T( new msOatom );LOGGER_ENTER_FUNCTION_DBG("msOatom::New()",T->getFullId());
                T->initialize(); T->update();
                LOGGER_EXIT_FUNCTION();
                return T;}
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msOatom> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            string getName() const {return "O";}
        };
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        /*! \class msSIatom
         * \brief definition of the silicum atom
         */
        class msSIatom : public msAtom
        {
        protected:
            static bool isSIatomRegisteredInPython;
            static msRegistrar Registrar;
            static msRegister* createInstance() { return new msSIatom; }
            void registryInPython();
            
            msSIatom() : msAtom(14,27.97,107.4,0.65,1.16,1.07,1.02,"240 200 160"){constructVar("msSIatom","SIatom","silicium");};
        public:
            static boost::shared_ptr<msSIatom>  New() {
                
                boost::shared_ptr<msSIatom> T( new msSIatom );LOGGER_ENTER_FUNCTION_DBG("msSIatom::New()",T->getFullId());
                T->initialize(); T->update();
                LOGGER_EXIT_FUNCTION();
                return T;}
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msSIatom> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            string getName() const {return "Si";}
        };
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        /*! \class msCLatom
         * \brief definition of the chlore atom
         */
        class msCLatom : public msAtom
        {
        protected:
            static bool isCLatomRegisteredInPython;
            static msRegistrar Registrar;
            static msRegister* createInstance() { return new msCLatom; }
            void registryInPython();
            
            msCLatom() : msAtom(17,34.96885,28.980,0.7,0.99,0.95,0.93,"31 240 31") {constructVar("msCLatom","CLatom","chlorine");};
        public:
            static boost::shared_ptr<msCLatom>  New() {
                
                boost::shared_ptr<msCLatom> T( new msCLatom );LOGGER_ENTER_FUNCTION_DBG("msCLatom::New()",T->getFullId());
                T->initialize(); T->update();
                LOGGER_EXIT_FUNCTION();
                return T;}
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msCLatom> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            string getName() const {return "Cl";}
        };
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        /*! \class msNatom
         * \brief definition of the nitrogen atom
         */
        class msNatom : public msAtom
        {
        protected:
            static bool isNatomRegisteredInPython;
            static msRegistrar Registrar;
            static msRegister* createInstance() { return new msNatom; }
            void registryInPython();
            
            msNatom() : msAtom(7,18.9984032,112.9957, 0.7525,0.71,-1,-1,"50 0 255") {constructVar("msNatom","Natom","nitrogen");};
        public:
            static boost::shared_ptr<msNatom>  New() {
                
                boost::shared_ptr<msNatom> T( new msNatom );LOGGER_ENTER_FUNCTION_DBG("msNatom::New()",T->getFullId());
                T->initialize(); T->update();
                LOGGER_EXIT_FUNCTION();
                return T;}
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msNatom> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            string getName() const {return "N";}
        };
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
       
        /*! \class msFatom
         * \brief definition of the fluor atom
         */
        class msFatom : public msAtom
        {
        protected:
            static bool isFatomRegisteredInPython;
            static msRegistrar Registrar;
            static msRegister* createInstance() { return new msFatom; }
            void registryInPython();
            
            msFatom() : msAtom(9,14.007,18.87, 0.75,0.57,-1,-1,"178 255 255") {constructVar("msFatom","Fatom","fluor");};
	    
        public:
            static boost::shared_ptr<msFatom>  New() {
                
                boost::shared_ptr<msFatom> T( new msFatom );LOGGER_ENTER_FUNCTION_DBG("msFatom::New()",T->getFullId());
                T->initialize(); T->update();
                LOGGER_EXIT_FUNCTION();
                return T;}
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msFatom> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            string getName() const {return "F";}
        };
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        /*! \class msHEatom
         * \brief definition of the helium atom
         */
        class msHEatom : public msAtom
        {
        protected:
            static bool isHEatomRegisteredInPython;
            static msRegistrar Registrar;
            static msRegister* createInstance() { return new msHEatom; }
            void registryInPython();
            
            msHEatom() : msAtom(2,4.002603254,0, 0.7,0.37,-1,-1,"216 255 255") {constructVar("msHEatom","HEatom","helium");};
	    
        public:
            static boost::shared_ptr<msHEatom>  New() {
                
                boost::shared_ptr<msHEatom> T( new msHEatom );LOGGER_ENTER_FUNCTION_DBG("msHEatom::New()",T->getFullId());
                T->initialize(); T->update();
                LOGGER_EXIT_FUNCTION();
                return T;}
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msHEatom> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            string getName() const {return "He";}
        };
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        /*! \class msPatom
         * \brief definition of the phosphore atom
         */
        class msPatom : public msAtom
        {
        protected:
            static bool isPatomRegisteredInPython;
            static msRegistrar Registrar;
            static msRegister* createInstance() { return new msPatom; }
            void registryInPython();
            
            msPatom() : msAtom(15,30.97376163,75.25, 1.,0.55,-1,-1,"255 177.5 255") {constructVar("msPatom","Patom","phosphore");};
	    
        public:
            static boost::shared_ptr<msPatom>  New() {
                
                boost::shared_ptr<msPatom> T( new msPatom );LOGGER_ENTER_FUNCTION_DBG("msPatom::New()",T->getFullId());
                T->initialize(); T->update();
                LOGGER_EXIT_FUNCTION();
                return T;}
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msPatom> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            string getName() const {return "P";}
        };
	
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        /*! \class msNEatom
         * \brief definition of the neon atom
         */
        class msNEatom : public msAtom
        {
        protected:
            static bool isNEatomRegisteredInPython;
            static msRegistrar Registrar;
            static msRegister* createInstance() { return new msNEatom; }
            void registryInPython();
            
            msNEatom() : msAtom(10,19.99244018,0, 0.77,0.35,-1,-1,"178 226. 244.") {constructVar("msNEatom","NEatom","neon");};
	    
        public:
            static boost::shared_ptr<msNEatom>  New() {
                
                boost::shared_ptr<msNEatom> T( new msNEatom );LOGGER_ENTER_FUNCTION_DBG("msNEatom::New()",T->getFullId());
                T->initialize(); T->update();
                LOGGER_EXIT_FUNCTION();
                return T;}
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msNEatom> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            string getName() const {return "Ne";}
        };
	
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
               
        /*! \class msXatom
         * \brief definition of the ghost atom
         */
        class msXatom : public msAtom
        {
            msXatom() : msAtom(0,0,0,0.1,-1,-1,-1,"0 255 0") {constructVar("msXatom","Xatom","ghost atom");};
        public:
            static boost::shared_ptr<msXatom>  New() {
                
                boost::shared_ptr<msXatom> T( new msXatom );LOGGER_ENTER_FUNCTION_DBG("msXatom::New()",T->getFullId());
                T->initialize(); T->update();
                LOGGER_EXIT_FUNCTION();
                return T;}
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msXatom> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            string getName() const {return "X";}
        };
    }
}

#endif // msATOM_H
