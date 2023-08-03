/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ECMASCRIPT_TOOLING_TEST_UTILS_TESTCASES_JS_CLOSURE_SCOPE_SECOND_TEST_H
#define ECMASCRIPT_TOOLING_TEST_UTILS_TESTCASES_JS_CLOSURE_SCOPE_SECOND_TEST_H

#include <map>
#include "test/utils/test_util.h"

namespace panda::ecmascript::tooling::test {
class JsClosureScopeSecondTest : public TestEvents {
public:
    JsClosureScopeSecondTest()
    {
        breakpoint = [this](const JSPtLocation &location) {
            ASSERT_TRUE(location.GetMethodId().IsValid());
            ASSERT_LOCATION_EQ(location, location_);
            ++breakpointCounter_;
            debugger_->NotifyPaused(location, PauseReason::INSTRUMENTATION);
            TestUtil::SuspendUntilContinue(DebugEvent::BREAKPOINT, location);
            return true;
        };

        loadModule = [this](std::string_view moduleName) {
            std::string pandaFile = DEBUGGER_ABC_DIR "closure_scope_second.abc";
            std::string sourceFile = DEBUGGER_JS_DIR "closure_scope_second.js";
            static_cast<JsClosureScopeSecondTestChannel *>(channel_)->Initial(vm_, runtime_);
            runtime_->Enable();
            // 28: breakpointer line
            const int32_t line = 28;
            location_ = TestUtil::GetLocation(sourceFile.c_str(), line, 0, pandaFile.c_str());
            ASSERT_TRUE(location_.GetMethodId().IsValid());
            TestUtil::SuspendUntilContinue(DebugEvent::LOAD_MODULE);
            ASSERT_EQ(moduleName, pandaFile);
            ASSERT_TRUE(debugger_->NotifyScriptParsed(0, pandaFile));
            auto condFuncRef = FunctionRef::Undefined(vm_);
            auto ret = debugInterface_->SetBreakpoint(location_, condFuncRef);
            ASSERT_TRUE(ret);
            return true;
        };

        scenario = [this]() {
            TestUtil::WaitForLoadModule();
            TestUtil::Continue();
            TestUtil::WaitForBreakpoint(location_);
            TestUtil::Continue();
            auto ret = debugInterface_->RemoveBreakpoint(location_);
            ASSERT_TRUE(ret);
            ASSERT_EXITED();
            return true;
        };

        vmDeath = [this]() {
            ASSERT_EQ(breakpointCounter_, 1U);
            return true;
        };

        channel_ = new JsClosureScopeSecondTestChannel();
    }

    std::pair<std::string, std::string> GetEntryPoint() override
    {
        std::string pandaFile = DEBUGGER_ABC_DIR "closure_scope_second.abc";
        return {pandaFile, entryPoint_};
    }

    ~JsClosureScopeSecondTest()
    {
        delete channel_;
        channel_ = nullptr;
    }
private:
    class JsClosureScopeSecondTestChannel : public TestChannel {
    public:
        JsClosureScopeSecondTestChannel() = default;
        ~JsClosureScopeSecondTestChannel() = default;

        void Initial(const EcmaVM *vm, RuntimeImpl *runtime)
        {
            vm_ = vm;
            runtime_ = runtime;
        }

        void SendNotification(const PtBaseEvents &events) override
        {
            const static std::vector<std::function<bool(const PtBaseEvents &events)>> eventList = {
                [](const PtBaseEvents &events) -> bool {
                    std::string sourceFile = DEBUGGER_JS_DIR "closure_scope_second.js";
                    auto parsed = static_cast<const ScriptParsed *>(&events);
                    std::string str = parsed->ToJson()->Stringify();
                
                    ASSERT_EQ(parsed->GetName(), "Debugger.scriptParsed");
                    ASSERT_EQ(parsed->GetUrl(), sourceFile);
                    return true;
                },
                [this](const PtBaseEvents &events) -> bool {
                    auto paused = static_cast<const Paused *>(&events);
                    std::string str = paused->ToJson()->Stringify();

                    ASSERT_EQ(paused->GetName(), "Debugger.paused");
                    auto frame = paused->GetCallFrames()->at(0).get();
                    auto scopes = frame->GetScopeChain();
                    int ScopeIndex = 0;

                    for (uint32_t i = 0; i < scopes->size(); i++) {
                        auto scope = scopes->at(i).get();
                        if (scope->GetType() != Scope::Type::Closure()) {
                            continue;
                        }
                        auto closureScopeId = scope->GetObject()->GetObjectId();
                        GetPropertiesParams params;
                        params.SetObjectId(closureScopeId).SetOwnProperties(true);
                        std::vector<std::unique_ptr<PropertyDescriptor>> outPropertyDesc;
                        runtime_->GetProperties(params, &outPropertyDesc, {}, {}, {});
                        std::map<std::string, std::string> variables;
                        auto truthGroundScope_ = truthGroundMap_.at(ScopeIndex);
                        
                        for (const auto &property : outPropertyDesc) {
                            auto value = property->GetValue();
                            auto name = property->GetName();
                            std::cout << "name: " << name << std::endl;
                            ASSERT_TRUE(truthGroundScope_.find(name) != truthGroundScope_.end());
                            extractVariable(name, value, variables);
                            ASSERT_EQ(variables[name], truthGroundScope_.at(name));
                        }
                        ASSERT_EQ(variables.size(), truthGroundScope_.size());
                        ScopeIndex++;
                    }
                    ASSERT_EQ(ScopeIndex, scopeCount_);
                    return true;
                }
            };

            ASSERT_TRUE(eventList[index_++](events));
        }
    private:
        NO_COPY_SEMANTIC(JsClosureScopeSecondTestChannel);
        NO_MOVE_SEMANTIC(JsClosureScopeSecondTestChannel);

        void extractVariable(std::string name, RemoteObject *value, std::map<std::string, std::string> &variables)
        {
            std::string variableValue = "undefined";
            if (value->HasUnserializableValue()) {
                variableValue = value->GetUnserializableValue();
            }
            if (variables.find(name) == variables.end()) {
                variables.emplace(name, variableValue);
            }
        }

        const std::map<int, std::map<std::string, std::string>> truthGroundMap_ = {
            {0, {{"i", "5"}}},
            {1, {{"a", "10"}}}
        };

        int32_t index_ {0};
        const int scopeCount_ {2};
        const EcmaVM *vm_ {nullptr};
        RuntimeImpl *runtime_ {nullptr};
    };
    std::string entryPoint_ = "_GLOBAL::func_main_0";
    JSPtLocation location_ {nullptr, JSPtLocation::EntityId(0), 0};
    size_t breakpointCounter_ = 0;
};

std::unique_ptr<TestEvents> GetJsClosureScopeSecondTest()
{
    return std::make_unique<JsClosureScopeSecondTest>();
}
} // namespace panda::ecmascript::tooling::test

#endif  // ECMASCRIPT_TOOLING_TEST_UTILS_TESTCASES_JS_CLOSURE_SCOPE_SECOND_TEST_H
