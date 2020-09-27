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
#include <array>
#include <limits>
#include <stdlib.h>
#include <string>

/* Adesto Includes */
#include <Adesto/common.hpp>

/* Aurora Includes */
#include <Aurora/memory>

/* Chimera Includes */
#include <Chimera/common>
#include <Chimera/serial>

/* Test Driver Includes */
#include <tests/common/test_common_resources.hpp>

/* CppUTest Includes  */
#include <CppUTest/TestHarness.h>

#define CONSOLE_WRITE( serial, buffer, length ) \
  serial->lock();                               \
  serial->write( buffer, length );              \
  serial->unlock();

/*-------------------------------------------------------------------------------
Base Test Groups
-------------------------------------------------------------------------------*/
/* clang-format off */
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

/* clang-format on */
/*-------------------------------------------------------------------------------
Test Cases
-------------------------------------------------------------------------------*/
TEST( MemoryInterfacing, ReadWriteErase_Minimal )
{
  using namespace Adesto::Testing;
  using namespace Aurora::Memory;

  /*-------------------------------------------------
  Initialize some common data
  -------------------------------------------------*/
  auto dut   = Adesto::Testing::getDUT();
  auto props = dut->getDeviceProperties();

  CHECK( props.pageSize == readBuffer.size() );
  CHECK( props.pageSize == writeBuffer.size() );

  const size_t eraseChunkIdx = 0;

  /*-------------------------------------------------------------------------------
  Test Case: Erase A Chunk
  -------------------------------------------------------------------------------*/
  {
    /*-------------------------------------------------
    Initialize
    -------------------------------------------------*/
    readBuffer.fill( 0x00 );
    size_t chunk_address = chunkStartAddress( props, props.eraseChunk, eraseChunkIdx );
    size_t chunk_size    = chunkSize( props, props.eraseChunk );
    int bytesToRead      = chunk_size;
    auto readResult      = Status::ERR_BAD_ARG;

    /*-------------------------------------------------
    Call
    -------------------------------------------------*/
    auto eraseResult = dut->erase( chunk_address, chunk_size );
    auto pendResult  = dut->pendEvent( Event::MEM_ERASE_COMPLETE, Chimera::Threading::TIMEOUT_BLOCK );

    /*-------------------------------------------------
    Verify
    -------------------------------------------------*/
    CHECK( eraseResult == Status::ERR_OK );
    CHECK( pendResult == Status::ERR_OK );

    while ( bytesToRead )
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

  /*-------------------------------------------------------------------------------
  Test Case: Write Random Data To Previously Erased Page and Read It Back
  -------------------------------------------------------------------------------*/
  {
    /*-------------------------------------------------
    Initialize
    -------------------------------------------------*/
    size_t chunk_address = chunkStartAddress( props, props.eraseChunk, eraseChunkIdx );
    size_t chunk_size    = props.pageSize;

    readBuffer.fill( 0 );
    for ( auto x = 0; x < props.pageSize; x++ )
    {
      writeBuffer[ x ] = rand() % std::numeric_limits<uint8_t>::max();
    }

    /*-------------------------------------------------
    Call
    -------------------------------------------------*/
    auto writeResult = dut->write( chunk_address, writeBuffer.data(), chunk_size );
    auto pendResult  = dut->pendEvent( Event::MEM_WRITE_COMPLETE, Chimera::Threading::TIMEOUT_BLOCK );

    /*-------------------------------------------------
    Verify
    -------------------------------------------------*/
    CHECK( writeResult == Status::ERR_OK );
    CHECK( pendResult == Status::ERR_OK );

    auto readResult = dut->read( chunk_address, readBuffer.data(), chunk_size );

    CHECK( readResult == Status::ERR_OK );
    CHECK( memcmp( readBuffer.data(), writeBuffer.data(), chunk_size ) == 0 );
  }
}


TEST( MemoryInterfacing, WriteChip )
{
  using namespace Adesto::Testing;
  using namespace Aurora::Memory;

  /*-------------------------------------------------
  Initialize some common data
  -------------------------------------------------*/
  auto dut   = Adesto::Testing::getDUT();
  auto props = dut->getDeviceProperties();

  /*-------------------------------------------------
  Erase the whole chip in prep for writing new data
  -------------------------------------------------*/
  dut->eraseChip();
  dut->pendEvent( Event::MEM_ERASE_COMPLETE, Chimera::Threading::TIMEOUT_BLOCK );

  /*-------------------------------------------------
  Write every page on the device, then read it back
  -------------------------------------------------*/
  size_t currentPage    = 0;
  size_t currentAddress = 0;

  while ( currentPage < props.numPages )
  {
    /*-------------------------------------------------
    Initialize the buffers
    -------------------------------------------------*/
    readBuffer.fill( 0 );
    for ( auto x = 0; x < props.pageSize; x++ )
    {
      writeBuffer[ x ] = rand() % std::numeric_limits<uint8_t>::max();
    }

    currentAddress = chunkStartAddress( props, Chunk::PAGE, currentPage );

    /*-------------------------------------------------
    Write the data to the current page and ensure it can
    be read back properly
    -------------------------------------------------*/
    dut->write( currentAddress, writeBuffer.data(), props.pageSize );
    dut->pendEvent( Event::MEM_WRITE_COMPLETE, Chimera::Threading::TIMEOUT_BLOCK );
    dut->read( currentAddress, readBuffer.data(), props.pageSize );
    dut->pendEvent( Event::MEM_READ_COMPLETE, Chimera::Threading::TIMEOUT_BLOCK );
    CHECK( memcmp( readBuffer.data(), writeBuffer.data(), props.pageSize ) == 0 );

    /*-------------------------------------------------
    Move on to the next page
    -------------------------------------------------*/
    currentPage++;
  }
}


TEST( MemoryInterfacing, EraseChip )
{
  using namespace Adesto::Testing;
  using namespace Aurora::Memory;

  /*-------------------------------------------------
  Initialize some common data
  -------------------------------------------------*/
  auto dut   = Adesto::Testing::getDUT();
  auto props = dut->getDeviceProperties();

  readBuffer.fill( 0 );
  for ( auto x = 0; x < props.pageSize; x++ )
  {
    writeBuffer[ x ] = rand() % std::numeric_limits<uint8_t>::max();
  }

  /*-------------------------------------------------
  Write a page to ensure the chip has some kind of
  data on it before fully erasing.
  -------------------------------------------------*/
  const size_t erase_id = 5;
  size_t chunk_address  = chunkStartAddress( props, props.eraseChunk, erase_id );

  dut->erase( props.eraseChunk, erase_id );
  dut->pendEvent( Event::MEM_ERASE_COMPLETE, Chimera::Threading::TIMEOUT_BLOCK );
  dut->write( chunk_address, writeBuffer.data(), props.pageSize );
  dut->pendEvent( Event::MEM_WRITE_COMPLETE, Chimera::Threading::TIMEOUT_BLOCK );
  dut->read( chunk_address, readBuffer.data(), props.pageSize );
  dut->pendEvent( Event::MEM_READ_COMPLETE, Chimera::Threading::TIMEOUT_BLOCK );

  CHECK( memcmp( readBuffer.data(), writeBuffer.data(), props.pageSize ) == 0 );

  /*-------------------------------------------------
  Erase the whole chip
  -------------------------------------------------*/
  auto eraseResult = dut->eraseChip();
  dut->pendEvent( Event::MEM_ERASE_COMPLETE, Chimera::Threading::TIMEOUT_BLOCK );

  /*-------------------------------------------------
  Verify the whole chip has been erased
  -------------------------------------------------*/
  auto readResult       = Status::ERR_OK;
  int bytesToRead       = props.pageSize * props.numPages;
  size_t currentAddress = 0;

  while ( bytesToRead )
  {
    readBuffer.fill( 0x00 );
    readResult = dut->read( currentAddress, readBuffer.data(), props.pageSize );
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
    currentAddress += props.pageSize;
  }
}
