/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
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

#ifndef SpecializedThunkJIT_h
#define SpecializedThunkJIT_h

#if ENABLE(JIT)

#include "Executable.h"
#include "JSInterfaceJIT.h"
#include "LinkBuffer.h"

namespace JSC {

    class SpecializedThunkJIT : public JSInterfaceJIT {
    public:
        static const int ThisArgument = -1;
        SpecializedThunkJIT(int expectedArgCount, JSGlobalData* globalData)
            : m_expectedArgCount(expectedArgCount)
            , m_globalData(globalData)
        {
            // Check that we have the expected number of arguments
            m_failures.append(branch32(NotEqual, payloadFor(RegisterFile::ArgumentCount), TrustedImm32(expectedArgCount + 1)));
        }
        
        void loadDoubleArgument(int argument, FPRegisterID dst, RegisterID scratch)
        {
            unsigned src = CallFrame::argumentOffset(argument);
            m_failures.append(emitLoadDouble(src, dst, scratch));
        }
        
        void loadCellArgument(int argument, RegisterID dst)
        {
            unsigned src = CallFrame::argumentOffset(argument);
            m_failures.append(emitLoadJSCell(src, dst));
        }
        
        void loadJSStringArgument(int argument, RegisterID dst)
        {
            loadCellArgument(argument, dst);
            m_failures.append(branchPtr(NotEqual, Address(dst, JSCell::classInfoOffset()), TrustedImmPtr(&JSString::s_info)));
        }
        
        void loadInt32Argument(int argument, RegisterID dst, Jump& failTarget)
        {
            unsigned src = CallFrame::argumentOffset(argument);
            failTarget = emitLoadInt32(src, dst);
        }
        
        void loadInt32Argument(int argument, RegisterID dst)
        {
            Jump conversionFailed;
            loadInt32Argument(argument, dst, conversionFailed);
            m_failures.append(conversionFailed);
        }
        
        void appendFailure(const Jump& failure)
        {
            m_failures.append(failure);
        }

        void returnJSValue(RegisterID src)
        {
            if (src != regT0)
                move(src, regT0);
            loadPtr(payloadFor(RegisterFile::CallerFrame, callFrameRegister), callFrameRegister);
            ret();
        }
        
        void returnDouble(FPRegisterID src)
        {
#if USE(JSVALUE64)
            moveDoubleToPtr(src, regT0);
            Jump zero = branchTestPtr(Zero, regT0);
            subPtr(tagTypeNumberRegister, regT0);
            Jump done = jump();
            zero.link(this);
            move(tagTypeNumberRegister, regT0);
            done.link(this);
#else
            storeDouble(src, Address(stackPointerRegister, -(int)sizeof(double)));
            loadPtr(Address(stackPointerRegister, OBJECT_OFFSETOF(JSValue, u.asBits.tag) - sizeof(double)), regT1);
            loadPtr(Address(stackPointerRegister, OBJECT_OFFSETOF(JSValue, u.asBits.payload) - sizeof(double)), regT0);
            Jump lowNonZero = branchTestPtr(NonZero, regT1);
            Jump highNonZero = branchTestPtr(NonZero, regT0);
            move(TrustedImm32(0), regT0);
            move(TrustedImm32(Int32Tag), regT1);
            lowNonZero.link(this);
            highNonZero.link(this);
#endif
            loadPtr(payloadFor(RegisterFile::CallerFrame, callFrameRegister), callFrameRegister);
            ret();
        }

        void returnInt32(RegisterID src)
        {
            if (src != regT0)
                move(src, regT0);
            tagReturnAsInt32();
            loadPtr(payloadFor(RegisterFile::CallerFrame, callFrameRegister), callFrameRegister);
            ret();
        }

        void returnJSCell(RegisterID src)
        {
            if (src != regT0)
                move(src, regT0);
            tagReturnAsJSCell();
            loadPtr(payloadFor(RegisterFile::CallerFrame, callFrameRegister), callFrameRegister);
            ret();
        }
        
        MacroAssemblerCodeRef finalize(JSGlobalData& globalData, MacroAssemblerCodePtr fallback, const char* thunkKind)
        {
            LinkBuffer patchBuffer(globalData, this, GLOBAL_THUNK_ID);
            patchBuffer.link(m_failures, CodeLocationLabel(fallback));
            for (unsigned i = 0; i < m_calls.size(); i++)
                patchBuffer.link(m_calls[i].first, m_calls[i].second);
            return FINALIZE_CODE(patchBuffer, ("Specialized thunk for %s", thunkKind));
        }

        // Assumes that the target function uses fpRegister0 as the first argument
        // and return value. Like any sensible architecture would.
        void callDoubleToDouble(FunctionPtr function)
        {
            m_calls.append(std::make_pair(call(), function));
        }

    private:

        void tagReturnAsInt32()
        {
#if USE(JSVALUE64)
            orPtr(tagTypeNumberRegister, regT0);
#else
            move(TrustedImm32(JSValue::Int32Tag), regT1);
#endif
        }

        void tagReturnAsJSCell()
        {
#if USE(JSVALUE32_64)
            move(TrustedImm32(JSValue::CellTag), regT1);
#endif
        }
        
        int m_expectedArgCount;
        JSGlobalData* m_globalData;
        MacroAssembler::JumpList m_failures;
        Vector<std::pair<Call, FunctionPtr> > m_calls;
    };

}

#endif // ENABLE(JIT)

#endif // SpecializedThunkJIT_h
