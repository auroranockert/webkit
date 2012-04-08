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
#include "JSUint8Array.h"

#include "Error.h"

namespace JSC {

static JSValue uint8ArrayLength(ExecState* exec, JSValue slotBase, const Identifier&);

}

#include "JSUint8Array.lut.h"

namespace JSC {

ASSERT_HAS_TRIVIAL_DESTRUCTOR(JSUint8Array);

const ClassInfo JSUint8Array::s_info = { "Uint8Array", &Base::s_info, 0, ExecState::uint8ArrayTable, CREATE_METHOD_TABLE(JSUint8Array) };

ASSERT_CLASS_FITS_IN_CELL(JSUint8Array);

/* Source for JSUint8Array.lut.h
@begin uint8ArrayTable
  length       uint8ArrayLength         DontDelete|ReadOnly 0
@end
*/

JSUint8Array::JSUint8Array(JSGlobalData& globalData, Structure* structure, PassRefPtr<Uint8Array> impl)
      : JSArrayBufferView(globalData, structure, impl)
{
}

JSUint8Array::~JSUint8Array()
{
  releaseImplIfNotNull();
}

static void uint8ArraySetterByIndex(JSUint8Array* current, unsigned index, double value)
{
    current->impl()->set(index, static_cast<uint8_t>(value));
}

static JSValue uint8ArrayGetByIndex(JSUint8Array* current, unsigned index)
{
    double result = static_cast<Uint8Array*>(current->impl())->item(index);

    if (isnan(result)) {
        return jsNaN();
	}

    return JSValue(result);
}

bool JSUint8Array::getOwnPropertySlot(JSCell* cell, ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    JSUint8Array* thisObject = jsCast<JSUint8Array*>(cell);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok && index < static_cast<Uint8Array*>(thisObject->impl())->length()) {
        slot.setValue(uint8ArrayGetByIndex(thisObject, index));
        return true;
    }

    return getStaticValueSlot<JSUint8Array, Base>(exec, ExecState::uint8ArrayTable(exec), thisObject, propertyName, slot);
}

bool JSUint8Array::getOwnPropertyDescriptor(JSObject* object, ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    JSUint8Array* thisObject = jsCast<JSUint8Array*>(object);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok && index < static_cast<Uint8Array*>(thisObject->impl())->length()) {
        descriptor.setDescriptor(uint8ArrayGetByIndex(thisObject, index), DontDelete);
        return true;
    }

    return getStaticValueDescriptor<JSUint8Array, Base>(exec, ExecState::uint8ArrayTable(exec), thisObject, propertyName, descriptor);
}

bool JSUint8Array::getOwnPropertySlotByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, PropertySlot& slot)
{
    JSUint8Array* thisObject = jsCast<JSUint8Array*>(cell);

    if (propertyName < static_cast<Uint8Array*>(thisObject->impl())->length()) {
        slot.setValue(uint8ArrayGetByIndex(thisObject, propertyName));
        return true;
    }

    return thisObject->methodTable()->getOwnPropertySlot(thisObject, exec, Identifier::from(exec, propertyName), slot);
}

void JSUint8Array::put(JSCell* cell, ExecState* exec, const Identifier& propertyName, JSValue value, PutPropertySlot& slot)
{
    JSUint8Array* thisObject = jsCast<JSUint8Array*>(cell);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok) {
        uint8ArraySetterByIndex(thisObject, index, value.toNumber(exec));

        return;
    }

    Base::put(thisObject, exec, propertyName, value, slot);
}

void JSUint8Array::putByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, JSValue value, bool)
{
    JSUint8Array* thisObject = jsCast<JSUint8Array*>(cell);

    uint8ArraySetterByIndex(thisObject, propertyName, value.toNumber(exec));

    return;
}

static JSValue uint8ArrayLength(ExecState* exec, JSValue slotBase, const Identifier&)
{
    Uint8Array* impl = static_cast<Uint8Array*>(static_cast<JSUint8Array*>(asObject(slotBase))->impl());
    JSValue result = jsNumber(impl->length());
    return result;
}

}
