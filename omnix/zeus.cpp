//
// Created by cakilgan on 30/11/2025.
//


#include "oxlib/math.h"
#define _USE_MATH_DEFINES
#include <fstream>
#include <string>
#include "zeus.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#define __LOG(fmt,...) printf(fmt,__VA_ARGS__);
#define PXINF(fmt,...) __LOG("[INFO:]" fmt,__VA_ARGS__)
#define PXDBG(fmt,...) __LOG("[DEBUG:]" fmt,__VA_ARGS__)
#define PXERR(fmt,...) __LOG("[ERROR:]" fmt,__VA_ARGS__)
#define PXFATAL(fmt,...) __LOG("[FATAL:]" fmt,__VA_ARGS__)

void zs::main::BindProgram(program *shader) {
    glUseProgram(shader->reference());
}
void zs::main::ClearScreen(bool color, bool depth) {
    auto result = 0x0;
    if (color) result|= GL_COLOR_BUFFER_BIT;
    if (depth) result|= GL_DEPTH_BUFFER_BIT;
    glClear(result);
}
void zs::main::ScreenClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}
void zs::main::ScreenClearDepth(float depth) {
    glClearDepth(depth);
}
void zs::main::UnbindCurrentProgram() {
    glUseProgram(0);
}

void zs::main::EnableBlend() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void zs::main::DisableBlend() {
    glDisable(GL_BLEND);
}




void zs::program::attach(shader *shader) {
    glAttachShader(this->ref,shader->ref);
}
void zs::program::detach(shader *shader) {
    glDetachShader(ref,shader->ref);
}
void zs::program::create() {
    ref = glCreateProgram();
}
void zs::program::link() {
    glLinkProgram(ref);

    GLint status;
    glGetProgramiv(ref, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length = 0;
        glGetProgramiv(ref, GL_INFO_LOG_LENGTH, &length);

        std::string message(length, '\0');
        glGetProgramInfoLog(ref, length, &length, message.data());

        PXERR("program link error: %s", message.c_str());
    }
}
int zs::program::__get_uniform_location(const char *name) {
    auto loc = glGetUniformLocation(ref,name);
    if (loc==-1) {
        PXFATAL("glGetUniformLocation not found error: %s", name);
        return ZS_NOT_BINDED_REFERENCE;
    }
    if (!uniforms.contains(name)) {
        uniforms[name] = loc;
    }
    return uniforms[name];
}



void zs::shader::create_from_source(const char* source, bool handle_errors) {
    if (!source) {
        PXFATAL("null source for shader.");
        return;
    }

    ref = glCreateShader(static_cast<GLenum>(Type));

    glShaderSource(ref, 1, &source, nullptr);
    glCompileShader(ref);

    if (handle_errors) {
        GLint status;
        glGetShaderiv(ref, GL_COMPILE_STATUS, &status);

        if (status == GL_FALSE) {
            GLint length = 0;
            glGetShaderiv(ref, GL_INFO_LOG_LENGTH, &length);

            if (length > 0) {
                char* message = (char*)malloc(length + 1);

                glGetShaderInfoLog(ref, length, &length, message);
                message[length] = '\0';

                PXERR("shader compile error: %s", message);

                free(message);
            } else {
                PXERR("shader compile error: <no log>");
            }

            glDeleteShader(ref);
            ref = 0;
        }
    }
}

void zs::shader::create_from_file(const char* path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        PXERR("could not open file %s", path);
        return;
    }

    std::string source;
    std::string line;

    while (std::getline(file, line)) {
        source += line;
        source += '\n';
    }

    file.close();
    create_from_source(source.c_str());
}
void zs::shader::destroy() {
    glDeleteShader(ref);
}


