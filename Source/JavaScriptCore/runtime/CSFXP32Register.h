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

#ifndef CSFXP32Register_h
#define CSFXP32Register_h

#include "JSObject.h"
#include "ObjectPrototype.h"

#include "JSGlobalObject.h"

namespace JSC {
    class FXP32Register : public JSNonFinalObject {
    public:
        typedef JSNonFinalObject Base;
        typedef union { uint32_t u; int32_t s; float sp; } Union;

        typedef struct { Union a; } Op1;
        typedef struct { Union a; Union b; } Op2;

        static FXP32Register* create(ExecState* exec, JSGlobalObject* globalObject, Structure* structure)
        {
            FXP32Register* reg = new (NotNull, allocateCell<FXP32Register>(*exec->heap())) FXP32Register(exec, structure);

            reg->finishCreation(exec->globalData(), globalObject);

            return reg;
        }

        static const ClassInfo s_info;

        static Structure* createStructure(JSGlobalData& globalData, JSGlobalObject* globalObject, JSValue prototype)
        {
            return Structure::create(globalData, globalObject, prototype, TypeInfo(ObjectType, StructureFlags), &s_info);
        }

        uint32_t m_storage;

    protected:
        void finishCreation(JSGlobalData& globalData, JSGlobalObject*);

        static const unsigned StructureFlags = OverridesGetOwnPropertySlot | Base::StructureFlags;

    private:
        FXP32Register(ExecState* exec, Structure* structure);

        static bool getOwnPropertySlot(JSCell*, ExecState*, PropertyName, PropertySlot&);
        static bool getOwnPropertySlotByIndex(JSCell*, ExecState*, unsigned propertyName, PropertySlot&);
        static bool getOwnPropertyDescriptor(JSObject*, ExecState*, PropertyName, PropertyDescriptor&);
    };

    static inline FXP32Register* asFXP32Register(JSCell* cell)
    {
        ASSERT(cell->inherits(&FXP32Register::s_info));

        return jsCast<FXP32Register*>(cell);
    }

    static inline FXP32Register* asFXP32Register(JSValue value)
    {
        return asFXP32Register(value.asCell());
    }
}

#endif // CSFXP32Register_h
