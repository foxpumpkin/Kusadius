#include <iostream>
#include <algorithm>
#include <fstream>

#include <arpa/inet.h>

#include <SDL2/SDL.h>

#include <Model.hpp>
#include <Control.hpp>

using namespace Kusadius;

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

enum {
   DEBUG_CATEGORY_MODEL = SDL_LOG_CATEGORY_CUSTOM,
   DEBUG_CATEGORY_VIEW,
   DEBUG_CATEGORY_CONTROL
};

// ------------ Model ------------//

Model& Model::getInstance(){
   static Model model;
   return model;
}

void Model::test(){
   SDL_LogMessage(_cat_system, _lev_verbose, "test function");
}

/**
 * プレーヤを探す．なければ文字列の例外を返す．
 */
Object &Model::getPlayer(Window window){
   std::vector<Object>::iterator object_itr;
   std::vector<Object>::iterator object_begin = object_vector.begin();
   do{
      object_itr = std::find(object_begin, object_vector.end(), Player);
      if(object_itr == object_vector.end()) throw "(Model) Player is not found.";
      Object &object = *object_itr;
      if(object.window == window) return object;
      object_begin++;
   } while(true);
   return *object_vector.begin();
}

Model::Model(){
   SDL_LogInfo(_cat_system, "model launch");

   state = Title;
   screen_width = 1024;
   screen_height = 768;
   id = 0;
   cursor_speed = 0.01;


   // TODO This generating area methods is not beautiful.
   area[A].x = screen_width  *0.01;
   area[A].w = screen_width  *0.48;
   area[A].y = screen_height *0.01;
   area[A].h = screen_height *0.70;

   area[B].x = (area[A].w + area[A].x + screen_width*0.01) + screen_width*0.01;
   area[B].y = area[A].y;//area[A].h + area[A].y + 1;
   area[B].w = area[A].w;
   area[B].h = area[A].h;

   for(unsigned int window = 0; window < convertWindowToUint(WindowNum); window++){
      hp_area[window].x = area[window].x;
      hp_area[window].w = area[window].w;
      hp_area[window].y = area[window].y * 2 + area[window].h;
      hp_area[window].h = screen_height * 0.05;

      skill_area[window].x = area[window].x;
      skill_area[window].w = area[window].w;
      skill_area[window].y = area[window].y * 3 + area[window].h + hp_area[window].h;
      skill_area[window].h = skill_area[window].w / 8;

      state_area[window].x = area[window].x;
      state_area[window].w = area[window].w;
      state_area[window].y = area[window].y * 4 + area[window].h + hp_area[window].h + skill_area[window].h;
      state_area[window].h = skill_area[window].w / 16;

      difficulty[window] = LowDifficulty;
   }

   frame = 0;

   for(unsigned int player_type = 0; player_type < PlayerTypeNum; player_type++){
      for(unsigned int skill_type = 0; skill_type <SkillNum;skill_type++){
         skill_icon_id[player_type][skill_type] = inet_addr("0.0.0.0");
      }
   }
   skill_icon_id[Balancer][Skill0] = inet_addr("0.0.2.0");
   skill_icon_id[Balancer][Skill1] = inet_addr("0.0.2.21");
   skill_icon_id[Balancer][Skill2] = inet_addr("0.1.0.117");
   skill_icon_id[Balancer][Skill3] = inet_addr("0.0.1.74");
   skill_icon_id[Balancer][Skill4] = inet_addr("0.1.1.117");
   skill_icon_id[Balancer][Skill5] = inet_addr("0.1.3.8");
   skill_icon_id[Balancer][Skill6] = inet_addr("0.0.3.3");
   skill_icon_id[Balancer][Skill7] = inet_addr("0.0.3.96");

   skill_icon_id[Enhancer][Skill0] = inet_addr("0.0.0.0");
   skill_icon_id[Enhancer][Skill1] = inet_addr("0.0.0.0");
   skill_icon_id[Enhancer][Skill2] = inet_addr("0.0.0.0");
   skill_icon_id[Enhancer][Skill3] = inet_addr("0.0.0.0");
   skill_icon_id[Enhancer][Skill4] = inet_addr("0.0.0.0");
   skill_icon_id[Enhancer][Skill5] = inet_addr("0.0.0.0");
   skill_icon_id[Enhancer][Skill6] = inet_addr("0.0.0.0");
   skill_icon_id[Enhancer][Skill7] = inet_addr("0.0.0.0");

   skill_icon_id[Heckler ][Skill0] = inet_addr("0.0.0.0");
   skill_icon_id[Heckler ][Skill1] = inet_addr("0.0.0.0");
   skill_icon_id[Heckler ][Skill2] = inet_addr("0.0.0.0");
   skill_icon_id[Heckler ][Skill3] = inet_addr("0.0.0.0");
   skill_icon_id[Heckler ][Skill4] = inet_addr("0.0.0.0");
   skill_icon_id[Heckler ][Skill5] = inet_addr("0.0.0.0");
   skill_icon_id[Heckler ][Skill6] = inet_addr("0.0.0.0");
   skill_icon_id[Heckler ][Skill7] = inet_addr("0.0.0.0");
}

