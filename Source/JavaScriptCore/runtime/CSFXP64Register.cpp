/*
 * Copyright (C) 2012 Jens Nockert (jens@ofmlabs.org)
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "CSFXP64Register.h"

#include "CSInstructions.h"

#include "TypedArray.h"
#include "JSArrayBufferViewPrototype.h"

#include "Error.h"
#include "Lookup.h"

namespace JSC {

static EncodedJSValue JSC_HOST_CALL cs_fxp64_ld(ExecState*);
static EncodedJSValue JSC_HOST_CALL cs_fxp64_st(ExecState*);

}

#include "CSFXP64Register.lut.h"

namespace JSC {

ASSERT_HAS_TRIVIAL_DESTRUCTOR(FXP64Register);

const ClassInfo FXP64Register::s_info = { "FXP64Register", &JSNonFinalObject::s_info, 0, ExecState::fxp64RegisterTable, CREATE_METHOD_TABLE(FXP64Register) };

/* Source for CSFXP64Register.lut.h
@begin fxp64RegisterTable
  ld            cs_fxp64_ld                                     Function 2
  st            cs_fxp64_st                                     Function 2
  sadd          Hydrazine::cs_fxp_sadd<FXP64Register>           Function 1
  uadd          Hydrazine::cs_fxp_uadd<FXP64Register>           Function 1
  ssub          Hydrazine::cs_fxp_ssub<FXP64Register>           Function 1
  usub          Hydrazine::cs_fxp_usub<FXP64Register>           Function 1
  sdiv          Hydrazine::cs_fxp_sdiv<FXP64Register>           Function 1
  udiv          Hydrazine::cs_fxp_udiv<FXP64Register>           Function 1
  smod          Hydrazine::cs_fxp_smod<FXP64Register>           Function 1
  umod          Hydrazine::cs_fxp_umod<FXP64Register>           Function 1
  smullo        Hydrazine::cs_fxp_smullo<FXP64Register>         Function 1
  umullo        Hydrazine::cs_fxp_umullo<FXP64Register>         Function 1
  and           Hydrazine::cs_fxp_and<FXP64Register>            Function 1
  or            Hydrazine::cs_fxp_or<FXP64Register>             Function 1
  xor           Hydrazine::cs_fxp_xor<FXP64Register>            Function 1
  andnot        Hydrazine::cs_fxp_andnot<FXP64Register>         Function 1
  rol           Hydrazine::cs_fxp_rol<FXP64Register>            Function 1
  ror           Hydrazine::cs_fxp_ror<FXP64Register>            Function 1
  popcnt        Hydrazine::cs_fxp_popcnt<FXP64Register>         Function 0
  insert        Hydrazine::cs_fxp_insert<FXP64Register>         Function 1
  extract       Hydrazine::cs_fxp_extract<FXP64Register>        Function 0
@end
*/

bool FXP64Register::getOwnPropertySlot(JSCell* cell, ExecState* exec, PropertyName propertyName, PropertySlot &slot)
{
    return getStaticFunctionSlot<Base>(exec, ExecState::fxp64RegisterTable(exec), jsCast<FXP64Register*>(cell), propertyName, slot);
}

bool FXP64Register::getOwnPropertySlotByIndex(JSCell*, ExecState*, unsigned, PropertySlot&)
{
    return false;
}

bool FXP64Register::getOwnPropertyDescriptor(JSObject* object, ExecState* exec, PropertyName propertyName, PropertyDescriptor& descriptor)
{
    return getStaticFunctionDescriptor<Base>(exec, ExecState::fxp64RegisterTable(exec), jsCast<FXP64Register*>(object), propertyName, descriptor);
}

