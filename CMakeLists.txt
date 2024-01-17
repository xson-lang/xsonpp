cmake_minimum_required(VERSION 3.15)

project(XsonCpp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)



string(CONCAT BUILD_FOR_NATIVE_ARCH_DESC
    "Specifies if program should be built for the native cpu"
    "(i.e. it's exact capabilities) of the system the program being built on."
    "DO NOT enable this if you plan to distribute the resulting library binary."
)

option(BUILD_FOR_NATIVE_ARCH ${BUILD_FOR_NATIVE_ARCH_DESC} FALSE)

option(HEADER_ONLY "Use XsonCpp as a header-only library, requiring no compilation" FALSE)


if(${HEADER_ONLY})
message(STATUS "Configuring XsonCpp as header only")
add_library(${PROJECT_NAME} INTERFACE)

target_compile_definitions(${PROJECT_NAME} INTERFACE XSON_CPP_HEADER_ONLY)
target_compile_definitions(${PROJECT_NAME} INTERFACE $<$<CONFIG:DEBUG>:_DEBUG>)
else()
set(CPP_FILES
    src/error.cpp
)

add_library(${PROJECT_NAME} SHARED ${CPP_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:DEBUG>:_DEBUG>)


list(APPEND CLANG_ARGS "-Wall" "-Wextra")

if(${BUILD_FOR_NATIVE_ARCH})
    list(APPEND CLANG_ARGS "-march=native")
else()
    list(APPEND CLANG_ARGS "-mtune=generic")
endif()

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


get_target_property(COMPILER_FLAGS ${PROJECT_NAME} COMPILE_OPTIONS)
message(STATUS "Compiler options: ${COMPILER_FLAGS}")
endif()

cmake_path(SET INCLUDE_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}\\include")
file(TO_NATIVE_PATH "${INCLUDE_CMAKE_DIR}" INCLUDE_DIR)
if(${HEADER_ONLY})
target_include_directories(${PROJECT_NAME} INTERFACE "${INCLUDE_DIR}")
else()
target_include_directories(${PROJECT_NAME} PUBLIC "${INCLUDE_DIR}")
endif()
message(STATUS "Including local directory: ${INCLUDE_DIR}")

