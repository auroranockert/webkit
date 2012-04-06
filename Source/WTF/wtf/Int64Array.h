/*
 * Copyright (C) 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Google Inc. All rights reserved.
 * Copyright (C) 2012 Jens Nockert (jens@nockert.se)
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef Int64Array_h
#define Int64Array_h

#include <stdint.h>
#include <wtf/IntegralTypedArrayBase.h>

namespace WTF {

class Int64Array : public IntegralTypedArrayBase<int64_t> {
public:
    static inline PassRefPtr<Int64Array> create(unsigned length);
    static inline PassRefPtr<Int64Array> create(const int64_t* array, unsigned length);
    static inline PassRefPtr<Int64Array> create(PassRefPtr<ArrayBuffer>, unsigned byteOffset, unsigned length);

    // Can’t use "using" here due to a bug in the RVCT compiler.
    bool set(TypedArrayBase<int64_t>* array, unsigned offset) { return TypedArrayBase<int64_t>::set(array, offset); }
    void set(unsigned index, double value) { IntegralTypedArrayBase<int64_t>::set(index, value); }

    inline PassRefPtr<Int64Array> subarray(int start) const;
    inline PassRefPtr<Int64Array> subarray(int start, int end) const;

private:
    inline Int64Array(PassRefPtr<ArrayBuffer>,
                  unsigned byteOffset,
                  unsigned length);
    // Make constructor visible to superclass.
    friend class TypedArrayBase<int64_t>;

    // Overridden from ArrayBufferView.
    virtual bool isLongLongArray() const { return true; }
};

PassRefPtr<Int64Array> Int64Array::create(unsigned length)
{
    return TypedArrayBase<int64_t>::create<Int64Array>(length);
}

PassRefPtr<Int64Array> Int64Array::create(const int64_t* array, unsigned length)
{
    return TypedArrayBase<int64_t>::create<Int64Array>(array, length);
}

PassRefPtr<Int64Array> Int64Array::create(PassRefPtr<ArrayBuffer> buffer, unsigned byteOffset, unsigned length)
{
    return TypedArrayBase<int64_t>::create<Int64Array>(buffer, byteOffset, length);
}

Int64Array::Int64Array(PassRefPtr<ArrayBuffer> buffer, unsigned byteOffset, unsigned length)
    : IntegralTypedArrayBase<int64_t>(buffer, byteOffset, length)
{
}

PassRefPtr<Int64Array> Int64Array::subarray(int start) const
{
    return subarray(start, length());
}

PassRefPtr<Int64Array> Int64Array::subarray(int start, int end) const
{
    return subarrayImpl<Int64Array>(start, end);
}

} // namespace WTF

using WTF::Int64Array;

#endif // Int64Array_h
