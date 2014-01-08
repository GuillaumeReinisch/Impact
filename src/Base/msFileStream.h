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


#ifndef MSFileStream_H
#define MSFileStream_H

#include <msTreeMapper.h>

namespace impact {
        
        /*! \class msFileStream
         * \brief Input output TreeMapper object in/from files
         */
        class msFileStream : public msTreeMapper
        {
                       
        protected:
            
            static msRegistrar Registrar;
            static bool  isFileStreamRegisteredInPython;
            static msRegister* createInstance() { return new msFileStream; }
            
            void registryInPython();
            
        public:
            
            msFileStream(): msTreeMapper(){};
            
            void update()  {
                
                msTreeMapper::update();
            };
            
            void initialize() {
                
                msTreeMapper::initialize();
            };
            
            static boost::shared_ptr<msFileStream> New(){
                
                boost::shared_ptr<msFileStream> T( new msFileStream );
                T->initialize();
                T->update();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msFileStream> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
        private:
            
        };
    
}
#endif

