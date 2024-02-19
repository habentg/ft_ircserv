#include <iostream>
#include <string>
#include <vector>

#include <iostream>
#include <vector>
#include <string>

std::vector<std::string> newline_split(const std::string& str) {
    std::vector<std::string> arr;
    std::string::size_type start = 0;
    std::string::size_type end = 0;
    
    while ((end = str.find('\n', start)) != std::string::npos) {
        arr.push_back(str.substr(start, end - start));
        start = end + 1; // Skip the newline character
    }
    if (start < str.length()) {
        arr.push_back(str.substr(start));
    }
    
    return arr;
}

int main() {
    std::string str = "HELLO\nWORLD\r\nHOW\r\r\r\r\nARE\nYOU\r\nTODAY\n";
    std::vector<std::string> arr = newline_split(str);
    
    // Output the resulting vector of strings
    std::vector<std::string>::iterator it = arr.begin();
    for (; it != arr.end(); ++it) {
        std::cout << "[" << (*it) << "]" << std::endl;
    }
    
    return 0;
}