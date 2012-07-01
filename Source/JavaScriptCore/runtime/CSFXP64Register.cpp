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
#include "TypedArray.h"

#include "Error.h"
#include "Lookup.h"

namespace JSC {

static EncodedJSValue JSC_HOST_CALL cs_fxp64_ld(ExecState*);
static EncodedJSValue JSC_HOST_CALL cs_fxp64_st(ExecState*);

static EncodedJSValue JSC_HOST_CALL cs_fxp64_sadd(ExecState*);
static EncodedJSValue JSC_HOST_CALL cs_fxp64_uadd(ExecState*);

static EncodedJSValue JSC_HOST_CALL cs_fxp64_ssub(ExecState*);
static EncodedJSValue JSC_HOST_CALL cs_fxp64_usub(ExecState*);
}

#include "CSFXP64Register.lut.h"

namespace JSC {

ASSERT_HAS_TRIVIAL_DESTRUCTOR(FXP64Register);

const ClassInfo FXP64Register::s_info = { "FXP64Register", &Base::s_info, 0, ExecState::fxp64RegisterTable, CREATE_METHOD_TABLE(FXP64Register) };

/* Source for CSFXP64Register.lut.h
@begin fxp64RegisterTable
  ld            cs_fxp64_ld             Function 1
  st            cs_fxp64_st             Function 1
  sadd          cs_fxp64_sadd           Function 1
  uadd          cs_fxp64_uadd           Function 1
  ssub          cs_fxp64_ssub           Function 1
  usub          cs_fxp64_usub           Function 1
@end
*/

FXP64Register::FXP64Register(ExecState* exec, Structure* structure) : Base(exec->globalData(), structure), m_storage(0)
{
}

void FXP64Register::finishCreation(JSGlobalData& globalData, JSGlobalObject*)
{
    Base::finishCreation(globalData);

    ASSERT(inherits(&s_info));
}

bool FXP64Register::getOwnPropertySlotByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, PropertySlot& slot)
{
    return false;
}

bool FXP64Register::getOwnPropertySlot(JSCell* cell, ExecState* exec, PropertyName propertyName, PropertySlot &slot)
{
    return getStaticFunctionSlot<JSNonFinalObject>(exec, ExecState::fxp64RegisterTable(exec), jsCast<FXP64Register*>(cell), propertyName, slot);
}

bool FXP64Register::getOwnPropertyDescriptor(JSObject* object, ExecState* exec, PropertyName propertyName, PropertyDescriptor& descriptor)
{
    return getStaticFunctionDescriptor<JSNonFinalObject>(exec, ExecState::fxp64RegisterTable(exec), jsCast<FXP64Register*>(object), propertyName, descriptor);
}

EncodedJSValue JSC_HOST_CALL constructFXP64Register(ExecState* callFrame) {
    Structure* structure = FXP64Register::createStructure(callFrame->globalData(), callFrame->lexicalGlobalObject(), callFrame->lexicalGlobalObject()->objectPrototype());

    return JSValue::encode(FXP64Register::create(callFrame, callFrame->lexicalGlobalObject(), structure));
}

static inline FXP64Register::Union cs_load_receiver(ExecState* exec, EncodedJSValue* error) {
    JSValue reg = exec->thisValue(); FXP64Register::Union value;

    if (!reg.inherits(&FXP64Register::s_info)) {
        value.u = 0;
        *error = throwVMError(exec, createTypeError(exec, "Reciever is not a Hydrazine 64-bit Fixed Point Register."));
    } else {
        value.u = asFXP64Register(reg)->m_storage;
    }

    return value;
}

static inline FXP64Register::Union cs_load_argument(ExecState* exec, size_t argument, EncodedJSValue* error) {
    JSValue reg = exec->argument(argument); FXP64Register::Union value;

    if (!reg.inherits(&FXP64Register::s_info)) {
        value.u = 0;
        *error = throwVMError(exec, createTypeError(exec, "Argument is not a Hydrazine 64-bit Fixed Point Register."));
    } else {
        value.u = asFXP64Register(reg)->m_storage;
    }

    return value;
}

static inline FXP64Register::Op2 cs_load_2_operand(ExecState* exec, EncodedJSValue* error) {
    size_t arguments = exec->argumentCount(); FXP64Register::Op2 result;

    if (arguments == 0) {
        *error = throwVMError(exec, createTypeError(exec, "Not enough arguments, needs at least one."));
    } else if (arguments == 1) {
        result.a = cs_load_receiver(exec, error);
        result.b = cs_load_argument(exec, 0, error);
    } else {
        result.a = cs_load_argument(exec, 0, error);
        result.b = cs_load_argument(exec, 1, error);
    }

    return result;
}

static inline JSValue cs_store_receiver(ExecState* exec, FXP64Register::Union value, EncodedJSValue* error) {
    JSValue reg = exec->thisValue();

    if (!reg.inherits(&FXP64Register::s_info)) {
        *error = throwVMError(exec, createTypeError(exec, "Receiever is not a Hydrazine 64-bit Fixed Point Register."));
    } else {
        asFXP64Register(reg)->m_storage = value.u;
    }

    return reg;
}

