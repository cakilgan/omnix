//
// Created by cakilgan on 30/11/2025.
//

#ifndef ZEUS_H
#define ZEUS_H
#define ZEUS_NAMESPACE zs
#include <array>
#include <map>
#include <unordered_map>
#include <vector>
#include <glad/gl.h>
#include <oxlib/math.h>
#include <string>
#include <cmath>
#include <algorithm>


#include "../omnix/stb_truetype.h"

#define ZS_NOT_BINDED_REFERENCE GL_INVALID_VALUE
#define ZS_IGNORE_TXID -33

namespace ZEUS_NAMESPACE {
    typedef GLuint glref;

    struct shader;
    struct program;

    class main {
        public:
        static void BindProgram(program* shader);
        static void UnbindCurrentProgram();

        static void ClearScreen(bool color = true,bool depth = false);
        static void ScreenClearColor(float r, float g, float b, float a);
        static void ScreenClearDepth(float depth);

        static void EnableBlend();
        static void DisableBlend();
    };

    struct vertex_buffer_object {
        glref reference = ZS_NOT_BINDED_REFERENCE;
        unsigned int _Type1 = 0;
        void create(unsigned int Type) {
            glGenBuffers(1,&reference);
            glBindBuffer(Type,reference);
            _Type1 = Type;
        }
        template<typename T>
        void data_array(T* arr, size_t count,GLuint Mode = GL_STATIC_DRAW) {
            glBufferData(_Type1, sizeof(T) * count, arr, Mode);
        }
        template<typename T>
        void data(T _data,size_t sz,GLuint Mode = GL_STATIC_DRAW) {
            glBufferData(_Type1, sz, _data, Mode);
        }
    };

    struct vertex_array_object {
        glref reference = ZS_NOT_BINDED_REFERENCE;
        void create() {
            glGenVertexArrays(1,&reference);
            glBindVertexArray(reference);
        }
        void bind() const{
            glBindVertexArray(reference);
        }
        vertex_buffer_object create_array_buffer() {
            auto vbo = vertex_buffer_object{};
            vbo.create(GL_ARRAY_BUFFER);
            return vbo;
        }
    };


    typedef vertex_array_object vao;
    typedef vertex_buffer_object vbo;

    enum vertex_attribute_type:unsigned int {
        FLOAT=GL_FLOAT,
        INT=GL_INT,
        BOOL=GL_BOOL,
        DOUBLE=GL_DOUBLE,
        INVALID=GL_INVALID_VALUE
    };
    inline size_t get_size(vertex_attribute_type _get_Type) {
        switch (_get_Type) {
            case FLOAT: return sizeof(float);
            case INT: return sizeof(int);
            case BOOL: return sizeof(bool);
            case DOUBLE: return sizeof(double);
            default: 0;
        }
        return 0;
    }
    struct vertex_attribute {
        vertex_attribute_type type{INVALID};
        unsigned int size{0};
        unsigned int location{0};
        bool normalized{false};
    };

    template<size_t AttrSize>
    struct vertex {
    private:
        unsigned int __loc = 0;
        vertex_attribute attributes[AttrSize]{};
    public:
        void add_attrib(vertex_attribute attr) {
            attributes[__loc++] = attr;
        }

