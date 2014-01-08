
from libimpact import *
from xml.dom import minidom

class msXmlWritter( msTreeMapper ):
    """
    The class msXmlWritter is used to save a msTreeMapper derived object
    in a xml file. It used by the menu File->save database.
    """
    def __init__(self):
        super( msXmlWritter , self).__init__()
        
    def _initialize_(self):
        """ declare and initialize childs and attributes of the object.
        See msTreeMapper documentation.
        """
        super(msXmlWritter, self)._initialize_()
        # add your declarations
     
    def _update_(self):
        
        """ update childs and attributes of the object, when then internal
        state of self is changed.
        See msTreeMapper documentation.
        """
        super(msXmlWritter, self)._update_()
        # update your variables
        
    @staticmethod
    def New(  ):
        """
        New(  ) -> msTreeMapper
        create a new writter
        
        """
        Self = msXmlWritter()
        Self._initialize_()
        Self._update_()
        return Self
   
    def write(self,node):
        """
        print(  (msXmlWritte) self , (msTreeMapper)  arg1 ) -> void.

        print the node given
        arg1: node to print 
        """
        rootxml = minidom.Document()
        self.__writeChild(node,rootxml,rootxml)
        print rootxml.toprettyxml()

    def save(self,node,filename):
        """
        saveAs(  (msXmlWritte) self , (msTreeMapper)  arg1, (string) arg2 ) -> void.

        save in a file the node given 
        arg1: node to print 
        arg2: filename
        """
        rootxml = minidom.Document()
        self.__writeChild(node,rootxml,rootxml,0)
        with open(filename, "w") as f:
            f.write(rootxml.toprettyxml()) 
    
    def __writeChild( self, node, parent, doc, nodeparent ):
   
        children = node.getChildren()
        attributs = node.getAttributes()
        nodename = "workspace" 
        if(not nodeparent == 0):
            nodename = nodeparent.getVariableName(node) 
        newchild = doc.createElement(node.getType().split(":")[-2] + "-" + nodename )
        parent.appendChild(newchild)

        for att in attributs:
                newchild.setAttribute(att.split("=")[0][:-1],att.split("=")[1][1:])
            
        for child in children:

            if( child.isToBeSaved() and not child.isAffiliatedTo(node) ):
                newSL = doc.createElement(child.getType().split(":")[-2] + "-" + node.getVariableName(child))
                newSL.setAttribute("PointerTo", node.getSymbolicLinkPath(child)[:-1] )
                newchild.appendChild(newSL)

            if( child.isToBeSaved() and child.isAffiliatedTo(node) ):
                self.__writeChild( child, newchild, doc,node)
