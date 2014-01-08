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
 * \file c_msVisualizer
 * \brief 1D scalar function from data fit object definition
 * \author G.Reinisch
 * \date 2011
 */
#ifndef msmsVisualizer_H
#define msmsVisualizer_H

#include <msTreeMapper.h>

namespace impact
{
    
    /*! \class msVisualizer
     *
     * Virtual base class for visualizers, do nothing more than registrates itself.
     */
    class msVisualizer : public msTreeMapper
    {
        
    private:
        
        //!\name from msRegister
        //@{
        static msRegistrar Registrar;
        static bool  isVisualizerRegisteredInPython;
        static msRegister* createInstance() { return new msVisualizer; }
        //@}
        
    protected:
        
        void registryInPython();
        
        void update(){ msTreeMapper::update(); }
        
    public:
        
        void initialize()  {
            msTreeMapper::initialize();
        };
        
        static boost::shared_ptr<msVisualizer> New(){
            
            LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msVisualizer> msVisualizer::New(string unitexpr)","");
            LOGGER_WRITE(msLogger::DEBUG, "New msVisualizer object")
            boost::shared_ptr<msVisualizer> T( new msVisualizer );
            T->initialize();
            T->update();
            LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msVisualizer> msVisualizer::New(string unitexpr)");
            return T;
        }
        
        virtual boost::shared_ptr<msTreeMapper> clone() {
            boost::shared_ptr<msVisualizer> clone = New();
            clone->msTreeMapper::operator=(*this);
            return clone;
        }
        
        msVisualizer () : msTreeMapper() { constructVar("msVisualizer","Visualizer","visualizer");
        }
            
        std::ostream& print(std::ostream& out) const;
    
    
    };
    
}
#endif // msFCT1DFROMDATA_H
