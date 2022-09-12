//
//  Server.cpp
//  
//  Basic Server Object
//
//  Created by theHugh on 11/08/2022.
//

#include "Server.hpp"

WEBLIBV1::Server::Server(int pDomain, int pService, int pProtocal, int pPort, u_long pInterface, int pBacklog){
    _listeningSock = new ListeningSocket(pDomain, pService, pProtocal, pPort, pInterface, pBacklog);
}


WEBLIBV1::ListeningSocket * WEBLIBV1::Server::Get_Socket(){
    return _listeningSock;
}