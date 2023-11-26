# TinyMuduo - C++11

## 开发环境
- 编译器：g++ 4.8.5
- 项目构建：CMake版本2.8.12.2
- 操作系统：CentOS 7（内核版本：3.10.0-1160.71.1.el7.x86_64）

## 介绍
本项目参考于Muduo网络库实现的基于Reactor模式的多线程网络库；移除原本Muduo库中对Boost库的依赖，原本的相应功能使用C++11标准库代为实现；测试用例暂时只编写了echo服务器的内容，可使用telnet或者ncat进行测试。

## 组件

### Channel
- 相当于Reactor模式中的事件组件。
- 封装文件描述符（fd），设置感兴趣的事件，并将其注册到相应的EventLoop的EpollPoller上。
- 在TcpConnection中为与连接的客户端通信设置一个Channel，并设置相应的回调函数。当Poller通知Channel发生了感兴趣的事件时，Channel调用相应的回调函数，类似于Reactor模式中的事件处理器。
- Channel的fd可分为：依赖于Acceptor的wakeupfd和用于与连接客户端通信的fd。

### Poller/EpollPoller
- 相当于Reactor模式中的事件分发器组件。
- 在Poller中，通过fd可以找到对应的Channel。

### EventLoop
- 相当于Reactor模式中的反应堆组件。
- 有一个向量用于记录所有活跃的Channel。
- 记录wakeupfd，当主EventLoop获得新用户连接的Channel后，通过轮询选择唤醒一个子loop，处理Channel。
- EventLoop管理多个Channel和一个Poller。当Channel需要注册到Poller上或更改感兴趣的事件时，通过EventLoop获取对应Poller对象的unique_ptr进行操作。
- 在多线程环境中，需要保证每个EventLoop对象都在创建它的线程中运行，以确保EventLoop是线程安全的。
- EventLoop内的回调函数需要在当前loop对应的线程执行。如果非IO线程，则需要通过EventLoop::runInLoop()函数将任务添加到任务队列，并唤醒IO线程来执行任务。

### Thread
- 底层线程，一个Thread对应一个loop。

### EventLoopThread
- 事件线程，每一个线程创建一个独立的EventLoop（one thread one loop）。

### EventLoopThreadPool
- 事件循环的线程池。
- 池内不同的子loop通过轮询算法进行选择获取。
- 如果没有设置多线程，则只保存了mainLoop，也就是主线程创建的EventLoop对象的引用。

### Acceptor
- 完成网络编程的基础部分，包括socket、bind、listen。
- 将listenfd封装在Channel后，交给mainLoop，监听可读事件（新连接事件）。
- 当客户端请求连接时，监听套接字变为可读，Acceptor会执行回调操作：根据轮询算法选择一个subloop，唤醒subloop，将当前connfd封装成Channel，注册相应的回调函数，分发给subloop。

### Buffer
- 双缓冲区，写数据->缓冲区->TCP发送缓冲区->send。
- 每个TcpConnection都有自己的Buffer，并且每个TcpConnection只会在其所属的IO线程中被操作，避免多个线程同时操作同一个Buffer。
- Buffer类使用连续的内存块来存储数据，提高内存利用率，减少内存碎片。Buffer类的大小可以自动增长，适应不同大小的消息。

### TcpConnection
- 一个成功连接的客户端对应一个TcpConnection。
- 封装了socket、Channel、相应的回调函数、input和output缓冲区。
- 主要负责封装一次TCP连接，向Channel注册回调函数（可读、可写、可关闭、错误处理）。
- 当Channel上的事件发生时，调用相应的回调函数进行数据收发或错误处理。

### TcpServer
- 封装了Acceptor、EventLoopThreadPool，记录所有已连接对象的指针。
- 创建TcpConnection连接：当有新连接到达时，先调用系统函数accept，再回调函数newConnectionCallback_让TcpServer去创建连接。
- 处理TcpConnection连接：TcpConnection类主要负责封装一次TCP连接，向Channel类注册回调函数（可读、可写、可关闭、错误处理）。
- 关闭TcpConnection连接：当TCP连接关闭时，调用的回调函数，由TcpServer设置，用于TcpServer将要关闭的TcpConnection从保存着所有TcpConnection的map中删除。

## 关于Reactor模型
来自 wikipedia 的 Reactor Pattern 的说明：
The reactor design pattern is an event handling pattern for handling service requests delivered concurrently to a service handler by one or more inputs. The service handler then demultiplexes the incoming requests and dispatches them synchronously to the associated request handlers.

## 大致说明
已实现三个核心组件：Channel类、EpollPoller类、EventLoop类；用于支撑一个Reactor实现持续的监听一组fd，并根据每个fd上发生的事件调用相应的回调处理函数。

使用者仅需先构造TcpServer对象，设置回调，设置线程数量，开启start后开启主线程loop；TcpServer构造函数创建了Acceptor；Acceptor构造函数创建了非阻塞的listenfd，封装为Channel交给mainLoop的Poller，（只关注读事件）；Acceptor对应的读事件的回调函数正是将新连接分装为TcpConnection（该回调函数为TcpServer构造函数内设置的），设置的回调函数其实就是TcpServer::newConnection。

服务器在处理客户端连接请求时，为了不阻塞在accept函数上，会将监听fd注册到Epoll中，当客户端请求连接时，监听套接字变为可读，Acceptor会执行这个回调操作：根据轮询算法选择一个subloop，唤醒subloop，把当前connfd封装成Channel，Channel注册相应的回调函数，分发给subloop。创建TcpConnection连接：当有新连接到达时，先调用系统函数accept，再回调函数newConnectionCallback_让TcpServer去创建连接。这个回调函数是TcpServer中的newConnection，用于创建一个TcpConnection连接。处理TcpConnection连接：TcpConnection类主要负责封装一次TCP连接，向Channel类注册回调函数（可读、可写、可关闭、错误处理）。关闭TcpConnection连接：当TCP连接关闭时，调用的回调函数，由TcpServer设置，用于TcpServer将这个要关闭的TcpConnection从保存着所有TcpConnection的map中删除。

## 小坑
- wakeupChannel的初始化依赖于wakeupFd作为fd传入channel的ctor。
- 成员变量的初始化顺序取决于它们在类定义中的声明顺序，而非在成员初始化列表中的顺序。
- 若wakeupFd在wakeupChannel之前未被正确初始化，虽不会引起编译错误，但会导致程序运行时错误。
- 因此，应该需要先声明wakeupFd，再声明wakeupChannel：

```cpp
/net/EventLoop.hpp
...
line61	int wakeupFd_;
line62	std::unique_ptr wakeupChannel_;
...