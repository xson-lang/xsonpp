cmake_minimum_required(VERSION 3.15)

project(xsonpp VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)



option(BUILD_SHARED_LIBS "Build shared libraries" ON)

### check build shared override ###
if(DEFINED BUILD_SHARED_XSONPP)
    set(BUILD_SHARED_LIBS "${BUILD_SHARED_XSONPP}")
endif()



### define target ###
if(DEFINED XSONPP_HEADER_ONLY)
    ### header only
    add_library(${PROJECT_NAME} INTERFACE)

    target_compile_definitions(${PROJECT_NAME} INTERFACE -DXSONPP_HEADER_ONLY=1)
    target_compile_definitions(${PROJECT_NAME} INTERFACE -DLLFIO_HEADERS_ONLY=1)
    target_compile_definitions(${PROJECT_NAME} INTERFACE -DLLFIO_EXPERIMENTAL_STATUS_CODE=1)
        
    target_compile_definitions(${PROJECT_NAME} INTERFACE -DXSONPP_STATIC_DEFINE)
else()
    ### static and shared
    set(CPP_FILES
        src/parser.cpp
        src/reader.cpp
    )

    add_library(${PROJECT_NAME} ${CPP_FILES})

    target_compile_definitions(${PROJECT_NAME} PUBLIC -DLLFIO_HEADERS_ONLY=0)

    if(BUILD_SHARED_LIBS)
        target_link_libraries     (${PROJECT_NAME} PUBLIC llfio_dl)
        target_compile_definitions(${PROJECT_NAME} PUBLIC -DLLFIO_DYN_LINK)
    else()
        target_link_libraries     (${PROJECT_NAME} PUBLIC llfio_sl)
        target_compile_definitions(${PROJECT_NAME} PUBLIC -DLLFIO_STATIC_LINK)
        
        target_compile_definitions(${PROJECT_NAME} PUBLIC -DXSONPP_STATIC_DEFINE)
    endif()
    
    
    ## compiler args
    list(APPEND CLANG_ARGS "-Wall" "-Wextra")
    
    # cl (msvc)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") 
        target_compile_options(${PROJECT_NAME} PRIVATE "/W4")
    # gcc & clang
    else() 
        target_compile_options(${PROJECT_NAME} PRIVATE ${CLANG_ARGS})
        target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:RELEASE>:-Werror>")
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang") # clang only
            target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-c++98-compat-pedantic" "-Wc++17-compat-pedantic" "-fsized-deallocation")
        else() # gcc only
            target_compile_options(${PROJECT_NAME} PRIVATE "-Wc++17-compat")
        endif()
    endif()
    
    get_target_property(COMPILER_FLAGS ${PROJECT_NAME} COMPILE_OPTIONS)
    message(STATUS "Compiler options: ${COMPILER_FLAGS}")

endif()



### include directory ###
cmake_path(SET SELF_INC_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
file(TO_NATIVE_PATH "${SELF_INC_CMAKE_DIR}" SELF_INC_DIR)
target_include_directories(${PROJECT_NAME} INTERFACE "${SELF_INC_DIR}")
message(STATUS "Including self directory: ${SELF_INC_DIR}")



### export header ###
include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME} BASE_NAME XSONPP EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/include/xsonpp/export_defs.h")



### clang compile commands ###
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)



### install ###
install(TARGETS ${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY src/ DESTINATION src)

