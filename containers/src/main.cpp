#include <iostream>
#include <sstream>
#include <unordered_map>
#include <map>

class poly
{
    struct factor
    {
        long long degree_;
        long long coef_;

        bool operator<(factor const& right) const;
    };

public:
    poly();
    poly(poly & other);
    poly(std::unordered_map<long long, long long> elems);
    explicit poly(std::string const & str);

    poly& operator=(poly other);

    std::istream& operator>>(std::istream& is);

    std::string to_string() const;

    poly diff();

    friend void swap(poly & lhs, poly &rhs);
private:
    void from_string(std::string const&);

    std::unordered_map<long long, long long> data_;
};

std::ostream& operator<<(std::ostream& os, poly const& p)
{
    os << p.to_string();
    return os;
}

void swap(poly& lhs, poly& rhs)
{
    std::swap(lhs.data_, rhs.data_);
}

bool poly::factor::operator<(factor const& other) const
{
    return degree_ < other.degree_;
}

poly::poly()
{}

poly::poly(poly & other)
    : data_(other.data_)
{}

poly::poly(std::unordered_map<long long, long long> elems)
    :data_(elems)
{}

poly::poly(std::string const& str)
{
    from_string(str);
}

poly& poly::operator=(poly other)
{
    swap(*this, other);
    return *this;
}

std::istream& poly::operator>>(std::istream& is)
{
    std::string str;
    getline(is, str);
    from_string(str);

    return is;
}

std::string poly::to_string() const
{
    std::ostringstream result;
    auto is_first = true;
    std::map<long long, long long> ordered(data_.begin(), data_.end());
    for (auto i = ordered.rbegin(); i != ordered.rend(); ++i)
    {
        auto const& elem = *i;
        if(elem.second != 0)
        {
            if (!is_first && elem.second > 0) result << "+";
            result << elem.second;
            if (elem.first != 0) result << "*x";
            if(elem.first > 1)
            {
                result << "^" << elem.first;
            }
        }

        is_first = false;
    }

    return result.str();
}

poly poly::diff()
{
    std::unordered_map<long long, long long> result;

    for(auto const& elem : data_)
    {
        if(elem.first != 0)
        {
            result.insert_or_assign(elem.first - 1, elem.second * elem.first);
        }
    }

    return result;
}

void poly::from_string(std::string const & str)
{
    long long coef = 0;
    long long degree = 0;
    int sing = 1;

    auto stete_num = 1;
    auto ch = str.cbegin();
    while (ch != str.cend())
    {
        switch (stete_num)
        {
        case 1: // начало
            if (*ch == '-')
            {
                ++ch;
                sing = -1;
                coef = degree = 0;
            }
            else if(*ch == '+')
            {
                ++ch;
                sing = 1;
                coef = degree = 0;
            }
            else if (*ch == 'x')
            {
                ++ch;
                coef = 1;
                degree = 0;
                stete_num = 3;
            }
            else
            {
                stete_num = 2;
            }
            break;
        case 2: // Чтение коэф.
            if (*ch == '*')
            {
                stete_num = 3;
            }
            else if (*ch == '+')
            {
                if (!data_.count(degree))
                {
                    data_[degree] = 0;
                }

                data_[degree] += coef * sing;
                stete_num = 1;
            }
            else if (*ch == '-')
            {
                if (!data_.count(degree))
                {
                    data_[degree] = 0;
                }

                data_[degree] += coef * sing;
                stete_num = 1;
            }
            else
            {
                coef = coef * 10 + (*ch - '0');
                ++ch;
            }
            break;
        case 3: // Чтение промежуточных символов
            if(*ch == '+')
            {
                if (!data_.count(1))
                {
                    data_[1] = 0;
                }

                data_[1] += coef * sing;
                sing = 1;
                coef = degree = 0;
                stete_num = 1;
            }
            else if(*ch == '-')
            {
                if (!data_.count(1))
                {
                    data_[1] = 0;
                }

                data_[1] += coef * sing;
                sing = -1;
                coef = degree = 0;
                stete_num = 1;
            }
            else if(*ch == '^')
            {
                ++ch;
                stete_num = 4;
            }
            else
            {
                ++ch;
            }
            break;
        case 4: // Чтение степени
            if(*ch == '+')
            {
                if(!data_.count(degree))
                {
                    data_[degree] = 0;
                }

                data_[degree] += coef * sing;
                stete_num = 1;
            }
            else if(*ch == '-')
            {
                if (!data_.count(degree))
                {
                    data_[degree] = 0;
                }

                data_[degree] += coef * sing;
                stete_num = 1;
            }
            else
            {
                degree = degree * 10 + (*ch - '0');
                ++ch;
            }
        }
    }
    if(stete_num == 4)
    {
        if (!data_.count(degree))
        {
            data_[degree] = 0;
        }

        data_[degree] += coef * sing;
    }
}

std::ostream operator<<(const std::ostream& lhs, const poly& p1);

void run_tests() 
{
    poly p1("x^2+x");
    std::cout << p1 << " | "  << poly(p1.diff()) << std::endl;

    poly p2("2*x^100+100*x^2");
    std::cout << p2 << " | "  << poly(p2.diff()) << std::endl;

    poly p3("x^10000+x+1");
    std::cout << p3 << " | "  << poly(p3.diff()) << std::endl;

    poly p4("-x^2-x^3");
    std::cout << p4 << " | "  << poly(p4.diff()) << std::endl;

    poly p5("x+x+x+x+x+x+x+x+x+x");
    std::cout << p5 << " | " << poly(p5.diff()) << std::endl;
}

int main() {
    poly p;
    run_tests();
}