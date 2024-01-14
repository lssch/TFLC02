//
// Created by Lars Schwarz on 24.10.2023.
//

#ifndef STM32_TOF_SPOT_TFLC02_H
#define STM32_TOF_SPOT_TFLC02_H

#include "stm32f4xx_hal.h"
#include <vector>
#include "types/sensor.h"
#include "types/request.h"
#include "types/state.h"

#define TFLC02_PACKET_HEADER_1 0x55
#define TFLC02_PACKET_HEADER_2 0xAA
#define TFLC02_PACKET_END 0xFA
#define TFLC02_COMMS_TIMEOUT 100

namespace TFLC02 {
  enum class Command : uint8_t {
    MEASURING_DISTANCE = 0x81,
    CROSSTALK_CORRECTION = 0x82,
    OFFSET_CORRECTION = 0x83,
    RESET = 0x84,
    FACTORY_SETTINGS = 0x85,
    PRODUCT_DETAILS = 0x86
  };

  enum class ErrorCode : uint8_t {
    VALID_DATA = 0x00,
    VCSEL_SHORT = 0x01,
    LOW_SIGNAL = 0x02,
    LOW_SN = 0x04,
    TOO_MUCH_AMB = 0x08,
    WAF = 0x10,
    CAL_ERROR = 0x20,
    CROSSTALK_ERROR = 0x80,
  };

  class Response {
  public:
    std::array<uint8_t, 4> header;
    std::vector<uint8_t> body;
    uint8_t tail;
  };

  class TFLC02 {
  public:
    /// @brief ToF Object which represents the TFLC02 chip
    /// @param[in,out] huart Uart interface to communicate with the connected camera.
    /// @param[out] state State of the sensor
    TFLC02(UART_HandleTypeDef &huart, State::TofSpot &state);

    /// @brief Initializer to setup the sensor.
    /// @todo This function can be eliminated if the Constructor is called inside the main function and after the initialisation of the huart
    void init();

    /// @brief Hardware reset
    void reset();

    /// @brief Get the current distance from the sensor.
    /// @param[in] sensor Container to store the values to.
    void get_distance(Sensor::TofSpot &sensor);

    void crosstalk_correction();
    void offset_correction();
    void get_factory_defaults();
    void get_product_information();
  private:
    UART_HandleTypeDef &_huart;
    State::TofSpot &_state;
    Response _response;

    void inline transmit(Command command);
    uint8_t inline receive();
    uint8_t inline transmit_receive(Command command);
  };
}

#endif //STM32_TOF_SPOT_TFLC02_H
