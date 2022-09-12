//
//  SimpleServer.hpp
//  
//  Basic Server Object
//
//  Created by theHugh on 4/08/2022.
//


#ifndef SimpleServer_hpp
#define SimpleServer_hpp

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>

#include "../Socket/WEBLIBV1-Socket.hpp"
#include "Server.hpp"

namespace WEBLIBV1
{

    class SimpleServer : public Server
    {

        private:
        char buffer[30000] = {0};
        int new_socket;
        ListeningSocket * _listeningSock;
        void _accepter();
        void _handler();
        void _responder();
        
        public:
        SimpleServer(int pDomain, int pService, int pProtocal, int pPort, u_long pInterface, int pBacklog);
        void Launch();
        
    };
}

#endif /* SimpleServer_hpp */
