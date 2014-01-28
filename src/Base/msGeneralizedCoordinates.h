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


#ifndef MSGENERALIZEDCOORDINATE_H
#define MSGENERALIZEDCOORDINATE_H

#include <msPhysicalInterface.h>
#include <msScalarVariable.h>

namespace impact
{
    
    class msScalarFunction;
    
    /*! \class msGeneralizedCoordinates
     *
     *  \brief A container for coordinates
     *
     * This class provides methods to interact with an ensemble of coordinates 
     * (represented as scalar variables - msScalarVariable -) that can be used
     * for many purposes. One, for instance, is to use them as generalized coordinates
     * in the analytical mechanic framework of impact::atomism.
     *
     * msGeneralizedCoordinates objects define a configuration
     * space with an elementary volume defined by the msScalarVariable::Dq attribut and
     * bounded by the msScalarVariable::min and msScalarVariable::max attributs;
     * any variable with dq==0 is called inactive, any variable with Dq==0 is called
     * freezed.
     *
     * To sample the configurational space, you can use msSampler derived classes.
     *
     * To construct a msGeneralizedCoordinates object, you add sequentially the
     * msScalarVariable, example:
     * ~~~~~~~~~~~~~{.py}
     from libimpact import *
     
     x = msScalarVariable.New().set(0,0,50,0.001,0.5).setUnit("m")
     y = msScalarVariable.New().set(0,0,360,0.001,1).setUnit("Degree")
     
     coors = msGeneralizedCoordinates.New().addVar(x).addVar(y)
     
     print coors.getVolumeDq() 
     
     for v in coors.getVariables():
        print v.getValue()
     * ~~~~~~~~~~~~~
     */
    class msGeneralizedCoordinates : public msPhysicalInterface
    {
        friend class msScalarFunction;
        
        //!\name from msRegister
        //@{
        static msRegistrar Registrar;
        static bool  isGeneralizedCoordinatesRegisteredInPython;
        static msRegister* createInstance() { return new msGeneralizedCoordinates; }
        //@}
        
    protected:
        
        void registryInPython();
        
    public:
        
        template<class Value,class Value2>
        class UnfreezedIterator : public boost::iterator_facade<UnfreezedIterator<Value,Value2>,
        Value,
        boost::forward_traversal_tag>
        {
        public:
            
            UnfreezedIterator() : Variable(0), GeneralizedCoordinates(0), Indice(-1) {};
            UnfreezedIterator(Value2* s) : Variable(0), GeneralizedCoordinates(s), Indice(-1)
            {if(s!=0)increment();
            };
            explicit UnfreezedIterator(Value* v) : Variable(v) {};
            Value& dereference() const {return *Variable;}
            
            msChildren<msScalarVariable>::iterator toIterator(){return GeneralizedCoordinates->begin()+Indice;};
            msChildren<msScalarVariable>::const_iterator toIterator()const{return GeneralizedCoordinates->begin()+Indice;};
            
        private:
            
            friend class boost::iterator_core_access;
            
            void increment() {
                Indice++;
                if( Indice==int(GeneralizedCoordinates->noOfDim()) ){ Variable=0; return; }
                else Variable=GeneralizedCoordinates->operator[](Indice).get();
                while( (Variable->dq==0) && ( Indice < int(GeneralizedCoordinates->noOfDim()) - 1 ) ){
                    Indice++;Variable=GeneralizedCoordinates->operator[](Indice).get();
                }
                if( Variable->dq !=0 ) return;
                Variable=0; return;
            }
            bool equal(UnfreezedIterator<Value,Value2> const& other) const{
                return this->Variable == other.Variable;
	        }
            
        protected:
            
            Value* Variable;
            Value2* GeneralizedCoordinates;
            int Indice;
        };
        
        typedef UnfreezedIterator<msScalarVariable,msGeneralizedCoordinates> UnfreezIt;
        typedef UnfreezedIterator<const msScalarVariable,const msGeneralizedCoordinates> const_UnfreezIt;
        typedef msChildren<msScalarVariable>::iterator iterator;
        typedef msChildren<msScalarVariable>::const_iterator const_iterator;
        
    protected:
        
        void initialize()  {
            msPhysicalInterface::initialize();
            declareChildren<msScalarVariable>(Variables,"Variables");
        };
        
        void update(){ msPhysicalInterface::update();}
        
    public:
        msGeneralizedCoordinates() : msPhysicalInterface(){
            constructVar("msGeneralizedCoordinates","GeneralizedCoordinates","generalizedCoordinates");
        }
        static boost::shared_ptr<msGeneralizedCoordinates> New(){
            
            LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msGeneralizedCoordinates> msGeneralizedCoordinates::New()","");
            LOGGER_WRITE(msLogger::DEBUG, "New msGeneralizedCoordinates object")
            
            boost::shared_ptr<msGeneralizedCoordinates> T( new msGeneralizedCoordinates );
            T->initialize();
            
            LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msGeneralizedCoordinates> msGeneralizedCoordinates::New()");
            return T;
        }
        
        virtual boost::shared_ptr<msTreeMapper> clone() {
            boost::shared_ptr<msGeneralizedCoordinates> clone = New();
            clone->msTreeMapper::operator=(*this);
            return clone;
        }
        
        /*! \brief add a variable to the container
         *
         * \param var msScalarVariable object
         */
        boost::shared_ptr<msTreeMapper> addVar(boost::shared_ptr<msScalarVariable> var);
        
        //! \brief clear the container
        void clear() { if( Variables.size() != 0 )
            msTreeMapper::clearChildren<msScalarVariable>(Variables);
        }
        
