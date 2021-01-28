#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <atomic>
#include <functional>
#include <RThread.h>

#include "meta.h"

template<typename Value, size_t _WIDTH_, size_t _HEIGHT_ = _WIDTH_,
         Value(*meteFunc)(Value,Value,Value,Value,Value,Value,Value,Value,Value) = Meta::lifeGameRule>
class Model
{
public:
    const size_t WIDTH =_WIDTH_;
    const size_t HEIGHT =_HEIGHT_;
    const unsigned THREAD_NUM = std::thread::hardware_concurrency() > 1 ? std::thread::hardware_concurrency() - 1 : 0;

    Model():
        cModel_(new int[_WIDTH_ * _HEIGHT_]),
        tModel_(new int[_WIDTH_ * _HEIGHT_]),
        updateIndex_(_HEIGHT_),
        func_(meteFunc)
    {
        std::fill_n(cModel_, _WIDTH_ * _HEIGHT_, 0);
        std::fill_n(tModel_, _WIDTH_ * _HEIGHT_, 0);

        threads_.reserve(THREAD_NUM);
    }

    ~Model()
    {
        delete [] cModel_;
        delete [] tModel_;
    }

    Value value(size_t x, size_t y) //依据坐标返回单个数据
    {
        return cModel_[y * _WIDTH_ + x];
    }

    void update() //更新数据
    {
        flush();

        updateIndex_ = 0;
        for(unsigned i = 0; i < THREAD_NUM; ++i)
        {
            threads_.emplace_back([this]{ updateInAThread(); });
        }
    }

    [[deprecated]] void updateDirect() // 仅测试用
    {
        for(size_t i = 0; i < _HEIGHT_; ++i)
        {
            size_t y = i * _WIDTH_;
            for(size_t x = 0; x < _WIDTH_; ++x)
            {
                size_t t = (i + _HEIGHT_ - 1) % _HEIGHT_ * _WIDTH_;
                size_t b = (i + 1) % _HEIGHT_ * _WIDTH_;

                size_t l = (x + _WIDTH_ - 1) % _WIDTH_;
                size_t r = (x + 1) % _WIDTH_;

                tModel_[y + x] = func_(cModel_[t + l], cModel_[t + x], cModel_[t + r],
                        cModel_[y + l], cModel_[y + x], cModel_[y + r],
                        cModel_[b + l], cModel_[b + x], cModel_[b + r]);
            }
        }

        std::swap(cModel_, tModel_);
    }

    void flush()
    {
        updateInAThread();
        threads_.clear();
        std::swap(cModel_, tModel_);
    }

    void setValue(size_t x, size_t y, Value value)
    {
        flush();
        cModel_[x + y * _WIDTH_] = value;
    }

    void setRangeValue(size_t x, size_t y, size_t widht, size_t height, Value value)
    {
# ifndef NDEBUG
        if(x + widht > _WIDTH_ || y + height > _HEIGHT_)
            throw "Invalid range!";
#endif
        flush();
        for(size_t i = y; i < height + y; ++i)
        {
            for(size_t j = x; j < widht + x; ++j)
            {
                cModel_[i * _WIDTH_ + j] = value;
            }
        }
    }

    void fill(Value value)
    {
        flush();
        std::fill_n(cModel_, _WIDTH_ * _HEIGHT_, value);
    }

    void clear()
    {
        flush();
        std::fill_n(cModel_, _WIDTH_ * _HEIGHT_, 0);
    }

private:
    void updateInAThread()
    {
        for(size_t i = updateIndex_++; i < _HEIGHT_; i = updateIndex_++)
        {
            size_t y = i * _WIDTH_;
            for(size_t x = 0; x < _WIDTH_; ++x)
            {
                size_t t = (i + _HEIGHT_ - 1) % _HEIGHT_ * _WIDTH_;
                size_t b = (i + 1) % _HEIGHT_ * _WIDTH_;

                size_t l = (x + _WIDTH_ - 1) % _WIDTH_;
                size_t r = (x + 1) % _WIDTH_;

                tModel_[y + x] = func_(cModel_[t + l], cModel_[t + x], cModel_[t + r],
                        cModel_[y + l], cModel_[y + x], cModel_[y + r],
                        cModel_[b + l], cModel_[b + x], cModel_[b + r]);
            }
        }
    }

    Value(* cModel_);
    Value(* tModel_);

    std::atomic_size_t updateIndex_;
    Value(*func_)(Value, Value, Value, Value, Value, Value, Value, Value, Value);
    std::vector<Redopera::RThread> threads_;
};

#endif // MODEL_H
