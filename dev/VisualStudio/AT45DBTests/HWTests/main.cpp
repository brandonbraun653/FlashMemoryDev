/********************************************************************************
 *   File Name:
 *       main.cpp
 *
 *   Description:
 *       Entry point for the AT45DB test suite
 *
 *   2019 | Brandon Braun | brandonbraun653@gmail.com
 ********************************************************************************/
#include <gtest/gtest.h>

using namespace ::testing;

#ifdef SW_SIM
#error Cannot have SW_SIM defined for HW tests
#endif

#ifndef HW_TEST
#error Please define HW_TEST in the preprocessor
#endif


int main( int argc, char **argv )
{
  InitGoogleTest( &argc, argv );
  ::testing::GTEST_FLAG( filter ) = "HardwareFlash.GFI_WriteRead_Binary_PageUnAligned_MultiPage*";
  return RUN_ALL_TESTS();
}
