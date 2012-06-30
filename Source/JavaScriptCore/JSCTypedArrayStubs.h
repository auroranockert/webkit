/*
 * Copyright (C) 2012 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef JSCTypedArrayStubs_h
#define JSCTypedArrayStubs_h

#include "JSObject.h"
#include "ObjectPrototype.h"
#include <wtf/Float32Array.h>
#include <wtf/Float64Array.h>
#include <wtf/Forward.h>
#include <wtf/Int16Array.h>
#include <wtf/Int32Array.h>
#include <wtf/Int8Array.h>
#include <wtf/Uint16Array.h>
#include <wtf/Uint32Array.h>
#include <wtf/Uint8Array.h>
#include <wtf/Uint8ClampedArray.h>

namespace JSC {

template <typename T, typename I>
class TypedArray : public JSNonFinalObject {
public:
    typedef JSNonFinalObject Base;
    typedef TypedArray<T, I> Current;
    typedef I Implementation;
    
    static TypedArray<T, I>* create(Structure* structure, JSGlobalObject* globalObject, PassRefPtr<I> impl)
    {
        Current* ptr = new (NotNull, allocateCell<Current>(globalObject->globalData().heap)) Current(structure, globalObject, impl);
        ptr->finishCreation(globalObject->globalData());
        return ptr;
    }

    static bool getOwnPropertySlot(JSCell* cell, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
    {
        Current* thisObject = jsCast<Current*>(cell);
        ASSERT_GC_OBJECT_INHERITS(thisObject, &s_info);
        unsigned index = propertyName.asIndex();

        if (index < thisObject->m_storageLength) {
            ASSERT(index != PropertyName::NotAnIndex);
            slot.setValue(thisObject->getByIndex(exec, index));
            return true;
        }

        return Base::getOwnPropertySlot(cell, exec, propertyName, slot);
    }

    static bool getOwnPropertyDescriptor(JSObject* object, ExecState* exec, PropertyName propertyName, PropertyDescriptor& descriptor)
    {
        Current* thisObject = jsCast<Current*>(object);
        ASSERT_GC_OBJECT_INHERITS(thisObject, &s_info);
        unsigned index = propertyName.asIndex();

        if (index < thisObject->m_storageLength) {
            ASSERT(index != PropertyName::NotAnIndex);
            descriptor.setDescriptor(thisObject->getByIndex(exec, index), DontDelete);
            return true;
        }

        return Base::getOwnPropertyDescriptor(object, exec, propertyName, descriptor);
    }
    
    static bool getOwnPropertySlotByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, PropertySlot& slot)
    {
        Current* thisObject = jsCast<Current*>(cell);
        ASSERT_GC_OBJECT_INHERITS(thisObject, &s_info);

        if (propertyName < thisObject->m_storageLength) {
            slot.setValue(thisObject->getByIndex(exec, propertyName));
            return true;
        }

        return thisObject->methodTable()->getOwnPropertySlot(thisObject, exec, Identifier::from(exec, propertyName), slot);
    }
    
    static void put(JSCell* cell, ExecState* exec, PropertyName propertyName, JSValue value, PutPropertySlot& slot)
    {
        Current* thisObject = jsCast<Current*>(cell);
        ASSERT_GC_OBJECT_INHERITS(thisObject, &s_info);
        unsigned index = propertyName.asIndex();

        if (index != PropertyName::NotAnIndex) {
            thisObject->indexSetter(exec, index, value);
            return;
        }

        Base::put(thisObject, exec, propertyName, value, slot);
    }
    
    static void putByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, JSValue value, bool)
    {
        Current* thisObject = jsCast<Current*>(cell);
        ASSERT_GC_OBJECT_INHERITS(thisObject, &s_info);
        thisObject->indexSetter(exec, propertyName, value);
    }

    static const ClassInfo s_info; // TODO: Implement

    static Structure* createStructure(JSGlobalData& globalData, JSGlobalObject* globalObject, JSValue prototype)
    {
        return JSC::Structure::create(globalData, globalObject, prototype, TypeInfo(JSC::ObjectType, StructureFlags), &s_info);
    }

    static void getOwnPropertyNames(JSObject* object, ExecState* exec, PropertyNameArray& propertyNames, EnumerationMode mode)
    {
        Current* thisObject = jsCast<Current*>(object);
        ASSERT_GC_OBJECT_INHERITS(thisObject, &s_info);

        for (unsigned i = 0; i < thisObject->m_storageLength; ++i) {
            propertyNames.add(Identifier::from(exec, i));
        }

        Base::getOwnPropertyNames(thisObject, exec, propertyNames, mode);
    }

    static JSValue getConstructor(ExecState*, JSC::JSGlobalObject*);

    // TODO: static const JSC::TypedArrayType TypedArrayStorageType = JSC::TypedArray##name;
    uint32_t m_storageLength;

    T* m_storage;

    RefPtr<I> m_impl;

protected:
    TypedArray<T, I>(Structure* structure, JSGlobalObject* globalObject, PassRefPtr<I> impl)
        : Base(globalObject->globalData(), structure)
        , m_impl(impl)
    {    
    }

    void finishCreation(JSC::JSGlobalData& globalData)
    {
        Base::finishCreation(globalData);
        TypedArrayDescriptor descriptor(&Current::s_info, OBJECT_OFFSETOF(Current, m_storage), OBJECT_OFFSETOF(Current, m_storageLength));
        globalData.registerTypedArrayDescriptor(m_impl.get(), descriptor);
        m_storage = m_impl->data();
        m_storageLength = m_impl->length();
        putDirect(globalData, globalData.propertyNames->length, jsNumber(m_storageLength), DontDelete | ReadOnly | DontEnum);
        ASSERT(inherits(&s_info));
    }

    static const unsigned StructureFlags = JSC::OverridesGetPropertyNames | JSC::OverridesGetOwnPropertySlot | Base::StructureFlags;

    JSC::JSValue getByIndex(ExecState*, unsigned index)
    {
        ASSERT_GC_OBJECT_INHERITS(this, &s_info);
        T result = m_impl->item(index);

        if (isnan((double)result)) {
            return jsNaN();
        }

        return JSValue(result);
        
    }

    void indexSetter(ExecState* exec, unsigned index, JSValue value)
    {
        m_impl->set(index, value.toNumber(exec));
    }
};

template <typename T>
static EncodedJSValue JSC_HOST_CALL constructTypedArray(ExecState* callFrame) {
    if (callFrame->argumentCount() < 1) {
        return JSValue::encode(jsUndefined());
    }

    int32_t length = callFrame->argument(0).toInt32(callFrame);

    if (length < 0) {
        return JSValue::encode(jsUndefined());
    }

    Structure* structure = T::createStructure(callFrame->globalData(), callFrame->lexicalGlobalObject(), callFrame->lexicalGlobalObject()->objectPrototype());

    return JSValue::encode(T::create(structure, callFrame->lexicalGlobalObject(), T::Implementation::create(length)));
}

typedef TypedArray<uint8_t, Uint8Array> JSUint8Array;
typedef TypedArray<uint8_t, Uint8ClampedArray> JSUint8ClampedArray;
typedef TypedArray<uint16_t, Uint16Array> JSUint16Array;
typedef TypedArray<uint32_t, Uint32Array> JSUint32Array;

typedef TypedArray<int8_t, Int8Array> JSInt8Array;
typedef TypedArray<int16_t, Int16Array> JSInt16Array;
typedef TypedArray<int32_t, Int32Array> JSInt32Array;

typedef TypedArray<float, Float32Array> JSFloat32Array;
typedef TypedArray<double, Float64Array> JSFloat64Array;

template <> const ClassInfo JSUint8Array::s_info = { "Uint8Array" , &Base::s_info, 0, 0, CREATE_METHOD_TABLE(JSUint8Array) };
template <> const ClassInfo JSUint8ClampedArray::s_info = { "Uint8ClampedArray" , &Base::s_info, 0, 0, CREATE_METHOD_TABLE(JSUint8ClampedArray) };
template <> const ClassInfo JSUint16Array::s_info = { "Uint16Array" , &Base::s_info, 0, 0, CREATE_METHOD_TABLE(JSUint16Array) };
template <> const ClassInfo JSUint32Array::s_info = { "Uint32Array" , &Base::s_info, 0, 0, CREATE_METHOD_TABLE(JSUint32Array) };

template <> const ClassInfo JSInt8Array::s_info = { "Int8Array" , &Base::s_info, 0, 0, CREATE_METHOD_TABLE(JSInt8Array) };
template <> const ClassInfo JSInt16Array::s_info = { "Int16Array" , &Base::s_info, 0, 0, CREATE_METHOD_TABLE(JSInt16Array) };
template <> const ClassInfo JSInt32Array::s_info = { "Int32Array" , &Base::s_info, 0, 0, CREATE_METHOD_TABLE(JSInt32Array) };

template <> const ClassInfo JSFloat32Array::s_info = { "Float32Array" , &Base::s_info, 0, 0, CREATE_METHOD_TABLE(JSFloat32Array) };
template <> const ClassInfo JSFloat64Array::s_info = { "Float64Array" , &Base::s_info, 0, 0, CREATE_METHOD_TABLE(JSFloat64Array) };

static EncodedJSValue JSC_HOST_CALL constructJSUint8Array(ExecState* callFrame) { return constructTypedArray<JSUint8Array>(callFrame); }
static EncodedJSValue JSC_HOST_CALL constructJSUint8ClampedArray(ExecState* callFrame) { return constructTypedArray<JSUint8ClampedArray>(callFrame); }
static EncodedJSValue JSC_HOST_CALL constructJSUint16Array(ExecState* callFrame) { return constructTypedArray<JSUint16Array>(callFrame); }
static EncodedJSValue JSC_HOST_CALL constructJSUint32Array(ExecState* callFrame) { return constructTypedArray<JSUint32Array>(callFrame); }

static EncodedJSValue JSC_HOST_CALL constructJSInt8Array(ExecState* callFrame) { return constructTypedArray<JSInt8Array>(callFrame); }
static EncodedJSValue JSC_HOST_CALL constructJSInt16Array(ExecState* callFrame) { return constructTypedArray<JSInt16Array>(callFrame); }
static EncodedJSValue JSC_HOST_CALL constructJSInt32Array(ExecState* callFrame) { return constructTypedArray<JSInt32Array>(callFrame); }

static EncodedJSValue JSC_HOST_CALL constructJSFloat32Array(ExecState* callFrame) { return constructTypedArray<JSFloat32Array>(callFrame); }
static EncodedJSValue JSC_HOST_CALL constructJSFloat64Array(ExecState* callFrame) { return constructTypedArray<JSFloat64Array>(callFrame); }

}

#endif
