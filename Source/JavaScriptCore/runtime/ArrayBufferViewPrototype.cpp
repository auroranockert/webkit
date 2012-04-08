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

#include "config.h"
#include "ArrayBufferViewPrototype.h"

#include "Error.h"

namespace JSC {

ASSERT_CLASS_FITS_IN_CELL(ArrayBufferViewPrototype);

}

#include "ArrayBufferViewPrototype.lut.h"

namespace JSC {

const ClassInfo ArrayBufferViewPrototype::s_info = { "ArrayBufferView", &Base::s_info, 0, ExecState::arrayBufferViewPrototypeTable, CREATE_METHOD_TABLE(ArrayBufferViewPrototype)};

/* Source for ArrayBufferViewPrototype.lut.h
@begin arrayBufferViewPrototypeTable
@end
*/

ArrayBufferViewPrototype::ArrayBufferViewPrototype(JSGlobalObject* globalObject, Structure* structure)
    : JSArrayBufferView(globalObject->globalData(), structure, NULL)
{
}

void ArrayBufferViewPrototype::finishCreation(JSGlobalObject* globalObject)
{
    Base::finishCreation(globalObject->globalData());
    ASSERT(inherits(&s_info));
}

}
