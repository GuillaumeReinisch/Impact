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


/**
 * \file c_parser.h
 * \brief  Expression parser
 * \author Jos de Jong
 * \date 2007-12-22
 */


#ifndef msPARSER_H
#define msPARSER_H
#include <msScalarFunction.h>
#include <muParser.h>

namespace impact
{
    
    /*! \class msParser
     *
     *  \brief A parser to define scalar function
     *
     *  A wrapper around the mu::Parser parser; many symbols available, see http://muparser.beltoforion.de/.
     *
     *  To create a new object, set the coordinates using msScalarFunction::setCoordinates and the expression
     * using msParser::setExpression. The variables' name used in the expression are the id
     * of the variable included in the coordinates. You can use the constant defined by msScalarFunction::setConstant.
     *
     *  example:
     * ~~~~~~~~~~~~~{.py}
     from libimpact import *
     
     x = msScalarVariable.New().set(0,0,50,0.5,0.5).setId("q0")
     y = msScalarVariable.New().set(0,0,50,0.5,0.5).setId("q1")
     coors = msGeneralizedCoordinates.New().addVar(x).addVar(y)
     
     fct = msParser.New()
     fct.setCoordinates(coors)
     fct.setConstant("a0",2)
     fct.setExpression("a0*q0*q1")
     
     fct.evaluate()
     
     * ~~~~~~~~~~~~~
     */
    class msParser : public msScalarFunction
    {
        //!\name from msRegister
        //@{
        static msRegistrar Registrar;
        static bool  isParserRegisteredInPython;
        static msRegister* createInstance() { return new msParser; }
        //@}
        
        
    protected:
        
        void registryInPython();
        
        msParser() : msScalarFunction(){
            constructVar("msParser","Parser","parser");
        }
        
    public:
        
        
        static boost::shared_ptr<msParser> New(){
            
            LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msParser> msParser::New()","");
            LOGGER_WRITE(msLogger::DEBUG,"New msParser object");
            
            boost::shared_ptr<msParser> T( new msParser );
            T->initialize();
            T->update();
            LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msParser> msParser::New()");
            return T;
        }
        
        virtual boost::shared_ptr<msTreeMapper> clone() {
            boost::shared_ptr<msParser> clone = New();
            clone->msTreeMapper::operator=(*this);
            return clone;
        }
        
        
        virtual void initialize()  {
            msScalarFunction::initialize();
            declareAttribute(Expression,"Expression");
        };
        
        virtual void update(){
            
            LOGGER_ENTER_FUNCTION_DBG("void msParser::update()",getFullId());
            msScalarFunction::update();
            LOGGER_WRITE(msLogger::DEBUG,"Update variables and expression");
            setVariables();
            setExpression(Expression);
            LOGGER_EXIT_FUNCTION2("void msParser::update()");
        }
        
        /*! \brief set the expression of the parser
         *
         * \param exp expression, see http://muparser.beltoforion.de/mup_features.html#idDef1 
         * for the available symbols
         */
        boost::shared_ptr<msTreeMapper> setExpression(std::string exp){
            
            Expression=exp;
            Parser.SetExpr(Expression);
            return mySharedPtr();
        }
        
        //! \brief redefinition of the msScalarFunction::setVariables to define the symbols in the Parser
        boost::shared_ptr<msTreeMapper> setVariables(){
            
            for( size_t i=0 ; i<getCoordinates()->noOfDim() ; ++i ){
                
                string id=(*getCoordinates())[i]->getId();
                
                double* value = getCoordinates()->getVariable(i)->getPtrOnValue();
                
                try{
                    Parser.DefineVar(id,value );
                }
                catch( mu::Parser::exception_type &e){
                    stringstream out;
                    out<<"Error in the parser variable definition.\n"<<e.GetMsg()<<endl;
                    msException e2(out.str(), "double msParser::setVariables()", getFullId());
                    
                    IMPACT_THROW_EXCEPTION(e2);
                }
                
            }
            
            std::map< std::string , double >::iterator it = Constants.begin();
            
            for(; it!=Constants.end() ; ++it ) Parser.DefineVar( (*it).first , &((*it).second) );
            return mySharedPtr();
        }
        //! \brief return the expression of the Parser
        std::string getExpression() const {return Expression;}
        
        double evaluate();
        
        
        std::ostream& print(std::ostream& out) const;
        
      	std::string getLabel(){return ("Id: "+getId()+"; expression: "+Expression);}
        
    private:
        
        double value;
        
        mu::Parser Parser;
        
        std::string Expression;
        
    };
    
}
#endif // msPARSER_H






