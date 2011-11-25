# cmake macro to test if we use svnclient
#
#  SVNClient_FOUND - system has svnclient
#  SVNClient_INCLUDE_DIR - the svnclient include directory
#  SVNClient_LIBRARIES - The libraries needed to use svnclient

FIND_PATH(SVNClient_INCLUDE_DIR subversion-1/svn_client.h
   /usr/include
   /usr/local/include
)

FIND_LIBRARY(SVNClient_LIBRARY NAMES  svn_client-1 libsvn_client-1
   PATHS
   /usr/lib
   /usr/local/lib
)

if (SVNClient_INCLUDE_DIR AND SVNClient_LIBRARY)
   set(SVNClient_FOUND TRUE)
   set(SVNClient_LIBRARIES ${SVNClient_LIBRARY})
else (SVNClient_INCLUDE_DIR AND SVNClient_LIBRARY)
   set(SVNClient_FOUND FALSE)
endif (SVNClient_INCLUDE_DIR AND SVNClient_LIBRARY)

if (SVNClient_FOUND)
   if (NOT svnclient_FIND_QUIETLY)
      message(STATUS "Found svnclient: ${SVNClient_LIBRARIES}")
   endif (NOT svnclient_FIND_QUIETLY)
else (SVNClient_FOUND)
   if (NOT svnclient_FIND_QUIETLY)

 message(STATUS "don't find svnclient")

   endif (NOT svnclient_FIND_QUIETLY)
endif (SVNClient_FOUND)

MARK_AS_ADVANCED(SVNClient_INCLUDE_DIR SVNClient_LIBRARIES SVNClient_LIBRARY)

