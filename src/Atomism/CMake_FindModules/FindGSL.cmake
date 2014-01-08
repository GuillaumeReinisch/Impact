 

########################################################################################################
#                                       GSL part
########################################################################################################
# Defines:
# GSL_LIBRARIES
# GSL_INCLUDE_DIRS

unset (GSL_INCLUDE_DIRS CACHE)

SET( TRY_INCLUDE_PATHS   
  $ENV{GSL_DIR}/include/
  $ENV{GSL_INC}/
  /usr/include/
  /opt/local/include/
  )
SET( TRY_LIB_PATHS
  $ENV{GSL_DIR}/lib
  $ENV{GSL_LIB}/
  /usr/lib
  /opt/local/lib
  )

SET( TARGET_HEADER gsl/gsl_version.h )
SET( TARGET_LIB gsl )
SET( TARGET_LIB2 gslcblas )

#-------------------
#-------------------

FIND_PATH(GSL_INCLUDE_DIRS ${TARGET_HEADER}
 PATHS
 ${TRY_INCLUDE_PATHS}
)

if (NOT GSL_INCLUDE_DIRS)
  message (FATAL_ERROR "\nThe include directory of gsl containing the file ${TARGET_HEADER} has not been found. Directory checked: ${TRY_INCLUDE_PATHS}.\nIf you don't have the libary on your computer, you can install it from http://www.gnu.org/software/gsl/. If the library is installed in another directory, you can set the environment variable GSL_DIR to the installed gsl directory or GSL_INC to its include subdirectory."
            )
endif (NOT GSL_INCLUDE_DIRS)

message(STATUS "GSL include path: ${GSL_INCLUDE_DIRS}")

#-------------------
#-------------------

find_library( GSL_LIB  ${TARGET_LIB}
 PATHS
 ${TRY_LIB_PATHS}
)

if (NOT GSL_LIB)
  message (FATAL_ERROR "\nThe lib directory of gsl containing the library ${TARGET_LIB} has not been found. Directory checked: ${TRY_LIB_PATHS}. \nIf you don't have the libary on your computer, you can install it from http://www.gnu.org/software/gsl/. If the library is installed in another directory, you can set the environment variable GSL_DIR to the installed gsl directory or GSL_LIB to its lib subdirectory."
            )
endif (NOT GSL_LIB)

#-------------------  gslcblas lib ------------------------

find_library( GSLCBLAS_LIB  ${TARGET_LIB2}
 PATHS
 ${TRY_LIB_PATHS}
)

if (NOT GSLCBLAS_LIB)
  message (FATAL_ERROR "\nThe lib directory of gsl containing the library ${TARGET_LIB2} has not been found. Directory checked: ${TRY_LIB_PATHS}. \nIf you don't have the libary on your computer, you can install it from http://www.gnu.org/software/gsl/. If the library is installed in another directory, you can set the environment variable GSL_DIR to the installed gsl directory or GSL_LIB to its lib subdirectory."
            )
endif (NOT GSLCBLAS_LIB)


SET (GSL_LIBRARIES ${GSL_LIB} ${GSLCBLAS_LIB})

message(STATUS "Found the following GSL libraries: ${GSL_LIBRARIES} ")


MARK_AS_ADVANCED( GSL_INCLUDE_DIRS  GSL_LIB  GSLCBLAS_LIB )


