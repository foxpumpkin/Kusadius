/**
 * @file View.hpp
 * @brief View部分<br>
 *        初のグラフィカルユーザインタフェース実装．
 * @author foxpumpkin
 * @date April 27th 2015<br>
 * @date <br>
 */

#ifndef _VIEW_HPP__
#define _VIEW_HPP__

#include <SDL2/SDL.h>

#include <Model.hpp>

#define FontWhiteRabbit "./fonts/white_rabbit/whitrabt.ttf"

namespace Kusadius{
   /**
    * Viewクラス
    * ModelクラスのObject vectorにもとづき，インタフェースを生成する．
    */
   class View{
      public:
         static View &getInstance();
         void update();
         bool debug = false;//true;
      private:
         View();
         virtual ~View();
         bool initialize();
         SDL_Texture *convertIDToTexture(unsigned int icon_id);
         SDL_Rect convertIDToRect(unsigned int icon_id);
         void updateTitle();
         void updateSelect();
         void updatePlay();
         void updateGameover();
         void renderingTextOnWhite(const std::string message, const std::string fonts, SDL_Color color, int font_size, SDL_Renderer *renderer, SDL_Rect &rect, int pos_x, int pos_y);
         SDL_Texture* getTextTexture(const std::string &message, const std::string &fontFile, SDL_Color color, int fontSize, SDL_Renderer *renderer);
         int update_times;

      private:
         SDL_Renderer *renderer = NULL;
         SDL_Window *window = NULL;
         void draw_circle( int pos_x, int pos_y, double radius);

         bool load();

         SDL_Surface *icon = NULL;

         SDL_Surface *flyangel_orange = NULL;
         SDL_Surface *flyangel_gold = NULL;
         SDL_Surface *shadow_set = NULL;
         SDL_Surface *goblin_red = NULL;
         SDL_Surface *bullet_small = NULL;

         SDL_Surface *equipment_set = NULL;
         SDL_Surface *item_set = NULL;
         SDL_Surface *food_set = NULL;
         SDL_Surface *flower_set = NULL;
         SDL_Surface *elements_1_set = NULL;
         SDL_Surface *elements_2_set = NULL;
         SDL_Surface *elements_3_set = NULL;
         SDL_Surface *elements_4_set = NULL;

         SDL_Surface *redhat_set = NULL;
         SDL_Surface *cat_set=NULL;

         SDL_Surface *toride_background = NULL;

         SDL_Texture *flyangel_orange_texture = NULL;
         SDL_Texture *flyangel_gold_texture = NULL;
         SDL_Texture *goblin_red_texture = NULL;
         SDL_Texture *shadow_set_texture = NULL;
         SDL_Texture *bullet_small_texture = NULL;

         SDL_Texture *equipment_set_texture = NULL;
         SDL_Texture *item_set_texture = NULL;
         SDL_Texture *food_set_texture = NULL;
         SDL_Texture *flower_set_texture = NULL;
         SDL_Texture *elements_1_set_texture = NULL;
         SDL_Texture *elements_2_set_texture = NULL;
         SDL_Texture *elements_3_set_texture = NULL;
         SDL_Texture *elements_4_set_texture = NULL;

         SDL_Texture *redhat_set_texture = NULL;
         SDL_Texture *cat_set_texture = NULL;

         SDL_Texture *toride_background_texture = NULL;

         SDL_Texture *white_dot_texture = NULL;
         SDL_Rect white_dot_clip;

         std::vector<SDL_Rect*> flyangel_rect_vector;
         std::vector<SDL_Rect*> goblin_rect_vector;
         int flyangel_width = 41; // 123 px / 3 char
         int flyangel_height = 36; // 144 px / 4 char
         double flyangel_radius = 16.0;
         double bullet_small_radius = 2.0;
         double bakeneko_item_radius = 12.0;
         int bakeneko_item_width = 24; // 384px / 16items
         int bakeneko_item_height = 24;
         SDL_Texture *bakeneko_texture_array[8]; // equipment, item, food, flower
         std::vector<SDL_Rect> bakeneko_set_list[8];
         std::vector<SDL_Rect*> shadow_rect_vector;

         static const int tukutte_common_width = 32;
         static const int tukutte_common_height = 32;
         static const int bullet_small_width = 16;
         static const int bullet_small_height = 16;
         SDL_Texture *tukutte_texture_array[2];
         std::vector<SDL_Rect*> tukutte_set_common;

         SDL_Rect bullet_small_array[5];


         //EachCharacterSkillPattern each_character_skill_pattern[PlayerTypeNum];
   };
};

#endif
