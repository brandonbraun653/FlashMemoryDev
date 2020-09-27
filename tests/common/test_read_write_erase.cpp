/********************************************************************************
 *  File Name:
 *    test_read_write_erase.cpp
 *
 *  Description:
 *    Common test for reading/writing/erasing to the memory device
 *
 *  2020 | Brandon Braun | brandonbraun653@gmail.com
 *******************************************************************************/

/* STL Includes */
#include <stdlib.h>
#include <limits>

/* Adesto Includes */
#include <Adesto/common.hpp>

/* Aurora Includes */
#include <Aurora/memory>

/* Test Driver Includes */
#include <tests/common/test_common_resources.hpp>

/* CppUTest Includes  */
#include <CppUTest/TestHarness.h>

/*-------------------------------------------------------------------------------
Static Data
-------------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------------
Base Test Groups
-------------------------------------------------------------------------------*/
TEST_GROUP( MemoryInterfacing )
{
  void setup()
  {
    srand( Chimera::millis() );
  }

  void teardown()
  {

  }
};

/*-------------------------------------------------
Test Cases
-------------------------------------------------*/
TEST( MemoryInterfacing, ReadWriteErase_Page )
{
  using namespace Adesto::Testing;
  using namespace Aurora::Memory;

  /*-------------------------------------------------
  Initialize
  -------------------------------------------------*/
  auto dut = Adesto::Testing::getDUT();
  auto props = dut->getDeviceProperties();

  CHECK( props.pageSize == readBuffer.size() );
  CHECK( props.pageSize == writeBuffer.size() );

  /*-------------------------------------------------
  Test Case: Erase a chunk
  -------------------------------------------------*/
  {
    // Initialize
    readBuffer.fill( 0x00 );
    size_t chunk_address = chunkStartAddress( props, props.eraseChunk, 0 );
    size_t chunk_size    = chunkSize( props, props.eraseChunk );
    int bytesToRead      = chunk_size;
    auto readResult      = Status::ERR_BAD_ARG;

    // Call
    auto eraseResult = dut->erase( chunk_address, chunk_size );

    // Wait for the erase to happen
    Chimera::delayMilliseconds( 100 );

    // Verify
    while( bytesToRead )
    {
      // Read a page of memory
      readResult = dut->read( chunk_address, readBuffer.data(), props.pageSize );
      CHECK( readResult == Status::ERR_OK );

      // Make sure the data equals the erased state
      for ( auto x = 0; x < props.pageSize; x++ )
      {
        if ( readBuffer[ x ] != 0xFF )
        {
          FAIL( "Chunk not erased" );
        }
      }

      // Update read counters
      bytesToRead -= props.pageSize;
      chunk_address += props.pageSize;
    }
  }

  /*-------------------------------------------------
  Test Case 1: Read the random page and ensure it
    doesn't already equal the write buffer.
  -------------------------------------------------*/

  // for ( auto x = 0; x < writeBuffer.size(); x++ )
  // {
  //   writeBuffer[ x ] = rand() % std::numeric_limits<uint8_t>::max();
  // }

  // readResult = dut->read( chunk_address, readBuffer.data(), props.pageSize );

  // CHECK( readResult == Aurora::Memory::Status::ERR_OK );
  // CHECK( memcmp( readBuffer.data(), writeBuffer.data(), props.pageSize ) != 0 );

  // /*-------------------------------------------------
  // Test Case 2: Fill the chosen page with random data
  // -------------------------------------------------*/
  // auto write1Result = dut->write( pageAddress, writeBuffer.data(), props.pageSize );
  // CHECK( write1Result == Aurora::Memory::Status::ERR_OK );

  // Chimera::delayMilliseconds( 50 );

  // readResult = dut->read( pageAddress, readBuffer.data(), props.pageSize );
  // CHECK( readResult == Aurora::Memory::Status::ERR_OK );
  // CHECK( memcmp( readBuffer.data(), writeBuffer.data(), props.pageSize ) != 0 );


  /*-------------------------------------------------
  Test Case 3:
  -------------------------------------------------*/


  /*-------------------------------------------------
  Verify
  -------------------------------------------------*/
  // Check the memory equals the erased state
}

TEST( MemoryInterfacing, ReadWriteErase_MultiPage )
{

}

TEST( MemoryInterfacing, EraseChip )
{

}