void Model::switchState( State state){
   switch(state){
      case Title:
         if(state_title){
         } else{
            object_vector.clear();
            state_title = new StateTitle();
            frame = 0;
         }

         if(state_select){
            delete state_select;
            state_select= NULL;
         } else{
         }

         if(state_play){
            delete state_play;
            state_play= NULL;
         } else{
         }

         if(state_gameover){
            delete state_gameover;
            state_gameover= NULL;
         } else{
         }
         break;
      case Select:
         if(state_title){
            delete state_title;
            state_title= NULL;
         } else{
         }

         if(state_select){
         } else{
            object_vector.clear();
            state_select = new StateSelect;
            frame = 0;
         }

         if(state_play){
            delete state_play;
            state_play= NULL;
         } else{
         }

         if(state_gameover){
            delete state_gameover;
            state_gameover= NULL;
         } else{
         }

         break;
      case Play:
         if(state_title){
            delete state_title;
            state_title= NULL;
         } else{
         }

         if(state_select){
            delete state_select;
            state_select= NULL;
         } else{
         }

         if(state_play){
         } else{
            object_vector.clear();
            state_play = new StatePlay;
            frame = 0;
         }

         if(state_gameover){
            delete state_gameover;
            state_gameover= NULL;
         } else{
         }

         break;
      case Gameover:
         if(state_title){
            delete state_title;
            state_title= NULL;
         } else{
         }

         if(state_select){
            delete state_select;
            state_select= NULL;
         } else{
         }

         if(state_play){
            delete state_play;
            state_play= NULL;
         } else{
         }

         if(state_gameover){
         } else{
            object_vector.clear();
            state_gameover = new StateGameover;
            frame = 0;
         }

         break;
      case StateNum:
         break;
   }
}

void Model::player_factory(enum Window window){
   Object object(Player);

   //for(unsigned int window = 0; window < convertUintToWindow(WindowNum); window++){
   object.id = id;
   object.window = convertUintToWindow(window);
   for( size_t dimension = 0; dimension < DimensionNum; dimension++){
      object.position[dimension] = 0.5;
      object.velocity[dimension] = 1.0;
      object.acceleration[dimension] = 0.0;
   }
   object.radius = 16.0;
   object.life_max = object.life = 100.0;
   object.score = 0.0;
   object.player_type = Balancer;

   object.icon_id = inet_addr("1.0.3.3");
   object.skill = SkillNum;
   object.shot_num = 1;

   object_vector.push_back(object);
   id++;
   //}
   SDL_Log("Model factory(Player) %d", object_vector.size());
}

void Model::enemy_factory(enum EnemyType enemy_type, enum Window window){
   Object object(Enemy);

   object.enemy_type = enemy_type;
   object.id = id;
   object.window = window;

   object.position[x] = 0.5;
   object.position[y] = 0.0;
   object.velocity[x] = 0.00;
   object.velocity[y] = 0.005;
   object.acceleration[x] = 0.0;
   object.acceleration[y] = 0.0;

   double level_bonus = (double)(difficulty[window]) * 0.25 + 1.0;
   object.shot_num = 1;
   switch(enemy_type){
      case Goblin:
         object.radius = 10.0;
         object.life_max = object.life = 30.0 * level_bonus;
         object.score = 100.0;
         object.icon_id = inet_addr("1.1.0.0");
         break;
      case Shadow:
         object.radius = 15.0;
         object.life_max = object.life = 100.0 * level_bonus;
         object.score = 1000.0;
         object.icon_id = inet_addr("1.1.1.0");
         object.acceleration[y] = -0.00003;
         break;
      case EnemyTypeNum:
         break;
   }

   object_vector.push_back(object);
   id++;
   SDL_Log("Model factory(Enemy) %d", object_vector.size());
}

void Model::spread_factory(unsigned int shot_num, Object &parent){
   Model &model = Model::getInstance();
   std::vector<Object> &object_vector = model.object_vector;

   for(double angle = 0.0;angle<360;angle+=(360.0/shot_num)){
      Object object(Bullet);
      object.id = id;
      object.window = parent.window;
      for( unsigned int dimension = 0; dimension < DimensionNum; ++dimension){
         object.position[dimension] = parent.position[dimension];
         object.velocity[dimension] = parent.velocity[dimension];
         object.acceleration[dimension] = parent.acceleration[dimension];
      }

      object.radius = 2.0;
      object.score = 0.0;
      object.parent_type = parent.type;

      const double vel = 0.01;
      object.velocity[x]=-cos(convertDegreeToRadian(angle+90.0)) * vel;
      object.velocity[y]=-sin(convertDegreeToRadian(angle+90.0)) * vel;
      switch(parent.type){
         case Player:
            object.life_max = object.life = 30.0;
            object.icon_id = inet_addr("2.0.0.1");
            break;
         case Enemy:
            object.life_max = object.life = 5.0;
            object.icon_id = inet_addr("2.0.0.0");
            break;
         case Bullet:
         case Item:
         case Text:
         case Cursor:
            break;
         case TypeNum:
            throw "(Model) invalid type";
      }
      SDL_LogVerbose(DEBUG_CATEGORY_MODEL, "(Object) bullet id(%u) type(%u) parent.id(%u) parent.type(%u) position(%lf,%lf)",object.id,object.type,parent.id,parent.type, object.position[x],object.position[y]);
      object_vector.push_back(object);
      id++;
   }
}


