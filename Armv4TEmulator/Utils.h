#pragma once

#include "Common.h"
#include <string>

inline unsigned getBit(u32 v, unsigned bit_number) {
	return ((v >> bit_number) & 0b1);
}
inline unsigned getBit(u32 v, signed bit_number) {
	if (bit_number < 0) throw std::string("Unimplemented opcode");
	return ((v >> bit_number) & 0b1);
}
//avoid implicit conversions
template <class T, class U>
unsigned getBit(T, U) = delete;