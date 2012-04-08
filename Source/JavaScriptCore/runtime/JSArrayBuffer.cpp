/*
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

#include "config.h"
#include "JSArrayBuffer.h"

namespace JSC {

static JSValue arrayBufferByteLength(ExecState* exec, JSValue slotBase, const Identifier&);

}

#include "JSArrayBuffer.lut.h"

namespace JSC {

ASSERT_HAS_TRIVIAL_DESTRUCTOR(JSArrayBuffer);

const ClassInfo JSArrayBuffer::s_info = { "ArrayBuffer", &Base::s_info, 0, ExecState::arrayBufferTable, CREATE_METHOD_TABLE(JSArrayBuffer) };

/* Source for JSArrayBuffer.lut.h
@begin arrayBufferTable
  byteLength   arrayBufferByteLength     DontDelete|ReadOnly 0
@end
*/

ASSERT_CLASS_FITS_IN_CELL(JSArrayBuffer);

JSArrayBuffer::JSArrayBuffer(JSGlobalData& globalData, Structure* structure, PassRefPtr<ArrayBuffer> impl)
    : JSNonFinalObject(globalData, structure)
    , m_impl(impl.leakRef())
{
}

JSArrayBuffer::~JSArrayBuffer()
{
    releaseImplIfNotNull();
}

void JSArrayBuffer::finishCreation(JSGlobalData& globalData)
{
    Base::finishCreation(globalData);
    ASSERT(inherits(&s_info));
}

bool JSArrayBuffer::getOwnPropertySlot(JSCell* cell, ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSArrayBuffer, Base>(exec, ExecState::arrayBufferTable(exec), jsCast<JSArrayBuffer*>(cell), propertyName, slot);
}

bool JSArrayBuffer::getOwnPropertyDescriptor(JSObject* object, ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticValueDescriptor<JSArrayBuffer, Base>(exec, ExecState::arrayBufferTable(exec), jsCast<JSArrayBuffer*>(object), propertyName, descriptor);
}

static JSValue arrayBufferByteLength(ExecState* exec, JSValue slotBase, const Identifier&)
{
    ArrayBuffer* impl = static_cast<ArrayBuffer*>(static_cast<JSArrayBuffer*>(asObject(slotBase))->impl());
	
    return jsNumber(impl->byteLength());
}

}
