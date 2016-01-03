#include <list>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <cassert>

class SmallAllocator {
    struct mem_region
    {
        char* ptr;
        size_t size;

        bool operator==(mem_region const& other) const;
    };

public:
    SmallAllocator();

    void* Alloc(unsigned int);
    void* ReAlloc(void*, unsigned int);
    void Free(void* Pointer);
private:
    static constexpr size_t mem_size = 1048576;
    char Memory[mem_size];

    mem_region* find(void* ptr);
    mem_region* find(char* ptr);

    std::list<mem_region> mems_;
};

bool SmallAllocator::mem_region::operator==(mem_region const& other) const
{
    return ptr == other.ptr && size == other.size;
}

SmallAllocator::SmallAllocator()
    :mems_()
{
    mems_.push_back(mem_region{ &Memory[0], 0 });
}

void* SmallAllocator::Alloc(unsigned int size)
{
    auto const back = mems_.back();
    
    if(back.ptr - Memory + back.size + size > mem_size)
    {
        return nullptr;
    }

    mem_region new_reg;
    new_reg.ptr = back.ptr + back.size;
    new_reg.size = size;
    mems_.push_back(new_reg);

    return new_reg.ptr;
}

void* SmallAllocator::ReAlloc(void* pointer, unsigned int size)
{
    auto ptr = find(pointer);
    if(ptr)
    {
        if(ptr->size >= size)
        {
            ptr->size = size;
            return ptr->ptr;
        }

        auto buf = new char[ptr->size];
        memcpy(buf, ptr->ptr, ptr->size);
        auto new_mem = static_cast<char*>(Alloc(size));
        if(new_mem == nullptr)
        {
            return nullptr;
        }

        for (size_t i = 0; i < ptr->size; ++i)
        {
            new_mem[i] = buf[i];
        }

        delete[] buf;

        return new_mem;
    }

    return nullptr;
}

void SmallAllocator::Free(void* pointer)
{
    auto elem = find(pointer);

    mems_.remove(*elem);
}

SmallAllocator::mem_region* SmallAllocator::find(void* ptr)
{
    return find(static_cast<char*>(ptr));
}

SmallAllocator::mem_region* SmallAllocator::find(char* ptr)
{
    for(auto& elem : mems_)
    {
        if(elem.ptr == ptr)
        {
            return &elem;
        }
    }

    return nullptr;
}

