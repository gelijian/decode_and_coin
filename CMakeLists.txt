#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(CAENdecode)
add_compile_options(-std=c++11)
#----------------------------------------------------------------------------
# Setup include directory for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(decode decode.cc ${sources} ${headers})
target_link_libraries(decode gsl gslcblas boost_system boost_filesystem)

add_executable(coin coin.cc ${sources} ${headers})
target_link_libraries(coin gsl gslcblas boost_system boost_filesystem)

configure_file(${PROJECT_SOURCE_DIR}/parameters.json 
	${PROJECT_BINARY_DIR}/parameters.json COPYONLY)
