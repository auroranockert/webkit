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
#include "JSUint32Array.h"

#include "Error.h"

namespace JSC {

static JSValue uint32ArrayLength(ExecState* exec, JSValue slotBase, const Identifier&);

}

#include "JSUint32Array.lut.h"

namespace JSC {

ASSERT_HAS_TRIVIAL_DESTRUCTOR(JSUint32Array);

const ClassInfo JSUint32Array::s_info = { "Uint32Array", &Base::s_info, 0, ExecState::uint32ArrayTable, CREATE_METHOD_TABLE(JSUint32Array) };

ASSERT_CLASS_FITS_IN_CELL(JSUint32Array);

/* Source for JSUint32Array.lut.h
@begin uint32ArrayTable
  length       uint32ArrayLength         DontDelete|ReadOnly 0
@end
*/

JSUint32Array::JSUint32Array(JSGlobalData& globalData, Structure* structure, PassRefPtr<Uint32Array> impl)
      : JSArrayBufferView(globalData, structure, impl)
{
}

JSUint32Array::~JSUint32Array()
{
  releaseImplIfNotNull();
}

static void uint32ArraySetterByIndex(JSUint32Array* current, unsigned index, double value)
{
    current->impl()->set(index, static_cast<uint32_t>(value));
}

static JSValue uint32ArrayGetByIndex(JSUint32Array* current, unsigned index)
{
    double result = static_cast<Uint32Array*>(current->impl())->item(index);

    if (isnan(result)) {
        return jsNaN();
	}

    return JSValue(result);
}

bool JSUint32Array::getOwnPropertySlot(JSCell* cell, ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    JSUint32Array* thisObject = jsCast<JSUint32Array*>(cell);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok && index < static_cast<Uint32Array*>(thisObject->impl())->length()) {
        slot.setValue(uint32ArrayGetByIndex(thisObject, index));
        return true;
    }

    return getStaticValueSlot<JSUint32Array, Base>(exec, ExecState::uint32ArrayTable(exec), thisObject, propertyName, slot);
}

bool JSUint32Array::getOwnPropertyDescriptor(JSObject* object, ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    JSUint32Array* thisObject = jsCast<JSUint32Array*>(object);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok && index < static_cast<Uint32Array*>(thisObject->impl())->length()) {
        descriptor.setDescriptor(uint32ArrayGetByIndex(thisObject, index), DontDelete);
        return true;
    }

    return getStaticValueDescriptor<JSUint32Array, Base>(exec, ExecState::uint32ArrayTable(exec), thisObject, propertyName, descriptor);
}

bool JSUint32Array::getOwnPropertySlotByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, PropertySlot& slot)
{
    JSUint32Array* thisObject = jsCast<JSUint32Array*>(cell);

    if (propertyName < static_cast<Uint32Array*>(thisObject->impl())->length()) {
        slot.setValue(uint32ArrayGetByIndex(thisObject, propertyName));
        return true;
    }

    return thisObject->methodTable()->getOwnPropertySlot(thisObject, exec, Identifier::from(exec, propertyName), slot);
}

void JSUint32Array::put(JSCell* cell, ExecState* exec, const Identifier& propertyName, JSValue value, PutPropertySlot& slot)
{
    JSUint32Array* thisObject = jsCast<JSUint32Array*>(cell);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok) {
        uint32ArraySetterByIndex(thisObject, index, value.toNumber(exec));

        return;
    }

    Base::put(thisObject, exec, propertyName, value, slot);
}

void JSUint32Array::putByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, JSValue value, bool)
{
    JSUint32Array* thisObject = jsCast<JSUint32Array*>(cell);

    uint32ArraySetterByIndex(thisObject, propertyName, value.toNumber(exec));

    return;
}

static JSValue uint32ArrayLength(ExecState* exec, JSValue slotBase, const Identifier&)
{
    Uint32Array* impl = static_cast<Uint32Array*>(static_cast<JSUint32Array*>(asObject(slotBase))->impl());
    JSValue result = jsNumber(impl->length());
    return result;
}

}