void tests()
{
    SmallAllocator A1;
    int * A1_P1 = (int *)A1.Alloc(sizeof(int));
    A1_P1 = (int *)A1.ReAlloc(A1_P1, 2 * sizeof(int));
    A1.Free(A1_P1);
    SmallAllocator A2;
    int * A2_P1 = (int *)A2.Alloc(10 * sizeof(int));
    for (unsigned int i = 0; i < 10; i++) A2_P1[i] = i;
    for (unsigned int i = 0; i < 10; i++) if (A2_P1[i] != i) std::cout << "ERROR 1" << std::endl;
    int * A2_P2 = (int *)A2.Alloc(10 * sizeof(int));
    for (unsigned int i = 0; i < 10; i++) A2_P2[i] = -1;
    for (unsigned int i = 0; i < 10; i++) if (A2_P1[i] != i) std::cout << "ERROR 2" << std::endl;
    for (unsigned int i = 0; i < 10; i++) if (A2_P2[i] != -1) std::cout << "ERROR 3" << std::endl;
    A2_P1 = (int *)A2.ReAlloc(A2_P1, 20 * sizeof(int));
    for (unsigned int i = 10; i < 20; i++) A2_P1[i] = i;
    for (unsigned int i = 0; i < 20; i++) if (A2_P1[i] != i) std::cout << "ERROR 4" << std::endl;
    for (unsigned int i = 0; i < 10; i++) if (A2_P2[i] != -1) std::cout << "ERROR 5" << std::endl;
    A2_P1 = (int *)A2.ReAlloc(A2_P1, 5 * sizeof(int));
    for (unsigned int i = 0; i < 5; i++) if (A2_P1[i] != i) std::cout << "ERROR 6" << std::endl;
    for (unsigned int i = 0; i < 10; i++) if (A2_P2[i] != -1) std::cout << "ERROR 7" << std::endl;
    A2.Free(A2_P1);
    A2.Free(A2_P2);

    int * in1 = (int *)A2.Alloc(1 * sizeof(int));
    *in1 = 56;
    int * in2 = (int *)A2.Alloc(1 * sizeof(int));
    *in2 = 63;
    int * in3 = (int *)A2.Alloc(1 * sizeof(int));
    *in3 = 58;
    int * in4 = (int *)A2.Alloc(1 * sizeof(int));
    *in4 = 23;
    int * in5 = (int *)A2.Alloc(1 * sizeof(int));
    *in5 = 29;

    assert(*in1 == 56);
    assert(*in2 == 63);
    assert(*in3 == 58);
    assert(*in4 == 23);
    assert(*in5 == 29);

    A2.Free(in2);
    A2.Free(in3);
    A2.Free(in4);

    in2 = (int *)A2.Alloc(1 * sizeof(int));
    *in2 = 163;
    in3 = (int *)A2.Alloc(1 * sizeof(int));
    *in3 = 158;
    in4 = (int *)A2.Alloc(1 * sizeof(int));
    *in4 = 123;

    assert(*in2 == 163);
    assert(*in3 == 158);
    assert(*in4 == 123);


    in5 = (int *)A2.ReAlloc(in5, 3 * sizeof(int));

    in5[1] = 25;
    in5[2] = 21;

    assert(in5[0] == 29);
    assert(in5[1] == 25);
    assert(in5[2] == 21);
    assert(*in1 == 56);
    assert(*in5 == 29);

    A2.Free(in1);
    A2.Free(in5);

    int * A2_P3 = (int *)A2.Alloc(10 * sizeof(int));
    for (unsigned int i = 0; i < 10; i++) A2_P3[i] = i;
    for (unsigned int i = 0; i < 10; i++) assert(A2_P3[i] == i);

    int * A2_P4 = (int *)A2.Alloc(10 * sizeof(int));
    for (unsigned int i = 0; i < 10; i++) A2_P4[i] = i;
    A2_P4 = (int *)A2.ReAlloc(A2_P4, 20 * sizeof(int));
    for (unsigned int i = 10; i < 20; i++) A2_P4[i] = i;
    for (unsigned int i = 0; i < 10; i++) assert(A2_P3[i] == i);
    for (unsigned int i = 0; i < 20; i++) assert(A2_P4[i] == i);

    A2.Free(A2_P3);

    A2_P1 = (int *)A2.Alloc(10 * sizeof(int));
    for (unsigned int i = 0; i < 10; i++) A2_P1[i] = i;
    for (unsigned int i = 0; i < 10; i++) assert(A2_P1[i] == i);

    A2_P2 = (int *)A2.Alloc(10 * sizeof(int));
    for (unsigned int i = 0; i < 10; i++) A2_P2[i] = i;
    A2_P2 = (int *)A2.ReAlloc(A2_P2, 20 * sizeof(int));
    for (unsigned int i = 10; i < 20; i++) A2_P2[i] = i;
    for (unsigned int i = 0; i < 10; i++) assert(A2_P1[i] == i);
    for (unsigned int i = 0; i < 20; i++) assert(A2_P2[i] == i);
    for (unsigned int i = 0; i < 20; i++) assert(A2_P4[i] == i);

    assert(*in2 == 163);
    assert(*in3 == 158);
    assert(*in4 == 123);
}

void tests2()
{
    SmallAllocator A2;
    int * A2_P1 = (int *)A2.Alloc(10 * sizeof(int));
    for (unsigned int i = 0; i < 10; i++) A2_P1[i] = i;
    int * A2_P2 = (int *)A2.Alloc(10 * sizeof(int));
    for (unsigned int i = 0; i < 10; i++) A2_P2[i] = -1;
    A2_P1 = (int *)A2.ReAlloc(A2_P1, 20 * sizeof(int));
    for (unsigned int i = 10; i < 20; i++) A2_P1[i] = i;
    A2_P1 = (int *)A2.ReAlloc(A2_P1, 5 * sizeof(int));
    for (unsigned int i = 0; i < 5; i++) if (A2_P1[i] != i) std::cout << "ERROR" << std::endl;
}

void tests3()
{
    SmallAllocator A2;
    int * A2_P1 = (int *)A2.Alloc(10 * sizeof(int));
    for (unsigned int i = 0; i < 10; i++) A2_P1[i] = i;
    int * A2_P2 = (int *)A2.Alloc(10 * sizeof(int));
    A2_P1 = (int *)A2.ReAlloc(A2_P1, 20 * sizeof(int));
    for (unsigned int i = 10; i < 20; i++) A2_P1[i] = i;
}

void tests4()
{
    SmallAllocator A2;
    int * A2_P1 = (int *)A2.Alloc(10 * sizeof(int));
    for (unsigned int i = 0; i < 10; i++) A2_P1[i] = i;
    int * A2_P2 = (int *)A2.Alloc(10 * sizeof(int));
}

int main()
{
    tests();

    tests2();

    tests3();

    tests4();

    return 0;
}
