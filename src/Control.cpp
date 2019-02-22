#include <Control.hpp>

#include <SDL2/SDL.h>

using namespace Kusadius;
enum {
   DEBUG_CATEGORY_MODEL = SDL_LOG_CATEGORY_CUSTOM,
   DEBUG_CATEGORY_VIEW,
   DEBUG_CATEGORY_CONTROL
};

#ifndef _lev
#define _lev
#define _lev_verbose SDL_LOG_PRIORITY_VERBOSE
#define _lev_debug SDL_LOG_PRIORITY_DEBUG
#define _lev_info SDL_LOG_PRIORITY_INFO
#define _lev_warn SDL_LOG_PRIORITY_WARN
#define _lev_errorSDL_LOG_PRIORITY_ERROR
#define _lev_critical SDL_LOG_PRIORITY_CRITICAL
#endif

#ifndef _cat
#define _lev
#define _cat_application SDL_LOG_CATEGORY_APPLICATION
#define _cat_error SDL_LOG_CATEGORY_ERROR
#define _cat_system SDL_LOG_CATEGORY_SYSTEM
#define _cat_audio SDL_LOG_CATEGORY_AUDIO
#define _cat_video SDL_LOG_CATEGORY_VIDEO
#define _cat_render SDL_LOG_CATEGORY_RENDER
#define _cat_input SDL_LOG_CATEGORY_INPUT
#endif

Control *Control::instance = NULL;
std::thread *Control::send_thread = NULL;
std::thread *Control::recv_thread = NULL;
TCPsocket Control::sock = NULL;
TCPsocket Control::base_sock = NULL;

Control &Control::getInstance(){
   if( !instance ) instance = new Control();
   //static Control control;
   return *instance;
}

void Control::resetInstance(){
   if(!send_thread) delete send_thread;
   if(!recv_thread) delete recv_thread;
   send_thread = recv_thread = NULL;
   if(!sock) SDLNet_TCP_Close(sock);
   if(!base_sock) SDLNet_TCP_Close(base_sock);
   sock = base_sock = NULL;
   if(instance) delete instance;
   instance = NULL;
}

void Control::update(){
   SDL_Event e;
   while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
         SDL_LogDebug(_cat_system, "Control::update() received QUIT signal");
         exit(-1);
      }
   }

   const uint8_t *keyStates = SDL_GetKeyboardState(NULL);
   static Model &model = Model::getInstance();
   Key &key = model.key;
   Mouse &mouse = model.mouse;

   key.q = (keyStates[SDL_SCANCODE_Q])? 1: 0;
   key.w = (keyStates[SDL_SCANCODE_W])? 1: 0;
   key.e = (keyStates[SDL_SCANCODE_E])? 1: 0;
   key.r = (keyStates[SDL_SCANCODE_R])? 1: 0;
   key.t = (keyStates[SDL_SCANCODE_T])? 1: 0;
   key.y = (keyStates[SDL_SCANCODE_Y])? 1: 0;
   key.u = (keyStates[SDL_SCANCODE_U])? 1: 0;
   key.i = (keyStates[SDL_SCANCODE_I])? 1: 0;
   key.o = (keyStates[SDL_SCANCODE_O])? 1: 0;
   key.p = (keyStates[SDL_SCANCODE_P])? 1: 0;

   key.a = (keyStates[SDL_SCANCODE_A])? 1: 0;
   key.s = (keyStates[SDL_SCANCODE_S])? 1: 0;
   key.d = (keyStates[SDL_SCANCODE_D])? 1: 0;
   key.f = (keyStates[SDL_SCANCODE_F])? 1: 0;
   key.g = (keyStates[SDL_SCANCODE_G])? 1: 0;
   key.h = (keyStates[SDL_SCANCODE_H])? 1: 0;
   key.j = (keyStates[SDL_SCANCODE_J])? 1: 0;
   key.k = (keyStates[SDL_SCANCODE_K])? 1: 0;
   key.l = (keyStates[SDL_SCANCODE_L])? 1: 0;

   key.z = (keyStates[SDL_SCANCODE_Z])? 1: 0;
   key.x = (keyStates[SDL_SCANCODE_X])? 1: 0;
   key.c = (keyStates[SDL_SCANCODE_C])? 1: 0;
   key.v = (keyStates[SDL_SCANCODE_V])? 1: 0;
   key.b = (keyStates[SDL_SCANCODE_B])? 1: 0;
   key.n = (keyStates[SDL_SCANCODE_N])? 1: 0;
   key.m = (keyStates[SDL_SCANCODE_M])? 1: 0;

   key.space = (keyStates[SDL_SCANCODE_SPACE])? 1: 0;
   key.escape = (keyStates[SDL_SCANCODE_ESCAPE])? 1: 0;

   const int ret_mouse_state = SDL_GetMouseState(&mouse.x, &mouse.y);
   SDL_PumpEvents();
   mouse.left = (ret_mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT))? true: false;
   mouse.right = (ret_mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT))? true: false;

}

