//
// Created by Lars Schwarz on 24.10.2023.
//

#ifndef STM32_TOF_SPOT_TFLC02_H
#define STM32_TOF_SPOT_TFLC02_H

#include "stm32f4xx_hal.h"
#include <vector>

#define PACKET_HEADER_1 0x55
#define PACKET_HEADER_2 0xAA
#define PACKET_END 0xFA

class TFLC02{
public:
  TFLC02(UART_HandleTypeDef *huart_);
  void reset();
  void get_distance();
  void crosstalk_correction();
  void offset_correction();
  void get_factory_defaults();
  void get_product_information();
private:
  typedef enum _command_e {
    CMD_MEASURING_DISTANCE = 0x81,
    CMD_CROSSTALK_CORRECTION = 0x82,
    CMD_OFFSET_CORRECTION = 0x83,
    CMD_RESET = 0x84,
    CMD_FACTORY_SETTINGS = 0x85,
    CMD_PRODUCT_DETAILS = 0x86
  } command_e;

  typedef enum _error_code_e {
    ERROR_VALID_DATA = 0x00,
    ERROR_VCSEL_SHORT = 0x01,
    ERROR_LOW_SIGNAL = 0x02,
    ERROR_LOW_SN = 0x04,
    ERROR_TOO_MUCH_AMB = 0x08,
    ERROR_WAF = 0x10,
    ERROR_CAL_ERROR = 0x20,
    ERROR_CROSSTALK_ERROR = 0x80,
  } error_code_e;

  UART_HandleTypeDef *huart;
  std::vector<uint8_t> response;

  void transmit(command_e command);
  void receive();
};


#endif //STM32_TOF_SPOT_TFLC02_H
