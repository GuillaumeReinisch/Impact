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


#if USE_VTK

#ifndef msVisualizerZMat_H
#define msVisualizerZMat_H
#define vtkRenderingCore_AUTOINIT 4(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingFreeTypeOpenGL,vtkRenderingOpenGL)

#include <msVisualizerSystem.h>
#include <msZMat.h>

#include <vtkMolecule.h>
#include <vtkMoleculeMapper.h>

#include <vtkSmartPointer.h>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRendererCollection.h>

#include <vtkObject.h>
#include <vtkActor.h>
#include <vtkActor2DCollection.h>

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCommand.h>

namespace impact {
    
    namespace atomism {
        
        class msVisualizerZMat : public msVisualizerSystem , public vtkCommand
        {
            
        private:
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool        isVisualizerZMatRegisteredInPython;
            static msRegister* createInstance() { return new msVisualizerZMat; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void initialize()  {
                msVisualizerSystem::initialize();
            };
            
            void update(){ msVisualizerSystem::update();
            }
            
        public:
	    
	   msVisualizerZMat() : msVisualizerSystem() {
                constructVar("msVisualizerZMat","VisualizerZMat","zmat visualizer");
                Resolution=6;
            }
            
            static boost::shared_ptr<msVisualizerZMat> New()
            {
                boost::shared_ptr<msVisualizerZMat> T( new msVisualizerZMat );
                LOGGER_ENTER_FUNCTION_DBG("msVisualizerZMat::New(boost::shared_ptr<msUnitsManager> units)",T->getFullId());
                T->initialize(); T->update();
                LOGGER_EXIT_FUNCTION();
                return T;
            }
            
            virtual void watch(boost::shared_ptr<msZMat> zmt);
            
            void launch(boost::shared_ptr<msZMat> zmt);
            
            void setResolution(int r);
            
        private:
            
            
            void createVtkMolecule(vtkSmartPointer<vtkMolecule> molecule);
            
            int isBondedByCovalent(msAtom* a1,msAtom* a2);
            
            int Resolution;
            
            void Execute( vtkObject *caller, unsigned long eventId,
                         void * vtkNotUsed(callData) );
            
            void addToRenderer(vtkSmartPointer<vtkRenderer> renderer);
            
            void clearRendered(vtkRenderer* Renderer);
            
            size_t TimerCount;
            
            boost::shared_ptr<msZMat> ZMat;
            
            vtkSmartPointer<vtkMolecule> Molecule;
            vtkSmartPointer<vtkRenderer> Renderer;
            
            vtkSmartPointer<vtkRenderWindow> renWin;
            
            vtkSmartPointer<vtkInteractorStyleTrackballCamera> vtkStyleInteract;
            
            vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
            
        };
        
        
    }
    
}
#endif // msVisualizerZMat_H
#endif