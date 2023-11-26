#ifndef _NONCOPYABLE_HPP_
#define _NONCOPYABLE_HPP_

class noncopyable
{
public:
    noncopyable(const noncopyable &) = delete;
    void operator=(const noncopyable &) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

#endif // _NONCOPYABLE_HPP_
