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
 * \file msSamplerRandom.h
 * \brief An explicit sampler
 * \author G.Reinisch
 * \date 2013
 */
#ifndef MSSAMPLERRND_H
#define MSSAMPLERRND_H

#include <msSampler.h>

namespace impact {
    
    
    namespace math{
        
        /** \class msSamplerRandom
         * \brief Random sampler
         *
         * The random sampler draw randomly a fixed number of samples.
	 * 
         * You can construct it by setting the number of sample to draw. \n
         * example:
         * ~~~~~~~~~~~~~{.py}
         from libimpact import *
         
         sampler= msSamplerRandom.New()
         sampler.setNoOfSample(100)
         * ~~~~~~~~~~~~~
         */
        class msSamplerRandom : public msSampler
        {
            
        protected:
            
            static msRegistrar Registrar;
            static bool  isSamplerRandomRegisteredInPython;
            static msRegister* createInstance() { return new msSamplerRandom; }
            void registryInPython();
         
            
        public:
            
	    msSamplerRandom() : msSampler() {
                
                constructVar("msSamplerRandom","SamplerRandom","random sampler");
            }
            
            void update()  { msSampler::update();
            };
            
            void initialize() {  msSampler::initialize();
                
		declareAttribute< size_t >(NoOfSamples,"NoOfSamples");
            };
            
            static boost::shared_ptr<msSamplerRandom> New(){
                
                boost::shared_ptr<msSamplerRandom> T( new msSamplerRandom );
                T->initialize(); T->update();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msSamplerRandom> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            /*! \brief return the number of sample.
             */
             boost::shared_ptr<msTreeMapper> setNoOfSamples(size_t n) {
                
                NoOfSamples = n;
		return mySharedPtr();
            }
           
            /*! \brief return the number of sample.
             */
            size_t getNoOfSamples() const{
                
                return NoOfSamples;
            }
            
            bool getNextPoint(boost::shared_ptr<msGeneralizedCoordinates> coor);
            
            
            std::ostream& print(std::ostream&) const;

            
        private:
                        
	    size_t NoOfSamples;  
        };
    }
}
#endif // MSDIRACS_H
