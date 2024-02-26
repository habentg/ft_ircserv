/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 08:53:46 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/04 08:54:32 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "irc.hpp"
/* Channel
    -> Channels names are strings (beginning with a '&', '#', '+' or '!' character)
    -> of length up to fifty (50) characters.  Channel names are case insensitive.
    -> The only restriction on a channel name is that it SHALL NOT contain any spaces (' '), a control G (^G or ASCII 7),
        a comma (',' which is used as a list item separator by the protocol).  Also, a colon (':') is used as
        a delimiter for the channel mask.
    -> In order for the channel members to keep some control over a channel, and some kind of sanity, some channel members are 
        privileged.  Only these members are allowed to perform the following actions on the channel:
                # INVITE  - Invite a client to an invite-only channel (mode +i)
                # KICK    - Eject a client from the channel
                # MODE    - Change the channel's mode, as well as members' privileges
                # PRIVMSG - Sending messages to the channel (mode +n, +m, +v)
                # TOPIC   - Change the channel topic in a mode +t channel
    -> channel operator (chanOp) -- has '@' with thier nicknmames
    -> Upon creation of the channel, this user is also given channel operator status.
    -> These channels are created implicitly when the first user joins it, and cease to exist when the last user leaves it.

    -> Channel modes (+/-): (we dont have to implement all of it)

        O - give "channel creator" status;
        o - give/take channel operator privilege;
        v - give/take the voice privilege;

        a - toggle the anonymous channel flag;
        i - toggle the invite-only channel flag;
        m - toggle the moderated channel;
        n - toggle the no messages to channel from clients on the
            outside;
        q - toggle the quiet channel flag;
        p - toggle the private channel flag;
        s - toggle the secret channel flag;
        r - toggle the server reop channel flag;
        t - toggle the topic settable by channel operator only flag;

        k - set/remove the channel key (password);
        l - set/remove the user limit to channel;

        b - set/remove ban mask to keep users out;
        e - set/remove an exception mask to override a ban mask;
        I - set/remove an invitation mask to automatically override
            the invite-only flag;
*/

class Client;
class Command;
class Server;

class Channel {
    private:
        std::string             _chanName;
        std::string             _chanKey;
        std::string             _creator;
        std::set<std::string>   _members;
        std::set<std::string>   _operators;
        std::set<char>          _chanModes;
    public:
        Channel(std::string chanName, std::string key, Client *creator);
        ~Channel();
        // members
        size_t          getNumOfChanMembers(void) const;
        std::string     getChannelName(void) const;
        void            addMember(std::string clientNick);
        void            deleteMember(std::string clientNick);
        bool            doesClientAlreadyAMember(std::string clientNick);
        std::string     getChanKey(void) const;
        void            setChanKey(std::string newKey);
};
#endif // !CHANNEL_HPP