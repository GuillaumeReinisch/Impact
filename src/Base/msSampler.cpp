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


#include "msSampler.h"


namespace impact
{
    bool msConfiguration::isConfigurationRegisteredInPython=0;
    msRegistrar msConfiguration::Registrar("msConfiguration", msConfiguration::createInstance);
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msConfiguration::registryInPython() {
      
#if USE_PYTHON
        msTreeMapper::registryInPython();
        
        if( ! msConfiguration::isConfigurationRegisteredInPython ) {
            
            using namespace boost::python;
            
            class_<msConfiguration,bases<msTreeMapper>,boost::shared_ptr<msConfiguration> >
            ("msConfiguration","store a particular configuration with some values",init<>()
             )
	    .def( "New",&msConfiguration::New ,"Return a shared pointer to a new object" )
            .staticmethod("New")
	    .def("getValues",&msConfiguration::getValues,
                 "return the values stored.")
	    .def("getConfiguration",&msConfiguration::getConfiguration,
                 "return the configuration coordinates.")	    
	    ;
	    msTreeMapper::finalizeDeclarationForPython<msConfiguration>("msConfiguration");
	                
            msConfiguration::isConfigurationRegisteredInPython=1;
        }
#endif
    }
   
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    	    
    bool msSampler::isSamplerRegisteredInPython=0;
    msRegistrar msSampler::Registrar("msSampler", msSampler::createInstance);
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::ostream& operator<<(std::ostream& out,const msSampler& sampler)
    {
        sampler.print(out);
        return out;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msSampler::registryInPython()
    {
#if USE_PYTHON
        msRegister::registryInPython();
        
        if( ! msSampler::isSamplerRegisteredInPython ) {
            
            using namespace boost::python;
            class_<msSampler::msSamplingData,boost::shared_ptr<msSampler::msSamplingData> >
            ("msSamplingData","store sampling statistics",init<>()
             );/*
            .def("getMinimum",&msSampler::msSamplingData::getMinimum,
                 "return the minimum")
            .def("getMinimumConfiguration",&msSampler::msSamplingData::getMinimumConfiguration,
                 "return the minimum's configuration")
            .def("getMaximum",&msSampler::msSamplingData::getMaximum,
                 "return the maximum")
            .def("getMaximumConfiguration",&msSampler::msSamplingData::getMaximumConfiguration,
                 "return the maximum's configuration");
            */
            class_<msSampler,bases<msTreeMapper>,boost::shared_ptr<msSampler> >
            ("msSampler",
             "(Virtual) base class for sampler",no_init
             )
            .def("setCoordinates",&msSampler::setCoordinates,
                 "set the coordinates (configurational space) to sample. arg2: coordinates.")
            .def("getCoordinates",&msSampler::getCoordinates,
                 "return the coordinates.")
	    .def("setField",&msSampler::setField,
                 "add a field to study. arg2: field.")
	    .def("getConfigurations",&msSampler::getConfigurations,
                 "return the configurations.")
            .def("sample", (msSampler::msSamplingData (msSampler::*)( ) )&msSampler::sample,
                 "sample the scalar function function.")
            .def("getNextPoint",  (bool (msSampler::*)() ) &msSampler::getNextPoint,
                 "set the next point of generalized coordinates after the current one (implementation depends on the derived class). arg2: generalized coordinates")
            .def("scalarProduct",   &msSampler::scalarProduct,
                 "compute the scalar product between two scalar functions. arg2: scalar function 1, arg3: scalar function 2")
            .def("begin",   &msSampler::begin,
                 "Initialize the sampler to begin the configurational scan")
	    .def("setLogStep",&msSampler::setLogStep,
		 "set the log verbosity when the method 'sample' is used. arg2: period of logging")
            .def(self_ns::str(self_ns::self));
            
            
            msTreeMapper::finalizeDeclarationForPython<msSampler>("msSampler");
            
            
            msSampler::isSamplerRegisteredInPython=1;
        }
#endif
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    boost::shared_ptr<msTreeMapper> msSampler::setCoordinates(boost::shared_ptr<msGeneralizedCoordinates> coors) {
        
        if(!coors->hasParent()) coors->setAffiliation(mySharedPtr());
        msTreeMapper::update(Coordinates,coors);
        
        msTreeMapper::clearChildren(RecordedConfigurations);
        
        return mySharedPtr();
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
     boost::shared_ptr<msTreeMapper> msSampler::setField(boost::shared_ptr<msScalarFunction> field) {
        
        if( getCoordinates()->noOfDim()==0 ) {
	
	    setCoordinates(field->getCoordinates());
        }
   
	if( getCoordinates() != field->getCoordinates() ) {
	
	    msError e("You can  add  fields with different generalized coordinates",
	       "boost::shared_ptr<msTreeMapper> msSampler::addField(boost::shared_ptr<msScalarFunction> field)",
	       getFullId());
	    BOOST_THROW_EXCEPTION(e);
        }
	
        if(!field->hasParent()) field->setAffiliation(mySharedPtr());
        msTreeMapper::update<msScalarFunction>(Field,field);
          
        return mySharedPtr();
    }
     
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    msSampler::msSamplingData msSampler::sample( msScalarFunction& fct , const msUnitsManager& units) {
        
        setCoordinates( fct.getCoordinates() );
        if( Coordinates == boost::shared_ptr<msGeneralizedCoordinates>() )
            BOOST_THROW_EXCEPTION( msError("the scalar function providen do not have initialized coordinates"
                                           ,"msSamplingData msSampler::sample( msScalarFunction& fct , const msUnitsManager& units)",getFullId()) );
        
        msSampler::msSamplingData s=sample(fct);
                
        double fctConv  = units.convert(fct.getYUnit(),1);
        double coorsConv = Coordinates->getVolumeDq(units) /  Coordinates->getVolumeDq() ;
        return  s*fctConv*coorsConv;
    };
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
       
    msSampler::msSamplingData msSampler::sample(msScalarFunction& fct) {
        
        LOGGER_ENTER_FUNCTION_DBG("double msSampler::sample(msScalarFunction& fct)",
                                  getFullId());
                
	int width=10;
        std::stringstream comment;
        double result=0;
	double v=0;
        msSampler::msSamplingData data;
	
        setCoordinates(fct.getCoordinates());
        begin();
        
	msTreeMapper::clearChildren(RecordedConfigurations);
        boost::shared_ptr<msConfiguration> minOfField = msConfiguration::New();
        minOfField->setId("minimum");
        minOfField->setAffiliation(mySharedPtr());
        boost::shared_ptr<msConfiguration> maxOfField = msConfiguration::New();
        maxOfField->setId("maximum");
        maxOfField->setAffiliation(mySharedPtr());
        
        msGeneralizedCoordinates::iterator it=Coordinates->begin();
        
        for(;it!=Coordinates->end();++it) comment<<std::setw(width)<<(*it)->getId();
        comment<<std::setw(30)<<"field value"<<std::setw(30)<<"integral"<<endl;
        
        LOGGER_WRITE(msLogger::INFO,comment.str());
        
        while( getNextPoint(Coordinates.getSharedPtr()) )
        {
            result += v = fct.evaluate();
	    
            if( v<data.Min ) { 
	      
	        LOGGER_WRITE(msLogger::DEBUG,"update minimum of the field");
	        data.Min = v;
	        minOfField->setCoordinates( Coordinates.getSharedPtr() );
		minOfField->addValue("min",v);          
	    }
	    if( v>data.Max ) { 
	      
		LOGGER_WRITE(msLogger::DEBUG,"update maximum of the field");
	        data.Max = v;
	        maxOfField->setCoordinates( Coordinates.getSharedPtr() );
	      	maxOfField->addValue("max",v);          
	    }
            //data.consider(Coordinates, v );
            
           if( ( nScanned!=0) && (nScanned % LogStep) == 0 )
	    {
                
                comment.str("");
                for(it=Coordinates->begin();it!=Coordinates->end();++it) comment<<std::setw(width)<<(*it)->getValue();
                comment<<std::setw(30)<<v<<std::setw(30)<<result<<endl;
                
                LOGGER_WRITE(msLogger::INFO,comment.str());
            }
        }
        data.Average=result/nScanned;
        result*=Coordinates->getVolumeDq();
        data.Integral=result;
	
	addElementToChildren<msConfiguration>(RecordedConfigurations,minOfField);
	addElementToChildren<msConfiguration>(RecordedConfigurations,maxOfField);
	
        LOGGER_WRITE(msLogger::INFO,"Sampling done: "+data.abstract());
        LOGGER_EXIT_FUNCTION2("double msSampler::sample(msScalarFunction& fct)");
        return data;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    double msSampler::scalarProduct(msScalarFunction& fct1 ,msScalarFunction& fct2) {
        
        LOGGER_ENTER_FUNCTION_DBG("double msSampler::scalarProduct(msScalarFunction& fct1 ,msScalarFunction& fct2)",
                                  getFullId());
        
        if(fct1.getCoordinates() !=fct2.getCoordinates())
        BOOST_THROW_EXCEPTION(msError("The two scalar functions must share the same coordinates",
                                      "double msSampler::scalarProduct(msScalarFunction& fct1 ,msScalarFunction& fct2)",getFullId())
                              );
        setCoordinates(fct1.getCoordinates());
        int width=10;
        std::stringstream comment;
        double result=0;double v=0;
        
        msSampler::msSamplingData data;
        Coordinates->setMinValues();
        
        msGeneralizedCoordinates::iterator it=Coordinates->begin();
        
        for(;it!=Coordinates->end();++it) comment<<std::setw(width)<<(*it)->getId();
        comment<<std::setw(30)<<"field value 1"<<std::setw(30)<<"field value 2"<<endl;
        LOGGER_WRITE(msLogger::INFO,comment.str());
        
        LogStep=10;
        begin();
        while( getNextPoint(Coordinates.getSharedPtr()) ){
            
            double Fct1=fct1.evaluate();
            double Fct2=fct2.evaluate();
            
            result += Fct1*Fct2; 
            if( ( nScanned!=0) && (nScanned % LogStep) == 0 ){   
                
                comment.str("");
                for(it=Coordinates->begin();it!=Coordinates->end();++it) comment<<std::setw(width)<<(*it)->getValue();
                comment<<std::setw(30)<<Fct1<<std::setw(30)<<Fct2<<std::setw(30)<<result<<endl;
                
                LOGGER_WRITE(msLogger::INFO,comment.str());
            } 
        }
        LOGGER_EXIT_FUNCTION();
        return result;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::ostream& msSampler::print(std::ostream& out) const
    {
        msTreeMapper::print(out);
        output::printHeader(out,"Sampler");
        
        if( Coordinates!=boost::shared_ptr<msGeneralizedCoordinates>() )
        {
            out<<"\nId of the generalized coordinates:  "<<Coordinates->getId()<<"; type: "<<Coordinates->getType()<<endl<<" size="<<Coordinates->noOfDim()<<"\t";
            Coordinates->getUnits()->abstract(out);
            out<<"\n\tList of unfreezed coordinates:\n";
            msGeneralizedCoordinates::UnfreezIt qi = Coordinates->beginUnfreezed();
            for(; qi!=Coordinates->endUnfreezed(); qi++)
            {
                out<<"\t\t"; (*qi).print(out); out<<endl;
            }
        }
        else out<<" no coordinates defined"<<endl;
        return out;
    }
    
}
