/*
 * Copyright (C) 2012 Jens Nockert (jens@nockert.se)
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef JSArrayBufferView_h
#define JSArrayBufferView_h

#include "JSObject.h"
#include "JSGlobalObject.h"

#include <wtf/ArrayBufferView.h>

namespace JSC {

    class JSArrayBufferView : public JSNonFinalObject {
    public:
        typedef JSArrayBufferView Base;

        static JSArrayBufferView* create(JSGlobalData& globalData, Structure* structure, PassRefPtr<ArrayBufferView> impl)
        {
            JSArrayBufferView* ptr = new (NotNull, allocateCell<JSArrayBufferView>(globalData.heap)) JSArrayBufferView(globalData, structure, impl);
            ptr->finishCreation(globalData);
            return ptr;
        }

        static bool getOwnPropertySlot(JSCell*, ExecState*, const Identifier&, PropertySlot&);
        static bool getOwnPropertyDescriptor(JSObject*, ExecState*, const Identifier&, PropertyDescriptor&);

        static const ClassInfo s_info;

        static Structure* createStructure(JSGlobalData& globalData, JSGlobalObject* globalObject, JSValue prototype)
        {
            return Structure::create(globalData, globalObject, prototype, TypeInfo(ObjectType, StructureFlags), &s_info);
        }

        ArrayBufferView* impl() const { return m_impl; }
        void releaseImpl() { m_impl->deref(); m_impl = 0; }

        void releaseImplIfNotNull() { if (m_impl) { m_impl->deref(); m_impl = 0; } }
    protected:
        JSArrayBufferView(JSGlobalData&, Structure*, PassRefPtr<ArrayBufferView>);
        ~JSArrayBufferView();

        void finishCreation(JSGlobalData&);

        static const unsigned StructureFlags = OverridesGetOwnPropertySlot | Base::StructureFlags;
    private:
        ArrayBufferView* m_impl;
    };

inline ArrayBufferView* toArrayBufferView(JSValue value)
{
    return value.inherits(&JSArrayBufferView::s_info) ? static_cast<JSArrayBufferView*>(asObject(value))->impl() : 0;
}

}

#endif
