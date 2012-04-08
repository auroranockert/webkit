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
#include "JSInt32Array.h"

#include "Error.h"

namespace JSC {

static JSValue int32ArrayLength(ExecState* exec, JSValue slotBase, const Identifier&);

}

#include "JSInt32Array.lut.h"

namespace JSC {

ASSERT_HAS_TRIVIAL_DESTRUCTOR(JSInt32Array);

const ClassInfo JSInt32Array::s_info = { "Int32Array", &Base::s_info, 0, ExecState::int32ArrayTable, CREATE_METHOD_TABLE(JSInt32Array) };

ASSERT_CLASS_FITS_IN_CELL(JSInt32Array);

/* Source for JSInt32Array.lut.h
@begin int32ArrayTable
  length       int32ArrayLength         DontDelete|ReadOnly 0
@end
*/

JSInt32Array::JSInt32Array(JSGlobalData& globalData, Structure* structure, PassRefPtr<Int32Array> impl)
      : JSArrayBufferView(globalData, structure, impl)
{
}

JSInt32Array::~JSInt32Array()
{
  releaseImplIfNotNull();
}

static void int32ArraySetterByIndex(JSInt32Array* current, unsigned index, double value)
{
    current->impl()->set(index, static_cast<int32_t>(value));
}

static JSValue int32ArrayGetByIndex(JSInt32Array* current, unsigned index)
{
    double result = static_cast<Int32Array*>(current->impl())->item(index);

    if (isnan(result)) {
        return jsNaN();
	}

    return JSValue(result);
}

bool JSInt32Array::getOwnPropertySlot(JSCell* cell, ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    JSInt32Array* thisObject = jsCast<JSInt32Array*>(cell);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok && index < static_cast<Int32Array*>(thisObject->impl())->length()) {
        slot.setValue(int32ArrayGetByIndex(thisObject, index));
        return true;
    }

    return getStaticValueSlot<JSInt32Array, Base>(exec, ExecState::int32ArrayTable(exec), thisObject, propertyName, slot);
}

bool JSInt32Array::getOwnPropertyDescriptor(JSObject* object, ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    JSInt32Array* thisObject = jsCast<JSInt32Array*>(object);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok && index < static_cast<Int32Array*>(thisObject->impl())->length()) {
        descriptor.setDescriptor(int32ArrayGetByIndex(thisObject, index), DontDelete);
        return true;
    }

    return getStaticValueDescriptor<JSInt32Array, Base>(exec, ExecState::int32ArrayTable(exec), thisObject, propertyName, descriptor);
}

bool JSInt32Array::getOwnPropertySlotByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, PropertySlot& slot)
{
    JSInt32Array* thisObject = jsCast<JSInt32Array*>(cell);

    if (propertyName < static_cast<Int32Array*>(thisObject->impl())->length()) {
        slot.setValue(int32ArrayGetByIndex(thisObject, propertyName));
        return true;
    }

    return thisObject->methodTable()->getOwnPropertySlot(thisObject, exec, Identifier::from(exec, propertyName), slot);
}

void JSInt32Array::put(JSCell* cell, ExecState* exec, const Identifier& propertyName, JSValue value, PutPropertySlot& slot)
{
    JSInt32Array* thisObject = jsCast<JSInt32Array*>(cell);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok) {
        int32ArraySetterByIndex(thisObject, index, value.toNumber(exec));

        return;
    }

    Base::put(thisObject, exec, propertyName, value, slot);
}

void JSInt32Array::putByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, JSValue value, bool)
{
    JSInt32Array* thisObject = jsCast<JSInt32Array*>(cell);

    int32ArraySetterByIndex(thisObject, propertyName, value.toNumber(exec));

    return;
}

static JSValue int32ArrayLength(ExecState* exec, JSValue slotBase, const Identifier&)
{
    Int32Array* impl = static_cast<Int32Array*>(static_cast<JSInt32Array*>(asObject(slotBase))->impl());
    JSValue result = jsNumber(impl->length());
    return result;
}

}