static EncodedJSValue JSC_HOST_CALL cs_fxp64_ld(ExecState* exec)
{
    FXP64Register::Union value;

    JSValue array = exec->argument(0);
    JSObject* obj = asObject(array);

    if (array.inherits(&JSUint8Array::s_info) || array.inherits(&JSUint8ClampedArray::s_info) ) {
        value.u = jsCast<JSUint8Array*>(obj)->m_storage[0];
    } else if (array.inherits(&JSUint16Array::s_info)) {
        value.u = jsCast<JSUint16Array*>(obj)->m_storage[0];
    } else if (array.inherits(&JSUint32Array::s_info)) {
        value.u = jsCast<JSUint32Array*>(obj)->m_storage[0];
    } else if (array.inherits(&JSInt8Array::s_info)) {
        value.s = jsCast<JSInt8Array*>(obj)->m_storage[0];
    } else if (array.inherits(&JSInt16Array::s_info)) {
        value.s = jsCast<JSInt16Array*>(obj)->m_storage[0];
    } else if (array.inherits(&JSInt32Array::s_info)) {
        value.s = jsCast<JSInt32Array*>(obj)->m_storage[0];
    } else if (array.inherits(&JSFloat32Array::s_info)) {
        value.sp = jsCast<JSFloat32Array*>(obj)->m_storage[0];
    } else if (array.inherits(&JSFloat64Array::s_info)) {
        value.dp = jsCast<JSFloat64Array*>(obj)->m_storage[0];
    } else {
        return throwVMError(exec, createTypeError(exec, "First argument was not a typed array, it should be."));
    }

    EncodedJSValue error = NULL;
    JSValue result = cs_store_receiver(exec, value, &error);

    return error ? error : JSValue::encode(result);
}

static EncodedJSValue JSC_HOST_CALL cs_fxp64_st(ExecState* exec)
{
    EncodedJSValue error = NULL;
    FXP64Register::Union value = cs_load_receiver(exec, &error);

    if (error) { return error; }

    JSValue array = exec->argument(0);
    JSObject* obj = asObject(array);

    if (array.inherits(&JSUint8Array::s_info) || array.inherits(&JSUint8ClampedArray::s_info) ) {
        jsCast<JSUint8Array*>(obj)->m_storage[0] = (uint8_t)value.u;
    } else if (array.inherits(&JSUint16Array::s_info)) {
        jsCast<JSUint16Array*>(obj)->m_storage[0] = (uint16_t)value.u;
    } else if (array.inherits(&JSUint32Array::s_info)) {
        jsCast<JSUint32Array*>(obj)->m_storage[0] = (uint32_t)value.u;
    } else if (array.inherits(&JSInt8Array::s_info)) {
        jsCast<JSInt8Array*>(obj)->m_storage[0] = (int8_t)value.s;
    } else if (array.inherits(&JSInt16Array::s_info)) {
        jsCast<JSInt16Array*>(obj)->m_storage[0] = (uint16_t)value.s;
    } else if (array.inherits(&JSInt32Array::s_info)) {
        jsCast<JSInt32Array*>(obj)->m_storage[0] = (uint32_t)value.s;
    } else if (array.inherits(&JSFloat32Array::s_info)) {
        jsCast<JSFloat32Array*>(obj)->m_storage[0] = (uint8_t)value.sp;
    } else if (array.inherits(&JSFloat64Array::s_info)) {
        jsCast<JSFloat64Array*>(obj)->m_storage[0] = (uint8_t)value.dp;
    } else {
        return throwVMError(exec, createTypeError(exec, "First argument was not a typed array, it should be."));
    }

    return JSValue::encode(exec->thisValue());
}

#define CS_2_OP(exec, op_r, op_a, op_b) EncodedJSValue error = NULL; \
FXP64Register::Op2 __ops = cs_load_2_operand(exec, &error); \
if (error) { return error; } \
FXP64Register::Union op_r, op_a = __ops.a, op_b = __ops.b

#define CS_2_ED(exec, op_r) JSValue result = cs_store_receiver(exec, op_r, &error); \
return error ? error : JSValue::encode(result)

static EncodedJSValue JSC_HOST_CALL cs_fxp64_sadd(ExecState* exec)
{
    CS_2_OP(exec, r, a, b);

    r.s = a.s + b.s;

    CS_2_ED(exec, r);
}

static EncodedJSValue JSC_HOST_CALL cs_fxp64_uadd(ExecState* exec)
{
    CS_2_OP(exec, r, a, b);

    r.u = a.u + b.u;

    CS_2_ED(exec, r);
}

static EncodedJSValue JSC_HOST_CALL cs_fxp64_ssub(ExecState* exec)
{
    CS_2_OP(exec, r, a, b);

    r.s = a.s - b.s;

    CS_2_ED(exec, r);
}

static EncodedJSValue JSC_HOST_CALL cs_fxp64_usub(ExecState* exec)
{
    CS_2_OP(exec, r, a, b);

    r.u = a.u - b.u;

    CS_2_ED(exec, r);
}

}
