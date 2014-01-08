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


#ifndef MSATT_CHILD_H
#define MSATT_CHILD_H

#include <msRegister.h>
#include <msLogger.h>


namespace impact
{
    
    void lexical_cast( std::string in, double& obj);
    void lexical_cast( std::string in, int& obj);
    void lexical_cast( std::string in, bool& obj);
    void lexical_cast( std::string in, size_t& obj);
    void lexical_cast( std::string in, string& obj);
    void lexical_cast( std::string in, std::map<std::string , double >& obj);
    void lexical_cast( std::string in, std::map< vector<int> , double >& obj);
    void lexical_cast( std::string str, std::vector< double >& obj);
    void lexical_cast( std::string str, std::vector< int >& obj);
    void lexical_cast( std::string str, std::vector< string >& obj);
    void lexical_cast( std::string str, std::vector< std::vector<double> >& obj);
    
    
    template<class T> inline
    std::ostream& operator<<( std::ostream& out , const std::map<std::string , T >& obj) {
        
        typedef typename std::map<std::string , T >::const_iterator iterator;
        iterator it = obj.begin();
        for(;it!=obj.end();++it) out<<(*it).first<<":"<<(*it).second<<" ";
        return out;
    }
    
    template<class T> inline
    std::ostream& operator<<( std::ostream& out , const std::map<string, vector<T> >& obj) {
        
        typedef typename std::map<vector<int> , T >::const_iterator iterator;
        iterator it = obj.begin();
        for(;it!=obj.end();++it){
            out<<(*it).first<<":[";
            for(size_t i=0;i<(*it).second.size();++i){
                out<<(*it).second[i];
                if( i< (*it).second.size()-1) out<<",";
            }
            out<<"]";
        }
        return out;
    }
    
    template<class T> inline
    std::ostream& operator<<( std::ostream& out , const std::map<vector<int> , T >& obj) {
        
        typedef typename std::map<vector<int> , T >::const_iterator iterator;
        iterator it = obj.begin();
        for(;it!=obj.end();++it){
            out<<"[";
            for(size_t i=0;i<(*it).first.size();++i){
                out<<(*it).first[i];
                if( i< (*it).first.size()-1) out<<",";
            }
            out<<"]"<<":"<<(*it).second<<" ";
        }
        return out;
    }
    
    template<class T> inline
    std::ostream& operator<<( std::ostream& out , const std::vector<std::vector< T > >& obj) {
        
        typedef typename std::vector<std::vector< T > >::const_iterator iterator;
        iterator it = obj.begin();
        for(;it!=obj.end();++it){
            out<<"[";
            for(size_t i=0;i<(*it).size();++i){
                out<<(*it)[i];
                if( i< (*it).size()-1) out<<",";
            }
            out<<"] ";
        }
        return out;
    }
    
    template<class T> inline
    std::ostream& operator<<( std::ostream& out , const std::vector<T>& obj) {
        
        typedef typename  std::vector<T>::const_iterator iterator;
        iterator it = obj.begin();
        for(;it!=obj.end();++it) out<<(*it)<<" ";
        return out;
    }
    
    
    void registerAttributesInPython();
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    class msTreeMapper;
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    class msAttributeBase // : boost::noncopyable
    {
    public:
        
        msAttributeBase(){};
        
        virtual void set(std::string str){};
        
        virtual std::string get() const {return "msAttributeBase";};
    };
    
    std::ostream& operator<<( std::ostream& out , const msAttributeBase& obj);
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    template<class T>
    class  msAttribute : public  msAttributeBase
    {
        friend class msTreeMapper;
        
    public:
        
        msAttribute(T& ptr){ PtrToVariable=&ptr;
        };
        
        void set(std::string str) { lexical_cast( str , *PtrToVariable );
        }
        
        std::string get() const{ std::stringstream out;
            out<<(*PtrToVariable);
            return out.str();
        };
        
    private:
        
        T* PtrToVariable;
    };
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    template<class T>
    class  msAttributePython : public  msAttributeBase
    {
        friend class msTreeMapper;
        
