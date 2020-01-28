#pragma once
#include <array>
#include <stdint.h>
#include <string>
#include "board.h"

void append_to_array(std::array<uint64_t, 5> & arr, const uint64_t& value);
std::string casenb_to_coo(const short& casenb);