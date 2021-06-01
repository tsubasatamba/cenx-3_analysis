set(TARGET calculate_pulse_fraction)

add_executable(${TARGET}
  main.cc
  ../src/PulseProfile.cc
  )

target_link_libraries(${TARGET}
  ${ROOT_LIB}
  /usr/local/Cellar/cfitsio/3.490/lib/libcfitsio.9.dylib
  )

install(TARGETS ${TARGET} LIBRARY RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/cenx-3)