        //! \brief return the number of variable
        size_t noOfDim() const;
        
        //! \brief return the number of active variable
        size_t noOfActive() const;
        
        //! \brief return the number of unfreezed variable
        size_t getIntegrationDim() const;
        
        //! \brief return the volume of the elementary volume of the configurational space
        double getVolumeDq() const;
        
        /*! \brief return the volume of the elementary volume of the configurational space in a particular unit system
         *
         * \param units units desired
         */
        double getVolumeDq(const msUnitsManager& units) const;
        
        void getUnfreezedValues(const msUnitsManager& units, vector_type& vec) const;
        
	
        //!\name operation on the variable's iterators
        //@{
        
        boost::shared_ptr<msTreeMapper> setValue(iterator qi, double v, const msUnitsManager& units);
        
        double getdq(const_iterator q, const msUnitsManager& units) const;
        
        double getValue(const_iterator q, const msUnitsManager& units) const;
        
        double getMin(const_iterator q, const msUnitsManager& units) const;
        
        double getMax(const_iterator q, const msUnitsManager& units) const;
        
        //@}

        //!\name set some attributs of the variables
        //@{
        
        //! \brief set all the variables' value to their minimum
        boost::shared_ptr<msTreeMapper> setMinValues();
        
        //! \brief set the variables' value randomly
        void setRandom();
        
        /*! \brief reset a variable
         *
         * \param i index of the variable
         * \param val variable
         */
        boost::shared_ptr<msTreeMapper> setVariable(size_t i,boost::shared_ptr<msScalarVariable> v) {
            
            try {
                msTreeMapper::replaceElementOfChildren(Variables,i,v);
            }
            catch (msException& e) {
                e.addContext("boost::shared_ptr<msTreeMapper> msGeneralizedCoordinates::setVariable(size_t i,boost::shared_ptr<msScalarVariable> v)");
                IMPACT_THROW_EXCEPTION(e);
            }
            
            return mySharedPtr();
        }

        
        /*! \brief set the value of a variable
         *
         * \param i index of the variable
         * \param val value of the variable
         */
        boost::shared_ptr<msTreeMapper> setCoordinate(size_t i,double val);

        /*! \brief set the value of the variables
         *
         * \param vals values of the variable
         */
        boost::shared_ptr<msTreeMapper> setCoordinates(const vector<double>& val);

        /*! \brief set the value of the unfreezed variables
         *
         * \param units units to interpret the vector 'vec'
         * \param vec values of the unfreezed variables; the i'th element of 'vec' has to be the i'th unfreezed value.
         */
        boost::shared_ptr<msTreeMapper> setUnfreezedValues(const msUnitsManager& units, const vector_type& vec);
        
        
        /*! \brief set the value of a particular variable
         *
         * \param i index of the variable
         * \param v normalized value between 0 and 1; 0 being the minimum of the variable, 1 the maximum
         * \return absolute value of the variable
         */
        double value(size_t i, double v) const { return getVariable(i)->value(v); };
        
        //}
        
        //!\name get the variables
        //@{
        
        //! \brief return the variables in a vector
        std::vector<boost::shared_ptr<msScalarVariable> > getVariables() const;
        
	/*! \brief return the active variables in a vector
         */	
        vector<boost::shared_ptr<msScalarVariable> > getActives() const;
	
        /*! \brief return a variable from its id
         *
         * \param str id of the variable
         */
        boost::shared_ptr<msScalarVariable> getVariable(std::string str) const;

        /*! \brief return a variable from its index
         *
         * \param i index of the variable
         */
        boost::shared_ptr<msScalarVariable> getVariable(size_t i) const;
        
        /*! \brief return a variable from its index as a constant reference
         *
         * \param i index of the variable
         */
        const msScalarVariable& getVariable_const(int i) const {return *(getVariable(i));};
        
        //! \copydoc getVariable(std::string)
        boost::shared_ptr<msScalarVariable> operator[](std::string str) const {return getVariable(str);};
        
        //! \copydoc getVariable(size_t)
        boost::shared_ptr<msScalarVariable> operator[]      (int i)       {return getVariable(i);};
        
        //! \brief same as getVariable(size_t) but return a constant pointer
        const boost::shared_ptr<msScalarVariable> operator[](int i) const {return getVariable(i);};
        //@}
        
        //!\name iterators on the variables
        //@{
        iterator begin(){return Variables.begin();}
        
        const_iterator begin()const {return Variables.begin();}
        
        iterator end(){return Variables.end();}
        
        const_iterator end()const {return Variables.end();}
        
        
        UnfreezIt beginUnfreezed() { UnfreezIt it(this); return it; }
        
        const_UnfreezIt beginUnfreezed() const { const_UnfreezIt it(this); return it; }
        
        UnfreezIt endUnfreezed()             { return UnfreezIt(); };
        
        const_UnfreezIt endUnfreezed() const { return const_UnfreezIt(); };
        //@}
        
        
        std::ostream& writePosAndField(std::ostream& out,double value);
        std::ostream& writePos(std::ostream& out);
        
        std::ostream& print(std::ostream& out)    const;
        std::ostream& abstract(std::ostream& out) const;
        
    protected:
        
        msChildren<msScalarVariable> Variables;
        
    };
    std::ostream& operator<<(std::ostream& out,const msGeneralizedCoordinates& gencoor);
    std::ostream& operator<<(std::ostream& out,const msScalarVariable& gencoor);
}
#endif // MSGENERALIZEDCOORDINATE_H