        void initialize_vertex() {
            int stride = 0;
            for (int i = 0; i < AttrSize; ++i) {
                vertex_attribute attr  = attributes[i];
                stride += get_size(attr.type) * attr.size;
            }

            int loc = 0;
            for (int i = 0; i < AttrSize; ++i) {
                vertex_attribute attr  = attributes[i];
                void* offset_ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(loc));
                if (attr.type == INT) {
                    glVertexAttribIPointer(attr.location, attr.size, attr.type, stride, offset_ptr);
                } else {
                    glVertexAttribPointer(attr.location, attr.size, attr.type, attr.normalized ? GL_TRUE : GL_FALSE, stride, offset_ptr);
                }
                glEnableVertexAttribArray(attr.location);
                loc += attr.size * get_size(attr.type);
            }
        }
    };

    struct program {
    private:
        glref ref = ZS_NOT_BINDED_REFERENCE;
        std::vector<shader> attached_shaders;

        int __get_uniform_location(const char* name);

        std::unordered_map<const char*,int> uniforms;
    public:
        [[nodiscard]] auto reference() const -> glref {
            return ref;
        }
        [[nodiscard]] auto shaders() -> const std::vector<shader>& {
            return attached_shaders;
        }

        void create();
        void link();

        void attach(shader* shader);
        void detach(shader* shader);

        void u1f(const char* name, float x){
            glUniform1f(__get_uniform_location(name),x);
        }
        void u2f(const char* name, float x,float y){
            glUniform2f(__get_uniform_location(name),x,y);
        }
        void u3f(const char* name, float x,float y,float z){
            glUniform3f(__get_uniform_location(name),x,y,z);
        }
        void u4f(const char* name, float x,float y,float z,float w){
            glUniform4f(__get_uniform_location(name),x,y,z,w);
        }

        void u1i(const char* name, int x){
            glUniform1i(__get_uniform_location(name),x);
        }
        void u2i(const char* name, int x,int y){
            glUniform2i(__get_uniform_location(name),x,y);
        }
        void u3i(const char* name, int x,int y,int z){
            glUniform3i(__get_uniform_location(name),x,y,z);
        }
        void u4i(const char* name, int x,int y,int z,int w){
            glUniform4i(__get_uniform_location(name),x,y,z,w);
        }

        void u1d(const char* name, double x){
            glUniform1d(__get_uniform_location(name),x);
        }
        void u2d(const char* name, double x,double y){
            glUniform2d(__get_uniform_location(name),x,y);
        }
        void u3d(const char* name, double x,double y,double z){
            glUniform3d(__get_uniform_location(name),x,y,z);
        }
        void u4d(const char* name, double x,double y,double z,double w){
            glUniform4d(__get_uniform_location(name),x,y,z,w);
        }
        void umat4f(const char* loc,const ox::matrix<float,4,4>& mat4,bool transpose = true) {
            glUniformMatrix4fv(__get_uniform_location(loc),1,transpose?GL_TRUE:GL_FALSE,&mat4.data[0][0]);
        }
    };

    struct shader {
        enum shader_type:unsigned int {
            VERTEX = GL_VERTEX_SHADER,
            FRAGMENT = GL_FRAGMENT_SHADER,
            GEOMETRY = GL_GEOMETRY_SHADER,
            COMPUTE = GL_COMPUTE_SHADER,
            INVALID = GL_INVALID_ENUM,
        }Type = INVALID;

        glref ref = ZS_NOT_BINDED_REFERENCE;
        void create_from_file(const char* path);
        void create_from_source(const char* source,bool handle_errors = true);

        void destroy();
    };

    struct texture {
        int width;
        int height;
        int channels;
        glref ref = ZS_NOT_BINDED_REFERENCE;
    public:
        void create_from_file(const char* path);
        void bind();
        void bind_as(int ID);

        struct batching {
            struct UV {
                float u,v;
            };
            static std::vector<std::array<UV, 4>> generate_sprite_sheet_uvs(
                 int texW, int texH,
                 int frameW, int frameH
             ){
                int cols = texW / frameW;
                int rows = texH / frameH;

                std::vector<std::array<UV,4>> all;
                all.resize(cols * rows);

                for (int row = 0; row < rows; row++)
                {
                    for (int col = 0; col < cols; col++)
                    {
                        int px0 = col * frameW;
                        int py0 = row * frameH;
                        int px1 = px0 + frameW;
                        int py1 = py0 + frameH;
                        
                        //? AI
                        float u0 = float(px0) / texW;
                        float v0 = 1.0f - float(py1) / texH;  // Flip edildi
                        float u1 = float(px1) / texW;
                        float v1 = 1.0f - float(py0) / texH;  // Flip edildi
                        std::array<UV,4> uv = {
                            UV{u0, v0}, // sol-alt
                            UV{u1, v0}, // sağ-alt
                            UV{u1, v1}, // sağ-üst
                            UV{u0, v1}  // sol-üst
                        };
                        //? AI

                        all[row * cols + col] = uv;
                    }
                }

                return all;
            }
            static std::vector<std::array<UV, 4>> mirror(
                const std::vector<std::array<UV, 4>>& _uvs,
                bool horizontal,
                bool vertical
            ) {
                std::vector<std::array<UV, 4>> out = _uvs;

                for (auto& quad : out)
                {
                    if (horizontal) {
                        std::swap(quad[0], quad[1]);
                        std::swap(quad[3], quad[2]);
                    }

                    if (vertical) {
                        std::swap(quad[0], quad[3]);
                        std::swap(quad[1], quad[2]);
                    }
                }

                return out;
            }

        };
    };
    struct anim_state {
        texture tx;
        std::array<texture::batching::UV,4> txCoords;
    };
    struct anim {
        struct __ANIM_PAIR {
            ox::u16 s;
            ox::u16 e;
        };

        std::unordered_map<std::string, __ANIM_PAIR> pairs;

        int frame_count = 0;
        int cursor = 0;
        anim_state* state;

        anim(int Frame_count) : frame_count(Frame_count) {
            state = new anim_state[Frame_count];
        }

        ~anim() {
            delete[] state;
        }

        void init_state(int loc, anim_state _state) {
            state[loc] = _state;
        }

        void lock(ox::cstr key, ox::u16 s, ox::u16 e) {
            pairs[key] = { s, e };
        }

        anim_state* read() {
            if (cursor >= frame_count)
                cursor = 0;

            return &state[cursor++];
        }

        anim_state* read(ox::cstr key) {
            auto it = pairs.find(key);
            if (it == pairs.end()) return nullptr;

            auto pair = it->second;

            if (cursor < pair.s || cursor > pair.e)
                cursor = pair.s;

            anim_state* out = &state[cursor++];
            if (cursor > pair.e)
                cursor = pair.s;

            return out;
        }
    };


   struct cam {
   protected:
       ox::vec3f _pos;
       ox::vec3f _target;
       ox::vec3f _up;

   public:
       virtual ~cam() = default;

       cam(const ox::vec3f& pos = {0, 0, -3},
           const ox::vec3f& target = {0, 0, 0})
           : _pos(pos), _target(target) {}

       [[nodiscard]] virtual ox::mat4 view(){
           return ox::mat4::build::lookat(_pos, _pos+_target, _up);
       }

       virtual void move(const ox::vec3f& offset) {
           _pos += offset;
       }

       void look(const ox::vec3f& offset) { _target += offset; }

       void set(const ox::vec3f& pos, const ox::vec3f& target) {
           _pos = pos;
           _target = target;
       }

       ox::vec3f& position() { return _pos; }
       ox::vec3f& target()   { return _target; }
       ox::vec3f& up()       { return _up; }
   };


    struct default_camera:cam {
    private:
        ox::geo::angle _yaw{ox::geo::radians(45.0f)},_pitch{ox::geo::radians(0.0f)};
    public:

        ox::geo::angle& yaw() { return _yaw; }
        void yaw(ox::geo::angle angle) {
            yaw().rad.radians=angle.rad.radians;
            pitch().sync();
        }

        ox::geo::angle& pitch() { return _pitch; }
        void pitch(ox::geo::angle angle) {
            pitch().rad.radians=angle.rad.radians;
            yaw().sync();
        }

        ox::mat4 view() override{
            return cam::view();
        }

        virtual void update() {
            ox::vec3f front =
                {
                static_cast<float>(std::cos(_yaw.rad.radians) * cos(_pitch.rad.radians)),
                static_cast<float>(std::sin(_pitch.rad.radians)),
                static_cast<float>(std::sin(_yaw.rad.radians) * cos(_pitch.rad.radians))
                };

            target() = ox::vec3f::normalize(front);
        }
    };
    struct ortho_camera : cam {
    private:
        float base_width,base_height;
        float _left, _right, _bottom, _top;
        float _near = -1000.0f;
        float _far  =  1000.0f;
        float zoom = 2.0f;
    public:
        ortho_camera(float width, float height) {
            base_width = width;
            base_height = height;

            float hw = width  * 0.5f;
            float hh = height * 0.5f;

            _left = -hw;
            _right = hw;
            _bottom = -hh;
            _top = hh;

            _target = { 0, 0, 1 };
            _up    = { 0, 1, 0 };

            recalc();
        }

        void recalc() {
            float hw = (base_width  * 0.5f) / zoom;
            float hh = (base_height * 0.5f) / zoom;

            _left   = -hw;
            _right  =  hw;
            _bottom = -hh;
            _top    =  hh;
        }

        ox::mat4 projection() const {
            return ox::mat4::build::ortho(
                _left/zoom, _right/zoom,
                _bottom/zoom, _top/zoom,
                _near, _far
            );
        }

        ox::mat4 view() override {
            return ox::mat4::build::translate({-_pos.x(), -_pos.y(), 0.0f});
        }

        void set_zoom(float z) {
            zoom = std::clamp(z, 0.5f, 50.0f);
            recalc();
        }
        float get_zoom(){
            return zoom;
        }
        void resize(float w, float h) {
            base_width = w;
            base_height = h;
            recalc();
        }
    };

    struct world {
    ox::vec3f origin;

    ox::vec3f get_pos(const ox::vec3f& _relative) {
        return origin + _relative;
    }

   ox::vec2f world_to_screen(
        const ox::vec3f& world_pos,
        ortho_camera& cam,
        float w,
        float h
    ) {
        ox::mat4 vp = cam.projection() * cam.view();
    
        ox::vec4f clip = ox::mat4::mul_vector(ox::vec4f{
            world_pos.x(),
            world_pos.y(),
            0.0f,
            1.0f
        },vp);
    
        ox::vec3f ndc = {
            clip.x() / clip.w(),
            clip.y() / clip.w(),
            clip.z() / clip.w()
        };
    
        return {
            (ndc.x() * 0.5f + 0.5f) * w,
            (1.0f - (ndc.y() * 0.5f + 0.5f)) * h
        };
    }


    ox::vec2f screen_to_world(
        const ox::vec2f& screen,
        ortho_camera& cam,
        float w,
        float h
    ) {
        float x =  (screen.x() / w) * 2.0f - 1.0f;
        float y = -(screen.y() / h) * 2.0f + 1.0f;

        ox::vec4f ndc{ x, y, 0.0f, 1.0f };

        ox::mat4 inv_vp =
            ox::mat4::build::inverse(cam.projection()*cam.view());

        ox::vec4f world = ox::mat4::mul_vector(ndc, inv_vp);

        return {
            world.x() / world.w(),
            world.y() / world.w()
        };
    }

    ox::vec2f mouse_to_world(
        int mouse_x, int mouse_y,
        ortho_camera& cam,
        float viewport_width,
        float viewport_height
    ) {
        return screen_to_world({(float)mouse_x, (float)mouse_y}, cam, viewport_width, viewport_height);
    }
};


    namespace dbg2d{
        void toggle(bool value);
        void init();
        void shutdown();

        void begin(); 
        void end(const ox::mat4& view,const ox::mat4& projection);   

        void dot  (ox::vec2f p, float size, ox::vec4f color);
        void line (ox::vec2f a, ox::vec2f b, ox::vec4f color);

        void rect (ox::vec2f center, ox::vec2f size, ox::vec4f color);
        void rect_outline(ox::vec2f center, ox::vec2f size, ox::vec4f color);

        void cross(ox::vec2f center, float size, ox::vec4f color);
    }

    constexpr static char vShader[]=R"(
        #version 460 core
        layout(location = 0) in vec2 inPos;
        layout(location = 1) in vec4 inColor;
        layout(location = 2) in vec2 inTexCoords;
        layout(location = 3) in int inTxId;
        layout(location = 4) in vec2 inSpritePos;
        layout(location = 5) in vec2 inSpriteScale;
        layout(location = 6) in float inRotation;

        out vec4 outColor;
        out vec2 outTexCoords;
        flat out int outTxId;

        uniform mat4 view;
        uniform mat4 proj;

        void main() {
            vec2 local = inPos - vec2(0.5);
          
            if (inRotation != 0.0) {
                float c = cos(inRotation);
                float s = sin(inRotation);
                mat2 rot = mat2(c, -s,
                                s,  c);
                local = rot * local;
            }
          
            local *= inSpriteScale;
          
            vec2 pos = inSpritePos + local;

            gl_Position = proj * view * vec4(pos, 0.0, 1.0);

            outColor = inColor;
            outTexCoords = inTexCoords;
            outTxId = inTxId;
        }
        )";
    constexpr static char fShader[]=R"(
        #version 460 core
        out vec4 Main_color;

        in vec4 outColor;
        in vec2 outTexCoords;
        flat in int outTxId;

        uniform sampler2D outTexture[16];

        #define if_txid(id) if(outTxId==id){Main_color = texture(outTexture[id], outTexCoords) * outColor;}

        void main(){
            if(outTxId==-33){
                Main_color = outColor;
                return;
            }
            else if(outTxId < 0 || outTxId >= 16) {
                Main_color = vec4(1.0, 0.0, 1.0, 1.0); // Magenta : err
                return;
            }
            if_txid(0) if_txid(1) if_txid(2) if_txid(3) if_txid(4) if_txid(5)
            if_txid(6) if_txid(7) if_txid(8) if_txid(9) if_txid(10) if_txid(11)
            if_txid(12) if_txid(13) if_txid(14) if_txid(15)
        }
        )";

    struct spritevertexb {
        float x,y;
        float r,g,b,a;
        float u,v;
        int txID;
        float sprite_x, sprite_y;
        float scale_x, scale_y;
        float rotation;
    };
    using spritevertex = std::array<spritevertexb, 6>;

    struct sprite {
        ox::vec2f position{};
        ox::vec2f scale{};
        ox::geo::angle rotation = ox::geo::radians(0);
        ox::vec4f color{1,1,1,1};
        std::array<texture::batching::UV,4> QuadTXCoords
        {
            texture::batching::UV{0,0},
            texture::batching::UV{1,0},
            texture::batching::UV{1,1},
            texture::batching::UV{0,1}
        };
        int textureID = 0;


        bool dirty = false;
        int Ut_batch_index{};
        int z_index = 0;
        size_t draw_index = 0;
    };

    inline void fill_vertex_from_sprite(spritevertex& q, const sprite& s) {
        ox::vec4f c = s.color;
        int tid = s.textureID;

        const auto& bl = s.QuadTXCoords[0];
        const auto& br = s.QuadTXCoords[1];
        const auto& tr = s.QuadTXCoords[2];
        const auto& tl = s.QuadTXCoords[3];

        float px = s.position.x();
        float py = s.position.y();
        float sx = s.scale.x();
        float sy = s.scale.y();
        float rot = s.rotation.rad.radians;

        q[0] = {0.0f, 0.0f, c.x(), c.y(), c.z(), c.w(), bl.u, bl.v, tid, px, py, sx, sy, rot};
        q[1] = {1.0f, 0.0f, c.x(), c.y(), c.z(), c.w(), br.u, br.v, tid, px, py, sx, sy, rot};
        q[2] = {1.0f, 1.0f, c.x(), c.y(), c.z(), c.w(), tr.u, tr.v, tid, px, py, sx, sy, rot};

        q[3] = {0.0f, 0.0f, c.x(), c.y(), c.z(), c.w(), bl.u, bl.v, tid, px, py, sx, sy, rot};
        q[4] = {1.0f, 1.0f, c.x(), c.y(), c.z(), c.w(), tr.u, tr.v, tid, px, py, sx, sy, rot};
        q[5] = {0.0f, 1.0f, c.x(), c.y(), c.z(), c.w(), tl.u, tl.v, tid, px, py, sx, sy, rot};
    }


    struct spritebatch {
        vao vao;
        vbo buf;
        sprite* sprites;
        size_t num_sprites;
        size_t cursor = 0;
        program program;

        std::vector<size_t> draw_order;
        bool z_dirty = true;

        std::vector<bool> in_use_slots{};
        texture textures[16];

        spritevertex* data;
        void* gl_Data;

        GLsync fence = nullptr;


        spritebatch(size_t SpriteSize) : num_sprites(SpriteSize) {
            sprites = new sprite[SpriteSize];
            in_use_slots.resize(16);
            data = new spritevertex[SpriteSize];
            draw_order.reserve(num_sprites);
        }

        void sort_by_z() {
            if (!z_dirty) return;

            std::sort(draw_order.begin(), draw_order.begin() + cursor,
                [&](size_t a, size_t b) {
                    return sprites[a].z_index < sprites[b].z_index;
                });
            
            z_dirty = false;
            
            for (size_t i = 0; i < cursor; i++) {
                size_t sprite_idx = draw_order[i];
                sprites[sprite_idx].draw_index = i;
            }

            dirty_sprites.clear();
            for (size_t i = 0; i < cursor; i++)
                dirty_sprites.push_back(draw_order[i]);

                
        }
        void set_texture(int loc,texture _ref) {
            textures[loc] = _ref;
            in_use_slots[loc] = true;
        }

        zs::program& get_program() {
            return program;
        }

        void init() {
            vao.create();
            vao.bind();

            buf = vao.create_array_buffer();


            glBufferStorage(GL_ARRAY_BUFFER,num_sprites*6*sizeof(spritevertexb),
                nullptr,GL_MAP_WRITE_BIT|GL_MAP_PERSISTENT_BIT|GL_MAP_COHERENT_BIT);

            gl_Data = glMapBufferRange(GL_ARRAY_BUFFER, 0, num_sprites*6*sizeof(spritevertexb),
                           GL_MAP_WRITE_BIT |
                           GL_MAP_PERSISTENT_BIT |
                           GL_MAP_COHERENT_BIT);

            vertex<7> lv;
            lv.add_attrib({FLOAT, 2, 0});
            lv.add_attrib({FLOAT, 4, 1});
            lv.add_attrib({FLOAT, 2, 2});
            lv.add_attrib({INT,   1, 3});
            lv.add_attrib({FLOAT, 2, 4});
            lv.add_attrib({FLOAT, 2, 5});
            lv.add_attrib({FLOAT, 1, 6});
            lv.initialize_vertex();

            program.create();
            shader v{shader::VERTEX}, f{shader::FRAGMENT};
            v.create_from_source(vShader);
            f.create_from_source(fShader);
            program.attach(&v);
            program.attach(&f);
            program.link();
            v.destroy();
            f.destroy();

            main::BindProgram(&program);
            for (int i = 0; i < 16; i++) {
                glUniform1i(glGetUniformLocation(program.reference(), ("outTexture[" + std::to_string(i) + "]").c_str()), i);
            }
            main::UnbindCurrentProgram();

        }


        std::vector<int> dirty_sprites{};
        void mark_dirty(sprite& sprite) {
            if (!sprite.dirty) {
                dirty_sprites.push_back(sprite.Ut_batch_index);
                sprite.dirty = true;
            }
        }

        void update() {
            if(cursor == 0) return;
            
            if (fence) {
                glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
                glDeleteSync(fence);
                fence = nullptr;
            }
        
            if (z_dirty) {
                sort_by_z();
                        
                for (size_t draw_idx = 0; draw_idx < cursor; draw_idx++) {
                    size_t sprite_idx = draw_order[draw_idx];
                
                    fill_vertex_from_sprite(data[draw_idx], sprites[sprite_idx]);
                
                    memcpy((char*)gl_Data + draw_idx * 6 * sizeof(spritevertexb),
                           &data[draw_idx],
                           6 * sizeof(spritevertexb));
                
                    sprites[sprite_idx].dirty = false;
                }
            
                dirty_sprites.clear();
                z_dirty = false;
                return;
            }

            for (size_t sprite_idx : dirty_sprites) {
                size_t draw_idx = sprites[sprite_idx].draw_index;

                fill_vertex_from_sprite(data[draw_idx], sprites[sprite_idx]);

                memcpy((char*)gl_Data + draw_idx * 6 * sizeof(spritevertexb),
                       &data[draw_idx],
                       6 * sizeof(spritevertexb));

                sprites[sprite_idx].dirty = false;
            }

            dirty_sprites.clear();

        }

        void draw() {
            if (cursor == 0) return;

            for (int i = 0; i < 16; i++) {
                glActiveTexture(GL_TEXTURE0 + i);
                if (in_use_slots[i]) {
                    glBindTexture(GL_TEXTURE_2D, textures[i].ref);
                }
            }

            vao.bind();
            glDrawArrays(GL_TRIANGLES, 0, cursor * 6);

            fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        }

        void set_sprite_z(sprite& s, int newZ) {
            if (s.z_index != newZ) {
                s.z_index = newZ;
                z_dirty = true;
            }
        }
        sprite* add_sprite() {
            if (cursor >= num_sprites) return nullptr;

            sprites[cursor] = sprite{};
            sprites[cursor].Ut_batch_index = cursor;

            draw_order.push_back(cursor);
            z_dirty = true;

            return &sprites[cursor++];
        }

        ~spritebatch() {
            if (fence != nullptr) {
                glDeleteSync(fence);
            }
            delete[] sprites;
            delete[] data;
        }
    };

    namespace font{
        struct glyph {
            float width;
            float height;
                
            float bearing_x;
            float bearing_y;
                
            float advance;
                
            int atlas_x;
            int atlas_y;
                
            int atlas_w;
            int atlas_h;
                
            float u0, v0;
            float u1, v1;
                
            unsigned char* bitmap; 
        };
        class font {
        public:
            const int ATLAS_W = 1024;
            const int ATLAS_H = 1024;
            bool load(const char* path, float pixel_height);
            const glyph& get(char32_t c) const;
            const zs::texture& texture(){return atlas_texture;}
        private:
            int pen_x = 0;
            int pen_y = 0;
            int row_h = 0;
            stbtt_fontinfo info;
            std::unordered_map<char32_t, glyph> glyphs;
            std::vector<unsigned char> atlas_pixels;
            std::vector<unsigned char> font_file;
            float scale;
            zs::texture atlas_texture;
        };

        struct text_vertex {
            float x, y;
            float u, v;
        };
        static_assert(sizeof(text_vertex) == 16);

        class textbatch {
        public:
            textbatch() = default;
            ~textbatch();
                
            void init(size_t max_chars = 4096);
            void begin();
            void push_text(
                const font& f,
                const char32_t* text,
                float x,
                float y
            );
            void draw(font& f,const ox::mat4& proj = ox::mat4::identity());
        
        private:
            zs::vao  _vao;
            zs::vbo  _vbo;
            zs::program _prog;
        
            text_vertex* _mapped = nullptr;
            size_t _max_verts = 0;
            size_t _vert_count = 0;
        };
    }
    
}



#endif //ZEUS_H
