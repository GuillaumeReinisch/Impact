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


#ifndef MSSAMPLER_H
#define MSSAMPLER_H

#include<msScalarFunction.h>

namespace impact
{
    
  /*! \class msConfiguration
     * \brief Class to save a configuration
     *
     */
    class msConfiguration : public msTreeMapper
    {
        friend class msScalarFunction;
        
    protected:
                //!\name from msRegister
        //@{
        static msRegistrar Registrar;
        static bool  isConfigurationRegisteredInPython;
        static msRegister* createInstance() { return new msConfiguration; }
        void registryInPython();
        //@}
        
    public:
        msConfiguration() : msTreeMapper(){
            
            constructVar("msConfiguration","Configuration","Configuration");
        }
        
        void update()  { msTreeMapper::update();
        };
        
        void initialize() {
            
            msTreeMapper::initialize();
	    msTreeMapper::declareChild< msGeneralizedCoordinates> (Coordinates,msGeneralizedCoordinates::New(),"Coordinates");
	    msTreeMapper::declareAttribute< vector<double> > (Configuration,"Configuration");
	    msTreeMapper::declareAttribute< map<string, double> >(ValuesRecorded,"ValuesRecorded");
        };
        
        static boost::shared_ptr<msConfiguration> New(){
            
            boost::shared_ptr<msConfiguration> T( new msConfiguration );
            LOGGER_ENTER_FUNCTION_DBG("msConfiguration::New()",T->getFullId());
            T->initialize(); T->update();
            LOGGER_EXIT_FUNCTION();
            return T;
        }
        
        virtual boost::shared_ptr<msTreeMapper> clone() {
            boost::shared_ptr<msConfiguration> clone = New();
            clone->msTreeMapper::operator=(*this);
            return clone;
        }
        
        boost::shared_ptr<msTreeMapper> setCoordinates(boost::shared_ptr<msGeneralizedCoordinates> coors) {
            
	    if(Coordinates.getSharedPtr() != coors) {
	      
	      msTreeMapper::update<msGeneralizedCoordinates>(Coordinates,coors);
	    }
	    
            Configuration.resize(coors->noOfDim(),0);
	    
            for(size_t i=0;i<coors->noOfDim(); i++){
	      
                Configuration[i] = coors->getVariable(i)->getValue();
	    }
	    return mySharedPtr();
        };
	
	boost::shared_ptr<msGeneralizedCoordinates> getCoordinates() const {
	  
	    return Coordinates.getSharedPtr();
	}
	
        boost::shared_ptr<msTreeMapper> addValue(string key , double value ) {
            
            ValuesRecorded[key] = value;
	    return mySharedPtr();
        };
        
	map<string, double> getValues() {
            
            return ValuesRecorded;
        };
	
	vector<double> getConfiguration() {
            
            return Configuration;
        };
        
    private:
           
        msChild<msGeneralizedCoordinates> Coordinates;
	
        vector<double> Configuration;
        
        map<string, double> ValuesRecorded;
        
    };
    
    /*! \class msSampler
     * \brief Virtual base class of sampler
     *
     * A sampler is used to generate configurations in a configurational space defined
     * by a generalized coordinates container (msGeneralizedCoordinates) and to study
     * the behavior of scalar function over that space.
     *
     * Clever sampling strategy can be defined by derived classes to scan the configurational
     * space; the scanning strategy is implemented by defining the virtual function
     * msSampler::getNextPoint.
     * Also the redefinition of the methods msSampler::sample and msSampler::scalarProduct
     * may be considered if the serial sampling strategy using the getNextPoint implemented
     * here is not suited.
     */
    class msSampler : public msTreeMapper
    {
        friend class msScalarFunction;
        
    protected:
        
        class msSamplingData{
            
        public:
            
            double Average,Integral,Max,Min;
            
            vector<double> posMinimum;
            vector<double> posMaximum;
            
            double getMaximum() {return Max;}
            double getMinimum() {return Min;}
            
            void consider(boost::shared_ptr<msGeneralizedCoordinates> coors, double value){
                
                if (value>Max){
                    Max = value;
                    saveConfiguration(*coors,posMaximum);
                }
                if (value<Min){
                    Min = value;
                    saveConfiguration(*coors,posMinimum);
                }
            }
            
            void saveConfiguration(msGeneralizedCoordinates& coors , vector<double>& pos) {
                
                pos.resize(coors.noOfDim());
                for(size_t i=0;i<coors.noOfDim(); i++){
                    
                    pos[i] = coors.getVariable(i)->getValue();
                }
            }
            vector<double> getMaximumConfiguration() {return posMaximum;}
            vector<double> getMinimumConfiguration() {return posMinimum;}
            
            msSamplingData(){Max=-1e30;Min=1e30;Average=0;Integral=0;}
            
            msSamplingData operator*(double a){Average*=a;Integral*=a;Max*=a;Min*=a;return *this;}
            
            std::string abstract(){ std::stringstream out;out<<" Min="<<Min<<" Max="<<Max<<" Average="<<Average<<" Integral="<<Integral; return out.str();}
        };
        
        //!\name from msRegister
        //@{
        static msRegistrar Registrar;
        static bool  isSamplerRegisteredInPython;
        static msRegister* createInstance() { return new msSampler; }
        void registryInPython();
        //@}
        
    public:
                
        msSampler() : msTreeMapper(){
            
            constructVar("msSampler","Sampler","sampler");
            LogStep = 10;
            nScanned=0;
        }
        
