# ====================================================
# Common
# ====================================================
set(LINK_LIBS
  adesto_inc
  aurora_inc
  Boost::boost
  chimera_inc       # Chimera public headers
  CppUTest_inc
  prj_device_target # Compiler options for target device
)

set(LIB adesto_common_tests)
add_library(${LIB} STATIC
  test_common_resources.cpp
  test_get_device_id.cpp
  test_open_close.cpp
  test_read_write_erase.cpp
)
target_link_libraries(${LIB} PRIVATE ${LINK_LIBS})
target_include_directories(${LIB} PRIVATE ${PROJECT_ROOT})
export(TARGETS ${LIB} FILE "${PROJECT_BINARY_DIR}/Adesto/${LIB}.cmake")