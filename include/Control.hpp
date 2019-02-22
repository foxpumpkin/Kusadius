#ifndef _CONTROL_HPP__
#define _CONTROL_HPP__

#include <thread>
#include <vector>
#include <string>
#include <mutex>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>

#include <Model.hpp>

namespace Kusadius{
   class Control{
      private:
         Control();
      public:
         static Control *instance;
         virtual ~Control();
         static Control &getInstance();
         static void resetInstance();
         void update();
         int connect(std::string ip_str, uint16_t port); // if ip_str = "" then server-mode
         int recv();
         int send();
         void pushRecvVector(const char *buffer, unsigned int length);
         unsigned int popRecvVector(char *buffer, const unsigned int limit_length);
         void pushSendVector(const char *buffer, unsigned int length);
         unsigned int popSendVector(char *buffer, const unsigned int limit_length);
         bool isSendVectorEmpty();
         bool isRecvVectorEmpty();
     private:
         std::vector<char*> send_vector;
         std::mutex send_vector_mutex;
         static std::thread *send_thread;
         std::vector<char*> recv_vector;
         std::mutex recv_vector_mutex;
         static std::thread *recv_thread;
         static TCPsocket base_sock;
         static TCPsocket sock;
         SDLNet_SocketSet set = NULL;
         IPaddress ip;
         static void _send();
         static void _recv();
   };
};

#endif