void Model::bullet_factory(Object &parent){
   Model &model = Model::getInstance();
   std::vector<Object> &object_vector = model.object_vector;
   unsigned int parent_shot_num = parent.shot_num;
   for(unsigned int shot_num = 0; shot_num<parent_shot_num; shot_num++){
      SDL_LogVerbose(DEBUG_CATEGORY_MODEL,
            "(Object) bullet parent id(%u) type(%u) window(%u) position(%lf,%lf) shot_num(%u)",
            parent.id,parent.type,parent.window,
            parent.position[x],parent.position[y],parent.shot_num);
      Object object(Bullet);
      object.id = id;
      object.window = parent.window;
      for( unsigned int dimension = 0; dimension < DimensionNum; ++dimension){
         object.position[dimension] = parent.position[dimension];
         object.velocity[dimension] = parent.velocity[dimension];
         object.acceleration[dimension] = parent.acceleration[dimension];
      }
      object.position[x] += (0.02*shot_num - 0.01*(parent.shot_num-1));

      double angle = 0.0;
      try{
         Object &target = getPlayer(parent.window);
         angle= getAngle(object.position, target.position);
      } catch( const char *e){
         SDL_Log("%s",e);
      }

      object.radius = 2.0;
      object.score = 0.0;
      object.parent_type = parent.type;

      const double vel = 0.01;
      switch(parent.type){
         case Player:
            object.life_max = object.life = 30.0;
            object.velocity[x] = 0.0;
            object.velocity[y] = -vel;
            object.icon_id = inet_addr("2.0.0.1");
            break;
         case Enemy:
            object.life_max = object.life = 5.0;
            object.velocity[x]=-cos(angle) * vel;
            object.velocity[y]=-sin(angle) * vel;
            object.icon_id = inet_addr("2.0.0.0");
            break;
         case Bullet:
         case Item:
         case Text:
         case Cursor:
            break;
         case TypeNum:
            throw "(Model) invalid type";
      }
      SDL_LogVerbose(DEBUG_CATEGORY_MODEL, "(Object) bullet id(%u) type(%u) parent.id(%u) parent.type(%u) position(%lf,%lf)",object.id,object.type,parent.id,parent.type, object.position[x],object.position[y]);
      object_vector.push_back(object);
      id++;
   }
}

void Model::item_factory(ItemType item_type, Window window, Position position){
   Object object(Item);

   object.id = id;
   object.icon_id = inet_addr("0.1.3.3");
   object.window = window;
   object.item_type = item_type;

   object.position[x] = position[x];
   object.position[y] = position[y];
   object.velocity[x] = 0.00;
   object.velocity[y] = -0.005;
   object.acceleration[x] = 0.0;
   object.acceleration[y] = 0.00003;

   double level_bonus = (double)(difficulty[window]) * 0.25 + 1.0;
   object.shot_num = 1;
   switch(item_type){
      case LevelUp:
         object.radius = 10.0;
         object.life_max = object.life = 30.0 * level_bonus;
         object.score = 100.0;
         //object.icon_id = inet_addr("0.1.3.144");
         break;
      case ItemTypeNum:
         break;
   }


   object_vector.push_back(object);
   id++;
   SDL_Log("Model factory(Item) %d", object_vector.size());
}

void Model::cursor_factory(){
   Object object(Cursor);

   object.id = id;
   object.window = A;
   for( size_t dimension = 0; dimension < DimensionNum; dimension++){
      object.position[dimension] = 0.0;
      object.velocity[dimension] = 0.0;
      object.acceleration[dimension] = 0.0;
   }
   object.radius = 0.0;
   object.life = 0.0;
   object.score = 0.0;
   object.icon_id = inet_addr("0.1.1.80");

   object_vector.push_back(object);
   id++;
   SDL_Log("Model factory(Cursor) %d", object_vector.size());
}

void Model::text_factory(std::string message, Position position, int font_size){
   Object object(Text);

   object.id = id;
   object.window = A;
   for( size_t dimension = 0; dimension < DimensionNum; dimension++){
      object.position[dimension] = position[dimension];
      object.velocity[dimension] = 0.0;
      object.acceleration[dimension] = 0.0;
   }
   object.radius = 0.0;
   object.life = 0.0;
   object.score = 0.0;
   object.icon_id = inet_addr("0.1.1.80");
   object.message = message;
   object.font_size = font_size;

   object_vector.push_back(object);
   id++;
   SDL_Log("Model factory %d", object_vector.size());
}

bool Model::update(){
   // 現在の状態にそったインスタンスを生成．
   // NULLでない状態をアップデート．
   switchState(state);
   if(state_title) state_title->update();
   if(state_select) state_select->update();
   if(state_play) state_play->update();
   if(state_gameover) state_gameover->update();

   frame++;

   std::string message;
   message = state_title?"title":"";
   message += state_select?"select":"";
   message += state_play?"play":"";
   message += state_gameover?"gameover":"";
   SDL_Log("frame: %d, now state: %s", frame,message.c_str());

   return true;
}

void Model::keychecker(){
   SDL_LogVerbose( SDL_LOG_CATEGORY_SYSTEM,
         "Control::KeyState - %c %c %c %c %s %s: %c %c %4d %4d",
         (key.w)?'w':' ', (key.a)?'a':' ', (key.s)?'s':' ', (key.d)?'d':' ',
         (key.space)?"spc":"   ", (key.escape)?"esc":"   ",
         (mouse.left)?'l':' ',(mouse.right)?'r':' ', mouse.x, mouse.y);
}

