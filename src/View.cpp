#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include <View.hpp>
#include <Model.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

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

using namespace std;
using namespace Kusadius;

View &View::getInstance(){
   static View view;
   return view;
}

void View::update(){
   Model &model = Model::getInstance();

	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0);

   SDL_Rect clip_background;
   clip_background.x = 0;
   clip_background.y = 0;
   SDL_QueryTexture(toride_background_texture, NULL, NULL, &clip_background.w, &clip_background.h);
   SDL_Rect texture_background;
   texture_background.x = 0;
   texture_background.y = 0;
   texture_background.w = model.screen_width;
   texture_background.h = model.screen_height;
   SDL_RenderCopy(renderer, toride_background_texture, &clip_background, &texture_background);
   // 背景更新

   switch(model.state){
      case Title:
         updateTitle(); 
         break;
      case Select:
         updateSelect();
         break;
      case Play:
         updatePlay();
         break;
      case Gameover:
         updateGameover();
         break;
      case StateNum:
         break;
   }
   // 状態に合わせた更新関数

   SDL_RenderPresent(renderer);
   update_times++;
   // フレーム数と等価
}

View::View(){
   if(initialize()){
      SDL_LogDebug(_cat_system, "View initializing is completed");
   } else{
      SDL_LogDebug(_cat_error, "View could not initializing");
      exit(-1);
   }
   update_times = 0;
}

View::~View(){
}

