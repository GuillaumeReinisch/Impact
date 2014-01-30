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

#include <msRegister.h>

#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
//#include "boost/date_time/posix_time/posix_time.hpp"

//#ifdef ENABLE_LOGGER

#ifndef MSLOGGER_H
#define MSLOGGER_H


#define LOGGER_START(MIN_PRIORITY, FILE) msLogger::start(MIN_PRIORITY, FILE);
#define LOGGER_STOP() msLogger::Stop();
#define LOGGER_WRITE_ALONE(PRIORITY, MESSAGE, FCT) msLogger::write(PRIORITY, MESSAGE,FCT);
#define LOGGER_WRITE(PRIORITY,MESSAGE) msLogger::write(PRIORITY,MESSAGE);
#define LOGGER_ENTER_FUNCTION(FCT, OBJID) msLogger::enterFunction(FCT, OBJID);
#define LOGGER_ENTER_FUNCTION(FCT, OBJID) msLogger::enterFunction(FCT, OBJID);
#define LOGGER_ENTER_FUNCTION_DBG(FCT, OBJID) msLogger::enterFunctionDbg(FCT, OBJID);

#define LOGGER_HEADER4COLUMNS(p,a,au,b,bu,c,cu,d,du)  msLogger::header4Columns(p,a,au,b,bu,c,cu,d,du);
#define LOGGER_WRITE4COLUMNS(p,a,b,c,d)  msLogger::write4Columns(p,a,b,c,d);
#define LOGGER_HEADER3COLUMNS(p,a,au,b,bu,c,cu)  msLogger::header3Columns(p,a,au,b,bu,c,cu);
#define LOGGER_WRITE3COLUMNS(p,a,b,c)  msLogger::write3Columns(p,a,b,c);
#define LOGGER_HEADER2COLUMNS(p,a,au,b,bu)  msLogger::header2Columns(p,a,au,b,bu);
#define LOGGER_WRITE2COLUMNS(p,a,b)  msLogger::write2Columns(p,a,b);

#define LOGGER_EXIT_FUNCTION()     msLogger::exitFunction();
#define LOGGER_EXIT_FUNCTION2(FCT) msLogger::exitFunction(FCT);


#define IMPACT_LOGIN()         msLogger::enterFunctionDbg( __PRETTY_FUNCTION__  , myConstSharedPtr()->getFullId() );
#define IMPACT_LOGIN_STATIC()  msLogger::enterFunctionDbg( __PRETTY_FUNCTION__  , "static" );
#define IMPACT_LOGOUT()        msLogger::exitFunction(__PRETTY_FUNCTION__);
#define IMPACT_LOGIN_LOGOUT_RETURNME(FCT)  IMPACT_LOGIN();\ doLambda(FCT);\ IMPACT_LOGOUT();\ return mySharedPtr();

//#else
/*
 #define LOGGER_START(MIN_PRIORITY, FILE)
 #define LOGGER_STOP()
 #define LOGGER_WRITE(PRIORITY, MESSAGE)
 
 #endif*/
template<class Func>
void doLambda( Func func) {func();}

using namespace std;
namespace impact
{
    
    class msTreeMapper;
    
    class msLogElement : public msRegister
    {
        struct msLogMessage{ std::string MessagePriority; std::string Message; };
        
	friend class msException;
	
        /** @Name From msRegister
         *
         */
        //@{
        static msRegistrar Registrar;
        static bool  isLogElementRegisteredInPython;
        static msRegister* createInstance() {return new msLogElement;}
        
        void registryInPython();
        //@}
        
    public:
        
        msLogElement();
        
        msLogElement* addFunctionCall(std::string fctpriority, std::string function);
        
        std::vector<boost::shared_ptr<msLogElement> > getFunctionsCalled();
        
        msLogElement* getParent();
        
        void addMessage( std::string messagepriority , std::string message );
        
        std::string getFunctionName();
        std::vector<std::string> getMessages();
        std::string getPriority();
        std::string getTime();
        std::string getTimeElapsed();
        
        void closeFunction(std::string);
        
    private:
        
       
        double TimeBegin;
        double TimeEnd;
        
        std::vector<boost::shared_ptr<msLogElement> > Children;
        
        std::string FunctionPriority;
        
        std::string FunctionCalled;
        
        std::vector<std::string> Messages;
        
        std::string TimeStamp;
        
        msLogElement* Parent;
        
        size_t PositionInParent;
        
    };
    
    class msLogger : public msRegister ,  boost::noncopyable
    {
      friend class msException;
      
    public:
        // log priorities
        enum Priority
        {
            DEBUG,
            CONFIG,
            INFO,
            WARNING,
            ERROR,
            EXCEPTION
        };
        
        /** @Name From msRegister
         *
         */
        //@{
        static msRegistrar Registrar;
        static bool  isLoggerRegisteredInPython;
        static msRegister* createInstance() {
            
            msLogger::start(msLogger::INFO,3);
            return new msLogger;
        }
        
        void registryInPython();
        //@}
        
    public:
        
        
        // start/stop logging
        // - messages with priority >= minPriority will be written in log
        
        static void start(Priority minPriority, int llm);
        static void restart();
        static void stop();
        
        static void startTreeRepresentation();
	static void stopTreeRepresentation();
	static void clearTreeRepresentation();
	
        static std::vector<boost::shared_ptr<msLogElement> > getTreeRepresentation();
        static void clearTree();
        
        static void setVerboseLevel(int llm);
        static void setPriorityMin(Priority minPriority);
        
        static void write(Priority priority, const string& message,const string& fct);
        
        static void enterFunction(std::string fct,std::string objId);
        static void enterFunctionDbg(std::string fct,std::string objId);
        
        static void write(Priority priority,const string& message);
        static void header2Columns(Priority priority, std::string c0, std::string u0,
                                   std::string c1, std::string u1);
        static void write2Columns( Priority priority,double a, double b);
        static void header3Columns(Priority priority, std::string c0, std::string u0,
                                   std::string c1, std::string u1,
                                   std::string c2, std::string u2);
        static void write3Columns(Priority priority, double a, double b, double c );
        
        static void header4Columns(Priority priority, std::string c0, std::string u0,
                                   std::string c1, std::string u1,
                                   std::string c2, std::string u2,
                                   std::string c3, std::string u3);
        static void write4Columns(Priority priority, double a, double b, double c, double d );
        
        static void exitFunction();
	
        static void exitFunction(std::string);
        
        static bool isDebug(){if(minPriority==0) return 1; return 0;};
        
        static int NoOfFunctionOpen;
        
	static std::string LastFunction;
		
    private:
        
        // msLogger adheres to the singleton design pattern, hence the private
        // constructor, copy constructor and assignment operator.
	
        msLogger& operator = (const msLogger& logger) {return *this;}
        msLogger();
        msLogger(const msLogger& logger) {}
        
        // private instance data
        static bool        active;
        ofstream           OutStream;
        static Priority    minPriority;
        
        static bool        TreeRepresentation;
        
        static msLogElement* CurrentElement;
        
        static std::string LastObject;
        
        static int InfoLevel;
        static int InfoLevelMax;
        
        // names describing the items in enum Priority
        static const string PRIORITY_NAMES[];
        
        // the sole msLogger instance (singleton)
        // static msLogger instance;
        
    };
}
#endif // MSLOGGER_H
