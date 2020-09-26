
# cmake -DTOOLCHAIN=arm_none_eabi -DFREERTOS_THREADS=ON -DDEVICE_TARGET=stm32l432kb -DCMAKE_BUILD_TYPE=Debug --configure ../


set(TGT1 test_at25)
add_executable(${TGT1} "${PROJECT_ROOT}/tests/hardware/${TGT1}.cpp")
target_link_libraries(${TGT1} PRIVATE
  # Public Includes
  adesto_inc
  aurora_inc
  Boost::boost
  chimera_inc
  freertos_inc
  lfs_inc
  thor_inc
  type_safe_inc

  # Static Libraries
  CppUTest
  adesto_common_tests
  adesto_core
  chimera_src
  freertos_cfg
  freertos_core
  freertos_heap
  freertos_port
  lib_adesto_at25
  thor_cfg_freertos
  thor_cmn_cm4
  thor_hld
  thor_lld_intf
  thor_lld_stm32l4

  # Target Properties
  prj_device_target
)