Control::Control(){
   SDL_LogDebug( _cat_system, "launch control");
}

Control::~Control(){
   if(!send_thread) delete send_thread;
   if(!recv_thread) delete recv_thread;
   send_thread = recv_thread = NULL;
   if(!sock) SDLNet_TCP_Close(sock);
   if(!base_sock) SDLNet_TCP_Close(base_sock);
   sock = base_sock = NULL;
}

int Control::connect( std::string ip_str, uint16_t port){
   int ret;
   try{
      if(ip_str == ""){
         SDL_LogInfo(_cat_system, "as a server");

         if(!base_sock){// throw "(Control) there is already the connection";
            //SDL_LogInfo(_cat_system, "connecting to %u.%u.%u.%u",
            //ip_s->A,ip_s->B,ip_s->C,ip_s->D);

            ret = SDLNet_ResolveHost(&ip,NULL,port);
            if(ret==-1){
               char throw_str[256];
               sprintf(throw_str,"(Control) could not resolved(%s)",ip_str.c_str());
               throw throw_str;
            }

            base_sock = SDLNet_TCP_Open(&ip);
            if(!base_sock) throw "(Control) could not open tcp socket.";
         }

         if(!sock){
            sock = SDLNet_TCP_Accept(base_sock);
            if(!sock){
               char throw_str[256];
               sprintf(throw_str,"(Control) could not accept (%s)",SDLNet_GetError());
               throw throw_str;
            }

            SDL_LogInfo(_cat_system, "Accepted");
         }
      } else{
         SDL_LogInfo(_cat_system, "as a client");
         ret = SDLNet_ResolveHost(&ip,ip_str.c_str(),port);
         if(ret==-1){
            char throw_str[256];
            sprintf(throw_str,"(Control) could not resolved(%s)",ip_str.c_str());
            throw throw_str;
         }

         sock = SDLNet_TCP_Open(&ip);
         if(!sock) throw "(Control) could not open tcp socket.";
         SDL_LogInfo(_cat_system, "could open");
      }

      SDL_LogVerbose(_cat_system,"socket was created.");

      set = SDLNet_AllocSocketSet(1);
      if(!set) throw "(Control) socket set was not created.";

      ret = SDLNet_TCP_AddSocket(set,sock);
      if(ret== -1) throw "(Control) could not create tcp socket.";

      //send_thread = new std::thread(&Control::send,this);
      if(!send_thread){ // インスタンスがないときだけ生成すればよい
         send_thread = new std::thread(&Control::_send);
         send_thread->detach();
      }
      if(!recv_thread){
         recv_thread = new std::thread(&Control::_recv);
         recv_thread->detach();
      }

   } catch(const char *e){
      SDL_LogError(_cat_error, e);
      return 0;
   }

   return 1;
}

