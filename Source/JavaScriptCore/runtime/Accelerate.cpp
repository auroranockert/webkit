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

#include "Accelerate.h"

#include "JSGlobalObject.h"
#include "JSCTypedArrayStubs.h"

#include <runtime/Error.h>

#define cs_unary_intrinsic(name, intrinsic) EncodedJSValue JSC_HOST_CALL name(ExecState* exec) { \
    JSValue thisValue = exec->hostThisValue(); \
\
    if (!thisValue.inherits(&Accelerate::s_info)) { \
        return throwVMTypeError(exec); \
    } \
\
    size_t argsCount = exec->argumentCount(); \
\
    unsigned n = exec->argument(0).toUInt32(exec); \
\
    if (n > 32 || __builtin_popcount(n) != 1) { \
        return throwVMError(exec, createTypeError(exec, "n is larger than 32 or not a power-of-two")); \
    } \
\
    unsigned ri = exec->argument(2).toUInt32(exec); \
    unsigned ai = exec->argument(4).toUInt32(exec); \
\
    JSValue rv(exec->argument(1)); \
    JSValue av(exec->argument(3)); \
\
    if (argsCount < 5) { \
        return throwVMError(exec, createTypeError(exec, "Not enough arguments, needs 5")); \
    } \
\
    if (rv.isNull() || av.isNull()) { \
        return throwVMTypeError(exec); \
    } \
\
    JSObject* ro = asObject(rv); \
    JSObject* ao = asObject(av); \
\
    if (ro->inherits(&JSUint8Array::s_info) && ao->inherits(&JSUint8Array::s_info)) { \
        return intrinsic<uint8_t, Uint8Array>(exec, n, toUint8Array(rv), ri, toUint8Array(av), ai); \
    } \
\
    if (ro->inherits(&JSUint16Array::s_info) && ao->inherits(&JSUint16Array::s_info)) { \
        return intrinsic<uint16_t, Uint16Array>(exec, n, toUint16Array(rv), ri, toUint16Array(av), ai); \
    } \
\
    if (ro->inherits(&JSUint32Array::s_info) && ao->inherits(&JSUint32Array::s_info)) { \
        return intrinsic<uint32_t, Uint32Array>(exec, n, toUint32Array(rv), ri, toUint32Array(av), ai); \
    } \
\
    if (ro->inherits(&JSInt8Array::s_info) && ao->inherits(&JSInt8Array::s_info)) { \
        return intrinsic<int8_t, Int8Array>(exec, n, toInt8Array(rv), ri, toInt8Array(av), ai); \
    } \
\
    if (ro->inherits(&JSInt16Array::s_info) && ao->inherits(&JSInt16Array::s_info)) { \
        return intrinsic<int16_t, Int16Array>(exec, n, toInt16Array(rv), ri, toInt16Array(av), ai); \
    } \
\
    if (ro->inherits(&JSInt32Array::s_info) && ao->inherits(&JSInt32Array::s_info)) { \
        return intrinsic<int32_t, Int32Array>(exec, n, toInt32Array(rv), ri, toInt32Array(av), ai); \
    } \
\
    if (ro->inherits(&JSFloat32Array::s_info) && ao->inherits(&JSFloat32Array::s_info)) { \
        return intrinsic<float, Float32Array>(exec, n, toFloat32Array(rv), ri, toFloat32Array(av), ai); \
    } \
\
    if (ro->inherits(&JSFloat64Array::s_info) && ao->inherits(&JSFloat64Array::s_info)) { \
        return intrinsic<double, Float64Array>(exec, n, toFloat64Array(rv), ri, toFloat64Array(av), ai); \
    } \
\
    return throwVMError(exec, createTypeError(exec, "views are not of matching type")); \
}

#define cs_binary_intrinsic(name, intrinsic) EncodedJSValue JSC_HOST_CALL name(ExecState* exec) { \
    JSValue thisValue = exec->hostThisValue(); \
\
    if (!thisValue.inherits(&Accelerate::s_info)) { \
        return throwVMTypeError(exec); \
    } \