    public:
        
        msAttributePython(){ Variable = boost::shared_ptr<T>(new T( ));
        };
        msAttributePython(T value){ Variable = boost::shared_ptr<T>(new T( value ));
        };
        
        boost::shared_ptr<T> getRefOnValue() { return Variable;}
        T getValue() { return *Variable;}
        
        void setValue(T value)          { Variable = boost::shared_ptr<T>(new T( value ));}
        
        void set(std::string str)       { lexical_cast( str , *Variable );
        }
        
        std::string get() const{ std::stringstream out;
            out<<*Variable;
            return out.str();
        };
        
    private:
        
        boost::shared_ptr<T> Variable;
    };
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    class msChildBase //: boost::noncopyable
    {
        friend class msTreeMapper;
        
    public:
        msChildBase(){};
        
        virtual msTreeMapper* getBasePtr() 		const {
            
            return ptr_Base.get();
        }
        
        virtual boost::shared_ptr<msTreeMapper> getSharedBasePtr() 	const {
            
            return ptr_Base;
        }
        
    protected:
        
        boost::shared_ptr<msTreeMapper> ptr_Base;
        
        virtual void set(boost::shared_ptr<msTreeMapper> elem){ ptr_Base = elem; }
        
    };
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    /** \class msChild
     * \brief  Wrapper around shared pointers to store the objects representing a child of the msTreeMapper database
     *
     * Its primary objective is to remove the public copy assignement to ensure that only msTreeMapper can change the pointer
     * adress; for the synchronization between object and database.
     */
    template<class T>
    class msChild : public msChildBase //: boost::noncopyable
    {
        friend class msTreeMapper;
        
    public:
        
        static boost::shared_ptr<msChild<T> > New() {
            
            boost::shared_ptr< msChild<T> > r( new msChild<T>() );
            return r;
        }
        
        msChild(){ptr=  boost::shared_ptr<T>(new T) ;
            ptr_Base= boost::static_pointer_cast<msTreeMapper>(ptr); };
        
        msChild(boost::shared_ptr<T> obj){
            
            ptr = obj ;
            ptr_Base= boost::static_pointer_cast<msTreeMapper>(ptr);
        };
        
        boost::shared_ptr<T> getSharedPtr() const { return ptr; };
        
        boost::shared_ptr<msTreeMapper> getSharedBasePtr() 	const {
            
            return boost::static_pointer_cast<msTreeMapper>(ptr);
        }
        
        virtual msTreeMapper* getBasePtr() 		const {return ptr.get();}
        
        
        
        T* get() 		const {return ptr.get();}
        
        T* operator->() 	const {return ptr.get();}
        
        T& operator*()  	const { return *(ptr.get()); }
        
        bool operator==(const msChild& rhs) const {return ptr==rhs.ptr; }
        
        bool operator!=(const msChild& rhs) const {return ptr!=rhs.ptr; }
        
        bool isNull() 	const { return ptr==boost::shared_ptr<T>(); }
        
    private:
        
        boost::shared_ptr<T> ptr;
        
        void set(boost::shared_ptr<msTreeMapper> elem){
            
            ptr_Base = elem;
            ptr = boost::static_pointer_cast<T>(elem);
        }
        
        msChild(T* obj){
            
            ptr = boost::shared_ptr<T>(obj);
            ptr_Base = boost::static_pointer_cast<msTreeMapper>(obj);
        };
        
        msChild& operator= (const msChild & other) {
            
            ptr = other.ptr;
            ptr_Base = other.ptr_Base;
            return *this;}
        
        msChild& operator= (const boost::shared_ptr<T> rhs) {
            
            ptr=rhs;
            ptr_Base = boost::static_pointer_cast<msTreeMapper>(rhs);
            return *this;
        }
        
        template<class V>
        msChild<V> static_child_cast() { return msChild<V>( boost::static_pointer_cast<V>(ptr) );}
    };
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    /** \class msChildrenBase
     * \brief  Defines the base class for msChildren
     */
    class msChildrenBase // : boost::noncopyable
    {
        friend class msTreeMapper;
        
