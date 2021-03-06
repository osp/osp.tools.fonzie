# cmake macro to test if we use Fontforge
#
#  FONTFORGE_FOUND - system has Fontforge
#  FONTFORGE_INCLUDE_DIR - the Fontforge include directory
#  FONTFORGE_LIBRARIES - The libraries needed to use Fontforge

FIND_PATH(FONTFORGE_INCLUDE_DIR NAMES fontforge/fontforge.h
PATHS
   /usr/include
   /usr/local/include
   /opt/local/incude
)

FIND_LIBRARY(FONTFORGE_LIBRARY NAMES  fontforge
   PATHS
   /usr/lib
   /usr/local/lib
   /opt/local/lib
)




if (FONTFORGE_INCLUDE_DIR AND FONTFORGE_LIBRARY)
   set(FONTFORGE_FOUND TRUE)
# we NEED UFO support in Fonzie
FIND_PACKAGE(LibXml2 REQUIRED)
   set(FONTFORGE_INCLUDE_DIR ${FONTFORGE_INCLUDE_DIR}/fontforge)
	IF(FF_PYTHON_MODULE)
		FIND_PACKAGE(PythonLibs REQUIRED)
		SET(FONTFORGE_LIBRARIES ${FONTFORGE_LIBRARY} ${LIBXML2_LIBRARIES} ${PYTHON_LIBRARIES})
	ELSE(FF_PYTHON_MODULE)
		SET(FONTFORGE_LIBRARIES ${FONTFORGE_LIBRARY} ${LIBXML2_LIBRARIES})
	ENDIF(FF_PYTHON_MODULE)

   message(STATUS "FF_found ${FONTFORGE_LIBRARY} ${FONTFORGE_INCLUDE_DIR}")
else (FONTFORGE_INCLUDE_DIR AND FONTFORGE_LIBRARY)
	message(STATUS "FF_notfound ${FONTFORGE_LIBRARY} ${FONTFORGE_INCLUDE_DIR}")
	set(FONTFORGE_FOUND FALSE)
	set(FONTFORGE_LIBRARIES "")
	set(FONTFORGE_INCLUDE_DIR "")
endif (FONTFORGE_INCLUDE_DIR AND FONTFORGE_LIBRARY)

if (FONTFORGE_FOUND)
   if (NOT Fontforge_FIND_QUIETLY)
      message(STATUS "Found Fontforge: ${FONTFORGE_LIBRARIES}")
   endif (NOT Fontforge_FIND_QUIETLY)
else (FONTFORGE_FOUND)
   if (NOT Fontforge_FIND_QUIETLY)

 message(STATUS "don't find Fontforge")

   endif (NOT Fontforge_FIND_QUIETLY)
endif (FONTFORGE_FOUND)

MARK_AS_ADVANCED(FONTFORGE_INCLUDE_DIR FONTFORGE_LIBRARIES FONTFORGE_LIBRARY)
