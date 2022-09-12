//
//  ConnectingSocket.cpp
//  
//
//  Created by theHugh on 05/07/2022.
//

#include "ConnectingSocket.hpp"

// INSTANTIATE ConnectingSocket Through Constructor
// PASS Parameters to Parent Class Socket
WEBLIBV1::ConnectingSocket::ConnectingSocket(int pDomain, int pService, int pProtocal, int pPort, u_long pInterface) : 
        Socket(pDomain, pService, pProtocal, pPort, pInterface)
        {
            std::cout<< "Socket Connecting" <<std::endl;
            struct sockaddr_in address;
            address = Get_Address();
            Set_Connection(connect(Get_Sockfd(),(struct sockaddr *)&address,sizeof(Get_Address())));
            Test_Connection(Get_Connection());
            std::cout<< "Socket Connected"<<std::endl;
        };

int WEBLIBV1::ConnectingSocket::Connect_Socket(int pSock, struct sockaddr_in pAddress)
{
    return connect(pSock, (struct sockaddr *) &pAddress, sizeof(pAddress));
}
