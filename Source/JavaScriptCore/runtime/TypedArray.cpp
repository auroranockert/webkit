/*
 * Copyright (C) 2012 Apple Inc. All rights reserved.
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

#include "config.h"

#include "TypedArray.h"
#include "JSArrayBufferViewPrototype.h"

namespace JSC {

template <> const ClassInfo JSUint8Array::s_info = { "Uint8Array" , &Base::s_info, 0, 0, CREATE_METHOD_TABLE(JSUint8Array) };
template <> const ClassInfo JSUint8ClampedArray::s_info = { "Uint8ClampedArray" , &Base::s_info, 0, 0, CREATE_METHOD_TABLE(JSUint8ClampedArray) };
template <> const ClassInfo JSUint16Array::s_info = { "Uint16Array" , &Base::s_info, 0, 0, CREATE_METHOD_TABLE(JSUint16Array) };
template <> const ClassInfo JSUint32Array::s_info = { "Uint32Array" , &Base::s_info, 0, 0, CREATE_METHOD_TABLE(JSUint32Array) };

template <> const ClassInfo JSInt8Array::s_info = { "Int8Array" , &Base::s_info, 0, 0, CREATE_METHOD_TABLE(JSInt8Array) };
template <> const ClassInfo JSInt16Array::s_info = { "Int16Array" , &Base::s_info, 0, 0, CREATE_METHOD_TABLE(JSInt16Array) };
template <> const ClassInfo JSInt32Array::s_info = { "Int32Array" , &Base::s_info, 0, 0, CREATE_METHOD_TABLE(JSInt32Array) };

template <> const ClassInfo JSFloat32Array::s_info = { "Float32Array" , &Base::s_info, 0, 0, CREATE_METHOD_TABLE(JSFloat32Array) };
template <> const ClassInfo JSFloat64Array::s_info = { "Float64Array" , &Base::s_info, 0, 0, CREATE_METHOD_TABLE(JSFloat64Array) };

template <typename T>
static inline EncodedJSValue constructTypedArray(ExecState* callFrame) {
    if (callFrame->argumentCount() < 1) {
        return JSValue::encode(jsUndefined());
    }

    int32_t length = callFrame->argument(0).toInt32(callFrame);

    if (length < 0) {
        return JSValue::encode(jsUndefined());
    }

    JSCell* prototype = static_cast<JSCell*>(callFrame->lexicalGlobalObject()->arrayBufferViewPrototype());

    Structure* structure = T::createStructure(callFrame->globalData(), callFrame->lexicalGlobalObject(), JSValue(prototype));

    return JSValue::encode(T::create(structure, callFrame->lexicalGlobalObject(), T::Implementation::create(length)));
}

EncodedJSValue constructJSUint8Array(ExecState* callFrame) { return constructTypedArray<JSUint8Array>(callFrame); }
EncodedJSValue constructJSUint8ClampedArray(ExecState* callFrame) { return constructTypedArray<JSUint8ClampedArray>(callFrame); }
EncodedJSValue constructJSUint16Array(ExecState* callFrame) { return constructTypedArray<JSUint16Array>(callFrame); }
EncodedJSValue constructJSUint32Array(ExecState* callFrame) { return constructTypedArray<JSUint32Array>(callFrame); }

EncodedJSValue constructJSInt8Array(ExecState* callFrame) { return constructTypedArray<JSInt8Array>(callFrame); }
EncodedJSValue constructJSInt16Array(ExecState* callFrame) { return constructTypedArray<JSInt16Array>(callFrame); }
EncodedJSValue constructJSInt32Array(ExecState* callFrame) { return constructTypedArray<JSInt32Array>(callFrame); }

EncodedJSValue constructJSFloat32Array(ExecState* callFrame) { return constructTypedArray<JSFloat32Array>(callFrame); }
EncodedJSValue constructJSFloat64Array(ExecState* callFrame) { return constructTypedArray<JSFloat64Array>(callFrame); }

}
