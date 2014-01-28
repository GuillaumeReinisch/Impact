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


#ifndef MSDIRACS_H
#define MSDIRACS_H

#include <msScalarFunction.h>
#include <msSamplerExplicit.h>

namespace impact {
    
    
    namespace math{
        
        
        /** \class msDiracs
         * \brief A dirac scalar function
         *
         * A dirac scalar function associates a scalar values to particular configurations.
         * The particular configurations are defined using an explicit sampler (msExplicitSampler),
         * and all the msDiracs::evaluate methods return the scalar value associated to the
         * current sample of the sampler. You can scan the samples using the msSampler::getNextPoint
         * method. This object is for instance used to approximate scalar field using a set of data
         * points (see msReproModel).
         *
         * To construct the dirac function you can either do it from a data file (msDiracs::load),
         * or by defining the sampler and then the values associated. \n
         * example:
         * ~~~~~~~~~~~~~{.py}
         from libimpact import *
         
         sampler= msSamplerExplicit.New()
         values = msVector_double()
         for i in range(0,10):
            for j in range(0,2):
                for k in range(0,4):
                    sample = msVector_double()
                    sample.append(i)
                    sample.append(j)
                    sample.append(k)
                    sampler.addSample(sample)
                    values.append(i+j+k)
         
         coors = msGeneralizedCoordinates.New()
         coors.addVar(msScalarVariable(0,-10,10,0,0).setId("q0")
         coors.addVar(msScalarVariable(0,-10,10,0,0).setId("q1")
         coors.addVar(msScalarVariable(0,-10,10,0,0).setId("q2")
         
         diracs = msDiracs.New()
         diracs.setCoordinates(coors)
         diracs.setDiracSampler(sampler)
         diracs.setValues(values)
         
         sampler.begin()
         while sampler.getNextPoint(coors):
            print diracs.evaluate()
            #you can access the coordinates values using the object coors
         * ~~~~~~~~~~~~~
         */
        class msDiracs : public msScalarFunction
        {
        private:
            
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isDiracsRegisteredInPython;
            static msRegister* createInstance() { return new msDiracs; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void update()  {msScalarFunction::update();
            };
           
            
        public:
	  
	  
            msDiracs() : msScalarFunction(){
                constructVar("msDiracs","Diracs","a dirac function");
            }
            
            static boost::shared_ptr<msDiracs> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("msScalarFunction::New(boost::shared_ptr<msGeneralizedCoordinates> unit)","");
                
                boost::shared_ptr<msDiracs> T( new msDiracs );
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION2("msScalarFunction::New(boost::shared_ptr<msGeneralizedCoordinates> unit)");
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msScalarFunction> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            void initialize()  {
                
                msScalarFunction::initialize();
                
                declareChild<msSamplerExplicit >(Sampler,
                                                 msSamplerExplicit::New(),
                                                 "Sampler");
                declareAttribute<vector<double> >(Values,"Values");
            };
            
            
            /*! \brief set the explicit sampler 
             *
             * The explicit sampler defines the configurations associated to non zero value.
             * \param sampler explicit sampler
             */
            boost::shared_ptr<msTreeMapper> setDiracSampler(boost::shared_ptr<msSamplerExplicit> sampler){
                
                LOGGER_ENTER_FUNCTION("boost::shared_ptr<msTreeMapper> msDirac::setDiracSampler(boost::shared_ptr<msSamplerExplicit> sampler)",
                                      getFullId());
                
                msTreeMapper::update(Sampler,sampler);
                
                LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msDirac::setDiracSampler(boost::shared_ptr<msSamplerExplicit> sampler)");
                return mySharedPtr();
            }
            
            /*! \brief set the values of each sample of the sampler
             *
             * The sampler has to be set before calling this function.
             * \param values values associated to each sample of the sampler
             */
            boost::shared_ptr<msTreeMapper> setValues(vector<double> values){
                
                LOGGER_ENTER_FUNCTION("boost::shared_ptr<msTreeMapper> msDirac::setValues(vector<double> values)",
                                      getFullId());
            
                if(! Sampler.getSharedPtr() ) {
                    
                    msException e("You have to set the sampler before adding dirac function",
                              "boost::shared_ptr<msTreeMapper> msDirac::setValues(vector<double> values)",
                              getFullId());
                    IMPACT_THROW_EXCEPTION(e);
                }
                
                if(values.size()!=Sampler->noOfSamples()){
                    
                    stringstream out;
                    out<<"The size of the vector of values provided ("<<values.size()
                    <<") does not match the number of sample defined in the sampler"
                    <<" ("<<Sampler->noOfSamples()<<").";
                    
                    msException e(out.str(),
                              "boost::shared_ptr<msTreeMapper> msDirac::setValues(vector<double> values)",
                              getFullId());
                    IMPACT_THROW_EXCEPTION(e);
                }
                Values=values;
                
                LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msDirac::setValues(vector<double> values)");
                return mySharedPtr();
            }
            
            /*! \brief construct the function from a data file
             *
             * The coordinates of the function have to be set before calling the
             * function, we call N the dimension of the coordinates.
             *
             * The first line of the file is not accounted; then each lines: \n
             * N values (coordinate's values) and the scalar field associated \n
             * and do it for each sample.
             * \filename file name
             */
            boost::shared_ptr<msTreeMapper> load(string filename);
            
            /*! \brief add a dirac to the function
             *
             * \param coors value of the coordinates
             * \param value value of the scalar field associated
             */
            void addDirac(const vector<double> coors,double value){
                
                Values.push_back(value);
                Sampler->addSample(coors);
            };
            
            
            //! return the sampler
            boost::shared_ptr<msSamplerExplicit> getSampler(){ return Sampler.getSharedPtr();
            }
            

            double evaluate(){
                
                return Values[Sampler->currentSampleIndex()];
            }
            
            //! \brief return the number of dirac defined
            size_t noOfValues() const {return Values.size();};
            
        private:
            
            msChild<msSamplerExplicit> Sampler;
            
            vector<double> Values;
        };
    }
}
#endif // MSDIRACS_H
