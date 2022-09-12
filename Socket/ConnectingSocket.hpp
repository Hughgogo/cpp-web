//
//  ConnectingSocket.hpp
//  
//
//  Created by theHugh on 05/07/2022.
//


#ifndef ConnectingSocket_hpp
#define ConnectingSocket_hpp

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Socket.hpp"

namespace WEBLIBV1
{

class ConnectingSocket : public Socket
{
    public:
        ConnectingSocket(int pDomain, int pService, int pProtocal, int pPort, u_long pInterface);
        int Connect_Socket(int pSock, struct sockaddr_in pAddress);

};

}
#endif /* ConnectingSocket_hpp */