static EncodedJSValue JSC_HOST_CALL cs_fxp64_ld(ExecState* exec)
{
    JSValue array, base;
    
    int32_t offs; size_t arguments = exec->argumentCount();
    
    if (arguments > 2) {
        array = exec->argument(0), base = exec->argument(1), offs = exec->argument(2).toInt32(exec);
    } else if (arguments == 2) {
        array = exec->argument(0), base = exec->argument(1), offs = 0;
    } else {
        return throwVMError(exec, createTypeError(exec, "You need to provide at least a typed array, and a base register."));
    }

    JSObject* obj = asObject(array);

    if (!base.inherits(&FXP64Register::s_info)) {
        return throwVMError(exec, createTypeError(exec, "Second argument is not a Hydrazine 32-bit Fixed Point Register."));
    }

    uint32_t offset = (uint32_t)(asFXP64Register(base)->m_storage.s + offs);

    Hydrazine::R64 value;

    if (array.inherits(&JSUint8Array::s_info) || array.inherits(&JSUint8ClampedArray::s_info) ) {
        JSUint8Array* view = jsCast<JSUint8Array*>(obj);

        value.u = offset < view->m_storageLength ? view->m_storage[offset] : 0;
    } else if (array.inherits(&JSUint16Array::s_info)) {
        JSUint16Array* view = jsCast<JSUint16Array*>(obj);

        value.u = offset < view->m_storageLength ? view->m_storage[offset] : 0;
    } else if (array.inherits(&JSUint32Array::s_info)) {
        JSUint32Array* view = jsCast<JSUint32Array*>(obj);

        value.u = offset < view->m_storageLength ? view->m_storage[offset] : 0;
    } else if (array.inherits(&JSInt8Array::s_info)) {
        JSInt8Array* view = jsCast<JSInt8Array*>(obj);

        value.s = offset < view->m_storageLength ? view->m_storage[offset] : 0;
    } else if (array.inherits(&JSInt16Array::s_info)) {
        JSInt16Array* view = jsCast<JSInt16Array*>(obj);

        value.s = offset < view->m_storageLength ? view->m_storage[offset] : 0;
    } else if (array.inherits(&JSInt32Array::s_info)) {
        JSInt32Array* view = jsCast<JSInt32Array*>(obj);

        value.s = offset < view->m_storageLength ? view->m_storage[offset] : 0;
    } else if (array.inherits(&JSFloat64Array::s_info)) {
        JSFloat64Array* view = jsCast<JSFloat64Array*>(obj);

        value.dp = offset < view->m_storageLength ? view->m_storage[offset] : (0.0 / 0.0);
    } else {
        return throwVMError(exec, createTypeError(exec, "First argument was of an unsupported type."));
    }

    EncodedJSValue error = NULL;
    JSValue result = Hydrazine::storeReceiver<FXP64Register>(exec, value, &error);

    return error ? error : JSValue::encode(result);
}

static EncodedJSValue JSC_HOST_CALL cs_fxp64_st(ExecState* exec)
{
    JSValue array, base;
    
    int32_t offs; size_t arguments = exec->argumentCount();
    
    if (arguments > 2) {
        array = exec->argument(0), base = exec->argument(1), offs = exec->argument(2).toInt32(exec);
    } else if (arguments == 2) {
        array = exec->argument(0), base = exec->argument(1), offs = 0;
    } else {
        return throwVMError(exec, createTypeError(exec, "You need to provide at least a typed array, and a base register."));
    }

    JSObject* obj = asObject(array);

    if (!base.inherits(&FXP64Register::s_info)) {
        return throwVMError(exec, createTypeError(exec, "Second argument is not a Hydrazine 32-bit Fixed Point Register."));
    }

    uint32_t offset = (uint32_t)(asFXP64Register(base)->m_storage.s + offs);
    
    EncodedJSValue error = NULL;
    Hydrazine::R64 value = Hydrazine::loadReceiver<FXP64Register>(exec, &error);

    if (error) { return error; }

    if (array.inherits(&JSUint8Array::s_info) || array.inherits(&JSUint8ClampedArray::s_info) ) {
        JSUint8Array* view = jsCast<JSUint8Array*>(obj);

        if (offset < view->m_storageLength) { view->m_storage[offset] = value.u; }
    } else if (array.inherits(&JSUint16Array::s_info)) {
        JSUint16Array* view = jsCast<JSUint16Array*>(obj);

        if (offset < view->m_storageLength) { view->m_storage[offset] = value.u; }
    } else if (array.inherits(&JSUint32Array::s_info)) {
        JSUint32Array* view = jsCast<JSUint32Array*>(obj);

        if (offset < view->m_storageLength) { view->m_storage[offset] = value.u; }
    } else if (array.inherits(&JSInt8Array::s_info)) {
        JSInt8Array* view = jsCast<JSInt8Array*>(obj);

        if (offset < view->m_storageLength) { view->m_storage[offset] = value.s; }
    } else if (array.inherits(&JSInt16Array::s_info)) {
        JSInt16Array* view = jsCast<JSInt16Array*>(obj);

        if (offset < view->m_storageLength) { view->m_storage[offset] = value.s; }
    } else if (array.inherits(&JSInt32Array::s_info)) {
        JSInt32Array* view = jsCast<JSInt32Array*>(obj);

        if (offset < view->m_storageLength) { view->m_storage[offset] = value.s; }
    } else if (array.inherits(&JSFloat64Array::s_info)) {
          JSFloat32Array* view = jsCast<JSFloat32Array*>(obj);

          if (offset < view->m_storageLength) { view->m_storage[offset] = value.dp; }
    } else {
        return throwVMError(exec, createTypeError(exec, "First argument was of an unsupported type."));
    }

    return JSValue::encode(exec->thisValue());
}

}
