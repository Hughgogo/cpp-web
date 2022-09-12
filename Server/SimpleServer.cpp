//
//  SimpleServer.hpp
//  
//  Simple Server Object
//
//  Created by theHugh on 4/08/2022.
//


#include "SimpleServer.hpp"

WEBLIBV1::SimpleServer::SimpleServer(int pDomain, int pService, int pProtocal, int pPort, u_long pInterface, int pBacklog) : Server (pDomain, pService, pProtocal, pPort, pInterface, pBacklog)
{
    Launch();
}


void WEBLIBV1::SimpleServer::_accepter()
{
    struct sockaddr_in address = Get_Socket()->Get_Address();
    socklen_t addrlen = sizeof(address);
    new_socket = accept(Get_Socket()->Get_Sockfd(),(struct sockaddr *) &address,& addrlen);
    std::cout << "Accepted on New Socket :"<< new_socket << std::endl;
    read(new_socket,buffer,30000);
}

void WEBLIBV1::SimpleServer::_handler()
{
    std::cout << buffer << std::endl;
}

void WEBLIBV1::SimpleServer::_responder()
{
    char * response = "Simple Hello";
    write(new_socket,response,strlen(response));
    close(new_socket);
}

void WEBLIBV1::SimpleServer::Launch()
{
    while (true)
    {
        _accepter();
        _handler();
        _responder();
    }
}