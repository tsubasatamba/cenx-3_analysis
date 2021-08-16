set(TARGET extract_lightcurve_from_spectrum)

add_executable(${TARGET}
  main.cc
  ../src/Spectrum.cc
  )

target_link_libraries(${TARGET}
  ${ROOT_LIB}
  /usr/local/Cellar/cfitsio/3.490/lib/libcfitsio.9.dylib
  )

install(TARGETS ${TARGET} LIBRARY RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/cenx-3)