int Control::recv(){
   // SDL_Log("(Control) recv_vector.size() = %u",recv_vector.size());
   int ret;
   unsigned int length;
   char data[1024];

   try{
      ret = SDLNet_CheckSockets(set,1);
      if(ret == -1) throw "(Control) error on checking sockets.";

      if(SDLNet_SocketReady(sock)){
         length = SDLNet_TCP_Recv(sock,data,1024);
         if(length == 0){
            SDL_LogInfo(_cat_system, "lost connection");
            if(SDLNet_TCP_DelSocket(set,sock)==-1) throw "(Control) socket is not in set.";
            SDLNet_TCP_Close(sock);
            sock = NULL;
            return 0;
         }
         pushRecvVector(data,length);
         //pushSendVector(data,length);
         return length;
      } else {
         //SDL_Log("(control) timed out");//throw "(Control) connection is timed out.";
      }
   } catch(const char *e){
      SDL_LogError(_cat_error, e);
      return -1;
   }
   return 1;
}

int Control::send(){
   //SDL_Log("(Control) send_vector.size() = %u",send_vector.size());
   int ret = 1;
   while(!send_vector.empty()){
      unsigned int length = 1024;
      char data[length];

      /*
         try{
         ret = SDLNet_CheckSockets(set,10000); // 10秒以上通信されないばあいはタイムアウト
         if(ret == -1) throw "(Control) error on checking sockets.";

         if(SDLNet_SocketReady(sock)){
         */
      ret = popSendVector(data,length);
      SDL_LogVerbose(_cat_system,"SENDING LENGTH(%d)",ret);
      if( ret > SDLNet_TCP_Send(sock,data,ret)){
         SDL_LogInfo(_cat_system, "send error, maybe lost connection.");
         if(SDLNet_TCP_DelSocket(set,sock)==-1) throw "(Control) socket is not in set.";
         SDLNet_TCP_Close(sock);
         sock = NULL;
         return 0;
      }
   }
   return ret;
   }

   void Control::pushRecvVector(const char *buffer, unsigned int length){
      std::lock_guard<std::mutex> lock(recv_vector_mutex);
      char *push = new char[length + sizeof(unsigned int)];
      memcpy(push,&length,sizeof(unsigned int)); // 全長の記録
      memcpy(push+sizeof(unsigned int),buffer,length); // ペイロードの記録
      recv_vector.push_back(push);
   }

   unsigned int Control::popRecvVector(char *buffer, const unsigned int limit_length){
      if(recv_vector.size()<= 0) return 0;
      std::lock_guard<std::mutex> lock(recv_vector_mutex);
      char *data = recv_vector.front();
      unsigned int *length = (unsigned int *)data;

      if(limit_length< *length) *length = limit_length;
      memcpy(buffer,data+sizeof(unsigned int),*length);

      recv_vector.erase(recv_vector.begin());

      return *length;
   }

   void Control::pushSendVector(const char *buffer, unsigned int length){
      std::lock_guard<std::mutex> lock(send_vector_mutex);
      char *push = new char[length + sizeof(unsigned int)];
      memcpy(push,&length,sizeof(unsigned int)); // 全長の記録
      memcpy(push+sizeof(unsigned int),buffer,length); // ペイロードの記録
      send_vector.push_back(push);
   }

   unsigned int Control::popSendVector(char *buffer, const unsigned int limit_length){
      std::lock_guard<std::mutex> lock(send_vector_mutex);
      char *data = send_vector.front();
      unsigned int *length = (unsigned int *)data;

      if(limit_length< *length) *length = limit_length;
      memcpy(buffer,data+sizeof(unsigned int),*length);

      send_vector.erase(send_vector.begin());

      return *length;
   }

   bool Control::isSendVectorEmpty(){
      return send_vector.empty();
   }

   bool Control::isRecvVectorEmpty(){
      return recv_vector.empty();
   }

   void Control::_send(){
      Control &control = getInstance();
      do{
         int ret = control.send();
         if(ret<=0) break;
         SDL_Delay(1);
      }while(true);
      Model &model = Model::getInstance();
      model.state = Title;
   }

   void Control::_recv(){
      Control &control = getInstance();
      do{
         int ret = control.recv();
         if(ret<=0) break;
      }while(true);
      Model &model = Model::getInstance();
      model.state = Title;
   }
