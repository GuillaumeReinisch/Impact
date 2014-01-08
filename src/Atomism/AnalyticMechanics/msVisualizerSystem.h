/*
 <one line to give the library's name and an idea of what it does.>
 Copyright (C) 2013  Guillaume <email>
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef MSSYSTEMVISUALIZER_H
#define MSSYSTEMVISUALIZER_H

#include <msSystem.h>
#include <msVisualizer.h>

namespace impact {
    
    namespace atomism {
        
        class msVisualizerSystem : public msVisualizer
        {
            
        private:
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool        isVisualizerSystemRegisteredInPython;
            static msRegister* createInstance() { return new msVisualizerSystem; }
            //@}
            
        protected:
            
            void registryInPython();

            void initialize()  {
                msVisualizer::initialize();
            };
            
            void update(){ msVisualizer::update();  }
            
        public:
            
	              
            msVisualizerSystem() : msVisualizer() { constructVar("msVisualizerSystem","VisualizerSystem","system visualizer"); }
            
            static boost::shared_ptr<msVisualizerSystem> New()
            {
                boost::shared_ptr<msVisualizerSystem> T( new msVisualizerSystem );
                LOGGER_ENTER_FUNCTION_DBG("msVisualizerSystem::New(boost::shared_ptr<msUnitsManager> units)",T->getFullId());
                T->initialize(); T->update();
                LOGGER_EXIT_FUNCTION();
                return T;
            }
            
            virtual void watch(boost::shared_ptr<msSystem> system);
            
        private:
            
            
            
        };
        
    }
    
    
}
#endif // MSSYSTEMVISUALIZER_H
