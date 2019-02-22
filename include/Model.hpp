#ifndef _MODEL_HPP__
#define _MODEL_HPP__

#include <vector>

#include <SDL2/SDL.h>

#include <View.hpp>

namespace Kusadius{
   class View;
   class Model;
   class Control;

   class Object;
   class StateInterface;
   class StateTitle;
   class StateSelect;
   class StatePlay;
   class StateGameover;

   enum Type{ Player = 0, Enemy, Bullet, Item, Cursor, Text, TypeNum};
   enum PlayerType{ Balancer = 0, Enhancer, Heckler, PlayerTypeNum};

   enum EnemyType{ Goblin = 0, Shadow, EnemyTypeNum};
   enum ItemType{ LevelUp, ItemTypeNum};
   enum SkillType { SKILL_0 = 0, SKILL_1, SKILL_2, SKILL_3, SKILL_4, SKILL_5, SKILL_6, SKILL_7, SKILL_8, SkillTypeNum };
   enum Skill{ Skill0 = 0, Skill1, Skill2, Skill3, Skill4, Skill5, Skill6, Skill7, SkillNum};
   typedef bool SkillBoolean[SkillNum];
   struct EachCharacterSkillPattern {
      std::vector<SDL_Texture*> texture_set;
      std::vector<SDL_Rect> clip;
   } static  each_character_skill_pattern[PlayerTypeNum];
   enum Window{ A = 0 , B, WindowNum};
   enum OpenParameter{ HP = 0, Skill, OpenParameterNum};
   enum Dimension{ x = 0, y, DimensionNum};
   enum State{ Title = 0, Select, Play, Gameover, StateNum };
   enum Shot{ Straight = 0, Aiming, Spread, ShotNum };
   typedef double Position[DimensionNum];
   typedef double Velocity[DimensionNum];
   typedef double Acceleration[DimensionNum];
   enum RectSetList{ Clip = 0, Texture, RectSetNum};
   typedef SDL_Rect RectSet[RectSetNum];
   typedef unsigned int SkillIconID[PlayerTypeNum][SkillNum];
   static const unsigned int shot_interval = 3;
   static const unsigned int enemy_shot_interval = 60;
   static const unsigned int spawn_interval = 30;
   static const unsigned int spawn_switch_interval = 150;

   struct Key{
      uint16_t q: 1, w: 1, e: 1, r: 1,
               t: 1, y: 1, u: 1, i: 1,
               o: 1, p: 1,
               observedQ;
      uint16_t a: 1, s: 1, d: 1, f: 1,
               g: 1, h: 1, j: 1, k: 1,
               l: 1,
               observedA;
      uint16_t z: 1, x: 1, c: 1, v: 1,
               b: 1, n: 1, m: 1,
               observedZ;
      uint16_t space: 1, escape: 1,
               observedFunc;
   };
   struct Mouse{
      int x;
      int y;
      bool left;
      bool right;
   };
   enum Difficulty{ LowDifficulty = 0, MidDifficulty, HighDifficulty, DifficultyNum}; 
   struct DataSynchronous{
      uint32_t type:4, // 16
               window:2, // 4
               life:8, // 256
               life_max:8, // 256
               skill:4, // 16
               shot_num:4, // 16
               observedA:2;
      uint32_t skill_boolean:8,
               observedB:24;// 8
      uint32_t icon_id;
      Position position;
   };
   struct DataControl{
      uint32_t left:1,
               right:1,
               up:1,
               down:1,
               shot:1,
               skill:1,
               skill_num:3;
   };
   struct DataSystem{
      uint32_t down:1;
   };
   struct DataHeader{
      uint32_t data_id:30,
               data_type:2;
      // data_type 0b00 : system
      // data_type 0b01 : control
      // data_type 0b10 : synchronous
      // data_type 0b11 : emergency shutdown
   };
   struct Ip{
      uint32_t A:8,
               B:8,
               C:8,
               D:8;
   };

   inline double getDistance(Position A, Position B){
      double distance_sum = 0.0;
      for(unsigned int dimension = 0; dimension < DimensionNum; dimension++){
         distance_sum += pow(A[dimension] - B[dimension],2);
      }
      return sqrt(distance_sum);
   }
   inline double getAngle(Position A, Position B){
      return atan2(A[y]-B[y],A[x]-B[x]);
   }
   inline double convertRadianToDegree(double radian){
      return radian * 180.0 / (atan(1.0) * 4.0);
   }
   inline double convertDegreeToRadian(double degree){
      return degree / 180.0 * (atan(1.0) * 4.0);
   }
   inline Window convertUintToWindow(unsigned int uint){
      if(uint == 0) return A;
      if(uint == 1) return B;
      return WindowNum;
   }
   inline unsigned int convertWindowToUint(enum Window window){
      if(window == A) return 0;
      if(window == B) return 1;
      return 2;
   }
   inline void levelUP(enum Skill &skill){
      unsigned int u_skill = skill;
      u_skill = (u_skill<SkillNum)?u_skill+1:0;
      skill = static_cast<enum Skill>(u_skill);
   }

