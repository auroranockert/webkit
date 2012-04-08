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

#ifndef JSInt16Array_h
#define JSInt16Array_h

#include "JSArrayBufferView.h"
#include <wtf/Int16Array.h>

namespace JSC {

    class JSInt16Array : public JSArrayBufferView {
    public:
        typedef JSArrayBufferView Base;

        JSInt16Array* create(JSGlobalData& globalData, Structure* structure, PassRefPtr<Int16Array> impl)
        {
            JSInt16Array* ptr = new (NotNull, allocateCell<JSInt16Array>(globalData.heap)) JSInt16Array(globalData, structure, impl);
            ptr->finishCreation(globalData);
            return ptr;
        }

        static bool getOwnPropertySlot(JSCell* cell, ExecState* exec, const Identifier& propertyName, PropertySlot& slot);
        static bool getOwnPropertyDescriptor(JSObject* object, ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor);
        static bool getOwnPropertySlotByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, PropertySlot& slot);

        static void put(JSCell* cell, ExecState* exec, const Identifier& propertyName, JSValue value, PutPropertySlot& slot);
        static void putByIndex(JSCell* cell, ExecState* exec, unsigned propertyName, JSValue value, bool);

        static const ClassInfo s_info;

        Int16Array* impl() const { return m_impl; }

        void releaseImpl() { m_impl->deref(); m_impl = 0; }
        void releaseImplIfNotNull() { if (m_impl) { m_impl->deref(); m_impl = 0; } }
    protected:
	    JSInt16Array(JSGlobalData&, Structure*, PassRefPtr<Int16Array>);
        ~JSInt16Array();

        static const unsigned StructureFlags = OverridesGetOwnPropertySlot | ImplementsHasInstance | Base::StructureFlags;
    private:
        Int16Array* m_impl;
    };

static inline Int16Array* toInt16Array(JSValue value)
{
    return value.inherits(&JSInt16Array::s_info) ? static_cast<Int16Array*>(static_cast<JSInt16Array*>(asObject(value))->impl()) : 0;
}

}

#endif
