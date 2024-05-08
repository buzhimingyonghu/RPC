# RPC 项目文档

## 项目介绍

这个项目是一个RPC（Remote Procedure Call，远程过程调用）框架，采用了以下技术：

- 线程池
- 异步高并发
- 主从 Reactor 架构
- 底层采用 epoll 实现 IO 多路复用
- 应用层采用 protobuf 自定义 rpc 通信协议
- 异步日志，支持同步和异步模式，可以写入文件或直接输出
- 定时器，用于防止程序崩坏超时，防止阻塞，从而避免长时间等待。

## 本项目需要以下依赖：

Protobuf 3.21.12
TinyXML
CMake
## 安装

# RPC 项目文档

## 安装

1. 在 `build` 目录下执行以下命令进行项目编译：

    ```bash
    $ cmake ..
    $ make
    ```

2. 执行 `install.sh` 安装RPC所需的依赖：

    ```bash
    $ ./install.sh
    ```

## 测试

1. 用户提供自定义服务的 `.proto` 文件。

2. 使用 `makeService` Python脚本构建服务的执行环境：

    ```bash
    $ python3 RPC/makeService/makeService.py -i {servicefile}.proto -o [installpath]
    ```

3. 继续执行服务中的CMake和编译：

    ```bash
    $ cd [installpath]
    $ cmake .
    $ make
    ```

4. 执行 `bin` 目录下的 `start.sh` 开启服务：

    ```bash
    $ ./start.sh
    ```

## 测试客户端

在 `test_client` 目录下提供了客户端测试程序，可以使用该程序测试RPC服务的功能。

