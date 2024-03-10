#include <iostream>
#include <sstream>
#include <set>
#include <map>
#include <string>

int main(void) {
    std::set<std::string> myset;

    myset.insert("a");
    myset.insert("g");
    myset.insert("g");
    myset.insert("f");
    myset.insert("c");

    std::cout << "size: " << myset.size() << "\n";
    myset.erase("this");
    std::cout << "size: " << myset.size() << "\n";
}