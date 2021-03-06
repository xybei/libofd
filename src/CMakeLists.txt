PROJECT(libofd)

AUX_SOURCE_DIRECTORY(. LIBOFD_SRCS)
AUX_SOURCE_DIRECTORY(./ofd LIBOFD_SRCS)

# -------- libpng --------
if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    SET(LIBPNG png)
endif ()

ADD_LIBRARY(ofd SHARED ${LIBOFD_SRCS})
SET_TARGET_PROPERTIES(ofd PROPERTIES CLEAN_DIRECT_OUTPUT 1)

ADD_LIBRARY(ofd_static STATIC ${LIBOFD_SRCS})
SET_TARGET_PROPERTIES(ofd_static PROPERTIES OUTPUT_NAME "ofd")
SET_TARGET_PROPERTIES(ofd_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# -------- LibSodium --------
FIND_PACKAGE(LibSodium REQUIRED)
INCLUDE_DIRECTORIES(${LIBSODIUM_INCLUDE_DIRS})

### -------- Poppler --------
#FIND_PACKAGE(Poppler REQUIRED)
#INCLUDE_DIRECTORIES(${POPPLER_INCLUDE_DIRS})

TARGET_LINK_LIBRARIES(ofd ${LIBOFD_DEPENDICES} ${LIBSODIUM_LIBRARIES}  ${POPPLER_LIBRARIES} ${LIBPNG} utils)
TARGET_LINK_LIBRARIES(ofd_static ${LIBOFD_DEPENDICES} ${LIBSODIUM_LIBRARIES} ${POPPLER_LIBRARIES} ${LIBPNG} )

INSTALL(TARGETS ofd_static
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

