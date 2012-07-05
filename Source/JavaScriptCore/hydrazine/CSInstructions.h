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

#ifndef CSFXPInstructions_h
#define CSFXPInstructions_h

#include "CSMath.h"

#define CS_1_OP_S(T, exec, op_r, op_a) EncodedJSValue error = NULL; \
typename T::Op1 __ops = Hydrazine::loadOneOperandSigned<T>(exec, &error); \
if (error) { return error; } \
typename T::Type op_r, op_a = __ops.a

#define CS_1_OP_U(T, exec, op_r, op_a) EncodedJSValue error = NULL; \
typename T::Op1 __ops = Hydrazine::loadOneOperandUnsigned<T>(exec, &error); \
if (error) { return error; } \
typename T::Type op_r, op_a = __ops.a

#define CS_2_OP_S(T, exec, op_r, op_a, op_b) EncodedJSValue error = NULL; \
typename T::Op2 __ops = Hydrazine::loadTwoOperandSigned<T>(exec, &error); \
if (error) { return error; } \
typename T::Type op_r, op_a = __ops.a, op_b = __ops.b

#define CS_2_OP_U(T, exec, op_r, op_a, op_b) EncodedJSValue error = NULL; \
typename T::Op2 __ops = Hydrazine::loadTwoOperandUnsigned<T>(exec, &error); \
if (error) { return error; } \
typename T::Type op_r, op_a = __ops.a, op_b = __ops.b

#define CS_ED(T, exec, op_r) JSValue result = Hydrazine::storeReceiver<T>(exec, op_r, &error); \
return error ? error : JSValue::encode(result)

namespace JSC {
namespace Hydrazine {

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_mov(ExecState* exec)
{
    CS_1_OP_S(T, exec, r, a); r.u = a.u; CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_sadd(ExecState* exec)
{
    CS_2_OP_S(T, exec, r, a, b); r = Hydrazine::sadd(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_uadd(ExecState* exec)
{
    CS_2_OP_U(T, exec, r, a, b); r = Hydrazine::uadd(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_ssub(ExecState* exec)
{
    CS_2_OP_S(T, exec, r, a, b); r = Hydrazine::ssub(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_usub(ExecState* exec)
{
    CS_2_OP_U(T, exec, r, a, b); r = Hydrazine::usub(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_sdiv(ExecState* exec)
{
    CS_2_OP_S(T, exec, r, a, b); r = Hydrazine::sdiv(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_udiv(ExecState* exec)
{
    CS_2_OP_U(T, exec, r, a, b); r = Hydrazine::udiv(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_smod(ExecState* exec)
{
    CS_2_OP_S(T, exec, r, a, b); r = Hydrazine::smod(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_umod(ExecState* exec)
{
    CS_2_OP_U(T, exec, r, a, b); r = Hydrazine::umod(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_smullo(ExecState* exec)
{
    CS_2_OP_S(T, exec, r, a, b); r = Hydrazine::smullo(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_umullo(ExecState* exec)
{
    CS_2_OP_U(T, exec, r, a, b); r = Hydrazine::umullo(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_smulhi(ExecState* exec)
{
    CS_2_OP_S(T, exec, r, a, b); r = Hydrazine::smulhi(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_umulhi(ExecState* exec)
{
    CS_2_OP_U(T, exec, r, a, b); r = Hydrazine::umulhi(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_not(ExecState* exec)
{
    CS_1_OP_S(T, exec, r, a); r = Hydrazine::inot(a); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_and(ExecState* exec)
{
    CS_2_OP_S(T, exec, r, a, b); r = Hydrazine::iand(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_or(ExecState* exec)
{
    CS_2_OP_S(T, exec, r, a, b); r = Hydrazine::ior(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_xor(ExecState* exec)
{
    CS_2_OP_S(T, exec, r, a, b); r = Hydrazine::ixor(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_nand(ExecState* exec)
{
    CS_2_OP_S(T, exec, r, a, b); r = Hydrazine::inand(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_nor(ExecState* exec)
{
    CS_2_OP_S(T, exec, r, a, b); r = Hydrazine::inor(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_andnot(ExecState* exec)
{
    CS_2_OP_S(T, exec, r, a, b); r = Hydrazine::iandnot(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_ornot(ExecState* exec)
{
    CS_2_OP_S(T, exec, r, a, b); r = Hydrazine::iornot(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_sal(ExecState* exec)
{
    CS_2_OP_U(T, exec, r, a, b); r = Hydrazine::sal(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_sar(ExecState* exec)
{
    CS_2_OP_U(T, exec, r, a, b); r = Hydrazine::sar(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_sll(ExecState* exec)
{
    CS_2_OP_U(T, exec, r, a, b); r = Hydrazine::sll(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_slr(ExecState* exec)
{
    CS_2_OP_U(T, exec, r, a, b); r = Hydrazine::slr(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_rol(ExecState* exec)
{
    CS_2_OP_U(T, exec, r, a, b); r = Hydrazine::rol(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_ror(ExecState* exec)
{
    CS_2_OP_U(T, exec, r, a, b); r = Hydrazine::ror(a, b); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_popcnt(ExecState* exec)
{
    CS_1_OP_S(T, exec, r, a); r = Hydrazine::popcnt(a); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_clz(ExecState* exec)
{
    CS_1_OP_S(T, exec, r, a); r = Hydrazine::clz(a); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_ctz(ExecState* exec)
{
    CS_1_OP_S(T, exec, r, a); r = Hydrazine::ctz(a); CS_ED(T, exec, r);
}

template <typename T>
static EncodedJSValue JSC_HOST_CALL cs_fxp_extract(ExecState* exec)
{
    EncodedJSValue error = NULL;

    typename T::Type value = Hydrazine::loadReceiver<T>(exec, &error);

    return error ? error : JSValue::encode(JSValue((int32_t)value.s));
}

}
}

#undef CS_1_OP_S
#undef CS_2_OP_S
#undef CS_ED

#endif // CSFXPInstructions_h
