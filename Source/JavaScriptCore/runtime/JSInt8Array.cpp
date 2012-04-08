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
#include "JSInt8Array.h"

#include "Error.h"

namespace JSC {

static JSValue int8ArrayLength(ExecState* exec, JSValue slotBase, const Identifier&);

}

#include "JSInt8Array.lut.h"

namespace JSC {

ASSERT_HAS_TRIVIAL_DESTRUCTOR(JSInt8Array);

const ClassInfo JSInt8Array::s_info = { "Int8Array", &Base::s_info, 0, ExecState::int8ArrayTable, CREATE_METHOD_TABLE(JSInt8Array) };

ASSERT_CLASS_FITS_IN_CELL(JSInt8Array);

/* Source for JSInt8Array.lut.h
@begin int8ArrayTable
  length       int8ArrayLength         DontDelete|ReadOnly 0
@end
*/

JSInt8Array::JSInt8Array(JSGlobalData& globalData, Structure* structure, PassRefPtr<Int8Array> impl)
      : JSArrayBufferView(globalData, structure, impl)
{
}

JSInt8Array::~JSInt8Array()
{
  releaseImplIfNotNull();
}

static void int8ArraySetterByIndex(JSInt8Array* current, unsigned index, double value)
{
    current->impl()->set(index, static_cast<int8_t>(value));
}

static JSValue int8ArrayGetByIndex(JSInt8Array* current, unsigned index)
{
    double result = static_cast<Int8Array*>(current->impl())->item(index);

    if (isnan(result)) {
        return jsNaN();
	}

    return JSValue(result);
}

bool JSInt8Array::getOwnPropertySlot(JSCell* cell, ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    JSInt8Array* thisObject = jsCast<JSInt8Array*>(cell);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok && index < static_cast<Int8Array*>(thisObject->impl())->length()) {
        slot.setValue(int8ArrayGetByIndex(thisObject, index));
        return true;
    }

    return getStaticValueSlot<JSInt8Array, Base>(exec, ExecState::int8ArrayTable(exec), thisObject, propertyName, slot);
}

bool JSInt8Array::getOwnPropertyDescriptor(JSObject* object, ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    JSInt8Array* thisObject = jsCast<JSInt8Array*>(object);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok && index < static_cast<Int8Array*>(thisObject->impl())->length()) {
        descriptor.setDescriptor(int8ArrayGetByIndex(thisObject, index), DontDelete);
        return true;
    }

    return getStaticValueDescriptor<JSInt8Array, Base>(exec, ExecState::int8ArrayTable(exec), thisObject, propertyName, descriptor);
}

bool JSInt8Array::getOwnPropertySlotByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, PropertySlot& slot)
{
    JSInt8Array* thisObject = jsCast<JSInt8Array*>(cell);

    if (propertyName < static_cast<Int8Array*>(thisObject->impl())->length()) {
        slot.setValue(int8ArrayGetByIndex(thisObject, propertyName));
        return true;
    }

    return thisObject->methodTable()->getOwnPropertySlot(thisObject, exec, Identifier::from(exec, propertyName), slot);
}

void JSInt8Array::put(JSCell* cell, ExecState* exec, const Identifier& propertyName, JSValue value, PutPropertySlot& slot)
{
    JSInt8Array* thisObject = jsCast<JSInt8Array*>(cell);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok) {
        int8ArraySetterByIndex(thisObject, index, value.toNumber(exec));

        return;
    }

    Base::put(thisObject, exec, propertyName, value, slot);
}

void JSInt8Array::putByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, JSValue value, bool)
{
    JSInt8Array* thisObject = jsCast<JSInt8Array*>(cell);

    int8ArraySetterByIndex(thisObject, propertyName, value.toNumber(exec));

    return;
}

static JSValue int8ArrayLength(ExecState* exec, JSValue slotBase, const Identifier&)
{
    Int8Array* impl = static_cast<Int8Array*>(static_cast<JSInt8Array*>(asObject(slotBase))->impl());
    JSValue result = jsNumber(impl->length());
    return result;
}

}
