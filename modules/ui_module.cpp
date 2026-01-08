#include "ui_module.h"
#include "zeus.h"



namespace {
    int WindowW,WindowH;
    bool single_time_init = true;
}

ox::result ui_module::init(){
    Logger = dynamic_cast<logger*>(Managers[0]);
    Controller = dynamic_cast<controller*>(Managers[1]);
    Modules = dynamic_cast<module_manager*>(Managers[3]);
    OX_ASSERT(Logger);
    OX_ASSERT(Controller);
    OX_ASSERT(Modules);
    input = dynamic_cast<input_module*>(Modules->get_modules()[776]);
    window = dynamic_cast<window_module*>(Modules->get_modules()[32]);
    OX_ASSERT(input);
    OX_ASSERT(window);

    window->PushWindowSizeChangeEvent("__UIModule", [](int w,int h){
        glViewport(0,0,w,h);
        WindowW = w;
        WindowH = h;
    });

    Engine = new ui::engine{new ui::renderer{{1920,1080},2400}};
    Engine->init_renderer();
    Engine->FontRenderer.init();
    return ox::ok;
}
ox::result ui_module::update(double dt){
    if(single_time_init){
        Engine->init();
        single_time_init = false;
    }

    zs::dbg2d::begin();
    Engine->data.mouse_clicked[0] = input->IsMouseDown(0);
    Engine->data.mouse_clicked[1] = input->IsMouseDown(1);
    Engine->data.mouse_just_clicked[0] = input->IsMouseJustDown(0);
    Engine->data.mouse_just_clicked[1] = input->IsMouseJustDown(1);
    Engine->data.mouse_scroll_force = input->MouseYScroll();
    Engine->data.mouse_pos = {input->GetMousePos().x(),input->GetMousePos().y()};
    Engine->update_drag(dt, input);
    Engine->update(dt);

    auto dirty_ui = utf8_to_u32("DirtyUIParts: "+std::to_string(Engine->Renderer->batch.dirty_sprites.size()));
    Engine->FontRenderer.push_text(Engine->Font, dirty_ui.c_str(), 50, 750);

    zs::main::BindProgram(&Engine->Renderer->batch.get_program());
    Engine->Renderer->batch.get_program().umat4f("proj",ox::mat4::build::ortho(0.0f, WindowW, 0.0f, WindowH, -1.0f, 1.0f));
    Engine->Renderer->batch.get_program().umat4f("view",ox::mat4::identity());
    Engine->Renderer->batch.update();
    Engine->Renderer->batch.draw();
    zs::main::UnbindCurrentProgram();

    
    //Engine->FontRenderer.draw(Engine->Font,ox::mat4::build::ortho(0.0f, WindowW, 0.0f, WindowH, -1.0f, 1.0f));
    zs::dbg2d::end(ox::mat4::identity(),ox::mat4::build::ortho(0.0f, WindowW, 0.0f, WindowH, -1.0f, 1.0f));
    return ox::ok;
}
ox::result ui_module::shutdown(){
	return ox::ok;
}








ui::engine* ui_module::GetEngine(){
    return Engine;
}



ui::element* ui::element::add(ui::element* child){
    child->parent = this;
    child->engine = this->engine;
    this->engine->Renderer->add_element(*child);
    child->sync();
    this->engine->Renderer->dirt_element(*child);
    child->data = data;
    childs.push_back(child);
    child->default_font = default_font;
    child->font_renderer = font_renderer;
    return child;
}

void ui::panel::update(float dt){
    if(!disable_anchor){
        if(append_anchor()){
            sync();
            dirt();
        }
    }

    if(batch_loc->dirty){
        batch_loc->dirty = false;
        engine->Renderer->dirt_element(*this);
    }

    int i = 0;
    for(auto& child:childs){
        if(layout->format(i,childs.size(),position,size,child->position,child->size)){
            child->sync();
            child->dirt();
        }
        i++;
    }
    
    //center
    zs::dbg2d::cross(position, 5.0f, {0,0,1,1});
    for(auto* child: childs){
        child->update(dt);
    }
}

void ui::root::update(float dt){
   if(batch_loc->dirty){
       batch_loc->dirty = false;
       engine->Renderer->dirt_element(*this);
   }
   for(auto* child: childs)
   child->update(dt);
}

void ui::element::update(float dt){
    if(!disable_anchor){
        if(append_anchor()){
            sync();
            dirt();
        }
    }

    if(batch_loc->dirty){
        batch_loc->dirty = false;
        engine->Renderer->dirt_element(*this);
    }
    
    //NOTE: center
    zs::dbg2d::cross(position, 5.0f, {0,0,1,1});
    for(auto* child: childs)
        child->update(dt);
}

void ui::element::set_z(int z){
    engine->Renderer->batch.set_sprite_z(*batch_loc, z);
}
