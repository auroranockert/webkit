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
#include "JSFloat64Array.h"

#include "Error.h"

namespace JSC {

static JSValue float64ArrayLength(ExecState* exec, JSValue slotBase, const Identifier&);

}

#include "JSFloat64Array.lut.h"

namespace JSC {

ASSERT_HAS_TRIVIAL_DESTRUCTOR(JSFloat64Array);

const ClassInfo JSFloat64Array::s_info = { "Float64Array", &Base::s_info, 0, ExecState::float64ArrayTable, CREATE_METHOD_TABLE(JSFloat64Array) };

ASSERT_CLASS_FITS_IN_CELL(JSFloat64Array);

/* Source for JSFloat64Array.lut.h
@begin float64ArrayTable
  length       float64ArrayLength         DontDelete|ReadOnly 0
@end
*/

JSFloat64Array::JSFloat64Array(JSGlobalData& globalData, Structure* structure, PassRefPtr<Float64Array> impl)
      : JSArrayBufferView(globalData, structure, impl)
{
}

JSFloat64Array::~JSFloat64Array()
{
  releaseImplIfNotNull();
}

static void float64ArraySetterByIndex(JSFloat64Array* current, unsigned index, double value)
{
    current->impl()->set(index, static_cast<double>(value));
}

static JSValue float64ArrayGetByIndex(JSFloat64Array* current, unsigned index)
{
    double result = static_cast<Float64Array*>(current->impl())->item(index);

    if (isnan(result)) {
        return jsNaN();
	}

    return JSValue(result);
}

bool JSFloat64Array::getOwnPropertySlot(JSCell* cell, ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    JSFloat64Array* thisObject = jsCast<JSFloat64Array*>(cell);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok && index < static_cast<Float64Array*>(thisObject->impl())->length()) {
        slot.setValue(float64ArrayGetByIndex(thisObject, index));
        return true;
    }

    return getStaticValueSlot<JSFloat64Array, Base>(exec, ExecState::float64ArrayTable(exec), thisObject, propertyName, slot);
}

bool JSFloat64Array::getOwnPropertyDescriptor(JSObject* object, ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    JSFloat64Array* thisObject = jsCast<JSFloat64Array*>(object);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok && index < static_cast<Float64Array*>(thisObject->impl())->length()) {
        descriptor.setDescriptor(float64ArrayGetByIndex(thisObject, index), DontDelete);
        return true;
    }

    return getStaticValueDescriptor<JSFloat64Array, Base>(exec, ExecState::float64ArrayTable(exec), thisObject, propertyName, descriptor);
}

bool JSFloat64Array::getOwnPropertySlotByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, PropertySlot& slot)
{
    JSFloat64Array* thisObject = jsCast<JSFloat64Array*>(cell);

    if (propertyName < static_cast<Float64Array*>(thisObject->impl())->length()) {
        slot.setValue(float64ArrayGetByIndex(thisObject, propertyName));
        return true;
    }

    return thisObject->methodTable()->getOwnPropertySlot(thisObject, exec, Identifier::from(exec, propertyName), slot);
}

void JSFloat64Array::put(JSCell* cell, ExecState* exec, const Identifier& propertyName, JSValue value, PutPropertySlot& slot)
{
    JSFloat64Array* thisObject = jsCast<JSFloat64Array*>(cell);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok) {
        float64ArraySetterByIndex(thisObject, index, value.toNumber(exec));

        return;
    }

    Base::put(thisObject, exec, propertyName, value, slot);
}

void JSFloat64Array::putByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, JSValue value, bool)
{
    JSFloat64Array* thisObject = jsCast<JSFloat64Array*>(cell);

    float64ArraySetterByIndex(thisObject, propertyName, value.toNumber(exec));

    return;
}

static JSValue float64ArrayLength(ExecState* exec, JSValue slotBase, const Identifier&)
{
    Float64Array* impl = static_cast<Float64Array*>(static_cast<JSFloat64Array*>(asObject(slotBase))->impl());
    JSValue result = jsNumber(impl->length());
    return result;
}

}
