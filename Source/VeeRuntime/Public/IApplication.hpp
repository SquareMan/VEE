//    Copyright 2025 Steven Casper
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.


#pragma once

namespace vee {
class Engine;
class Renderer;
class IApplication {
public:
    IApplication() = default;
    IApplication(const IApplication&) = delete;
    IApplication(IApplication&&) = delete;
    IApplication& operator=(const IApplication&) = delete;
    IApplication& operator=(IApplication&&) = delete;
    virtual ~IApplication() = default;

    virtual void run() = 0;
    virtual Engine& get_engine() = 0;
    virtual Renderer& get_renderer() = 0;
};
} // namespace vee
