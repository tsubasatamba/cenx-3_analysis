set(TARGET correct_lightcurve)

add_executable(${TARGET}
  main.cc
  ../src/DetectedEvent.cc
  )

target_link_libraries(${TARGET}
  ${ROOT_LIB}
  )

install(TARGETS ${TARGET} LIBRARY RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/cenx-3)
