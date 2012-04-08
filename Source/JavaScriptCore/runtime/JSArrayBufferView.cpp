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
#include "JSArrayBufferView.h"

#include "JSArrayBuffer.h"

namespace JSC {

static JSValue arrayBufferViewBuffer(ExecState* exec, JSValue slotBase, const Identifier&);
static JSValue arrayBufferViewByteOffset(ExecState* exec, JSValue slotBase, const Identifier&);
static JSValue arrayBufferViewByteLength(ExecState* exec, JSValue slotBase, const Identifier&);

}

#include "JSArrayBufferView.lut.h"

namespace JSC {

ASSERT_HAS_TRIVIAL_DESTRUCTOR(JSArrayBufferView);

const ClassInfo JSArrayBufferView::s_info = { "ArrayBufferView", &JSArrayBufferView::s_info, 0, ExecState::arrayBufferViewTable, CREATE_METHOD_TABLE(JSArrayBufferView) };

/* Source for JSArrayBufferView.lut.h
@begin arrayBufferViewTable
  buffer       arrayBufferViewBuffer         DontDelete|ReadOnly 0
  byteOffset   arrayBufferViewByteOffset     DontDelete|ReadOnly 0
  byteLength   arrayBufferViewByteLength     DontDelete|ReadOnly 0
@end
*/

ASSERT_CLASS_FITS_IN_CELL(JSArrayBufferView);

JSArrayBufferView::JSArrayBufferView(JSGlobalData& globalData, Structure* structure, PassRefPtr<ArrayBufferView> impl)
    : JSNonFinalObject(globalData, structure)
    , m_impl(impl.leakRef())
{
}

JSArrayBufferView::~JSArrayBufferView()
{
    releaseImplIfNotNull();
}

void JSArrayBufferView::finishCreation(JSGlobalData& globalData)
{
    Base::finishCreation(globalData);
    ASSERT(inherits(&s_info));
}

bool JSArrayBufferView::getOwnPropertySlot(JSCell* cell, ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSArrayBufferView, Base>(exec, ExecState::arrayBufferTable(exec), jsCast<JSArrayBufferView*>(cell), propertyName, slot);
}

bool JSArrayBufferView::getOwnPropertyDescriptor(JSObject* object, ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticValueDescriptor<JSArrayBufferView, Base>(exec, ExecState::arrayBufferTable(exec), jsCast<JSArrayBufferView*>(object), propertyName, descriptor);
}

static JSValue arrayBufferViewByteOffset(ExecState* exec, JSValue slotBase, const Identifier&)
{
    ArrayBufferView* impl = static_cast<ArrayBufferView*>(static_cast<JSArrayBufferView*>(asObject(slotBase))->impl());

    return jsNumber(impl->byteOffset());
}

static JSValue arrayBufferViewByteLength(ExecState* exec, JSValue slotBase, const Identifier&)
{
    ArrayBufferView* impl = static_cast<ArrayBufferView*>(static_cast<JSArrayBufferView*>(asObject(slotBase))->impl());

    return jsNumber(impl->byteLength());
}

static JSValue arrayBufferViewBuffer(ExecState* exec, JSValue slotBase, const Identifier&)
{
    ArrayBufferView* impl = static_cast<ArrayBufferView*>(static_cast<JSArrayBufferView*>(asObject(slotBase))->impl());

    return toJS(exec, asObject(slotBase)->globalObject(), impl->buffer().get());
}

}
