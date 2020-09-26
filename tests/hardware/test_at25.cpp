/********************************************************************************
 *  File Name:
 *    test_lfs_integration.cpp
 *
 *  Description:
 *    HIL integration testing with Little FS. This requires several pieces of
 *    hardware:
 *      1. Development board that exposes an SPI port.
 *      2. A flash driver of some kind
 *      3. NOR flash memory chip with an SPI interface.
 *
 *  2020 | Brandon Braun | brandonbraun653@gmail.com
 *******************************************************************************/

/* STL Includes */
#include <array>
#include <cstring>

/* Boost Includes */
#include <boost/circular_buffer.hpp>

/* Chimera Includes */
#include <Chimera/common>
#include <Chimera/gpio>
#include <Chimera/serial>
#include <Chimera/spi>
#include <Chimera/thread>

/* Aurora Includes */
#include <Aurora/memory>

/* Little FS Includes */
#include "lfs.h"

/* Memory Driver Includes */
#include <Adesto/at25/at25_driver.hpp>

/* Test Framework Includes */
#include <CppUTest/CommandLineTestRunner.h>

/*-------------------------------------------------------------------------------
Constants
-------------------------------------------------------------------------------*/
static constexpr Chimera::SPI::Channel spiChannel       = Chimera::SPI::Channel::SPI1;
static constexpr Chimera::Serial::Channel serialChannel = Chimera::Serial::Channel::SERIAL1;

/*-------------------------------------------------------------------------------
Forward Declarations
-------------------------------------------------------------------------------*/
static void test_thread( void *arg );
static void initializeSPI();
static void initializeSerial();

/*-------------------------------------------------------------------------------
Static Data
-------------------------------------------------------------------------------*/
static std::array<uint8_t, 500> buffer;

/*-------------------------------------------------
Serial Driver Configuration
-------------------------------------------------*/
// Length of the hardware buffer for transceiving a Serial message
static constexpr size_t HWBufferSize = 128;

// Length of the user buffer for queueing multiple messages
static constexpr size_t CircularBufferSize = 2 * HWBufferSize;

// Serial Transmit Buffers
static std::array<uint8_t, HWBufferSize> sTXHWBuffer;
static boost::circular_buffer<uint8_t> sTXCircularBuffer( CircularBufferSize );

// Serial Recieve Buffers
static std::array<uint8_t, HWBufferSize> sRXHWBuffer;
static boost::circular_buffer<uint8_t> sRXCircularBuffer( CircularBufferSize );

/*-------------------------------------------------------------------------------
Public Data
-------------------------------------------------------------------------------*/
Adesto::AT25::Driver DUT;

/*-------------------------------------------------------------------------------
Public Functions
-------------------------------------------------------------------------------*/
/**
 *  Entry point to the tests. Initializes hardware, starts the test.
 *  Will never exit.
 *
 *  @return int
 */
int main()
{
  using namespace Chimera::Threading;
  ChimeraInit();

  Thread testing;
  testing.initialize( test_thread, nullptr, Priority::LEVEL_3, STACK_KILOBYTES( 10 ), "test" );
  testing.start();

  startScheduler();
  return 0;
}


/*-------------------------------------------------------------------------------
Static Functions
-------------------------------------------------------------------------------*/
static void initializeSPI()
{
  /*-------------------------------------------------
  Initialize the SPI driver
  -------------------------------------------------*/
  Chimera::SPI::DriverConfig cfg;
  cfg.clear();
  cfg.validity = true;

  cfg.HWInit.bitOrder    = Chimera::SPI::BitOrder::MSB_FIRST;
  cfg.HWInit.clockFreq   = 8000000;
  cfg.HWInit.clockMode   = Chimera::SPI::ClockMode::MODE0;
  cfg.HWInit.dataSize    = Chimera::SPI::DataSize::SZ_8BIT;
  cfg.HWInit.hwChannel   = spiChannel;
  cfg.HWInit.txfrMode    = Chimera::SPI::TransferMode::INTERRUPT;
  cfg.HWInit.controlMode = Chimera::SPI::ControlMode::MASTER;
  cfg.HWInit.csMode      = Chimera::SPI::CSMode::MANUAL;
  cfg.HWInit.validity    = true;

  // Chip Select
  cfg.externalCS       = false;
  cfg.CSInit.alternate = Chimera::GPIO::Alternate::NONE;
  cfg.CSInit.drive     = Chimera::GPIO::Drive::OUTPUT_PUSH_PULL;
  cfg.CSInit.pin       = 15;
  cfg.CSInit.port      = Chimera::GPIO::Port::PORTC;
  cfg.CSInit.pull      = Chimera::GPIO::Pull::PULL_UP;
  cfg.CSInit.state     = Chimera::GPIO::State::HIGH;
  cfg.CSInit.threaded  = false;
  cfg.CSInit.validity  = true;

  // SCK
  cfg.SCKInit.alternate = Chimera::GPIO::Alternate::SPI1_SCK;
  cfg.SCKInit.drive     = Chimera::GPIO::Drive::ALTERNATE_PUSH_PULL;
  cfg.SCKInit.pin       = 5;
  cfg.SCKInit.port      = Chimera::GPIO::Port::PORTA;
  cfg.SCKInit.threaded  = false;
  cfg.SCKInit.validity  = true;

  // MISO
  cfg.MISOInit.alternate = Chimera::GPIO::Alternate::SPI1_MISO;
  cfg.MISOInit.drive     = Chimera::GPIO::Drive::ALTERNATE_PUSH_PULL;
  cfg.MISOInit.pin       = 6;
  cfg.MISOInit.port      = Chimera::GPIO::Port::PORTA;
  cfg.MISOInit.threaded  = false;
  cfg.MISOInit.validity  = true;

  // MOSI
  cfg.MOSIInit.alternate = Chimera::GPIO::Alternate::SPI1_MOSI;
  cfg.MOSIInit.drive     = Chimera::GPIO::Drive::ALTERNATE_PUSH_PULL;
  cfg.MOSIInit.pin       = 7;
  cfg.MOSIInit.port      = Chimera::GPIO::Port::PORTA;
  cfg.MOSIInit.threaded  = false;
  cfg.MOSIInit.validity  = true;

  auto spi = Chimera::SPI::getDriver( spiChannel );
  spi->init( cfg );
}

