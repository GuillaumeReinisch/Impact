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
 * \file c_msSolver
 * \brief 1D scalar function from data fit object definition
 * \author G.Reinisch
 * \date 2011
 */
#ifndef msSolver_H
#define msSolver_H

#include <msPhysicalInterface.h>

namespace impact
{
    
    /*! \class msSolver
     *
     * Virtual base class for solvers, do nothing more than registrates itself for now. 
     */
    class msSolver : public msPhysicalInterface
    {
    protected:
        class msSolverParams : public msParamsManager
        {
            static msRegistrar Registrar;
            static msTreeMapper* createInstance() { return new msSolverParams; }
            
        public:
            static boost::shared_ptr<msSolverParams> New() {
                
                boost::shared_ptr<msSolverParams> T( new msSolverParams );
                T->initialize(); T->update();
                return T;
            }
            msSolverParams() : msParamsManager() {}
        };
        
    private:
        
        //!\name from msRegister
        //@{
        static msRegistrar Registrar;
        static bool  isSolverRegisteredInPython;
        static msRegister* createInstance() { return new msSolver; }
        //@}
        
    protected:
        
        void registryInPython();
        
        void update(){ msPhysicalInterface::update(); }
        
    public:
        
        void initialize()  {
            msPhysicalInterface::initialize();
        };
        
        static boost::shared_ptr<msSolver> New(){
            
            LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msSolver> msSolver::New(string unitexpr)","");
            LOGGER_WRITE(msLogger::DEBUG, "New msSolver object")
            boost::shared_ptr<msSolver> T( new msSolver );
            T->initialize();
            T->setParameters(msSolverParams::New());
            T->update();
            LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msSolver> msSolver::New(string unitexpr)");
            return T;
        }
        
        virtual boost::shared_ptr<msTreeMapper> clone() {
            boost::shared_ptr<msSolver> clone = New();
            clone->msTreeMapper::operator=(*this);
            return clone;
        }
        
        msSolver () : msPhysicalInterface() { constructVar("msSolver","Solver","Solver");
        }
        
        std::ostream& print(std::ostream& out) const;
        
        
    };
    
}
#endif // msFCT1DFROMDATA_H
