/********************************************************************************
 *  File Name:
 *    test_common_resources.hpp
 *
 *  Description:
 *    Common resources for running agnostic memory device tests
 *
 *  2020 | Brandon Braun | brandonbraun653@gmail.com
 *******************************************************************************/

#pragma once
#ifndef ADESTO_MEMORY_TESTS_COMMON_HPP
#define ADESTO_MEMORY_TESTS_COMMON_HPP

/* Aurora Includes */
#include <Aurora/memory>

/* Chimera Includes */
#include <Chimera/serial>
#include <Chimera/spi>
#include <Chimera/thread>

namespace Adesto::Testing
{
  /*-------------------------------------------------------------------------------
  External Data
  -------------------------------------------------------------------------------*/
  extern std::array<uint8_t, 256> readBuffer;
  extern std::array<uint8_t, 256> writeBuffer;
  extern std::array<char, 100> printBuffer;

  /*-------------------------------------------------------------------------------
  Public Functions
  -------------------------------------------------------------------------------*/
  /**
   *  Assigns the memory device being tested
   *
   *  @param[in]  dut     The DUT
   *  @return void
   */
  void assignDUT( Aurora::Memory::IGenericDevice_sPtr dut );

  /**
   *  Gets the device that is under test
   *
   *  @return Aurora::Memory::IGenericDevice_sPtr&
   */
  Aurora::Memory::IGenericDevice_sPtr getDUT();

  /**
   *  Sets the SPI channel the DUT is using
   *
   *  @param[in]  channel   The SPI channel to use
   *  @return void
   */
  void assignSPIChannelConfig( const Chimera::SPI::Channel channel );

  /**
   *  Gets the configured SPI channel for the DUT
   *
   *  @return Chimera::SPI::Channel
   */
  Chimera::SPI::Channel getSPIChannelConfig();

  /**
   *  Sets the Serial channel the DUT is using
   *
   *  @param[in]  channel   The Serial channel to use
   *  @return void
   */
  void assignSerialChannelConfig( const Chimera::Serial::Channel channel );

  /**
   *  Gets the configured Serial channel for the DUT
   *
   *  @return Chimera::Serial::Channel
   */
  Chimera::Serial::Channel getSerialChannelConfig();
}  // namespace Adesto::Testing

#endif  /* !ADESTO_MEMORY_TESTS_COMMON_HPP */