\
    size_t argsCount = exec->argumentCount(); \
\
    unsigned n = exec->argument(0).toUInt32(exec); \
\
    if (n > 32 || __builtin_popcount(n) != 1) { \
        return throwVMError(exec, createTypeError(exec, "n is larger than 32 or not a power-of-two")); \
    } \
\
    unsigned ri = exec->argument(2).toUInt32(exec); \
    unsigned ai = exec->argument(4).toUInt32(exec); \
    unsigned bi = exec->argument(6).toUInt32(exec); \
\
    JSValue rv(exec->argument(1)); \
    JSValue av(exec->argument(3)); \
    JSValue bv(exec->argument(5)); \
\
    if (argsCount < 7) { \
        return throwVMError(exec, createTypeError(exec, "Not enough arguments, needs 7")); \
    } \
\
    if (rv.isNull() || av.isNull() || bv.isNull()) { \
        return throwVMTypeError(exec); \
    } \
\
    JSObject* ro = asObject(rv); \
    JSObject* ao = asObject(av); \
    JSObject* bo = asObject(av); \
\
    if (ro->inherits(&JSUint8Array::s_info) && ao->inherits(&JSUint8Array::s_info) && bo->inherits(&JSUint8Array::s_info)) { \
        return intrinsic<uint8_t, Uint8Array>(exec, n, toUint8Array(rv), ri, toUint8Array(av), ai, toUint8Array(bv), bi); \
    } \
\
    if (ro->inherits(&JSUint16Array::s_info) && ao->inherits(&JSUint16Array::s_info) && bo->inherits(&JSUint16Array::s_info)) { \
        return intrinsic<uint16_t, Uint16Array>(exec, n, toUint16Array(rv), ri, toUint16Array(av), ai, toUint16Array(bv), bi); \
    } \
\
    if (ro->inherits(&JSUint32Array::s_info) && ao->inherits(&JSUint32Array::s_info) && bo->inherits(&JSUint32Array::s_info)) { \
        return intrinsic<uint32_t, Uint32Array>(exec, n, toUint32Array(rv), ri, toUint32Array(av), ai, toUint32Array(bv), bi); \
    } \
\
    if (ro->inherits(&JSInt8Array::s_info) && ao->inherits(&JSInt8Array::s_info) && bo->inherits(&JSInt8Array::s_info)) { \
        return intrinsic<int8_t, Int8Array>(exec, n, toInt8Array(rv), ri, toInt8Array(av), ai, toInt8Array(bv), bi); \
    } \
\
    if (ro->inherits(&JSInt16Array::s_info) && ao->inherits(&JSInt16Array::s_info) && bo->inherits(&JSInt16Array::s_info)) { \
        return intrinsic<int16_t, Int16Array>(exec, n, toInt16Array(rv), ri, toInt16Array(av), ai, toInt16Array(bv), bi); \
    } \
\
    if (ro->inherits(&JSInt32Array::s_info) && ao->inherits(&JSInt32Array::s_info) && bo->inherits(&JSInt32Array::s_info)) { \
        return intrinsic<int32_t, Int32Array>(exec, n, toInt32Array(rv), ri, toInt32Array(av), ai, toInt32Array(bv), bi); \
    } \
\
    if (ro->inherits(&JSFloat32Array::s_info) && ao->inherits(&JSFloat32Array::s_info) && bo->inherits(&JSFloat32Array::s_info)) { \
        return intrinsic<float, Float32Array>(exec, n, toFloat32Array(rv), ri, toFloat32Array(av), ai, toFloat32Array(bv), bi); \
    } \
\
    if (ro->inherits(&JSFloat64Array::s_info) && ao->inherits(&JSFloat64Array::s_info) && bo->inherits(&JSFloat64Array::s_info)) { \
        return intrinsic<double, Float64Array>(exec, n, toFloat64Array(rv), ri, toFloat64Array(av), ai, toFloat64Array(bv), bi); \
    } \
