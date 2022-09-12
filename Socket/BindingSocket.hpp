//
//  BindingSocket.hpp
//  
//
//  Created by theHugh on 01/07/2022.
//


#ifndef BindingSocket_hpp
#define BindingSocket_hpp

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Socket.hpp"

namespace WEBLIBV1
{

class BindingSocket : public Socket
{
    public:
        BindingSocket(int pDomain, int pService, int pProtocal, int pPort, u_long pInterface);
        int Connect_Socket(int pSock, struct sockaddr_in pAddress);

};

}
#endif /* Socket_hpp */


