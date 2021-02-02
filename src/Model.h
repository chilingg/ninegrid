#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <atomic>
#include <functional>
#include <RThread.h>
#include <RSize.h>

#include "Meta.h"

template<typename Value>
class Model
{
    using RuleFunc = Value(*)(Value, Value, Value, Value, Value, Value, Value, Value, Value);
    using DisplayFunc = Redopera::RColor(*)(Value);

public:
    const unsigned THREAD_NUM = std::thread::hardware_concurrency() > 1 ? std::thread::hardware_concurrency() - 1 : 0;

    const size_t WIDTH;
    const size_t HEIGHT;

    Model(size_t width, size_t height, RuleFunc rFunc = Meta::lifeGameRule, DisplayFunc dFunc = Meta::explicitDisplay);
    ~Model();

    Value value(size_t x, size_t y); //依据坐标返回单个数据
    Redopera::RSize size() const { return Redopera::RSize(WIDTH, HEIGHT); }

    Redopera::RColor display(size_t x, size_t y) { return displayFunc(value(x, y)); } ;

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
    RuleFunc ruleFunc;
    DisplayFunc displayFunc;
    std::vector<Redopera::RThread> threads_;
};

template<typename Value>
inline Model<Value>::Model(size_t width, size_t height, RuleFunc rFunc, DisplayFunc dFunc):
    WIDTH(width),
    HEIGHT(height),
    cModel_(new Value[WIDTH * HEIGHT]),
    tModel_(new Value[WIDTH * HEIGHT]),
    updateIndex_(HEIGHT),
    ruleFunc(rFunc),
    displayFunc(dFunc)
{
    std::fill_n(cModel_, WIDTH * HEIGHT, 0);
    std::fill_n(tModel_, WIDTH * HEIGHT, 0);

    threads_.reserve(THREAD_NUM);
}

template<typename Value>
inline Model<Value>::~Model()
{
    flush();

    delete [] cModel_;
    delete [] tModel_;
}

template<typename Value>
inline Value Model<Value>::value(size_t x, size_t y)
{
    return cModel_[y * WIDTH + x];
}

template<typename Value>
inline void Model<Value>::update()
{
    flush();
    std::swap(cModel_, tModel_);

    updateIndex_ = 0;
    for(unsigned i = 0; i < THREAD_NUM; ++i)
    {
        threads_.emplace_back([this]{ updateInAThread(); });
    }
}

template<typename Value>
inline void Model<Value>::updateDirect()
{
    std::swap(cModel_, tModel_);

    for(size_t i = 0; i < HEIGHT; ++i)
    {
        size_t y = i * WIDTH;
        for(size_t x = 0; x < WIDTH; ++x)
        {
            size_t b = i == 0 ? HEIGHT * WIDTH - WIDTH: i * WIDTH - WIDTH;
            size_t t = i == HEIGHT - 1 ? 0 : i * WIDTH + WIDTH;

            size_t l = x == 0 ? WIDTH - 1 : x - 1;
            size_t r = x == WIDTH - 1 ? 0 : x + 1;

            tModel_[y + x] = ruleFunc(cModel_[t + l], cModel_[t + x], cModel_[t + r],
                    cModel_[y + l], cModel_[y + x], cModel_[y + r],
                    cModel_[b + l], cModel_[b + x], cModel_[b + r]);
        }
    }
}

template<typename Value>
inline void Model<Value>::flush()
{
    updateInAThread();
    threads_.clear();
}

template<typename Value>
inline void Model<Value>::setValue(size_t x, size_t y, Value value)
{
    flush();
    tModel_[x + y * WIDTH] = value;
}

template<typename Value>
inline void Model<Value>::setRangeValue(size_t x, size_t y, size_t widht, size_t height, Value value)
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

template<typename Value>
inline void Model<Value>::fill(Value value)
{
    flush();
    std::fill_n(tModel_, WIDTH * HEIGHT, value);
}

template<typename Value>
inline void Model<Value>::clear()
{
    flush();
    std::fill_n(tModel_, WIDTH * HEIGHT, 0);
}

template<typename Value>
inline void Model<Value>::updateInAThread()
{
    for(size_t i = updateIndex_++; i < HEIGHT; i = updateIndex_++)
    {
        size_t y = i * WIDTH;
        for(size_t x = 0; x < WIDTH; ++x)
        {
            size_t b = i == 0 ? HEIGHT * WIDTH - WIDTH: i * WIDTH - WIDTH;
            size_t t = i == HEIGHT - 1 ? 0 : i * WIDTH + WIDTH;

            size_t l = x == 0 ? WIDTH - 1 : x - 1;
            size_t r = x == WIDTH - 1 ? 0 : x + 1;

            tModel_[y + x] = ruleFunc(cModel_[t + l], cModel_[t + x], cModel_[t + r],
                    cModel_[y + l], cModel_[y + x], cModel_[y + r],
                    cModel_[b + l], cModel_[b + x], cModel_[b + r]);
        }
    }
}

#endif // MODEL_H
