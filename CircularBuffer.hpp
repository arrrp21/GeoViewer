#pragma once
#include <array>

template <typename T, std::size_t maxSize = 20>
class CircularBuffer
{
public:
    CircularBuffer()
    {
    }

    void reset();
    void add(T& value)
    {
        if (not isEmpty())
        {
            m_head = (m_head + 1) % maxSize;
        }
        values[m_head] = value;

        if (m_head > m_size)
        {
            values[0] = value;
            m_head = 0;
        }

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

    T& tail()
    {
        return values[m_tail];
    }

    bool isFull() const
    {
        return m_size == maxSize;
    }

    bool isEmpty() const
    {
        return m_size == 0;
    }

private:
    std::array<T, maxSize> values;
    std::size_t m_head{0};
    std::size_t m_tail{0};
    std::size_t m_size{0};
};

