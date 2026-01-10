#pragma once
#include "oxlib/math.h"
#include "oxlib/types.h"
#define OXLIB_DEFAULT_CONFIG
#include <oxlib/oxlib.h>
#include <oxlib/result.h>
#include <oxlib/time.h>
#include "../omnix/common.h"

#include <zeus.h>

#include "../modules/window_module.h"
#include "../modules/input_module.h"

#include <locale>
#include <codecvt>




inline std::u32string utf8_to_u32(const std::string& s)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    return conv.from_bytes(s);
}

namespace ui{
    struct engine;
    struct data{
        ox::vec2i mouse_pos;
        bool mouse_clicked[12]{false};
        bool mouse_just_clicked[12]{false};
        int mouse_scroll_force = 0;
        ox::vec2f screen_dimensions;
    };
    enum class anchor{
        top_left,
        top_right,
        bottom_left,
        bottom_right,
        center,
        top_center,
        bottom_center
    };
    struct element{
        ui::engine* engine;
        ui::data* data;
        zs::font::textbatch* font_renderer;
        zs::font::font* default_font;

        ox::i32 texture_id = -33;
        ox::vec2f size;
        ox::vec2f position;
        ox::vec4f color {1,1,1,1};
        std::array<zs::texture::batching::UV,4> QuadTXCoords
        {
            zs::texture::batching::UV{0,0},
            zs::texture::batching::UV{1,0},
            zs::texture::batching::UV{1,1},
            zs::texture::batching::UV{0,1}
        };
        zs::sprite* batch_loc;

        void sync(){
            batch_loc->position = position;
            batch_loc->textureID = texture_id;
            batch_loc->QuadTXCoords = QuadTXCoords;
            batch_loc->scale = size;
            batch_loc->color = color;
        }

        void sync_pos(){
            batch_loc->position = position;
        }
        void sync_tid(){
            batch_loc->textureID = texture_id;
        }
        void sync_txcoords(){
            batch_loc->QuadTXCoords = QuadTXCoords;
        }
        void sync_size(){
            batch_loc->scale = size;
        }
        void sync_color(){
            batch_loc->color = color;
        }

        void dirt(){
            batch_loc->dirty = true;
        }

        virtual void init(){
            //NOTE: one time anchor set
            if(!disable_anchor)
                append_anchor();
            sync();
            dirt();

            for(auto& child:childs){
                child->init();
            }
        };

        bool disable_anchor = false;
        virtual void update(float dt);
        virtual void update_drag(float dt,input_module* input){
            for(auto& ch:childs){
                ch->update_drag(dt,input);
            }
        }
        virtual void dispose() = 0;

        anchor anchor = anchor::center;
        ox::vec2f pivot{0.f,0.f};
        ox::vec2f offset{};

        element* parent;
        std::vector<element*> childs;

        ox::vec2f getAnchorPoint(ui::anchor anchor, ox::vec2f parentPos, ox::vec2f parentSize)
        {
            switch (anchor) {
            case anchor::top_left:     return parentPos + ox::vec2f(-parentSize.x()/2, parentSize.y()/2);
            case anchor::top_right:    return parentPos + ox::vec2f(parentSize.x()/2, parentSize.y()/2);
            case anchor::bottom_left:  return parentPos + ox::vec2f(-parentSize.x()/2, -parentSize.y()/2);
            case anchor::bottom_right: return parentPos + ox::vec2f(parentSize.x()/2, -parentSize.y()/2);
            case anchor::center:      return parentPos;
            case anchor::top_center: return parentPos + ox::vec2f(0,parentSize.y()/2);
            case anchor::bottom_center:  return parentPos + ox::vec2f(0,-parentSize.y()/2);
              break;
            }
            return {};
        }

        bool append_anchor(){
            ox::vec2f parentSize;
            ox::vec2f parentPos;
            if(parent){
                parentSize = parent->size;
                parentPos = parent->position;
            }else{
                parentSize = data->screen_dimensions;
                parentPos = {0,0};
            }
            ox::vec2f anchorPos = getAnchorPoint(anchor, parentPos, parentSize);
            ox::vec2f pivotOffset = size * pivot;

            auto old_pos = position;
            position = anchorPos + offset + pivotOffset;
            zs::dbg2d::line(anchorPos+offset, position, {0,1,0,1});
            zs::dbg2d::line(parentPos, position, {1,0.6,0.6,1});
            return !ox::vec2f::equals(position, old_pos);
        }

