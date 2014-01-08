/*
 <one line to give the program's name and a brief idea of what it does.>
 Copyright (C) <2011>  <Reinisch>
 
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
/*!
 * \file msQuantumInterface.h
 * \brief defines the msQuantumInterface class
 * \author G.Reinisch
 * \date 2013
 */
#ifndef msQuantumInterface_H
#define msQuantumInterface_H

#include <msUnitsManager.h>
#include <msVectorFit1d.h>

namespace impact {
    
    namespace atomism {
        
        /*!\class msQuantumInterface
         *
         * \brief Virtual class adding the interface (attributs & fcts) associated to the quantum formalism
         *
         * The virtual function 'void ComputeEigenStates(int n)' is the only one which must be redefine. It could be convenient to
         * to redefine the function writeEV(int n,ostream& out) to write the eigenvalues using an ad-hoc organization.
         */
        class msQuantumInterface
        {
            
        protected:
            static bool isQuantumInterfaceRegisteredInPython;
            
            void registryInPython(){
#if USE_PYTHON
                if( ! msQuantumInterface::isQuantumInterfaceRegisteredInPython ) {
                    
                    using namespace boost::python;
                    
                    class_<msQuantumInterface,boost::shared_ptr<msQuantumInterface> >
                    ("msQuantumInterface",
                     "Virtual interface for quantum motion.",no_init)
                    .def("computeEigenValues", &msQuantumInterface::computeEigenValues , "Compute the eigenvalues of the motion. May also compute the eigenstates, depending of the derived class")
                    .def( "eigenvalue" , 	 &msQuantumInterface::eigenvalue
                         , "return an eigenvalue. arg2: indice of the eigenvalue")
                    .def( "degenerency" , 	 &msQuantumInterface::degenerency
                         , "return the degenerency of an eigenvalue. arg2: indice of the eigenvalue")
                    .def("printEigenValues", &msQuantumInterface::printEigenValues , "Print the list of eigenvalues computed. arg2: number of eigenvalues to display")
                    .def( "getOccupencyProbability" , 	&msQuantumInterface::getOccupencyProbability
                         , "Return the occupency probability at a particular temperature. arg2: temperature");
                    
                    msQuantumInterface::isQuantumInterfaceRegisteredInPython=1;
                }
#endif
            }
            
            vector_type EigenValues;  	//!< List of the eigenvalues, computed using the virtual fct ComputeEigenStates(int n), in kcal/mol
            
            vector<int> g; 		//!< List of the degenerency, computed using the virtual fct ComputeEigenStates(int n)
            
            double Emin;
            
        public:
            
            msQuantumInterface() {R=-1;Emin=0; }
            
            virtual void computeEigenValues(){
                BOOST_THROW_EXCEPTION( msError("msQuantumInterface does not implement computeEigenValues"
                                               ,"virtual void computeEigenValues()","") );
            };
            
            //! partition function by direct count, origin of energy=0, R in same units than the energies in Eigenvalues
            double Q(double T,double R) const {
                
                double q=0;
                if( EigenValues.size()==0 )
                    BOOST_THROW_EXCEPTION( msError("No eigenvalues have been computed yet, please call 'computeEigenValues'"
                                                   ,"double msQuantumInterface::Q(double T,double R)","") );
                
                for(int i=0;i<EigenValues.size();i++)
                    
                    q+=g[i]*exp( -( EigenValues[i] - Emin) /(R*T));
                
                return q;
            }
            
