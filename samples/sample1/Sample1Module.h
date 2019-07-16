/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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
 *
 */

#pragma once

#include "interface/AFIPlugin.h"
#include "interface/AFIPluginManager.h"
#include "utility/interface/AFITimerModule.h"
#include "log/interface/AFILogModule.h"
#include "utility/interface/AFIScheduleModule.h"
#include "utility/interface/AFIGUIDModule.h"

namespace ark {

class AFCHttpClient;

class Sample1Module : public AFIModule
{
public:
    explicit Sample1Module() = default;

    bool Init() override;
    bool PostInit() override;
    bool Update() override;
    bool PreShut() override;
    bool Shut() override;

protected:
    void TestTimer(const std::string& name, const AFGUID& entity_id);
    void TestOssLog();
    // bool TestSchduler(const int id, const int arg);

protected:
    AFITimerModule* m_pTimerModule;
    AFILogModule* m_pLogModule;
    AFIGUIDModule* m_pGUIDModule;
    AFIScheduleModule* m_pScheduleModule;

    AFCHttpClient* http_client{nullptr};
};

} // namespace ark
