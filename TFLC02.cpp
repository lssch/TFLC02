//
// Created by Lars Schwarz on 24.10.2023.
//

#include <vector>
#include <iostream>
#include "TFLC02.h"

TFLC02::TFLC02(UART_HandleTypeDef *huart_):
  huart(huart_) {
}

void TFLC02::reset() {
  transmit(CMD_RESET);

  HAL_Delay(1000);
}

void TFLC02::get_distance() {
  transmit(CMD_MEASURING_DISTANCE);

  auto distance = (response.at(4) << 8 | response.at(5));

  std::cout << "Distance: " << +distance << " mm" << std::endl;
}

void TFLC02::transmit(command_e command) {
  std::vector<uint8_t> tx_data {PACKET_HEADER_1, PACKET_HEADER_2};
  tx_data.push_back(command);
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

void TFLC02::receive() {
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
