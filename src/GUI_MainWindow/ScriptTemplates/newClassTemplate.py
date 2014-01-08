"""
Created the $date 

@author:
"""
from libimpact import *

class $classname( $parentclass ):
    """
    $classdoc
    """
    def __init__(self):
        super( $classname , self).__init__()
        self._constructVar_("$classname","$defaultVarName","$defaultId")

    @staticmethod
    def New():
        """
        New() -> $classname
        Create a new $classname object.
        """
        Self = $classname()
        Self._initialize_()
        Self._update_()
        return Self
    

    def _update_(self):
        """  _update_( ($classname) self ) -> void :
        update the object from its childs and attributes.
        See msTreeMapper documentation.
        """
        super($classname, self)._update_()
        # update your variables
        
    def _initialize_(self):
        """ _initialize_( ($classname) self ) -> void :
        declare and initialize childs and attributes.
        See msTreeMapper documentation.
        """
        super($classname, self)._initialize_()