        element* add(element*);
        virtual ~element(){
            for(auto& el:childs){
                delete el;
            }
        }

        void set_z(int z);
    };
    struct image:public element{
        image(ox::vec2f position,ox::vec2f size){
            this->position = position;
            this->size = size;
        }
        void init() override{
            element::init();
        }
        void update(float dt) override{
            element::update(dt);
        }
        void dispose() override{
            
        }
    };
    struct button:public element{
        enum class state{
            Idle,
            Hover,
            Click,
            Disabled
        }State;

        OX_TYPEDEF(void(*)(button*,state state,int key,int action), button_callback);

        button_callback C_button = [](button*,state state,int key,int action){
        };

        bool disabled = false;

        button(ox::vec2f position,ox::vec2f scale){
            this->position = position;
            this->size = scale;
        };

        void set_callback(const button_callback& set){
            C_button = set;
        }
        
        void init() override{
            element::init();
        };
        void update(float dt) override{
            if(disabled){
                State = state::Disabled;
                return;
            }else{
                bool is_hovered = ox::vector2<float>::pointInRect({(float)data->mouse_pos.x(),(float)data->screen_dimensions.y()-data->mouse_pos.y()}, position, size);
            if(is_hovered){
                int i = 0;
                for(auto&v:data->mouse_clicked){
                    if(v){
                        State = state::Click;
                        C_button(this,State,i,1);
                    }                   
                    i++; 
                }
                i = 0;
                for(auto&v:data->mouse_just_clicked){
                    if(v){
                        State = state::Click;
                        C_button(this,State,i,2);
                    }                
                    i++;    
                }
                C_button(this,State,-1,-1);

                State = state::Hover;
            }else{
                State = state::Idle;
            }
            }


            auto size = 
            default_font->get('B').width+
            +default_font->get('u').width
            +default_font->get('t').width
            +default_font->get('t').width
            +default_font->get('o').width
            +default_font->get('n').width;

            font_renderer->push_text(*default_font, U"Button", position.x()-(size/2), position.y()-default_font->get('t').height/2);

            element::update(dt);
        };
        void dispose() override{
            C_button = nullptr;
        };
    };


    struct dragger:public element{
        private:
        bool dragging = false;
        ox::vec2f drag_offset;
        bool disabled = false;

        public:
        dragger(ox::vec2f position,ox::vec2f scale){
            this->position = position;
            this->size = scale;
        };

        void init() override{
            element::init();
        };

        void update_drag(float dt,input_module* input)override{ 
            if(disabled){
                return;
            }
            auto mouse_now  = ox::vec2f{(float)input->GetMousePos().x(),(float)input->GetMousePos().y()};
            mouse_now.y() = data->screen_dimensions.y()-mouse_now.y();
            if(!dragging){
                bool is_hovered = ox::vector2<float>::pointInRect(mouse_now, position, size);
                if(is_hovered&&input->IsMouseJustDown(0)){
                    dragging = true;
                    drag_offset = mouse_now - parent->position;
                }
            }else{
                if (input->IsMouseDown(0)) {
                    auto old = parent->position;
                    parent->position = mouse_now - drag_offset;
                    if(!ox::vec2f::equals(parent->position, old)){
                        parent->sync();
                        parent->dirt();
                    }
                } else {
                    dragging = false;
                }

                zs::dbg2d::dot(mouse_now, 5, {1,0,0,1});
            }

            element::update_drag(dt, input);
        }

        void update(float dt) override{
            std::u32string a = U"dragging:"+utf8_to_u32(std::to_string(dragging));
            font_renderer->push_text(*default_font,a.c_str(),60,60);
            element::update(dt);
        };
        void dispose() override{
        };
    };


    template<typename T = int>
    struct slider:public element{
        struct slider_button:public element{
            slider* slider;
            slider_button(ox::vec2f position,ox::vec2f scale){
                this->position = position;
                this->size = scale;
            };
            void init() override{
                slider = dynamic_cast<struct slider*>(this->parent);
                element::init();
            };
        
            bool dragging = false;
            bool disabled = false;
            int current_step = 0;
        
