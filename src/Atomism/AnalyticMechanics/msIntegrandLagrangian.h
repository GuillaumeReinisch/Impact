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
 *
 * \author G. Reinisch, based, some times ago, on code of Peter Selinger, updated by Norman Hardy for the dynamic part.
 * \see http://cap-lore.com/MathPhys/DynamJavaDemo/ for the original source
 */

#ifndef msINTEGRANDS_H
#define msINTEGRANDS_H

#include <msScalarFunction.h>


namespace impact {
    
    namespace atomism {
        
        class msLagrangian;
        
        //! Functionnoid for observables, see function averageByBoltzmann
        class msObservable { public:
            msObservable(){};
            virtual double operator()() = 0;
        };
        
        
        /*! \class msQIntegrand
         *
         * Integrand of partition function.
         *
         */
        class msQIntegrand : public msScalarFunction
        {
            
            static msRegistrar Registrar;
            static bool  isQIntegrandRegisteredInPython;
            static msRegister* createInstance() { return new msQIntegrand; }
            
        protected:
            
            void registryInPython();
            
            void initialize()  { msScalarFunction::initialize();
            };
            
            void update()     {  msScalarFunction::update();
            };
            
        public:
            
            msQIntegrand();
            
            static boost::shared_ptr<msQIntegrand> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msQIntegrand> msQIntegrand::New()","");
                boost::shared_ptr<msQIntegrand> T( new msQIntegrand );
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msQIntegrand> msQIntegrand::New()");
                return T;
            }
            
            boost::shared_ptr<msTreeMapper> setSystemAndTemperature(const msLagrangian* sys,double temp);
            
            double evaluate();
            
        private:
             msUnitsManager SiUnits;
            
            double Econv, fact, R, T;
            const msLagrangian* system;
        };
        
        
        /*! \class msDOSIntegrand
         *
         * Integrand of density of states.
         *
         */
        class msDOSIntegrand : public msScalarFunction
        {
            static msRegistrar Registrar;
            static bool  isDOSIntegrandRegisteredInPython;
            static msRegister* createInstance() { return new msDOSIntegrand; }
            
        protected:
            
            void registryInPython();
            
            void initialize()  { msScalarFunction::initialize();
            };
            
            void update()     {  msScalarFunction::update();
            };
            
        public:
            
            msDOSIntegrand();
            
            static boost::shared_ptr<msDOSIntegrand> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msDOSIntegrand> msDOSIntegrand::New()","");
                
                boost::shared_ptr<msDOSIntegrand> T( new msDOSIntegrand );
                T->initialize();
                T->update();
                
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msDOSIntegrand> msDOSIntegrand::New()");
                return T;
            }
            boost::shared_ptr<msTreeMapper> setSystemAndEnergy(const msLagrangian* sys,double ener);
            
            double evaluate();
            
        private:
            
            msUnitsManager SiUnits;
            
            double Econv, EconvSi, fact, E, emin, ndof;
            
            const msLagrangian* system;
        };
        
 
        /*! \class msQbyObservable
         *
         * Integrand of Q by an observable.
         *
         */
        class msQbyObservable : public msQIntegrand
        {
            static msRegistrar Registrar;
            static bool  isQbyObservableRegisteredInPython;
            static msRegister* createInstance() { return new msQbyObservable; }
            
        protected:
            
            void registryInPython();
            
            void initialize()  { msQIntegrand::initialize();
            };
            
            void update()     {  msQIntegrand::update();
            };
            
        public:
            
            msQbyObservable();
            
            static boost::shared_ptr<msQbyObservable> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msQbyObservable> msQbyObservable::New()","");
                boost::shared_ptr<msQbyObservable> T( new msQbyObservable );
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msQbyObservable> msQbyObservable::New()");
                return T;
            }
            
            boost::shared_ptr<msTreeMapper> setObservable(msObservable* observable);
            
            msQbyObservable(msObservable* observable, const msLagrangian* sys,double temp);
            
            double evaluate();
            
            msObservable* Observable;
            
            double sumQ;
        };
        

        
        
    }
    //}ATOMIMS
}

#endif // msMECHSYSTEM_H
