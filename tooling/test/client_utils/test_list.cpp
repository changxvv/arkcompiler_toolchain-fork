/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "tooling/test/client_utils/test_list.h"

#include "tooling/test/client_utils/test_util.h"

// testcase list
#include "tooling/test/testcases/js_breakpoint_arrow_test.h"
#include "tooling/test/testcases/js_breakpoint_async_test.h"
#include "tooling/test/testcases/js_breakpoint_test.h"
#include "tooling/test/testcases/js_closure_scope_test.h"
#include "tooling/test/testcases/js_container_test.h"
#include "tooling/test/testcases/js_exception_test.h"
#include "tooling/test/testcases/js_heapsampling_test.h"
#include "tooling/test/testcases/js_local_variable_scope_test.h"
#include "tooling/test/testcases/js_module_variable_test.h"
#include "tooling/test/testcases/js_source_test.h"
#include "tooling/test/testcases/js_tracing_test.h"
#include "tooling/test/testcases/js_watch_test.h"
#include "tooling/test/testcases/js_heapdump_test.h"
#include "tooling/test/testcases/js_allocationtrack_test.h"

namespace panda::ecmascript::tooling::test {
static std::string g_currentTestName = "";

static void RegisterTests()
{
    // Register testcases
    TestUtil::RegisterTest("JsBreakpointTest", GetJsBreakpointTest());
    TestUtil::RegisterTest("JsBreakpointArrowTest", GetJsBreakpointArrowTest());
    TestUtil::RegisterTest("JsBreakpointAsyncTest", GetJsBreakpointAsyncTest());
    TestUtil::RegisterTest("JsClosureScopeTest", GetJsClosureScopeTest());
    TestUtil::RegisterTest("JsLocalVariableScopeTest", GetJsLocalVariableScopeTest());
    TestUtil::RegisterTest("JsExceptionTest", GetJsExceptionTest());
    TestUtil::RegisterTest("JsContainerTest", GetJsContainerTest());
    TestUtil::RegisterTest("JsModuleVariableTest", GetJsModuleVariableTest());
    TestUtil::RegisterTest("JsSourceTest", GetJsSourceTest());
    TestUtil::RegisterTest("JsTracingTest", GetJsTracingTest());
    TestUtil::RegisterTest("JsHeapsamplingTest", GetJsHeapsamplingTest());
    TestUtil::RegisterTest("JsWatchTest", GetJsWatchTest());
    TestUtil::RegisterTest("JsHeapdumpTest", GetJsHeapdumpTest());
    TestUtil::RegisterTest("JsAllocationtrackTest", GetJsAllocationtrackTest());
}

std::vector<const char *> GetTestList()
{
    RegisterTests();
    std::vector<const char *> res;

    auto &tests = TestUtil::GetTests();
    for (const auto &entry : tests) {
        res.push_back(entry.first.c_str());
    }
    return res;
}

void SetCurrentTestName(const std::string &testName)
{
    g_currentTestName = testName;
}

std::string GetCurrentTestName()
{
    return g_currentTestName;
}

std::pair<std::string, std::string> GetTestEntryPoint(const std::string &testName)
{
    return TestUtil::GetTest(testName)->GetEntryPoint();
}
}  // namespace panda::ecmascript::tooling::test
