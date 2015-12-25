#include <list>
#include <cassert>

class SmallAllocator {
    struct mem_region
    {
        char* ptr;
        size_t size;

        bool operator==(mem_region const& other) const;
    };

public:
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

void* SmallAllocator::Alloc(unsigned int size)
{
    auto const& back = mems_.back();
    
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
        auto buf = new char[ptr->size];
        memcpy(buf, ptr->ptr, ptr->size);
        auto new_mem = static_cast<char*>(Alloc(size));
        for (auto i = 0; i < ptr->size; ++i)
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
    SmallAllocator allocator{};

    auto mem_reg = static_cast<int*>(allocator.Alloc(10));
    
    mem_reg[0] = 100;
    mem_reg = static_cast<int*>(allocator.ReAlloc(mem_reg, 20));
    assert(mem_reg[0] == 100);
}

int main()
{
    tests();

    return 0;
}
