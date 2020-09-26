/********************************************************************************
 *  File Name:
 *    test_get_device_id.cpp
 *
 *  Description:
 *    Common test for grabbing the device identifiers
 *
 *  2020 | Brandon Braun | brandonbraun653@gmail.com
 *******************************************************************************/

/* CppUTest Includes  */
#include <CppUTest/TestHarness.h>

TEST_GROUP(DeviceID)
{
};


TEST(DeviceID, Readable)
{
  CHECK_EQUAL( true, true ); // Passes
  FAIL("Fail me!");
}