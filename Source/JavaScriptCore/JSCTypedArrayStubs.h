/*
 * Copyright (C) 2012 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef JSCTypedArrayStubs_h
#define JSCTypedArrayStubs_h

#include "JSArrayBufferView.h"
#include "JSArrayBufferViewPrototype.h"

#include "TypedArray.h"

namespace JSC {

template <typename T>
static EncodedJSValue JSC_HOST_CALL constructTypedArray(ExecState* callFrame) {
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

static EncodedJSValue JSC_HOST_CALL constructJSUint8Array(ExecState* callFrame) { return constructTypedArray<JSUint8Array>(callFrame); }
static EncodedJSValue JSC_HOST_CALL constructJSUint8ClampedArray(ExecState* callFrame) { return constructTypedArray<JSUint8ClampedArray>(callFrame); }
static EncodedJSValue JSC_HOST_CALL constructJSUint16Array(ExecState* callFrame) { return constructTypedArray<JSUint16Array>(callFrame); }
static EncodedJSValue JSC_HOST_CALL constructJSUint32Array(ExecState* callFrame) { return constructTypedArray<JSUint32Array>(callFrame); }

static EncodedJSValue JSC_HOST_CALL constructJSInt8Array(ExecState* callFrame) { return constructTypedArray<JSInt8Array>(callFrame); }
static EncodedJSValue JSC_HOST_CALL constructJSInt16Array(ExecState* callFrame) { return constructTypedArray<JSInt16Array>(callFrame); }
static EncodedJSValue JSC_HOST_CALL constructJSInt32Array(ExecState* callFrame) { return constructTypedArray<JSInt32Array>(callFrame); }

static EncodedJSValue JSC_HOST_CALL constructJSFloat32Array(ExecState* callFrame) { return constructTypedArray<JSFloat32Array>(callFrame); }
static EncodedJSValue JSC_HOST_CALL constructJSFloat64Array(ExecState* callFrame) { return constructTypedArray<JSFloat64Array>(callFrame); }

}

#endif
