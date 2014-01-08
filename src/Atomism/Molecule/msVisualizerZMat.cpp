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


#include "msVisualizerZMat.h"


namespace impact {
    
    namespace atomism {
#if USE_VTK
        
        
        bool msVisualizerZMat::isVisualizerZMatRegisteredInPython=0;
        
        msRegistrar msVisualizerZMat::Registrar("msVisualizerZMat", msVisualizerZMat::createInstance);
        
        
        void msVisualizerZMat::registryInPython() {
            
#if USE_PYTHON
            msVisualizerSystem::registryInPython();
            
            if( ! msVisualizerZMat::isVisualizerZMatRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msVisualizerZMat,bases<msVisualizerSystem>,boost::shared_ptr<msVisualizerZMat> >
                ("msVisualizerZMat",
                 "Visualizer of Z-Matrix",no_init)
                .def( "New", &msVisualizerZMat::New , "Create a new object.")
                .staticmethod("New")
                .def("watch",&msVisualizerZMat::watch,"display the 3D struture of the ZMat")
                .def("launch",&msVisualizerZMat::launch,"same as warch, but the 3D window is updated with a timer / zmat.");
                
                msVisualizerZMat::isVisualizerZMatRegisteredInPython=1;
            }
#endif
        }
        
        void msVisualizerZMat::setResolution(int r) {
            
            Resolution=r;
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msVisualizerZMat::watch(boost::shared_ptr<msZMat> zmat){
            
            
            vtkMolecule* mol = vtkMolecule::New();
            
            vector<vtkAtom> atomsVtk;
            
            for(msEntity::ElementItr it= zmat->beginElements();
                it!= zmat->endElements();it++) {
                
                msAtom* msatom = (msAtom*)( (*it).get() );
                Eigen::Vector3d pos=msatom->getPosition();
                atomsVtk.push_back( mol->AppendAtom(msatom->AtomicNb,pos[0],pos[1],pos[2]) );
            }
            
            msEntity::ElementItr it1=zmat->beginElements();
            msEntity::ElementItr it2=zmat->beginElements();
            int order=0;
            
            for( int i=0; it1 != zmat->endElements(); it1++,i++ ){
                
                msAtom* atom1 =(msAtom*)( (*it1).get() );
                int j=i;
                
                for( it2=it1; it2 != zmat->endElements() ; it2++,j++) {
                    
                    msAtom* atom2 = (msAtom*)( (*it2).get() );
                    
                    if( (order=isBondedByCovalent(atom1,atom2))>0 )
                        mol->AppendBond( atomsVtk[i], atomsVtk[j], isBondedByCovalent(atom1,atom2) );
                }
            }
            
            Renderer = vtkSmartPointer<vtkRenderer>::New();
            Renderer->SetBackground(25/255.,50/255.,100./255.);
            
            vtkSmartPointer<vtkMoleculeMapper> molmapper = vtkSmartPointer<vtkMoleculeMapper>::New();
            
            molmapper->SetInputData(mol);
            molmapper->UseBallAndStickSettings();
            
            vtkSmartPointer<vtkActor> Actor = vtkSmartPointer<vtkActor>::New();
            Actor->SetMapper(molmapper);
            Renderer->AddActor(Actor);
            
            renWin = vtkSmartPointer<vtkRenderWindow>::New();;
            renWin->AddRenderer( Renderer );
            renWin->SetSize( 400, 400 );
            
            renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
            renderWindowInteractor->SetRenderWindow(renWin);
            
            renWin->Render();/*
                              vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
                              vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
                              
                              iren->SetInteractorStyle(style);
                              */
            renderWindowInteractor->Initialize();
            renderWindowInteractor->Start();
            std::cout << "Window 1 closed..." << std::endl;
            
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msVisualizerZMat::createVtkMolecule(vtkSmartPointer<vtkMolecule> molecule) {
            
            vector<vtkAtom> atomsVtk;
            
            for(msEntity::ElementItr it= ZMat->beginElements();
                it!= ZMat->endElements();it++) {
                
                msAtom* msatom = (msAtom*)( (*it).get() );
                Eigen::Vector3d pos=msatom->getPosition();
                atomsVtk.push_back( molecule->AppendAtom(msatom->AtomicNb,pos[0],pos[1],pos[2]) );
            }
            
            msEntity::ElementItr it1=ZMat->beginElements();
            msEntity::ElementItr it2=ZMat->beginElements();
            int order=0;
            
            for( int i=0; it1 != ZMat->endElements(); it1++,i++ ){
                
                msAtom* atom1 =(msAtom*)( (*it1).get() );
                int j=i;
                
                for( it2=it1; it2 != ZMat->endElements() ; it2++,j++) {
                    
                    msAtom* atom2 = (msAtom*)( (*it2).get() );
                    
                    if( (order=isBondedByCovalent(atom1,atom2))>0 )
                        molecule->AppendBond( atomsVtk[i], atomsVtk[j], isBondedByCovalent(atom1,atom2) );
                }
            }
            
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msVisualizerZMat::addToRenderer(vtkSmartPointer<vtkRenderer> renderer){
            
            vtkSmartPointer<vtkMolecule> molecule = vtkSmartPointer<vtkMolecule>::New();
            
            createVtkMolecule(molecule);
            
            renderer->SetBackground(25/255.,50/255.,100./255.);
            
            vtkSmartPointer<vtkMoleculeMapper> molmapper = vtkSmartPointer<vtkMoleculeMapper>::New();
            
            molmapper->SetInputData(molecule);
            molmapper->UseBallAndStickSettings();
            
            vtkSmartPointer<vtkActor> Actor = vtkSmartPointer<vtkActor>::New();
            Actor->SetMapper(molmapper);
            renderer->AddActor(Actor);
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        int msVisualizerZMat::isBondedByCovalent(msAtom* a1,msAtom* a2) {
            
            if(a1==a2) return 0;
            Eigen::Vector3d v = a1->getPosition()-a2->getPosition();
            int order=0;
            for( int k=0; k<3 ; k++) {
                
                double covlength = 1.1 * getCovalentLength(k,*a1,*a2);
                if( v.norm()<covlength) order=k;
            }
            return order;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        
        void msVisualizerZMat::launch(boost::shared_ptr<msZMat> zmt){
            
            LOGGER_ENTER_FUNCTION("void msVisualizerZMat::launch(boost::shared_ptr<msZMat> zmt)"
                                  ,getFullId());
            
            ZMat = zmt;
            
            Renderer =  vtkSmartPointer<vtkRenderer>::New();
            Renderer->SetBackground(25./255.,50./255.,100/255.); // Background color white
            
            renWin = vtkSmartPointer<vtkRenderWindow>::New();
            renWin->AddRenderer(Renderer);
            renWin->Render();
            
            renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
            renderWindowInteractor->SetRenderWindow(renWin);
            renderWindowInteractor->Initialize();
            vtkStyleInteract = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
            renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, this);
            renderWindowInteractor->CreateRepeatingTimer(10);
            
            LOGGER_WRITE(msLogger::DEBUG, "Start render window interactor");
            TimerCount = 0;
            //renderWindowInteractor->Initialize();
            renderWindowInteractor->Start();
            
            std::cout << "Window 1 closed..." << std::endl;
            
            LOGGER_EXIT_FUNCTION();
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msVisualizerZMat::Execute( vtkObject *caller, unsigned long eventId,
                                       void * vtkNotUsed(callData) )
        {   if( !ZMat ) return;
            
            if (vtkCommand::TimerEvent == eventId) {
                
                if (vtkStyleInteract == 0) renderWindowInteractor->SetInteractorStyle(vtkStyleInteract);
                
                vtkSmartPointer<vtkRenderWindow> renderWindow = renderWindowInteractor->GetRenderWindow();
                vtkSmartPointer<vtkRenderer>     renderer     = renderWindow->GetRenderers()->GetFirstRenderer();
                clearRendered(renderer);
                renderer->SetBackground(.3, .6, .3);
                addToRenderer( renderer );
                renderWindow->Render();
                TimerCount++;
            }
            LOGGER_EXIT_FUNCTION();
            
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msVisualizerZMat::clearRendered(vtkRenderer* Renderer) {
            
            vtkActorCollection *Actors = Renderer->GetActors();
            vtkActor *Actor;
            for( Actors->InitTraversal(); (Actor = Actors->GetNextItem())!=NULL; )
                Renderer->RemoveActor( Actor );
            
            vtkActor2DCollection *Actors2D = Renderer->GetActors2D();
            vtkActor2D *Actor2D;
            for( Actors2D->InitTraversal(); (Actor2D = Actors2D->GetNextItem())!=NULL; )
                Renderer->RemoveActor2D( Actor2D );
            Renderer->Clear();
        }
#endif
    }
}
