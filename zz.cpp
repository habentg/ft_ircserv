
# include <iostream>
# include <exception>
# include <string>
# include <vector>
# include <map>
# include <ctime>
# include <set>
# include <fstream>

int main(void) {

    std::ifstream file("MOTD.txt", std::ifstream::in);
    if (!file.is_open()) {
        std::cerr << "Error opening MOTD file!" << std::endl;
    } else {
        std::string line;
        while (std::getline(file, line)) {
            std::cerr << line << std::endl;
        }
        file.close();
    }

}

