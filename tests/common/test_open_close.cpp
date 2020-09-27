/********************************************************************************
 *  File Name:
 *    test_open_close.cpp
 *
 *  Description:
 *    Common test for opening/closing a memory device
 *
 *  2020 | Brandon Braun | brandonbraun653@gmail.com
 *******************************************************************************/

/* Adesto Includes */
#include <Adesto/common.hpp>

/* Test Driver Includes */
#include <tests/common/test_common_resources.hpp>

/* CppUTest Includes  */
#include <CppUTest/TestHarness.h>

/*-------------------------------------------------------------------------------
Base Test Groups
-------------------------------------------------------------------------------*/
TEST_GROUP( IOCtl ){};

/*-------------------------------------------------
Test Cases
-------------------------------------------------*/
TEST( IOCtl, Open )
{
  /*-------------------------------------------------
  Initialize
  -------------------------------------------------*/
  auto DUT = Adesto::Testing::getDUT();

  /*-------------------------------------------------
  Call FUT
  -------------------------------------------------*/
  Aurora::Memory::Status result = DUT->open();

  /*-------------------------------------------------
  Verify:
  -------------------------------------------------*/
  CHECK( Aurora::Memory::Status::ERR_OK == result );
}

TEST( IOCtl, Close )
{
  /*-------------------------------------------------
  Initialize
  -------------------------------------------------*/
  auto DUT = Adesto::Testing::getDUT();

  /*-------------------------------------------------
  Call FUT
  -------------------------------------------------*/
  DUT->open();
  Aurora::Memory::Status result = DUT->close();

  /*-------------------------------------------------
  Verify:
  -------------------------------------------------*/
  CHECK( Aurora::Memory::Status::ERR_OK == result );
}