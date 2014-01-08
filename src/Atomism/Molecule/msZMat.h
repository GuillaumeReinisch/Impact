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

/*!
 * \file msZMat.h
 * \brief Defines the class msZMat
 * \author G.Reinisch
 */

#ifndef msZMAT_H
#define msZMAT_H


#include <msEntity.h>
#include "msAtom.h"

namespace impact {
    
    namespace atomism {
        
        /*! \class msZMat
         * \brief Definition of a Z-Matrix
         *
         * The Z-Matrix class defines the internal coordinates of a system, from which
         * the description of internal motion (derived from msIntMotion) are based.
         * The class is composed by a list of Atoms (msAtom), including their name and
         * connectivity (using internal coordinates).
         * See <a href="http://en.wikipedia.org/wiki/Z-matrix_%28chemistry%29"> here </a>
         * for description of this internal coordinates type.
         *
         * Considering the following Z-Matrix of \f$ CH_3 \f$ :\n
         * C \n
         * H \f$ i_0 \f$ \f$ q_0 \f$ \n
         * H \f$ i_1 \f$ \f$ q_1 \f$  \f$ i_2 \f$ \f$ q_2 \f$ \n
         * H \f$ i_3 \f$ \f$ q_3 \f$  \f$ i_4 \f$ \f$ q_4 \f$ \f$ i_5 \f$ \f$ q_5 \f$ \n
         *
         * The associated Atoms list structure is: \n
         *     - Atoms[0]: type: msCatom
         *              - Name: C
         *     - Atoms[1]: type: msHatom
         *              - Name: H
         *              - Bond.Ref: \f$ i_0 \f$ ; Bond.Length: \f$ q_0 \f$
         *     - Atoms[2]: type: msHatom
         *              - Name: H ;
         *              - Bond.Ref: \f$ i_1 \f$ ; Bond.Length: \f$ q_1 \f$
         *              - Angle.Ref: \f$ i_2 \f$ ; Angle.Angle: \f$ q_1 \f$
         *     - Atoms[3]: type: msHatom
         *              - Name: H ;
         *              - Bond.Ref: \f$ i_3 \f$ ; Bond.Length: \f$ q_3 \f$
         *              - Angle.Ref: \f$ i_4 \f$ ; Angle.Angle: \f$ q_4 \f$
         *              - Dihedre.Ref: \f$ i_4 \f$ ; Dihedre.Angle: \f$ q_4 \f$
         *
         * The atoms recognized are listed in the inheritence diagram for msAtom, the documentation
         * also explains how to add a new type of atom.
         *
         * In terms of calculation, this class allows to compute the kinetic function for
         * a variation \f$ d\mathbf{q} \f$ of the internal coordinates: ChangeValue(double*).
         * This function is the base for the computation of the kinetic matrix of any LAM (see \ref MPA_zmat).
         *
         * In a practical point of view, this class provides a lot of functions to create, modify,
         * study, and save a Z-Matrix. A nice feature is that the Z-Matrix handle bending angle
         * superior to 180 degrees (which to use the Z-Matrix for dynamical studies).
         *
         * \anchor wikizmat
         * \xrefitem REF "Ref" "References" Z_Matrix description, http://en.wikipedia.org/wiki/Z-matrix_%28chemistry%29
         *
         *
         */
        class msZMat : public msEntity
        {
        public:
            enum varType    { Length , Angle , Dihedre , undefined };
            enum outputType { Internal , XYZ , Formal };
        private:
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isZMatRegisteredInPython;
            static msRegister* createInstance() { return new msZMat; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void initialize()     { msEntity::initialize();
                msTreeMapper::declareAttribute(IndCoors,"IndCoors");
            }
            void update(){msEntity::update();}
            

        public:
            
	    msZMat() : msEntity() {constructVar("msZMat","ZMat","zmat");PrintType=Internal;
                CenterOfGravity<<0,0,0;GlobalOrientation<<0,0,0;
            }
            
            static boost::shared_ptr<msZMat> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msZMat> msZMat::New()","");
                LOGGER_WRITE(msLogger::DEBUG, "New msZMat object")
                boost::shared_ptr<msZMat> T( new msZMat );
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msZMat> msZMat::New()");
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msZMat> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            boost::shared_ptr<msTreeMapper> set(std::string input);
            
            boost::shared_ptr<msTreeMapper> updatePositions();
            
            //! \brief set the zmat from the Cartesian coors of the Element using the current connectivity
            void setZmatFromCartesian();
            
            //! \brief set the zmat from a list of Cartesian coordinates using a chemical connectivity
            boost::shared_ptr<msTreeMapper> setFromCartesians(std::vector<std::string> names, std::vector<double> xs,std::vector<double> ys,std::vector<double> zs);
            
            void setOutputFormat(string type);
            
            void setCenterOfMass(const Eigen::Vector3d& vec){setCenterOfMass(vec[0],vec[1],vec[2]);}
            void setCenterOfMass(double x,double y, double z);
            void setEulerAngles(double x,double y,double z);
            void setRndEulerAngles();
            
            double separation(size_t i,size_t j);
            // varType getTypeOfDof(size_t i);
            
            size_t  getFirstIndiceOfAtom(size_t atom) const;
            
            size_t  PrevClosest(int at,int maxAt,int noAt=-1,int noAt1=-1);
            
            size_t noOfElectrons() const;
            
            std::ostream&  writeFormal(ostream& out) const;
            std::ostream& print(std::ostream& out) const;
            
            std::string getString() const;
            
            ~msZMat(){}
            
            outputType PrintType;
            
        private:
            
            vector<int> IndCoors;		//! vector to store the indice of the internal coordinates
            
            struct msPlan{ public:
                msPlan(){i=j=k=-1;};msPlan(int a,int b,int c){i=a; j=b; k=c;}
                int i,j,k;};
            
            void addPlanInversed(int i,int j,int k);
            bool isPlanInversed(int i,int j,int k);
            vector<msPlan> InversedPlan;
            
            //! \brief Set the reference indices of the internal coordinates
            void setChemConnectivity(); 
            
            msAtom* getAtom(size_t atom) const;
            
            Vector3d CenterOfGravity;
            Vector3d GlobalOrientation;
            
        };
        /*
         size_t  getAtomByIndice(size_t indice);
         varType getTypeOfDof(size_t i); 
         bool areLinked(int at,int targ); 
         int isBondedByCovalent(int i,int j);
         void updateBondStructure();*/
    }
    
}
#endif // msZMAT_H