void Model::stateswitcher(){
   if(key.z) state = Title;
   if(key.x) state = Select;
   if(key.c) state = Play;
   if(key.v) state = Gameover;
}

// ---------- Object -------------//

Object::Object( Type type){
   this->type = type;
   type = TypeNum;
   window = WindowNum;
   for(unsigned int dimension=0;dimension<DimensionNum;dimension++){
      position[dimension] = 0.0;
      velocity[dimension] = 0.0;
      acceleration[dimension] = 0.0;
   }
   enemy_type = EnemyTypeNum;
   radius = 0.0;
   life = 0.0;
   life_max = 0.0;
   score = 0.0;
   icon_id = 0;
   message = "NULL";
   font_size = 0;
   SDL_Rect zero = {0}; area = zero;
   is_live = true;
   spawn_tick = SDL_GetTicks();
   past_frame = 0;
   parent_type = TypeNum;
   skill = SkillNum;
   for(unsigned int skill = 0; skill<SkillNum; skill++){
      skill_boolean[skill] = false;
   }
   shot_num = 1;
   item_type = ItemTypeNum;
   item_counter = 0;
}

Object::Object( const Object &master){
   id = master.id;
   type = master.type;
   window = master.window;
   for(uint16_t num = 0; num< convertUintToWindow(WindowNum); ++num){
      position[num] = master.position[num];
      velocity[num] = master.velocity[num];
      acceleration[num] = master.acceleration[num];
   }
   radius = master.radius;
   life = master.life;
   life_max = master.life_max;
   score = master.score;
   icon_id = master.icon_id;
   message = master.message;
   font_size = master.font_size;
   area = master.area;
   is_live = master.is_live;
   spawn_tick = master.spawn_tick;
   past_frame = master.past_frame;
   parent_type = master.parent_type;
   player_type = master.player_type;
   skill = master.skill;
   for(unsigned int skill = 0; skill<SkillNum; skill++){
      skill_boolean[skill] = master.skill_boolean[skill];
   }
   shot_num = master.shot_num;
   enemy_type = master.enemy_type;
   item_type = master.item_type;
   item_counter = master.item_counter;
}

Object::~Object(){
   //SDL_LogVerbose(DEBUG_CATEGORY_MODEL,"(Object) Destructor id(%d) type(%d) window(%d) position(%lf,%lf) life(%lf)",id,type,window,position[x],position[y],life);
}

bool Object::operator==(const Type &type){
   return (this->type == type);
}

// ---------- StateInterface -----//

StateInterface::StateInterface(){
}

StateInterface::~StateInterface(){
}

void StateInterface::update(){
   // SDL_Log("sizeof(DataSet:%d)",sizeof(DataSet));
   // SDL_Log("sizeof(DataHeader:%d)",sizeof(DataHeader));
   // SDL_Log("sizeof(DataControl:%d)",sizeof(DataControl));
   // SDL_Log("sizeof(DataSynchronous:%d)",sizeof(DataSynchronous));
   Model &model = Model::getInstance();

   SDL_LogVerbose(DEBUG_CATEGORY_MODEL,"(StateInterface) control");
   control();

   SDL_LogVerbose(DEBUG_CATEGORY_MODEL,"(StateInterface) factory");
   factory();

   SDL_LogVerbose(DEBUG_CATEGORY_MODEL,"(StateInterface) move");
   move();
   // 領域外に出たインスタンスは破棄
   for(std::vector<Object>::iterator object_itr = model.object_vector.begin(); object_itr< model.object_vector.end(); object_itr++){
      Object &object = *object_itr;
      if(object.position[x]< -0.1) object.is_live = false;
      if(object.position[x]> 1.1) object.is_live = false;
      if(object.position[y]< -0.1) object.is_live = false;
      if(object.position[y]> 1.1) object.is_live = false;
   }

   SDL_LogVerbose(DEBUG_CATEGORY_MODEL,"(StateInterface) detection");
   detection();
   SDL_LogVerbose(DEBUG_CATEGORY_MODEL,"(StateInterface) detection has completed");
   // 死んだオブジェクト(!is_live)の破棄
   std::vector<Object>::iterator object_itr;
   for( object_itr = model.object_vector.begin(); object_itr< model.object_vector.end(); object_itr++){
      Object &object = *object_itr;
      if(object.is_live) continue;
      if(object.type == Player){
         SDL_Log("Player is death.");
      } else {
         model.object_vector.erase(object_itr);
      }
   }

   SDL_LogVerbose(DEBUG_CATEGORY_MODEL,"(StateInterface) updating has completed");
}

// ---------- StateTitle --------//

StateTitle::StateTitle(): StateInterface(){
   SDL_LogDebug(_cat_system, "StateTitle launch");

   // make cursor
   Model &model = Model::getInstance();
   model.cursor_factory();

   // make text
   Position position_title = {0.5, 0.5};
   model.text_factory("KUSADIUS", position_title, 100);

   Position position_state = {0.1, 0.1};
   model.text_factory("press X key to selecting mode", position_state, 25);
}

