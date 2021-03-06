/********************************************************************************
 *  File Name:
 *    test_get_device_id.cpp
 *
 *  Description:
 *    Common test for grabbing the device identifiers
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
TEST_GROUP( DeviceID ){};

/*-------------------------------------------------
Test Cases
-------------------------------------------------*/
TEST( DeviceID, Readable )
{
  /*-------------------------------------------------
  Initialize
  -------------------------------------------------*/
  auto DUT = Adesto::Testing::getDUT();

  /*-------------------------------------------------
  Call FUT
  -------------------------------------------------*/
  Aurora::Memory::Properties props = DUT->getDeviceProperties();

  /*-------------------------------------------------
  Verify:
    - Device is some kind of Adesto memory
    - Expected fields are non-zero
  -------------------------------------------------*/
  CHECK_EQUAL( Adesto::JEDEC_CODE, props.jedec );
  CHECK( props.pageSize > 0 );
  CHECK( props.numPages > 0 );
  CHECK( props.blockSize > 0 );
  CHECK( props.numBlocks > 0 );
  CHECK( props.sectorSize > 0 );
  CHECK( props.numSectors > 0 );
  CHECK( props.endAddress > 0 );
  // Don't check start address as the user might locate it at a non-zero address
}