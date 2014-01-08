
from libimpact import *
import matplotlib.pyplot as plt
from Dialogs import simpleDialogs
import itertools

class msPlotter1D( msVisualizer ):
    """
    Class to plot multiple scalar functions with respect to one of their coordinates.
    """
    def __init__(self):
        super( msPlotter1D , self).__init__()
        self._constructVar_("msPlotter1D","plotter1D","1D plotter");

    def _initialize_(self):
        """ _initialize( (msPlotter1D) self ) -> void :
        declare and initialize childs and attributes.
        See msTreeMapper documentation.
        """
        super(msPlotter1D, self)._initialize_()
        
        self.ScalarFunctions = msChildren_msScalarFunction.New()
        self._declareChildren_(self.ScalarFunctions,"ScalarFunctions")
        
        self.Sampling = msAttribute_int(100)
        self._declareAttribute_(self.Sampling,"Sampling")
        
        self.xLabel = msAttribute_string("xLabel")
        self._declareAttribute_(self.xLabel,"xLabel")

        self.yLabel = msAttribute_string("yLabel")
        self._declareAttribute_(self.yLabel,"yLabel")
        
        self.Indexes = msAttribute_vectorInt(msVector_int() )
        self._declareAttribute_(self.Indexes,"Indexes")
        
        self.Styles = msAttribute_vectorString( msVector_string() )
        self._declareAttribute_( self.Styles,"Styles")
        # add your declarations
     
    def _update_(self):
        """  _update( (msPlotter1D) self ) -> void :
        update the object from its childs and attributes.
        See msTreeMapper documentation.
        """
        super(msPlotter1D, self)._update_()
    
    
    @staticmethod
    def New(  ):
        """
        New(  ) -> msPlotter1D :
        New plotter
        """
        Self = msPlotter1D()
        Self._initialize_()
        Self._update_()
        return Self
    
    def setSampling(self,val):
        """ setSampling( (msPlotter1D) self, (int) val ) -> msPlotter1D :
            set the sampling size of the x absciss.
            arg2: number of samples
            """
        self.Sampling.setValue(val)
        return self
    
    def addFunction(self,fct):
        """ addFunction(  (msPlotter1D) self , (msScalarFunction)fct ) -> msPlotter1D :
        add a function to the plotter
        """
        self._addElementToChildren_(self.ScalarFunctions,fct)
        self.Styles.getRefOnValue().append("bo")
        self.Indexes.getRefOnValue().append(0)
        
        if not fct.hasParent():
	  fct.setAffiliation(self)
	  
        return self

    def setStyleIndex(self,i,label,index):
        """ setStyleIndex( (msPlotter1D) self, (int) i, (str) label, (int) index) -> msPlotter1D :
            set the plotting style of a function.
            arg2: index of the function
            arg3: label of the plotting style (see )
            arg4: index of the coordinates scanned
        """
        self.Styles.getRefOnValue()[i] = str(label)
        self.Indexes.getRefOnValue()[i] = index

        return self

    def plot(self):
        """
            plot( (msPlotter1D) self ) -> msPlotter1D :
            plot the functions
        """
        fig, axes = plt.subplots(1, 1)
        zip = itertools.izip(self.ScalarFunctions.getElements(),self.Styles.getValue())
        
        for fct,style in zip:
            axes.plot(fct.getXSample1d(0,self.Sampling.getValue()),
                      fct.getYSample1d(0,self.Sampling.getValue()),
                      style)
        axes.set_xlabel(self.xLabel.getValue())
        axes.set_ylabel(self.yLabel.getValue())
        fig.show()
        return self



