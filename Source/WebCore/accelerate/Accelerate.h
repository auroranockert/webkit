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

#ifndef Accelerate_h
#define Accelerate_h

#include <wtf/ArrayBufferView.h>

#include <wtf/Uint8Array.h>
#include <wtf/Uint16Array.h>
#include <wtf/Uint32Array.h>
#include <wtf/Uint8ClampedArray.h>

#include <wtf/Int8Array.h>
#include <wtf/Int16Array.h>
#include <wtf/Int32Array.h>

#include <wtf/Float32Array.h>
#include <wtf/Float64Array.h>

namespace WebCore {

typedef int ExceptionCode;

class Accelerate : public RefCounted<Accelerate> {
public:
    static PassRefPtr<Accelerate> create();

    void abs(unsigned short n, Uint8Array* r, unsigned ri, Uint8Array* a, unsigned ai);
    void abs(unsigned short n, Uint16Array* r, unsigned ri, Uint16Array* a, unsigned ai);
    void abs(unsigned short n, Uint32Array* r, unsigned ri, Uint32Array* a, unsigned ai);

    void abs(unsigned short n, Uint8ClampedArray* r, unsigned ri, Uint8ClampedArray* a, unsigned ai);

    void abs(unsigned short n, Int8Array* r, unsigned ri, Int8Array* a, unsigned ai);
    void abs(unsigned short n, Int16Array* r, unsigned ri, Int16Array* a, unsigned ai);
    void abs(unsigned short n, Int32Array* r, unsigned ri, Int32Array* a, unsigned ai);

    void abs(unsigned short n, Float32Array* r, unsigned ri, Float32Array* a, unsigned ai);
    void abs(unsigned short n, Float64Array* r, unsigned ri, Float64Array* a, unsigned ai);

    void add(unsigned short n, Uint8Array* r, unsigned ri, Uint8Array* a, unsigned ai, Uint8Array* b, unsigned bi);
    void add(unsigned short n, Uint16Array* r, unsigned ri, Uint16Array* a, unsigned ai, Uint16Array* b, unsigned bi);
    void add(unsigned short n, Uint32Array* r, unsigned ri, Uint32Array* a, unsigned ai, Uint32Array* b, unsigned bi);

    void add(unsigned short n, Uint8ClampedArray* r, unsigned ri, Uint8ClampedArray* a, unsigned ai, Uint8ClampedArray* b, unsigned bi);

    void add(unsigned short n, Int8Array* r, unsigned ri, Int8Array* a, unsigned ai, Int8Array* b, unsigned bi);
    void add(unsigned short n, Int16Array* r, unsigned ri, Int16Array* a, unsigned ai, Int16Array* b, unsigned bi);
    void add(unsigned short n, Int32Array* r, unsigned ri, Int32Array* a, unsigned ai, Int32Array* b, unsigned bi);

    void add(unsigned short n, Float32Array* r, unsigned ri, Float32Array* a, unsigned ai, Float32Array* b, unsigned bi);
    void add(unsigned short n, Float64Array* r, unsigned ri, Float64Array* a, unsigned ai, Float64Array* b, unsigned bi);

    void div(unsigned short n, Uint8Array* r, unsigned ri, Uint8Array* a, unsigned ai, Uint8Array* b, unsigned bi);
    void div(unsigned short n, Uint16Array* r, unsigned ri, Uint16Array* a, unsigned ai, Uint16Array* b, unsigned bi);
    void div(unsigned short n, Uint32Array* r, unsigned ri, Uint32Array* a, unsigned ai, Uint32Array* b, unsigned bi);

    void div(unsigned short n, Uint8ClampedArray* r, unsigned ri, Uint8ClampedArray* a, unsigned ai, Uint8ClampedArray* b, unsigned bi);

    void div(unsigned short n, Int8Array* r, unsigned ri, Int8Array* a, unsigned ai, Int8Array* b, unsigned bi);
    void div(unsigned short n, Int16Array* r, unsigned ri, Int16Array* a, unsigned ai, Int16Array* b, unsigned bi);
    void div(unsigned short n, Int32Array* r, unsigned ri, Int32Array* a, unsigned ai, Int32Array* b, unsigned bi);

    void div(unsigned short n, Float32Array* r, unsigned ri, Float32Array* a, unsigned ai, Float32Array* b, unsigned bi);
    void div(unsigned short n, Float64Array* r, unsigned ri, Float64Array* a, unsigned ai, Float64Array* b, unsigned bi);

    void max(unsigned short n, Uint8Array* r, unsigned ri, Uint8Array* a, unsigned ai, Uint8Array* b, unsigned bi);
    void max(unsigned short n, Uint16Array* r, unsigned ri, Uint16Array* a, unsigned ai, Uint16Array* b, unsigned bi);
    void max(unsigned short n, Uint32Array* r, unsigned ri, Uint32Array* a, unsigned ai, Uint32Array* b, unsigned bi);

    void max(unsigned short n, Uint8ClampedArray* r, unsigned ri, Uint8ClampedArray* a, unsigned ai, Uint8ClampedArray* b, unsigned bi);

    void max(unsigned short n, Int8Array* r, unsigned ri, Int8Array* a, unsigned ai, Int8Array* b, unsigned bi);
    void max(unsigned short n, Int16Array* r, unsigned ri, Int16Array* a, unsigned ai, Int16Array* b, unsigned bi);
    void max(unsigned short n, Int32Array* r, unsigned ri, Int32Array* a, unsigned ai, Int32Array* b, unsigned bi);

