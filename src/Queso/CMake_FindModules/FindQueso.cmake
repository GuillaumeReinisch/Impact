 

########################################################################################################
#                                       QUESO
########################################################################################################
# Defines:
# QUESO_LIBRARIES
# QUESO_INCLUDE_DIRS

#include dir

FIND_PATH(PECOSQUESO_INCLUDE_DIRS queso/queso.h
 PATHS
   ${QUESO_INCLUDE_PATH}/
   /usr/include/
          )

if (NOT PECOSQUESO_INCLUDE_DIRS)
  message (FATAL_ERROR 
" \nThe include directory of the library QUESO containing queso/queso.h has not been found.
Please set the cmake option QUESO_INCLUDE_PATH to the correct path.\n"
            )
endif (NOT PECOSQUESO_INCLUDE_DIRS)

message(STATUS "QUESO include path: ${PECOSQUESO_INCLUDE_DIRS}")


find_library (QUESO_LIBRARIES libqueso.so
  PATHS
  ${QUESO_LIBRARIES_PATH}/
  /usr/lib
)

if (NOT QUESO_LIBRARIES )
  message (FATAL_ERROR 
"\nThe library queso has not been found.
Please set the cmake option QUESO_LIBRARIES_PATH to the correct path.\n"
            )
endif (NOT QUESO_LIBRARIES )

SET (PECOSQUESO_LIBRARIES ${QUESO_LIBRARIES})

message(STATUS "QUESO library path: ${PECOSQUESO_LIBRARIES}")