\
    return throwVMError(exec, createTypeError(exec, "views are not of matching type")); \
}

static inline unsigned cs_check_alignment(unsigned n, unsigned elementsize, unsigned offset) {
    unsigned alignment = elementsize * n;
    
    if (alignment > 16) { alignment = 16; }
    
    return offset % alignment == 0; // TODO: Really check alignment
}

namespace JSC {

static inline Uint8Array* toUint8Array(JSC::JSValue value)
{
    return value.inherits(&JSUint8Array::s_info) ? static_cast<Uint8Array*>(static_cast<JSUint8Array*>(asObject(value))->impl()) : 0;
}

static inline Uint16Array* toUint16Array(JSC::JSValue value)
{
    return value.inherits(&JSUint16Array::s_info) ? static_cast<Uint16Array*>(static_cast<JSUint16Array*>(asObject(value))->impl()) : 0;
}

static inline Uint32Array* toUint32Array(JSC::JSValue value)
{
    return value.inherits(&JSUint32Array::s_info) ? static_cast<Uint32Array*>(static_cast<JSUint32Array*>(asObject(value))->impl()) : 0;
}

static inline Int8Array* toInt8Array(JSC::JSValue value)
{
    return value.inherits(&JSInt8Array::s_info) ? static_cast<Int8Array*>(static_cast<JSInt8Array*>(asObject(value))->impl()) : 0;
}

static inline Int16Array* toInt16Array(JSC::JSValue value)
{
    return value.inherits(&JSInt16Array::s_info) ? static_cast<Int16Array*>(static_cast<JSInt16Array*>(asObject(value))->impl()) : 0;
}

static inline Int32Array* toInt32Array(JSC::JSValue value)
{
    return value.inherits(&JSInt32Array::s_info) ? static_cast<Int32Array*>(static_cast<JSInt32Array*>(asObject(value))->impl()) : 0;
}

static inline Float32Array* toFloat32Array(JSC::JSValue value)
{
    return value.inherits(&JSFloat32Array::s_info) ? static_cast<Float32Array*>(static_cast<JSFloat32Array*>(asObject(value))->impl()) : 0;
}

static inline Float64Array* toFloat64Array(JSC::JSValue value)
{
    return value.inherits(&JSFloat64Array::s_info) ? static_cast<Float64Array*>(static_cast<JSFloat64Array*>(asObject(value))->impl()) : 0;
}

}

