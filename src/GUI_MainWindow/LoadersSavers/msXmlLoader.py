
from libimpact import *
from Misc.msPlotter1D import *
import inspect
from xml.dom import minidom

class msXmlLoader( msTreeMapper ):
    """
    The class msXmlLoader is used to load a msTreeMapper derived object
    from a xml file written by msXmlWritter. It used by the menu File->open database.
    """
    def __init__(self):
        super( msXmlLoader , self).__init__()
        
    def _initialize_(self):
        """ declare and initialize childs and attributes of the object.
        See msTreeMapper documentation.
        """
        super(msXmlLoader, self)._initialize_()
        # add your declarations
     
    def _update_(self):
        
        """ update childs and attributes of the object, when then internal
        state of self is changed.
        See msTreeMapper documentation.
        """
        super(msXmlLoader, self)._update_()
        # update your variables
        
    @staticmethod
    def New(  ):
        """
        New(  ) -> msTreeMapper
        method New documentation
        
        """
        Self = msXmlLoader()
        Self._initialize_()
        Self._update_()
        return Self
   
    def open(self,mapper,filename):
        """
        open(  (msXmlWritte) self , (msTreeMapper)  arg1, (string) arg2 ) -> void.

        save in a file the node given 
        arg1: node to print 
        arg2: filename
        """
        msLogger.enterFunction("void msXmlLoader::open(self,mapper,filename)","")
        doc = minidom.parse(filename)
        node =  doc.childNodes[0]
        attributes = mapper.getAttributes()
        
        for att in attributes:
            id = att.split('=')[0][:-1]
            mapper.setAttribute(id ,str(node.getAttribute(id)))
        self.listSymLink=[]
        self.__createObject( mapper, node) 
        mapper._update_()
        self.root = mapper
        self.__updateAll( self.root)
        self.replaceBySymbolicLinks()
        msLogger.exitFunction("void msXmlLoader::open(self,mapper,filename)")
      
    def replaceBySymbolicLinks(self):
         
        msLogger.enterFunction("void msXmlLoader::replaceBySymbolicLinks(self)","")
        for symlink in self.listSymLink:
            obj = self.getObject( symlink[0], symlink[2] )
            parent = obj.getParent()
            symlink[0].addChild( obj ,str( symlink[1] ) )
            symlink[0]._update_()
            obj.setAffiliation(parent)
        msLogger.exitFunction("void msXmlLoader::replaceBySymbolicLinks(self)")


    def __updateAll(self, parent):
        msLogger.enterFunction("void msXmlLoader::__updateAll(self)","")
        for child in parent.getChildren():
            if child.isAffiliatedTo(parent):
                msLogger.write(DEBUG,"child "+child.getVariableName(parent)+" is not symbolic link, update it")
                self.__updateAll(child)
        msLogger.write(DEBUG,"update object of id "+parent.getId())
        parent._update_()
        msLogger.exitFunction("void msXmlLoader::__updateAll(self)")


    def __createObject(self, parent, xmlnode):

        msLogger.enterFunction("void msXmlLoader::__createObject(self, parent, xmlnode)","")
    
        for xmlChild in xmlnode.childNodes:
            if hasattr(xmlChild, 'tagName'):
                
                vartype = xmlChild.tagName.split('-')[0]
                varname = xmlChild.tagName.split('-')[1]
                if( xmlChild.getAttribute("PointerTo") == "" ):
                
                    msLogger.write(DEBUG,"create object from xml node "+ str(xmlChild.tagName))
                    
                    if( not parent.hasChildOfName(str(varname))):
                        msLogger.write(DEBUG,"Object not included naturally, 'New' called")
                        child = self.__newObject(xmlChild,parent)
                        parent.addChild(child,str(varname))
                    else:
                        child = parent.getChild(str(varname))
                        childType = child.getType().split(":")[-1]
                        xmlType = xmlChild.tagName.split('-')[0]
                        msLogger.write(DEBUG,"Object of type "+childType+" and variable name "+str(varname)+" included naturally")
                        if( childType !=  xmlType ):
                            msLogger.write(DEBUG,"Type "+str(xmlChild.tagName.split('-')[0])+" requested, 'New' called")
                            child = self.__newObject(xmlChild,parent)
                            parent.addChild(child,str(varname))
                    
                    msLogger.write(DEBUG,"set attributes")
                    attributes = child.getAttributes()
                    for att in attributes:
                        if(len(att.split("="))>0):
                            id = att.split('=')[0]
                            id = id.replace(" ","")
                            msLogger.write(DEBUG,"set attribute "+id)
                            value =str(xmlChild.getAttribute(id))
                            child.setAttribute(id ,value)
                    self.__createObject(child,xmlChild)
                else:
                    self.listSymLink.append( (parent, varname, xmlChild.getAttribute("PointerTo") ))

        msLogger.exitFunction("void msXmlLoader::__createObject(self, parent, xmlnode)")
    
    def findInClass(self, cls0,myobject):
      
	for cls in cls0.__subclasses__():
	    name = str(cls).split('.')[1].split('\'')[0]
            if(name == myobject):
                self.class0 =cls
                return self.class0
                    
	    self.findInClass(cls,myobject)
	
      
    def getObject( self,  parent, path):

        node = self.root
        for elem in path.split("/")[2:]:
            if( elem==".."):
                node = node.getParent()
            elif(elem!=""):
                node = node.getChild(str(elem))
        return node
        
    def __newObject(self, xmlchild,parent):
        msLogger.enterFunction("void msXmlLoader::__newObject(self, xmlchild,parent)","")
        child = 0
        try:
            child = globals()[ xmlchild.tagName.split('-')[0] ].New()
        except:
            self.class0 = 0
            self.findInClass(msTreeMapper, xmlchild.tagName.split('-')[0] )
            if(self.class0==0):
                print "can not create "+xmlchild.tagName.split('-')[0]
            child = self.class0.New()
	      
        msLogger.exitFunction("void msXmlLoader::__newObject(self, xmlchild,parent)")
    
        return child
