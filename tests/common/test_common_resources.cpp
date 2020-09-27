/********************************************************************************
 *  File Name:
 *    test_common_resources.cpp
 *
 *  Description:
 *    Common resources for the high level memory tests
 *
 *  2020 | Brandon Braun | brandonbraun653@gmail.com
 *******************************************************************************/

/* Aurora Includes */
#include <Aurora/memory>

/* Chimera Includes */
#include <Chimera/spi>
#include <Chimera/serial>

/* Adesto Includes */
#include <tests/common/test_common_resources.hpp>

/*-------------------------------------------------------------------------------
Static Data
-------------------------------------------------------------------------------*/
static Aurora::Memory::IGenericDevice_sPtr DUT;
static Chimera::SPI::Channel spiChannel;
static Chimera::Serial::Channel serialChannel;

namespace Adesto::Testing
{
  /*-------------------------------------------------------------------------------
  Public Data
  -------------------------------------------------------------------------------*/
  std::array<uint8_t, 256> readBuffer;
  std::array<uint8_t, 256> writeBuffer;
  std::array<char, 100> printBuffer;

  /*-------------------------------------------------------------------------------
  Public Functions
  -------------------------------------------------------------------------------*/
  void assignDUT( Aurora::Memory::IGenericDevice_sPtr dut )
  {
    DUT = dut;
  }


  Aurora::Memory::IGenericDevice_sPtr getDUT()
  {
    return DUT;
  }


  void assignSPIChannelConfig( const Chimera::SPI::Channel channel )
  {
    spiChannel = channel;
  }


  Chimera::SPI::Channel getSPIChannelConfig()
  {
    return spiChannel;
  }


  void assignSerialChannelConfig( const Chimera::Serial::Channel channel )
  {
    serialChannel = channel;
  }

  Chimera::Serial::Channel getSerialChannelConfig()
  {
    return serialChannel;
  }
}  // namespace Adesto::Testing
