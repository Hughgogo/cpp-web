//
//  HHTTPServerOSX.cpp
//  
//  Server with 1 listening socket
//  OSX specification 
//  Event managed by K queue
//
//  Created by theHugh on 13/08/2022.
//


#include "HTTPServerOSX.hpp"

WEBLIBV1::HTTPServerOSX::HTTPServerOSX(int pDomain, int pService, int pProtocal, int pPort, u_long pInterface, int pBacklog) : Server (pDomain, pService, pProtocal, pPort, pInterface, pBacklog)
{
  Start();
  Launch();
}


// Server start
void WEBLIBV1::HTTPServerOSX::Start()
{

  SetUp_kq();

  _running_status = 1;

}

// Create kqueue.
void WEBLIBV1::HTTPServerOSX::SetUp_kq()
{
  _listening_socket_fd = Get_Socket()->Get_Sockfd();
  _kq = kqueue();
  if(_kq ==- 1){
    exit(EXIT_FAILURE);
  }
  /* Register listening socket file descriptor with k q */
  EV_SET(&_sckevent, _listening_socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
  std::cout << "KQ Set up"<<std::endl;
}

// Accept event from listening sockect and resigter event with kqueue 
void WEBLIBV1::HTTPServerOSX::_accepter()
{
  struct sockaddr_in address = Get_Socket()->Get_Address();
  socklen_t addrlen = sizeof(address);
  _new_socket_fd = accept(_listening_socket_fd,(struct sockaddr *) &address,& addrlen);
  int nevent = kevent(_kq,&_sckevent,1,_tevent,EventSize,NULL);
  if(nevent < 0){
    std::cout << "kevent register failed" <<std::endl;
  }
  else{
    std::cout << "kevent registered" <<std::endl;
  }
}

// Retrive event and Process HTTP request
void WEBLIBV1::HTTPServerOSX::_handler()
{
  int nevent = kevent(_kq,&_sckevent,1,_tevent,EventSize,NULL);
    if(nevent < 0){
      std::cout << "kevent retrieve failed" <<std::endl;
    }
    else{
      if(_tevent[0].flags & EV_EOF){
        exit(EXIT_FAILURE);
      }
      else{
        for (int i = 0; i < nevent ;i ++){
          /* Report Error*/
          if (_tevent[i].flags & EV_ERROR){
            fprintf(stderr, "EV_ERROR: %s\n", strerror(_tevent[i].data));
            exit(EXIT_FAILURE);
          }
          /* Retrive event */
          else{
            memset(buffer,0,MaxBufferSize);
            if(read(_new_socket_fd,buffer,MaxBufferSize)< 0){
              std::cout << "Failed to read buffer" << std::endl;
            }
            std::cout << buffer << std::endl;
            //HandleHttpData();
          }
        }
      }
    }
}

// Respond to HTTP request
void WEBLIBV1::HTTPServerOSX::_responder()
{
  char * response = "Simple Hello";
  write(_new_socket_fd,response,strlen(response));
  close(_new_socket_fd);
}

// Launch state
void WEBLIBV1::HTTPServerOSX::Launch()
{
  Listen();
}

// Server continues listening on socket after started 
void WEBLIBV1::HTTPServerOSX::Listen() {

  int current_worker_id = 0;
  std::cout << "Server Start Listening"<<std::endl;
  // Register event with k q
  while (_running_status == 1) {
    _accepter();
    _handler();
  }
}

