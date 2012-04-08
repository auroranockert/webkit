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
#include "JSUint16Array.h"

#include "Error.h"

namespace JSC {

static JSValue uint16ArrayLength(ExecState* exec, JSValue slotBase, const Identifier&);

}

#include "JSUint16Array.lut.h"

namespace JSC {

ASSERT_HAS_TRIVIAL_DESTRUCTOR(JSUint16Array);

const ClassInfo JSUint16Array::s_info = { "Uint16Array", &Base::s_info, 0, ExecState::uint16ArrayTable, CREATE_METHOD_TABLE(JSUint16Array) };

ASSERT_CLASS_FITS_IN_CELL(JSUint16Array);

/* Source for JSUint16Array.lut.h
@begin uint16ArrayTable
  length       uint16ArrayLength         DontDelete|ReadOnly 0
@end
*/

JSUint16Array::JSUint16Array(JSGlobalData& globalData, Structure* structure, PassRefPtr<Uint16Array> impl)
      : JSArrayBufferView(globalData, structure, impl)
{
}

JSUint16Array::~JSUint16Array()
{
  releaseImplIfNotNull();
}

static void uint16ArraySetterByIndex(JSUint16Array* current, unsigned index, double value)
{
    current->impl()->set(index, static_cast<uint16_t>(value));
}

static JSValue uint16ArrayGetByIndex(JSUint16Array* current, unsigned index)
{
    double result = static_cast<Uint16Array*>(current->impl())->item(index);

    if (isnan(result)) {
        return jsNaN();
	}

    return JSValue(result);
}

bool JSUint16Array::getOwnPropertySlot(JSCell* cell, ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    JSUint16Array* thisObject = jsCast<JSUint16Array*>(cell);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok && index < static_cast<Uint16Array*>(thisObject->impl())->length()) {
        slot.setValue(uint16ArrayGetByIndex(thisObject, index));
        return true;
    }

    return getStaticValueSlot<JSUint16Array, Base>(exec, ExecState::uint16ArrayTable(exec), thisObject, propertyName, slot);
}

bool JSUint16Array::getOwnPropertyDescriptor(JSObject* object, ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    JSUint16Array* thisObject = jsCast<JSUint16Array*>(object);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok && index < static_cast<Uint16Array*>(thisObject->impl())->length()) {
        descriptor.setDescriptor(uint16ArrayGetByIndex(thisObject, index), DontDelete);
        return true;
    }

    return getStaticValueDescriptor<JSUint16Array, Base>(exec, ExecState::uint16ArrayTable(exec), thisObject, propertyName, descriptor);
}

bool JSUint16Array::getOwnPropertySlotByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, PropertySlot& slot)
{
    JSUint16Array* thisObject = jsCast<JSUint16Array*>(cell);

    if (propertyName < static_cast<Uint16Array*>(thisObject->impl())->length()) {
        slot.setValue(uint16ArrayGetByIndex(thisObject, propertyName));
        return true;
    }

    return thisObject->methodTable()->getOwnPropertySlot(thisObject, exec, Identifier::from(exec, propertyName), slot);
}

void JSUint16Array::put(JSCell* cell, ExecState* exec, const Identifier& propertyName, JSValue value, PutPropertySlot& slot)
{
    JSUint16Array* thisObject = jsCast<JSUint16Array*>(cell);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok) {
        uint16ArraySetterByIndex(thisObject, index, value.toNumber(exec));

        return;
    }

    Base::put(thisObject, exec, propertyName, value, slot);
}

void JSUint16Array::putByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, JSValue value, bool)
{
    JSUint16Array* thisObject = jsCast<JSUint16Array*>(cell);

    uint16ArraySetterByIndex(thisObject, propertyName, value.toNumber(exec));

    return;
}

static JSValue uint16ArrayLength(ExecState* exec, JSValue slotBase, const Identifier&)
{
    Uint16Array* impl = static_cast<Uint16Array*>(static_cast<JSUint16Array*>(asObject(slotBase))->impl());
    JSValue result = jsNumber(impl->length());
    return result;
}

}