            void update_drag(float dt, input_module* input) override{
                if (disabled) return;
            
                auto raw = input->GetMousePos();
                auto mouse_now = ox::vec2f{(float)raw.x(),(float)raw.y()};
                auto local_mouse_x = mouse_now.x();
                mouse_now.y() = data->screen_dimensions.y() - mouse_now.y();
                
                bool is_hovered_mtr = ox::vec2f::pointInRect(mouse_now, parent->position, parent->size);
                if(is_hovered_mtr){
                    if(input->IsMouseJustDown(0)){
                        float local_mouse_x = mouse_now.x();
                    
                        float min_pos = slider->position.x() - slider->size.x()/2.0f + this->size.x()/2.0f;
                        float max_pos = slider->position.x() + slider->size.x()/2.0f - this->size.x()/2.0f;
                    
                        if (local_mouse_x < min_pos) local_mouse_x = min_pos;
                        if (local_mouse_x > max_pos) local_mouse_x = max_pos;
                    
                        float t = (local_mouse_x - min_pos) / (max_pos - min_pos);
                    
                        int target_step = (int)(t * slider->step);

                        if (target_step != current_step) {
                            current_step = target_step;
                            slider->set_step(current_step);
                        }
                    }
                }

                if (!dragging) {
                    bool is_hovered = ox::vec2f::pointInRect(mouse_now, position, size);
                    if (is_hovered && input->IsMouseJustDown(0)) {
                        dragging = true;
                    }
                } else {
                    if (input->IsMouseDown(0)) {
                        float local_mouse_x = mouse_now.x();
                    
                        float min_pos = slider->position.x() - slider->size.x()/2.0f + this->size.x()/2.0f;
                        float max_pos = slider->position.x() + slider->size.x()/2.0f - this->size.x()/2.0f;
                    
                        if (local_mouse_x < min_pos) local_mouse_x = min_pos;
                        if (local_mouse_x > max_pos) local_mouse_x = max_pos;
                    
                        float t = (local_mouse_x - min_pos) / (max_pos - min_pos);
                    
                        int target_step = (int)(t * slider->step);

                        if (target_step != current_step) {
                            current_step = target_step;
                            slider->set_step(current_step);
                        }
                    } else {
                        dragging = false;
                    }
                }
                element::update_drag(dt, input);
            }
            void update(float dt) override{
                element::update(dt);
            };
            void dispose() override{
            };
        };

    private:
        int step;
        int value_count = 0;
        float zero_offset = 0;
        T max_v,min_v;
    public:
        int current_step = 0;
        slider_button* button;
        slider(ox::vec2f position,ox::vec2f scale,T min,T max,int step){
            this->position = position;
            this->size = scale;
            this->step = step;
            this->min_v = min;
            this->max_v = max;
        };

        void init() override{
            button = (slider_button*)add(new slider_button{{},{size.x()/step, size.y()}});
            button->color = {0,1,0,1};
            zero_offset = -size.x()/2.0f + button->size.x()/2.0f;
            button->offset.x() = zero_offset;
            element::init();
        };

        void set_step(int x) {
            if (x > step || x < 0) return;
            float ratio = (float)x / (float)step;
            float travel_range = size.x() - button->size.x();
            button->offset.x() = zero_offset + (ratio * travel_range);
            button->sync();
            button->dirt();
            current_step = x;
        }

        T get_value() {
            float t = (float)current_step / (float)step;

            if constexpr (std::is_integral_v<T>) {
                return min_v + (T)((max_v - min_v) * t);
            }
            else if constexpr (std::is_floating_point_v<T>) {
                return min_v + (T)((max_v - min_v) * t);
            }
        }

        void update_drag(float dt,input_module* input) override{
            element::update_drag(dt, input);
        }
        void update(float dt) override{
            element::update(dt);
        };
        void dispose() override{
        };
    };
    struct checkbox:public button{
        public:
        element* check_texture;
        bool checked = false;
        checkbox(ox::vec2f position,ox::vec2f size):button(position,size){}

        void(*CallBack)(checkbox* self,bool checked);

        bool disable_callback_update = false;
        void callback(void(*call)(checkbox* self,bool checked)){
            CallBack = call;
        }

        void init() override{
            check_texture = add(new image{{},size/1.5});
            check_texture->color = {1,0,0,1};
            set_callback([](button * self, state, int key, int action){
                if(key==0&&action==2){
                    auto s =  (checkbox*)(self);
                    s->checked = !s->checked;
                }
            });
            button::init();
        }
        void update(float dt) override{
            if(!disable_callback_update)
                CallBack(this,checked);

            button::update(dt);
        }
        void dispose() override{
            button::dispose();
        }
    };

