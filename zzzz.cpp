#include <iostream>
#include <sstream>
#include <ctime>
#include <cstring>

std::string timeToString(time_t time) {
    char buffer[80];
    std::memset(buffer, 0, sizeof(buffer));
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
    return std::string(buffer);
}

void sendRplChannelCreationTime(int clientSocket, const char* serverName, const char* channel, time_t creationTime) {
    std::string timeString = timeToString(creationTime);
    std::ostringstream message;
    message << ":" << serverName << " 329 <nickname> " << channel << " " << creationTime << " :" << timeString << "\r\n";
    std::cout << message.str() << std::endl;
    // send(clientSocket, message.str().c_str(), message.str().length(), 0);
}

int main() {
    // Example usage
    int clientSocket = 5;
    const char* serverName = "hostsailor.ro.quakenet.org";
    const char* channel = "#sdff";
    time_t creationTime = 1709910616;

    // Send RPL message to the client
    sendRplChannelCreationTime(clientSocket, serverName, channel, creationTime);

    return 0;
}