bool View::initialize() {
   Model &model = Model::getInstance();
   int &width = model.screen_width;
   int &height = model.screen_height;

   // SDL initializing
   if (SDL_Init( SDL_INIT_VIDEO) < 0) {
      SDL_LogDebug( _cat_error, "View::Error SDL_Init");
      return false;
   }

   // creating window
   window = SDL_CreateWindow("Kusadius", SDL_WINDOWPOS_UNDEFINED,
         SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
   if (window) {
      SDL_LogVerbose( _cat_system, "View SDL_CreateWindow %d %d", width, height);
   } else {
      SDL_LogDebug( _cat_error, "View::Error SDL_CreateWindow");
      return false;
   }

   renderer = SDL_CreateRenderer(window, -1, 0);
   if (renderer == NULL) {
      SDL_LogDebug( _cat_error, "View::Error SDL_CreateRenderer");
      return false;
   }

   int flag = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
   if ((IMG_Init(flag) & flag) != flag) {
      SDL_LogDebug( _cat_error, "View::Error IMG_Init");
      return false;
   }

   if (TTF_Init()) { // Returns: 0 on success, -1 on any error
      SDL_LogDebug( _cat_error, "View::Error TTF_Init");
      SDL_Quit();
      return false;
   }

   if(load()){
      SDL_LogDebug( _cat_system, "View - loading files are completed");
   } else{
      return false;
   }

	SDL_SetWindowIcon(window, icon);

   return true;
}

SDL_Texture *View::convertIDToTexture(unsigned int icon_id){
   size_t site = (icon_id&0x000000FF); 
   size_t tip_group = (icon_id&0x0000FF00)>> 8;
   size_t tip_set = (icon_id&0x00FF0000)>> 16;
   //size_t tip = (icon_id&0xFF000000)>> 24;
   // ネットワークバイトオーダなので逆順に格納

   SDL_Texture *return_texture = NULL;
   return_texture = elements_1_set_texture; // default
   try{
      switch(site){
         case 0: // bakeneko
            if(tip_group == 0){
               if(tip_set == 0) return_texture = elements_1_set_texture;
               if(tip_set == 1) return_texture = elements_2_set_texture;
               if(tip_set == 2) return_texture = elements_3_set_texture;
               if(tip_set == 3) return_texture = elements_4_set_texture;
            } else if(tip_group == 1) {
               if(tip_set == 0) return_texture = equipment_set_texture;
               if(tip_set == 1) return_texture = item_set_texture;
               if(tip_set == 2) return_texture = flower_set_texture;
               if(tip_set == 3) return_texture = food_set_texture;
            }
            break;
         case 1:
            if(tip_group == 0){ // fly angel
               if(tip_set == 0) throw "(View) texture is not found";
               if(tip_set == 1) return_texture = flyangel_gold_texture;
               if(tip_set == 2) throw "(View) texture is not found";
               if(tip_set == 3) return_texture = flyangel_orange_texture;
               if(tip_set == 4) throw "(View) texture is not found";
               if(tip_set == 5) throw "(View) texture is not found";
            } else if(tip_group == 1){ // monster
               if(tip_set == 0) return_texture = goblin_red_texture;
               if(tip_set == 1) return_texture = shadow_set_texture;
            }
            break;
         case 2:
            if(tip_group == 0){ // bullet
               if(tip_set == 0) return_texture = bullet_small_texture;
            }
            break;
         case 3:
         default:
            return_texture = bakeneko_texture_array[0];
      }
   } catch(const char *e){
      SDL_LogDebug(_cat_render, e);
   }
   return return_texture;
}

SDL_Rect View::convertIDToRect(unsigned int icon_id){
   size_t site = (icon_id&0x000000FF); 
   size_t tip_group = (icon_id&0x0000FF00)>> 8;
   size_t tip_set = (icon_id&0x00FF0000)>> 16;
   size_t tip = (icon_id&0xFF000000)>> 24;

   //SDL_Log("site:%d tip_group:%d tip_set:%d tip:%d", site, tip_group, tip_set, tip);

   SDL_Rect return_rect;
   switch(site){
      case 0: // bakeneko
         return_rect = bakeneko_set_list[(tip_group==0)?tip_set:tip_set+4][tip];
         break;
      case 1:
         if(tip_group == 0){ // only flyangel
            return_rect = flyangel_rect_vector[tip][(update_times/10)%3];
         } else if(tip_group == 1){ // goblin or shadow
            if(tip_set == 0){ // goblin
               return_rect = goblin_rect_vector[tip][(update_times/10)%3];
            } else if(tip_set == 1){ // shadow
               return_rect = shadow_rect_vector[tip][(update_times/10)%3];
            }
         }
         break;
      case 2:
         return_rect = bullet_small_array[tip];
         break;
      case 3:
      default:
         return_rect = bakeneko_set_list[0][0];
   }
   return return_rect;
}

void View::updateTitle(){
   Model &model = Model::getInstance();
   vector<Object> &object_vector = model.object_vector;

   // text
   vector<Object>::iterator object_begin = object_vector.begin();
   for(;;){
      vector<Object>::iterator object_itr = find(object_begin, object_vector.end(), Text);
      if(object_itr == object_vector.end()) break; // not found

      Object &object = *object_itr;
      std::string message = object.message;
      int font_size = object.font_size;
      SDL_Rect rect;
      SDL_Color black = {0x0,0x0,0x0};
      renderingTextOnWhite(message, FontWhiteRabbit, black, font_size, renderer, rect, model.screen_width*object.position[x], model.screen_height*object.position[y]);

      object_begin = object_itr + 1; // for nect find
   }

   // cursor
   vector<Object>::iterator object_itr =  find(object_vector.begin(), object_vector.end(), Cursor);
   if(object_itr == object_vector.end()){
      // not found
   } else{
      Object &object = *object_itr;
      SDL_Rect texture;
      SDL_Rect clip = convertIDToRect(object.icon_id);//(bakeneko_set_list[0])[0];
      texture.w = clip.w * ((double)model.screen_width/1024.0);
      texture.h = clip.h * ((double)model.screen_height/768.0);
      texture.x = model.screen_width * object.position[x] - texture.w;
      texture.y = model.screen_height * object.position[y] - texture.h;
      SDL_RenderCopy(renderer, convertIDToTexture(object.icon_id), &clip, &texture);
   }

}

void View::updateSelect(){
   Model &model = Model::getInstance();
   vector<Object> &object_vector = model.object_vector;

   // text
   vector<Object>::iterator object_begin = object_vector.begin();
   for(;;){
      vector<Object>::iterator object_itr = find(object_begin, object_vector.end(), Text);
      if(object_itr == object_vector.end()) break; // not found

      Object &object = *object_itr;
      std::string message = object.message;
      int font_size = object.font_size;
      SDL_Rect rect;
      SDL_Color black = {0x0,0x0,0x0};
      renderingTextOnWhite(message, FontWhiteRabbit, black, font_size, renderer, rect, model.screen_width*object.position[x], model.screen_height*object.position[y]);

      object_begin = object_itr + 1; // for nect find
   }

}

void View::updatePlay(){
   Model &model = Model::getInstance();
   vector<Object> &object_vector = model.object_vector;

   try{ // window
      for( unsigned int window = 0 ; window < WindowNum; ++window){
         SDL_Rect texture;
         int render_return;

         texture= model.area[window];
         render_return = SDL_RenderCopy(renderer, white_dot_texture, &white_dot_clip, &texture);
         if( render_return < 0 ) throw "(View) render does not complete.";

         texture = model.hp_area[window];
         render_return = SDL_RenderCopy(renderer, white_dot_texture, &white_dot_clip, &texture);
         if( render_return < 0 ) throw "(View) render does not complete.";

         Object &object = model.getPlayer(static_cast<Window>(window));
         texture.w *= (object.life / object.life_max);
         render_return = SDL_RenderCopy(renderer, white_dot_texture, &white_dot_clip, &texture);
         render_return = SDL_RenderCopy(renderer, white_dot_texture, &white_dot_clip, &texture);
         if( render_return < 0 ) throw "(View) render does not complete.";
      }
   } catch ( const char *e){
      SDL_LogDebug(_cat_render, e);
   }

   try{ // text
      vector<Object>::iterator object_begin = object_vector.begin();
      for(;;){
         vector<Object>::iterator object_itr = find(object_begin, object_vector.end(), Text);
         if(object_itr == object_vector.end()) throw "(View) text is not found.";

         Object &object = *object_itr;
         std::string message = object.message;
         int font_size = object.font_size;
         SDL_Rect rect;
         SDL_Color black = {0x0,0x0,0x0};
         renderingTextOnWhite(message, FontWhiteRabbit, black, font_size, renderer, rect, model.screen_width*object.position[x], model.screen_height*object.position[y]);

         object_begin = object_itr + 1; 
      }
   } catch ( const char *e){
      SDL_LogDebug(_cat_render, e);
   }

   try{ // bullet
      vector<Object>::iterator object_begin = object_vector.begin();
      for(;;){
         vector<Object>::iterator object_itr = find(object_begin, object_vector.end(), Bullet);
         if(object_itr == object_vector.end()) throw "(View) player is not found.";

         Object &object = *object_itr;
         SDL_Rect area = model.area[object.window];
         SDL_Rect texture;
         SDL_Rect clip = convertIDToRect(object.icon_id);//(bakeneko_set_list[0])[0];
         texture.w = clip.w * ((double)model.screen_width/1024.0);
         texture.h = clip.h * ((double)model.screen_height/768.0);
         texture.x = area.x + area.w * object.position[x] - texture.w/ 2;
         texture.y = area.y + area.h * object.position[y] - texture.h/ 2;
         double angle = atan2(object.velocity[y],object.velocity[x]);
         SDL_RenderCopyEx(renderer, convertIDToTexture(object.icon_id), &clip, &texture, convertRadianToDegree(angle) - 90, NULL, SDL_FLIP_NONE);
         //SDL_RenderCopy(renderer, convertIDToTexture(object.icon_id), &clip, &texture);

         draw_circle( texture.x + texture.w/2.0, texture.y + texture.h/2.0, object.radius);
         object_begin = object_itr + 1; // for nect find
      }
   } catch ( const char *e){
      SDL_LogDebug(_cat_render, e);
   }

   try{ // enemy
      vector<Object>::iterator object_begin = object_vector.begin();
      for(;;){
         vector<Object>::iterator object_itr = find(object_begin, object_vector.end(), Enemy);
         if(object_itr == object_vector.end()) throw "(View) enemy is not found.";

         Object &object = *object_itr;
         SDL_Rect area = model.area[object.window];
         SDL_Rect texture;
         SDL_Rect clip = convertIDToRect(object.icon_id);//(bakeneko_set_list[0])[0];
         texture.w = clip.w * ((double)model.screen_width/1024.0);
         texture.h = clip.h * ((double)model.screen_height/768.0);
         texture.x = area.x + area.w * object.position[x] - texture.w/ 2;
         texture.y = area.y + area.h * object.position[y] - texture.h/ 2;
         SDL_RenderCopy(renderer, convertIDToTexture(object.icon_id), &clip, &texture);

         draw_circle( texture.x + texture.w/2.0, texture.y + texture.h/2.0, object.radius);
         object_begin = object_itr + 1; // for nect find
      }
   } catch ( const char *e){
      SDL_LogDebug(_cat_render, e);
   }

   try{ // item
      vector<Object>::iterator object_begin = object_vector.begin();
      for(;;){
         vector<Object>::iterator object_itr = find(object_begin, object_vector.end(), Item);
         if(object_itr == object_vector.end()) throw "(View) item is not found.";

         Object &object = *object_itr;
         SDL_Rect area = model.area[object.window];
         SDL_Rect texture;
         SDL_Rect clip = convertIDToRect(object.icon_id);//(bakeneko_set_list[0])[0];
         texture.w = clip.w * ((double)model.screen_width/1024.0);
         texture.h = clip.h * ((double)model.screen_height/768.0);
         texture.x = area.x + area.w * object.position[x] - texture.w/ 2;
         texture.y = area.y + area.h * object.position[y] - texture.h/ 2;
         SDL_RenderCopy(renderer, convertIDToTexture(object.icon_id), &clip, &texture);

         draw_circle( texture.x + texture.w/2.0, texture.y + texture.h/2.0, object.radius);
         object_begin = object_itr + 1; // for nect find
      }
   } catch ( const char *e){
      SDL_LogDebug(_cat_render, e);
   }

   try{ // player
      vector<Object>::iterator object_begin = object_vector.begin();
      for(;;){
         vector<Object>::iterator object_itr = find(object_begin, object_vector.end(), Player);
         if(object_itr == object_vector.end()) throw "(View) player is not found.";

         Object &object = *object_itr;
         SDL_Rect area = model.area[object.window];
         SDL_Rect texture;
         SDL_Rect clip = convertIDToRect(object.icon_id);//(bakeneko_set_list[0])[0];
         texture.w = clip.w * ((double)model.screen_width/1024.0);
         texture.h = clip.h * ((double)model.screen_height/768.0);
         texture.x = area.x + area.w * object.position[x] - texture.w/ 2;
         texture.y = area.y + area.h * object.position[y] - texture.h/ 2;
         SDL_RenderCopy(renderer, convertIDToTexture(object.icon_id), &clip, &texture);

         draw_circle( texture.x + texture.w/2.0, texture.y + texture.h/2.0, object.radius);

         object_begin = object_itr + 1; // for nect find
      }
   } catch ( const char *e){
      SDL_LogDebug(_cat_render, e);
   }

   try{ // skill
      for (unsigned int window = 0; window < WindowNum; window++) {
         SDL_Rect rect;
         int render_return;

         rect= model.skill_area[window];
         render_return = SDL_RenderCopy(renderer, white_dot_texture, &white_dot_clip, &rect);
         if( render_return < 0 ) throw "(View) render does not complete.";

         for(unsigned int skill = 0; skill< SkillNum; skill++){
            Object &object = model.getPlayer(static_cast<Window>(window));
            unsigned int icon_id = model.skill_icon_id[object.player_type][skill];
            SDL_Rect clip = convertIDToRect(icon_id);//(bakeneko_set_list[0])[0];
            SDL_Rect texture;
            SDL_Rect area = model.skill_area[window];
            texture.w = area.w / 8 * ((double)model.screen_width/1024.0);
            texture.h = area.h * ((double)model.screen_height/768.0);
            texture.x = area.x + area.w * skill / 8.0;
            texture.y = area.y;
            render_return = SDL_RenderCopy(renderer, convertIDToTexture(icon_id), &clip, &texture);
            if( render_return < 0 ) throw "(View) render does not complete.";
            if(object.skill_boolean[skill]){
               render_return = SDL_RenderCopy(renderer, white_dot_texture, &white_dot_clip, &texture);
               if( render_return < 0 ) throw "(View) render does not complete.";
            }
         }

         Object &object = model.getPlayer(static_cast<Window>(window));
         if(object.skill != SkillNum){
            SDL_Rect rect = model.skill_area[window];
            SDL_Rect now_skill; //= model.skill_area[window];
            unsigned int rect_width = now_skill.h / 10.0;
            for(unsigned int width = 0;width<rect_width;width++){
               if(width%2) continue;
               now_skill.x = rect.w*object.skill/8.0 + rect.x + width;
               now_skill.y = rect.y + width;
               now_skill.w = rect.w/8.0 - width * 2.0;
               now_skill.h = rect.h - width * 2.0;
               SDL_RenderDrawRect(renderer,&now_skill);
            }
         }
      }

   } catch (const char *e){
      SDL_LogDebug(_cat_render, e);
   }

   try{ // state
      SDL_Rect rect;
      int render_return;
      for(unsigned int window = 0; window < WindowNum; window++){
         Object &object = model.getPlayer(static_cast<Window>(window));
         rect = model.state_area[window];
         render_return = SDL_RenderCopy(renderer, white_dot_texture, &white_dot_clip, &rect);
         if( render_return < 0 ) throw "(View) render does not complete.";
         SDL_Color black = {0x0,0x0,0x0};
         SDL_Rect clip;
         clip.x = clip.y = 0;
         char state_str[1024];
         char difficulty_str[DifficultyNum][5] = {"Low ","Mid ","High"};
         Difficulty difficulty = model.difficulty[window];

         sprintf(state_str, "Difficulty: %s, Score: %10d",difficulty_str[difficulty],(int)object.score);
         SDL_Texture *state_text = getTextTexture(state_str, FontWhiteRabbit, black, 50, renderer);
         SDL_QueryTexture(state_text, NULL, NULL, &clip.w, &clip.h);
         render_return = SDL_RenderCopy(renderer, state_text, &clip, &rect);
         if( render_return < 0 ) throw "(View) render does not complete.";
         SDL_DestroyTexture(state_text);
      }
   } catch ( const char *e){
      SDL_LogDebug(_cat_render, e);
   }

}

void View::updateGameover(){
   Model &model = Model::getInstance();
   vector<Object> &object_vector = model.object_vector;

   // text
   vector<Object>::iterator object_begin = object_vector.begin();
   for(;;){
      vector<Object>::iterator object_itr = find(object_begin, object_vector.end(), Text);
      if(object_itr == object_vector.end()) break; // not found

      Object &object = *object_itr;
      std::string message = object.message;
      int font_size = object.font_size;
      SDL_Rect rect;
      SDL_Color black = {0x0,0x0,0x0};
      renderingTextOnWhite(message, FontWhiteRabbit, black, font_size, renderer, rect, model.screen_width*object.position[x], model.screen_height*object.position[y]);

      object_begin = object_itr + 1; // for nect find
   }
}

void View::renderingTextOnWhite(const  string message, const  string fonts, SDL_Color color, int font_size, SDL_Renderer *renderer, SDL_Rect &rect, int pos_x, int pos_y) {
   Model &model = Model::getInstance();
   SDL_Texture *message_start = getTextTexture(message.c_str(), fonts.c_str(), color, font_size, renderer);

   SDL_Rect clip;
   clip.x = 0;
   clip.y = 0;
   SDL_QueryTexture(message_start, NULL, NULL, &clip.w, &clip.h);

   SDL_Rect &texture = rect;
   texture.x = pos_x;
   texture.y = pos_y;
   texture.h = clip.h;// * rendering_y_ratio;
   texture.w = clip.w;// * rendering_x_ratio;

   SDL_RenderCopy(renderer, white_dot_texture, &white_dot_clip, &texture);
   SDL_RenderCopy(renderer, message_start, &clip, &texture);
   SDL_DestroyTexture(message_start);
}

SDL_Texture* View::getTextTexture(const  string &message, const  string &fontFile, SDL_Color color, int fontSize, SDL_Renderer *renderer) {
   TTF_Font *font = TTF_OpenFont(fontFile.c_str(), fontSize);
   if (font == nullptr) {
      cerr << "Error: View::getTextTexture::TTF_OpenFont" <<  endl;
      return nullptr;
   }
   SDL_Surface *surf = TTF_RenderText_Blended(font, message.c_str(), color);
   if (surf){ // not null
   } else { // if null
      TTF_CloseFont(font);
      cerr << "Error: View::getTextTexture::TTF_CloseFont" <<  endl;
      return nullptr;
   }
   SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
   if (texture == nullptr) {
      cerr << "Error: View::getTextTexture::CreateTextureFromSurface" <<  endl;
   }
   //Clean up the surface and font
   SDL_FreeSurface(surf);
   TTF_CloseFont(font);

   return texture;
}

void View::draw_circle( int pos_x, int pos_y, double radius){
   if(debug){

      uint8_t r, g, b, a;
      SDL_GetRenderDrawColor(renderer,&r,&g,&b,&a);
      SDL_SetRenderDrawColor(renderer,0x00,0xFF,0x00,0x00);
      for(double angle = 0.0; angle < 360.0; angle += 1){
         int x = radius * sin(angle) + pos_x;
         int y = radius * cos(angle) + pos_y;
         SDL_RenderDrawPoint(renderer, x, y);
      }
      SDL_SetRenderDrawColor(renderer,r,g,b,a);
   }
}
bool View::load(){

   icon = IMG_Load("./image/flaticon/bear.png");
   if(icon){
      SDL_LogVerbose(_cat_system, "View::load() icon");
   } else{
      SDL_LogDebug(_cat_error, "View::load() icon: %s", IMG_GetError());
      return false;
   }

   toride_background = IMG_Load("./image/kajiji/torideyuugata.png");
   if (toride_background) {
      SDL_LogVerbose(_cat_system, "View::load() toride_background");
   } else{
      SDL_LogDebug(_cat_error, "View::load() toride_background: %s", IMG_GetError());
      return false;
   }
   toride_background_texture = SDL_CreateTextureFromSurface(renderer, toride_background);
   if (toride_background_texture) {
      SDL_LogVerbose(_cat_system, "View::load() CreateTextureFromSurface"); 
   } else{
      SDL_LogDebug(_cat_error, "View::load() CreateTextureFromSurface"); 
      return false;
   }

   SDL_FreeSurface(toride_background);
   toride_background = NULL;
   SDL_LogDebug(_cat_system, "View::load() toride background was loaded.");

   flyangel_orange = IMG_Load("./image/makapri/flyangel_orange.png");
   if (flyangel_orange){
   }else{
      cerr << "flyangel orange: " << IMG_GetError() <<  endl;
      return false;
   }
   flyangel_orange_texture = SDL_CreateTextureFromSurface(renderer, flyangel_orange);
   if (flyangel_orange_texture == NULL) {
      cerr << "CreateTextureFromSurface" <<  endl;
      return false;
   }
   SDL_FreeSurface(flyangel_orange);
   flyangel_orange = NULL;
   cout << "flyorange_orange was loaded." <<  endl;

   flyangel_gold = IMG_Load("./image/makapri/flyangel_gold.png");
   if (!flyangel_gold) {
      cerr << "flyangel orange: " << IMG_GetError() <<  endl;
      return false;
   }
   flyangel_gold_texture = SDL_CreateTextureFromSurface(renderer, flyangel_gold);
   if (flyangel_gold_texture == NULL) {
      cerr << "CreateTextureFromSurface" <<  endl;
      return false;
   }
   SDL_FreeSurface(flyangel_gold);
   flyangel_gold = NULL;
   cout << "flyorange_gold was loaded." <<  endl;

   goblin_red = IMG_Load("./image/makapri/goblin_red.png");
   if (!goblin_red) {
      cerr << "gobli red: " << IMG_GetError() <<  endl;
      return false;
   }
   goblin_red_texture = SDL_CreateTextureFromSurface(renderer, goblin_red);
   if (goblin_red_texture == NULL) {
      cerr << "CreateTextureFromSurface" <<  endl;
      return false;
   }
   SDL_FreeSurface(goblin_red);
   goblin_red = NULL;
   cout << "goblin red was loaded." <<  endl;

   shadow_set = IMG_Load("./image/makapri/shadow.png");
   if (!shadow_set) {
      cerr << "shadow set: " << IMG_GetError() <<  endl;
      return false;
   }
   shadow_set_texture = SDL_CreateTextureFromSurface(renderer, shadow_set);
   if (shadow_set_texture == NULL) {
      cerr << "CreateTextureFromSurface" <<  endl;
      return false;
   }
   SDL_FreeSurface(shadow_set);
   shadow_set = NULL;
   cout << "shadow set was loaded." <<  endl;

   bullet_small = IMG_Load("./image/dvdm/bullet_small.png");
   if (!bullet_small) {
      cerr << "bullet small: " << IMG_GetError() <<  endl;
      return false;
   }
   bullet_small_texture = SDL_CreateTextureFromSurface(renderer, bullet_small);
   if (bullet_small_texture == NULL) {
      cerr << "CreateTextureFromSurface" <<  endl;
      return false;
   }
   SDL_FreeSurface(bullet_small);
   bullet_small = NULL;
   cout << "bullet small was loaded." <<  endl;

   equipment_set = IMG_Load("./image/bakenekokan/item_1.png");
   if (!equipment_set) {
      cerr << "equipment_set: " << IMG_GetError() <<  endl;
      return false;
   }
   SDL_Surface* optimized;
   if (equipment_set != NULL) {
      optimized = SDL_ConvertSurfaceFormat(equipment_set, SDL_PIXELFORMAT_RGB24, 0);
      SDL_FreeSurface(equipment_set);
      if (optimized != NULL) {
         SDL_SetColorKey(optimized, SDL_TRUE, SDL_MapRGB(optimized->format, 192, 192, 192));
      }
   }
   equipment_set = optimized;
   equipment_set_texture = SDL_CreateTextureFromSurface(renderer, equipment_set);
   if (equipment_set_texture == NULL) {
      cerr << "CreateTextureFromSurface" <<  endl;
      return false;
   }
   SDL_FreeSurface(equipment_set);
   equipment_set = NULL;
   cout << "equipment set was loaded." <<  endl;

   item_set = IMG_Load("./image/bakenekokan/item_2.png");
   if (!item_set) {
      cerr << "item_set: " << IMG_GetError() <<  endl;
      return false;
   }
   if (item_set != NULL) {
      optimized = SDL_ConvertSurfaceFormat(item_set, SDL_PIXELFORMAT_RGB24, 0);
      SDL_FreeSurface(item_set);
      if (optimized != NULL) {
         SDL_SetColorKey(optimized, SDL_TRUE, SDL_MapRGB(optimized->format, 192, 192, 192));
      }
   }
   item_set = optimized;
   item_set_texture = SDL_CreateTextureFromSurface(renderer, item_set);
   if (item_set_texture == NULL) {
      cerr << "CreateTextureFromSurface" <<  endl;
      return false;
   }
   SDL_FreeSurface(item_set);
   item_set = NULL;
   cout << "item set was loaded." <<  endl;

   flower_set = IMG_Load("./image/bakenekokan/item_3.png");
   if (!flower_set) {
      cerr << "flower_set: " << IMG_GetError() <<  endl;
      return false;
   }
   if (flower_set != NULL) {
      optimized = SDL_ConvertSurfaceFormat(flower_set, SDL_PIXELFORMAT_RGB24, 0);
      SDL_FreeSurface(flower_set);
      if (optimized != NULL) {
         SDL_SetColorKey(optimized, SDL_TRUE, SDL_MapRGB(optimized->format, 192, 192, 192));
      }
   }
   flower_set = optimized;
   flower_set_texture = SDL_CreateTextureFromSurface(renderer, flower_set);
   if (flower_set_texture == NULL) {
      cerr << "CreateTextureFromSurface" <<  endl;
      return false;
   }
   SDL_FreeSurface(flower_set);
   flower_set = NULL;
   cout << "flower set was loaded." <<  endl;

   food_set = IMG_Load("./image/bakenekokan/item_food.png");
   if (!food_set) {
      cerr << "food_set: " << IMG_GetError() <<  endl;
      return false;
   }
   if (food_set != NULL) {
      optimized = SDL_ConvertSurfaceFormat(food_set, SDL_PIXELFORMAT_RGB24, 0);
      SDL_FreeSurface(food_set);
      if (optimized != NULL) {
         SDL_SetColorKey(optimized, SDL_TRUE, SDL_MapRGB(optimized->format, 192, 192, 192));
      }
   }
   food_set = optimized;
   food_set_texture = SDL_CreateTextureFromSurface(renderer, food_set);
   if (food_set_texture == NULL) {
      cerr << "CreateTextureFromSurface" <<  endl;
      return false;
   }
   SDL_FreeSurface(food_set);
   food_set = NULL;
   cout << "food set was loaded." <<  endl;

   elements_1_set = IMG_Load("./image/bakenekokan/four_elements_1.png");
   if (!elements_1_set) {
      cerr << "elements_1_set: " << IMG_GetError() <<  endl;
      return false;
   }
   if (elements_1_set != NULL) {
      optimized = SDL_ConvertSurfaceFormat(elements_1_set, SDL_PIXELFORMAT_RGB24, 0);
      SDL_FreeSurface(elements_1_set);
      if (optimized != NULL) {
         SDL_SetColorKey(optimized, SDL_TRUE, SDL_MapRGB(optimized->format, 192, 192, 192));
      }
   }
   elements_1_set = optimized;
   elements_1_set_texture = SDL_CreateTextureFromSurface(renderer, elements_1_set);
   if (elements_1_set_texture == NULL) {
      cerr << "CreateTextureFromSurface" <<  endl;
      return false;
   }
   SDL_FreeSurface(elements_1_set);
   elements_1_set = NULL;
   cout << "elements 1 set was loaded." <<  endl;

   elements_2_set = IMG_Load("./image/bakenekokan/four_elements_2.png");
   if (!elements_2_set) {
      cerr << "elements_2_set: " << IMG_GetError() <<  endl;
      return false;
   }
   if (elements_2_set != NULL) {
      optimized = SDL_ConvertSurfaceFormat(elements_2_set, SDL_PIXELFORMAT_RGB24, 0);
      SDL_FreeSurface(elements_2_set);
      if (optimized != NULL) {
         SDL_SetColorKey(optimized, SDL_TRUE, SDL_MapRGB(optimized->format, 192, 192, 192));
      }
   }
   elements_2_set = optimized;
   elements_2_set_texture = SDL_CreateTextureFromSurface(renderer, elements_2_set);
   if (elements_2_set_texture == NULL) {
      cerr << "CreateTextureFromSurface" <<  endl;
      return false;
   }
   SDL_FreeSurface(elements_2_set);
   elements_2_set = NULL;
   cout << "elements 2 set was loaded." <<  endl;

   elements_3_set = IMG_Load("./image/bakenekokan/four_elements_3.png");
   if (!elements_3_set) {
      cerr << "elements_3_set: " << IMG_GetError() <<  endl;
      return false;
   }
   if (elements_3_set != NULL) {
      optimized = SDL_ConvertSurfaceFormat(elements_3_set, SDL_PIXELFORMAT_RGB24, 0);
      SDL_FreeSurface(elements_3_set);
      if (optimized != NULL) {
         SDL_SetColorKey(optimized, SDL_TRUE, SDL_MapRGB(optimized->format, 192, 192, 192));
      }
   }
   elements_3_set = optimized;
   elements_3_set_texture = SDL_CreateTextureFromSurface(renderer, elements_3_set);
   if (elements_3_set_texture == NULL) {
      cerr << "CreateTextureFromSurface" <<  endl;
      return false;
   }
   SDL_FreeSurface(elements_3_set);
   elements_3_set = NULL;
   cout << "elements 3 set was loaded." <<  endl;

   elements_4_set = IMG_Load("./image/bakenekokan/four_elements_4.png");
   if (!elements_4_set) {
      cerr << "elements_4_set: " << IMG_GetError() <<  endl;
      return false;
   }
   if (elements_4_set != NULL) {
      optimized = SDL_ConvertSurfaceFormat(elements_4_set, SDL_PIXELFORMAT_RGB24, 0);
      SDL_FreeSurface(elements_4_set);
      if (optimized != NULL) {
         SDL_SetColorKey(optimized, SDL_TRUE, SDL_MapRGB(optimized->format, 192, 192, 192));
      }
   }
   elements_4_set = optimized;
   elements_4_set_texture = SDL_CreateTextureFromSurface(renderer, elements_4_set);
   if (elements_4_set_texture == NULL) {
      cerr << "CreateTextureFromSurface" <<  endl;
      return false;
   }
   SDL_FreeSurface(elements_4_set);
   elements_4_set = NULL;
   cout << "elements 4 set was loaded." <<  endl;

   redhat_set = IMG_Load("./image/tukuttenanbo/redhat.png");
   if (!redhat_set) {
      cerr << "redhat_set: " << IMG_GetError() <<  endl;
      return false;
   }
   redhat_set_texture = SDL_CreateTextureFromSurface(renderer, redhat_set);
   if (redhat_set_texture == NULL) {
      cerr << "CreateTextureFromSurface" <<  endl;
      return false;
   }
   SDL_FreeSurface(redhat_set);
   redhat_set = NULL;
   cout << "redhat set was loaded." <<  endl;

   cat_set = IMG_Load("./image/tukuttenanbo/cat.png");
   if (!cat_set) {
      cerr << "cat_set: " << IMG_GetError() <<  endl;
      return false;
   }
   cat_set_texture = SDL_CreateTextureFromSurface(renderer, cat_set);
   if (cat_set_texture == NULL) {
      cerr << "CreateTextureFromSurface" <<  endl;
      return false;
   }
   SDL_FreeSurface(cat_set);
   cat_set = NULL;
   cout << "cat set was loaded." <<  endl;

   toride_background = IMG_Load("./image/kajiji/torideyuugata.png");
   if (!toride_background) {
      cerr << "toride_background: " << IMG_GetError() <<  endl;
      return false;
   }
   toride_background_texture = SDL_CreateTextureFromSurface(renderer, toride_background);
   if (toride_background_texture == NULL) {
      cerr << "CreateTextureFromSurface" <<  endl;
      return false;
   }
   SDL_FreeSurface(toride_background);
   toride_background = NULL;
   cout << "toride background was loaded." <<  endl;

   // left right up down
   SDL_Rect *flyangel_down_rect = new SDL_Rect[3];
   SDL_Rect *flyangel_left_rect = new SDL_Rect[3];
   SDL_Rect *flyangel_right_rect = new SDL_Rect[3];
   SDL_Rect *flyangel_up_rect = new SDL_Rect[3];
   for (unsigned int x_itr = 0; x_itr < 3; x_itr++) {
      flyangel_down_rect[x_itr].x = flyangel_width * x_itr;
      flyangel_down_rect[x_itr].y = 0;
      flyangel_down_rect[x_itr].w = flyangel_width;
      flyangel_down_rect[x_itr].h = flyangel_height;
      flyangel_left_rect[x_itr].x = flyangel_width * x_itr;
      flyangel_left_rect[x_itr].y = flyangel_height;
      flyangel_left_rect[x_itr].w = flyangel_width;
      flyangel_left_rect[x_itr].h = flyangel_height;
      flyangel_right_rect[x_itr].x = flyangel_width * x_itr;
      flyangel_right_rect[x_itr].y = flyangel_height * 2;
      flyangel_right_rect[x_itr].w = flyangel_width;
      flyangel_right_rect[x_itr].h = flyangel_height;
      flyangel_up_rect[x_itr].x = flyangel_width * x_itr;
      flyangel_up_rect[x_itr].y = flyangel_height * 3;
      flyangel_up_rect[x_itr].w = flyangel_width;
      flyangel_up_rect[x_itr].h = flyangel_height;
   }
   flyangel_rect_vector.push_back(flyangel_down_rect);
   flyangel_rect_vector.push_back(flyangel_left_rect);
   flyangel_rect_vector.push_back(flyangel_right_rect);
   flyangel_rect_vector.push_back(flyangel_up_rect);

   for (unsigned int x_itr = 0; x_itr < sizeof(bullet_small_array) / sizeof(bullet_small_array[0]); x_itr++) {
      bullet_small_array[x_itr].x = bullet_small_width * x_itr;
      bullet_small_array[x_itr].y = 0;
      bullet_small_array[x_itr].w = bullet_small_width;
      bullet_small_array[x_itr].h = bullet_small_height;
   }


   SDL_Rect *goblin_down_rect = new SDL_Rect[3];
   SDL_Rect *goblin_left_rect = new SDL_Rect[3];
   SDL_Rect *goblin_right_rect = new SDL_Rect[3];
   SDL_Rect *goblin_up_rect = new SDL_Rect[3];
   int goblin_width, goblin_height;
   SDL_QueryTexture(goblin_red_texture, NULL, NULL, &goblin_width, &goblin_height);
   goblin_width /= 3;
   goblin_height /= 4;
   for (unsigned int x_itr = 0; x_itr < 3; x_itr++) {
      goblin_down_rect[x_itr].x = goblin_width * x_itr;
      goblin_down_rect[x_itr].y = 0;
      goblin_down_rect[x_itr].w = goblin_width;
      goblin_down_rect[x_itr].h = goblin_height;
      goblin_left_rect[x_itr].x = goblin_width * x_itr;
      goblin_left_rect[x_itr].y = goblin_height;
      goblin_left_rect[x_itr].w = goblin_width;
      goblin_left_rect[x_itr].h = goblin_height;
      goblin_right_rect[x_itr].x = goblin_width * x_itr;
      goblin_right_rect[x_itr].y = goblin_height * 2;
      goblin_right_rect[x_itr].w = goblin_width;
      goblin_right_rect[x_itr].h = goblin_height;
      goblin_up_rect[x_itr].x = goblin_width * x_itr;
      goblin_up_rect[x_itr].y = goblin_height * 3;
      goblin_up_rect[x_itr].w = goblin_width;
      goblin_up_rect[x_itr].h = goblin_height;
   }
   goblin_rect_vector.push_back(goblin_down_rect);
   goblin_rect_vector.push_back(goblin_left_rect);
   goblin_rect_vector.push_back(goblin_right_rect);
   goblin_rect_vector.push_back(goblin_up_rect);

   SDL_Rect clip;
   clip.x = 0;
   clip.y = 0;
   bakeneko_texture_array[0] = equipment_set_texture;
   bakeneko_texture_array[1] = item_set_texture;
   bakeneko_texture_array[2] = flower_set_texture;
   bakeneko_texture_array[3] = food_set_texture;
   bakeneko_texture_array[4] = elements_1_set_texture;
   bakeneko_texture_array[5] = elements_2_set_texture;
   bakeneko_texture_array[6] = elements_3_set_texture;
   bakeneko_texture_array[7] = elements_4_set_texture;
   for (unsigned int set_number = 0; set_number < (sizeof(bakeneko_texture_array) / sizeof(bakeneko_texture_array[0])); ++set_number) {
      SDL_QueryTexture(bakeneko_texture_array[set_number], NULL, NULL, &clip.w, &clip.h);
      for (int y_itr = 0; y_itr < clip.h / bakeneko_item_height; y_itr++) {
         for (int x_itr = 0; x_itr < clip.w / bakeneko_item_width; x_itr++) {
            SDL_Rect texture;
            texture.x = bakeneko_item_width * x_itr;
            texture.y = bakeneko_item_height * y_itr;
            texture.w = bakeneko_item_width;
            texture.h = bakeneko_item_height;
            bakeneko_set_list[set_number].push_back(texture);
         }
      }
   }

   tukutte_texture_array[0] = redhat_set_texture;
   tukutte_texture_array[1] = cat_set_texture;
   //for (unsigned int set_number = 0; set_number < (sizeof(tukutte_texture_array) / sizeof(tukutte_texture_array[0])); ++set_number) {
   SDL_QueryTexture(tukutte_texture_array[0], NULL, NULL, &clip.w, &clip.h);
   unsigned int MovePattern = 4; // Model::MoveStateSizeはSLOWなど他の移動状態も含むので，十字方向のみの4にならない．
   for (unsigned int move_number = 0; move_number < MovePattern; move_number++) {
      SDL_Rect *texture = new SDL_Rect[4];
      for (int x_itr = 0; x_itr < clip.w / tukutte_common_width; x_itr++) {
         texture[x_itr].x = tukutte_common_width * x_itr;
         texture[x_itr].y = tukutte_common_height * move_number;
         texture[x_itr].w = tukutte_common_width;
         texture[x_itr].h = tukutte_common_height;
      }
      tukutte_set_common.push_back(texture);
   }
   // reverse( begin(tukutte_set_common),  end(tukutte_set_common));

   SDL_Rect *shadow_down_rect = new SDL_Rect[3];
   SDL_Rect *shadow_left_rect = new SDL_Rect[3];
   SDL_Rect *shadow_right_rect = new SDL_Rect[3];
   SDL_Rect *shadow_up_rect = new SDL_Rect[3];
   int shadow_width = 0;
   int shadow_height = 0;
   SDL_QueryTexture(shadow_set_texture, NULL, NULL, &shadow_width, &shadow_height);
   shadow_width /= 3;
   shadow_height /= 4;
   for (unsigned int x_itr = 0; x_itr < 3; x_itr++) {
      shadow_down_rect[x_itr].x = shadow_width * x_itr;
      shadow_down_rect[x_itr].y = 0;
      shadow_down_rect[x_itr].w = shadow_width;
      shadow_down_rect[x_itr].h = shadow_height;
      shadow_left_rect[x_itr].x = shadow_width * x_itr;
      shadow_left_rect[x_itr].y = shadow_height;
      shadow_left_rect[x_itr].w = shadow_width;
      shadow_left_rect[x_itr].h = shadow_height;
      shadow_right_rect[x_itr].x = shadow_width * x_itr;
      shadow_right_rect[x_itr].y = shadow_height * 2;
      shadow_right_rect[x_itr].w = shadow_width;
      shadow_right_rect[x_itr].h = shadow_height;
      shadow_up_rect[x_itr].x = shadow_width * x_itr;
      shadow_up_rect[x_itr].y = shadow_height * 3;
      shadow_up_rect[x_itr].w = shadow_width;
      shadow_up_rect[x_itr].h = shadow_height;
   }
   shadow_rect_vector.push_back(shadow_down_rect);
   shadow_rect_vector.push_back(shadow_left_rect);
   shadow_rect_vector.push_back(shadow_right_rect);
   shadow_rect_vector.push_back(shadow_up_rect);

   // キャラごとのスキルチップ格納 //
   for (unsigned int type_number = 0; type_number < PlayerTypeNum; type_number++) {
      //for (unsigned int skill_number = 0; skill_number < PlayerModel::SkillTypeSize; skill_number++) {
      unsigned int texture_number;
      unsigned int texture_id;

      for (unsigned int skill_level = 0; skill_level < SkillTypeNum; ++skill_level) {
         switch (type_number) {
            case Balancer:
               switch (skill_level) {
                  case SKILL_0:
                     texture_number = 0;
                     texture_id = 15 + 16 * 15;
                     break;
                  case SKILL_1:
                     texture_number = 6;
                     texture_id = 0 + 16 * 0;
                     break;
                  case SKILL_2:
                     texture_number = 6;
                     texture_id = 5 + 16 * 1;
                     break;
                  case SKILL_3:
                     texture_number = 0;
                     texture_id = 5 + 16 * 7;
                     break;
                  case SKILL_4:
                     texture_number = 5;
                     texture_id = 10 + 16 * 4;
                     break;
                  case SKILL_5:
                     texture_number = 1;
                     texture_id = 5 + 16 * 7;
                     break;
                  case SKILL_6:
                     texture_number = 3;
                     texture_id = 8 + 16 * 0;
                     break;
                  case SKILL_7:
                     texture_number = 7;
                     texture_id = 2 + 16 * 0;
                     break;
                  case SKILL_8:
                     texture_number = 7;
                     texture_id = 0 + 16 * 6;
                     break;
               }
               //each_character_skill_pattern[type_number].clip.push_back(bakeneko_set_list[skill_level][texture_id]);
               break;
            case Enhancer:// ModelInformation::PlayerType::TYPE_ENHANCER:
               switch (skill_level) {
                  case SKILL_0:
                     texture_number = 0;
                     texture_id = 15 + 16 * 15;
                     break;
                  case SKILL_1:
                     texture_number = 6;
                     texture_id = 0 + 16 * 0;
                     break;
                  case SKILL_2:
                     texture_number = 6;
                     texture_id = 5 + 16 * 1;
                     break;
                  case SKILL_3:
                     texture_number = 4;
                     texture_id = 11 + 16 * 3;
                     break;
                  case SKILL_4:
                     texture_number = 0;
                     texture_id = 6 + 16 * 2;
                     break;
                  case SKILL_5:
                     texture_number = 1;
                     texture_id = 4 + 16 * 7;
                     break;
                  case SKILL_6:
                     texture_number = 1;
                     texture_id = 8 + 16 * 12;
                     break;
                  case SKILL_7:
                     texture_number = 7;
                     texture_id = 0 + 16 * 1;
                     break;
                  case SKILL_8:
                     texture_number = 7;
                     texture_id = 3 + 16 * 1;
                     break;
               }
               break;
            case Heckler:
               switch (skill_level) {
                  case SKILL_0:
                     texture_number = 0;
                     texture_id = 15 + 16 * 15;
                     break;
                  case SKILL_1: // TODO TYPE_HECKLER: skill 1 : 強化（弾増加，威力？）（four_elements_3: 6 - 1,1）
                     texture_number = 6;
                     texture_id = 0 + 16 * 0;
                     break;
                  case SKILL_2: // TODO TYPE_HECKLER: skill 2 : 妨害（敵弾ばらまき多い）（four_elements_3: 6 - 6,2）
                     texture_number = 6;
                     texture_id = 5 + 16 * 1;
                     break;
                  case SKILL_3: // TODO TYPE_HECKLER: skill 3 : 画面下見えなくする（狭い）（four_elements_2: 5 - 9,5）
                     texture_number = 5;
                     texture_id = 8 + 16 * 4;
                     break;
                  case SKILL_4: // TODO TYPE_HECKLER: skill 4 : 今画面にいる敵の強化（four_elements_2: 5 - 5,1）
                     texture_number = 5;
                     texture_id = 4 + 16 * 0;
                     break;
                  case SKILL_5: // TODO TYPE_HECKLER: skill 5 : 全範囲（威力低，雑魚しか倒せない）（item_2: 1 - 4,8）
                     texture_number = 1;
                     texture_id = 3 + 16 * 7;
                     break;
                  case SKILL_6: // TODO TYPE_HECKLER: skill 6 : 相手画面の高速化（敵弾，敵動き）（item_2: 1 - 10,5）
                     texture_number = 1;
                     texture_id = 9 + 16 * 4;
                     break;
                  case SKILL_7: // TODO TYPE_HECKLER: skill 7 : 相手の段階を強制的に自分に揃える（four_elements_4: 7 - 6,2）
                     texture_number = 7;
                     texture_id = 5 + 16 * 1;
                     break;
                  case SKILL_8: // TODO TYPE_HECKLER: skill 8 : 相手に陣にボスクラス複数召喚（four_elements_4: 7 - 2,7）
                     texture_number = 7;
                     texture_id = 1 + 16 * 6;
                     break;
               }
               break;
         }
         each_character_skill_pattern[type_number].texture_set.push_back(bakeneko_texture_array[texture_number]);
         SDL_Texture *texture_set = each_character_skill_pattern[type_number].texture_set[skill_level];
         SDL_QueryTexture(texture_set, NULL, NULL, &clip.w, &clip.h);
         each_character_skill_pattern[type_number].clip.push_back(bakeneko_set_list[texture_number][texture_id]);
         //each_character_skill_pattern[type_number].clip.push_back(bakeneko_set_list[skill_level][texture_id]);
      }
   }

   // 半透明の白テクスチャ生成
   white_dot_clip.x = 0;
   white_dot_clip.y = 0;
   white_dot_clip.w = 1;
   white_dot_clip.h = 1;
   white_dot_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, white_dot_clip.w, white_dot_clip.h);

   unsigned char* bytes = nullptr;
   int pitch = 0;
   SDL_LockTexture(white_dot_texture, nullptr, reinterpret_cast<void**>(&bytes), &pitch);
   unsigned char rgb[] = { 0xFF, 0xFF, 0xFF };
   for (int y = 0; y < white_dot_clip.h; ++y) {
      for (int x = 0; x < white_dot_clip.w; ++x) {
         memcpy(&bytes[(y * white_dot_clip.w + x) * sizeof(rgb)], rgb, sizeof(rgb));
      }
   }
   SDL_SetTextureBlendMode(white_dot_texture, SDL_BLENDMODE_BLEND);
   SDL_SetTextureAlphaMod(white_dot_texture, 0x80);
   SDL_UnlockTexture(white_dot_texture);

   return true;
   }
