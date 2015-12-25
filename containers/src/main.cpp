#include <iostream>
#include <forward_list>

class poly {
  private:
    struct factor {
        long long degree;
        long long coef;
    };

  public:
    poly() {
        
    }

  private:
    std::forward_list<factor> data_;
};

void run_tests() {
}

int main() {
    poly p;
}