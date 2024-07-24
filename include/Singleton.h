#ifndef __KX_SINGLETON_H__
#define __KX_SINGLETON_H__

/**
 * 类Singleton：单例模式模板类，派生类需要将此基类设置为友元，以保证基类可以访问派生类的构造函数
 */

#include <memory>
#include <mutex>
#include <iostream>

template <typename T>
class Singleton
{
public:
    static T *GetInstance()
    {
        static T instance; // C++11后，静态局部变量都是线程安全的
        return &instance;
    }

protected:
    Singleton() = default;                               // 为了让子类有构造函数，模板单例类的构造函数不能删除
    Singleton(const Singleton<T> &) = delete;            // 删除拷贝构造函数
    Singleton &operator=(const Singleton<T> &) = delete; // 删除赋值运算符函数
};

#endif // SINGLETON_H