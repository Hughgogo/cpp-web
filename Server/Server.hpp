//
//  Server.hpp
//  
//  Basic Server Object
//
//  Created by theHugh on 11/08/2022.
//

#ifndef Server_hpp
#define Server_hpp

#include <stdio.h>

#include "../Socket/WEBLIBV1-Socket.hpp"

namespace WEBLIBV1
{

    class Server
    {
        private:
        ListeningSocket * _listeningSock;
        virtual void _accepter() = 0;
        virtual void _handler() = 0;
        virtual void _responder() = 0;
        
        public:
        Server(int pDomain, int pService, int pProtocal, int pPort, u_long pInterface, int pBacklog);

        virtual void Launch() = 0;
        
        ListeningSocket * Get_Socket();
    };
}

#endif /* Server_hpp */
