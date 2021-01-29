#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <atomic>
#include <functional>
#include <RThread.h>
#include <RSize.h>

#include "meta.h"

template<typename Value,
         Value(*meteFunc)(Value,Value,Value,Value,Value,Value,Value,Value,Value) = Meta::lifeGameRule>
class Model
{
public:
    const unsigned THREAD_NUM = std::thread::hardware_concurrency() > 1 ? std::thread::hardware_concurrency() - 1 : 0;

    const size_t WIDTH;
    const size_t HEIGHT;

    Model(size_t width, size_t height);
    ~Model();

    Value value(size_t x, size_t y); //依据坐标返回单个数据
    Redopera::RSize size() const { return { WIDTH, HEIGHT }; }

    void update(); //更新数据
    [[deprecated]] void updateDirect(); // 仅测试用
    void flush();

    void setValue(size_t x, size_t y, Value value);
    void setRangeValue(size_t x, size_t y, size_t widht, size_t height, Value value);

    void fill(Value value);
    void clear();

private:
    void updateInAThread();

    Value(* cModel_);
    Value(* tModel_);

    std::atomic_size_t updateIndex_;
    Value(*func_)(Value, Value, Value, Value, Value, Value, Value, Value, Value);
    std::vector<Redopera::RThread> threads_;
};

template<typename Value, Value (*meteFunc)(Value, Value, Value, Value, Value, Value, Value, Value, Value)>
inline Model<Value, meteFunc>::Model(size_t width, size_t height):
    WIDTH(width),
    HEIGHT(height),
    cModel_(new int[WIDTH * HEIGHT]),
    tModel_(new int[WIDTH * HEIGHT]),
    updateIndex_(HEIGHT),
    func_(meteFunc)
{
    std::fill_n(cModel_, WIDTH * HEIGHT, 0);
    std::fill_n(tModel_, WIDTH * HEIGHT, 0);

    threads_.reserve(THREAD_NUM);
}

template<typename Value, Value (*meteFunc)(Value, Value, Value, Value, Value, Value, Value, Value, Value)>
inline Model<Value, meteFunc>::~Model()
{
    flush();

    delete [] cModel_;
    delete [] tModel_;
}

template<typename Value, Value (*meteFunc)(Value, Value, Value, Value, Value, Value, Value, Value, Value)>
inline Value Model<Value, meteFunc>::value(size_t x, size_t y)
{
    return cModel_[y * WIDTH + x];
}

template<typename Value, Value (*meteFunc)(Value, Value, Value, Value, Value, Value, Value, Value, Value)>
inline void Model<Value, meteFunc>::update()
{
    flush();
    std::swap(cModel_, tModel_);

    updateIndex_ = 0;
    for(unsigned i = 0; i < THREAD_NUM; ++i)
    {
        threads_.emplace_back([this]{ updateInAThread(); });
    }
}

template<typename Value, Value (*meteFunc)(Value, Value, Value, Value, Value, Value, Value, Value, Value)>
inline void Model<Value, meteFunc>::updateDirect()
{
    for(size_t i = 0; i < HEIGHT; ++i)
    {
        size_t y = i * WIDTH;
        for(size_t x = 0; x < WIDTH; ++x)
        {
            size_t t = (i + HEIGHT - 1) % HEIGHT * WIDTH;
            size_t b = (i + 1) % HEIGHT * WIDTH;

            size_t l = (x + WIDTH - 1) % WIDTH;
            size_t r = (x + 1) % WIDTH;

            tModel_[y + x] = func_(cModel_[t + l], cModel_[t + x], cModel_[t + r],
                    cModel_[y + l], cModel_[y + x], cModel_[y + r],
                    cModel_[b + l], cModel_[b + x], cModel_[b + r]);
        }
    }

    std::swap(cModel_, tModel_);
}

template<typename Value, Value (*meteFunc)(Value, Value, Value, Value, Value, Value, Value, Value, Value)>
inline void Model<Value, meteFunc>::flush()
{
    updateInAThread();
    threads_.clear();
}

template<typename Value, Value (*meteFunc)(Value, Value, Value, Value, Value, Value, Value, Value, Value)>
inline void Model<Value, meteFunc>::setValue(size_t x, size_t y, Value value)
{
    flush();
    tModel_[x + y * WIDTH] = value;
}

template<typename Value, Value (*meteFunc)(Value, Value, Value, Value, Value, Value, Value, Value, Value)>
inline void Model<Value, meteFunc>::setRangeValue(size_t x, size_t y, size_t widht, size_t height, Value value)
{
# ifndef NDEBUG
    if(x + widht > WIDTH || y + height > HEIGHT)
        throw "Invalid range!";
#endif
    flush();
    for(size_t i = y; i < height + y; ++i)
    {
        for(size_t j = x; j < widht + x; ++j)
        {
            tModel_[i * WIDTH + j] = value;
        }
    }
}

template<typename Value, Value (*meteFunc)(Value, Value, Value, Value, Value, Value, Value, Value, Value)>
inline void Model<Value, meteFunc>::fill(Value value)
{
    flush();
    std::fill_n(tModel_, WIDTH * HEIGHT, value);
}

template<typename Value, Value (*meteFunc)(Value, Value, Value, Value, Value, Value, Value, Value, Value)>
inline void Model<Value, meteFunc>::clear()
{
    flush();
    std::fill_n(tModel_, WIDTH * HEIGHT, 0);
}

template<typename Value, Value (*meteFunc)(Value, Value, Value, Value, Value, Value, Value, Value, Value)>
inline void Model<Value, meteFunc>::updateInAThread()
{
    for(size_t i = updateIndex_++; i < HEIGHT; i = updateIndex_++)
    {
        size_t y = i * WIDTH;
        for(size_t x = 0; x < WIDTH; ++x)
        {
            size_t t = i == 0 ? HEIGHT * WIDTH - WIDTH: i * WIDTH - WIDTH;
            size_t b = i == HEIGHT - 1 ? 0 : i * WIDTH + WIDTH;

            size_t l = x == 0 ? WIDTH - 1 : x - 1;
            size_t r = x == WIDTH - 1 ? 0 : x + 1;

            tModel_[y + x] = func_(cModel_[t + l], cModel_[t + x], cModel_[t + r],
                    cModel_[y + l], cModel_[y + x], cModel_[y + r],
                    cModel_[b + l], cModel_[b + x], cModel_[b + r]);
        }
    }
}

#endif // MODEL_H
