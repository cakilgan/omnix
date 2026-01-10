#include "checknull_Module.h"
#include "GLFW/glfw3.h"
#include "oxlib/color.h"

namespace {
    scene *Scene;
    int WindowW,WindowH;
    bool changed_win = false;
    zs::world world;
    checknull game;
    float zoom = 2.38f;
}

ox::result game_main::init(){
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

    glfwSetWindowAttrib(
        (GLFWwindow*)window->Handle(),
        GLFW_RESIZABLE,
        GLFW_FALSE
    );

    window->PushWindowSizeChangeEvent("_CheckNull", [](int w,int h){
        glViewport(0,0,w,h);
        WindowW = w;
        WindowH = h;
        changed_win = true;
    });

    window->SetTitle("CheckNull");
    window->ResizeWindow({800,800});

    Scene = new scene{{800,800},{(128)}};

    zs::texture tex;
    tex.create_from_file("../../resources/checknull/chess.png");
    zs::texture board_png;
    board_png.create_from_file("../../resources/checknull/chess_board.png");

    Scene->Batch.set_texture(0, tex);
    Scene->Batch.set_texture(1, board_png);

    Scene->Batch.init();

    zs::dbg2d::init();
    Scene->Camera.set_zoom(zoom);
    game.init(&Scene->Batch);
    return ox::ok;
}

ox::result game_main::update(double dt){

    glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //auto dirty_scene = utf8_to_u32("DirtyWorldParts: "+std::to_string(Scene->Batch.dirty_sprites.size()));

    zs::dbg2d::begin();
    zs::main::BindProgram(&Scene->Batch.get_program());
    Scene->Batch.get_program().umat4f("proj",Scene->Camera.projection());
    Scene->Batch.get_program().umat4f("view",Scene->Camera.view());
	glfwPollEvents();
    game.update_drag(dt,Scene->Camera,world,WindowW,WindowH,input);
    Scene->Batch.update();
    Scene->Batch.draw();
    zs::main::UnbindCurrentProgram();
    zs::dbg2d::end(ox::mat4::identity(),ox::mat4::build::ortho(0.0f, WindowW, 0.0f, WindowH, -1.0f, 1.0f));


    //printf("center:%f,%f\n",Map->chunks[0].origin.x(),Map->chunks[0].origin.y());

    //zs::dbg2d::begin();
    //zs::dbg2d::end(Scene->Camera.view(), Scene->Camera.projection());

    //auto inf = U"Debug";
    //auto fps_text = utf8_to_u32("FPS:"+std::to_string(1.0f/dt));
    //#if OX_PLATFORM == OX_PLATFORM_WINDOWS
    //auto platform = U"PLATFORM: WINDOWS";
    //#elif OX_PLATFORM == OX_PLATFORM_LINUX
    //auto platform = U"PLATFORM: LINUX";
    //#endif
    //auto screen_dimensions = utf8_to_u32("DIMENSIONS[w,h]: "+std::to_string(WindowW)+","+std::to_string(WindowH));

    //Scene->UIEngine->FontRenderer.push_text(Scene->UIEngine->Font, inf, 50, 900);
    //Scene->UIEngine->FontRenderer.push_text(Scene->UIEngine->Font, fps_text.c_str(), 50, 870);
    //Scene->UIEngine->FontRenderer.push_text(Scene->UIEngine->Font, platform, 50, 840);
    //Scene->UIEngine->FontRenderer.push_text(Scene->UIEngine->Font, screen_dimensions.c_str(), 50, 780);
    //Scene->UIEngine->FontRenderer.push_text(Scene->UIEngine->Font, dirty_scene.c_str(), 50, 720);
    return ox::ok;
}
ox::result game_main::shutdown(){
    delete Scene;
    return ox::ok;
}





