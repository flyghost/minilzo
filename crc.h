// Copyright 2018-2019 trans-semi inc
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _CRC_H
#define _CRC_H
/**
  * @brief Crc32 value that is in little endian.
  *
  * @param  uint32_t crc : init crc value, use 0 at the first use.
  *
  * @param  uint8_t const *buf : buffer to start calculate crc.
  *
  * @param  uint32_t len : buffer length in byte.
  *
  * @return None
  */
uint32_t crc32_le(uint32_t crc, uint8_t const *buf, uint32_t len);

/**
  * @brief Crc16 value that is in little endian.
  *
  * @param  uint16_t crc : init crc value, use 0 at the first use.
  *
  * @param  uint8_t const *buf : buffer to start calculate crc.
  *
  * @param  uint32_t len : buffer length in byte.
  *
  * @return None
  */
uint16_t crc16_le(uint16_t crc, uint8_t const *buf, uint32_t len);

/**
  * @brief Crc8 value that is in little endian.
  *
  * @param  uint8_t crc : init crc value, use 0 at the first use.
  *
  * @param  uint8_t const *buf : buffer to start calculate crc.
  *
  * @param  uint32_t len : buffer length in byte.
  *
  * @return None
  */
uint8_t crc8_le(uint8_t crc, uint8_t const *buf, uint32_t len);

#endif