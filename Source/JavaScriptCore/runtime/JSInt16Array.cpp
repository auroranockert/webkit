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
#include "JSInt16Array.h"

#include "Error.h"

namespace JSC {

static JSValue int16ArrayLength(ExecState* exec, JSValue slotBase, const Identifier&);

}

#include "JSInt16Array.lut.h"

namespace JSC {

ASSERT_HAS_TRIVIAL_DESTRUCTOR(JSInt16Array);

const ClassInfo JSInt16Array::s_info = { "Int16Array", &Base::s_info, 0, ExecState::int16ArrayTable, CREATE_METHOD_TABLE(JSInt16Array) };

ASSERT_CLASS_FITS_IN_CELL(JSInt16Array);

/* Source for JSInt16Array.lut.h
@begin int16ArrayTable
  length       int16ArrayLength         DontDelete|ReadOnly 0
@end
*/

JSInt16Array::JSInt16Array(JSGlobalData& globalData, Structure* structure, PassRefPtr<Int16Array> impl)
      : JSArrayBufferView(globalData, structure, impl)
{
}

JSInt16Array::~JSInt16Array()
{
  releaseImplIfNotNull();
}

static void int16ArraySetterByIndex(JSInt16Array* current, unsigned index, double value)
{
    current->impl()->set(index, static_cast<int16_t>(value));
}

static JSValue int16ArrayGetByIndex(JSInt16Array* current, unsigned index)
{
    double result = static_cast<Int16Array*>(current->impl())->item(index);

    if (isnan(result)) {
        return jsNaN();
	}

    return JSValue(result);
}

bool JSInt16Array::getOwnPropertySlot(JSCell* cell, ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    JSInt16Array* thisObject = jsCast<JSInt16Array*>(cell);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok && index < static_cast<Int16Array*>(thisObject->impl())->length()) {
        slot.setValue(int16ArrayGetByIndex(thisObject, index));
        return true;
    }

    return getStaticValueSlot<JSInt16Array, Base>(exec, ExecState::int16ArrayTable(exec), thisObject, propertyName, slot);
}

bool JSInt16Array::getOwnPropertyDescriptor(JSObject* object, ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    JSInt16Array* thisObject = jsCast<JSInt16Array*>(object);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok && index < static_cast<Int16Array*>(thisObject->impl())->length()) {
        descriptor.setDescriptor(int16ArrayGetByIndex(thisObject, index), DontDelete);
        return true;
    }

    return getStaticValueDescriptor<JSInt16Array, Base>(exec, ExecState::int16ArrayTable(exec), thisObject, propertyName, descriptor);
}

bool JSInt16Array::getOwnPropertySlotByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, PropertySlot& slot)
{
    JSInt16Array* thisObject = jsCast<JSInt16Array*>(cell);

    if (propertyName < static_cast<Int16Array*>(thisObject->impl())->length()) {
        slot.setValue(int16ArrayGetByIndex(thisObject, propertyName));
        return true;
    }

    return thisObject->methodTable()->getOwnPropertySlot(thisObject, exec, Identifier::from(exec, propertyName), slot);
}

void JSInt16Array::put(JSCell* cell, ExecState* exec, const Identifier& propertyName, JSValue value, PutPropertySlot& slot)
{
    JSInt16Array* thisObject = jsCast<JSInt16Array*>(cell);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok) {
        int16ArraySetterByIndex(thisObject, index, value.toNumber(exec));

        return;
    }

    Base::put(thisObject, exec, propertyName, value, slot);
}

void JSInt16Array::putByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, JSValue value, bool)
{
    JSInt16Array* thisObject = jsCast<JSInt16Array*>(cell);

    int16ArraySetterByIndex(thisObject, propertyName, value.toNumber(exec));

    return;
}

static JSValue int16ArrayLength(ExecState* exec, JSValue slotBase, const Identifier&)
{
    Int16Array* impl = static_cast<Int16Array*>(static_cast<JSInt16Array*>(asObject(slotBase))->impl());
    JSValue result = jsNumber(impl->length());
    return result;
}

}
