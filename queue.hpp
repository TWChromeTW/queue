#pragma once
#ifndef __QUEUE_HPP__
#define __QUEUE_HPP__
#include <iostream>
#include <exception>
#include <string>

template <class T>
class Queue
{
public:
    virtual ~Queue() {};
    virtual void enQueue(const T& element) = 0;
    virtual T deQueue() = 0;
    virtual bool isEmpty() = 0;
};

template <class T>
class QueueRing: public Queue<T>
{
public:
    static constexpr std::size_t MAX_STACK_SIZE = 100'000'000;

    QueueRing(std::size_t size = 100);
    QueueRing(const QueueRing<T>& src);
    QueueRing(QueueRing<T>&& src);

    QueueRing& operator=(const QueueRing<T>& src);
    QueueRing& operator=(QueueRing<T>&& src);

    virtual ~QueueRing() override;

    void enQueue(const T& element) override;
    T deQueue() override;

    bool isEmpty();
    bool isFull();

    template <class U>
    friend std::ostream& operator<<(std::ostream& out, QueueRing<U>& queue);

    template <class U>
    friend void swap(QueueRing<U>& first, QueueRing<U>& second);

private:
    T* array_;
    std::size_t head_;
    std::size_t tail_;
    std::size_t size_;
};

template <class T>
class QueueOverflow: public std::exception
{
public:
    QueueOverflow() : reason_("QueueOverflow") {}
    const char* what() const noexcept override { return reason_.c_str(); }

private:
    std::string reason_;
};

template <class T>
class WrongQueueSize: public std::exception
{
public:
    WrongQueueSize(std::string reason) : reason_(reason) {}
    const char* what() const noexcept override { return reason_.c_str(); }
private:
    std::string reason_;
};

template <class T>
QueueRing<T>:: QueueRing(std::size_t size):
    head_(0),
    tail_(0),
    size_(size)
{
    if (size > MAX_STACK_SIZE)
    {
        throw WrongQueueSize<T>("Maximum size reached");
    }

    try
    {
        array_ = new T[size_ + 1];
    }
    catch(...)
    {
        throw WrongQueueSize<T>("Memory allocation failed");
    }
}

template <class T>
QueueRing<T>:: QueueRing(const QueueRing<T>& src) :
    head_(src.head_),
    tail_(src.tail_),
    size_(src.size_)
{
    array_ = new T[size_ + 1];

    std::size_t i = src.head_;

    do
    {
        array_[i] = src.array_[i];
        i = (i + 1) % src.size_;
    }
    while (i != (src.tail_ + 1) % src.size_);
}

template <class T>
QueueRing<T>:: QueueRing(QueueRing<T>&& src) :
    array_(src.array_),
    head_(src.head_),
    tail_(src.tail_),
    size_(src.size_)
{
    src.array_ = nullptr;
    src.head_ = 0;
    src.tail_ = 0;
    src.size_ = 0;
}

template <class T>
QueueRing<T>& QueueRing<T>:: operator=(const QueueRing<T>&src)
{
    if (this != &src)
    {
        QueueRing<T> tmp(src);
        swap(*this, tmp);
    }

    return *this;
}

template <class T>
QueueRing<T>& QueueRing<T>:: operator=(QueueRing<T>&&src)
{
    if (this != &src)
    {
        delete[] array_;
        array_ = src.array_;
        head_ = src.head_;
        tail_ = src.tail_;
        size_ = src.size_;

        src.array_ = nullptr;
        src.head_ = 0;
        src.tail_ = 0;
        src.size_ = 0;
    }

    return *this;
}

template <class T>
QueueRing<T>:: ~QueueRing()
{
    delete[] array_;
}

template <class T>
void QueueRing<T>:: enQueue(const T& element)
{
    if (isFull())
    {
        throw QueueOverflow<T>();
    }
    else
    {
        if (head_ == 0) head_ = 1;
        tail_ = (tail_ + 1) % size_;
        array_[tail_] = element;
    }
}

template <class T>
T QueueRing<T>:: deQueue()
{
    if (isEmpty())
    {
        throw WrongQueueSize<T>("Empty queue");
    }
    else
    {
        T tmp = array_[head_];

        if (head_ == tail_)
        {
            head_ = 0;
            tail_ = 0;
        }
        else
        {
            head_ = (head_ + 1) % size_;
        }

        return tmp;
    }
}

template <class T>
bool QueueRing<T>::isEmpty()
{
    return head_ == 0;
}

template <class T>
bool QueueRing<T>::isFull()
{
    if (head_ == 0 && tail_ == (size_ - 1)) return true;
    if (head_ == (tail_ + 1)) return true;

    return false;
}

template<class U>
std::ostream& operator<<(std::ostream& out, QueueRing<U>& queue)
{
    out << '\n' << "Elements of QueueRing:" << '\n';

    if (!queue.isEmpty())
    {
        std::size_t i = queue.head_;

        do
        {
            out << queue.array_[i] << '\n';
            i = (i + 1) % queue.size_;
        }
        while (i != (queue.tail_ + 1) % queue.size_);
    }

    out << "End of queue's elements" << '\n';

    return out;
}

template <class U>
void swap(QueueRing<U>& first, QueueRing<U>& second)
{
    std::swap(first.array_, second.array_);
    std::swap(first.head_, second.head_);
    std::swap(first.tail_, second.tail_);
    std::swap(first.size_, second.size_);
}

#endif // !__QUEUE_HPP