    void max(unsigned short n, Float32Array* r, unsigned ri, Float32Array* a, unsigned ai, Float32Array* b, unsigned bi);
    void max(unsigned short n, Float64Array* r, unsigned ri, Float64Array* a, unsigned ai, Float64Array* b, unsigned bi);

    void min(unsigned short n, Uint8Array* r, unsigned ri, Uint8Array* a, unsigned ai, Uint8Array* b, unsigned bi);
    void min(unsigned short n, Uint16Array* r, unsigned ri, Uint16Array* a, unsigned ai, Uint16Array* b, unsigned bi);
    void min(unsigned short n, Uint32Array* r, unsigned ri, Uint32Array* a, unsigned ai, Uint32Array* b, unsigned bi);

    void min(unsigned short n, Uint8ClampedArray* r, unsigned ri, Uint8ClampedArray* a, unsigned ai, Uint8ClampedArray* b, unsigned bi);

    void min(unsigned short n, Int8Array* r, unsigned ri, Int8Array* a, unsigned ai, Int8Array* b, unsigned bi);
    void min(unsigned short n, Int16Array* r, unsigned ri, Int16Array* a, unsigned ai, Int16Array* b, unsigned bi);
    void min(unsigned short n, Int32Array* r, unsigned ri, Int32Array* a, unsigned ai, Int32Array* b, unsigned bi);

    void min(unsigned short n, Float32Array* r, unsigned ri, Float32Array* a, unsigned ai, Float32Array* b, unsigned bi);
    void min(unsigned short n, Float64Array* r, unsigned ri, Float64Array* a, unsigned ai, Float64Array* b, unsigned bi);

    void mul(unsigned short n, Uint8Array* r, unsigned ri, Uint8Array* a, unsigned ai, Uint8Array* b, unsigned bi);
    void mul(unsigned short n, Uint16Array* r, unsigned ri, Uint16Array* a, unsigned ai, Uint16Array* b, unsigned bi);
    void mul(unsigned short n, Uint32Array* r, unsigned ri, Uint32Array* a, unsigned ai, Uint32Array* b, unsigned bi);

    void mul(unsigned short n, Uint8ClampedArray* r, unsigned ri, Uint8ClampedArray* a, unsigned ai, Uint8ClampedArray* b, unsigned bi);

    void mul(unsigned short n, Int8Array* r, unsigned ri, Int8Array* a, unsigned ai, Int8Array* b, unsigned bi);
    void mul(unsigned short n, Int16Array* r, unsigned ri, Int16Array* a, unsigned ai, Int16Array* b, unsigned bi);
    void mul(unsigned short n, Int32Array* r, unsigned ri, Int32Array* a, unsigned ai, Int32Array* b, unsigned bi);

    void mul(unsigned short n, Float32Array* r, unsigned ri, Float32Array* a, unsigned ai, Float32Array* b, unsigned bi);
    void mul(unsigned short n, Float64Array* r, unsigned ri, Float64Array* a, unsigned ai, Float64Array* b, unsigned bi);

    void sqrt(unsigned short n, Uint8Array* r, unsigned ri, Uint8Array* a, unsigned ai);
    void sqrt(unsigned short n, Uint16Array* r, unsigned ri, Uint16Array* a, unsigned ai);
    void sqrt(unsigned short n, Uint32Array* r, unsigned ri, Uint32Array* a, unsigned ai);

    void sqrt(unsigned short n, Uint8ClampedArray* r, unsigned ri, Uint8ClampedArray* a, unsigned ai);

    void sqrt(unsigned short n, Int8Array* r, unsigned ri, Int8Array* a, unsigned ai);
    void sqrt(unsigned short n, Int16Array* r, unsigned ri, Int16Array* a, unsigned ai);
    void sqrt(unsigned short n, Int32Array* r, unsigned ri, Int32Array* a, unsigned ai);

    void sqrt(unsigned short n, Float32Array* r, unsigned ri, Float32Array* a, unsigned ai);
    void sqrt(unsigned short n, Float64Array* r, unsigned ri, Float64Array* a, unsigned ai);

    void sub(unsigned short n, Uint8Array* r, unsigned ri, Uint8Array* a, unsigned ai, Uint8Array* b, unsigned bi);
    void sub(unsigned short n, Uint16Array* r, unsigned ri, Uint16Array* a, unsigned ai, Uint16Array* b, unsigned bi);
    void sub(unsigned short n, Uint32Array* r, unsigned ri, Uint32Array* a, unsigned ai, Uint32Array* b, unsigned bi);

    void sub(unsigned short n, Uint8ClampedArray* r, unsigned ri, Uint8ClampedArray* a, unsigned ai, Uint8ClampedArray* b, unsigned bi);

    void sub(unsigned short n, Int8Array* r, unsigned ri, Int8Array* a, unsigned ai, Int8Array* b, unsigned bi);
    void sub(unsigned short n, Int16Array* r, unsigned ri, Int16Array* a, unsigned ai, Int16Array* b, unsigned bi);
    void sub(unsigned short n, Int32Array* r, unsigned ri, Int32Array* a, unsigned ai, Int32Array* b, unsigned bi);

    void sub(unsigned short n, Float32Array* r, unsigned ri, Float32Array* a, unsigned ai, Float32Array* b, unsigned bi);
    void sub(unsigned short n, Float64Array* r, unsigned ri, Float64Array* a, unsigned ai, Float64Array* b, unsigned bi);
};


} // namespace WebCore

#endif // Accelerate_h