static void initializeSerial()
{
  using namespace Chimera::Serial;
  using namespace Chimera::Hardware;

  /*------------------------------------------------
  Configuration info for the serial object
  ------------------------------------------------*/
  IOPins pins;
  pins.tx.alternate = Chimera::GPIO::Alternate::USART1_TX;
  pins.tx.drive     = Chimera::GPIO::Drive::ALTERNATE_PUSH_PULL;
  pins.tx.pin       = 9;
  pins.tx.port      = Chimera::GPIO::Port::PORTA;
  pins.tx.pull      = Chimera::GPIO::Pull::NO_PULL;
  pins.tx.threaded  = true;
  pins.tx.validity  = true;

  pins.rx.alternate = Chimera::GPIO::Alternate::USART1_RX;
  pins.rx.drive     = Chimera::GPIO::Drive::ALTERNATE_PUSH_PULL;
  pins.rx.pin       = 10;
  pins.rx.port      = Chimera::GPIO::Port::PORTA;
  pins.rx.pull      = Chimera::GPIO::Pull::NO_PULL;
  pins.rx.threaded  = true;
  pins.rx.validity  = true;


  Config cfg;
  cfg.baud     = 115200;
  cfg.flow     = FlowControl::FCTRL_NONE;
  cfg.parity   = Parity::PAR_NONE;
  cfg.stopBits = StopBits::SBITS_ONE;
  cfg.width    = CharWid::CW_8BIT;

  /*------------------------------------------------
  Create the serial object and initialize it
  ------------------------------------------------*/
  auto result = Chimera::Status::OK;
  auto Serial = Chimera::Serial::getDriver( serialChannel );

  if ( !Serial )
  {
    Chimera::insert_debug_breakpoint();
  }

  result |= Serial->assignHW( serialChannel, pins );
  result |= Serial->configure( cfg );
  result |= Serial->enableBuffering( SubPeripheral::TX, &sTXCircularBuffer, sTXHWBuffer.data(), sTXHWBuffer.size() );
  result |= Serial->enableBuffering( SubPeripheral::RX, &sRXCircularBuffer, sRXHWBuffer.data(), sRXHWBuffer.size() );
  result |= Serial->begin( PeripheralMode::INTERRUPT, PeripheralMode::INTERRUPT );
}

/**
 *  Primary thread for intializing test resources and then executing tests.
 *  Will never exit.
 *
 *  @param[in]  arg     Unused
 *  @return void
 */
static void test_thread( void *arg )
{
  /*-------------------------------------------------
  Configure the output style:
    - Colorized
    - Verbose
    - JUnit
  -------------------------------------------------*/
  const char *av_override[] = { "-c", "-v", "-ojunit" };

  /*-------------------------------------------------
  Initialize HW Resources
  -------------------------------------------------*/
  initializeSPI();
  initializeSerial();

  /*-------------------------------------------------
  Initialize the Adesto Driver
  -------------------------------------------------*/
  Adesto::AT25::DeviceInfo info;
  memset( &info, 0, sizeof( info ) );

  auto mem = Adesto::AT25::Driver();
  mem.configure( spiChannel );
  mem.readDeviceInfo( info );

  // Test a read
  buffer.fill( 0 );
  mem.read( 0, buffer.data(), buffer.size() );


  /*-------------------------------------------------
  Break into the debugger
  -------------------------------------------------*/
  volatile int rcode = CommandLineTestRunner::RunAllTests( 2, av_override );
  while ( 1 )
  {
    Chimera::insert_debug_breakpoint();
    info.density = Adesto::DENSITY_32MBIT;
  }
}