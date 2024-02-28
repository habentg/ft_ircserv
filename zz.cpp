#include <iostream>
#include <string>
#include <map>

int main(void) {
    std::map<int, std::string> mp;

    mp.insert(std::make_pair(0, "this0"));
    mp.insert(std::make_pair(1, "this1"));
    mp.insert(std::make_pair(2, "this2"));
    mp.insert(std::make_pair(3, "this3"));
    mp.insert(std::make_pair(4, "this4"));
    mp.insert(std::make_pair(5, "this5"));
    mp.insert(std::make_pair(6, "this6"));
    mp.insert(std::make_pair(7, "this7"));
    mp.insert(std::make_pair(8, "this8"));
    mp.insert(std::make_pair(0, "this0"));
    mp.insert(std::make_pair(0, "this0"));

    std::cout << mp[0] << " : " << mp.size() << std::endl;
}