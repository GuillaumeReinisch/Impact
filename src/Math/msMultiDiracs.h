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


#ifndef msMultiDiracs_H
#define msMultiDiracs_H


#include <msDiracs.h>

namespace impact {
    
    
    namespace math{
        
        /*!  \class msMultiDiracs
         * \brief A container of Diracs scalar functions that share the same samples of the configurational space
         *
         * A multi-dimensional diracs object is used to define multiple values for particular state of 
         * a configurational space (i.e. at particular value of variables it associates multiple scalar
         * values).
         *
         *
         */
        class msMultiDiracs : public msTreeMapper
        {
            
            static msRegistrar Registrar;
            static bool  isMultiDiracRegisteredInPython;
            static msRegister* createInstance() { return new msMultiDiracs; }
            
        protected:
            
            void registryInPython();
            
            void initialize()  {
                msTreeMapper::initialize();
                
                declareChild<msSamplerExplicit>(Sampler,msSamplerExplicit::New(),"Sampler");
                declareChild<msGeneralizedCoordinates>(Coordinates,msGeneralizedCoordinates::New(),"Coordinates");
                declareChildren<msDiracs>(Diracs,"Diracs");
            };
            
            void update(){  
	      
	      msTreeMapper::update(); 
	      IndexFromId.clear();
	      
	      for( size_t i = 0; i<Diracs.size();++i)
		 IndexFromId[Diracs[i]->getId()] = i;
	    }
            
        public:
            
            msMultiDiracs() : msTreeMapper(){
                
                constructVar("msMultiDiracs","msMultiDiracs","multiple dirac");
            }
            
            static boost::shared_ptr<msMultiDiracs> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msMultiDiracs> msMultiDiracs::New()","");
                LOGGER_WRITE(msLogger::DEBUG, "New msMultiDiracs object")
                
                boost::shared_ptr<msMultiDiracs> T( new msMultiDiracs );
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msMultiDiracs> msMultiDiracs::New()");
                return T;
            }
            
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                
                boost::shared_ptr<msMultiDiracs> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            /*! \brief set the explicit sampler and the coordinates
             *
             * \param sampler sampler to set
             */
            boost::shared_ptr<msTreeMapper> setSampler(boost::shared_ptr<msSamplerExplicit> sampler);
          
            /*! \brief return the sampler
             */
            boost::shared_ptr<msSamplerExplicit> getSampler() const {
                
                return Sampler.getSharedPtr();
            };
          
            /*! \brief add a dirac
             *
             * Add a dirac and set its id to a particular name. Number of values 
             * has to be the number of samples in the sampler. Before calling this 
             * method, the sampler has to be already set.
             *
             * \param values values of the dirac function for each sample
             * \param name id of the dirac function
             */
            boost::shared_ptr<msTreeMapper> addDirac(vector<double> values, string name);
            
            /*! \brief return the Diracs scalar functions in a vector.
             */
            vector<boost::shared_ptr<msDiracs> > getDiracsFunctions() const {
                
                return Diracs.getElements();
            }
            
            /*! \brief return the coordinates.
             */
            boost::shared_ptr<msGeneralizedCoordinates> getCoordinates() const {
                
                return Coordinates.getSharedPtr();
            }
            
            /*!\brief return the value of a Diracs function for the current sample of the sampler
             *
             *\param i index of the Diracs function
             */
            double getValueById( string id) const {
                
	        map<string,size_t>::const_iterator it = IndexFromId.find(id);
		
		if( it == IndexFromId.end() ) {
		     msException e("the Dirac of id "+id+"can not be found",
		             "double msMultiDiracs::getValueById( string id) const ",
			     getFullId());
		   IMPACT_THROW_EXCEPTION(e);
		}
				  
                return Diracs[(*it).second]->evaluate();
            }
            
            /*!\brief return the value of a Diracs function for the current sample of the sampler
             *
             *\param i index of the Diracs function
             */
            double getValue( size_t i) const {
                
                return Diracs[i]->evaluate();
            }
            
            /*!\brief return the values of all the Diracs function for the current sample of the sampler in a vector
             */
            vector<double> getValues() const {
                
                vector<double> values;
                for(size_t i =0 ; i<Diracs.size(); ++i){
                    
                    values.push_back(Diracs[i]->evaluate());
                }
                return values;
            }
            
            ostream&  print(ostream& out) const {
                return out;
            }
            
        private:
            
            msChildren<msDiracs> Diracs;
            
            msChild<msSamplerExplicit> Sampler;
            
            msChild<msGeneralizedCoordinates> Coordinates;
            
	    map<string,size_t> IndexFromId;
        };
        
    }
}

#endif