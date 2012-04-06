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

#ifndef Accelerate_h
#define Accelerate_h

#include "JSObject.h"

namespace JSC {

    class Accelerate : public JSNonFinalObject {
    private:
        Accelerate(JSGlobalObject*, Structure*);

    public:
        typedef JSNonFinalObject Base;

        static Accelerate* create(ExecState* exec, JSGlobalObject* globalObject, Structure* structure)
        {
            Accelerate* object = new (NotNull, allocateCell<Accelerate>(*exec->heap())) Accelerate(globalObject, structure);
            object->finishCreation(exec, globalObject);
            return object;
        }

        static bool getOwnPropertySlot(JSC::JSCell*, JSC::ExecState*, const JSC::Identifier& propertyName, JSC::PropertySlot&);
        static bool getOwnPropertyDescriptor(JSC::JSObject*, JSC::ExecState*, const JSC::Identifier& propertyName, JSC::PropertyDescriptor&);

        static const JSC::ClassInfo s_info;

        static JSC::Structure* createStructure(JSC::JSGlobalData& globalData, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
        {
            return JSC::Structure::create(globalData, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), &s_info);
        }
    protected:
        void finishCreation(ExecState*, JSGlobalObject*);
        static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | JSObject::StructureFlags;
    };

} // namespace WebCore

#endif