namespace JSC {

ASSERT_CLASS_FITS_IN_CELL(Accelerate);

JSC::EncodedJSValue JSC_HOST_CALL accelerateAbs(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL accelerateAdd(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL accelerateDiv(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL accelerateMax(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL accelerateMin(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL accelerateMul(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL accelerateSqrt(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL accelerateSub(JSC::ExecState*);

}

#include "Accelerate.lut.h"

namespace JSC {

ASSERT_HAS_TRIVIAL_DESTRUCTOR(Accelerate);

const ClassInfo Accelerate::s_info = { "Accelerate", &JSNonFinalObject::s_info, &accelerateTable, 0, CREATE_METHOD_TABLE(Accelerate) };

/* Source for Accelerate.lut.h
@begin accelerateTable
  abs           accelerateAbs               DontEnum|Function 5
  add           accelerateAdd               DontEnum|Function 7
  div           accelerateDiv               DontEnum|Function 7
  max           accelerateMax               DontEnum|Function 7
  min           accelerateMin               DontEnum|Function 7
  mul           accelerateMul               DontEnum|Function 7
  sqrt          accelerateSqrt              DontEnum|Function 5
  sub           accelerateSub               DontEnum|Function 7
@end
*/

Accelerate::Accelerate(JSGlobalObject* globalObject, Structure* structure)
    : JSNonFinalObject(globalObject->globalData(), structure)
{
}

void Accelerate::finishCreation(ExecState* exec, JSGlobalObject* globalObject)
{
    Base::finishCreation(globalObject->globalData());
    ASSERT(inherits(&s_info));
}

bool Accelerate::getOwnPropertySlot(JSCell* cell, ExecState* exec, const Identifier& propertyName, PropertySlot &slot)
{
    return getStaticFunctionSlot<JSObject>(exec, ExecState::accelerateTable(exec), jsCast<Accelerate*>(cell), propertyName, slot);
}

bool Accelerate::getOwnPropertyDescriptor(JSObject* object, ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticFunctionDescriptor<JSObject>(exec, ExecState::accelerateTable(exec), jsCast<Accelerate*>(object), propertyName, descriptor);
}

template <class Scalar, class View>
EncodedJSValue cs_abs(ExecState* exec, unsigned n, View* r, unsigned ri, View* a, unsigned ai)
{
    if (!cs_check_alignment(n, sizeof(Scalar), ri) || !cs_check_alignment(n, sizeof(Scalar), ai)) {
        return throwVMError(exec, createTypeError(exec, "view is unaligned"));
    }
    
    if (r->length() < ri + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    if (a->length() < ai + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    
    Scalar* rPtr = r->data();
    Scalar* aPtr = a->data();
    
    for (unsigned i = 0; i < n; i++) {
        rPtr[ri++] = abs(aPtr[ai++]);
    }
    
    return JSValue::encode(jsUndefined());
}

template <class Scalar, class View>
EncodedJSValue cs_add(ExecState* exec, unsigned n, View* r, unsigned ri, View* a, unsigned ai, View* b, unsigned bi)
{
    if (!cs_check_alignment(n, sizeof(Scalar), ri) || !cs_check_alignment(n, sizeof(Scalar), ai) || !cs_check_alignment(n, sizeof(Scalar), bi)) {
        return throwVMError(exec, createTypeError(exec, "view is unaligned"));
    }
    
    if (r->length() < ri + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    if (a->length() < ai + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    if (b->length() < bi + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    
    Scalar* rPtr = r->data();
    Scalar* aPtr = a->data();
    Scalar* bPtr = b->data();
    
    for (unsigned i = 0; i < n; i++) {
        rPtr[ri++] = aPtr[ai++] + bPtr[bi++];
    }
    
    return JSValue::encode(jsUndefined());
}


template <class Scalar, class View>
EncodedJSValue cs_div(ExecState* exec, unsigned n, View* r, unsigned ri, View* a, unsigned ai, View* b, unsigned bi)
{
    if (!cs_check_alignment(n, sizeof(Scalar), ri) || !cs_check_alignment(n, sizeof(Scalar), ai) || !cs_check_alignment(n, sizeof(Scalar), bi)) {
        return throwVMError(exec, createTypeError(exec, "view is unaligned"));
    }
    
    if (r->length() < ri + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    if (a->length() < ai + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    if (b->length() < bi + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    
    Scalar* rPtr = r->data();
    Scalar* aPtr = a->data();
    Scalar* bPtr = b->data();
    
    for (unsigned i = 0; i < n; i++) {
        rPtr[ri++] = aPtr[ai++] / bPtr[bi++];
    }
    
    return JSValue::encode(jsUndefined());
}

template <class Scalar, class View>
EncodedJSValue cs_max(ExecState* exec, unsigned n, View* r, unsigned ri, View* a, unsigned ai, View* b, unsigned bi)
{
    if (!cs_check_alignment(n, sizeof(Scalar), ri) || !cs_check_alignment(n, sizeof(Scalar), ai) || !cs_check_alignment(n, sizeof(Scalar), bi)) {
        return throwVMError(exec, createTypeError(exec, "view is unaligned"));
    }
    
    if (r->length() < ri + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    if (a->length() < ai + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    if (b->length() < bi + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    
    Scalar* rPtr = r->data();
    Scalar* aPtr = a->data();
    Scalar* bPtr = b->data();
    
    for (unsigned i = 0; i < n; i++) {
        rPtr[ri++] = std::max(aPtr[ai++], bPtr[bi++]);
    }
    
    return JSValue::encode(jsUndefined());
}

template <class Scalar, class View>
EncodedJSValue cs_min(ExecState* exec, unsigned n, View* r, unsigned ri, View* a, unsigned ai, View* b, unsigned bi)
{
    if (!cs_check_alignment(n, sizeof(Scalar), ri) || !cs_check_alignment(n, sizeof(Scalar), ai) || !cs_check_alignment(n, sizeof(Scalar), bi)) {
        return throwVMError(exec, createTypeError(exec, "view is unaligned"));
    }
    
    if (r->length() < ri + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    if (a->length() < ai + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    if (b->length() < bi + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    
    Scalar* rPtr = r->data();
    Scalar* aPtr = a->data();
    Scalar* bPtr = b->data();
    
    for (unsigned i = 0; i < n; i++) {
        rPtr[ri++] = std::min(aPtr[ai++], bPtr[bi++]);
    }
    
    return JSValue::encode(jsUndefined());
}

template <class Scalar, class View>
EncodedJSValue cs_mul(ExecState* exec, unsigned n, View* r, unsigned ri, View* a, unsigned ai, View* b, unsigned bi)
{
    if (!cs_check_alignment(n, sizeof(Scalar), ri) || !cs_check_alignment(n, sizeof(Scalar), ai) || !cs_check_alignment(n, sizeof(Scalar), bi)) {
        return throwVMError(exec, createTypeError(exec, "view is unaligned"));
    }
    
    if (r->length() < ri + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    if (a->length() < ai + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    if (b->length() < bi + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    
    Scalar* rPtr = r->data();
    Scalar* aPtr = a->data();
    Scalar* bPtr = b->data();
    
    for (unsigned i = 0; i < n; i++) {
        rPtr[ri++] = aPtr[ai++] * bPtr[bi++];
    }
    
    return JSValue::encode(jsUndefined());
}

template <class Scalar, class View>
EncodedJSValue cs_sqrt(ExecState* exec, unsigned n, View* r, unsigned ri, View* a, unsigned ai)
{
    if (!cs_check_alignment(n, sizeof(Scalar), ri) || !cs_check_alignment(n, sizeof(Scalar), ai)) {
        return throwVMError(exec, createTypeError(exec, "view is unaligned"));
    }
    
    if (r->length() < ri + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    if (a->length() < ai + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    
    Scalar* rPtr = r->data();
    Scalar* aPtr = a->data();
    
    for (unsigned i = 0; i < n; i++) {
        rPtr[ri++] = sqrt(aPtr[ai++]);
    }
    
    return JSValue::encode(jsUndefined());
}

template <class Scalar, class View>
EncodedJSValue cs_sub(ExecState* exec, unsigned n, View* r, unsigned ri, View* a, unsigned ai, View* b, unsigned bi)
{
    if (!cs_check_alignment(n, sizeof(Scalar), ri) || !cs_check_alignment(n, sizeof(Scalar), ai) || !cs_check_alignment(n, sizeof(Scalar), bi)) {
        return throwVMError(exec, createTypeError(exec, "view is unaligned"));
    }
    
    if (r->length() < ri + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    if (a->length() < ai + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    if (b->length() < bi + n) { return throwVMError(exec, createTypeError(exec, "index is out of range")); }
    
    Scalar* rPtr = r->data();
    Scalar* aPtr = a->data();
    Scalar* bPtr = b->data();
    
    for (unsigned i = 0; i < n; i++) {
        rPtr[ri++] = aPtr[ai++] - bPtr[bi++];
    }
    
    return JSValue::encode(jsUndefined());
}

cs_unary_intrinsic(accelerateAbs, cs_abs);
cs_binary_intrinsic(accelerateAdd, cs_add);
cs_binary_intrinsic(accelerateDiv, cs_div);
cs_binary_intrinsic(accelerateMax, cs_max);
cs_binary_intrinsic(accelerateMin, cs_min);
cs_binary_intrinsic(accelerateMul, cs_mul);
cs_unary_intrinsic(accelerateSqrt, cs_sqrt);
cs_binary_intrinsic(accelerateSub, cs_sub);
}