   class Model{
      public:
         friend View;
         friend Control;
         friend StateInterface;
         friend StateTitle;
         friend StateSelect;
         friend StatePlay;
         friend StateGameover;
      public:
         Key key;
         Mouse mouse;
         double cursor_speed;
         static Model& getInstance();
         bool update();
         void test();
         Object &getPlayer(Window window);
         void keychecker();
         void stateswitcher();
      private:
         Model();
         void switchState( State state);
         std::vector<Object> object_vector;
         State state;
         StateTitle *state_title = NULL;
         StateSelect *state_select = NULL;
         StatePlay *state_play = NULL;
         StateGameover *state_gameover = NULL;
         void player_factory(enum Window window);
         void enemy_factory(enum EnemyType enemy_type, enum Window window);
         void bullet_factory( Object& parent);
         void spread_factory( unsigned int shot_num, Object& parent);
         void item_factory(enum ItemType item_type, enum Window window, Position position);
         void cursor_factory();
         void text_factory(std::string message, Position position,  int font_size);
         int screen_width;
         int screen_height;
         int id;
         SDL_Rect area[WindowNum];
         SDL_Rect hp_area[WindowNum];
         SDL_Rect skill_area[WindowNum];
         SDL_Rect state_area[WindowNum];
         Difficulty difficulty[WindowNum];
         unsigned int frame;
         SkillIconID skill_icon_id;
         std::string host;// = "localhost";
   };

   class Object{
      public:
         friend View;
         friend Model;
         friend StateInterface;
         friend StateTitle;
         friend StateSelect;
         friend StatePlay;
         friend StateGameover;
      public:
         Object( Type type);
         Object( const Object &master);
         virtual ~Object();
      private:
         unsigned int id;
         Type type;
         Window window;
         Position position;
         Velocity velocity;
         Acceleration acceleration;
         double radius;
         double life;
         double life_max;
         double score;
         unsigned int icon_id;
         /*
          * bakenekokan  -  0.0.0.0/8
          *    four elements  --  0.0.0.0/16
          *       four elements 1  --  0.0.0.0/24
          *       four elements 2  --  0.0.1.0/24
          *       four elements 3  --  0.0.2.0/24
          *       four elements 4  --  0.0.3.0/24
          *    item           --  0.1.0.0/16
          *       item 1           --  0.1.0.0/24
          *       item 2           --  0.1.1.0/24
          *       item 3           --  0.1.2.0/24
          *       item 4           --  0.1.3.0/24
          * makapri      -  1.0.0.0/8
          *    fly angel      --  1.0.0.0/16
          *       blue             --  1.0.0.0/24
          *       gold             --  1.0.1.0/24
          *       green            --  1.0.2.0/24
          *       orange           --  1.0.3.0/24
          *       pink             --  1.0.4.0/24
          *       purple           --  1.0.5.0/24
          *    monster         --  1.1.0.0/16
          *       red goblin       --  1.1.0.0/24
          *       shadow           --  1.1.1.0/24
          * dvdm         -  2.0.0.0/8
          *    bullet         --  2.0.0.0/16
          *       small            -- 2.0.0.0/24
          */
         std::string message;
         int font_size;
         SDL_Rect area;
         bool is_live;
         unsigned int spawn_tick;
         unsigned int past_frame;
         Type parent_type;
         PlayerType player_type;
         enum Skill skill;
         SkillBoolean skill_boolean;
         unsigned int shot_num;
         EnemyType enemy_type;
         ItemType item_type;
         unsigned int item_counter;
      public:
         bool operator==(const Type &type);
   };

   class StateInterface{
      public:
         StateInterface();
         virtual ~StateInterface()= 0;
         virtual void update() final;
      private:
         virtual void control() = 0;
         virtual void factory() = 0;
         virtual void move() = 0;
         virtual void detection() = 0;
   };

   class StateTitle: public StateInterface{
      public:
         StateTitle();
         virtual ~StateTitle();
      private:
         virtual void control() override;
         virtual void factory() override;
         virtual void move() override;
         virtual void detection() override;
   };

   class StateSelect: public StateInterface{
      public:
         StateSelect();
         virtual ~StateSelect();
      private:
         virtual void control() override;
         virtual void factory() override;
         virtual void move() override;
         virtual void detection() override;
   }; 

   class StatePlay: public StateInterface{
      public:
         StatePlay();
         virtual ~StatePlay();
      private:
         bool finish;
         virtual void control() override;
         virtual void factory() override;
         virtual void move() override;
         virtual void detection() override;
         unsigned int past_spawn[WindowNum];
   };

   class StateGameover: public StateInterface{
      public:
         StateGameover();
         virtual ~StateGameover();
      private:
         virtual void control() override;
         virtual void factory() override;
         virtual void move() override;
         virtual void detection() override;
   };
};
#endif
