set(TARGET correct_cleaned_event)

add_executable(${TARGET}
  main.cc
  ../src/DetectedEvent.cc
  ../src/LightCurve.cc
  ../src/AstroUnits.cc
  ../src/CleanedEvents.cc
  )

target_link_libraries(${TARGET}
  ${ROOT_LIB}
  /usr/local/Cellar/cfitsio/3.490/lib/libcfitsio.9.dylib
  )

install(TARGETS ${TARGET} LIBRARY RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/cenx-3)
