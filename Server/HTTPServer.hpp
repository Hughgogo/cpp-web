//
//  HTTPServer.hpp
//  
//  Server with 1 listening socket
//  "ThreadPoolSize" working thread managed by epoll
//
//  Created by theHugh on 11/08/2022.
//


#ifndef HTTPServer_hpp
#define HTTPServer_hpp

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>


#include "../Socket/WEBLIBV1-Socket.hpp"
#include "Server.hpp"
namespace WEBLIBV1
{

    // Buffer Size
    constexpr size_t MaxBufferSize = 4096;


    struct HTTPEventData
    {
        HTTPEventData(): fd(0), length(0), cursor(0), buffer() {}
        void *ptr;
        int fd;
        size_t length;
        size_t cursor;
        char buffer[MaxBufferSize];
    };

    struct epoll_event 
    {
        uint32_t     events;      /* Epoll events */
        struct HTTPEventData data;        /* User data variable */
    };


    class HTTPServer : public Server
    {

        private:
        // Set worker thread pool size to 5
        static constexpr int ThreadPoolSize  = 5;
        // MAX event number for server
        static constexpr int EventSize  = 10000;

        // Server Running status
        int _running_status;
        char buffer[30000] = {0};
        int _listening_socket_fd;
        ListeningSocket * _listeningSock;

        // Thread listening on port
        std::thread _listening_thread;
        // Worker threads, do labor for listening socket
        std::thread _worker_threads[ThreadPoolSize];
        // Worker thread file descriptor
        int _worker_epoll_fd[ThreadPoolSize];
        // Worker event pool
        struct epoll_event _worker_events[ThreadPoolSize][EventSize];
        
        public:
        HTTPServer(int pDomain, int pService, int pProtocal, int pPort, u_long pInterface, int pBacklog);

        void Start();
        void Launch();
        void SetUp_Epoll();
        void Listen();
        void PossessEvents(int pWorker_id);
        void ControlEpollEvent(int epoll_fd, int op, int fd, std::uint32_t events, void *data);
        void HandleEpollEvent(int pEpoll_fd, struct EventData *pData, std::uint32_t pEvents);  
        void Stop();
        
    };
}

#endif /* HTTPServer_hpp */
