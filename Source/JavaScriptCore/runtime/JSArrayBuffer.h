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

#ifndef JSArrayBuffer_h
#define JSArrayBuffer_h

#include "JSObject.h"
#include "JSGlobalObject.h"

#include <wtf/ArrayBuffer.h>

namespace JSC {

    class JSArrayBuffer : public JSNonFinalObject {
    public:
        typedef JSNonFinalObject Base;

        static JSArrayBuffer* create(JSGlobalData& globalData, Structure* structure, PassRefPtr<ArrayBuffer> impl)
        {
            JSArrayBuffer* ptr = new (NotNull, allocateCell<JSArrayBuffer>(globalData.heap)) JSArrayBuffer(globalData, structure, impl);
            ptr->finishCreation(globalData);
            return ptr;
        }
        
        static bool getOwnPropertySlot(JSCell*, ExecState*, const Identifier&, PropertySlot&);
        static bool getOwnPropertyDescriptor(JSObject*, ExecState*, const Identifier&, PropertyDescriptor&);
        
        static void finalize(JSCell*);
        
        static const ClassInfo s_info;
        
        static Structure* createStructure(JSGlobalData& globalData, JSGlobalObject* globalObject, JSValue prototype)
        {
            return Structure::create(globalData, globalObject, prototype, TypeInfo(ObjectType, StructureFlags), &s_info);
        }
        
	    ArrayBuffer* impl() const { return m_impl; }
	    void releaseImpl() { m_impl->deref(); m_impl = 0; }

	    void releaseImplIfNotNull() { if (m_impl) { m_impl->deref(); m_impl = 0; } }
		
    protected:
        JSArrayBuffer(JSGlobalData&, Structure*, PassRefPtr<ArrayBuffer>);
		~JSArrayBuffer();
		
        void finishCreation(JSGlobalData&);
        static const unsigned StructureFlags = OverridesGetOwnPropertySlot | JSObject::StructureFlags;
    private:
        ArrayBuffer* m_impl;
    };

inline JSArrayBuffer* asArrayBuffer(JSCell* cell)
{
    ASSERT(cell->inherits(&JSArrayBuffer::s_info));
    return static_cast<JSArrayBuffer*>(cell);
}

inline JSArrayBuffer* asArrayBuffer(JSValue value)
{
    return asArrayBuffer(value.asCell());
}

inline bool isJSArrayBuffer(JSCell* cell)
{
    return cell->classInfo() == &JSArrayBuffer::s_info;
}

inline bool isJSArrayBuffer(JSValue v)
{
    return v.isCell() && isJSArrayBuffer(v.asCell());
}

inline JSValue toJS(ExecState* exec, JSGlobalObject* globalObject, ArrayBuffer* impl)
{
	JSGlobalData& globalData = globalObject->globalData();
	
	JSValue prototype((JSCell*)globalObject->arrayBufferPrototype());
	
	Structure* structure = JSArrayBuffer::createStructure(globalData, globalObject, prototype);
	PassRefPtr<ArrayBuffer> reference(impl);
	
	return JSArrayBuffer::create(globalObject->globalData(), structure, reference);
}

inline ArrayBuffer* toArrayBuffer(JSValue value)
{
    return value.inherits(&JSArrayBuffer::s_info) ? static_cast<JSArrayBuffer*>(asObject(value))->impl() : 0;
}

} // namespace JSC

#endif
