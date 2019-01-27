#include "unique_ptr.hpp"
#include <iostream>
#include <cassert>

using namespace my;

struct C {

    double val;

    C(int a, int b) : val(a + b) {}
    C(double x) : val(x * 10) {}

};

void test_compile() {

    unique_ptr<int> r(new int(1));
    if (r) {
        assert(*r == 1);
    }
    int *ptr = r.release();
    r.reset(new int(2));

    unique_ptr<int> p(std::move(ptr));
    assert(p.get() == ptr);
    assert(*p.get() == 1);

    unique_ptr<int> q = std::move(r);
    assert(*q == 2);

    auto f = make_unique<C>(1, 1);
    auto g = make_unique<C>(10.);
    f.swap(g);

    assert(f->val == 100.);
    assert(g->val == 2.);

}

int main() {

    test_compile();

}