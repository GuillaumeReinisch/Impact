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

#include "msSolver.h"



namespace impact
{
    
    
    bool msSolver::isSolverRegisteredInPython=0;
    
    msRegistrar msSolver::Registrar("msSolver", msSolver::createInstance);
    
    std::ostream& operator<<(std::ostream& out,const msSolver& fit)
    {
        fit.print(out);
        return out;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msSolver::registryInPython()
    {
#if USE_PYTHON
        msPhysicalInterface::registryInPython();
        
        if( ! msSolver::isSolverRegisteredInPython ) {
            
            using namespace boost::python;
            
            class_<msSolver,bases<msPhysicalInterface>,boost::shared_ptr<msSolver> >
            ("msSolver",
             "Virtual base class for visulalizers.",init<>() );
            
            msSolver::isSolverRegisteredInPython=1;
            
        }
#endif
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::ostream& msSolver::print(std::ostream& out) const {
        msPhysicalInterface::print(out); return out;
    }
    
    
}