void zs::texture::create_from_file(const char *path) {
   stbi_set_flip_vertically_on_load(1);

    unsigned char* data = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
    if (!data) {
        std::cerr << "Failed to load image: " << path << std::endl;
        return;
    }

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    glGenTextures(1, &ref);
    glBindTexture(GL_TEXTURE_2D, ref);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void zs::texture::bind() {
    glBindTexture(GL_TEXTURE_2D, ref);
}
void zs::texture::bind_as(int ID = 0) {
    glActiveTexture(GL_TEXTURE0 + ID);
}






namespace zs::dbg2d{
    struct dbg_vertex {
        float x,y;
        float r,g,b,a;
    };
    using namespace zs;

    constexpr static char DBGvShader[]=R"(
        #version 460 core
        layout(location = 0) in vec2 inPos;
        layout(location = 1) in vec4 inColor;

        out vec4 outColor;

        uniform mat4 view;
        uniform mat4 proj;

        void main() {
            gl_Position = proj * view * vec4(inPos, 0.0, 1.0);
            outColor = inColor;
        }
        )";
    constexpr static char DBGfShader[]=R"(
        #version 460 core
        out vec4 Main_color;
        in vec4 outColor;
        void main(){
            Main_color = outColor;
        }
        )";

    static vao vao;
    static vbo vbo;
    static program program;
    static std::vector<dbg_vertex> vertices;
    static bool toggled = true;

    void init(){
        vao.create();
        vao.bind();
        vbo = vao.create_array_buffer();

        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(dbg_vertex) * 10000,
                     nullptr,
                     GL_DYNAMIC_DRAW);

        vertex<2> v;
        v.add_attrib({FLOAT, 2, 0});
        v.add_attrib({FLOAT, 4, 1});
        v.initialize_vertex();

        shader vS{shader::shader_type::VERTEX};
        vS.create_from_source(DBGvShader);
        shader fS{shader::shader_type::FRAGMENT};
        fS.create_from_source(DBGfShader);
        program.create();
        program.attach(&vS);
        program.attach(&fS);
        program.link();
        vS.destroy();
        fS.destroy();
    }
    void draw(const ox::mat4& view,const ox::mat4& projection){
        if(!toggled) return;

        if (vertices.empty()) return;

        main::BindProgram(&program);
        program.umat4f("view", view);
        program.umat4f("proj", projection);

        vao.bind();
        glBindBuffer(GL_ARRAY_BUFFER, vbo.reference);
        vbo.data_array(vertices.data(), vertices.size(), GL_DYNAMIC_DRAW);

        glDisable(GL_DEPTH_TEST);

        glLineWidth(2.0f);
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glDrawArrays(GL_LINES, 0, vertices.size());
    }

    static bool is_b = false;
    void begin(){
        if(!toggled) return;
        assert(!is_b);
        vertices.clear();
        is_b = true;
    } 
    void end(const ox::mat4& view,const ox::mat4& projection){
        if(!toggled) return;
        
        assert(is_b);
        is_b = false;
        draw(view, projection);

        main::UnbindCurrentProgram();
        glDisable(GL_LINE_SMOOTH);
    }

    void line(ox::vec2f a, ox::vec2f b, ox::vec4f c){
        if(!toggled) return;
        vertices.push_back({a.x(),a.y(),c.x(),c.y(),c.z(),c.w()});
        vertices.push_back({b.x(),b.y(),c.x(),c.y(),c.z(),c.w()});
    }
    void dot(ox::vec2f p, float s, ox::vec4f c){
        if(!toggled) return;
        
        ox::vec2f h = {s*0.5f, s*0.5f};

        line(p-h, {p.x()+h.x(), p.y()-h.y()}, c);
        line({p.x()+h.x(), p.y()-h.y()}, p+h, c);
        line(p+h, {p.x()-h.x(), p.y()+h.y()}, c);
        line({p.x()-h.x(), p.y()+h.y()}, p-h, c);
    }
    void cross(ox::vec2f center, float size, ox::vec4f color) {
        if(!toggled) return;
        
        float half = size * 0.5f;

        ox::vec2f p1 = { center.x() - half, center.y() + half };
        ox::vec2f p2 = { center.x() + half, center.y() - half };
        line(p1, p2, color);

        ox::vec2f p3 = { center.x() + half, center.y() + half };
        ox::vec2f p4 = { center.x() - half, center.y() - half };
        line(p3, p4, color);
    }

    void rect (ox::vec2f center, ox::vec2f size, ox::vec4f color){
        auto left_bottom = center-size/2;
        auto right_top = center+size/2;
        line(left_bottom,left_bottom+ox::vec2f{0,size.y()},color);
        line(left_bottom,left_bottom+ox::vec2f{size.x(),0},color);
        line(right_top,right_top-ox::vec2f{size.x(),0},color);
        line(right_top,right_top-ox::vec2f{0,size.y()},color);
    }

    void toggle(bool value){
        toggled = value;
    }

}



