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
#include "JSFloat32Array.h"

#include "Error.h"

namespace JSC {

static JSValue float32ArrayLength(ExecState* exec, JSValue slotBase, const Identifier&);

}

#include "JSFloat32Array.lut.h"

namespace JSC {

ASSERT_HAS_TRIVIAL_DESTRUCTOR(JSFloat32Array);

const ClassInfo JSFloat32Array::s_info = { "Float32Array", &Base::s_info, 0, ExecState::float32ArrayTable, CREATE_METHOD_TABLE(JSFloat32Array) };

ASSERT_CLASS_FITS_IN_CELL(JSFloat32Array);

/* Source for JSFloat32Array.lut.h
@begin float32ArrayTable
  length       float32ArrayLength         DontDelete|ReadOnly 0
@end
*/

JSFloat32Array::JSFloat32Array(JSGlobalData& globalData, Structure* structure, PassRefPtr<Float32Array> impl)
      : JSArrayBufferView(globalData, structure, impl)
{
}

JSFloat32Array::~JSFloat32Array()
{
  releaseImplIfNotNull();
}

static void float32ArraySetterByIndex(JSFloat32Array* current, unsigned index, double value)
{
    current->impl()->set(index, static_cast<float>(value));
}

static JSValue float32ArrayGetByIndex(JSFloat32Array* current, unsigned index)
{
    double result = static_cast<Float32Array*>(current->impl())->item(index);

    if (isnan(result)) {
        return jsNaN();
	}

    return JSValue(result);
}

bool JSFloat32Array::getOwnPropertySlot(JSCell* cell, ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    JSFloat32Array* thisObject = jsCast<JSFloat32Array*>(cell);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok && index < static_cast<Float32Array*>(thisObject->impl())->length()) {
        slot.setValue(float32ArrayGetByIndex(thisObject, index));
        return true;
    }

    return getStaticValueSlot<JSFloat32Array, Base>(exec, ExecState::float32ArrayTable(exec), thisObject, propertyName, slot);
}

bool JSFloat32Array::getOwnPropertyDescriptor(JSObject* object, ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    JSFloat32Array* thisObject = jsCast<JSFloat32Array*>(object);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok && index < static_cast<Float32Array*>(thisObject->impl())->length()) {
        descriptor.setDescriptor(float32ArrayGetByIndex(thisObject, index), DontDelete);
        return true;
    }

    return getStaticValueDescriptor<JSFloat32Array, Base>(exec, ExecState::float32ArrayTable(exec), thisObject, propertyName, descriptor);
}

bool JSFloat32Array::getOwnPropertySlotByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, PropertySlot& slot)
{
    JSFloat32Array* thisObject = jsCast<JSFloat32Array*>(cell);

    if (propertyName < static_cast<Float32Array*>(thisObject->impl())->length()) {
        slot.setValue(float32ArrayGetByIndex(thisObject, propertyName));
        return true;
    }

    return thisObject->methodTable()->getOwnPropertySlot(thisObject, exec, Identifier::from(exec, propertyName), slot);
}

void JSFloat32Array::put(JSCell* cell, ExecState* exec, const Identifier& propertyName, JSValue value, PutPropertySlot& slot)
{
    JSFloat32Array* thisObject = jsCast<JSFloat32Array*>(cell);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok) {
        float32ArraySetterByIndex(thisObject, index, value.toNumber(exec));

        return;
    }

    Base::put(thisObject, exec, propertyName, value, slot);
}

void JSFloat32Array::putByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, JSValue value, bool)
{
    JSFloat32Array* thisObject = jsCast<JSFloat32Array*>(cell);

    float32ArraySetterByIndex(thisObject, propertyName, value.toNumber(exec));

    return;
}

static JSValue float32ArrayLength(ExecState* exec, JSValue slotBase, const Identifier&)
{
    Float32Array* impl = static_cast<Float32Array*>(static_cast<JSFloat32Array*>(asObject(slotBase))->impl());
    JSValue result = jsNumber(impl->length());
    return result;
}

}
