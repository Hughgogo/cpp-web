//
//  HTTPServer.cpp
//  
//  Server with 1 listening socket
//  "ThreadPoolSize" working thread managed by epoll
//
//  Created by theHugh on 11/08/2022.
//


#include "HTTPServer.hpp"

WEBLIBV1::HTTPServer::HTTPServer(int pDomain, int pService, int pProtocal, int pPort, u_long pInterface, int pBacklog) : Server (pDomain, pService, pProtocal, pPort, pInterface, pBacklog)
{
  Start();
}

void WEBLIBV1::HTTPServer::Start()
{
    SetUp_Epoll();
    _running_status = 1;

    // Assign and construt listening thread 
    _listening_thread = std::thread(&HTTPServer::Listen, this);

    // Assign and construt working thread
    for (int i = 0;i < ThreadPoolSize; i++){
        _worker_threads[i] = std::thread(&HTTPServer::PossessEvents, this, i);
    }
}


void WEBLIBV1::HTTPServer::SetUp_Epoll()
{
    for (int i = 0; i < ThreadPoolSize; i++) {
    if ((_worker_epoll_fd[i] = epoll_create1(0)) < 0) {
      throw std::runtime_error("Failed to create epoll file descriptor for worker");
    }
  } 
}

// Listen event on socket and assign event to worker 
void WEBLIBV1::HTTPServer::Listen() 
{

  struct HTTPEventData * new_incoming_data;
  int current_worker_id = 0;

  // accept new connections and distribute tasks to worker threads
  while (_running_status == 1) {
    struct sockaddr_in address = Get_Socket()->Get_Address();
    socklen_t addrlen = sizeof(address);
    _listening_socket_fd = accept4(Get_Socket()->Get_Sockfd(),(struct sockaddr *) &address,& addrlen, SOCK_NONBLOCK);

    if (_listening_socket_fd < 0) continue;

    new_incoming_data = new EventData();
    new_incoming_data->fd = _listening_socket_fd;

    ControlEpollEvent(_worker_epoll_fd[current_worker_id], EPOLL_CTL_ADD, _listening_socket_fd, EPOLLIN, new_incoming_data);
    current_worker_id++;
    if (current_worker_id == HTTPServer::ThreadPoolSize) current_worker_id = 0;
  }
}

// Epoll process event
void WEBLIBV1::HTTPServer::PossessEvents(int pWorker_id) 
{
  struct HTTPEventData *data;
  int epoll_fd = _worker_epoll_fd[pWorker_id];

  while (_running_status == 1) {
    int nfds = epoll_wait(_worker_epoll_fd[pWorker_id], worker_events_[pWorker_id], HTTPServer::EventSize, 0);
    if (nfds < 0) {
      continue;
    }

    for (int i = 0; i < nfds; i++) {
      const struct epoll_event& current_event = worker_events_[pWorker_id][i];

      data = reinterpret_cast<HTTPEventData *>(current_event.data.ptr);
      if ((current_event.events & EPOLLHUP) || (current_event.events & EPOLLERR)) {
        ControlEpollEvent(epoll_fd, EPOLL_CTL_DEL, data->fd);
        close(data->fd);
        delete data;
      } else if ((current_event.events == EPOLLIN) || (current_event.events == EPOLLOUT)) {
        HandleEpollEvent(epoll_fd, data, current_event.events);
      } else {  // something unexpected
        ControlEpollEvent(epoll_fd, EPOLL_CTL_DEL, data->fd);
        close(data->fd);
        delete data;
      }
    }
  }
}


// Handle Assigned Epoll Event
void WEBLIBV1::HTTPServer::HandleEpollEvent(int pEpoll_fd, EventData *pData, std::uint32_t pEvents) 
{
  int fd = pData->fd;
  EventData * request, * response;

  if ( pEvents == EPOLLIN) {
    request = pData;
    ssize_t byte_count = recv(fd, request->buffer, MaxBufferSize, 0);
    if (byte_count > 0) {           // we have fully received the message
      response = new EventData();
      response->fd = fd;
      HandleHttpData(*request, response);
      ControlEpollEvent(pEpoll_fd, EPOLL_CTL_MOD, fd, EPOLLOUT, response);
      delete request;
    } else if (byte_count == 0) {   // client has closed connection
      ControlEpollEvent(pEpoll_fd, EPOLL_CTL_DEL, fd);
      close(fd);
      delete request;
    } else {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {  // retry
        request->fd = fd;
        ControlEpollEvent(pEpoll_fd, EPOLL_CTL_MOD, fd, EPOLLIN, request);
      } else {                                        // other error
        ControlEpollEvent(pEpoll_fd, EPOLL_CTL_DEL, fd);
        close(fd);
        delete request;
      }
    }
  } 
  else {
    response = pData;
    ssize_t byte_count = send(fd, response->buffer + response->cursor, response->length, 0);
    if (byte_count >= 0) {
      if (byte_count < response->length) {  // there are still bytes to write
        response->cursor += byte_count;
        response->length -= byte_count;
        ControlEpollEvent(pEpoll_fd, EPOLL_CTL_MOD, fd, EPOLLOUT, response);
      } else {                              // we have written the complete message
        request = new EventData();
        request->fd = fd;
        ControlEpollEvent(pEpoll_fd, EPOLL_CTL_MOD, fd, EPOLLIN, request);
        delete response;
      }
    } else {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {  // retry
        ControlEpollEvent(pEpoll_fd, EPOLL_CTL_ADD, fd, EPOLLOUT, response);
      } else {                                        // other error
        ControlEpollEvent(pEpoll_fd, EPOLL_CTL_DEL, fd);
        close(fd);
        delete response;
      }
    }
  }
}



// Assign incoming HTTP request data with epoll
void WEBLIBV1::HTTPServer::ControlEpollEvent(int epoll_fd, int op, int fd, std::uint32_t events, void *data) 
{
  if (op == EPOLL_CTL_DEL) {
    if (epoll_ctl(epoll_fd, op, fd, nullptr) < 0) {
      throw std::runtime_error("Failed to remove file descriptor");
    }
  } else {
    struct epoll_event ev;
    ev.events = events;
    ev.data.ptr = data;
    if (epoll_ctl(epoll_fd, op, fd, &ev) < 0) {
      throw std::runtime_error("Failed to add file descriptor");
    }
  }
}
