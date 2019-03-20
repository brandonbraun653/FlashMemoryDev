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

#ifndef SW_SIM
#error Please define SW_SIM in the preprocessor
#endif

#ifndef SW_TEST
#error Please define SW_TEST in the preprocessor
#endif

#ifndef GMOCK_TEST
#error Please define GMOCK_TEST in the preprocessor
#endif

int main(int argc, char **argv)
{
  InitGoogleTest(&argc, argv);
  //::testing::GTEST_FLAG( filter ) = "VirtualFlash.DirectPageRead_Null*";
  return RUN_ALL_TESTS();
}
