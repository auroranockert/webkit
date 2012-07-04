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

#include "config.h"
#include "CSFXP64Register.h"
#include "CSFXP64RegisterConstructor.h"

namespace JSC {

FXP64RegisterConstructor::FXP64RegisterConstructor(JSGlobalObject* globalObject, Structure* structure) : InternalFunction(globalObject, structure)
{
}

void FXP64RegisterConstructor::finishCreation(ExecState* exec, ObjectPrototype* objectPrototype)
{
    Base::finishCreation(exec->globalData(), Identifier(exec, "FXP64Register").ustring());
    putDirectWithoutTransition(exec->globalData(), exec->propertyNames().prototype, objectPrototype, DontEnum | DontDelete | ReadOnly);
}

const ClassInfo FXP64RegisterConstructor::s_info = { "FXP64Register" , &Base::s_info, 0, 0, CREATE_METHOD_TABLE(FXP64RegisterConstructor) };

static EncodedJSValue JSC_HOST_CALL constructWithFXP64RegisterConstructor(ExecState* callFrame)
{
    Structure* structure = FXP64Register::createStructure(callFrame->globalData(), callFrame->lexicalGlobalObject(), callFrame->lexicalGlobalObject()->objectPrototype());

    return JSValue::encode(FXP64Register::create(callFrame, callFrame->lexicalGlobalObject(), structure));
}

ConstructType FXP64RegisterConstructor::getConstructData(JSCell*, ConstructData& constructData)
{
    constructData.native.function = constructWithFXP64RegisterConstructor;
    return ConstructTypeHost;
}

CallType FXP64RegisterConstructor::getCallData(JSCell*, CallData& callData)
{
    callData.native.function = constructWithFXP64RegisterConstructor;
    return CallTypeHost;
}

}
