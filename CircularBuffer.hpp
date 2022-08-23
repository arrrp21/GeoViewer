#pragma once
#include <array>

template <typename T, std::size_t maxSize = 20>
class CircularBuffer
{
public:
    CircularBuffer() {}

    void reset()
    {
        m_size = m_head = m_tail = 0;
    }

    void add(T&& value)
    {
        if (not isEmpty())
        {
            m_head = (m_head + 1) % maxSize;
        }
        values[m_head] = value;

        if (isFull())
        {
            m_tail = (m_tail + 1) % maxSize;
        }
        else
        {
            m_size++;
        }
    }

    T& head()
    {
        return values[m_head];
    }

    T& at(std::size_t pos)
    {
        if (pos >= m_size)
        {
            return values[m_tail];
        }
        std::size_t p = (m_head - pos) % maxSize;
        return values[p];
    }

    T& tail()
    {
        return values[m_tail];
    }

    void removeHead(std::size_t numOfElements = 1)
    {
        if (numOfElements == 0)
        {
            return;
        }

        if (numOfElements >= m_size)
        {
            reset();
            return;
        }

        m_head = (m_head - numOfElements) % maxSize;
        m_size -= numOfElements;
    }

    bool isFull() const { return m_size == maxSize; }
    bool isEmpty() const { return m_size == 0; }
    std::size_t size() const { return m_size; }

private:
    std::array<T, maxSize> values;
    std::size_t m_head{0};
    std::size_t m_tail{0};
    std::size_t m_size{0};
};

