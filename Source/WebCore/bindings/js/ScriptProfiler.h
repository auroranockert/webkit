/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#ifndef ScriptProfiler_h
#define ScriptProfiler_h

#if ENABLE(JAVASCRIPT_DEBUGGER)
#include "ScriptHeapSnapshot.h"
#include "ScriptProfile.h"
#include "ScriptState.h"
#include <wtf/PassRefPtr.h>


namespace WebCore {

class ExternalArrayVisitor;
class ExternalStringVisitor;
class NodeWrapperVisitor;
class Page;
class ScriptObject;
class ScriptValue;
class WorkerContext;

class ScriptProfiler {
    WTF_MAKE_NONCOPYABLE(ScriptProfiler);
public:
    class HeapSnapshotProgress {
    public:
        virtual ~HeapSnapshotProgress() { }
        virtual void Start(int totalWork) = 0;
        virtual void Worked(int workDone) = 0;
        virtual void Done() = 0;
        virtual bool isCanceled() = 0;
    };

    static void collectGarbage();
    static ScriptObject objectByHeapObjectId(unsigned id);
    static unsigned getHeapObjectId(const ScriptValue&);
    static void start(ScriptState* state, const String& title);
    static void startForPage(Page*, const String& title);
#if ENABLE(WORKERS)
    static void startForWorkerContext(WorkerContext*, const String& title);
#endif
    static PassRefPtr<ScriptProfile> stop(ScriptState* state, const String& title);
    static PassRefPtr<ScriptProfile> stopForPage(Page*, const String& title);
#if ENABLE(WORKERS)
    static PassRefPtr<ScriptProfile> stopForWorkerContext(WorkerContext*, const String& title);
#endif
    static PassRefPtr<ScriptHeapSnapshot> takeHeapSnapshot(const String&, HeapSnapshotProgress*) { return 0; }
    static bool causesRecompilation() { return true; }
    static bool isSampling() { return false; }
    static bool hasHeapProfiler() { return false; }
    // FIXME: Implement this counter for JSC. See bug 73936 for more details.
    static void visitNodeWrappers(NodeWrapperVisitor*) { }
    static void visitExternalStrings(ExternalStringVisitor*) { }
    static void visitExternalArrays(ExternalArrayVisitor*) { }
    static size_t profilerSnapshotsSize() { return 0; }
};

} // namespace WebCore

#endif // ENABLE(JAVASCRIPT_DEBUGGER)

#endif // ScriptProfiler_h
