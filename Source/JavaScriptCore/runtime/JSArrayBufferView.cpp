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

#include "JSGlobalObject.h"
#include "JSArrayBufferView.h"

#include "Lookup.h"

#include <wtf/ArrayBuffer.h>
#include <wtf/GetPtr.h>

namespace JSC {

ASSERT_CLASS_FITS_IN_CELL(JSArrayBufferView);

static const HashTableValue JSArrayBufferViewTableValues[] = {
    { "buffer",     DontDelete | ReadOnly, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsArrayBufferViewBuffer),     (intptr_t)0, NoIntrinsic },
    { "byteOffset", DontDelete | ReadOnly, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsArrayBufferViewByteOffset), (intptr_t)0, NoIntrinsic },
    { "byteLength", DontDelete | ReadOnly, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsArrayBufferViewByteLength), (intptr_t)0, NoIntrinsic },
    { 0, 0, 0, 0, NoIntrinsic }
};

static const HashTable JSArrayBufferViewTable = { 8, 7, JSArrayBufferViewTableValues, 0 };

static const HashTable* getJSArrayBufferViewTable(ExecState* exec)
{
    UNUSED_PARAM(exec);
    return &JSArrayBufferViewTable;
}

const ClassInfo JSArrayBufferView::s_info = { "ArrayBufferView", &Base::s_info, 0, getJSArrayBufferViewTable, CREATE_METHOD_TABLE(JSArrayBufferView) };

JSArrayBufferView::JSArrayBufferView(Structure* structure, JSGlobalObject* globalObject, PassRefPtr<ArrayBufferView> impl) : Base(globalObject->globalData(), structure), m_impl(impl.leakRef())
{
}

void JSArrayBufferView::finishCreation(JSGlobalData& globalData)
{
    Base::finishCreation(globalData);

    ASSERT(inherits(&s_info));
}

void JSArrayBufferView::destroy(JSC::JSCell* cell)
{
    JSArrayBufferView* thisObject = static_cast<JSArrayBufferView*>(cell);

    thisObject->JSArrayBufferView::~JSArrayBufferView();
}

JSArrayBufferView::~JSArrayBufferView()
{
    if (m_impl) {
        m_impl->deref(), m_impl = NULL;
    }
}

bool JSArrayBufferView::getOwnPropertySlot(JSCell* cell, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSArrayBufferView* thisObject = jsCast<JSArrayBufferView*>(cell);

    ASSERT_GC_OBJECT_INHERITS(thisObject, &s_info);

    return getStaticValueSlot<JSArrayBufferView, Base>(exec, getJSArrayBufferViewTable(exec), thisObject, propertyName, slot);
}

bool JSArrayBufferView::getOwnPropertyDescriptor(JSObject* object, ExecState* exec, PropertyName propertyName, PropertyDescriptor& descriptor)
{
    JSArrayBufferView* thisObject = jsCast<JSArrayBufferView*>(object);

    ASSERT_GC_OBJECT_INHERITS(thisObject, &s_info);

    return getStaticValueDescriptor<JSArrayBufferView, Base>(exec, getJSArrayBufferViewTable(exec), thisObject, propertyName, descriptor);
}

JSValue jsArrayBufferViewBuffer(ExecState* exec, JSValue slotBase, PropertyName propertyName)
{
    UNUSED_PARAM(exec); UNUSED_PARAM(slotBase); UNUSED_PARAM(propertyName);
    // JSArrayBufferView* castedThis = jsCast<JSArrayBufferView*>(asObject(slotBase));

    return jsUndefined(); // toJS(exec, castedThis->globalObject(), WTF::getPtr(castedThis->m_impl->buffer()));
}


JSValue jsArrayBufferViewByteOffset(ExecState* exec, JSValue slotBase, PropertyName)
{
    UNUSED_PARAM(exec);

    return jsNumber(jsCast<JSArrayBufferView*>(asObject(slotBase))->m_impl->byteOffset());;
}


JSValue jsArrayBufferViewByteLength(ExecState* exec, JSValue slotBase, PropertyName)
{
    UNUSED_PARAM(exec);

    return jsNumber(jsCast<JSArrayBufferView*>(asObject(slotBase))->m_impl->byteLength());;
}

}