    public:
        
        msChildrenBase(){};
        
        virtual size_t size() const{return 0; }
        
    private:
        
        virtual void push_back(boost::shared_ptr<msTreeMapper> elem){ cout<<"msChildrenBase"<<endl;
        }
        virtual void insert(size_t i,boost::shared_ptr<msTreeMapper> elem){ cout<<"msChildrenBase"<<endl;
        }
        virtual void set(size_t i, boost::shared_ptr<msTreeMapper> elem ){}
        
        
        
    };
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    /** \class msChildren
     * \brief  Same concepts as msChild, but for a vector of child of same base-type
     */
    template<class T>
    class msChildren : public msChildrenBase
    {
        friend class msTreeMapper;
        
    public:
        
        typedef typename std::vector< boost::shared_ptr<T> >::iterator iterator;
        
        typedef typename std::vector< boost::shared_ptr<T> >::const_iterator const_iterator;
        
        msChildren(){};
        
        static boost::shared_ptr<msChildren<T> > New() {
            
            boost::shared_ptr<msChildren<T> > r( new msChildren );
            return r;
        }
        
        size_t size() const{return Elements.size(); }
        
        iterator       begin()	   	{return Elements.begin();}
        const_iterator begin() 	const 	{return Elements.begin();}
        iterator       back()		{return Elements.back();}
        const_iterator back() 	const 	{return Elements.back();}
        iterator       end()		{return Elements.end();}
        const_iterator end() 	const 	{return Elements.end();}
        
        
        iterator       find(std::string id) {
            
            iterator it=begin();
            
            for(;it<end();++it) if( (*it)->getId()==id) return it;
            return it;
        }
        
        const_iterator find(std::string id) const {
            
            const_iterator it=begin();
            for(;it<end();++it) if( (*it)->getId()==id) return it;
            return it;
        }
        
        
        iterator find(boost::shared_ptr<T> ptr) {
            return Elements.find(ptr);
        }
        
        const_iterator find(boost::shared_ptr<T> ptr) const {
            return Elements.find(ptr);
        }
        
        msChild<T> operator[] (int i) const {return Elements[i];}
        
        std::vector<  boost::shared_ptr<T> > getElements() const { return Elements;
        }
        
        boost::shared_ptr<T> getElementFromId(std::string id) const {
            
            for(size_t i=0;i<Elements.size();i++) {
                
                if(Elements[i]->getId() == id) {
                    
                    return Elements[i];
                }
            }
            
            BOOST_THROW_EXCEPTION(msError("can not find the element of id "+id+" in the children list",
                                          "boost::shared_ptr<T> msChildren::getElementById(std::string id) const ","")
                                  );
        }
        
    private:
        
        std::vector<  boost::shared_ptr<T> > Elements;
        
        void push_back(boost::shared_ptr<msTreeMapper> elem){
            Elements.push_back( boost::static_pointer_cast<T>(elem) );
            // Elements.push_back(elem);
        }
        
        void push_back(boost::shared_ptr<T> elem){
            Elements.push_back(elem);
        }
        
        void insert(size_t i,boost::shared_ptr<msTreeMapper> elem){
            
            typename std::vector<  boost::shared_ptr<T> >::iterator it = Elements.begin() + i;
            Elements.insert(it, boost::static_pointer_cast<T>(elem) );
        }
        
        void insert(size_t i,boost::shared_ptr<T> elem){
            
            typename std::vector<  boost::shared_ptr<T> >::iterator it = Elements.begin() + i;
            Elements.insert(it,elem);
        }
        
        virtual void set(size_t i, boost::shared_ptr<msTreeMapper> elem ){ Elements[i] = boost::static_pointer_cast<T>(elem); }
        
        void clear()	   {Elements.clear(); }
        
        void resize(size_t s){Elements.resize(s); }
    };
    
}

#endif