StateTitle::~StateTitle(){
   SDL_LogDebug(_cat_system, "StateTitle killed");
   Control::resetInstance();
}

void StateTitle::control(){
   Model &model = Model::getInstance();
   model.stateswitcher();
}

void StateTitle::factory(){
}

void StateTitle::move(){
   Model &model = Model::getInstance();
   Key &key = model.key;
   std::vector<Object> &object_vector = model.object_vector;
   std::vector<Object>::iterator object_itr;
   object_itr = std::find(object_vector.begin(), object_vector.end(), Cursor);
   if(object_itr == object_vector.end()){
      // not found
   } else{
      Object &object = *object_itr;
      double cursor_speed = model.cursor_speed;
      object.position[x] += (cursor_speed*(double)key.d);
      object.position[x] -= (cursor_speed*(double)key.a);
      object.position[y] -= (((double)model.screen_width/(double)model.screen_height)*cursor_speed*(double)key.w);
      object.position[y] += (((double)model.screen_width/(double)model.screen_height)*cursor_speed*(double)key.s);

      SDL_Log("Cursor %.3lf,%.3lf - id:%d.%d.%d.%d", object.position[x], object.position[y],
            (object.icon_id&0xFF000000)>>24, (object.icon_id&0xFF0000)>>16, (object.icon_id&0xFF0000)>>8, object.icon_id&0xFF);
   }
}

void StateTitle::detection(){
}

// ---------- StateSelect -------//

StateSelect::StateSelect(): StateInterface(){
   Model &model = Model::getInstance();
   Control::resetInstance();
   SDL_LogDebug(_cat_system, "StateSelect launch");

   std::ifstream host_conf("connection.ini");
   if(host_conf.fail()){ 
      SDL_Log("(StateSelect) could nod open the file for host configuration.");
   } else {
      getline(host_conf,model.host);
      SDL_Log(model.host.c_str());
      //SDL_Delay(100000);
      Position position = {0.1,0.7};
      model.text_factory("server mode : press F key", position, 25);
      position[y]+=0.1;
      char message[100];
      sprintf(message,"client mode : press J key, connect to %s", model.host.c_str());
      model.text_factory(message, position, 25);
   }
   // make cursor
   model.cursor_factory();

   // make text
   Position position_title = {0.5, 0.5};
   model.text_factory("KUSADIUS", position_title, 100);

   //Position position_state = {0.1, 0.1};
   //model.text_factory("Select", position_state, 30);
}

StateSelect::~StateSelect(){
}

void StateSelect::control(){
   Model &model = Model::getInstance();
   model.stateswitcher();
   Key &key = model.key;
   static int roll = 0;
   if(key.f) roll = 1;
   if(key.j) roll = 2;

   Control &control = Control::getInstance();
   if(roll == 1){
      if(control.connect("",7014)==1){
         model.state = Play;
         roll = 0;
      }
   }
   if(roll == 2){
      if(control.connect(model.host,7014)==1){
         model.state = Play;
         roll = 0;
      }
   }
}

void StateSelect::factory(){
}

void StateSelect::move(){
}

void StateSelect::detection(){
}

// ---------- StatePlay ---------//

StatePlay::StatePlay(): StateInterface(){
   SDL_LogDebug(_cat_system, "StatePlay launch");
   //Control::resetInstance();
   finish = false;

   // make cursor
   Model &model = Model::getInstance();
   for(unsigned int window = 0; window < convertUintToWindow(WindowNum); window++){
      model.player_factory(convertUintToWindow(window));
      past_spawn[window] = 0;
   }
}

StatePlay::~StatePlay(){
}

