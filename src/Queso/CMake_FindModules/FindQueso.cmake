 

########################################################################################################
#                                       QUESO
########################################################################################################
# Defines:
# QUESO_LIBRARIES
# QUESO_INCLUDE_DIRS

#include dir

FIND_PATH(PECOSQUESO_INCLUDE_DIRS uqEnvironment.h
 PATHS
  ${CMAKE_SOURCE_DIR}/../dependancies/build/queso/include
          )

if (NOT PECOSQUESO_INCLUDE_DIRS)
  message (FATAL_ERROR " \n The include directory of the library QUESO containing the file uqEnvironment.h has not been found. \n"
            )
endif (NOT PECOSQUESO_INCLUDE_DIRS)

message(STATUS "pecos-QUESO include path: ${PECOSQUESO_INCLUDE_DIRS}")



FOREACH(_canttlib grvy queso )

unset(PECOSQUESO_LIB)

find_library (QUESO_LIB_${_canttlib} ${_canttlib}
  PATHS
  ${CMAKE_SOURCE_DIR}/../dependancies/build/queso/lib
)

if (NOT QUESO_LIB_${_canttlib} )
  message (FATAL_ERROR QUESO_LIB_${_canttlib} "\n The libraries for QUESO ${_cantlib} has not been found. \n"
            )

endif (NOT QUESO_LIB_${_canttlib} )
SET (PECOSQUESO_LIBRARIES ${QUESO_LIBRARIES} ${QUESO_LIB_${_canttlib}} )
ENDFOREACH(_canttlib)


message(STATUS "QUESO library path(s): ${PECOSQUESO_LIBRARIES}")


