#  PODOFO_FOUND - system has Potrace
#  PODOFO_INCLUDE_DIR - the Potrace include directory
#  PODOFO_LIBRARIES - The libraries needed to use Potrace

FIND_PATH(PODOFO_INCLUDE_DIR podofo/podofo.h
   /usr/include
   /usr/local/include
)

FIND_LIBRARY(PODOFO_LIBRARY NAMES  podofo libpodofo
   PATHS
   /usr/lib
   /usr/local/lib
)

if (PODOFO_INCLUDE_DIR AND PODOFO_LIBRARY)
   set(PODOFO_FOUND TRUE)
   set(PODOFO_LIBRARIES ${PODOFO_LIBRARY})
else (PODOFO_INCLUDE_DIR AND PODOFO_LIBRARY)
   set(PODOFO_FOUND FALSE)
endif (PODOFO_INCLUDE_DIR AND PODOFO_LIBRARY)

if (PODOFO_FOUND)
   if (NOT Potrace_FIND_QUIETLY)
      message(STATUS "Found podofo: ${PODOFO_LIBRARIES}")
   endif (NOT Potrace_FIND_QUIETLY)
else (PODOFO_FOUND)
   if (NOT Potrace_FIND_QUIETLY)

 message(STATUS "don't find podofo")

   endif (NOT Potrace_FIND_QUIETLY)
endif (PODOFO_FOUND)

MARK_AS_ADVANCED(PODOFO_INCLUDE_DIR PODOFO_LIBRARIES PODOFO_LIBRARY)

