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
#include "CSFXP32Register.h"

#include "CSInstructions.h"

#include "TypedArray.h"
#include "JSArrayBufferViewPrototype.h"

#include "Error.h"
#include "Lookup.h"

namespace JSC {

static EncodedJSValue JSC_HOST_CALL cs_fxp32_ld(ExecState*);
static EncodedJSValue JSC_HOST_CALL cs_fxp32_st(ExecState*);

}

#include "CSFXP32Register.lut.h"

namespace JSC {

ASSERT_HAS_TRIVIAL_DESTRUCTOR(FXP32Register);

const ClassInfo FXP32Register::s_info = { "FXP32Register", &JSNonFinalObject::s_info, 0, ExecState::fxp32RegisterTable, CREATE_METHOD_TABLE(FXP32Register) };

/* Source for CSFXP32Register.lut.h
@begin fxp32RegisterTable
  ld            cs_fxp32_ld                                     Function 2
  st            cs_fxp32_st                                     Function 2
  mov           Hydrazine::cs_fxp_mov<FXP32Register>            Function 1
  sadd          Hydrazine::cs_fxp_sadd<FXP32Register>           Function 1
  uadd          Hydrazine::cs_fxp_uadd<FXP32Register>           Function 1
  ssub          Hydrazine::cs_fxp_ssub<FXP32Register>           Function 1
  usub          Hydrazine::cs_fxp_usub<FXP32Register>           Function 1
  sdiv          Hydrazine::cs_fxp_sdiv<FXP32Register>           Function 1
  udiv          Hydrazine::cs_fxp_udiv<FXP32Register>           Function 1
  smod          Hydrazine::cs_fxp_smod<FXP32Register>           Function 1
  umod          Hydrazine::cs_fxp_umod<FXP32Register>           Function 1
  smullo        Hydrazine::cs_fxp_smullo<FXP32Register>         Function 1
  umullo        Hydrazine::cs_fxp_umullo<FXP32Register>         Function 1
  not           Hydrazine::cs_fxp_not<FXP32Register>            Function 0
  and           Hydrazine::cs_fxp_and<FXP32Register>            Function 1
  or            Hydrazine::cs_fxp_or<FXP32Register>             Function 1
  xor           Hydrazine::cs_fxp_xor<FXP32Register>            Function 1
  nand          Hydrazine::cs_fxp_nand<FXP32Register>           Function 1
  nor           Hydrazine::cs_fxp_nor<FXP32Register>            Function 1
  andnot        Hydrazine::cs_fxp_andnot<FXP32Register>         Function 1
  ornot         Hydrazine::cs_fxp_ornot<FXP32Register>          Function 1
  sal           Hydrazine::cs_fxp_sal<FXP32Register>            Function 1
  sar           Hydrazine::cs_fxp_sar<FXP32Register>            Function 1
  sll           Hydrazine::cs_fxp_sll<FXP32Register>            Function 1
  slr           Hydrazine::cs_fxp_slr<FXP32Register>            Function 1
  rol           Hydrazine::cs_fxp_rol<FXP32Register>            Function 1
  ror           Hydrazine::cs_fxp_ror<FXP32Register>            Function 1
  popcnt        Hydrazine::cs_fxp_popcnt<FXP32Register>         Function 0
  clz           Hydrazine::cs_fxp_clz<FXP32Register>            Function 0
  ctz           Hydrazine::cs_fxp_ctz<FXP32Register>            Function 0
  extract       Hydrazine::cs_fxp_extract<FXP32Register>        Function 0
@end
*/

bool FXP32Register::getOwnPropertySlot(JSCell* cell, ExecState* exec, PropertyName propertyName, PropertySlot &slot)
{
    return getStaticFunctionSlot<Base>(exec, ExecState::fxp32RegisterTable(exec), jsCast<FXP32Register*>(cell), propertyName, slot);
}

bool FXP32Register::getOwnPropertySlotByIndex(JSCell*, ExecState*, unsigned, PropertySlot&)
{
    return false;
}

bool FXP32Register::getOwnPropertyDescriptor(JSObject* object, ExecState* exec, PropertyName propertyName, PropertyDescriptor& descriptor)
{
    return getStaticFunctionDescriptor<Base>(exec, ExecState::fxp32RegisterTable(exec), jsCast<FXP32Register*>(object), propertyName, descriptor);
}

static EncodedJSValue JSC_HOST_CALL cs_fxp32_ld(ExecState* exec)
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

    if (!base.inherits(&FXP32Register::s_info)) {
        return throwVMError(exec, createTypeError(exec, "Second argument is not a Hydrazine 32-bit Fixed Point Register."));
    }

    uint32_t offset = (uint32_t)(asFXP32Register(base)->m_storage.s + offs);

    Hydrazine::R32 value;

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
    } else if (array.inherits(&JSFloat32Array::s_info)) {
        JSFloat32Array* view = jsCast<JSFloat32Array*>(obj);

        value.sp = offset < view->m_storageLength ? view->m_storage[offset] : (0.0f / 0.0f);
    } else {
        return throwVMError(exec, createTypeError(exec, "First argument was of an unsupported type."));
    }

    EncodedJSValue error = NULL;
    JSValue result = Hydrazine::storeReceiver<FXP32Register>(exec, value, &error);

    return error ? error : JSValue::encode(result);
}

static EncodedJSValue JSC_HOST_CALL cs_fxp32_st(ExecState* exec)
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

    if (!base.inherits(&FXP32Register::s_info)) {
        return throwVMError(exec, createTypeError(exec, "Second argument is not a Hydrazine 32-bit Fixed Point Register."));
    }

    uint32_t offset = (uint32_t)(asFXP32Register(base)->m_storage.s + offs);
    
    EncodedJSValue error = NULL;
    Hydrazine::R32 value = Hydrazine::loadReceiver<FXP32Register>(exec, &error);

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
    } else if (array.inherits(&JSFloat32Array::s_info)) {
        JSFloat32Array* view = jsCast<JSFloat32Array*>(obj);

        if (offset < view->m_storageLength) { view->m_storage[offset] = value.sp; }
    } else {
        return throwVMError(exec, createTypeError(exec, "First argument was of an unsupported type."));
    }

    return JSValue::encode(exec->thisValue());
}

}
