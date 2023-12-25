//
// Created by Lars Schwarz on 24.10.2023.
//

#include <vector>
#include <iostream>
#include "TFLC02.h"

TFLC02::TFLC02::TFLC02(UART_HandleTypeDef &huart, State::TofSpot &state)
  : _huart(huart),
    _state(state) {
}

void TFLC02::TFLC02::init() {
  reset();
}

void TFLC02::TFLC02::reset() {
  transmit(Command::RESET);
}

void TFLC02::TFLC02::get_distance(Sensor::TofSpot &sensor) {
  if (transmit_receive(Command::MEASURING_DISTANCE) != EXIT_SUCCESS)
    return;

  sensor.distance = (_response.body.at(0) << 8 | _response.body.at(1));
  _state = static_cast<State::TofSpot>(_response.body.at(2));
}

void TFLC02::TFLC02::crosstalk_correction() {
  if (transmit_receive(Command::CROSSTALK_CORRECTION) != EXIT_SUCCESS)
    return;

  std::cout << "Error code: " << +_response.body.at(0) << "; "
            << "xtalkLsb: " << _response.body.at(1) << "; "
            << "xtalkMsb: " << _response.body.at(2)
            << std::endl;
}

void TFLC02::TFLC02::offset_correction() {
  if (transmit_receive(Command::OFFSET_CORRECTION) != EXIT_SUCCESS)
    return;

  std::cout << "Error code: " << +_response.body.at(0) << "; "
            << "Offset_short1: " << _response.body.at(1) << "; "
            << "Offset_short2: " << _response.body.at(2) << "; "
            << "Offset_long1: " << _response.body.at(3) << "; "
            << "Offset_long2: " << _response.body.at(4) << "; "
            << std::endl;
}

void TFLC02::TFLC02::get_factory_defaults() {
  if (transmit_receive(Command::FACTORY_SETTINGS) != EXIT_SUCCESS)
    return;

  std::cout << "Offset_short1: " << _response.body.at(0) << "; "
            << "Offset_short2: " << _response.body.at(1) << "; "
            << "Offset_long1: " << _response.body.at(2) << "; "
            << "Offset_long2: " << _response.body.at(3) << "; "
            << "xtalkLsb: " << _response.body.at(4) << "; "
            << "xtalkMsb: " << _response.body.at(5)
            << std::endl;
}

void TFLC02::TFLC02::get_product_information() {
  if (transmit_receive(Command::PRODUCT_DETAILS) != EXIT_SUCCESS)
    return;

  std::cout << "Sensor_ic: ";
  switch (_response.body.at(0)) {
    case 0x02:
      std::cout << "gp2ap02vt";
      break;
    case 0x03:
      std::cout << "gp2ap03vt";
      break;
    default:
      break;
  }
  std::cout << "; Port: ";
  switch (_response.body.at(1)) {
    case 0x41:
      std::cout << "UART/I2C";
      break;
    case 0x49:
      std::cout << "I2C";
      break;
    case 0x55:
      std::cout << "UART";
    default:
      break;
  }
  std::cout << "; Firmware: " << +_response.body.at(2) << std::endl;
}


void inline TFLC02::TFLC02::transmit(Command command) {
  std::vector<uint8_t> tx_data {
    TFLC02_PACKET_HEADER_1,
    TFLC02_PACKET_HEADER_2,
    static_cast<uint8_t>(command),
    0,
    TFLC02_PACKET_END};

  /*
  std::cout << "tx_data = ";
  for (auto byte: tx_data) {
    std::cout << std::hex << +byte << " ";
  }
  std::cout << std::endl;
  */

  HAL_UART_Transmit(&_huart, tx_data.data(), tx_data.size(), TFLC02_COMMS_TIMEOUT);
}

uint8_t inline TFLC02::TFLC02::receive() {
  // Receive the header part of the package
  HAL_UART_Receive(&_huart, _response.header.data(), _response.header.size(), TFLC02_COMMS_TIMEOUT);

  // Receive the body part of the package
  std::vector<uint8_t> body(_response.header.at(3));
  HAL_UART_Receive(&_huart, body.data(), body.size(), TFLC02_COMMS_TIMEOUT);
  _response.body = body;

  // Receive the tail part of the package
  HAL_UART_Receive(&_huart, &_response.tail, 1, TFLC02_COMMS_TIMEOUT);

  /*
  std::cout << "rx_data: ";
  for (auto byte: _response.header)
    std::cout << std::hex << +byte << ", ";
  for (auto byte: _response.body)
    std::cout << std::hex << +byte << ", ";
  std::cout << std::hex << +_response.tail << std::endl;
  */

  if (_response.header.at(0) != TFLC02_PACKET_HEADER_1 || _response.header.at(1) != TFLC02_PACKET_HEADER_2 || _response.tail != TFLC02_PACKET_END)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

uint8_t inline TFLC02::TFLC02::transmit_receive(Command command) {
  transmit(command);
  return receive();
}
