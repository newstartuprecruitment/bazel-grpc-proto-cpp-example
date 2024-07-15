#include <boost/coroutine/all.hpp>
#include <iostream>

using namespace boost::coroutines;

void fn(coroutine<void>::push_type &sink)
{
    std::cout << "fn1\n";
    sink();
    std::cout << "fn2\n";
}

int main()
{
    coroutine<void>::pull_type source{fn};
    std::cout << "main\n";
    source();
}