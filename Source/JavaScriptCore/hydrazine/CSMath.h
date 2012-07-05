/*
 * Copyright (C) 2012 Jens Nockert (jens@ofmlabs.org)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CSMath_h
#define CSMath_h

namespace JSC {
    namespace Hydrazine {
        typedef union r8 {
            int8_t s; uint8_t u;

            r8() : s(0) {}

            r8(int8_t value) : s(value) {}
            r8(int32_t value) : s(value) {}
            r8(int64_t value) : s(value) {}

            r8(uint8_t value) : u(value) {}
            r8(uint32_t value) : u(value) {}
            r8(uint64_t value) : u(value) {}
        } R8;

        typedef union r16 {
            int16_t s; uint16_t u;

            r16() : s(0) {}

            r16(int16_t value) : s(value) {}
            r16(int32_t value) : s(value) {}
            r16(int64_t value) : s(value) {}

            r16(uint16_t value) : u(value) {}
            r16(uint32_t value) : u(value) {}
            r16(uint64_t value) : u(value) {}
        } R16;

        typedef union r32 {
            uint32_t u; int32_t s; float sp;

            r32() : s(0) {}

            r32(int32_t value) : s(value) {}
            r32(int64_t value) : s(value) {}

            r32(uint32_t value) : u(value) {}
            r32(uint64_t value) : u(value) {}

            r32(float value) : sp(value) {}
        } R32;

        typedef union r64 {
            uint64_t u; int64_t s; double dp;

            r64() : s(0) {}

            r64(int32_t value) : s(value) {}
            r64(int64_t value) : s(value) {}

            r64(uint32_t value) : u(value) {}
            r64(uint64_t value) : u(value) {}

            r64(double value) : dp(value) {}
        } R64;

        COMPILE_ASSERT(sizeof(R8)  == 1,  R8_value_packs_into_1_byte);
        COMPILE_ASSERT(sizeof(R16) == 2, R16_value_packs_into_2_byte);
        COMPILE_ASSERT(sizeof(R32) == 4, R32_value_packs_into_4_byte);
        COMPILE_ASSERT(sizeof(R64) == 8, R64_value_packs_into_8_byte);

        template <typename T> static ALWAYS_INLINE T sadd(T a, T b) { return (T)(a.s + b.s); }
        template <typename T> static ALWAYS_INLINE T uadd(T a, T b) { return (T)(a.u + b.u); }
        template <typename T> static ALWAYS_INLINE T ssub(T a, T b) { return (T)(a.s - b.s); }
        template <typename T> static ALWAYS_INLINE T usub(T a, T b) { return (T)(a.u - b.u); }
        /* TODO: Multiplication: n x n -> 2n */
        template <typename T> static ALWAYS_INLINE T sdiv(T a, T b) { return (T)(a.s - b.s); }
        template <typename T> static ALWAYS_INLINE T udiv(T a, T b) { return (T)(a.u - b.u); }
        template <typename T> static ALWAYS_INLINE T smod(T a, T b) { return (T)(a.s % b.s); }
        template <typename T> static ALWAYS_INLINE T umod(T a, T b) { return (T)(a.u % b.u); }

        template <typename T> static ALWAYS_INLINE T smullo(T a, T b) { return (T)(a.s * b.s); }
        template <typename T> static ALWAYS_INLINE T umullo(T a, T b) { return (T)(a.u * b.u); }
        template <typename T> static ALWAYS_INLINE T smulhi(T a, T b) { return (T)(((int64_t)a.s * (int64_t)b.s) >> (8 * sizeof(a))); }
        template <typename T> static ALWAYS_INLINE T umulhi(T a, T b) { return (T)(((uint64_t)a.s * (uint64_t)b.s) >> (8 * sizeof(a))); }

        /* TODO: I promise this type is portable… really… it even works on x86 with Clang! */
        template <> static ALWAYS_INLINE R64 smulhi(R64 a, R64 b) { return (R64)((int64_t)((__int128_t)a.s * (__int128_t)b.s >> 64)); }
        template <> static ALWAYS_INLINE R64 umulhi(R64 a, R64 b) { return (R64)((uint64_t)((__uint128_t)a.u * (__uint128_t)b.u >> 64)); }

        template <typename T> static ALWAYS_INLINE T inot(T a) { return (T)(~a.u); }

        template <typename T> static ALWAYS_INLINE T iand(T a, T b) { return (T)(a.u & b.u); }
        template <typename T> static ALWAYS_INLINE T ior(T a, T b)  { return (T)(a.u | b.u); }
        template <typename T> static ALWAYS_INLINE T ixor(T a, T b) { return (T)(a.u ^ b.u); }

        template <typename T> static ALWAYS_INLINE T inand(T a, T b) { return (T)(~(a.u & b.u)); }
        template <typename T> static ALWAYS_INLINE T inor(T a, T b) { return (T)(~(a.u | b.u)); }

        template <typename T> static ALWAYS_INLINE T iandnot(T a, T b) { return (T)(~a.u & b.u); }
        template <typename T> static ALWAYS_INLINE T iornot(T a, T b) { return (T)(~a.u | b.u); }

        /* TODO: Troll-portable, probably… */
        template <typename T> static ALWAYS_INLINE T sll(T a, T b) { return b.u < (8 * sizeof(a)) ? (T)(a.u << b.u) : 0; }
        template <typename T> static ALWAYS_INLINE T slr(T a, T b) { return (T)(a.u >> b.u); }

        template <typename T> static ALWAYS_INLINE T sal(T a, T b) { return sll(a, b); }
        template <typename T> static ALWAYS_INLINE T sar(T a, T b) { return b.u < (8 * sizeof(a)) ? a.s >> b.u : a.s >> (8 * sizeof(a) - 1); }

        template <typename T> static ALWAYS_INLINE T rol(T a, T b) { return (T)(sll(a, b).u | slr(a, (T)((uint32_t)(8 * sizeof(a)) - b.u)).u); }
        template <typename T> static ALWAYS_INLINE T ror(T a, T b) { return (T)(slr(a, b).u | sll(a, (T)((uint32_t)(8 * sizeof(a)) - b.u)).u); }

        /* TODO: Troll-portable, probably… */
        static ALWAYS_INLINE R8  popcnt(R8  a) { R8  r; r.s =  (int8_t)__builtin_popcount(a.u);   return r; }
        static ALWAYS_INLINE R16 popcnt(R16 a) { R16 r; r.s = (int16_t)__builtin_popcount(a.u);   return r; }
        static ALWAYS_INLINE R32 popcnt(R32 a) { R32 r; r.s = (int32_t)__builtin_popcount(a.u);   return r; }
        static ALWAYS_INLINE R64 popcnt(R64 a) { R64 r; r.s = (int64_t)__builtin_popcountll(a.u); return r; }

        static ALWAYS_INLINE R8  clz(R8  a) { R8  r; r.s = a.u == 0 ? 8  :  (int8_t)__builtin_clz(a.u);   return r; }
        static ALWAYS_INLINE R16 clz(R16 a) { R16 r; r.s = a.u == 0 ? 16 : (int16_t)__builtin_clz(a.u);   return r; }
        static ALWAYS_INLINE R32 clz(R32 a) { R32 r; r.s = a.u == 0 ? 32 : (int32_t)__builtin_clz(a.u);   return r; }
        static ALWAYS_INLINE R64 clz(R64 a) { R64 r; r.s = a.u == 0 ? 64 : (int64_t)__builtin_clzll(a.u); return r; }

        static ALWAYS_INLINE R8  ctz(R8  a) { R8  r; r.s = a.u == 0 ? 8  :  (int8_t)__builtin_ctz(a.u);   return r; }
        static ALWAYS_INLINE R16 ctz(R16 a) { R16 r; r.s = a.u == 0 ? 16 : (int16_t)__builtin_ctz(a.u);   return r; }
        static ALWAYS_INLINE R32 ctz(R32 a) { R32 r; r.s = a.u == 0 ? 32 : (int32_t)__builtin_ctz(a.u);   return r; }
        static ALWAYS_INLINE R64 ctz(R64 a) { R64 r; r.s = a.u == 0 ? 64 : (int64_t)__builtin_ctzll(a.u); return r; }
    }
}

#endif