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

#ifndef ArrayBufferPrototype_h
#define ArrayBufferPrototype_h

#include "JSArrayBuffer.h"
#include "Lookup.h"

namespace JSC {

    class ArrayBufferPrototype : public JSArrayBuffer {
    private:
        ArrayBufferPrototype(JSGlobalObject*, Structure*);
    public:
        typedef JSArrayBuffer Base;

        static ArrayBufferPrototype* create(ExecState* exec, JSGlobalObject* globalObject, Structure* structure)
        {
            ArrayBufferPrototype* prototype = new (NotNull, allocateCell<ArrayBufferPrototype>(*exec->heap())) ArrayBufferPrototype(globalObject, structure);
            prototype->finishCreation(globalObject);
            return prototype;
        }
        
        static bool getOwnPropertySlot(JSCell*, ExecState*, const Identifier&, PropertySlot&);
        static bool getOwnPropertyDescriptor(JSObject*, ExecState*, const Identifier&, PropertyDescriptor&);

        static const ClassInfo s_info;

        static Structure* createStructure(JSGlobalData& globalData, JSGlobalObject* globalObject, JSValue prototype)
        {
            return Structure::create(globalData, globalObject, prototype, TypeInfo(ObjectType, StructureFlags), &s_info);
        }
    protected:
        void finishCreation(JSGlobalObject*);
        static const unsigned StructureFlags = OverridesGetOwnPropertySlot | JSNonFinalObject::StructureFlags;
    };

}

#endif