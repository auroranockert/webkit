/*
 *  Copyright (C) 2012 Jens Nockert (jens@nockert.se)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include "Accelerate.h"

#include <cmath>
#include <cstdlib>

static inline unsigned cs_calculate_alignment(unsigned n, unsigned elementsize) {
    if (n > 32 || __builtin_popcount(n) != 1) {
        return 0;
    }
    
    unsigned alignment = elementsize * n;
    
    if (alignment > 16) {
        return 16;
    } else {
        return alignment;
    }
}

static inline bool cs_check_array(ArrayBufferView* view, unsigned offset, unsigned alignment) {
    if (!view) {
        return false;
    }
    
    if (offset % alignment != 0) { // TODO: Really check alignment
        return false;
    }
    
    return true;
}


namespace WebCore {

PassRefPtr<Accelerate> Accelerate::create()
{
    return adoptRef(new Accelerate());
}

template <typename Scalar, typename View>
static inline void cs_abs(unsigned short n, View* r, unsigned ri, View* a, unsigned ai)
{
    unsigned alignment = cs_calculate_alignment(n, sizeof(Scalar));
    
    if (alignment == 0 || !cs_check_array(r, ri, alignment) || !cs_check_array(a, ai, alignment)) {
        return;
    }
    
    if (r->length() < ri + n) { return; }
    if (a->length() < ai + n) { return; }
    
    Scalar* rPtr = r->data();
    Scalar* aPtr = a->data();
    
    for (unsigned i = 0; i < n; i++) {
        rPtr[ri++] = abs(aPtr[ai++]);
    }
}

template <typename Scalar, typename View>
static inline void cs_add(unsigned short n, View* r, unsigned ri, View* a, unsigned ai, View* b, unsigned bi)
{
    unsigned alignment = cs_calculate_alignment(n, sizeof(Scalar));
    
    if (alignment == 0 || !cs_check_array(r, ri, alignment) || !cs_check_array(a, ai, alignment) || !cs_check_array(b, bi, alignment)) {
        return;
    }
    
    if (r->length() < ri + n) { return; }
    if (a->length() < ai + n) { return; }
    if (b->length() < bi + n) { return; }
    
    Scalar* rPtr = r->data();
    Scalar* aPtr = a->data();
    Scalar* bPtr = a->data();
    
    for (unsigned i = 0; i < n; i++) {
        rPtr[ri++] = aPtr[ai++] + bPtr[bi++];
    }
}

template <typename Scalar, typename View>
static inline void cs_div(unsigned short n, View* r, unsigned ri, View* a, unsigned ai, View* b, unsigned bi)
{
    unsigned alignment = cs_calculate_alignment(n, sizeof(Scalar));
    
    if (alignment == 0 || !cs_check_array(r, ri, alignment) || !cs_check_array(a, ai, alignment) || !cs_check_array(b, bi, alignment)) {
        return;
    }
    
    if (r->length() < ri + n) { return; }
    if (a->length() < ai + n) { return; }
    if (b->length() < bi + n) { return; }
    
    Scalar* rPtr = r->data();
    Scalar* aPtr = a->data();
    Scalar* bPtr = a->data();
    
    for (unsigned i = 0; i < n; i++) {
        rPtr[ri++] = aPtr[ai++] / bPtr[bi++];
    }
}

template <typename Scalar, typename View>
static inline void cs_max(unsigned short n, View* r, unsigned ri, View* a, unsigned ai, View* b, unsigned bi)
{
    unsigned alignment = cs_calculate_alignment(n, sizeof(Scalar));
    
    if (alignment == 0 || !cs_check_array(r, ri, alignment) || !cs_check_array(a, ai, alignment) || !cs_check_array(b, bi, alignment)) {
        return;
    }
    
    if (r->length() < ri + n) { return; }
    if (a->length() < ai + n) { return; }
    if (b->length() < bi + n) { return; }
    
    Scalar* rPtr = r->data();
    Scalar* aPtr = a->data();
    Scalar* bPtr = a->data();
    
    for (unsigned i = 0; i < n; i++) {
        rPtr[ri++] = std::max(aPtr[ai++], bPtr[bi++]);
    }
}

template <typename Scalar, typename View>
static inline void cs_min(unsigned short n, View* r, unsigned ri, View* a, unsigned ai, View* b, unsigned bi)
{
    unsigned alignment = cs_calculate_alignment(n, sizeof(Scalar));
    
    if (alignment == 0 || !cs_check_array(r, ri, alignment) || !cs_check_array(a, ai, alignment) || !cs_check_array(b, bi, alignment)) {
        return;
    }
    
    if (r->length() < ri + n) { return; }
    if (a->length() < ai + n) { return; }
    if (b->length() < bi + n) { return; }
    
    Scalar* rPtr = r->data();
    Scalar* aPtr = a->data();
    Scalar* bPtr = a->data();
    
    for (unsigned i = 0; i < n; i++) {
        rPtr[ri++] = std::min(aPtr[ai++], bPtr[bi++]);
    }
}

template <typename Scalar, typename View>
static inline void cs_mul(unsigned short n, View* r, unsigned ri, View* a, unsigned ai, View* b, unsigned bi)
{
    unsigned alignment = cs_calculate_alignment(n, sizeof(Scalar));
    
    if (alignment == 0 || !cs_check_array(r, ri, alignment) || !cs_check_array(a, ai, alignment) || !cs_check_array(b, bi, alignment)) {
        return;
    }
    
    if (r->length() < ri + n) { return; }
    if (a->length() < ai + n) { return; }
    if (b->length() < bi + n) { return; }
    
    Scalar* rPtr = r->data();
    Scalar* aPtr = a->data();
    Scalar* bPtr = a->data();
    
    for (unsigned i = 0; i < n; i++) {
        rPtr[ri++] = aPtr[ai++] * bPtr[bi++];
    }
}

template <typename Scalar, typename View>
static inline void cs_sqrt(unsigned short n, View* r, unsigned ri, View* a, unsigned ai)
{
    unsigned alignment = cs_calculate_alignment(n, sizeof(Scalar));
    
    if (alignment == 0 || !cs_check_array(r, ri, alignment) || !cs_check_array(a, ai, alignment)) {
        return;
    }
    
    if (r->length() < ri + n) { return; }
    if (a->length() < ai + n) { return; }
    
    Scalar* rPtr = r->data();
    Scalar* aPtr = a->data();
    
    for (unsigned i = 0; i < n; i++) {
        rPtr[ri++] = sqrt(aPtr[ai++]);
    }
}

template <typename Scalar, typename View>
static inline void cs_sub(unsigned short n, View* r, unsigned ri, View* a, unsigned ai, View* b, unsigned bi)
{
    unsigned alignment = cs_calculate_alignment(n, sizeof(Scalar));
    
    if (alignment == 0 || !cs_check_array(r, ri, alignment) || !cs_check_array(a, ai, alignment) || !cs_check_array(b, bi, alignment)) {
        return;
    }
    
    if (r->length() < ri + n) { return; }
    if (a->length() < ai + n) { return; }
    if (b->length() < bi + n) { return; }
    
    Scalar* rPtr = r->data();
    Scalar* aPtr = a->data();
    Scalar* bPtr = a->data();
    
    for (unsigned i = 0; i < n; i++) {
        rPtr[ri++] = aPtr[ai++] - bPtr[bi++];
    }
}


void Accelerate::abs(unsigned short n, Uint8Array* r, unsigned ri, Uint8Array* a, unsigned ai) { cs_abs<uint8_t, Uint8Array>(n, r, ri, a, ai); }
void Accelerate::abs(unsigned short n, Uint16Array* r, unsigned ri, Uint16Array* a, unsigned ai) { cs_abs<uint16_t, Uint16Array>(n, r, ri, a, ai); }
void Accelerate::abs(unsigned short n, Uint32Array* r, unsigned ri, Uint32Array* a, unsigned ai) { cs_abs<uint32_t, Uint32Array>(n, r, ri, a, ai); }
void Accelerate::abs(unsigned short n, Uint8ClampedArray* r, unsigned ri, Uint8ClampedArray* a, unsigned ai) { cs_abs<uint8_t, Uint8ClampedArray>(n, r, ri, a, ai); }

void Accelerate::abs(unsigned short n, Int8Array* r, unsigned ri, Int8Array* a, unsigned ai) { cs_abs<int8_t, Int8Array>(n, r, ri, a, ai); }
void Accelerate::abs(unsigned short n, Int16Array* r, unsigned ri, Int16Array* a, unsigned ai) { cs_abs<int16_t, Int16Array>(n, r, ri, a, ai); }
void Accelerate::abs(unsigned short n, Int32Array* r, unsigned ri, Int32Array* a, unsigned ai) { cs_abs<int32_t, Int32Array>(n, r, ri, a, ai); }

void Accelerate::abs(unsigned short n, Float32Array* r, unsigned ri, Float32Array* a, unsigned ai) { cs_abs<float, Float32Array>(n, r, ri, a, ai); }
void Accelerate::abs(unsigned short n, Float64Array* r, unsigned ri, Float64Array* a, unsigned ai) { cs_abs<double, Float64Array>(n, r, ri, a, ai); }

void Accelerate::add(unsigned short n, Uint8Array* r, unsigned ri, Uint8Array* a, unsigned ai, Uint8Array* b, unsigned bi) { cs_add<uint8_t, Uint8Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::add(unsigned short n, Uint16Array* r, unsigned ri, Uint16Array* a, unsigned ai, Uint16Array* b, unsigned bi) { cs_add<uint16_t, Uint16Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::add(unsigned short n, Uint32Array* r, unsigned ri, Uint32Array* a, unsigned ai, Uint32Array* b, unsigned bi) { cs_add<uint32_t, Uint32Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::add(unsigned short n, Uint8ClampedArray* r, unsigned ri, Uint8ClampedArray* a, unsigned ai, Uint8ClampedArray* b, unsigned bi) { cs_add<uint8_t, Uint8ClampedArray>(n, r, ri, a, ai, b, bi); }

void Accelerate::add(unsigned short n, Int8Array* r, unsigned ri, Int8Array* a, unsigned ai, Int8Array* b, unsigned bi) { cs_add<int8_t, Int8Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::add(unsigned short n, Int16Array* r, unsigned ri, Int16Array* a, unsigned ai, Int16Array* b, unsigned bi) { cs_add<int16_t, Int16Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::add(unsigned short n, Int32Array* r, unsigned ri, Int32Array* a, unsigned ai, Int32Array* b, unsigned bi) { cs_add<int32_t, Int32Array>(n, r, ri, a, ai, b, bi); }

void Accelerate::add(unsigned short n, Float32Array* r, unsigned ri, Float32Array* a, unsigned ai, Float32Array* b, unsigned bi) { cs_add<float, Float32Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::add(unsigned short n, Float64Array* r, unsigned ri, Float64Array* a, unsigned ai, Float64Array* b, unsigned bi) { cs_add<double, Float64Array>(n, r, ri, a, ai, b, bi); }

void Accelerate::div(unsigned short n, Uint8Array* r, unsigned ri, Uint8Array* a, unsigned ai, Uint8Array* b, unsigned bi) { cs_div<uint8_t, Uint8Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::div(unsigned short n, Uint16Array* r, unsigned ri, Uint16Array* a, unsigned ai, Uint16Array* b, unsigned bi) { cs_div<uint16_t, Uint16Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::div(unsigned short n, Uint32Array* r, unsigned ri, Uint32Array* a, unsigned ai, Uint32Array* b, unsigned bi) { cs_div<uint32_t, Uint32Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::div(unsigned short n, Uint8ClampedArray* r, unsigned ri, Uint8ClampedArray* a, unsigned ai, Uint8ClampedArray* b, unsigned bi) { cs_div<uint8_t, Uint8ClampedArray>(n, r, ri, a, ai, b, bi); }

void Accelerate::div(unsigned short n, Int8Array* r, unsigned ri, Int8Array* a, unsigned ai, Int8Array* b, unsigned bi) { cs_div<int8_t, Int8Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::div(unsigned short n, Int16Array* r, unsigned ri, Int16Array* a, unsigned ai, Int16Array* b, unsigned bi) { cs_div<int16_t, Int16Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::div(unsigned short n, Int32Array* r, unsigned ri, Int32Array* a, unsigned ai, Int32Array* b, unsigned bi) { cs_div<int32_t, Int32Array>(n, r, ri, a, ai, b, bi); }

void Accelerate::div(unsigned short n, Float32Array* r, unsigned ri, Float32Array* a, unsigned ai, Float32Array* b, unsigned bi) { cs_div<float, Float32Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::div(unsigned short n, Float64Array* r, unsigned ri, Float64Array* a, unsigned ai, Float64Array* b, unsigned bi) { cs_div<double, Float64Array>(n, r, ri, a, ai, b, bi); }

void Accelerate::max(unsigned short n, Uint8Array* r, unsigned ri, Uint8Array* a, unsigned ai, Uint8Array* b, unsigned bi) { cs_max<uint8_t, Uint8Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::max(unsigned short n, Uint16Array* r, unsigned ri, Uint16Array* a, unsigned ai, Uint16Array* b, unsigned bi) { cs_max<uint16_t, Uint16Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::max(unsigned short n, Uint32Array* r, unsigned ri, Uint32Array* a, unsigned ai, Uint32Array* b, unsigned bi) { cs_max<uint32_t, Uint32Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::max(unsigned short n, Uint8ClampedArray* r, unsigned ri, Uint8ClampedArray* a, unsigned ai, Uint8ClampedArray* b, unsigned bi) { cs_max<uint8_t, Uint8ClampedArray>(n, r, ri, a, ai, b, bi); }

void Accelerate::max(unsigned short n, Int8Array* r, unsigned ri, Int8Array* a, unsigned ai, Int8Array* b, unsigned bi) { cs_max<int8_t, Int8Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::max(unsigned short n, Int16Array* r, unsigned ri, Int16Array* a, unsigned ai, Int16Array* b, unsigned bi) { cs_max<int16_t, Int16Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::max(unsigned short n, Int32Array* r, unsigned ri, Int32Array* a, unsigned ai, Int32Array* b, unsigned bi) { cs_max<int32_t, Int32Array>(n, r, ri, a, ai, b, bi); }

void Accelerate::max(unsigned short n, Float32Array* r, unsigned ri, Float32Array* a, unsigned ai, Float32Array* b, unsigned bi) { cs_max<float, Float32Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::max(unsigned short n, Float64Array* r, unsigned ri, Float64Array* a, unsigned ai, Float64Array* b, unsigned bi) { cs_max<double, Float64Array>(n, r, ri, a, ai, b, bi); }

void Accelerate::min(unsigned short n, Uint8Array* r, unsigned ri, Uint8Array* a, unsigned ai, Uint8Array* b, unsigned bi) { cs_min<uint8_t, Uint8Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::min(unsigned short n, Uint16Array* r, unsigned ri, Uint16Array* a, unsigned ai, Uint16Array* b, unsigned bi) { cs_min<uint16_t, Uint16Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::min(unsigned short n, Uint32Array* r, unsigned ri, Uint32Array* a, unsigned ai, Uint32Array* b, unsigned bi) { cs_min<uint32_t, Uint32Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::min(unsigned short n, Uint8ClampedArray* r, unsigned ri, Uint8ClampedArray* a, unsigned ai, Uint8ClampedArray* b, unsigned bi) { cs_min<uint8_t, Uint8ClampedArray>(n, r, ri, a, ai, b, bi); }

void Accelerate::min(unsigned short n, Int8Array* r, unsigned ri, Int8Array* a, unsigned ai, Int8Array* b, unsigned bi) { cs_min<int8_t, Int8Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::min(unsigned short n, Int16Array* r, unsigned ri, Int16Array* a, unsigned ai, Int16Array* b, unsigned bi) { cs_min<int16_t, Int16Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::min(unsigned short n, Int32Array* r, unsigned ri, Int32Array* a, unsigned ai, Int32Array* b, unsigned bi) { cs_min<int32_t, Int32Array>(n, r, ri, a, ai, b, bi); }

void Accelerate::min(unsigned short n, Float32Array* r, unsigned ri, Float32Array* a, unsigned ai, Float32Array* b, unsigned bi) { cs_min<float, Float32Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::min(unsigned short n, Float64Array* r, unsigned ri, Float64Array* a, unsigned ai, Float64Array* b, unsigned bi) { cs_min<double, Float64Array>(n, r, ri, a, ai, b, bi); }

void Accelerate::mul(unsigned short n, Uint8Array* r, unsigned ri, Uint8Array* a, unsigned ai, Uint8Array* b, unsigned bi) { cs_mul<uint8_t, Uint8Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::mul(unsigned short n, Uint16Array* r, unsigned ri, Uint16Array* a, unsigned ai, Uint16Array* b, unsigned bi) { cs_mul<uint16_t, Uint16Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::mul(unsigned short n, Uint32Array* r, unsigned ri, Uint32Array* a, unsigned ai, Uint32Array* b, unsigned bi) { cs_mul<uint32_t, Uint32Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::mul(unsigned short n, Uint8ClampedArray* r, unsigned ri, Uint8ClampedArray* a, unsigned ai, Uint8ClampedArray* b, unsigned bi) { cs_mul<uint8_t, Uint8ClampedArray>(n, r, ri, a, ai, b, bi); }

void Accelerate::mul(unsigned short n, Int8Array* r, unsigned ri, Int8Array* a, unsigned ai, Int8Array* b, unsigned bi) { cs_mul<int8_t, Int8Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::mul(unsigned short n, Int16Array* r, unsigned ri, Int16Array* a, unsigned ai, Int16Array* b, unsigned bi) { cs_mul<int16_t, Int16Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::mul(unsigned short n, Int32Array* r, unsigned ri, Int32Array* a, unsigned ai, Int32Array* b, unsigned bi) { cs_mul<int32_t, Int32Array>(n, r, ri, a, ai, b, bi); }

void Accelerate::mul(unsigned short n, Float32Array* r, unsigned ri, Float32Array* a, unsigned ai, Float32Array* b, unsigned bi) { cs_mul<float, Float32Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::mul(unsigned short n, Float64Array* r, unsigned ri, Float64Array* a, unsigned ai, Float64Array* b, unsigned bi) { cs_mul<double, Float64Array>(n, r, ri, a, ai, b, bi); }

void Accelerate::sqrt(unsigned short n, Uint8Array* r, unsigned ri, Uint8Array* a, unsigned ai) { cs_sqrt<uint8_t, Uint8Array>(n, r, ri, a, ai); }
void Accelerate::sqrt(unsigned short n, Uint16Array* r, unsigned ri, Uint16Array* a, unsigned ai) { cs_sqrt<uint16_t, Uint16Array>(n, r, ri, a, ai); }
void Accelerate::sqrt(unsigned short n, Uint32Array* r, unsigned ri, Uint32Array* a, unsigned ai) { cs_sqrt<uint32_t, Uint32Array>(n, r, ri, a, ai); }
void Accelerate::sqrt(unsigned short n, Uint8ClampedArray* r, unsigned ri, Uint8ClampedArray* a, unsigned ai) { cs_sqrt<uint8_t, Uint8ClampedArray>(n, r, ri, a, ai); }

void Accelerate::sqrt(unsigned short n, Int8Array* r, unsigned ri, Int8Array* a, unsigned ai) { cs_sqrt<int8_t, Int8Array>(n, r, ri, a, ai); }
void Accelerate::sqrt(unsigned short n, Int16Array* r, unsigned ri, Int16Array* a, unsigned ai) { cs_sqrt<int16_t, Int16Array>(n, r, ri, a, ai); }
void Accelerate::sqrt(unsigned short n, Int32Array* r, unsigned ri, Int32Array* a, unsigned ai) { cs_sqrt<int32_t, Int32Array>(n, r, ri, a, ai); }

void Accelerate::sqrt(unsigned short n, Float32Array* r, unsigned ri, Float32Array* a, unsigned ai) { cs_sqrt<float, Float32Array>(n, r, ri, a, ai); }
void Accelerate::sqrt(unsigned short n, Float64Array* r, unsigned ri, Float64Array* a, unsigned ai) { cs_sqrt<double, Float64Array>(n, r, ri, a, ai); }

void Accelerate::sub(unsigned short n, Uint8Array* r, unsigned ri, Uint8Array* a, unsigned ai, Uint8Array* b, unsigned bi) { cs_sub<uint8_t, Uint8Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::sub(unsigned short n, Uint16Array* r, unsigned ri, Uint16Array* a, unsigned ai, Uint16Array* b, unsigned bi) { cs_sub<uint16_t, Uint16Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::sub(unsigned short n, Uint32Array* r, unsigned ri, Uint32Array* a, unsigned ai, Uint32Array* b, unsigned bi) { cs_sub<uint32_t, Uint32Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::sub(unsigned short n, Uint8ClampedArray* r, unsigned ri, Uint8ClampedArray* a, unsigned ai, Uint8ClampedArray* b, unsigned bi) { cs_sub<uint8_t, Uint8ClampedArray>(n, r, ri, a, ai, b, bi); }

void Accelerate::sub(unsigned short n, Int8Array* r, unsigned ri, Int8Array* a, unsigned ai, Int8Array* b, unsigned bi) { cs_sub<int8_t, Int8Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::sub(unsigned short n, Int16Array* r, unsigned ri, Int16Array* a, unsigned ai, Int16Array* b, unsigned bi) { cs_sub<int16_t, Int16Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::sub(unsigned short n, Int32Array* r, unsigned ri, Int32Array* a, unsigned ai, Int32Array* b, unsigned bi) { cs_sub<int32_t, Int32Array>(n, r, ri, a, ai, b, bi); }

void Accelerate::sub(unsigned short n, Float32Array* r, unsigned ri, Float32Array* a, unsigned ai, Float32Array* b, unsigned bi) { cs_sub<float, Float32Array>(n, r, ri, a, ai, b, bi); }
void Accelerate::sub(unsigned short n, Float64Array* r, unsigned ri, Float64Array* a, unsigned ai, Float64Array* b, unsigned bi) { cs_sub<double, Float64Array>(n, r, ri, a, ai, b, bi); }

}
