project(OFDM)

cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 23)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB SOURCES 
    ${SOURCE_DIR} "source/*.h"    
    ${SOURCE_DIR} "source/*.cpp"
    ${SOURCE_DIR} "source/tests/*.h"
    ${SOURCE_DIR} "source/tests/*.cpp"
)

find_package(Gnuradio "3.10.5" REQUIRED COMPONENTS runtime fft filter)
find_package(iir REQUIRED)
find_package(Tracy REQUIRED)

# add_definitions(-DTRACY_ENABLE)

add_executable(ofdm ${SOURCES})

target_link_libraries(ofdm PRIVATE 
    fftw3
    gnuradio::gnuradio-runtime 
    gnuradio::gnuradio-fft 
    gnuradio::gnuradio-filter
    iir::iir
    Tracy::TracyClient
)