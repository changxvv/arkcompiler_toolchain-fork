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

#include "tooling/client/utils/cli_command.h"

#include <functional>
#include <iostream>

#include "tooling/client/domain/debugger_client.h"
#include "tooling/client/domain/runtime_client.h"
#include "common/log_wrapper.h"
#include "tooling/client/manager/breakpoint_manager.h"
#include "tooling/client/manager/domain_manager.h"
#include "tooling/client/manager/stack_manager.h"
#include "tooling/client/manager/variable_manager.h"
#include "tooling/client/session/session.h"

namespace OHOS::ArkCompiler::Toolchain {
const std::string HELP_MSG = "usage: <command> <options>\n"
    " These are common commands list:\n"
    "  allocationtrack(at)                           allocation-track-start with options\n"
    "  allocationtrack-stop(at-stop)                 allocation-track-stop\n"
    "  heapdump(hd)                                  heapdump with options\n"
    "  heapprofiler-enable(hp-enable)                heapdump enable\n"
    "  heapprofiler-disable(hp-disable)              heapdump disable\n"
    "  sampling(sampling)                            heapprofiler sampling\n"
    "  sampling-stop(sampling-stop)                  heapprofiler sampling stop\n"
    "  collectgarbage(gc)                            heapprofiler collectgarbage\n"
    "  cpuprofile(cp)                                cpuprofile start\n"
    "  cpuprofile-stop(cp-stop)                      cpuprofile stop\n"
    "  cpuprofile-enable(cp-enable)                  cpuprofile enable\n"
    "  cpuprofile-disable(cp-disable)                cpuprofile disable\n"
    "  cpuprofile-show(cp-show)                      cpuprofile show\n"
    "  cpuprofile-setSamplingInterval(cp-ssi)        cpuprofile setSamplingInterval\n"
    "  runtime-enable(rt-enable)                     runtime enable\n"
    "  heapusage(hu)                                 runtime getHeapUsage\n"
    "  break(b)                                      break with options\n"
    "  backtrack(bt)                                 backtrace\n"
    "  continue(c)                                   continue\n"
    "  delete(d)                                     delete with options\n"
    "  disable                                       disable\n"
    "  display                                       display\n"
    "  enable                                        debugger enable\n"
    "  finish(fin)                                   finish\n"
    "  frame(f)                                      frame\n"
    "  help(h)                                       list available commands\n"
    "  ignore(ig)                                    ignore\n"
    "  infobreakpoints(infob)                        info-breakpoints\n"
    "  infosource(infos)                             info-source\n"
    "  jump(j)                                       jump\n"
    "  list(l)                                       list\n"
    "  next(n)                                       next\n"
    "  print(p)                                      print with options\n"
    "  ptype                                         ptype\n"
    "  quit(q)                                       quit\n"
    "  run(r)                                        run\n"
    "  setvar(sv)                                    set value with options\n"
    "  step(s)                                       step\n"
    "  undisplay                                     undisplay\n"
    "  watch(wa)                                     watch\n"
    "  resume                                        resume\n"
    "  showstack(ss)                                 showstack\n"
    "  step-into(si)                                 step-into\n"
    "  step-out(so)                                  step-out\n"
    "  step-over(sov)                                step-over\n"
    "  runtime-disable                               rt-disable\n"
    "  session-new                                   add new session\n"
    "  session-remove                                del a session\n"
    "  session-list                                  list all sessions\n"
    "  session                                       switch session\n"
    "  forall                                        command for all sessions\n"
    "  success                                       test success\n"
    "  fail                                          test fail\n";

const std::vector<std::string> cmdList = {
    "allocationtrack",
    "allocationtrack-stop",
    "heapdump",
    "heapprofiler-enable",
    "heapprofiler-disable",
    "sampling",
    "sampling-stop",
    "collectgarbage",
    "cpuprofile",
    "cpuprofile-stop",
    "cpuprofile-enable",
    "cpuprofile-disable",
    "cpuprofile-show",
    "cpuprofile-setSamplingInterval",
    "runtime-enable",
    "heapusage",
    "break",
    "backtrack",
    "continue",
    "delete",
    "disable",
    "display",
    "enable",
    "finish",
    "frame",
    "help",
    "ignore",
    "infobreakpoints",
    "infosource",
    "jump",
    "list",
    "next",
    "print",
    "ptype",
    "run",
    "setvar",
    "step",
    "undisplay",
    "watch",
    "resume",
    "step-into",
    "step-out",
    "step-over",
    "runtime-disable",
    "session-new",
    "session-remove",
    "session-list",
    "session",
    "success",
    "fail"
};

ErrCode CliCommand::ExecCommand()
{
    CreateCommandMap();

    ErrCode result = OnCommand();
    return result;
}

void CliCommand::CreateCommandMap()
{
    commandMap_ = {
        {std::make_pair("allocationtrack", "at"), std::bind(&CliCommand::HeapProfilerCommand, this, "allocationtrack")},
        {std::make_pair("allocationtrack-stop", "at-stop"),
            std::bind(&CliCommand::HeapProfilerCommand, this, "allocationtrack-stop")},
        {std::make_pair("heapdump", "hd"), std::bind(&CliCommand::HeapProfilerCommand, this, "heapdump")},
        {std::make_pair("heapprofiler-enable", "hp-enable"),
            std::bind(&CliCommand::HeapProfilerCommand, this, "heapprofiler-enable")},
        {std::make_pair("heapprofiler-disable", "hp-disable"),
            std::bind(&CliCommand::HeapProfilerCommand, this, "heapprofiler-disable")},
        {std::make_pair("sampling", "sampling"), std::bind(&CliCommand::HeapProfilerCommand, this, "sampling")},
        {std::make_pair("sampling-stop", "sampling-stop"),
            std::bind(&CliCommand::HeapProfilerCommand, this, "sampling-stop")},
        {std::make_pair("collectgarbage", "gc"), std::bind(&CliCommand::HeapProfilerCommand, this, "collectgarbage")},
        {std::make_pair("cpuprofile", "cp"), std::bind(&CliCommand::CpuProfileCommand, this, "cpuprofile")},
        {std::make_pair("cpuprofile-stop", "cp-stop"),
            std::bind(&CliCommand::CpuProfileCommand, this, "cpuprofile-stop")},
        {std::make_pair("cpuprofile-enable", "cp-enable"),
            std::bind(&CliCommand::CpuProfileCommand, this, "cpuprofile-enable")},
        {std::make_pair("cpuprofile-disable", "cp-disable"),
            std::bind(&CliCommand::CpuProfileCommand, this, "cpuprofile-disable")},
        {std::make_pair("cpuprofile-show", "cp-show"),
            std::bind(&CliCommand::CpuProfileCommand, this, "cpuprofile-show")},
        {std::make_pair("cpuprofile-setSamplingInterval", "cp-ssi"),
            std::bind(&CliCommand::CpuProfileCommand, this, "cpuprofile-setSamplingInterval")},
        {std::make_pair("runtime-enable", "rt-enable"), std::bind(&CliCommand::RuntimeCommand, this, "runtime-enable")},
        {std::make_pair("heapusage", "hu"), std::bind(&CliCommand::RuntimeCommand, this, "heapusage")},
        {std::make_pair("break", "b"), std::bind(&CliCommand::DebuggerCommand, this, "break")},
        {std::make_pair("backtrack", "bt"), std::bind(&CliCommand::DebuggerCommand, this, "backtrack")},
        {std::make_pair("continue", "c"), std::bind(&CliCommand::DebuggerCommand, this, "continue")},
        {std::make_pair("delete", "d"), std::bind(&CliCommand::DebuggerCommand, this, "delete")},
        {std::make_pair("disable", "disable"), std::bind(&CliCommand::DebuggerCommand, this, "disable")},
        {std::make_pair("display", "display"), std::bind(&CliCommand::DebuggerCommand, this, "display")},
        {std::make_pair("enable", "enable"), std::bind(&CliCommand::DebuggerCommand, this, "enable")},
        {std::make_pair("finish", "fin"), std::bind(&CliCommand::DebuggerCommand, this, "finish")},
        {std::make_pair("frame", "f"), std::bind(&CliCommand::DebuggerCommand, this, "frame")},
        {std::make_pair("help", "h"), std::bind(&CliCommand::ExecHelpCommand, this)},
        {std::make_pair("ignore", "ig"), std::bind(&CliCommand::DebuggerCommand, this, "ignore")},
        {std::make_pair("infobreakpoints", "infob"), std::bind(&CliCommand::DebuggerCommand, this, "infobreakpoints")},
        {std::make_pair("infosource", "infos"), std::bind(&CliCommand::DebuggerCommand, this, "infosource")},
        {std::make_pair("jump", "j"), std::bind(&CliCommand::DebuggerCommand, this, "jump")},
        {std::make_pair("list", "l"), std::bind(&CliCommand::DebuggerCommand, this, "list")},
        {std::make_pair("next", "n"), std::bind(&CliCommand::DebuggerCommand, this, "next")},
        {std::make_pair("print", "p"), std::bind(&CliCommand::RuntimeCommand, this, "print")},
        {std::make_pair("print2", "p2"), std::bind(&CliCommand::RuntimeCommand, this, "print2")},
        {std::make_pair("ptype", "ptype"), std::bind(&CliCommand::DebuggerCommand, this, "ptype")},
        {std::make_pair("run", "r"), std::bind(&CliCommand::RuntimeCommand, this, "run")},
        {std::make_pair("setvar", "sv"), std::bind(&CliCommand::DebuggerCommand, this, "setvar")},
        {std::make_pair("step", "s"), std::bind(&CliCommand::DebuggerCommand, this, "step")},
        {std::make_pair("undisplay", "undisplay"), std::bind(&CliCommand::DebuggerCommand, this, "undisplay")},
        {std::make_pair("watch", "wa"), std::bind(&CliCommand::DebuggerCommand, this, "watch")},
        {std::make_pair("resume", "resume"), std::bind(&CliCommand::DebuggerCommand, this, "resume")},
        {std::make_pair("showstack", "ss"), std::bind(&CliCommand::DebuggerCommand, this, "showstack")},
        {std::make_pair("step-into", "si"), std::bind(&CliCommand::DebuggerCommand, this, "step-into")},
        {std::make_pair("step-out", "so"), std::bind(&CliCommand::DebuggerCommand, this, "step-out")},
        {std::make_pair("step-over", "sov"), std::bind(&CliCommand::DebuggerCommand, this, "step-over")},
        {std::make_pair("runtime-disable", "rt-disable"),
            std::bind(&CliCommand::RuntimeCommand, this, "runtime-disable")},
        {std::make_pair("session-new", "session-new"),
            std::bind(&CliCommand::SessionAddCommand, this, "session-new")},
        {std::make_pair("session-remove", "session-remove"),
            std::bind(&CliCommand::SessionDelCommand, this, "session-remove")},
        {std::make_pair("session-list", "session-list"),
            std::bind(&CliCommand::SessionListCommand, this, "session-list")},
        {std::make_pair("session", "session"),
            std::bind(&CliCommand::SessionSwitchCommand, this, "session")},
        {std::make_pair("success", "success"),
            std::bind(&CliCommand::TestCommand, this, "success")},
        {std::make_pair("fail", "fail"),
            std::bind(&CliCommand::TestCommand, this, "fail")}
    };
}

ErrCode CliCommand::HeapProfilerCommand(const std::string &cmd)
{
    std::cout << "exe success, cmd is " << cmd << std::endl;
    Session *session = SessionManager::getInstance().GetSessionById(sessionId_);
    DomainManager &domainManager = session->GetDomainManager();
    HeapProfilerClient &heapProfilerClient = domainManager.GetHeapProfilerClient();
    VecStr argList = GetArgList();
    if (argList.empty()) {
        argList.push_back("/data/");
    }

    bool result = heapProfilerClient.DispatcherCmd(cmd, argList[0]);
    return result ? ErrCode::ERR_OK : ErrCode::ERR_FAIL;
}

ErrCode CliCommand::CpuProfileCommand(const std::string &cmd)
{
    std::cout << "exe success, cmd is " << cmd << std::endl;
    Session *session = SessionManager::getInstance().GetSessionById(sessionId_);
    DomainManager &domainManager = session->GetDomainManager();
    ProfilerClient &profilerClient = domainManager.GetProfilerClient();
    ProfilerSingleton &pro = session->GetProfilerSingleton();
    if (cmd == "cpuprofile-show") {
        pro.ShowCpuFile();
        return ErrCode::ERR_OK;
    }
    if (cmd == "cpuprofile-setSamplingInterval") {
        profilerClient.SetSamplingInterval(std::atoi(GetArgList()[0].c_str()));
    }
    if (cmd == "cpuprofile-stop" && GetArgList().size() == 1) {
        pro.SetAddress(GetArgList()[0]);
    }
    bool result = profilerClient.DispatcherCmd(cmd);
    return result ? ErrCode::ERR_OK : ErrCode::ERR_FAIL;
}

ErrCode CliCommand::HandleDebuggerCommand(const std::string &cmd)
{
    Session *session = SessionManager::getInstance().GetSessionById(sessionId_);
    BreakPointManager &breakpoint = session->GetBreakPointManager();
    SourceManager &sourceManager = session->GetSourceManager();
    WatchManager &watchManager = session->GetWatchManager();
    if (cmd == "display") {
        breakpoint.Show();
        return ErrCode::ERR_OK;
    }

    if (cmd == "infosource") {
        if (GetArgList().size() == 1) {
            sourceManager.GetFileSource(std::atoi(GetArgList()[0].c_str()));
        } else {
            sourceManager.GetFileName();
        }
        return ErrCode::ERR_OK;
    }

    if (cmd == "list" && watchManager.GetDebugState()) {
        if (GetArgList().size() == 1) {
            sourceManager.GetListSource(GetArgList()[0]);
        } else {
            sourceManager.GetListSource("");
        }
        return ErrCode::ERR_OK;
    }

    if (cmd == "watch" && GetArgList().size() == 1) {
        watchManager.AddWatchInfo(GetArgList()[0]);
        if (watchManager.GetDebugState()) {
            watchManager.SendRequestWatch(watchManager.GetWatchInfoSize() - 1, watchManager.GetCallFrameId());
        }
        return ErrCode::ERR_OK;
    }
    return ErrCode::ERR_FAIL;
}

ErrCode CliCommand::DebuggerCommand(const std::string &cmd)
{
    std::cout << "exe success, cmd is " << cmd << std::endl;
    bool result = false;
    Session *session = SessionManager::getInstance().GetSessionById(sessionId_);
    DebuggerClient &debuggerCli = session->GetDomainManager().GetDebuggerClient();
    BreakPointManager &breakpoint = session->GetBreakPointManager();

    if (cmd == "delete") {
        std::string bnumber = GetArgList()[0];
        int tmpNum = std::stoi(bnumber);
        if (tmpNum < 0) {
            LOGE("ardb: the entered sequence number cannot be negativ!");
            return ErrCode::ERR_FAIL;
        }
        size_t num = static_cast<size_t>(tmpNum);
        if (breakpoint.Getbreaklist().size() >= num && num > 0) {
            debuggerCli.AddBreakPointInfo(breakpoint.Getbreaklist()[num - 1].breakpointId, 0); // 1: breakpoinId
            breakpoint.Deletebreaklist(num);
        } else {
            return ErrCode::ERR_FAIL;
        }
    }

    if (cmd == "step-into" || cmd == "step-out" || cmd == "step-over") {
        RuntimeClient &runtimeClient = session->GetDomainManager().GetRuntimeClient();
        runtimeClient.SetIsInitializeTree(true);
    }

    if (cmd == "showstack") {
        StackManager &stackManager = session->GetStackManager();
        stackManager.ShowCallFrames();
    }

    if (cmd == "break" && GetArgList().size() == 2) { // 2: two parameters
        debuggerCli.AddBreakPointInfo(GetArgList()[0], std::stoi(GetArgList()[1]));
    }

    if (HandleDebuggerCommand(cmd) == ErrCode::ERR_OK) {
        return ErrCode::ERR_OK;
    }

    result = debuggerCli.DispatcherCmd(cmd);
    return result ? ErrCode::ERR_OK : ErrCode::ERR_FAIL;
}

ErrCode CliCommand::RuntimeCommand(const std::string &cmd)
{
    std::cout << "exe success, cmd is " << cmd << std::endl;
    bool result = false;
    Session *session = SessionManager::getInstance().GetSessionById(sessionId_);
    RuntimeClient &runtimeClient = session->GetDomainManager().GetRuntimeClient();

    if (cmd == "print" && GetArgList().size() == 1) {
        runtimeClient.SetIsInitializeTree(false);
        VariableManager &variableManager = session->GetVariableManager();
        int32_t objectId = variableManager.FindObjectIdWithIndex(std::stoi(GetArgList()[0]));
        runtimeClient.SetObjectId(std::to_string(objectId));
    }

    result = runtimeClient.DispatcherCmd(cmd);
    if (result) {
        runtimeClient.SetObjectId("0");
    } else {
        return ErrCode::ERR_FAIL;
    }
    return ErrCode::ERR_OK;
}

ErrCode CliCommand::SessionAddCommand([[maybe_unused]] const std::string &cmd)
{
    VecStr argList = GetArgList();
    if (argList.size() >= 1) {
        if (!SessionManager::getInstance().CreateNewSession(argList[0])) {
            std::cout << "session create success" << std::endl;
            return ErrCode::ERR_OK;
        }
    }

    std::cout << "session add failed" << std::endl;
    return ErrCode::ERR_FAIL;
}

ErrCode CliCommand::SessionDelCommand([[maybe_unused]] const std::string &cmd)
{
    VecStr argList = GetArgList();
    if (argList.size() >= 1) {
        uint32_t sessionId = 0;
        if (Utils::StrToUInt(argList[0].c_str(), &sessionId)) {
            if (sessionId == 0) {
                std::cout << "cannot remove default session 0" << std::endl;
                return ErrCode::ERR_OK;
            }
            if (SessionManager::getInstance().DelSessionById(sessionId) == 0) {
                std::cout << "session remove success" << std::endl;
                return ErrCode::ERR_OK;
            }
        }
    }

    return ErrCode::ERR_FAIL;
}

ErrCode CliCommand::SessionListCommand([[maybe_unused]] const std::string &cmd)
{
    SessionManager::getInstance().SessionList();
    return ErrCode::ERR_OK;
}

ErrCode CliCommand::SessionSwitchCommand([[maybe_unused]] const std::string &cmd)
{
    VecStr argList = GetArgList();
    if (argList.size() >= 1) {
        uint32_t sessionId = 0;
        if (Utils::StrToUInt(argList[0].c_str(), &sessionId)) {
            if (SessionManager::getInstance().SessionSwitch(sessionId) == 0) {
                std::cout << "session switch success" << std::endl;
                return ErrCode::ERR_OK;
            }
        }
    }
    return ErrCode::ERR_OK;
}

ErrCode CliCommand::TestCommand(const std::string &cmd)
{
    if (cmd == "success" || cmd == "fail") {
        Session *session = SessionManager::getInstance().GetSessionById(sessionId_);
        TestClient &testClient = session->GetDomainManager().GetTestClient();
        testClient.DispatcherCmd(cmd);
    } else {
        return ErrCode::ERR_FAIL;
    }
    return ErrCode::ERR_OK;
}

ErrCode CliCommand::ExecHelpCommand()
{
    std::cout << HELP_MSG;
    return ErrCode::ERR_OK;
}

ErrCode CliCommand::OnCommand()
{
    std::map<StrPair, std::function<ErrCode()>>::iterator it;
    StrPair cmdPair;
    bool haveCmdFlag = false;

    for (it = commandMap_.begin(); it != commandMap_.end(); it++) {
        cmdPair = it->first;
        if (!strcmp(cmdPair.first.c_str(), cmd_.c_str())
            ||!strcmp(cmdPair.second.c_str(), cmd_.c_str())) {
            auto respond = it->second;
            return respond();
        }
    }

    for (unsigned int i = 0; i < cmdList.size(); i++) {
        if (!strncmp(cmdList[i].c_str(), cmd_.c_str(), std::strlen(cmd_.c_str()))) {
            haveCmdFlag = true;
            std::cout << cmdList[i] << " ";
        }
    }

    if (haveCmdFlag) {
        std::cout << std::endl;
    } else {
        ExecHelpCommand();
    }

    return ErrCode::ERR_FAIL;
}
} // namespace OHOS::ArkCompiler::Toolchain
