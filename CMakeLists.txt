cmake_minimum_required(VERSION 3.15)

project(XsonCpp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)


set(PROJECT_NAME_HEADER_ONLY "${PROJECT_NAME}_HO")
set(PROJECT_NAME_STATIC      "${PROJECT_NAME}_S")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

string(CONCAT BUILD_FOR_NATIVE_ARCH_DESC
    "Specifies if program should be built for the native cpu"
    "(i.e. it's exact capabilities) of the system the program being built on."
    "DO NOT enable this if you plan to distribute the resulting library binary."
)

option(BUILD_FOR_NATIVE_ARCH ${BUILD_FOR_NATIVE_ARCH_DESC} OFF)


#header only
add_library(${PROJECT_NAME_HEADER_ONLY} INTERFACE)

target_compile_definitions(${PROJECT_NAME_HEADER_ONLY} INTERFACE XSON_CPP_HEADER_ONLY)
target_compile_definitions(${PROJECT_NAME_HEADER_ONLY} INTERFACE $<$<CONFIG:DEBUG>:_DEBUG>)


#static and shared
set(CPP_FILES
    src/error.cpp
)

add_library(${PROJECT_NAME} SHARED ${CPP_FILES})
add_library(${PROJECT_NAME_STATIC} STATIC ${CPP_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "")
set_target_properties(${PROJECT_NAME_STATIC} PROPERTIES COMPILE_FLAGS "")


target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:DEBUG>:_DEBUG>)
target_compile_definitions(${PROJECT_NAME_STATIC} PRIVATE $<$<CONFIG:DEBUG>:_DEBUG>)


list(APPEND CLANG_ARGS "-Wall" "-Wextra")

if(${BUILD_FOR_NATIVE_ARCH})
    list(APPEND CLANG_ARGS "-march=native")
else()
    list(APPEND CLANG_ARGS "-mtune=generic")
endif()

foreach(PROJECT IN ITEMS ${PROJECT_NAME} ${RPOJECT_NAME_STATIC})
#cl (msvc)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") 
    target_compile_options(${PROJECT_NAME} PRIVATE "/W4" "$<$<CONFIG:RELEASE>:/O2>" "$<$<CONFIG:DEBUG>:/EHa>" "$<$<CONFIG:RELEASE>:/EHsc>")

#clang-cl
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC") 
    list(TRANSFORM CLANG_ARGS PREPEND "/clang:")
    target_compile_options(${PROJECT_NAME} PRIVATE ${CLANG_ARGS})
    target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:RELEASE>:/clang:-Werror>" "$<$<CONFIG:RELEASE>:/clang:-Ofast>")
    target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:/EHa>" "$<$<CONFIG:RELEASE>:/EHsc>")

#gcc & clang
else() 
    target_compile_options(${PROJECT_NAME} PRIVATE ${CLANG_ARGS})
    target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:RELEASE>:-Werror>" "$<$<CONFIG:RELEASE>:-Ofast>")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang") #clang only
        target_compile_options(${PROJECT_NAME} PRIVATE "-stdlib=libc++" "-Wno-c++98-compat-pedantic" "-Wc++17-compat-pedantic")
	else() #gcc only
        target_compile_options(${PROJECT_NAME} PRIVATE "-Wc++17-compat")
    endif()
endif()
endforeach()

get_target_property(COMPILER_FLAGS ${PROJECT_NAME} COMPILE_OPTIONS)
message(STATUS "Compiler options: ${COMPILER_FLAGS}")

cmake_path(SET SELF_INC_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
file(TO_NATIVE_PATH "${SELF_INC_CMAKE_DIR}" SELF_INC_DIR)
target_include_directories(${PROJECT_NAME_HEADER_ONLY} INTERFACE "${SELF_INC_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${SELF_INC_DIR}")
target_include_directories(${PROJECT_NAME_STATIC} PUBLIC "${SELF_INC_DIR}")
message(STATUS "Including self directory: ${SELF_INC_DIR}")
    


target_compile_definitions(${PROJECT_NAME_HEADER_ONLY} INTERFACE -DLLFIO_HEADERS_ONLY=1)
target_compile_definitions(${PROJECT_NAME_HEADER_ONLY} INTERFACE -DLLFIO_EXPERIMENTAL_STATUS_CODE=1)



target_compile_definitions(${PROJECT_NAME} PUBLIC -DLLFIO_HEADERS_ONLY=0)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DLLFIO_DYN_LINK)


target_compile_definitions(${PROJECT_NAME_STATIC} PUBLIC -DLLFIO_HEADERS_ONLY=0)
target_compile_definitions(${PROJECT_NAME_STATIC} PUBLIC -DLLFIO_STATIC_LINK)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME} BASE_NAME XSONCPP EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/include/export_defs.h")
target_compile_definitions(${PROJECT_NAME_STATIC} PUBLIC -DXSONCPP_STATIC_DEFINE)

target_link_libraries(${PROJECT_NAME} PUBLIC llfio_dl)
target_link_libraries(${PROJECT_NAME_STATIC} PUBLIC llfio_sl)

configure_file(${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/build/compile_commands.json COPYONLY)

