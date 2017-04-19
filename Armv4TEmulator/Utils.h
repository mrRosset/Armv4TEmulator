#pragma once

#include "Common.h"
#include <string>

/*
Code in this file is compiler-specific.
Some functions for circular functions are only defined for VC++ and GCC.
Also the operator >> is used. This operator is can be logical or arithmetic
In VC++ it's arithmetic. https://msdn.microsoft.com/en-us/library/336xbhcz.aspx
Not tested in any other compiler,
*/

#if defined(__GNUG__)
inline u8 ror8(const u8 x, const u8 n) {
	u8 result = x;
	__asm__("rorb %[n], %[result]" : [result] "+g" (result) : [n] "c" (n));
	return result;
}

inline u16 ror16(const u16 x, const u16 n) {
	u16 result = x;
	__asm__("rorw %b[n], %[result]" : [result] "+g" (result) : [n] "c" (n));
	return result;
}

inline u32 ror32(const u32 x, const u32 n) {
	u32 result = x;
	__asm__("rorl %b[n], %[result]" : [result] "+g" (result) : [n] "c" (n));
	return result;
}

inline u64 ror64(const u64 x, const u64 n) {
	u64 result = x;
	__asm__("rorq %b[n], %[result]" : [result] "+g" (result) : [n] "c" (n));
	return result;
}
#elif defined(_MSC_VER)
inline u8 ror8(const u8 x, const u8 n) { return _rotr8(x, n); }
inline u16 ror16(const u16 x, const u16 n) { return _rotr16(x, n); }
inline u32 ror32(const u32 x, const u32 n) { return _rotr(x, n); }
inline u64 ror64(const u64 x, const u64 n) { return _rotr64(x, n); }
#endif

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


