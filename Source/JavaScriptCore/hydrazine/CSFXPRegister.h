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

#ifndef CSFXPRegister_h
#define CSFXPRegister_h

#include "CSMath.h"

#include "JSObject.h"
#include "ObjectPrototype.h"

#include "JSGlobalObject.h"

#include "Error.h"

namespace JSC {
    namespace Hydrazine {

        template <typename T>
        class FXPRegister : public JSNonFinalObject {
        public:
            typedef JSNonFinalObject Base;

            typedef struct { T a; } Op1;
            typedef struct { T a; T b; } Op2;

            T m_storage;

        protected:
            FXPRegister(ExecState* exec, Structure* structure) : Base(exec->globalData(), structure), m_storage(0) {}

            inline void finishCreation(JSGlobalData& globalData, JSGlobalObject*)
            {
                Base::finishCreation(globalData);

                ASSERT(inherits(&s_info));
            }

            static const unsigned StructureFlags = Base::StructureFlags;
        };

        template <typename T>
        static inline FXPRegister<T>* asFXPRegister(JSCell* cell)
        {
            ASSERT(cell->inherits(&FXPRegister<T>::s_info));

            return jsCast<FXPRegister<T>*>(cell);
        }

        template <typename T>
        static inline FXPRegister<T>* asFXPRegister(JSValue value)
        {
            return asFXPRegister<T>(value.asCell());
        }

        template <typename T>
        static inline T loadReceiver(ExecState* exec, EncodedJSValue* error) {
            JSValue reg = exec->thisValue();

            if (!reg.inherits(&FXPRegister<T>::s_info)) {
                *error = throwVMError(exec, createTypeError(exec, "Reciever is not a correctly typed Hydrazine Fixed-Point Register."));
                return (T)(0);
            } else {
                return asFXPRegister<T>(reg)->m_storage;
            }
        }

        template <typename T>
        static inline T loadArgument(ExecState* exec, size_t argument, EncodedJSValue* error) {
            JSValue reg = exec->argument(argument);

            if (!reg.inherits(&FXPRegister<T>::s_info)) {
                *error = throwVMError(exec, createTypeError(exec, "Argument is not a correctly typed Hydrazine Fixed-Point Register."));
                return (T)(0);
            } else {
                return asFXPRegister<T>(reg)->m_storage;
            }
        }

        template <typename T>
        static inline T loadSignedImmediate(ExecState* exec, size_t argument, EncodedJSValue*) {
            return (T)(exec->argument(argument).asInt32());
        }

        template <typename T>
        static inline typename FXPRegister<T>::Op1 loadOneOperand(ExecState* exec, EncodedJSValue* error) {
            size_t arguments = exec->argumentCount(); typename FXPRegister<T>::Op1 result;

            if (arguments == 0) {
                result.a = loadReceiver<T>(exec, error);
            } else {
                result.a = loadArgument<T>(exec, 0, error);
            }

            return result;
        }

        template <typename T>
        static inline typename FXPRegister<T>::Op2 loadTwoOperand(ExecState* exec, EncodedJSValue* error) {
            size_t arguments = exec->argumentCount(); typename FXPRegister<T>::Op2 result;

            if (arguments == 0) {
                *error = throwVMError(exec, JSC::createTypeError(exec, "Not enough arguments, needs at least one."));
            } else if (arguments == 1) {
                result.a = loadReceiver<T>(exec, error);
                result.b = loadArgument<T>(exec, 0, error);
            } else {
                result.a = loadArgument<T>(exec, 0, error);
                result.b = loadArgument<T>(exec, 1, error);
            }

            return result;
        }

        template <typename T>
        static inline typename FXPRegister<T>::Op2 loadTwoOperandWithSignedImmediate(ExecState* exec, EncodedJSValue* error) {
            size_t arguments = exec->argumentCount(); typename FXPRegister<T>::Op2 result;

            if (arguments == 0) {
                *error = throwVMError(exec, createTypeError(exec, "Not enough arguments, needs at least one."));
            } else if (arguments == 1) {
                result.a = loadReceiver<T>(exec, error);
                result.b = loadSignedImmediate<T>(exec, 0, error);
            } else {
                result.a = loadArgument<T>(exec, 0, error);
                result.b = loadSignedImmediate<T>(exec, 1, error);
            }

            return result;
        }

        template <typename T>
        static inline JSValue storeReceiver(ExecState* exec, T value, EncodedJSValue* error) {
            JSValue reg = exec->thisValue();

            if (!reg.inherits(&FXPRegister<T>::s_info)) {
                *error = throwVMError(exec, createTypeError(exec, "Receiver is not a Hydrazine correctly typed Fixed-Point Register."));
            } else {
                asFXPRegister<T>(reg)->m_storage = value;
            }

            return reg;
        }
        
    }
}

#endif // CSFXPRegister_h
