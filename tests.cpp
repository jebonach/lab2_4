#include "N-aryTree.hpp"
#include "ui.h"
#include "errors.hpp"

#include <cassert>
#include <vector>
#include <string>


template<typename F>
void assertThrows(F fn, ErrorType type, uint8_t code)
{
    bool caught = false;
    try { fn(); }
    catch (const MyException& ex) {
        assert(ex.getType() == type);
        assert(ex.getCode()  == code);
        caught = true;
    }
    assert(caught && "exception not thrown");
}

// 1. конструктор
void testNegativeDegree()
{
    assertThrows([] { NAryTree<int> t(0); },
                 ErrorType::NegativeSize, 2);
}

// 2. вставка корня и дубликата
void testRootInsertAndDuplicate()
{
    NAryTree<int> t(2);
    t.insert({}, 42);
    assert(t.root()->value == 42);

    assertThrows([&] { t.insert({}, 1); },
                 ErrorType::InvalidArg, 6);
}

// 3. вставка ребёнка до корня
void testInsertChildBeforeRoot()
{
    NAryTree<int> t(3);
    assertThrows([&] { t.insert({0}, 5); },
                 ErrorType::OutOfRange, 8);
}

// 4. неверный индекс
void testInsertBadIndex()
{
    NAryTree<int> t(2);
    t.insert({}, 0);
    assertThrows([&] { t.insert({2}, 1); },
                 ErrorType::OutOfRange, 3);
}

// 5. дубликат ребёнка
void testInsertDuplicateChild()
{
    NAryTree<int> t(2);
    t.insert({}, 0);
    t.insert({0}, 1);
    assertThrows([&] { t.insert({0}, 2); },
                 ErrorType::InvalidArg, 7);
}

// 6. erase + ошибки
void testEraseRootAndInvalidPath()
{
    NAryTree<int> t(2);
    assertThrows([&] { t.erase({}); }, ErrorType::InvalidArg, 5);

    t.insert({}, 1);
    t.erase({});
    assert(t.root() == nullptr);

    assertThrows([&] { t.erase({0}); },
                 ErrorType::InvalidArg, 5);
}

// 7. map + reduce
void testMapReduce()
{
    NAryTree<int> t(2);
    t.insert({}, 1);
    t.insert({0}, 2);
    t.insert({1}, 3);

    auto doubled = t.map([](int x){ return x*2; });
    int sum2 = doubled.reduce([](int a,int b){ return a+b; }, 0);
    int sum1 = t.reduce([](int a,int b){ return a+b; }, 0);

    assert(sum2 == 12);
    assert(sum1 == 6);
}

// 8. containsSubtree (точное совпадение)
void testContainsExact()
{
    NAryTree<int> big(2);
    big.insert({},   10);
    big.insert({0},  20);
    big.insert({0,1},30);

    NAryTree<int> pat(2);
    pat.insert({}, 20);
    pat.insert({1},30);

    assert(big.containsSubtree(pat.root()));
}

// 9. deep chain
void testDeepTree()
{
    const int DEPTH = 15;
    NAryTree<int> t(1);
    std::vector<std::size_t> p;
    for(int i=0; i < DEPTH; i++){
        t.insert(p,i);
        p.push_back(0);
    }
    int cnt = t.reduce([](int a,int){ return a+1; }, 0);
    assert(cnt == DEPTH);
}

// 10. wide level
void testWideTree()
{
    NAryTree<int> t(5);
    t.insert({},0);
    for(int i=0;i<5;++i) {
        t.insert({static_cast<std::size_t>(i)}, i);
    }
    int nodes = t.reduce([](int a,int){return a+1; }, 0);
    assert(nodes == 6);
}

// 11. parsePath
void testParsePath()
{
    auto v1 = parsePath("0/1/2");
    assert((v1 == std::vector<std::size_t>({0,1,2})));

    auto v2 = parsePath("0/../1");
    assert((v2 == std::vector<std::size_t>({1})));

    assertThrows([]{ parsePath("../.."); },
                 ErrorType::OutOfRange, 8);
}

// 12. erase branch
void testEraseBranch()
{
    NAryTree<int> t(2);
    t.insert({},0);
    t.insert({0},1);
    t.insert({0,0},2);
    t.erase({0});

    assert(t.find({0}) == nullptr);
    assert(t.reduce([](int a,int b){ return a+b; }, 0) == 0);
}

// 13. массовые вставки/удаления 
void testManyInsertErase()
{
    NAryTree<int> t(3);
    t.insert({},0);
    for(int i=0;i<3;++i) {
        t.insert({static_cast<std::size_t>(i)}, i);
    }
    for(int i=0;i<3;++i) {
        t.erase({static_cast<std::size_t>(i)});
    }
    assert(t.reduce([](int a,int b){return a+b;},0)==0);
}

int main()
{
    testNegativeDegree();
    testRootInsertAndDuplicate();
    testInsertChildBeforeRoot();
    testInsertBadIndex();
    testInsertDuplicateChild();
    testEraseRootAndInvalidPath();
    testMapReduce();
    testContainsExact();
    testDeepTree();
    testWideTree();
    //cout << "testWideTree passed\n";
    testParsePath();
    testEraseBranch();
    testManyInsertErase();

    std::cout << "[OK] all tests passed\n";
    return 0;
}
