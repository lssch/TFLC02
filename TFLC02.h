//
// Created by Lars Schwarz on 24.10.2023.
//

#ifndef STM32_TOF_SPOT_TFLC02_H
#define STM32_TOF_SPOT_TFLC02_H

#include "stm32f4xx_hal.h"
#include <vector>
#include "types/sensor/sensor.h"

#define PACKET_HEADER_1 0x55
#define PACKET_HEADER_2 0xAA
#define PACKET_END 0xFA

namespace TFLC02 {
  enum class Command : uint16_t {
    MEASURING_DISTANCE = 0x81,
    CROSSTALK_CORRECTION = 0x82,
    OFFSET_CORRECTION = 0x83,
    RESET = 0x84,
    FACTORY_SETTINGS = 0x85,
    PRODUCT_DETAILS = 0x86
  };
  enum class ErrorCode : uint16_t {
    VALID_DATA = 0x00,
    VCSEL_SHORT = 0x01,
    LOW_SIGNAL = 0x02,
    LOW_SN = 0x04,
    TOO_MUCH_AMB = 0x08,
    WAF = 0x10,
    CAL_ERROR = 0x20,
    CROSSTALK_ERROR = 0x80,
  };

  class TFLC02{
  public:
    TFLC02(UART_HandleTypeDef *huart_);

    void init();
    void reset();
    void get_distance(Sensor::TofSpot* sensor);
    void crosstalk_correction();
    void offset_correction();
    void get_factory_defaults();
    void get_product_information();
  private:
    UART_HandleTypeDef *huart;
    std::vector<uint8_t> response;

    void transmit(Command command);
    void receive();
  };
}

#endif //STM32_TOF_SPOT_TFLC02_H
