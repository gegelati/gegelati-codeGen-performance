cmake_minimum_required(VERSION 3.8)
project(performance_CodeGen)


# Disable C and C++ compiler extensions.
# C/CXX_EXTENSIONS are ON by default to allow the compilers to use extended
# variants of the C/CXX language.
# However, this could expose cross-platform bugs in user code or in the headers
# of third-party dependencies and thus it is strongly suggested to turn
# extensions off.
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio.*")
    # Set to 1 to activate debug compilation (0 for release)
    set(DEBUG 0)

    # Link with pthread
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

    if(${DEBUG})
        MESSAGE("Generate Debug project")
        set(CMAKE_BUILD_TYPE Debug)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug)
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -pg -Wall")
    else()
        MESSAGE("Generate Release project")
        set(CMAKE_BUILD_TYPE Release)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall")
    endif()
    #add libmath during non visual studio builds
    set(CMAKE_EXTRA_LIB m)
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# *******************************************
# *********** GEGELATI LIBRARY **************
# *******************************************

if(WIN32)
    set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    # find the gegelatilib-x.y.z folder in the lib directory.
    file(GLOB GEGELATI_ROOT_DIR "${LIBS_DIR}/gegelatilib-[\\.|0-9]*")
    set(ENV{GEGELATI_DIR} ${GEGELATI_ROOT_DIR})
endif()
find_package(GEGELATI)


if (WIN32)
    file(GLOB
            GEGELATI_DLL
            ${GEGELATI_ROOT_DIR}/bin/*.dll
            )

    MESSAGE("Copy GEGELATI DLLs into ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    if(NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio.*")
        file(COPY ${GEGELATI_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    else()
        file(COPY ${GEGELATI_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
        file(COPY ${GEGELATI_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
    endif()
endif()


# *******************************************
# ************** Executable  ****************
# *******************************************

# Create the target that will generate the C code of the TPG
file(GLOB_RECURSE
        CodeGenCompile_files
        ./src/CodeGenCompile/*.cpp
        ./src/CodeGenCompile/*.h
        ./params.json
        )
set(TargetCodeGenCompile CodeGenCompile)
set(filename TPG)
add_executable(${TargetCodeGenCompile} ${CodeGenCompile_files})
target_link_libraries(${TargetCodeGenCompile} ${GEGELATI_LIBRARIES})
target_compile_definitions(${TargetCodeGenCompile} PRIVATE ROOT_DIR="${CMAKE_SOURCE_DIR}")
target_compile_definitions(${TargetCodeGenCompile} PRIVATE TPG_FILENAME="${filename}")


set(SRC_CODEGEN ${CMAKE_CURRENT_BINARY_DIR}/src/)
file(MAKE_DIRECTORY ${SRC_CODEGEN})
set(CODEGEN ${SRC_CODEGEN}/${filename}.c ${SRC_CODEGEN}/${filename}_program.c ${SRC_CODEGEN}/${filename}.h ${SRC_CODEGEN}/${filename}_program.h)

# wrap generation of source file in a custom command + custom target
add_custom_command(OUTPUT ${CODEGEN} COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TargetCodeGenCompile} )
set(ExecCodeGen CodeGenGenerate)
add_custom_target(${ExecCodeGen} DEPENDS ${CODEGEN})
add_dependencies(${ExecCodeGen} ${TargetCodeGenCompile})

include_directories(${SRC_CODEGEN} ./src/Inference/)
file(GLOB_RECURSE
        ComparaisonInference_files
        ./src/Inference/*.cpp
        ./src/Inference/*.h
        ./params.json
        )
set(TargetInference InferenceCompare)
add_executable(${TargetInference} ${ComparaisonInference_files} ${CODEGEN})
target_link_libraries(${TargetInference} ${GEGELATI_LIBRARIES})
target_compile_definitions(${TargetInference} PRIVATE ROOT_DIR="${CMAKE_SOURCE_DIR}")
target_compile_definitions(${TargetInference} PRIVATE TPG_FILENAME="${filename}")

# set the custom target that generate the source file as a dependency of the target
add_dependencies(${TargetInference} ${ExecCodeGen})