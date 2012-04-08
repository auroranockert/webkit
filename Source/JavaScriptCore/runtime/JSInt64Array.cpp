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
#include "JSInt64Array.h"

#include "Error.h"

namespace JSC {

static JSValue int64ArrayLength(ExecState* exec, JSValue slotBase, const Identifier&);

}

#include "JSInt64Array.lut.h"

namespace JSC {

ASSERT_HAS_TRIVIAL_DESTRUCTOR(JSInt64Array);

const ClassInfo JSInt64Array::s_info = { "Int64Array", &Base::s_info, 0, ExecState::int64ArrayTable, CREATE_METHOD_TABLE(JSInt64Array) };

ASSERT_CLASS_FITS_IN_CELL(JSInt64Array);

/* Source for JSInt64Array.lut.h
@begin int64ArrayTable
  length       int64ArrayLength         DontDelete|ReadOnly 0
@end
*/

JSInt64Array::JSInt64Array(JSGlobalData& globalData, Structure* structure, PassRefPtr<Int64Array> impl)
    : JSArrayBufferView(globalData, structure, impl)
{
}

JSInt64Array::~JSInt64Array()
{
  releaseImplIfNotNull();
}

static void int64ArraySetterByIndex(JSInt64Array* current, unsigned index, double value)
{
    current->impl()->set(index, static_cast<int64_t>(value));
}

static JSValue int64ArrayGetByIndex(JSInt64Array* current, unsigned index)
{
    double result = static_cast<Int64Array*>(current->impl())->item(index);

    if (isnan(result)) {
        return jsNaN();
	}

    return JSValue(result);
}

bool JSInt64Array::getOwnPropertySlot(JSCell* cell, ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    JSInt64Array* thisObject = jsCast<JSInt64Array*>(cell);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok && index < static_cast<Int64Array*>(thisObject->impl())->length()) {
        slot.setValue(int64ArrayGetByIndex(thisObject, index));
        return true;
    }

    return getStaticValueSlot<JSInt64Array, Base>(exec, ExecState::int64ArrayTable(exec), thisObject, propertyName, slot);
}

bool JSInt64Array::getOwnPropertyDescriptor(JSObject* object, ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    JSInt64Array* thisObject = jsCast<JSInt64Array*>(object);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok && index < static_cast<Int64Array*>(thisObject->impl())->length()) {
        descriptor.setDescriptor(int64ArrayGetByIndex(thisObject, index), DontDelete);
        return true;
    }

    return getStaticValueDescriptor<JSInt64Array, Base>(exec, ExecState::int64ArrayTable(exec), thisObject, propertyName, descriptor);
}

bool JSInt64Array::getOwnPropertySlotByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, PropertySlot& slot)
{
    JSInt64Array* thisObject = jsCast<JSInt64Array*>(cell);

    if (propertyName < static_cast<Int64Array*>(thisObject->impl())->length()) {
        slot.setValue(int64ArrayGetByIndex(thisObject, propertyName));
        return true;
    }

    return thisObject->methodTable()->getOwnPropertySlot(thisObject, exec, Identifier::from(exec, propertyName), slot);
}

void JSInt64Array::put(JSCell* cell, ExecState* exec, const Identifier& propertyName, JSValue value, PutPropertySlot& slot)
{
    JSInt64Array* thisObject = jsCast<JSInt64Array*>(cell);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok) {
        int64ArraySetterByIndex(thisObject, index, value.toNumber(exec));

        return;
    }

    Base::put(thisObject, exec, propertyName, value, slot);
}

void JSInt64Array::putByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, JSValue value, bool)
{
    JSInt64Array* thisObject = jsCast<JSInt64Array*>(cell);

    int64ArraySetterByIndex(thisObject, propertyName, value.toNumber(exec));

    return;
}

static JSValue int64ArrayLength(ExecState* exec, JSValue slotBase, const Identifier&)
{
    Int64Array* impl = static_cast<Int64Array*>(static_cast<JSInt64Array*>(asObject(slotBase))->impl());
    JSValue result = jsNumber(impl->length());
    return result;
}

}
