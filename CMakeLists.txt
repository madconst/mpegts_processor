cmake_minimum_required (VERSION 2.6)

project (mpegts_processor)

FIND_PACKAGE(Boost 1.40 COMPONENTS program_options REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "-Wall")

add_executable(mpegts_analyzer mpegts_analyzer.cc)

add_library(ts_packet ts_packet.cc)
add_library(ts_pmt ts_pmt.cc)
add_library(ts_pat ts_pat.cc)
add_library(ts_processor ts_processor.cc)
add_library(ts_demuxer ts_demuxer.cc)
add_library(ts_pes ts_pes.cc)

target_link_libraries (mpegts_analyzer ${Boost_LIBRARIES} ts_processor ts_demuxer ts_packet ts_pes ts_pat ts_pmt)
