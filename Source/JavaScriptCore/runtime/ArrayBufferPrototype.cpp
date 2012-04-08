/*
 *  Copyright (C) 2012 Jens Nockert (jens@nockert.se)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "ArrayBufferPrototype.h"

#include "Error.h"

namespace JSC {

ASSERT_CLASS_FITS_IN_CELL(ArrayBufferPrototype);

static EncodedJSValue JSC_HOST_CALL arrayBufferProtoFuncSlice(ExecState* exec);

}

#include "ArrayBufferPrototype.lut.h"

namespace JSC {

const ClassInfo ArrayBufferPrototype::s_info = { "ArrayBuffer", &JSArrayBuffer::s_info, 0, ExecState::arrayBufferPrototypeTable, CREATE_METHOD_TABLE(ArrayBufferPrototype)};

/* Source for ArrayBufferPrototype.lut.h
@begin arrayBufferPrototypeTable
  slice       arrayBufferProtoFuncSlice       DontDelete|Function 2
@end
*/

ArrayBufferPrototype::ArrayBufferPrototype(JSGlobalObject* globalObject, Structure* structure)
    : JSArrayBuffer(globalObject->globalData(), structure, NULL)
{
}

void ArrayBufferPrototype::finishCreation(JSGlobalObject* globalObject)
{
    Base::finishCreation(globalObject->globalData());
    ASSERT(inherits(&s_info));
}

bool ArrayBufferPrototype::getOwnPropertySlot(JSCell* cell, ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticFunctionSlot<JSArrayBuffer>(exec, ExecState::arrayBufferPrototypeTable(exec), jsCast<ArrayBufferPrototype*>(cell), propertyName, slot);
}

bool ArrayBufferPrototype::getOwnPropertyDescriptor(JSObject* object, ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticFunctionDescriptor<JSArrayBuffer>(exec, ExecState::arrayBufferPrototypeTable(exec), jsCast<ArrayBufferPrototype*>(object), propertyName, descriptor);
}

// Functions

static EncodedJSValue JSC_HOST_CALL arrayBufferProtoFuncSlice(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();

    if (!thisValue.inherits(&JSArrayBuffer::s_info)) {
        return throwVMTypeError(exec);
    }

    JSArrayBuffer* thisObj = asArrayBuffer(thisValue);

    ArrayBuffer* impl = static_cast<ArrayBuffer*>(thisObj->impl());
    
    size_t argsCount = exec->argumentCount();
    
    if (argsCount < 1) {
        return throwVMError(exec, createTypeError(exec, "Not enough arguments"));
    }

    int begin = exec->argument(0).toInt32(exec);

    if (exec->hadException()) {
        return JSValue::encode(jsUndefined());
    }

    JSValue result;

    if (argsCount < 2) {
        result = toJS(exec, thisObj->globalObject(), impl->slice(begin).get());
    } else {
        int end = exec->argument(1).toInt32(exec);

        if (exec->hadException()) {
            return JSValue::encode(jsUndefined());
        }

        result = toJS(exec, thisObj->globalObject(), impl->slice(begin, end).get());
    }

    return JSValue::encode(result);
}

}
