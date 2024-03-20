/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/07 13:50:10 by tpetros           #+#    #+#             */
/*   Updated: 2024/03/19 07:17:22 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "bot.hpp"

const std::string Bot::nickName = "bigbrother";
const std::string Bot::realName = "irc bot";
const std::string Bot::userName = "irc bot";

void Bot::setDictionary( void )
{
	dictionary.push_back("blyat");
	dictionary.push_back("shit");
	dictionary.push_back("fuck");
	dictionary.push_back("bitch");
	dictionary.push_back("dickhead");
	dictionary.push_back("asshole");
	dictionary.push_back("whore");
	dictionary.push_back("hiiiiii~"); // so gay!
}

Bot::Bot(const std::string& serverIP, int port, const std::string& password)
{
	setDictionary();
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        throw std::runtime_error("Error opening socket");
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

	establishConnection();
	std::string passCmd = "PASS " + password + "\r\n";
	sendMsg(passCmd);

    std::string nickCmd = "NICK " + nickName + "\r\n";
    sendMsg(nickCmd);

    std::string userCmd = "USER " + userName + " 0 * :" + userName + "\r\n";
    sendMsg(userCmd);
}

Bot::~Bot( void ) { close(sockfd); }

void Bot::establishConnection() 
{
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
		throw std::runtime_error("Error connecting to server!");
}

void Bot::joinChannel(const std::string& channel) 
{
    std::string joinMessage = "JOIN #" + channel + "\r\n";
    sendMsg(joinMessage);
}

bool Bot::isOffensiveWord(const std::string& word) {
    std::string lowercaseWord = word;
    std::transform(lowercaseWord.begin(), lowercaseWord.end(), lowercaseWord.begin(), ::tolower);

    for (size_t i = 0; i < dictionary.size(); ++i)
	{
        if (lowercaseWord.find(dictionary[i]) != std::string::npos)
            return true;
    }
    return false;
}

const bool Bot::checkOffensiveWords(const std::string& message)
{
	size_t pos = message.find_last_of(":");
	std::string the_message;
	if (pos != std::string::npos)
		the_message = message.substr(pos + 1, message.length());
	const std::vector<std::string> temp = split(the_message, ' ');

    for (int i = 0; i < temp.size(); i++)
	{
        if (isOffensiveWord(std::string(temp[i])))
            return true;
    }
    return false;
}

void Bot::kickUser(const std::string& user, std::string channel)
{
    std::string kickMessage = "KICK #" + channel + " " + user + " :Offensive language detected\r\n";
    sendMsg(kickMessage);
	std::cout << ("Kicked " + user) << " :Offensive language detected" << std::endl;
}

int	check_pass( std::string message )
{
    if (!message.empty())
    {
        size_t pos = message.find_last_of(":");
        std::string the_message;
        if (pos != std::string::npos)
        {
            the_message = message.substr(pos + 1, message.length());
            std::vector<std::string> temp = split(the_message, ' ');
            if (temp[1] == "incorrect" || temp[1] == "already")
                return (1);
        }
    }
	return (0);
}

bool serverRunning = true;
bool signalRecieved = false;

void signalHandler(int signum)
{
    (void) signum;
    serverRunning = false;
    signalRecieved = true;
}

void Bot::monitor(std::string channel) 
{
    char buffer[4096];
    std::memset(buffer, 0, sizeof(buffer));
    bool passwordChecked = false;

    std::signal(SIGINT, signalHandler);
    std::signal(SIGQUIT, signalHandler);

    struct pollfd fds[1];
    fds[0].fd = sockfd;
    fds[0].events = POLLIN;

    while (serverRunning) 
    {
        int ret = poll(fds, 1, -1);
        if (ret == -1 && signalRecieved) 
        {
            serverRunning = false;
            break;
        }
        else if (ret == -1)
            throw std::runtime_error("Error: polling problem");

        if (fds[0].revents == fds[0].events)
        {
            ssize_t bytesRead = recv(sockfd, buffer, sizeof(buffer), 0);
            if (bytesRead <= 0)
            {
                std::cerr << "Error receiving data" << std::endl;
                break;
            }
            std::string message(buffer, bytesRead);
            if (!passwordChecked)
            {
                if (check_pass(message)) {
                    throw std::runtime_error("Wrong password or User with that nick name already exists!\n, bot cannot be connected!");   
                }
                passwordChecked = true;
            }
            std::string temp = "PRIVMSG #" + channel;
            if (message.find(temp) != std::string::npos && checkOffensiveWords(message))
            {
                size_t pos = message.find("!");
                if (pos != std::string::npos)
                {
                    std::string user = message.substr(1, pos - 1);
                    kickUser(user, channel);
                }
            }
        }
    }
}


void Bot::sendMsg( const std::string& message )
{
    if (send(sockfd, message.c_str(), message.length(), 0) < 0)
	{
		std::cerr << "Message sending failed\n";
		std::exit(EXIT_FAILURE);
	}
}
