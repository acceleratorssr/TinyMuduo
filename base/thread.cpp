#include "thread.hpp"
#include "currentThread.hpp"

std::atomic_int Thread::numCreated_(0);

Thread::Thread(ThreadFunc func, const std::string &name)
    : started_(false),
      joined_(false),
      tid_(0),
      func_(std::move(func)),
      name_(name)
{
    setDefaultName();
}

Thread::~Thread()
{
    if (started_ && !joined_)
    {
        thread_->detach();
    }
}

void Thread::start()
{
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    std::condition_variable ready;
    std::atomic_bool tidready(false);
    started_ = true;
    thread_ = std::shared_ptr<std::thread>(new std::thread([&]()
                                                           {
        tid_= CurrentThread::GetTid();
        {
            std::unique_lock<std::mutex> lock(mutex);
            tidready = true;
            ready.notify_all();
        }
        func_(); }));

    ready.wait(lock, [&tidready]()
               { return tidready.load(); });
}

void Thread::join()
{
    joined_ = true;
    thread_->join();
}

void Thread::setDefaultName()
{
    numCreated_++;
    int num = numCreated_;
    if (name_.empty())
    {
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "Thread%d", num);
        name_ = buf;
    }
}
