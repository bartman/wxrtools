CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(wxr)
#set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE RelWithDebInfo)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
FIND_PACKAGE(PkgConfig REQUIRED)

PKG_CHECK_MODULES(GLIB2 REQUIRED glib-2.0>=2.44)
PKG_CHECK_MODULES(GIO REQUIRED gio-2.0>=2.44)
PKG_CHECK_MODULES(GIOUNIX REQUIRED gio-unix-2.0>=2.44)

LINK_DIRECTORIES(
    ${GLIB2_LIBRARY_DIRS}
    ${GIO_LIBRARY_DIRS}
    ${GIOUNIX_LIBRARY_DIRS}
)

SET(CMAKE_C_STANDARD 99)

if(CMAKE_COMPILER_IS_GNUCC)
        add_definitions(-Wall -Werror -Wno-variadic-macros)
endif(CMAKE_COMPILER_IS_GNUCC)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "ASAN enabled for debug build")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -gdwarf-3 -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-3 -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address")
endif()

include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
git_describe(GIT_REVISION)

add_definitions(-D"WXR_GIT_REFSPEC=${GIT_REFSPEC}")
add_definitions(-D"WXR_GIT_REVISION=${GIT_REVISION}")
add_definitions(-D"WXR_GIT_SHA1=${GIT_SHA1}")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)

# import blot library, this is kind a hacky, FIXME later
ADD_SUBDIRECTORY(blot)
IF(NOT BLOT_INCLUDE_DIR)
        SET(BLOT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/blot/include)
ENDIF()

ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(test)

#enable_testing()
