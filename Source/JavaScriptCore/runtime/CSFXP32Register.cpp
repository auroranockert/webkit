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
#include "TypedArray.h"
#include "JSArrayBufferViewPrototype.h"

#include "Error.h"
#include "Lookup.h"

namespace JSC {

static EncodedJSValue JSC_HOST_CALL cs_fxp32_ld(ExecState*);
static EncodedJSValue JSC_HOST_CALL cs_fxp32_st(ExecState*);

static EncodedJSValue JSC_HOST_CALL cs_fxp32_sadd(ExecState*);
static EncodedJSValue JSC_HOST_CALL cs_fxp32_saddi(ExecState*);

static EncodedJSValue JSC_HOST_CALL cs_fxp32_uadd(ExecState*);
static EncodedJSValue JSC_HOST_CALL cs_fxp32_uaddi(ExecState*);

static EncodedJSValue JSC_HOST_CALL cs_fxp32_ssub(ExecState*);
static EncodedJSValue JSC_HOST_CALL cs_fxp32_ssubi(ExecState*);

static EncodedJSValue JSC_HOST_CALL cs_fxp32_usub(ExecState*);
static EncodedJSValue JSC_HOST_CALL cs_fxp32_usubi(ExecState*);

static EncodedJSValue JSC_HOST_CALL cs_fxp32_popcnt(ExecState*);

static EncodedJSValue JSC_HOST_CALL cs_fxp32_insert(ExecState*);
static EncodedJSValue JSC_HOST_CALL cs_fxp32_extract(ExecState*);

}

#include "CSFXP32Register.lut.h"

namespace JSC {

ASSERT_HAS_TRIVIAL_DESTRUCTOR(FXP32Register);

const ClassInfo FXP32Register::s_info = { "FXP32Register", &JSNonFinalObject::s_info, 0, ExecState::fxp32RegisterTable, CREATE_METHOD_TABLE(FXP32Register) };

/* Source for CSFXP32Register.lut.h
@begin fxp32RegisterTable
  ld            cs_fxp32_ld             Function 2
  st            cs_fxp32_st             Function 2
  sadd          cs_fxp32_sadd           Function 1
  saddi         cs_fxp32_saddi          Function 1
  uadd          cs_fxp32_uadd           Function 1
  uaddi         cs_fxp32_uaddi          Function 1
  ssub          cs_fxp32_ssub           Function 1
  ssubi         cs_fxp32_ssubi          Function 1
  usub          cs_fxp32_usub           Function 1
  usubi         cs_fxp32_usubi          Function 1
  popcnt        cs_fxp32_popcnt         Function 0
  insert        cs_fxp32_insert         Function 1
  extract       cs_fxp32_extract        Function 0
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
    } else if (array.inherits(&JSFloat64Array::s_info)) {
        return throwVMError(exec, createTypeError(exec, "First argument was of an unsupported type."));
    }

    return JSValue::encode(exec->thisValue());
}

#define CS_1_OP(exec, op_r, op_a) EncodedJSValue error = NULL; \
FXP32Register::Op1 __ops = Hydrazine::loadOneOperand<FXP32Register>(exec, &error); \
if (error) { return error; } \
Hydrazine::R32 op_r, op_a = __ops.a

#define CS_2_OP(exec, op_r, op_a, op_b) EncodedJSValue error = NULL; \
FXP32Register::Op2 __ops = Hydrazine::loadTwoOperand<FXP32Register>(exec, &error); \
if (error) { return error; } \
Hydrazine::R32 op_r, op_a = __ops.a, op_b = __ops.b

#define CS_2_OP_IMM(exec, op_r, op_a, op_b) EncodedJSValue error = NULL; \
FXP32Register::Op2 __ops = Hydrazine::loadTwoOperandWithSignedImmediate<FXP32Register>(exec, &error); \
if (error) { return error; } \
Hydrazine::R32 op_r, op_a = __ops.a, op_b = __ops.b

#define CS_ED(exec, op_r) JSValue result = Hydrazine::storeReceiver<FXP32Register>(exec, op_r, &error); \
return error ? error : JSValue::encode(result)

static EncodedJSValue JSC_HOST_CALL cs_fxp32_sadd(ExecState* exec)
{
    CS_2_OP(exec, r, a, b); r = Hydrazine::sadd(a, b); CS_ED(exec, r);
}

static EncodedJSValue JSC_HOST_CALL cs_fxp32_saddi(ExecState* exec)
{
    CS_2_OP_IMM(exec, r, a, b); r = Hydrazine::sadd(a, b); CS_ED(exec, r);
}

static EncodedJSValue JSC_HOST_CALL cs_fxp32_uadd(ExecState* exec)
{
    CS_2_OP(exec, r, a, b); r = Hydrazine::uadd(a, b); CS_ED(exec, r);
}

static EncodedJSValue JSC_HOST_CALL cs_fxp32_uaddi(ExecState* exec)
{
    CS_2_OP_IMM(exec, r, a, b); r = Hydrazine::uadd(a, b); CS_ED(exec, r);
}

static EncodedJSValue JSC_HOST_CALL cs_fxp32_ssub(ExecState* exec)
{
    CS_2_OP(exec, r, a, b); r = Hydrazine::ssub(a, b); CS_ED(exec, r);
}

static EncodedJSValue JSC_HOST_CALL cs_fxp32_ssubi(ExecState* exec)
{
    CS_2_OP_IMM(exec, r, a, b); r = Hydrazine::ssub(a, b); CS_ED(exec, r);
}

static EncodedJSValue JSC_HOST_CALL cs_fxp32_usub(ExecState* exec)
{
    CS_2_OP(exec, r, a, b); r = Hydrazine::usub(a, b); CS_ED(exec, r);
}

static EncodedJSValue JSC_HOST_CALL cs_fxp32_usubi(ExecState* exec)
{
    CS_2_OP_IMM(exec, r, a, b); r = Hydrazine::usub(a, b); CS_ED(exec, r);
}

static EncodedJSValue JSC_HOST_CALL cs_fxp32_popcnt(ExecState* exec)
{
    CS_1_OP(exec, r, a); r = Hydrazine::popcnt(a); CS_ED(exec, r);
}

static EncodedJSValue JSC_HOST_CALL cs_fxp32_insert(ExecState* exec)
{
    EncodedJSValue error = NULL;

    Hydrazine::R32 value = Hydrazine::loadSignedImmediate<FXP32Register>(exec, 0, &error);

    if (error) { return error; }

    JSValue result = Hydrazine::storeReceiver<FXP32Register>(exec, value, &error);

    return error ? error : result;
}

static EncodedJSValue JSC_HOST_CALL cs_fxp32_extract(ExecState* exec)
{
    EncodedJSValue error = NULL;

    Hydrazine::R32 value = Hydrazine::loadSignedImmediate<FXP32Register>(exec, 0, &error);

    return error ? error : JSValue::encode(JSValue(value.s));
}

}