void StatePlay::control(){
   //if(finish) return ;
   Model &model = Model::getInstance();
   model.stateswitcher();
   Control &control = Control::getInstance();

   DataControl *recv_data_control = NULL;
   DataSynchronous *recv_data_synchronous = NULL;
   DataControl *send_data_control = NULL;
   DataSynchronous *send_data_synchronous = NULL;
   DataControl data_control[WindowNum]={0,0};

   // 受信処理
   if(control.isRecvVectorEmpty()){
      SDL_LogVerbose(_cat_system,"recv vector is empty.");
   } else {
      SDL_LogVerbose(_cat_system,"recv vector is not empty.");
   }
   while(!control.isRecvVectorEmpty()){
      unsigned int limit_length = 2000;
      char raw_data[limit_length];
      unsigned int ret = control.popRecvVector(raw_data,limit_length);
      if(ret>limit_length) throw "(StatePlay) invalid data length in recv.";

      DataHeader *header = (DataHeader*)raw_data;
      SDL_Log("(StatePlay) Header %x %x",header->data_id,header->data_type);
      SDL_Log("            %s",raw_data);
      if(header->data_type==0b00){
         //DataSystem *system = (DataSystem *)(raw_data + sizeof(DataHeader));
        // if(system->down){
            Object &target = model.getPlayer(B);
            Control &control = Control::getInstance();
            target.is_live = false;
            finish = true;
            Position position = {0.25, 0.5};
            model.text_factory("WIN",position,50);
            position[x] += 0.5;
            model.text_factory("LOSE",position,50);
        //  }
      } else if(header->data_type==0b01){ // for control
         recv_data_control = (DataControl *)(raw_data+sizeof(DataHeader));
         data_control[B].left = recv_data_control->left;
         data_control[B].right = recv_data_control->right;
         data_control[B].up = recv_data_control->up;
         data_control[B].down = recv_data_control->down;
         data_control[B].shot = recv_data_control->shot;
         data_control[B].skill = recv_data_control->skill;
         data_control[B].skill_num = recv_data_control->skill_num;
         /*
            SDL_Log("!!!!!!!!!! %s %s %s %s %s %s %u !!!!!!!!!!!!",
            (data_control[B].left)?"left":"    ",
            (data_control[B].right)?"right":"     ",
            (data_control[B].up)?"up":"  ",
            (data_control[B].down)?"down":"    ",
            (data_control[B].shot)?"shot":"    ",
            (data_control[B].skill)?"skill":"    ",
            (data_control[B].skill_num)
            );
            */
      } else if(header->data_type==0b10){ // for synchronous
         recv_data_synchronous = (DataSynchronous *)(raw_data+sizeof(DataHeader));
         /*
            SDL_Log("DataControl type(%u) window(%u) life(%u) life_max(%u) skill(%u) shot_num(%u)",
            recv_data_synchronous->type,
            recv_data_synchronous->window,
            recv_data_synchronous->life,
            recv_data_synchronous->life_max,
            recv_data_synchronous->skill,
            recv_data_synchronous->shot_num
            );
            SDL_Log("icon_id(%d) position(%lf,%lf)",
            recv_data_synchronous->icon_id,
            recv_data_synchronous->position[x],
            recv_data_synchronous->position[y]
            );
            */
         // 受信データの埋め込み
         Object &object = model.getPlayer(B);
         // TODO 変換関数作れ
         object.type = static_cast<Type>(recv_data_synchronous->type);
         // object.window = convertUintToWindow(recv_data_synchronous->window);
         object.life = recv_data_synchronous->life;
         object.life_max = recv_data_synchronous->life_max;
         object.skill = static_cast<enum Skill>(recv_data_synchronous->skill);
         object.shot_num = recv_data_synchronous->shot_num;
         for(unsigned int dimension = 0; dimension< DimensionNum; dimension++){
            object.position[dimension] = recv_data_synchronous->position[dimension];
         }
      } else if(header->data_type==0b11){ // for emergency shutdown
         exit(0);
      }
   }

   // シンクロナス（送信）
   SDL_Log("NOW FRAME %d",model.frame);
   if(!(model.frame % 120)){
      SDL_Log("SENDING SENDING SENDING");
      Object &object = model.getPlayer(A);
      unsigned int length = sizeof(DataHeader) + sizeof(DataSynchronous);
      char raw_data[length];

      DataHeader *header = (DataHeader *)raw_data;
      send_data_synchronous = (DataSynchronous *)(raw_data+sizeof(DataHeader));

      header->data_id = 0;
      header->data_type = 0b10;
      send_data_synchronous->type = object.type;
      send_data_synchronous->window = object.window;
      send_data_synchronous->life = (unsigned int)object.life;
      send_data_synchronous->life_max = (unsigned int)object.life_max;
      send_data_synchronous->skill = object.skill;
      send_data_synchronous->shot_num = object.shot_num;
      for(unsigned int num = 0 ; num < SkillNum; num++){
         send_data_synchronous->skill_boolean = 0;
         if(object.skill_boolean[num]){
            send_data_synchronous->skill_boolean = send_data_synchronous->skill_boolean | (int)pow(2,num);
         }
      }
      for( unsigned int dimension = 0; dimension < DimensionNum; dimension++){
         send_data_synchronous->position[dimension] = object.position[dimension];
      }

      control.pushSendVector(raw_data,length);
   }

   Key key = model.key;
   if(key.o){
      Object &object = model.getPlayer(A);
      object.score+= 100000;
   }


   data_control[A].right = key.d;
   data_control[A].left = key.a;
   data_control[A].up = key.w;
   data_control[A].down = key.s;


   Object &object = model.getPlayer(A);
   // ショット
   if(key.space){
      unsigned int interval = model.frame - object.past_frame;
      if(interval > shot_interval){
         data_control[A].shot = 1;
         object.past_frame = model.frame;
      }
   }

   // スキルレベルアップ
   if(key.n){
      levelUP(object.skill);
      //unsigned int skill = object.skill;
      //skill = (skill<SkillNum)?skill+1:0;
      //object.skill = static_cast<enum Skill>(skill);
   }

   // スキル発動
   if(key.j){
      if(!object.skill_boolean[object.skill]){
         data_control[A].skill = 1;
         data_control[A].skill_num = object.skill;
      }
   }

   for(unsigned int window = 0; window < convertWindowToUint(WindowNum); window++){
      try{
         Object &object = model.getPlayer(convertUintToWindow(window));

         double cursor_speed = model.cursor_speed;
         SDL_Rect area = model.area[window];

         object.position[x] += (cursor_speed*data_control[window].right);
         object.position[x] -= (cursor_speed*data_control[window].left);
         object.position[y] -= (((double)area.w/(double)area.h)*cursor_speed*data_control[window].up);
         object.position[y] += (((double)area.w/(double)area.h)*cursor_speed*data_control[window].down);

         if(data_control[window].shot) model.bullet_factory(object);
         if(data_control[window].skill){
            enum Skill skill = static_cast<enum Skill>(data_control[window].skill_num);
            switch(skill){
               case Skill0:
                  object.shot_num++;
                  object.skill_boolean[object.skill] = true;
                  break;
               case Skill1:
                  //if(object.window != A) break;
                  model.enemy_factory(Shadow, (object.window==A)?B:A);
                  break;
               case Skill2:
                  object.life += (0.5*object.life_max);
                  object.life_max *= 1.5;
                  object.skill_boolean[object.skill] = true;
                  break;
               case Skill3:
                  break;
               case Skill4:
                  break;
               case Skill5:
                  break;
               case Skill6:
                  break;
               case Skill7:
                  break;
               case SkillNum:
                  break;
            }
            object.skill = SkillNum;
         }
         //model.spread_factory(60,object);
      } catch(const char *e){
         SDL_LogDebug(_cat_system, e);
      }
   }   

   // コントロール（送信）
   if(!(model.frame % 1)){
      SDL_Log("SENDING CONTROL -- SENDING CONTROL -- SENDING CONTROL");
      unsigned int length = sizeof(DataHeader) + sizeof(DataControl);
      char raw_data[length];

      DataHeader *header = (DataHeader *)raw_data;
      send_data_control = (DataControl *)(raw_data+sizeof(DataHeader));

      header->data_id = 0;
      header->data_type = 0b01;
      send_data_control->left = data_control[A].left;
      send_data_control->right = data_control[A].right;
      send_data_control->up = data_control[A].up;
      send_data_control->down = data_control[A].down;
      send_data_control->shot = data_control[A].shot;
      send_data_control->skill = data_control[A].skill;
      send_data_control->skill_num = data_control[A].skill_num;

      control.pushSendVector(raw_data,length);

      send_data_control = NULL;
   }

}

