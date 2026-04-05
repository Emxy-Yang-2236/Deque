#include "deque.hpp"

#include <ctime>
#include <iostream>
#include <deque>
#include <random>
#include <string>
#include <vector>
#include <ctime>

using std::cout;
using std::endl;

static const int N = 10000;
static const int N_SPEED = 21000;


class Int{
private:
    int data;

public:
    Int() = default;
    Int(const int &data) : data(data) {}
    Int & operator =(const Int &rhs) = default;
    bool operator <(const Int &rhs) = delete;
    bool operator >(const Int &rhs) = delete;
    bool operator <=(const Int &rhs) = delete;
    bool operator >=(const Int &rhs) = delete;
    bool operator ==(const Int &rhs)const {
        return data == rhs.data;
    }
    bool operator !=(const Int &rhs)const {
        return data != rhs.data;
    }

};

bool isEqual(std::deque<Int> &a, sjtu::deque<Int> &b) {
    static std::vector<Int> resultA, resultB;
    resultA.clear();
    resultB.clear();
    for (auto x : a) resultA.push_back(x);
    for (auto x : b) resultB.push_back(x);
    if (resultA.size() != resultB.size()) return false;
    for (int i = 0; i < (int)resultA.size(); i++) {
        if (resultA[i] != resultB[i]) return false;
    }
    return true;
}


class Timer{
private:
    long dfnStart, dfnEnd;

public:
    void init() {
        dfnEnd = dfnStart = clock();
    }
    void stop() {
        dfnEnd = clock();
    }
    double getTime() {
        return 1.0 * (dfnEnd - dfnStart) / CLOCKS_PER_SEC;
    }

};

Timer timer;


std::pair<bool, double> iteratorAddNChecker() {
    std::deque<Int> a;
    sjtu::deque<Int> b;
    for (int i = 0; i < N; i++) {
        int pos = rand() % (a.size() + 1);
        int tmp = rand();
        a.insert(a.begin() + pos, tmp);
        b.insert(b.begin() + pos, tmp);
    }
    timer.init();
    for (int i = 0; i < N; i++) {
        if (*(a.begin() + i) != *(b.begin() + i)) {
            return std::make_pair(false, 0);
        }
        int tmp = rand();
        *(a.begin() + i) = tmp;
        *(b.begin() + i) = tmp;
    }
    for (int i = 1; i <= N; i++) {
        if (*(a.end() + (-i)) != *(b.end() + (-i))) {
            return std::make_pair(false, 0);
        }
    }
    timer.stop();
    if (!isEqual(a, b)) {
        return std::make_pair(false, 0);
    } else {
        return std::make_pair(true, timer.getTime());
    }
}

std::pair<bool, double> backChecker() {
    std::deque<Int> a;
    sjtu::deque<Int> b;
    timer.init();
    for (int i = 0; i < N; i++) {
        int pos = rand() % (a.size() + 1);
        int tmp = rand();
        a.push_back(tmp);
        b.push_back(tmp);
        if (a.back() != b.back()) {
            return std::make_pair(false, 0);
        }
    }
    timer.stop();
    return std::make_pair(true, timer.getTime());
}

int main() {
    cout<<backChecker().first<<endl;;

    return 0;
}