    struct layout{
        virtual bool format(
        size_t index,
        size_t count,
        const ox::vec2f& parent_pos,
        const ox::vec2f& parent_size,
        ox::vec2f& out_pos,
        const ox::vec2f& child_size
        ) const = 0;

        virtual ~layout() = default;
    };
    struct vertical_layout : layout {
        bool format(
            size_t index,
            size_t count,
            const ox::vec2f& parent_pos,
            const ox::vec2f& parent_size,
            ox::vec2f& pos,
            const ox::vec2f& child_size
        ) const override {
            if (count == 0) return false;

            auto old_pos = pos;
            float total_height = parent_size.y();
            float step = total_height / count;

            float top = parent_pos.y() - total_height * 0.5f;

            pos.y() = top
                   + step * index
                   + step * 0.5f;

            pos.x() = parent_pos.x();

            return !ox::vec2f::equals(old_pos, pos);
        }
    };
    struct panel:public element{
        layout* layout;

        panel(ox::vec2f position,ox::vec2f scale){
            this->position = position;
            this->size = scale;
        }

        void init() override{
            for(auto& child:childs){
                child->disable_anchor = true;
            }
            element::init();
        }
        void update(float dt) override;

        void dispose() override{
            delete layout;
        }
        void update_drag(float dt, input_module *input) override{
            element::update_drag(dt, input);
        }
    };

    struct root:public element{
        root(ox::vec2f position,ox::vec2f scale){
            this->position = position;
            this->size = scale;
        }

        void init() override{
            element::init();
        }
        void update(float dt) override;
        void update_drag(float dt, input_module *input) override{
            element::update_drag(dt, input);
        }
        void dispose() override{

        }
    };


    struct renderer{
        zs::ortho_camera LocalCamera;
        zs::spritebatch batch;
        void add_element(element& element){
            element.batch_loc = batch.add_sprite();
        }
        void dirt_element(ui::element& element){
            batch.mark_dirty(*element.batch_loc);
        }
    };
    struct engine{
        renderer* Renderer;
        zs::font::textbatch FontRenderer;

        zs::font::font Font;

        data data;

        std::vector<element*> elements;

        void add_element(element* element){
            element->engine = this;
            Renderer->add_element(*element);
            element->sync();
            Renderer->dirt_element(*element);
            elements.push_back(element);
            element->data = &data;

            element->font_renderer = &FontRenderer;
            element->default_font = &Font;
        }

        void init_renderer(){
            Renderer->batch.init();
            Font.load("../../ARIAL.TTF", 25.0f);
        }

        void init(){
            for(auto& el:elements){
                el->init();
            }
        }
        void update_drag(float dt,input_module* input){
            for(auto& el:elements){
                el->update_drag(dt,input);
            }
        }
        void update(float dt){
            for(auto& el:elements){
                el->update(dt);
            }
        }
        ~engine(){
            delete Renderer;
        }
    };
};



class ui_module :public module {
    private:
    ui::engine* Engine;
public:
	controller* Controller;
	logger* Logger;
	module_manager* Modules;

    window_module* window;
    input_module* input;
	ui_module(manager** Managers) :module(module_info{
		.Static = {
		.UniqueId = 1001,
		.Name = "ui_module",
		.Version = {0, 1, 0},
		.Type = module_type::STATIC,
		.Dependencies = {
			dependency{
                32,"window_module",{0,1,0},
                dependency::dependency_type::REQUIRED,
                {
                    .init_before = false,
                    .init_after = true,
                    .loop_before = true,
                    .loop_after = false,
                    .shutdown_before = true,
                    .shutdown_after = false,
                }
            },
            dependency{
                776,"input_module",{0,1,0},
                dependency::dependency_type::REQUIRED,
                {
                    .init_before = false,
                    .init_after = true,
                    .loop_before = true,
                    .loop_after = false,
                    .shutdown_before = true,
                    .shutdown_after = false,
                }
            },
            dependency{
                1000,"graphics_module",{0,1,0},
                dependency::dependency_type::REQUIRED,
                {
                    .init_before = false,
                    .init_after = true,
                    .loop_before = true,
                    .loop_after = false,
                    .shutdown_before = true,
                    .shutdown_after = false,
                }
            }
		},
		.Path = nullptr
		}
		}, Managers) {
	}
	ox::result init() override;
	ox::result update(double dt) override;
	ox::result shutdown() override;


    ui::engine* GetEngine();
};