void StatePlay::factory(){
   if(finish) return ;
   Model &model = Model::getInstance();
   Key key = model.key;

   // ゴブリン自動召喚
   if((model.frame/spawn_switch_interval)%2){
      for(unsigned int window = 0; window< convertUintToWindow(WindowNum); window++){
         double level_bonus = (double)(model.difficulty[window])*0.50 + 1.0;
         unsigned int interval = (model.frame - past_spawn[window])*level_bonus;
         if(interval > spawn_interval){
            model.enemy_factory(Goblin, convertUintToWindow(window));
            past_spawn[window] = model.frame;
         }
      }
   }

   for(unsigned int window = 0; window<WindowNum; window++){
      Object &object = model.getPlayer(convertUintToWindow(window));
      SDL_Log("object.item_factory %u",object.item_counter);
      unsigned int item_interval = 6;
      if((object.item_counter % item_interval) == (item_interval-1)){
         Position position = {0.5,0.5};
         model.item_factory(LevelUp,convertUintToWindow(window),position);
         //object.item_counter = 0;
         object.item_counter++;
      }
   }
   //SDL_Delay(100);

   // ゴブリン手動召喚
   if(key.p){
      for(unsigned int window = 0; window< convertUintToWindow(WindowNum); window++){
         model.enemy_factory(Goblin, convertUintToWindow(window));
      }
   }

   // 敵の定期ショット
   unsigned int now = SDL_GetTicks();
   std::vector<Object> object_vector = model.object_vector; // 参照渡し厳禁
   std::vector<Object>::iterator object_itr;
   for(object_itr = object_vector.begin(); object_itr< object_vector.end(); object_itr++){
      Object &object = *object_itr;
      if(object.type != Enemy) continue;
      double level_bonus = (double)(model.difficulty[object.window]) * 0.25 + 1.0;
      unsigned int difference = (now - object.spawn_tick)*level_bonus;
      if(!(difference % enemy_shot_interval)){
         //SDL_Log("object.id(%d) object.shot_num(%u) object.type(%d) Enemy(%d)",object.id,object.shot_num,object.type,Enemy);
         if(object.enemy_type == Shadow){
            model.spread_factory(60,object);
         } else {
            model.bullet_factory(object);
         }
      }
   }
}

void StatePlay::move(){
   if(finish) return ;
   Model &model = Model::getInstance();
   //Key &key = model.key;
   std::vector<Object> &object_vector = model.object_vector;
   std::vector<Object>::iterator object_itr;
   std::vector<Object>::iterator object_begin;

   for(std::vector<Object>::iterator object_itr = object_vector.begin(); object_itr< object_vector.end(); object_itr++){
      Object &object = *object_itr;
      if(object==Enemy || object == Bullet || object == Item){
         for( unsigned int dimension = 0; dimension < DimensionNum; ++dimension){
            object.velocity[dimension] += object.acceleration[dimension];
            object.position[dimension] += object.velocity[dimension];
         }
      }
   }
}

