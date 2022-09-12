//
//  HHTTPServerOSX.cpp
//  
//  Server with 1 listening socket
//  OSX specification 
//  Event managed by K queue
//
//  Created by theHugh on 13/08/2022.
//


#ifndef HTTPServerOSX_hpp
#define HTTPServerOSX_hpp

#ifndef SOCK_NONBLOCK
#include <fcntl.h>
# define SOCK_NONBLOCK O_NONBLOCK
#endif

#define _GNU_SOURCE             

#define ThreadPoolSize 5
#define EventSize 10000
#define MaxBufferSize 4096

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <thread>


#include "../Socket/WEBLIBV1-Socket.hpp"
#include "Server.hpp"
namespace WEBLIBV1
{

    // Buffer Size
    //constexpr size_t MaxBufferSize = 4096;

    class HTTPServerOSX : public Server
    {

        private:
        // Server Running status
        int _running_status;
        char buffer[30000] = {0};
        int _listening_socket_fd;
        int _new_socket_fd;
        ListeningSocket * _listeningSock;

        // Thread listening on port
        std::thread _listening_thread;
        // worker threads, do labor for listening socket
        std::thread _worker_threads[5];

        struct	kevent _sckevent;	 /* Event we want to monitor */
	    struct	kevent _tevent[10000];	 /* Event triggered */
	    int _kq;                 /* K Queue */
        
        public:
        HTTPServerOSX(int pDomain, int pService, int pProtocal, int pPort, u_long pInterface, int pBacklog);

        void _accepter();
        void _handler();
        void _responder();


        void Start();
        void Launch();
        void SetUp_kq();
        void Listen();

        //void HandleHttpData(const HTTPEventData& pRaw_request, HTTPEventData * pRaw_response);
        //HTTPResponse HandleHttpRequest(const HTTPEventData& pRequest) 
        void Stop();
        
    };
}

#endif /* HTTPServerOSX_hpp */
