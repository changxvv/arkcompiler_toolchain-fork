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
#include "tooling/test/testcases/js_breakpoint_test.h"

namespace panda::ecmascript::tooling::test {
static std::string g_currentTestName = "";

static void RegisterTests()
{
    // Register testcases
    TestUtil::RegisterTest("JsBreakpointTest", GetJsBreakpointTest());
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
