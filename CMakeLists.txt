# Project
project(xc3sprog)
set(xc3sprog_VERSION_MAJOR 0)
set(xc3sprog_VERSION_MINOR 0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(CMAKE_RANLIB amiga-ranlib PATHS ${COMPILER_PATH})
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
set(CMAKE_CXX_FLAGS "-mcrt=nix20 -O2 -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
cmake_minimum_required(VERSION 2.6)



if(EXISTS ${PROJECT_SOURCE_DIR}/.git) 
  set(VERSION_STRING "${xc3sprog_VERSION_MAJOR}.${xc3sprog_VERSION_MINOR}-git")
else(EXISTS ${PROJECT_SOURCE_DIR}/.git)
  FIND_PACKAGE(Subversion)
  IF(Subversion_FOUND AND EXISTS ${PROJECT_SOURCE_DIR}/.svn)
    Subversion_WC_INFO(${PROJECT_SOURCE_DIR} Project)
    set(VERSION_STRING "${xc3sprog_VERSION_MAJOR}.${xc3sprog_VERSION_MINOR}-svn${Project_WC_REVISION}")
  ELSE(Subversion_FOUND AND EXISTS ${PROJECT_SOURCE_DIR}/.svn)
    set(VERSION_STRING "unknown")
  ENDIF(Subversion_FOUND AND EXISTS ${PROJECT_SOURCE_DIR}/.svn)
endif(EXISTS ${PROJECT_SOURCE_DIR}/.git)

# Create suffix to eventually install inlib64
    SET(LIB_SUFFIX "")
    SET(PACK_ARCH ".m68k")

# Package information
set(CPACK_PACKAGE_VERSION              ${VERSION_STRING})
set(CPACK_PACKAGE_CONTACT              "Uwe Bonnes <bon@elektron.ikp.physik.tu-darmstadt.de>")
set(CPACK_PACKAGE_DESCRIPTION          "JTAG Progarmming tools")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY  ${CPACK_PACKAGE_DESCRIPTION}
)

       


set(CPACK_RESOURCE_FILE_LICENSE        ${CMAKE_SOURCE_DIR}/LICENSE)

set(CPACK_SOURCE_GENERATOR             TGZ)
set(CPACK_SOURCE_IGNORE_FILES          "\\\\.svn" "build*")
set(CPACK_SOURCE_PACKAGE_FILE_NAME     ${CPACK_PACKAGE_FILE_NAME})

# Subdirectories

set(CPACK_SET_DESTDIR "ON")

set (LIBUSRP_INCLUDE_DIR "fallback/")
include_directories(${LIBUSRP_INCLUDE_DIR})


include_directories(${CMAKE_SOURCE_DIR})

#add_executable(debug debug.cpp iobase.cpp ioparport.cpp iodebug.cpp)

add_executable(bitparse bitrev.cpp bitfile.cpp bitparse.cpp)
add_executable(jedecparse  jedecparse.cpp jedecfile.cpp)
add_executable(srecparse  srecparse.cpp srecfile.cpp)
add_executable(avrfuseparse  avrfuseparse.cpp avrfusefile.cpp)

ADD_CUSTOM_COMMAND(OUTPUT devices.h
    COMMAND ${CMAKE_COMMAND} -DDEVLIST_DIR=${CMAKE_SOURCE_DIR} -P ${CMAKE_SOURCE_DIR}/devlist.cmk
    DEPENDS devlist.txt
)

ADD_CUSTOM_COMMAND(OUTPUT cables.h
    COMMAND ${CMAKE_COMMAND} -DCABLELIST_DIR=${CMAKE_SOURCE_DIR} -P ${CMAKE_SOURCE_DIR}/cablelist.cmk
    DEPENDS cablelist.txt
)

INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_BINARY_DIR})

add_library(xc3sproglib  STATIC sysfs.cpp sysfscreator.cpp sysfsvoice.cpp  
                        devicedb.cpp jtag.cpp jedecfile.cpp bitfile.cpp iodebug.cpp iosolas.cpp
			iobase.cpp  progalgxc95x.cpp utilities.cpp
		  mapfile_xc2c.cpp
			progalgspiflash.cpp bitrev.cpp
                        cabledb.cpp pdioverjtag.cpp xmega_pdi_nvm.cpp
                        ${CONDITIONAL_FILES} devices.h cables.h)

SET(CMAKE_CXX_ARCHIVE_FINISH "amiga-ranlib libxc3sproglib.a")

add_executable(xc2c_warp xc2c_warp.cpp)
target_link_libraries(xc2c_warp xc3sproglib ${CONDITIONAL_LIBS})

add_executable(detectchain detectchain.cpp cables.h devices.h)
target_link_libraries(detectchain xc3sproglib ${CONDITIONAL_LIBS} ${LIBS} )

add_executable(xc3sprog xc3sprog.cpp srecfile.cpp amiga.cpp
			devices.h)
target_link_libraries(xc3sprog xc3sproglib ${CONDITIONAL_LIBS}  ${LIBS} )

add_executable(readdna readdna.cpp devices.h)
target_link_libraries(readdna xc3sproglib ${LIBFTDI_LIBRARIES}  ${CONDITIONAL_LIBS}  ${LIBS} )


install(TARGETS xc3sprog DESTINATION bin)
install(TARGETS xc2c_warp DESTINATION bin)
install(TARGETS readdna DESTINATION bin)
install(TARGETS bitparse DESTINATION bin)
install(TARGETS jedecparse DESTINATION bin)
install(TARGETS srecparse DESTINATION bin)
install(TARGETS detectchain DESTINATION bin)
add_subdirectory(packages)
include(CPack)


add_custom_target(create_zip COMMAND
    ${CMAKE_COMMAND} -E tar "cfv" "xc3sprog.zip" --format=zip
       "${CMAKE_CURRENT_SOURCE_DIR}/xc3sprog"
       "${CMAKE_CURRENT_SOURCE_DIR}/xc2c_warp"
       "${CMAKE_CURRENT_SOURCE_DIR}/readdna"
       "${CMAKE_CURRENT_SOURCE_DIR}/bitparse"
       "${CMAKE_CURRENT_SOURCE_DIR}/jedecparse"
       "${CMAKE_CURRENT_SOURCE_DIR}/srecparse"
       "${CMAKE_CURRENT_SOURCE_DIR}/detectchain"
       )

SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${GENERATED_FILES}") 