namespace zs{
    #include <fstream>

bool font::font::load(ox::cstr path,float pixel_height){
    atlas_pixels.resize(ATLAS_W * ATLAS_H);
    std::memset(atlas_pixels.data(), 0, atlas_pixels.size());

    std::ifstream file(path, std::ios::binary);
    file.seekg(0, std::ios::end);
    font_file.resize(file.tellg());
    file.seekg(0);
    file.read((char*)font_file.data(), font_file.size());

    if (!stbtt_InitFont(&info, font_file.data(), 0))
        return false;

    scale = stbtt_ScaleForPixelHeight(&info, pixel_height);

    for (char32_t c = 32; c < 256; ++c) {
        int w, h, xoff, yoff;

        unsigned char* bmp = stbtt_GetCodepointBitmap(
            &info,
            0,
            scale,
            c,
            &w,
            &h,
            &xoff,
            &yoff
        );

        int advance, lsb;
        stbtt_GetCodepointHMetrics(&info, c, &advance, &lsb);

        glyph g{};
        g.width  = (float)w;
        g.height = (float)h;

        g.bearing_x = (float)xoff;
        g.bearing_y = (float)-yoff;

        g.advance = advance * scale;
        g.bitmap  = bmp;

        glyphs[c] = g;

    }



    for (auto& [codepoint, g] : glyphs) {
        if (!g.bitmap || g.width == 0 || g.height == 0)
            continue;

        if (pen_x + g.width >= ATLAS_W) {
            pen_x = 0;
            pen_y += row_h;
            row_h = 0;
        }

        if (pen_y + g.height >= ATLAS_H) {
            OX_ASSERTMSG(false,"font atlas overflow!");
        }

        for (int y = 0; y < g.height; ++y) {
            unsigned char* dst =
                &atlas_pixels[(pen_y + y) * ATLAS_W + pen_x];

            unsigned char* src =
                g.bitmap + y * (int)g.width;

            std::memcpy(dst, src, (size_t)g.width);
        }

        g.atlas_x = pen_x;
        g.atlas_y = pen_y;
        g.atlas_w = (int)g.width;
        g.atlas_h = (int)g.height;

        g.u0 = (float)g.atlas_x / ATLAS_W;
        g.v0 = (float)g.atlas_y / ATLAS_H;
        g.u1 = (float)(g.atlas_x + g.atlas_w) / ATLAS_W;
        g.v1 = (float)(g.atlas_y + g.atlas_h) / ATLAS_H;

        pen_x += g.width;
        row_h = std::max(row_h, (int)g.height);

        stbtt_FreeBitmap(g.bitmap, nullptr);
        g.bitmap = nullptr;
    }


    GLuint& atlas_tex = atlas_texture.ref;
    glGenTextures(1, &atlas_tex);
    glBindTexture(GL_TEXTURE_2D, atlas_tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_R8,
        ATLAS_W,
        ATLAS_H,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        atlas_pixels.data()
    );
    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

const font::glyph& font::font::get(char32_t c) const{
    return this->glyphs.at(c);
}



void font::textbatch::init(size_t max_chars) {
    _max_verts = max_chars * 6;

    _vao.create();
    _vao.bind();

    _vbo.create(GL_ARRAY_BUFFER);
    
    glBufferStorage(
        GL_ARRAY_BUFFER,
       _max_verts * sizeof(text_vertex),
      nullptr,
      GL_MAP_WRITE_BIT |
      GL_MAP_PERSISTENT_BIT |
      GL_MAP_COHERENT_BIT
    );

    this->_mapped = (text_vertex*)glMapBufferRange(
        GL_ARRAY_BUFFER,
      0,
       _max_verts * sizeof(text_vertex),
      GL_MAP_WRITE_BIT |
      GL_MAP_PERSISTENT_BIT |
      GL_MAP_COHERENT_BIT
    );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, false,
        sizeof(text_vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, false,
        sizeof(text_vertex), (void*)(sizeof(float) * 2));


    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);



    constexpr ox::cstr vShader = R"(
        #version 330 core
        layout(location = 0) in vec2 inPos;
        layout(location = 1) in vec2 inUV;

        uniform mat4 u_proj;

        out vec2 vUV;

        void main() {
            vUV = inUV;
            gl_Position = u_proj * vec4(inPos, 0.0, 1.0);
        }
    )";
    constexpr ox::cstr fShader = R"(
        #version 330 core
        in vec2 vUV;
        out vec4 outColor;

        uniform sampler2D u_font;
        uniform vec4 u_color;

        void main() {
            float a = texture(u_font, vUV).r;
            outColor = vec4(u_color.rgb, u_color.a * a);
        }
    )";

    _prog.create();
    zs::shader v{zs::shader::VERTEX}, f{zs::shader::FRAGMENT};
    v.create_from_source(vShader);
    f.create_from_source(fShader);
   _prog.attach(&v);
   _prog.attach(&f);
   _prog.link();
    v.destroy();
    f.destroy();
}


void font::textbatch::push_text(
    const font& f,
    const char32_t* text,
    float x,
    float y
) {
    float pen_x = x;
    float pen_y = y;

    for (const char32_t* c = text; *c; ++c) {
        const glyph& g = f.get(*c);

        if (g.width == 0 || g.height == 0) {
            pen_x += g.advance;
            continue;
        }

        float x0 = pen_x + g.bearing_x;
        float y0 = pen_y - (g.height - g.bearing_y);
        float x1 = x0 + g.width;
        float y1 = y0 + g.height;

        auto push = [&](float px, float py, float u, float v) {
            _mapped[_vert_count++] = { px, py, u, v };
        };

        push(x0, y0, g.u0, g.v1);
        push(x1, y0, g.u1, g.v1);
        push(x1, y1, g.u1, g.v0);

        push(x0, y0, g.u0, g.v1);
        push(x1, y1, g.u1, g.v0);
        push(x0, y1, g.u0, g.v0);

        pen_x += g.advance;
    }
}

void font::textbatch::draw(font& f,const ox::mat4& proj) {
    if (_vert_count == 0)
        return;

    zs::main::BindProgram(&_prog);
    _vao.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, f.texture().ref);
    _prog.u1i("u_font", 0);
    
    _prog.umat4f("u_proj", proj);
    _prog.u4f("u_color", 1,1,1,1);

    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)_vert_count);
    zs::main::UnbindCurrentProgram();

    glBindVertexArray(0);
}

void font::textbatch::begin(){
    _vert_count = 0;
}


font::textbatch::~textbatch(){
}
}








