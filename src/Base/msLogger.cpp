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


#include "msLogger.h"

namespace impact
{
    
    
    // --------------------------------------
    // static members initialization
    // --------------------------------------
    
    bool msLogger::isLoggerRegisteredInPython=0;
    msRegistrar msLogger::Registrar("msLogger", msLogger::createInstance);
    
    string msLogger::LastFunction="";
    
    string msLogger::LastObject="";
    
    bool msLogger::TreeRepresentation = 0;
    
    msLogElement* msLogger::CurrentElement;
    
    int msLogger::InfoLevel = 0;
    
    int msLogger::InfoLevelMax = 3;
    
    int msLogger::NoOfFunctionOpen = 0;
    
    bool msLogger::active =0;
    
    msLogger::Priority msLogger::minPriority = msLogger::DEBUG;
    
    const string msLogger::PRIORITY_NAMES[] =
    {   "DEBUG",
        "CONFIG",
        "INFO",
        "WARNING",
        "ERROR",
        "EXCEPTION"
    };
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    

    msRegistrar msLogElement::Registrar("msLogElement", msLogElement::createInstance);
    bool  msLogElement::isLogElementRegisteredInPython = 0;
    
    void msLogElement::registryInPython() {
    
#if USE_PYTHON
        
        msRegister::registryInPython();
        
        if( ! msLogElement::isLogElementRegisteredInPython ) {
            
            using namespace boost::python;
            class_<msLogElement, boost::shared_ptr<msLogElement>, boost::noncopyable>(
                                                                                      "msLogElement", no_init)
            .def("getFunctionsCalled",
                 &msLogElement::getFunctionsCalled,
                 "return the functions called by the current function")
            .def("getFunctionName",&msLogElement::getFunctionName,
                 "return the function's name")
            .def("getPriority",&msLogElement::getPriority,
                 "return the function debug priority")
            .def("getTime",&msLogElement::getTime,
                 "return the function's call time ")
            .def("getTimeElapsed",&msLogElement::getTimeElapsed,
                 "return the duration of the function call")
            .def("getTimeElapsed",&msLogElement::getTimeElapsed,
                 "return the duration of the function call")
            .def("getMessages", &msLogElement::getMessages,
                 "return the messages");
            
	    class_< std::vector<boost::shared_ptr<msLogElement> > >("VectorOfLogElement",
                                                     "This object stores a vector of logger element",
                                                     init<>())
	    .def( vector_indexing_suite<std::vector<boost::shared_ptr<msLogElement> > >() )
            .def("__iter__",boost::python::iterator< std::vector<boost::shared_ptr<msLogElement> > >() );
	    
            msLogElement::isLogElementRegisteredInPython=1;
        }
#endif    
    }

    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    msLogElement::msLogElement(){Parent=0;}
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    msLogElement* msLogElement::addFunctionCall(std::string priority, std::string function){
        
        boost::shared_ptr<msLogElement> elem = boost::shared_ptr<msLogElement>(new msLogElement());
        std::time_t now = std::time(NULL);
        elem->TimeStamp  = std::string(std::ctime(&now));
        
        elem->FunctionPriority = priority;
        elem->FunctionCalled   = function;
        elem->Parent = this;
        elem->TimeBegin = clock();
        Children.push_back(elem);
        
        return elem.get();
    }

    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msLogElement::closeFunction(std::string fct){
        
        if( fct!= "" )
            if( fct != getFunctionName()){
                
                stringstream out;
                out<<"mismatch between open and close function. "
                <<"Function entered: "<<getFunctionName()
                <<", Function exited: "<<fct;
                BOOST_THROW_EXCEPTION( msError(out.str(),
                                               "void msLogElement::closeFunction(std::string fct)",
                                               ""
                                               ));
                
            }
        TimeEnd = clock();//boost::posix_time::microsec_clock::local_time();
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msLogElement::addMessage( std::string messagepriority , std::string message ) {
        
        if( messagepriority!="")
            Messages.push_back( messagepriority + "\n" + message );
        else
            Messages.push_back( message );
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::string msLogElement::getTimeElapsed(){
        
        /*using namespace boost::posix_time;
        time_duration duration(TimeEnd- TimeBegin);*/
        stringstream out;
        out<<TimeEnd- TimeBegin;
        return out.str();
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------

    std::vector<boost::shared_ptr<msLogElement> >
    msLogElement::getFunctionsCalled(){
        
        return Children;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------

    msLogElement* msLogElement::getParent() {
        
        return Parent;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::string msLogElement::getFunctionName() {
        
        return FunctionCalled;
    }
   
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::vector<std::string> msLogElement::getMessages() {
        
        return Messages;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::string msLogElement::getPriority() {
        
        return FunctionPriority;
    }
  
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::string msLogElement::getTime() {
        
        return TimeStamp;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    
    void msLogger::registryInPython()
    {
#if USE_PYTHON
        
        msRegister::registryInPython();
        
        if( ! msLogger::isLoggerRegisteredInPython ) {
            
            using namespace boost::python;
            class_<msLogger, boost::shared_ptr<msLogger>, boost::noncopyable>(
                                                                              "msLogger", no_init)
            .def( "setVerboseLevel" ,	&msLogger::setVerboseLevel )
            .staticmethod("setVerboseLevel")
            .def( "setPriorityMin" ,	&msLogger::setPriorityMin )
            .staticmethod("setPriorityMin")
            .def( "startTreeRepresentation" ,  &msLogger::startTreeRepresentation )
            .staticmethod("startTreeRepresentation")
	    .def( "stopTreeRepresentation" ,  &msLogger::stopTreeRepresentation )
            .staticmethod("stopTreeRepresentation")
	    .def( "clearTreeRepresentation" ,  &msLogger::clearTreeRepresentation )
            .staticmethod("clearTreeRepresentation")
            .def( "getTreeRepresentation" ,  &msLogger::getTreeRepresentation )
            .staticmethod("getTreeRepresentation")
            .def( "enterFunction" ,  &msLogger::enterFunction )
            .staticmethod("enterFunction")
            .def( "enterFunctionDbg" ,  &msLogger::enterFunctionDbg )
            .staticmethod("enterFunctionDbg")
            .def( "write" ,  (void(*)(Priority,const string&)) &msLogger::write )
            .staticmethod("write")
            .def( "exitFunction" , (void(*)(std::string) ) &msLogger::exitFunction )
            .staticmethod("exitFunction");
            enum_<Priority>("Priority")
            .value(PRIORITY_NAMES[DEBUG].c_str(), DEBUG)
            .value(PRIORITY_NAMES[CONFIG].c_str(), CONFIG)
            .value(PRIORITY_NAMES[INFO].c_str(), INFO)
            .value(PRIORITY_NAMES[WARNING].c_str(), WARNING)
            .value(PRIORITY_NAMES[ERROR].c_str(), ERROR)
            .value(PRIORITY_NAMES[EXCEPTION].c_str(), EXCEPTION)
            .export_values();
            msLogger::isLoggerRegisteredInPython=1;
        }
#endif
    }
    
    //msLogger msLogger::instance;
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    msLogger::msLogger() {}
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msLogger::setVerboseLevel(int llm) { InfoLevelMax = llm;}
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msLogger::setPriorityMin(Priority min){ minPriority = min;}
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msLogger::start(Priority min, int llm) {
      
        CurrentElement = new msLogElement();
        InfoLevel = 0;
        InfoLevelMax = llm ;
        active = true;
        min = minPriority;
    }
    void msLogger::restart() {
        active = true;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msLogger::startTreeRepresentation() {
        
        TreeRepresentation = 1;
    }

    void msLogger::stopTreeRepresentation() {
        
        TreeRepresentation = 0;
    }
    
    void msLogger::clearTreeRepresentation() {
        
        if  (!active)  return;
	
        delete CurrentElement;
        CurrentElement = new msLogElement();
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::vector<boost::shared_ptr<msLogElement> > msLogger::getTreeRepresentation(){
        
        if(!TreeRepresentation)
            BOOST_THROW_EXCEPTION( msError("The tree representation has not been started, no representation available"
                                           ,"std::vector<boost::shared_ptr<msLogElement> > msLogger::getTreeRepresentation()"
                                           ,"singleton Logger" ));
        
        msLogElement* element = CurrentElement;
        while(element->getParent() != 0) element =element->getParent();
        return element->getFunctionsCalled();
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msLogger::stop()
    {
        active = false;
        delete CurrentElement;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msLogger::enterFunction(std::string fct,std::string objId) {
        
        if  (!active)  return;
        NoOfFunctionOpen++;
        if ( minPriority > DEBUG)  return;
	if( (minPriority == INFO ) && (NoOfFunctionOpen > InfoLevelMax ) ) return;
	
        for(int i=0;i<NoOfFunctionOpen-1;i++) std::cout<<"\t";
        
        std::cout<<output::printGreen(objId)
        <<" "<<fct
        <<std::endl;
        
        if(TreeRepresentation)
            CurrentElement = CurrentElement->addFunctionCall( PRIORITY_NAMES[INFO], fct );
        
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msLogger::enterFunctionDbg( std::string fct, std::string objId) {
        
        if  (!active)  return;
        NoOfFunctionOpen++;
        if ( minPriority > DEBUG)  return;
               
        for(int i=0;i<NoOfFunctionOpen-1;i++) std::cout<<"\t";
        
        std::cout<<output::printYellow(objId)
        <<" "<<fct
        <<std::endl;
        if(TreeRepresentation)
            CurrentElement = CurrentElement->addFunctionCall( PRIORITY_NAMES[DEBUG], fct );
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msLogger::exitFunction() {
      
        if  (!active)  return;
        NoOfFunctionOpen--;
        if ( minPriority > DEBUG)  return;
        
        if(TreeRepresentation){
            
            CurrentElement->closeFunction("");
            if( CurrentElement->getParent() != 0 )
               CurrentElement = CurrentElement->getParent();
        }
    }
   
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msLogger::exitFunction(std::string fct) {
        
        if  (!active)  return;
        NoOfFunctionOpen--;
        if ( minPriority > DEBUG)  return;
	
        LOGGER_WRITE(msLogger::DEBUG,"exit function");
                
        if(TreeRepresentation) {
            try {
                CurrentElement->closeFunction(fct);
            }
            catch (msError& e) {
                e.addContext("void msLogger::exitFunction(std::string fct)");
                throw e;
            }
            if( CurrentElement->getParent() != 0 )
                CurrentElement = CurrentElement->getParent();
        }
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msLogger::write(Priority priority, const string& message , const string& fct) {
        
        if( (!active) || (priority<minPriority) ) return;
        if( (minPriority == INFO ) && (NoOfFunctionOpen > InfoLevelMax ) ) return;
        
        if(priority==ERROR){
            
            std::cout<<PRIORITY_NAMES[priority]<<" : "
            <<output::printRed(message)<<std::endl;
            char a[10];std::cin>>a;return;
        }
        if(priority==WARNING){
            
            std::cout<<PRIORITY_NAMES[priority]<<" : "
            <<output::printYellow(message)<<std::endl;
        }
        if(priority==INFO){
            
            std::cout<<PRIORITY_NAMES[priority]<<" : "
            <<output::printGreen(message)<<std::endl;
        }
        if(priority==DEBUG){
            
            std::cout<<PRIORITY_NAMES[priority]<<" : "
            <<message<<std::endl;
        }
        if(TreeRepresentation)
            CurrentElement->addMessage(PRIORITY_NAMES[priority], message );
    }
   
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msLogger::write( Priority priority , const string& message )
    {
        if( (!active) || (priority<minPriority) ) return;
        if( (minPriority == INFO ) && (NoOfFunctionOpen > InfoLevelMax ) ) return;
        
        for(int i=0;i<NoOfFunctionOpen;i++) std::cout<<"\t";
        
        if(priority==ERROR){
            
            std::cout<<PRIORITY_NAMES[priority]<<" : "
            <<output::printRed(message)<<std::endl;
            char a[10];std::cin>>a;
        }
        if(priority==WARNING){
            
            std::cout<<PRIORITY_NAMES[priority]<<" : "
            <<output::printYellow(message)<<std::endl;
        }
        if(priority==INFO){
            
            std::cout<<PRIORITY_NAMES[priority]<<" : "
            <<output::printGreen(message)<<std::endl;
        }
        if(priority==DEBUG){
            
            std::cout<<PRIORITY_NAMES[priority]<<" : "
            <<message<<std::endl;
        }
        if(TreeRepresentation)
            CurrentElement->addMessage(PRIORITY_NAMES[priority], message );
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msLogger::header2Columns(Priority priority,string c0, string u0, string c1, string u1)
    {
        if( (!active) || (priority<minPriority) ) return;
        if( (minPriority == INFO ) && (NoOfFunctionOpen > InfoLevelMax ) ) return;
        
        int witdh=25;
        std::string v0=c0+"["+u0+"]";
        std::string v1=c1+"["+u1+"]";
        std::stringstream comment;
        comment<<std::setw(witdh)<<v0<<std::setw(witdh)<<v1;
        std::cout<<comment.str()<<std::endl;
        
        if(TreeRepresentation)
            CurrentElement->addMessage(PRIORITY_NAMES[priority], comment.str() );
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msLogger::write2Columns(Priority priority,double a, double b)
    {
        if( (!active) || (priority<minPriority) ) return;
        if( (minPriority == INFO ) && (NoOfFunctionOpen > InfoLevelMax ) ) return;
        
        int witdh=25;
        std::stringstream comment;
        comment<<std::setw(witdh)<<a<<std::setw(witdh)<<b<<std::endl;
        std::cout<<comment.str();
        
        if(TreeRepresentation)
            CurrentElement->addMessage(PRIORITY_NAMES[priority], comment.str() );
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msLogger::header3Columns(Priority priority,string c0, string u0, string c1, string u1, string c2, string u2) {
        
        if( (!active) || (priority<minPriority) ) return;
        if( (minPriority == INFO ) && (NoOfFunctionOpen > InfoLevelMax ) ) return;
        
        int witdh=25;
        std::string v0=c0+"["+u0+"]";
        std::string v1=c1+"["+u1+"]";
        std::string v2=c2+"["+u2+"]";
        std::stringstream comment;
        comment<<std::setw(witdh)<<v0<<std::setw(witdh)<<v1<<std::setw(witdh)<<v2;
        std::cout<<comment.str()<<std::endl;
        
        if(TreeRepresentation)
            CurrentElement->addMessage(PRIORITY_NAMES[priority], comment.str() );
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msLogger::write3Columns(Priority priority,double a, double b, double c) {
        
        if( (!active) || (priority<minPriority) ) return;
        if( (minPriority == INFO ) && (NoOfFunctionOpen > InfoLevelMax ) ) return;
        
        int witdh=25;
        std::stringstream comment;
        comment<<std::setw(witdh)<<a<<std::setw(witdh)<<b<<std::setw(witdh)<<c;
        std::cout<<comment.str()<<std::endl;
        if(TreeRepresentation)
            CurrentElement->addMessage(PRIORITY_NAMES[priority], comment.str() );
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msLogger::header4Columns(Priority priority,string c0, string u0, string c1, string u1, string c2, string u2, string c3, string u3) {
        
        if( (!active) || (priority<minPriority) ) return;
        if( (minPriority == INFO ) && (NoOfFunctionOpen > InfoLevelMax ) ) return;
        
        int witdh=25;
        std::string v0=c0+"["+u0+"]";
        std::string v1=c1+"["+u1+"]";
        std::string v2=c2+"["+u2+"]";
        std::string v3=c3+"["+u3+"]";
        std::stringstream comment;
        comment<<std::setw(witdh)<<v0<<std::setw(witdh)<<v1<<std::setw(witdh)<<v2<<std::setw(witdh)<<v3;
	std::cout<<comment.str()<<std::endl;
	
        if(TreeRepresentation)
            CurrentElement->addMessage(PRIORITY_NAMES[priority], comment.str() );
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msLogger::write4Columns( Priority priority, double a, double b, double c, double d) {
        
        if( (!active) || (priority<minPriority) ) return;
        if( (minPriority == INFO ) && (NoOfFunctionOpen > InfoLevelMax ) ) return;
        
        int witdh=25;
        std::stringstream comment;
        comment<<std::setw(witdh)<<a<<std::setw(witdh)<<b<<std::setw(witdh)<<c<<std::setw(witdh)<<d;
	std::cout<<comment.str()<<std::endl;
	
        if(TreeRepresentation)
            CurrentElement->addMessage(PRIORITY_NAMES[priority], comment.str() );
    }
    
    
    
}
