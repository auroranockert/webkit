/*
 * Copyright (C) 2012 Apple Inc. All rights reserved.
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

#ifndef TypedArray_h
#define TypedArray_h

#include "JSObject.h"
#include "JSGlobalObject.h"
#include "InternalFunction.h"

#include "JSArrayBufferView.h"
#include "JSArrayBufferViewPrototype.h"

#include <wtf/Forward.h>
#include <wtf/Int8Array.h>
#include <wtf/Int16Array.h>
#include <wtf/Int32Array.h>
#include <wtf/Uint8Array.h>
#include <wtf/Uint8ClampedArray.h>
#include <wtf/Uint16Array.h>
#include <wtf/Uint32Array.h>
#include <wtf/Float32Array.h>
#include <wtf/Float64Array.h>

namespace JSC {

template <typename T, typename I>
class TypedArray : public JSArrayBufferView {
public:
    typedef JSArrayBufferView Base;
    typedef TypedArray<T, I> Current;
    typedef I Implementation;
    
    static TypedArray<T, I>* create(Structure* structure, JSGlobalObject* globalObject, PassRefPtr<I> impl)
    {
        Current* ptr = new (NotNull, allocateCell<Current>(globalObject->globalData().heap)) Current(structure, globalObject, impl);
        ptr->finishCreation(globalObject->globalData());
        return ptr;
    }

    static const ClassInfo s_info;

    static Structure* createStructure(JSGlobalData& globalData, JSGlobalObject* globalObject, JSValue prototype)
    {
        return Structure::create(globalData, globalObject, prototype, TypeInfo(ObjectType, StructureFlags), &s_info);
    }

    // TODO: static const JSC::TypedArrayType TypedArrayStorageType = JSC::TypedArray##name;

    uint32_t m_storageLength;

    T* m_storage;

    I* impl() const { return static_cast<I*>(Base::impl()); }

protected:
    void finishCreation(JSGlobalData& globalData);

    static const unsigned StructureFlags = OverridesGetPropertyNames | OverridesGetOwnPropertySlot | Base::StructureFlags;

    JSValue getByIndex(ExecState*, unsigned index)
    {
        ASSERT_GC_OBJECT_INHERITS(this, &s_info);
        T result = impl()->item(index);

        if (isnan((double)result)) {
            return jsNaN();
        }

        return JSValue(result);
        
    }

    void indexSetter(ExecState* exec, unsigned index, JSValue value)
    {
        impl()->set(index, value.toNumber(exec));
    }
private:
    TypedArray<T, I>(Structure* structure, JSGlobalObject* globalObject, PassRefPtr<I> impl);
    
    static bool getOwnPropertySlot(JSCell* cell, ExecState* exec, PropertyName propertyName, PropertySlot& slot);
    static bool getOwnPropertySlotByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, PropertySlot& slot);
    static bool getOwnPropertyDescriptor(JSObject* object, ExecState* exec, PropertyName propertyName, PropertyDescriptor& descriptor);
    
    static void getOwnPropertyNames(JSObject* object, ExecState* exec, PropertyNameArray& propertyNames, EnumerationMode mode);
    
    static void put(JSCell* cell, ExecState* exec, PropertyName propertyName, JSValue value, PutPropertySlot& slot);
    static void putByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, JSValue value, bool);
};

template<typename T, typename I>
TypedArray<T, I>::TypedArray(Structure* structure, JSGlobalObject* globalObject, PassRefPtr<I> impl) : Base(structure, globalObject, impl)
{
}

template<typename T, typename I>
void TypedArray<T, I>::finishCreation(JSGlobalData& globalData)
{
    Base::finishCreation(globalData);
    TypedArrayDescriptor descriptor(&Current::s_info, OBJECT_OFFSETOF(Current, m_storage), OBJECT_OFFSETOF(Current, m_storageLength));
    globalData.registerTypedArrayDescriptor(impl(), descriptor);
    m_storage = impl()->data();
    m_storageLength = impl()->length();
    putDirect(globalData, globalData.propertyNames->length, jsNumber(m_storageLength), DontDelete | ReadOnly | DontEnum);
    ASSERT(inherits(&s_info));
}

template<typename T, typename I>
bool TypedArray<T, I>::getOwnPropertySlot(JSCell* cell, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
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

template<typename T, typename I>
bool TypedArray<T, I>::getOwnPropertySlotByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, PropertySlot& slot)
{
    Current* thisObject = jsCast<Current*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, &s_info);

    if (propertyName < thisObject->m_storageLength) {
        slot.setValue(thisObject->getByIndex(exec, propertyName));
        return true;
    }

    return thisObject->methodTable()->getOwnPropertySlot(thisObject, exec, Identifier::from(exec, propertyName), slot);
}

template<typename T, typename I>
bool TypedArray<T, I>::getOwnPropertyDescriptor(JSObject* object, ExecState* exec, PropertyName propertyName, PropertyDescriptor& descriptor)
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

template<typename T, typename I>
void TypedArray<T, I>::getOwnPropertyNames(JSObject* object, ExecState* exec, PropertyNameArray& propertyNames, EnumerationMode mode)
{
    Current* thisObject = jsCast<Current*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, &s_info);

    for (unsigned i = 0; i < thisObject->m_storageLength; ++i) {
        propertyNames.add(Identifier::from(exec, i));
    }

    Base::getOwnPropertyNames(thisObject, exec, propertyNames, mode);
}

template<typename T, typename I>
void TypedArray<T, I>::put(JSCell* cell, ExecState* exec, PropertyName propertyName, JSValue value, PutPropertySlot& slot)
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

template<typename T, typename I>
void TypedArray<T, I>::putByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, JSValue value, bool)
{
    Current* thisObject = jsCast<Current*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, &s_info);
    thisObject->indexSetter(exec, propertyName, value);
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

template <typename T, typename I>
class TypedArrayConstructor : public InternalFunction {
public:
    typedef InternalFunction Base;
    typedef TypedArrayConstructor<T, I> Current;

    static Current* create(ExecState* exec, JSGlobalObject* globalObject, Structure* structure, JSArrayBufferViewPrototype* prototype)
    {
        Current* ptr = new (NotNull, allocateCell<Current>(*exec->heap())) Current(globalObject, structure);
        ptr->finishCreation(exec, prototype);
        return ptr;
    }

    static const ClassInfo s_info;

    static Structure* createStructure(JSGlobalData& globalData, JSGlobalObject* globalObject, JSValue prototype)
    {
        return Structure::create(globalData, globalObject, prototype, TypeInfo(ObjectType, StructureFlags), &s_info);
    }

protected:
    static const unsigned StructureFlags = Base::StructureFlags;
    static ConstructType getConstructData(JSCell*, ConstructData&);
    static CallType getCallData(JSCell*, CallData&);

private:
    TypedArrayConstructor(JSGlobalObject*, Structure*);
    void finishCreation(ExecState*, JSArrayBufferViewPrototype*);
};

template <typename T, typename I>
TypedArrayConstructor<T, I>::TypedArrayConstructor(JSGlobalObject* globalObject, Structure* structure)
    : InternalFunction(globalObject, structure)
{
}

template <typename T, typename I>
void TypedArrayConstructor<T, I>::finishCreation(ExecState* exec, JSArrayBufferViewPrototype* objectPrototype)
{
    Base::finishCreation(exec->globalData(), Identifier(exec, "Typed Array").ustring());
}

template <typename T, typename I>
static EncodedJSValue JSC_HOST_CALL constructWithTypedArrayConstructor(ExecState* callFrame)
{
    if (callFrame->argumentCount() < 1) {
        return JSValue::encode(jsUndefined());
    }

    int32_t length = callFrame->argument(0).toInt32(callFrame);

    if (length < 0) {
        return JSValue::encode(jsUndefined());
    }

    JSCell* prototype = static_cast<JSCell*>(callFrame->lexicalGlobalObject()->arrayBufferViewPrototype());

    Structure* structure = TypedArray<T, I>::createStructure(callFrame->globalData(), callFrame->lexicalGlobalObject(), JSValue(prototype));

    return JSValue::encode(TypedArray<T, I>::create(structure, callFrame->lexicalGlobalObject(), TypedArray<T, I>::Implementation::create(length)));
}

template <typename T, typename I>
ConstructType TypedArrayConstructor<T, I>::getConstructData(JSCell*, ConstructData& constructData)
{
    constructData.native.function = constructWithTypedArrayConstructor<T, I>;
    return ConstructTypeHost;
}

template <typename T, typename I>
CallType TypedArrayConstructor<T, I>::getCallData(JSCell*, CallData& callData)
{
    callData.native.function = constructWithTypedArrayConstructor<T, I>;
    return CallTypeHost;
}

typedef TypedArrayConstructor<uint8_t, Uint8Array> JSUint8ArrayConstructor;
typedef TypedArrayConstructor<uint8_t, Uint8ClampedArray> JSUint8ClampedArrayConstructor;
typedef TypedArrayConstructor<uint16_t, Uint16Array> JSUint16ArrayConstructor;
typedef TypedArrayConstructor<uint32_t, Uint32Array> JSUint32ArrayConstructor;

typedef TypedArrayConstructor<int8_t, Int8Array> JSInt8ArrayConstructor;
typedef TypedArrayConstructor<int16_t, Int16Array> JSInt16ArrayConstructor;
typedef TypedArrayConstructor<int32_t, Int32Array> JSInt32ArrayConstructor;

typedef TypedArrayConstructor<float, Float32Array> JSFloat32ArrayConstructor;
typedef TypedArrayConstructor<double, Float64Array> JSFloat64ArrayConstructor;

}

#endif // TypedArray_h
