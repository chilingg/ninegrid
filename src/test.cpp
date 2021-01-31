#include <RDebug.h>

#include <vector>
#include <array>
#include <iostream>

using namespace Redopera;

std::vector<std::array<int, 8>> sequences;

void insert(std::vector<int> &a1, std::vector<int> &a2, int value)
{
    for(int n = value; n < 8; ++n)
    {
        bool find = false;
        for(auto v : a1)
        {
            if(n == v)
                find = true;
        }
        for(auto v : a2)
        {
            if(n == v)
                find = true;
        }
        if(find)
            continue;

        a1.push_back(n);
        a2.push_back(9 - n);

        if(a1.size() == 4)
        {
            sequences.emplace_back();
            for(int k = 0; k < 4; ++k)
            {
                sequences.back()[k] = a1[k];
                sequences.back()[k + 4] = a2[k];
            }
        }
        else
            insert(a1, a2, 2);

        a1.pop_back();
        a2.pop_back();
    }
}

bool test(int side1, int side2, const std::array<int, 8>& ary)
{
    int v = (ary[side1] + ary[side2]) % 9;
    if(v == 0)
        return true;

    int angle = side2 - side1 - 1;
    int start, end;
    if(angle < 3)
    {
        start = side2 + 1;
        end = side1;
    }
    else
    {
        start = side1 + 1;
        end = side2;
    }

    for(int i = start; i % 8 != end % 8; ++i)
    {
        if(ary[i % 8] == v)
            return true;
    }

    return false;
}

bool test2(int side1, int side2, const std::array<int, 8>& ary)
{
    int v = (ary[side1] + ary[side2]) % 9;
    if(v == 0)
        return true;

    int start = (side1 + 4) % 8, end = (side2 + 4) % 8;

    for(int i = start; i % 8 != end; ++i)
    {
        if(ary[i % 8] == v)
            return true;
    }

    return false;
}

int main()
{
    std::vector<int> ary1, ary2;

    insert(ary1, ary2, 1);

    rDebug << "Sequence count: " << sequences.size();

    std::vector<std::array<int, 8>> result;
    for(auto& seq : sequences)
    {
        bool b = true;
        for(unsigned i = 0; i < seq.size() - 1; ++i)
        {
            for(unsigned j = i + 1; j < seq.size(); ++j)
            {
                b &= test(i, j, seq);
            }
        }
        if(b)
            result.push_back(seq);
    }

    for(auto it1 = result.begin(); it1 != result.end(); ++it1)
    {
        for(auto it2 = result.begin(); it2 != result.end();)
        {
            if(it1 == it2)
            {
                ++it2;
                continue;
            }

            bool b = true;
            for(size_t k = 1, l = 7; k < 4; ++k, --l)
            {
                b &= (*it1)[k] == (*it2)[l];
            }
            if(b)
                it2 = result.erase(it2);
            else
                ++it2;
        }
    }

    for(auto& seq : result)
    {
        bool b = true;
        for(unsigned i = 0; i < seq.size() - 1; ++i)
        {
            for(unsigned j = i + 1; j < seq.size(); ++j)
            {
                b &= test2(i, j, seq);
                if(!b) break;
            }
            if(!b) break;
        }
        if(b)
            for(auto& seq : result)
            {
                rDebug << seq[0] << seq[1] << seq[2] << seq[3]
                        << seq[4] << seq[5] << seq[6] << seq[7];
            }
    }

    rDebug << "Result count: " << result.size();
    for(auto& seq : result)
    {
        rDebug << seq[0] << seq[1] << seq[2] << seq[3]
                << seq[4] << seq[5] << seq[6] << seq[7];
    }

    /*
    std::array<int, 8> temp{1,2,4,3,8,7,5,6};
    bool b = true;
    for(unsigned i = 0; i < temp.size() - 1; ++i)
    {
        for(unsigned j = i + 1; j < temp.size(); ++j)
        {
            b &= test(i, j, temp);
            if(!b) break;
        }
        if(!b) break;
    }
    */

    return 0;
}
