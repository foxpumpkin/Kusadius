#include <iostream>
#include <chrono>

#include <SDL2/SDL.h>

#include <Model.hpp>
#include <Control.hpp>
#include <View.hpp>

#include <unistd.h>
#include <arpa/inet.h>

using namespace std;
using namespace Kusadius;
enum {
   DEBUG_CATEGORY_MODEL = SDL_LOG_CATEGORY_CUSTOM,
   DEBUG_CATEGORY_VIEW,
   DEBUG_CATEGORY_CONTROL
};
//typedef std::chrono::time_point<std::chrono::system_clock> chrono_clock
typedef int chrono_clock;
void _setLogPriority(){
   SDL_LogSetPriority( SDL_LOG_CATEGORY_SYSTEM, SDL_LOG_PRIORITY_VERBOSE);
   SDL_LogSetPriority( SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_VERBOSE);
   //SDL_LogSetPriority( DEBUG_CATEGORY_MODEL, SDL_LOG_PRIORITY_VERBOSE);
   SDL_LogSetPriority( DEBUG_CATEGORY_VIEW, SDL_LOG_PRIORITY_CRITICAL);
   SDL_LogSetPriority( DEBUG_CATEGORY_CONTROL, SDL_LOG_PRIORITY_CRITICAL);
}
void adjust_fps(double fps, chrono_clock &__start, chrono_clock &__end);

int main(){
   _setLogPriority();
   // ログレベル修正用


   /*
   while(true){
      if(control.connect("",7014)==1) break;
   }
   */
   /*
   sleep(10);
   return 1;

   while(true){
      if(control.recv()<= 0) break;
      int length = 1000;
      char pop[length];
      int ret = control.popRecvVector(pop,length);
      if(ret<=0) continue;
      SDL_Log("length(%d) %s",ret, pop);
      if(control.send()<= 0) break;
   }
   return 1;
   */

   Model &model = Model::getInstance();
   Key &key = model.key;
   chrono_clock start, end;
   do{
      start = SDL_GetTicks();//std::chrono::system_clock::now();
      //Mouse &mouse = model.mouse;
      { // FPS計算対象の処理
         model.update();
         // 計算
         View &view = View::getInstance();
         view.update();
         // 出力
         Control &control = Control::getInstance();
         control.update();
         // 入力の取得

         //model.keychecker();
         // デバグ用の出力関数
      }
      end = SDL_GetTicks();//std::chrono::system_clock::now();
      adjust_fps(60, start, end);
      // FPS調整用のスリープ関数，フレームスキップは未実装．
   } while( !key.escape);
   return 0;
}


void adjust_fps(double fps, chrono_clock &__start, chrono_clock &__end){
   static double sleep_time = 1.0/ fps* 1000;
   //static std::chrono::duration<double> elapsed_seconds = __end - __start;
   //double elapsed = (double)elapsed_seconds.count()*1000;
   int elapsed = __end - __start;
   int sleep = sleep_time - elapsed;
   //SDL_LogVerbose( SDL_LOG_CATEGORY_SYSTEM, "Adjust FPS sleep %.3lf - %.3lf = %.3lf", sleep_time, elapsed, sleep);
   std::this_thread::sleep_for(std::chrono::microseconds((int)sleep)*1000);
}

