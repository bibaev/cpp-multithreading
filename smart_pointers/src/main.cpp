#include <string>
#include <iostream>

class StringPointer
{
public:
    std::string *operator->() const;
    operator std::string*() const;

    StringPointer(std::string* Pointer);

    ~StringPointer();

private:
    std::string* ptr_;
};


std::string* StringPointer::operator->() const
{
    return ptr_;
}

StringPointer::operator std::string*() const
{
    return ptr_;
}

StringPointer::StringPointer(std::string* ptr)
    : ptr_(ptr == nullptr ? new std::string("") : ptr)
{}

StringPointer::~StringPointer()
{
    if(*ptr_ == "")
    {
        delete ptr_;
    }
}

void tests()
{
    std::string s1 = "Hello, world!";

    StringPointer sp1(&s1);
    StringPointer sp2(nullptr);

    std::cout << sp1->length() << std::endl;
    std::cout << *sp1 << std::endl;
    std::cout << sp2->length() << std::endl;
    std::cout << *sp2 << std::endl;
}

int main()
{
    tests();
}
