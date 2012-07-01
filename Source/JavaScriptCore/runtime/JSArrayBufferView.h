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

#ifndef JSArrayBufferView_h
#define JSArrayBufferView_h

#include <JSObject.h>

#include <wtf/Forward.h>
#include <wtf/ArrayBufferView.h>

namespace JSC {

class JSArrayBufferView : public JSNonFinalObject {
public:
    typedef JSNonFinalObject Base;
    
    static JSArrayBufferView* create(Structure* structure, JSGlobalObject* globalObject, PassRefPtr<ArrayBufferView> impl)
    {
        JSArrayBufferView* ptr = new (NotNull, allocateCell<JSArrayBufferView>(globalObject->globalData().heap)) JSArrayBufferView(structure, globalObject, impl);
        ptr->finishCreation(globalObject->globalData());
        return ptr;
    }

    static const ClassInfo s_info;

    static Structure* createStructure(JSGlobalData& globalData, JSGlobalObject* globalObject, JSValue prototype)
    {
        return Structure::create(globalData, globalObject, prototype, TypeInfo(ObjectType, StructureFlags), &s_info);
    }

    ArrayBufferView* m_impl;
    ArrayBufferView* impl() const { return m_impl; }

protected:
    JSArrayBufferView(Structure*, JSGlobalObject*, PassRefPtr<ArrayBufferView>);
    ~JSArrayBufferView();

    void finishCreation(JSC::JSGlobalData&);

    static const unsigned StructureFlags = OverridesGetOwnPropertySlot | Base::StructureFlags;

    static void destroy(JSCell*);

    static bool getOwnPropertySlot(JSCell*, ExecState*, PropertyName, PropertySlot&);
    static bool getOwnPropertyDescriptor(JSObject*, ExecState*, PropertyName, PropertyDescriptor&);
};

JSValue jsArrayBufferViewBuffer(ExecState*, JSValue, PropertyName);
JSValue jsArrayBufferViewByteOffset(ExecState*, JSValue, PropertyName);
JSValue jsArrayBufferViewByteLength(ExecState*, JSValue, PropertyName);

}

#endif // JSArrayBufferView_h