void StatePlay::detection(){
   if(finish) return ;
   Model &model = Model::getInstance();
   std::vector<Object> &object_vector = model.object_vector;
   std::vector<Object>::iterator object_itr;
   std::vector<Object>::iterator object_begin;
   try{ // enemy
      for( object_itr = object_vector.begin(); object_itr < object_vector.end(); object_itr++){
         Object &object = *object_itr;
         //object_itr = std::find(object_begin, object_vector.end(), Enemy);

         double left_side = 0.0;
         double right_side = 1.0;
         double top_side = 0.0;
         double bottom_side  = 1.0;
         bool over_area = false;

         switch(object.type){
            case Player:
               if(object.position[x]< left_side) object.position[x] = left_side;
               if(object.position[x]> right_side) object.position[x] = right_side;
               if(object.position[y]< top_side) object.position[y] = top_side;
               if(object.position[y]> bottom_side) object.position[y] = bottom_side;
               break;
            case Enemy:
            case Bullet:
            case Item:
               if(object.position[x]<= left_side) over_area = true;
               if(object.position[x]>= right_side) over_area = true;
               if(object.position[y]<= top_side) over_area = true;
               if(object.position[y]>= bottom_side) over_area = true;
               if(over_area){
                  object.is_live = false;
               }
               break;
            case Cursor:
            case Text:
               break;
            case TypeNum:
               throw "(StatePlay) invalid type";
               break;
         }
      }
   } catch(const char *e){
      SDL_LogError(_cat_error,e);
   }

   try{ // collision detection
      for( object_itr = object_vector.begin(); object_itr< object_vector.end(); object_itr++){
         Object &object = *object_itr;
         if(object.type == Bullet){
            if(object.parent_type == Player){ // 敵に対する判定
               std::vector<Object>::iterator target_itr;
               for(target_itr = object_vector.begin();target_itr<object_vector.end();target_itr++){ // 敵の探索
                  Object &target = *target_itr;
                  if(target.type != Enemy) continue; // ターゲットはプレーヤでない
                  if(target.window != object.window) continue; // ウィンドウは同じはず．
                  double radius = (double)(object.radius + target.radius);
                  double distance = getDistance(object.position,target.position) * (double)model.screen_width;
                  if(radius<distance) continue; // 遠い場合はキャンセル
                  double target_life = target.life;
                  double object_life = object.life;
                  target.life-= object_life;
                  if(target.life< 0){
                     target.life = 0;
                     target.is_live = false;
                     Object &parent = model.getPlayer(target.window);
                     parent.score += target.score;
                     parent.item_counter++;
                  }
                  object.life-= target_life;
                  if(object.life< 0){
                     object.life = 0;
                     object.is_live = false;
                  }
               }
            } else if(object.parent_type == Enemy){ // プレーヤに対する反応
               Object &target = model.getPlayer(object.window);
               double radius = (double)(object.radius + target.radius);
               double distance = getDistance(object.position,target.position) * (double)model.screen_width;
               if(radius<distance) continue;
               // SDL_LogVerbose(_cat_system, "radius(%lf), distance(%lf)",radius,distance);
               double target_life = target.life;
               double object_life = object.life;
               target.life-= object_life;
               if(target.life< 0){
                  target.life = 0;
                  if(target.window == A){
                     Control &control = Control::getInstance();
                     target.is_live = false;
                     finish = true;
                     Position position = {0.25, 0.5};
                     model.text_factory("LOSE",position,50);
                     position[x] += 0.5;
                     model.text_factory("WIN",position,50);

                     unsigned int length = sizeof(DataHeader) + sizeof(DataSystem);
                     char data[length];
                     DataHeader *header = (DataHeader *)data;
                     DataSystem *system = (DataSystem *)(data+sizeof(DataHeader));
                     header->data_id = 0;
                     header->data_type = 0b00;
                     system->down = 1;
                     control.pushSendVector(data,length);
                  }
                  // target.life = target.life_max;
               }
               object.life-= target_life;
               if(object.life< 0){
                  object.life = 0;
                  object.is_live = false;
               }
            } else throw "(StatePlay) whoes bullet is this?";
         } else if(object.type == Item){
            Object &target = model.getPlayer(object.window);
            double radius = (double)(object.radius + target.radius);
            double distance = getDistance(object.position,target.position) * (double)model.screen_width;
            if(radius<distance) continue;
            object.is_live = false;
            levelUP(target.skill);
         }
      }
   } catch(const char*e){
      SDL_LogError(_cat_error,e);
   }

   try{
      for(unsigned int window = 0; window<convertUintToWindow(WindowNum); window++){
         Object &player = model.getPlayer(convertUintToWindow(window));
         unsigned int target_window = (window == 0)?1:0;
         if(player.score< 1000){
            model.difficulty[target_window] = LowDifficulty;
         } else if(player.score < 1000000){
            model.difficulty[target_window] = MidDifficulty;
         } else{
            model.difficulty[target_window] = HighDifficulty;
         }
      }
   } catch(const char *e){
      SDL_LogError(_cat_error,e);
   }
}

// ---------- StateGameOver -----//

StateGameover::StateGameover(): StateInterface(){
   SDL_LogDebug(_cat_system, "StateGameover launch");
   Control::resetInstance();

   // make cursor
   Model &model = Model::getInstance();
   model.cursor_factory();

   // make text
   Position position_title = {0.5, 0.5};
   model.text_factory("Gameover", position_title, 100);

   Position position_state = {0.1, 0.1};
   model.text_factory("Gameover", position_state, 30);
}

StateGameover::~StateGameover(){
}

void StateGameover::control(){
   Model &model = Model::getInstance();
   model.stateswitcher();
}

void StateGameover::factory(){
}

void StateGameover::move(){
}

void StateGameover::detection(){
}