        void update()  { msTreeMapper::update();
        };
        
        void initialize() {
            
            msTreeMapper::initialize();
            msTreeMapper::declareChild<msGeneralizedCoordinates>(Coordinates,msGeneralizedCoordinates::New(),"Coordinates");
	    msTreeMapper::declareChild<msScalarFunction>(Field,msScalarFunction::New(),"Field");
            msTreeMapper::declareAttribute<size_t>(LogStep,"LogStep");
            msTreeMapper::declareChildren<msConfiguration>(RecordedConfigurations,"RecordedConfigurations");
        };
        
        static boost::shared_ptr<msSampler> New(){
            
            boost::shared_ptr<msSampler> T( new msSampler );
            LOGGER_ENTER_FUNCTION_DBG("msSampler::New()",T->getFullId());
            T->initialize(); T->update();
            LOGGER_EXIT_FUNCTION();
            return T;
        }
        
        virtual boost::shared_ptr<msTreeMapper> clone() {
            boost::shared_ptr<msSampler> clone = New();
            clone->msTreeMapper::operator=(*this);
            return clone;
        }
        
        /*! \brief set the coordinates defining the configurational space
         *
         * When called, the recorded configurations are erased.
         *
         *\param Coordinates generalized coordinates container
         */
        boost::shared_ptr<msTreeMapper> setCoordinates(boost::shared_ptr<msGeneralizedCoordinates> Coordinates);
        

        /*! \brief return the configurational space
         */
        boost::shared_ptr<msGeneralizedCoordinates> getCoordinates(){
            
            return Coordinates.getSharedPtr();
        }
        
        /*! \brief set the field sampled when the method msSampler::sample() is called
         *
         *\param field field
         */
        boost::shared_ptr<msTreeMapper> setField(boost::shared_ptr<msScalarFunction> field);
        

	 /*! \brief set the logstep when the method msSampler::sample is called
	  * 
	  * The sample properties is displayed every 'n' step. 
	  * \param n period of logging
         */
	boost::shared_ptr<msTreeMapper> setLogStep(size_t n){ LogStep=n; return mySharedPtr(); }
	
        /*! \brief return the configurational space
         */
        std::vector<boost::shared_ptr<msConfiguration> > getConfigurations(){
            
            return RecordedConfigurations.getElements();
        }
        
        /*! \brief set the 'next' configuration in a generalized coordinates container
         *
         * This method needs to be overiden by derived class and set a configuration in 'coors'.
         * \param coors generalized coordinates container
         */
        virtual bool getNextPoint(boost::shared_ptr<msGeneralizedCoordinates> coors){
            
            IMPACT_THROW_EXCEPTION( msException("The function getNextPoint() is not implemented in this class, you need to use derived class"
                                           ,"msSamplingData msSampler::getNextPoint(boost::shared_ptr<msGeneralizedCoordinates> coors)", getFullId()) );
        }

        
         bool getNextPoint(){
            
	    if(!getCoordinates()) {
                IMPACT_THROW_EXCEPTION( msException("The coordinates have not been set yet."
                                           ,"msSamplingData msSampler::getNextPoint()", getFullId()) );
	    }
	    return getNextPoint(getCoordinates());
        }
        /*! \brief generate a random configuration
         *
         * This method needs to be overiden by derived class and set a configuration in 'coors'.
         * \param coors generalized coordinates container
         *//*
        virtual boost::shared_ptr<msTreeMapper> getRndPoint(boost::shared_ptr<msGeneralizedCoordinates> coors){
            
            Coordinates->setRandom();
            return mySharedPtr();,msConfiguration::New(),
        }*/
        
        /*! \brief sample a scalar function and return information in a msSamplingData container
         *
         * By default it uses a serial sampling using the getNextPoint method.
         * You can overide this method if you want a different behavior.
         * \param fct scalar function to sample
         */
        virtual msSampler::msSamplingData sample( msScalarFunction& fct);
        
        
        /*! \copydoc sample( msScalarFunction& fct);
         */
        msSampler::msSamplingData sample() {
             return sample(*Field);
        };
        
        /*! \copydoc sample( msScalarFunction& fct, const msUnitsManager& units);
         */
        msSampler::msSamplingData sample( boost::shared_ptr<msScalarFunction> fct,boost::shared_ptr<msUnitsManager> units) {
            
            return sample(*fct,*units);
        };
        
        /*! \brief same as msSampler::sample(msScalarFunction) but results are in the providen units system
         *
         * \param units units system
         */
        msSampler::msSamplingData sample( msScalarFunction& fct , const msUnitsManager& units);
        
        
        std::ostream& print(std::ostream&) const;
        
        /*! \brief return the scalar product between 2 scalar functions
         *
         * By defaulsize_tt it uses a serial sampling using the getNextPoint method.
         * You can overide this method if you want a different behavior.
         */
        virtual double scalarProduct(msScalarFunction& fct1 ,msScalarFunction& fct2);
        
        virtual void begin() { nScanned=0;}
        
        size_t noOfScanned(){return nScanned;}
        
    protected:
        
        int 	nScanned;      //!< number of point already Scanned
        
    private:
        
        msChild<msGeneralizedCoordinates> Coordinates;
	msChild<msScalarFunction> Field;
        
        size_t 	LogStep;       //!< Step to display the integration point when Integrate() is called
        
       msChildren<msConfiguration> RecordedConfigurations;
        
    };
    
    std::ostream& operator<<(std::ostream& out,const msSampler& sampler);
}

#endif // MSSAMPLER_H
