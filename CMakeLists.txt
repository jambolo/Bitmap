cmake_minimum_required(VERSION 3.23)
project(Bitmap VERSION 1.0.0 LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build libraries as DLLs" FALSE)
option(${PROJECT_NAME}_BUILD_DOCS "Build API documentation with Doxygen" OFF)

#########################################################################
# Build                                                                 #
#########################################################################

if(PROJECT_IS_TOP_LEVEL)
    find_package(Rect REQUIRED)
endif()

add_library(${PROJECT_NAME})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PRIVATE
        Pixel.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            ${CMAKE_CURRENT_SOURCE_DIR}/include
        FILES
            include/Bitmap/Bitmap.h
            include/Bitmap/BitmapView.h
            include/Bitmap/detail/Clip.h
            include/Bitmap/Palette.h
            include/Bitmap/PalettizedBitmap.h
            include/Bitmap/Pixel.h
)

target_link_libraries(${PROJECT_NAME} PUBLIC Rect::Rect)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Bitmap
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_EXTENSIONS OFF
    EXPORT_NAME ${PROJECT_NAME}
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>
        $<$<CXX_COMPILER_ID:MSVC>:WIN32_LEAN_AND_MEAN>
        $<$<CXX_COMPILER_ID:MSVC>:VC_EXTRALEAN>
        $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
        $<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_WARNINGS>
)

#configure_file("${PROJECT_SOURCE_DIR}/Version.h.in" "${PROJECT_BINARY_DIR}/Version.h")

#########################################################################
# Documentation                                                         #
#########################################################################

if(${PROJECT_NAME}_BUILD_DOCS AND PROJECT_IS_TOP_LEVEL)
    find_package(Doxygen QUIET OPTIONAL_COMPONENTS dot)
    if(Doxygen_FOUND)
        if(NOT DEFINED DOXYGEN_OUTPUT_DIRECTORY)
            set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/docs")
        endif()
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        doxygen_add_docs(doxygen
            COMMENT "Generating API documentation with Doxygen"
            CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        )
    else()
        message(WARNING "Doxygen not found; docs target unavailable")
    endif()
endif()

#########################################################################
# Testing                                                               #
#########################################################################

if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
    if(BUILD_TESTING)
        message(STATUS "Testing is enabled.")
        add_subdirectory(test)
    else()
        message(STATUS "Testing is disabled. Turn on BUILD_TESTING to build tests.")
    endif()
endif()

#########################################################################
# Installation                                                          #
#########################################################################

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(${PROJECT_NAME}_install_cmakedir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT ${PROJECT_NAME}-targets
    FILE "${PROJECT_NAME}Targets.cmake"
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${${PROJECT_NAME}_install_cmakedir}
)

# Create a ConfigVersion.cmake file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION "${${PROJECT_NAME}_install_cmakedir}"
)

# Install the config, configversion and custom find modules
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION "${${PROJECT_NAME}_install_cmakedir}"
)

# Exporting from the build tree
export(EXPORT ${PROJECT_NAME}-targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
    NAMESPACE ${PROJECT_NAME}::
)

if(PROJECT_IS_TOP_LEVEL)
    export(PACKAGE ${PROJECT_NAME})
endif()
