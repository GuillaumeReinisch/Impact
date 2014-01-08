/*
 <one line to give the program's name and a brief idea of what it does.>
 Copyright (C) <year>  <name of author>
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */

#include "msParser.h"

namespace impact
{
    using namespace std;
    
    
    bool msParser::isParserRegisteredInPython=0;
    
    msRegistrar msParser::Registrar("msParser", msParser::createInstance);
    
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::ostream& operator<<(std::ostream& out,const msParser& parser)
    {
        parser.print(out);
        return( out );
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    
    void msParser::registryInPython()
    {
#if USE_PYTHON
        msScalarFunction::registryInPython();
        
        if( ! msParser::isParserRegisteredInPython ){
            
            using namespace boost::python;
            
            class_<msParser,bases<msScalarFunction>,boost::shared_ptr<msParser> >
            ("msParser",
             "A string parser for scalar function",no_init)
            .def( "New", &msParser::New , "Create a new object. Arg1: msUnitsManager object")
            .staticmethod("New")
            .def( "setExpression",&msParser::setExpression, "set the expression")
            .def( "getExpression",&msParser::getExpression, "get the expression")
            .def(self_ns::str(self_ns::self));
            msParser::isParserRegisteredInPython=1;
        }
#endif
    }
    
    std::ostream& msParser::print(std::ostream& out) const {
        
        msScalarFunction::print(out);
        output::printHeader(out,"Parser");
        
        out<<"Expression: "<<Parser.GetExpr()<<endl;
        mu::varmap_type vars = Parser.GetVar();
        out<<"Defined variables:\n";
        for( mu::varmap_type::iterator it=vars.begin();it!=vars.end();++it)
            out<<(*it).first<<" "<<*((*it).second)<<"\n";
        
        out<<endl;
        return out;
    }
    
    
    
    double msParser::evaluate() {
        
        double e=0;
        setVariables();
        
        try{ e = Parser.Eval();
        }
        catch( mu::Parser::exception_type &e){
            
            double pos =  e.GetPos() ;
            if( pos > 100000000 ) pos=0;
            stringstream out;
            out<<"Error in the parser evaluation.\n"
            <<"Message:  " << e.GetMsg()  << "\n"
            <<"Formula:  " << e.GetExpr() << "\n"
            <<"Token:    " << e.GetToken()<< "\n"
            <<"Position: " << e.GetPos()  <<"."<<endl;
            msError e2(out.str(), "double msParser::Evaluate()", getFullId());
            
            BOOST_THROW_EXCEPTION(e2);
        }
        return e;
    }
}
