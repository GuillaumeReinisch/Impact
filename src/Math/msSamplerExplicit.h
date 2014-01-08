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

/*!
 * \file msSamplerExplicit.h
 * \brief An explicit sampler
 * \author G.Reinisch
 * \date 2013
 */
#ifndef MSSAMPLEREXPLICIT_H
#define MSSAMPLEREXPLICIT_H

#include <msSampler.h>
#include <msAttribute_Child.h>
#include <cstdlib>
#include <ctime>

namespace impact {
    
    
    namespace math{
        
        /** \class msSamplerExplicit
         * \brief Samples a predefined chain of coordinates values
         *
         * An explicit sampler is a sampler that contains predefined values of 
         * coordinates in a chain. It is used to generate sequential configurations
         * of a generalized coordinates container (msGeneralizedCoordinates) using 
         * the method msSamplerExplicit::getNextPoint.
         *
         * You can construct it by adding sequentially the coordinate's values
         * using the method msSamplerExplicit::addSample. \n
         * example:
         * ~~~~~~~~~~~~~{.py}
         from libimpact import *
         
         sampler= msSamplerExplicit.New()
         
         for i in range(0,10):
            for j in range(0,2):
                for k in range(0,4):
                    sample = msVector_double()
                    sample.append(i)
                    sample.append(j)
                    sample.append(k)
                    sampler.addSample(sample)
         * ~~~~~~~~~~~~~
         */
        class msSamplerExplicit : public msSampler
        {
            
        protected:
            
            static msRegistrar Registrar;
            static bool  isSamplerExplicitRegisteredInPython;
            static msRegister* createInstance() { return new msSamplerExplicit; }
            void registryInPython();
         
            
        public:
            
	    msSamplerExplicit() : msSampler() {
                
                constructVar("msSamplerExplicit","SamplerExplicit","explicit sampler");
                CurrentIndex=0;
            }
            
            void update()  { msSampler::update();
            };
            
            void initialize() {  msSampler::initialize();
                
                declareAttribute< size_t >(CurrentIndex,"CurrentIndex");
                declareAttribute< vector< vector<double> > >(Samples,"Samples");
            };
            
            static boost::shared_ptr<msSamplerExplicit> New(){
                
                boost::shared_ptr<msSamplerExplicit> T( new msSamplerExplicit );
                T->initialize(); T->update();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msSamplerExplicit> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            /*! \brief set the current sample's index.
             */
            boost::shared_ptr<msTreeMapper> setSampleIndex(size_t i){
                
                CurrentIndex = i;
                return mySharedPtr();
            }
            
            /*! \brief return the index of the current sample.
             */
            size_t currentSampleIndex() const {
                
                return CurrentIndex;
            }
            /*! \brief return the number of sample.
             */
            size_t noOfSamples() const{
                
                return Samples.size();
            }
            
            /*! \brief return the dimension of a sample.
             */
            size_t noOfDim() const {
                
                if ( noOfSamples()>0 ) {
                    
                    return Samples[0].size();
                }
                msError e("no sample has been defined yet",
                          "size_t msSamplerExplicit::noOfDim() const",
                          getFullId());
                BOOST_THROW_EXCEPTION(e);
            }
            /*! \brief iterate the current sample
             *
             * This method is similar to the msSamplerExplicit::getNextPoint, 
             * but the generated configuration is not apply to a generalized
             * coordinates container.
             */
            bool iterate(){
                
                if( CurrentIndex==Samples.size()-1 ){ CurrentIndex=0; return 0;}
                
                CurrentIndex++;
                return 1;
            };
            
            bool getNextPoint(boost::shared_ptr<msGeneralizedCoordinates> coor);
            
            boost::shared_ptr<msTreeMapper> getRndPoint(boost::shared_ptr<msGeneralizedCoordinates> coor);
            
            /*! \brief set the coordinate container to the current sample index
             *
             */
            boost::shared_ptr<msTreeMapper> setCurrentSample(){
                
                getCoordinates()->setCoordinates(Samples[CurrentIndex]);
                return mySharedPtr();
            }
            
            /*! \brief set the coordinate to a particular sample of the chain
             *
             * \param i index of the sample
             */
            boost::shared_ptr<msTreeMapper> setSample(size_t i){
                
                if(i>=Samples.size())
                    BOOST_THROW_EXCEPTION(msError("indice out of range",
                                                  "boost::shared_ptr<msTreeMapper> setSample(boost::shared_ptr<msGeneralizedCoordinates> coor,size_t i)",
                                                  getFullId())
                                          );
                getCoordinates()->setCoordinates(Samples[i]);
                CurrentIndex = i;
                return mySharedPtr();
            }
            
            /*! \brief set the variable's range of a generalized coordinate
             *
             * Set each variable minimum and maximum to the associated minimum and maximum
             * defined in the samples, then set the coordinates to the first sample.
             *
             * \param coor generalized coordinates container
             */
            boost::shared_ptr<msTreeMapper> setCoordinatesRange(){
                
                if ( noOfSamples() == 0 ) {
                    
                    msError e("no sample has been defined yet",
                          "boost::shared_ptr<msTreeMapper> msSamplerExplicit::setCoordinatesRange(boost::shared_ptr<msGeneralizedCoordinates> coor)",
                          getFullId());
                    BOOST_THROW_EXCEPTION(e);
                }
                if( noOfDim() != getCoordinates()->noOfDim() ) {
                    
                    msError e("dimension of the coordinates and the samples different",
                              "boost::shared_ptr<msTreeMapper> msSamplerExplicit::setCoordinatesRange(boost::shared_ptr<msGeneralizedCoordinates> coor)",
                              getFullId());
                    BOOST_THROW_EXCEPTION(e);
                }
                
                for(size_t i=0;i<getCoordinates()->noOfDim();i++) {
                    
                    double min=Samples[0][i];double max=Samples[0][i];
                    
                    for(size_t j=0 ; j< Samples.size(); j++) {
                        
                        if( Samples[j][i]<min) min=Samples[j][i];
                        if( Samples[j][i]>max) max=Samples[j][i];
                    }
                    getCoordinates()->getVariable(i)->setMin(min);
                    getCoordinates()->getVariable(i)->setMax(max);
                }
                
                setSample(0);
                
                return mySharedPtr();
            }
            
            
            std::ostream& print(std::ostream&) const;

            /*! \brief add a sample to the sampler
             *
             * The sample size has to be the same as the previous sample, except for the first one.
             *
             * \param sample vector of the sample's values
             */
            boost::shared_ptr<msTreeMapper> addSample(vector<double> sample) {
                
                if( (Samples.size()>0) && (Samples[Samples.size()-1].size()!=sample.size()))
                    BOOST_THROW_EXCEPTION(msError("sample size not consistent with previous sample",
                                                  "boost::shared_ptr<msTreeMapper> msSamplerExplicit::addSample(vector<double> sample)",
                                                  getFullId())
                                          );
                Samples.push_back(sample);
                return mySharedPtr();
            }
            
            
            boost::shared_ptr<msTreeMapper> clear(){ Samples.clear();return mySharedPtr();}
            
        private:
            
            size_t CurrentIndex;
            
            vector< vector<double> >  Samples;
        };
    }
}
#endif // MSDIRACS_H
