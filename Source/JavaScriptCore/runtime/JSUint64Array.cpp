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
#include "JSUint64Array.h"

#include "Error.h"

namespace JSC {

static JSValue uint64ArrayLength(ExecState* exec, JSValue slotBase, const Identifier&);

}

#include "JSUint64Array.lut.h"

namespace JSC {

ASSERT_HAS_TRIVIAL_DESTRUCTOR(JSUint64Array);

const ClassInfo JSUint64Array::s_info = { "Uint64Array", &Base::s_info, 0, ExecState::uint64ArrayTable, CREATE_METHOD_TABLE(JSUint64Array) };

ASSERT_CLASS_FITS_IN_CELL(JSUint64Array);

/* Source for JSUint64Array.lut.h
@begin uint64ArrayTable
  length       uint64ArrayLength         DontDelete|ReadOnly 0
@end
*/

JSUint64Array::JSUint64Array(JSGlobalData& globalData, Structure* structure, PassRefPtr<Uint64Array> impl)
      : JSArrayBufferView(globalData, structure, impl)
{
}

JSUint64Array::~JSUint64Array()
{
  releaseImplIfNotNull();
}


static void uint64ArraySetterByIndex(JSUint64Array* current, unsigned index, double value)
{
    current->impl()->set(index, static_cast<uint64_t>(value));
}

static JSValue uint64ArrayGetByIndex(JSUint64Array* current, unsigned index)
{
    double result = static_cast<Uint64Array*>(current->impl())->item(index);

    if (isnan(result)) {
        return jsNaN();
	}

    return JSValue(result);
}

bool JSUint64Array::getOwnPropertySlot(JSCell* cell, ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    JSUint64Array* thisObject = jsCast<JSUint64Array*>(cell);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok && index < static_cast<Uint64Array*>(thisObject->impl())->length()) {
        slot.setValue(uint64ArrayGetByIndex(thisObject, index));
        return true;
    }

    return getStaticValueSlot<JSUint64Array, Base>(exec, ExecState::uint64ArrayTable(exec), thisObject, propertyName, slot);
}

bool JSUint64Array::getOwnPropertyDescriptor(JSObject* object, ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    JSUint64Array* thisObject = jsCast<JSUint64Array*>(object);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok && index < static_cast<Uint64Array*>(thisObject->impl())->length()) {
        descriptor.setDescriptor(uint64ArrayGetByIndex(thisObject, index), DontDelete);
        return true;
    }

    return getStaticValueDescriptor<JSUint64Array, Base>(exec, ExecState::uint64ArrayTable(exec), thisObject, propertyName, descriptor);
}

bool JSUint64Array::getOwnPropertySlotByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, PropertySlot& slot)
{
    JSUint64Array* thisObject = jsCast<JSUint64Array*>(cell);

    if (propertyName < static_cast<Uint64Array*>(thisObject->impl())->length()) {
        slot.setValue(uint64ArrayGetByIndex(thisObject, propertyName));
        return true;
    }

    return thisObject->methodTable()->getOwnPropertySlot(thisObject, exec, Identifier::from(exec, propertyName), slot);
}

void JSUint64Array::put(JSCell* cell, ExecState* exec, const Identifier& propertyName, JSValue value, PutPropertySlot& slot)
{
    JSUint64Array* thisObject = jsCast<JSUint64Array*>(cell);

    bool ok; unsigned index = propertyName.toUInt32(ok);

    if (ok) {
        uint64ArraySetterByIndex(thisObject, index, value.toNumber(exec));

        return;
    }

    Base::put(thisObject, exec, propertyName, value, slot);
}

void JSUint64Array::putByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, JSValue value, bool)
{
    JSUint64Array* thisObject = jsCast<JSUint64Array*>(cell);

    uint64ArraySetterByIndex(thisObject, propertyName, value.toNumber(exec));

    return;
}

static JSValue uint64ArrayLength(ExecState* exec, JSValue slotBase, const Identifier&)
{
    Uint64Array* impl = static_cast<Uint64Array*>(static_cast<JSUint64Array*>(asObject(slotBase))->impl());
    JSValue result = jsNumber(impl->length());
    return result;
}

}
