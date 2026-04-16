# ConcurrentTCPServer

ConcurrentTCPServer 是一个基于 One Thread One Loop Reactor 模型实现的高并发 TCP 服务器项目。项目从底层网络事件驱动、连接生命周期管理、定时器、日志系统，到上层的 HTTP 服务与测试体系，形成了一套完整的可运行、可测试、可发布的服务端工程。

## 项目目标

这个仓库主要用于验证和展示以下能力：

1. 基于 epoll、eventfd、timerfd 的 Reactor 网络模型。
2. 连接与定时任务解耦的高并发 TCP 服务框架。
3. 可独立安装和复用的基础库发布能力。
4. HTTP 服务的解析、超时控制与回归测试。
5. 可通过 CMake 构建、安装和测试的完整工程化流程。

## 架构组成

项目按功能分为几个主要部分：

1. CTS
	这是核心网络库，负责事件循环、连接管理、时间轮、socket 封装和 TCP 服务调度。上层服务通常直接基于 CTS 构建。

2. Logger
	独立日志库，提供日志级别控制、控制台输出和文件输出能力。它可以单独安装，也可以作为 CTS 的依赖被复用。

3. HttpServer
	基于 CTS 构建的 HTTP 服务实现，包含请求解析、响应构造、静态资源处理、请求体超时控制和测试入口。

4. EchoServer
	示例回显服务，保留为演示用途，但默认不参与安装发布。

5. tests
	历史测试目录，当前以 HttpServer/test 下的 CMake/CTest 测试入口为主。

## 技术栈

- C++17
- CMake 3.28.3
- Unix Makefiles
- Linux
- epoll
- eventfd
- timerfd
- pthread

## 目录说明

- CTS：核心 TCP 框架与对外接口
- Logger：日志库
- HttpServer：HTTP 服务实现与测试
- EchoServer：示例服务
- tests：旧版测试和手工验证用例

## 构建

项目使用 CMake 构建。推荐使用单独的 build 目录：

```bash
cmake -S . -B build
cmake --build build -j2
```

如果你希望只构建核心库和 HTTP 测试，当前默认配置已经足够。EchoServer 默认不参与构建和安装。

## 安装

当前发布版支持将 Logger 和 CTS 安装到指定前缀。安装内容包括：

- 动静态库
- 公开头文件
- CMake 包配置文件和导出目标文件

示例安装命令：

```bash
cmake -S . -B build
cmake --build build -j2
cmake --install build --prefix /usr/local
```

安装后，相关文件会放到下面这些位置：

- /usr/local/lib/libLogger.so
- /usr/local/lib/libLogger.a
- /usr/local/lib/libCTS.so
- /usr/local/lib/libCTS.a
- /usr/local/include/Logger/
- /usr/local/include/CTS/
- /usr/local/lib/cmake/Logger/
- /usr/local/lib/cmake/CTS/

## 外部工程使用方式

安装完成后，外部 CMake 工程可以通过 package config 直接复用这两个库。CTS 依赖 Logger，因此建议按下面顺序查找：

```cmake
find_package(Logger CONFIG REQUIRED)
find_package(CTS CONFIG REQUIRED)

target_link_libraries(your_target PRIVATE
	 CTS::CTS_shared
	 Logger::Logger_shared
)
```

如果你更倾向于静态链接，也可以改用 CTS::CTS_static 和 Logger::Logger_static。

## 测试

HttpServer 的 C++ 测试已经接入 CMake/CTest。常用方式如下：

```bash
cmake -S . -B build
cmake --build build -j2
cmake --build build --target httpserver_run_cpp
ctest --test-dir build
```

测试套件使用独立端口运行，并会自动启动测试服务端再顺序执行各个用例。

## 说明

1. CTS 是项目的核心发布产物，Logger 也是独立可安装库。
2. EchoServer 和 HttpServer 不参与安装。
3. 若你要做二次开发，优先从 CTS 和 Logger 的安装包入口接入。
4. 若你只关心服务端验证，直接运行 HttpServer 的 CTest 套件即可。
