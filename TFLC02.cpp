//
// Created by Lars Schwarz on 24.10.2023.
//

#include <vector>
#include <iostream>
#include "TFLC02.h"

TFLC02::TFLC02::TFLC02(UART_HandleTypeDef *huart_):
  huart(huart_) {
}

void TFLC02::TFLC02::init() {
  reset();
}

void TFLC02::TFLC02::reset() {
  transmit(Command::RESET);

  HAL_Delay(100);
}

void TFLC02::TFLC02::get_distance(Sensor::TofSpot* sensor) {
  transmit(Command::MEASURING_DISTANCE);

  sensor->distance = (response.at(4) << 8 | response.at(5));

  //std::cout << "Distance: " << +(response.at(4) << 8 | response.at(5)) << " mm (" << +response.at(6) <<")" << std::endl;
}

void TFLC02::TFLC02::crosstalk_correction() {
  transmit(Command::CROSSTALK_CORRECTION);

  std::cout << "Error code: " << +response.at(4) << "; "
            << "xtalkLsb: " << response.at(5) << "; "
            << "xtalkMsb: " << response.at(6)
            << std::endl;
}

void TFLC02::TFLC02::offset_correction() {
  transmit(Command::OFFSET_CORRECTION);

  std::cout << "Error code: " << +response.at(4) << "; "
            << "Offset_short1: " << response.at(5) << "; "
            << "Offset_short2: " << response.at(6) << "; "
            << "Offset_long1: " << response.at(7) << "; "
            << "Offset_long2: " << response.at(8) << "; "
            << std::endl;
}

void TFLC02::TFLC02::get_factory_defaults() {
  transmit(Command::FACTORY_SETTINGS);

  std::cout << "Offset_short1: " << response.at(4) << "; "
            << "Offset_short2: " << response.at(5) << "; "
            << "Offset_long1: " << response.at(6) << "; "
            << "Offset_long2: " << response.at(7) << "; "
            << "xtalkLsb: " << response.at(8) << "; "
            << "xtalkMsb: " << response.at(9)
            << std::endl;
}

void TFLC02::TFLC02::get_product_information() {
  transmit(Command::PRODUCT_DETAILS);

  std::cout << "Sensor_ic: ";
  switch (response.at(4)) {
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
  switch (response.at(5)) {
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
  std::cout << "; Firmware: " << +response.at(6) << std::endl;
}


void TFLC02::TFLC02::transmit(Command command) {
  std::vector<uint8_t> tx_data {PACKET_HEADER_1, PACKET_HEADER_2};
  tx_data.push_back(static_cast<uint8_t>(command));
  tx_data.push_back(0);
  tx_data.push_back(PACKET_END);

/*
  std::cout << "tx_data = ";
  for (auto byte: tx_data) {
    std::cout << std::hex << +byte << " ";
  }
  std::cout << std::endl;
*/

  HAL_UART_Transmit(huart, tx_data.data(), tx_data.size(), HAL_MAX_DELAY);

  receive();
}

void TFLC02::TFLC02::receive() {
  uint8_t rx_data[32] {0};
  uint8_t rx_length = 4;

  response.clear();

  HAL_UART_Receive(huart, rx_data, rx_length, HAL_MAX_DELAY);

  for (int i = 0; i < rx_length; ++i) {
    response.push_back(rx_data[i]);
  }

  rx_length = rx_data[3] + 1;

  HAL_UART_Receive(huart, &rx_data[4], rx_length, HAL_MAX_DELAY);

  for (int i = 0; i < rx_length; ++i) {
    response.push_back(rx_data[i + 4]);
  }

/*
  std::cout << "rx_data = ";
  for (auto byte: response) {
    std::cout << std::hex << +byte << " ";
  }
  std::cout << std::endl;
*/

}