            //! return the ZPE
            double ZPE() const {return EigenValues[0]; };
            
            
            /*! \brief compute the density of state and fill the given vector
             *
             *  \parameter DOS vector to fill, the size is the number of bins, the Dq attribute of its
             * first coordinates the energy grain size.
             */
            void fillDOS(msVectorFit1d& DOS) {
                
                LOGGER_ENTER_FUNCTION( "void msQuantumInterface::fillDOS(msVectorFit1d& DOS)" , "non affiliated msQuantumInterface" );
                
                double dE = DOS.getVariable(0)->getDq();
                
                if( dE== 0 ){
                    
                    msError e("The energy grain is null","void msQuantumInterface::fillDOS(msVectorFit1d& DOS)","");
                    BOOST_THROW_EXCEPTION(e);
                }
                
                stringstream out;
                out<<"Fill the DOS using dE="<<DOS.getVariable(0)->getDq()<<" and Emax="<<DOS.size()*dE<<"."
                <<" Number of eigenvalues to account:"<<EigenValues.size()<<endl;
                LOGGER_WRITE(msLogger::DEBUG,out.str());
                
                for(size_t i=0;i<DOS.size();++i) DOS[i]=0;
                
                for(int i=0; i<EigenValues.size(); i++ ){
                    cout<<i<<" "<<EigenValues[i]/dE<<" "<<DOS.size()<<endl;
                    int ind=EigenValues[i]/dE;
                    if( (ind< DOS.size()) && (i<g.size()) ) DOS[ind]+=g[i]/dE;
                }
                LOGGER_EXIT_FUNCTION2("void msQuantumInterface::fillDOS(msVectorFit1d& DOS)");
            }
            
            //! return the energy of the last eigenstate computed
            double Emax() const { return EigenValues[EigenValues.size()-1];
            }
            
            double eigenvalue(size_t n)  const {
                
                if( n >= EigenValues.size() ){
                    
                    BOOST_THROW_EXCEPTION( msError("Indice out of range. The number of eigenvalues computed is "+output::getString<double>(EigenValues.size())+
                                                   ". Call 'computeEigenValues' and/or see the parameters of the motion to increase the number of eigenvalues computed"
                                                   ,"double msQuantumInterface eigenvalue(size_t n) const ","") );
                }
                return EigenValues[n];
            }
            
            double degenerency(size_t n)  const {
                
                if( n >= g.size() ){
                    
                    BOOST_THROW_EXCEPTION( msError("Indice out of range. The number of eigenvalues computed is "+output::getString<double>(EigenValues.size())+
                                                   ". Call 'computeEigenValues' and/or see the parameters of the motion to increase the number of eigenvalues computed"
                                                   ,"double msQuantumInterface degenerency(size_t n) const ","") );
                }
                return g[n];
            }
            boost::shared_ptr<msVectorFit1d> getOccupencyProbability(double T){
                
                boost::shared_ptr<msVectorFit1d> proba = msVectorFit1d::New();
                
                proba->set(0, EigenValues.size(), 1, "" , "" );
                
                if( R==-1 )  BOOST_THROW_EXCEPTION( msError("The value of R has not been set by the derived class: derived class has to initialize R in proper units (same as those used to compute the eigenvalues)."
                                                            ,"boost::shared_ptr<msVectorFit1d> msQuantumInterface::occupencyProbability(double T)","") );
                double q = Q(T,R);
                
                for( int i=0; i<EigenValues.size(); ++i)
                    (*proba)[i] = g[i] * exp(- EigenValues[i] / ( R * T) ) / q;
                
                return proba;
            }
            
            
            void printEigenValues(size_t n)  const{ writeEV(n,cout); }
            
            /*! \brief Write the first energy eigenvalues
             *
             *   if call from this class the eigenvalue are printed one after the other. In some case it can be better to organized them (by main quantum number or whatever),
             *   in this case the function has to be redefined by the derived class.
             *  \param n : indicator of the amount of eigenvalue to display
             *  \param out : where to print
             */
            virtual ostream& writeEV(size_t n,ostream& out) const  {
                
                int width=20;
                std::string v0="eigenvalues";
                std::string v1="g";
                out<<std::setw(width)<<v0<<std::setw(width)<<v1<<std::endl;
                
                for( size_t i=0; i<fmin(n,EigenValues.size()); i++)   out<<std::setw(width)<<EigenValues[i]<<std::setw(width)<<g[i]<<endl;
                
                if( n > EigenValues.size() ){
                    
                    BOOST_THROW_EXCEPTION( msError("Indice out of range. The number of eigenvalues computed is "+output::getString<double>(EigenValues.size())+
                                                   ". Call 'computeEigenValues' and/or see the parameters of the motion to increase the number of eigenvalues computed"
                                                   ,"virtual ostream& msQuantumInterface writeEV(size_t n,ostream& out) const ","") );
                }
                return out;
            }
        protected:
            
            double R;
        };    
        
    }
}

#endif // msQuantumInterface_H
