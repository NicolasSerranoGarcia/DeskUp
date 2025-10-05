#The struct window_desc gets exported as a library created inside this CMakeLists.txt
add_library(window_desc_library STATIC)
target_sources(window_desc_library PUBLIC ${CMAKE_CURRENT_LIST_DIR}/window_desc.cc 
                                            ${CMAKE_CURRENT_LIST_DIR}/window_desc.h)