#pragma once
#ifndef INCLUDE_GUARD_OXLIB_MATH_
#define INCLUDE_GUARD_OXLIB_MATH_

#include <string>
#include <oxlib/types.h>
#include <algorithm>
#include <iostream>

namespace ox {
    namespace geo {
        typedef f64 angle_t;

        struct radian {
            angle_t radians;
            operator angle_t() const {
                return radians;
            }
        };
        struct degree {
            angle_t degrees;
            operator angle_t() const {
                return degrees;
            }
        };

        static radian degtorad(const degree& _use) {
            return { _use * (3.1415 / 180.0) };
        }
        static degree radtodeg(const radian& _use) {
            return { _use * 180.0 / 3.1415 };
        }


        struct angle {
            radian rad = { 0 };
            degree deg = { 0 };
            explicit angle(radian _withradian) :deg(radtodeg(_withradian)) { rad = _withradian; }
            explicit angle(degree _withdegree) :rad(degtorad(_withdegree)) { deg = _withdegree; }

            void sync(const bool& use_rad = true) {
                if (use_rad) {
                    deg = radtodeg(rad);
                }
                else {
                    rad = degtorad(deg);
                }
            }
        };

        inline angle degrees(double from_degree) {
            return angle{
                degree{from_degree}
            };
        }
        inline angle radians(double from_radian) {
            return angle{
                radian{from_radian}
            };
        }
    }

    enum class limit_mode {
        stop,
        ring
    };
    template<typename underlying, underlying Min, underlying Max, limit_mode mode = limit_mode::ring>
    struct limit {
        static_assert(Min <= Max, "Min must be <= Max");

        underlying type{ Min };

        limit() = default;
        limit(underlying value) { type = clamp(value); }

        operator underlying() const {
            return type;
        }

        static underlying clamp(underlying value) {
            if (mode == limit_mode::ring) {
                if (value < Min) return Max;
                if (value > Max) return Min;
            }
            else {
                if (value < Min) return Min;
                if (value > Max) return Max;
            }
            return value;
        }

        limit& operator=(underlying value) {
            type = clamp(value);
            return *this;
        }

        limit& operator+=(underlying rhs) { type = clamp(type + rhs); return *this; }
        limit& operator-=(underlying rhs) { type = clamp(type - rhs); return *this; }
        limit& operator*=(underlying rhs) { type = clamp(type * rhs); return *this; }
        limit& operator/=(underlying rhs) { type = clamp(type / rhs); return *this; }

        limit& operator++(underlying rhs_IGNORE) { *this += 1; return *this;}
        limit& operator--(underlying rhs_IGNORE) { *this -= 1; return *this;}

        friend limit operator+(limit lhs, underlying rhs) { lhs += rhs; return lhs; }
        friend limit operator-(limit lhs, underlying rhs) { lhs -= rhs; return lhs; }
        friend limit operator*(limit lhs, underlying rhs) { lhs *= rhs; return lhs; }
        friend limit operator/(limit lhs, underlying rhs) { lhs /= rhs; return lhs; }

        friend std::ostream& operator<<(std::ostream& os, const limit& l) {
            os << l.type;
            return os;
        }

        static bool valid(underlying _test) {
            return _test >= Min && _test <= Max;
        }
    };

    template<typename T>
    struct atomic_counter {
        std::atomic<T> value;
        explicit atomic_counter(T value) : value(value) {}

        atomic_counter& increment(T _add = 1) {
            value.store(value.load() + _add);
            return *this;
        }

        auto operator++() -> atomic_counter {
            return increment();
        }
        auto operator--() -> atomic_counter {
            return increment(-1);
        }
    };


    template<typename T, size_t N>
    struct vector {
        T data[N];

        T length{ T(0) };
        geo::angle angle{ geo::radian{0.0f} };

        T& operator[](size_t i) { return data[i]; }
        const T& operator[](size_t i) const { return data[i]; }

        vector operator+(const vector& other) const {
            vector result{};
            for (size_t i = 0; i < N; ++i) result[i] = data[i] + other[i];
            return result;
        }
        vector& operator+=(const vector& other) {
            for (size_t i = 0; i < N; ++i) data[i] += other[i];
            return *this;
        }

        vector operator-(const vector& other) const {
            vector result{};
            for (size_t i = 0; i < N; ++i) result[i] = data[i] - other[i];
            return result;
        }
        vector& operator-=(const vector& other) {
            for (size_t i = 0; i < N; ++i) data[i] -= other[i];
            return *this;
        }

        vector operator*(const vector& other) const {
            vector result{};
            for (size_t i = 0; i < N; ++i) result[i] = data[i] * other[i];
            return result;
        }
        vector& operator*=(const vector& other) {
            for (size_t i = 0; i < N; ++i) data[i] *= other[i];
            return *this;
        }

        vector operator/(const vector& other) const {
            vector result{};
            for (size_t i = 0; i < N; ++i) result[i] = data[i] / other[i];
            return result;
        }
        vector& operator/=(const vector& other) {
            for (size_t i = 0; i < N; ++i) data[i] /= other[i];
            return *this;
        }

        vector operator*(T scalar) {
            vector result{};
            for (int i = 0; i < N; ++i) {
                result[i] = data[i] * scalar;
            }
            return result;
        }
        vector& operator*=(T scalar) {
            for (int i = 0; i < N; ++i) {
                data[i] *= scalar;
            }
            return *this;
        }

        vector operator/(T scalar) {
            vector result{};
            for (int i = 0; i < N; ++i) {
                result[i] = data[i] / scalar;
            }
            return result;
        }
        vector& operator/=(T scalar) {
            for (int i = 0; i < N; ++i) {
                data[i] /= scalar;
            }
            return *this;
        }

        static vector unit(const vector& _use) {
            auto mag = magnitude(_use);
            vector result{};
            if (mag == 0) { return result; }
            for (int i = 0; i < N; ++i) {
                result[i] = _use.data[i] / mag;
            }
            return result;
        }
        static vector normalize(const vector& _use) {
            return unit(_use);
        }

        static vector fill(const T& _set) {
            vector result{};
            for (size_t i = 0; i < N; ++i) result[i] = _set;
            return result;
        }

        static vector zero() {
            return fill(0);
        }

        static T magnitude(const vector& _vec) {
            T result = 0;
            for (size_t i = 0; i < N; ++i) result += _vec[i] * _vec[i];
            return std::sqrt(result);
        }

        static T dot(const vector& _veca, const vector& _vecb) {
            T result = T();
            for (size_t i = 0; i < N; ++i) result += _veca[i] * _vecb[i];
            return result;
        }

        static geo::angle dot_angle(T _dot, const vector& _veca, const vector& _vecb) {
            T cosine = _dot / (magnitude(_veca) * magnitude(_vecb));
            cosine = std::clamp(cosine, T(-1), T(1));
            auto _ref = geo::angle(acos(cosine));
            _ref.sync(true);
            return _ref;
        }




        static vector<float, 2> rotate2D(geo::angle _angle, const vector<float, 2>& vec) {
            const float c = cos(_angle.rad);
            const float s = sin(_angle.rad);
            return {
                vec[0] * c - vec[1] * s,
                vec[0] * s + vec[1] * c
            };
        }

        static vector absolute(vector& vec){
            for (size_t i = 0; i < N; ++i) vec.data[i] = abs(vec.data[i]);
            return vec;
        }

        static bool equals(vector<T,N> a,vector<T,N> b){
            for(int i = 0;i<N;i++){
                if(a[i]!=b[i]){
                    return false;
                }
            }
            return true;
        }
    };

#define V2SW(A,B) \
vector2<T> A##B() const { return vector2<T>{A(), B()}; }

    template<typename T>
    struct vector2 :vector<T, 2> {
        vector2(const T& _x, const T& _y) {
            x() = _x; y() = _y;
        }
        vector2(const T& length, geo::angle angle) {
            this->length = length;
            this->angle = angle;
        }
        vector2(const vector<T, 2>& _from = vector<T, 2>::zero()) :vector2(_from[0], _from[1]) {};

        T x() const { return this->data[0]; }
        T y() const { return this->data[1]; }

        T& x() { return this->data[0]; }
        T& y() { return this->data[1]; }

        V2SW(x, x);
        V2SW(x, y);
        V2SW(y, y);
        V2SW(y, x);

        
        static bool pointInRect(vector2 p, vector2 c, vector2 s)
        {
            auto diff = (p - c);
            vector2 d = vector2::absolute(diff);
            vector2 h = s * 0.5f;
            return (d[0] <= h[0]) & (d[1] <= h[1]);
        }
        static vector2 revert_y(vector2 v,float h){
            return {v.x(),h-v.y()};
        }
    };




#define V3SW(A,B,C) \
vector3<T> A##B##C() const { return vector3<T>{A(), B(), C()}; }

    template<typename T>
    struct vector3 :vector<T, 3> {
        vector3(const T& _x, const T& _y, const T& _z) { x() = _x; y() = _y; z() = _z; }
        explicit vector3(const vector2<T>& _2d) { x() = _2d.x(); y() = _2d.y(); }
        vector3(const vector<T, 3>& _from = vector<T, 3>::zero()) :vector3(_from[0], _from[1], _from[2]) {};


        T x()const { return this->data[0]; }
        T y()const { return this->data[1]; }
        T z()const { return this->data[2]; }


        T& x() { return this->data[0]; }
        T& y() { return this->data[1]; }
        T& z() { return this->data[2]; }

        static vector3 cross(const vector3& a, const vector3& b)
        {
            return {
                a.y() * b.z() - a.z() * b.y(),
                a.z() * b.x() - a.x() * b.z(),
                a.x() * b.y() - a.y() * b.x()
            };
        }

        V2SW(x, x) V2SW(x, y) V2SW(x, z)
        V2SW(y, y) V2SW(y, x) V2SW(y, z)
        V2SW(z, z) V2SW(z, x) V2SW(z, y)

        V3SW(x, x, x) V3SW(x, x, y) V3SW(x, y, x) V3SW(x, y, y) V3SW(x, x, z) V3SW(x, z, x) V3SW(x, z, z) V3SW(x, y, z) V3SW(x, z, y)
        V3SW(y, y, y) V3SW(y, y, x) V3SW(y, x, y) V3SW(y, x, x) V3SW(y, y, z) V3SW(y, z, y) V3SW(y, z, z) V3SW(y, x, z) V3SW(y, z, x)
        V3SW(z, z, z) V3SW(z, z, y) V3SW(z, y, z) V3SW(z, y, y) V3SW(z, z, x) V3SW(z, x, z) V3SW(z, x, x) V3SW(z, y, x) V3SW(z, x, y)
    };

    template<typename T>
    struct vector4 :vector<T, 4> {
        vector4(const T& _x, const T& _y, const T& _z, const T& _w) {
            x() = _x; y() = _y; z() = _z;
            w() = _w;
        }
        vector4(const vector<T, 4>& _from = vector<T, 4>::zero()) :vector4(_from[0], _from[1], _from[2], _from[3]) {};

        T& x()const { return this->data[0]; }
        T& y()const { return this->data[1]; }
        T& z()const { return this->data[2]; }
        T& w()const { return this->data[3]; }

        T& x() { return this->data[0]; }
        T& y() { return this->data[1]; }
        T& z() { return this->data[2]; }
        T& w() { return this->data[3]; }

        V2SW(x, x) V2SW(x, y) V2SW(x, z)
        V2SW(y, y) V2SW(y, x) V2SW(y, z)
        V2SW(z, z) V2SW(z, x) V2SW(z, y)

        V3SW(x, x, x) V3SW(x, x, y) V3SW(x, y, x) V3SW(x, y, y) V3SW(x, x, z) V3SW(x, z, x) V3SW(x, z, z) V3SW(x, y, z) V3SW(x, z, y)
        V3SW(y, y, y) V3SW(y, y, x) V3SW(y, x, y) V3SW(y, x, x) V3SW(y, y, z) V3SW(y, z, y) V3SW(y, z, z) V3SW(y, x, z) V3SW(y, z, x)
        V3SW(z, z, z) V3SW(z, z, y) V3SW(z, y, z) V3SW(z, y, y) V3SW(z, z, x) V3SW(z, x, z) V3SW(z, x, x) V3SW(z, y, x) V3SW(z, x, y)
    };

    typedef vector2<f32> vec2f;
    typedef vector2<f64> vec2d;
    typedef vector2<i8>  vec2b;
    typedef vector2<i16> vec2s;
    typedef vector2<i32> vec2i;
    typedef vector2<i64> vec2l;

    typedef vector2<u8>  vec2ub;
    typedef vector2<u16> vec2us;
    typedef vector2<u32> vec2ui;
    typedef vector2<u64> vec2ul;

    typedef vec2i point2d;

    typedef vector3<f32> vec3f;
    typedef vector3<f64> vec3d;
    typedef vector3<i8>  vec3b;
    typedef vector3<i16> vec3s;
    typedef vector3<i32> vec3i;
    typedef vector3<i64> vec3l;

    typedef vector3<u8>  vec3ub;
    typedef vector3<u16> vec3us;
    typedef vector3<u32> vec3ui;
    typedef vector3<u64> vec3ul;

    typedef vec3i point3d;

    typedef vector4<f32> vec4f;
    typedef vector4<f64> vec4d;
    typedef vector4<i8>  vec4b;
    typedef vector4<i16> vec4s;
    typedef vector4<i32> vec4i;
    typedef vector4<i64> vec4l;

    typedef vector4<u8>  vec4ub;
    typedef vector4<u16> vec4us;
    typedef vector4<u32> vec4ui;
    typedef vector4<u64> vec4ul;

#undef V2SW
#undef V3SW


    template<typename T, size_t d1, size_t d2>
    struct matrix {
        T data[d1][d2];

        T& operator[](size_t loc) {
            return data[loc];
        }

        matrix operator+(const matrix& other) const {
            matrix res;
            for (int i = 0; i < d1; ++i) {
                for (int j = 0; j < d2; ++j) {
                    res.data[i][j] = data[i][j] + other.data[i][j];
                }
            }
            return res;
        }
        matrix operator-(const matrix& other) const {
            matrix res;
            for (int i = 0; i < d1; ++i) {
                for (int j = 0; j < d2; ++j) {
                    res.data[i][j] = data[i][j] - other.data[i][j];
                }
            }
            return res;
        }

        template<size_t Od1, size_t Od2>
        matrix<T, d1, Od2> operator*(const matrix<T, Od1, Od2>& other) const {
            static_assert(d2 == Od1, "left matrix columns must match right matrix rows");
            matrix<T, d1, Od2> res{};
            for (size_t i = 0; i < d1; ++i) {
                for (size_t j = 0; j < Od2; ++j) {
                    T sum = T(0);
                    for (size_t k = 0; k < d2; ++k) {
                        sum += data[i][k] * other.data[k][j];
                    }
                    res.data[i][j] = sum;
                }
            }
            return res;
        }


        template<size_t N>
        static vector<T, N> mul_vector(const vector<T, N>& vec, const matrix<T, N, N>& mat) {
            vector<T, N> result{};
            for (size_t i = 0; i < N; ++i) {
                T sum = 0;
                for (size_t j = 0; j < N; ++j) {
                    sum += mat.data[i][j] * vec[j];
                }
                result[i] = sum;
            }
            return result;
        }

        static vector<T, 4> rotateX(geo::angle angle, const vector<T, 4>& vec) {
            matrix<float, 4, 4> mat = matrix<float, 4, 4>::identity();
            mat.data[1][1] = cosf(angle.rad);
            mat.data[1][2] = -sinf(angle.rad);
            mat.data[2][1] = sinf(angle.rad);
            mat.data[2][2] = cosf(angle.rad);
            return mul_vector<4>(vec, mat);
        }

        static vector<T, 4> rotateY(geo::angle angle, const vector<T, 4>& vec) {
            matrix<float, 4, 4> mat = matrix<float, 4, 4>::identity();
            mat.data[0][0] = cosf(angle.rad);
            mat.data[0][2] = sinf(angle.rad);
            mat.data[2][0] = -sinf(angle.rad);
            mat.data[2][2] = cosf(angle.rad);
            return mul_vector<4>(vec, mat);
        }

        static vector<T, 4> rotateZ(geo::angle angle, const vector<T, 4>& vec) {
            matrix<float, 4, 4> mat = matrix<float, 4, 4>::identity();
            mat.data[0][0] = cosf(angle.rad);
            mat.data[0][1] = -sinf(angle.rad);
            mat.data[1][0] = sinf(angle.rad);
            mat.data[1][1] = cosf(angle.rad);
            return mul_vector<4>(vec, mat);
        }

        static matrix<float, 4, 4> RefrotateZ(geo::angle angle) {
            matrix<float, 4, 4> mat = matrix<float, 4, 4>::identity();
            mat.data[0][0] = cosf(angle.rad);
            mat.data[0][1] = -sinf(angle.rad);
            mat.data[1][0] = sinf(angle.rad);
            mat.data[1][1] = cosf(angle.rad);
            return mat;
        }

        static vector<T, 4> rotate(geo::angle angle, const vector<T, 4>& vec, const vector<T, 3>& around) {
            vector<T, 3> k = normalize(around);

            T x = k[0];
            T y = k[1];
            T z = k[2];

            T c = cosf(angle.rad);
            T s = sinf(angle.rad);
            T t = 1 - c;

            matrix<T, 4, 4> m = matrix<T, 4, 4>::identity();

            m.data[0][0] = t * x * x + c;
            m.data[0][1] = t * x * y - s * z;
            m.data[0][2] = t * x * z + s * y;

            m.data[1][0] = t * y * x + s * z;
            m.data[1][1] = t * y * y + c;
            m.data[1][2] = t * y * z - s * x;

            m.data[2][0] = t * z * x - s * y;
            m.data[2][1] = t * z * y + s * x;
            m.data[2][2] = t * z * z + c;

            return mul_vector<4>(vec, m);
        }


        struct build {
            static matrix<T, 4, 4> rotation(const geo::angle& angle, const vector<T, 3>& axis) {
                vector<T, 3> k = vector<T, 3>::normalize(axis);

                T x = k[0], y = k[1], z = k[2];
                T c = cosf(angle.rad);
                T s = sinf(angle.rad);
                T t = 1 - c;

                matrix<T, 4, 4> rot = matrix<T, 4, 4>::identity();

                rot.data[0][0] = t * x * x + c;
                rot.data[0][1] = t * x * y - s * z;
                rot.data[0][2] = t * x * z + s * y;

                rot.data[1][0] = t * y * x + s * z;
                rot.data[1][1] = t * y * y + c;
                rot.data[1][2] = t * y * z - s * x;

                rot.data[2][0] = t * z * x - s * y;
                rot.data[2][1] = t * z * y + s * x;
                rot.data[2][2] = t * z * z + c;

                return rot;
            }

            static matrix translate(const vector<T, d2 - 1>& _bury, const matrix& _mx = matrix::identity()) {
                matrix mx = _mx;
                for (size_t i = 0; i < d1; ++i) {
                    for (size_t j = 0; j < d2; ++j) {
                        if (j == d1 - 1) {
                            mx.data[i][j] = _bury[i];
                        }
                    }
                }
                mx.data[d1 - 1][d2 - 1] = 1.00f;
                return mx;
            }


            /*!!!!!!! THIS FUNCTION USES COLUMN-MAJOR MATRIX !!!!!!!*/
            static matrix<float, 4, 4> lookat(const vec3f& eye,
                const vec3f& center,
                const vec3f& up)
            {
                vec3f f = vec3f::normalize(center - eye);
                vec3f s = vec3f::normalize(vec3f::cross(f, up));
                vec3f u = vec3f::cross(s, f);

                matrix<float, 4, 4> m = matrix<float, 4, 4>::identity();

                //* ---- ROTATION (column-major) ----
                m.data[0][0] = s.x();
                m.data[1][0] = s.y();
                m.data[2][0] = s.z();
                m.data[0][1] = u.x();
                m.data[1][1] = u.y();
                m.data[2][1] = u.z();
                m.data[0][2] = -f.x();
                m.data[1][2] = -f.y();
                m.data[2][2] = -f.z();

                //* ---- TRANSLATION ----
                m.data[3][0] = -vec3f::dot(s, eye);
                m.data[3][1] = -vec3f::dot(u, eye);
                m.data[3][2] = vec3f::dot(f, eye);

                return m;
            }


            static matrix<float, 4, 4> perspective(geo::angle fov, float aspect, float _Near, float _Far) {
                float f = 1.0f / tanf(fov.rad / 2.0f);
                return {
                     {
                         f / aspect, 0, 0, 0,
                         0, f, 0, 0,
                         0, 0, (_Far + _Near) / (_Near - _Far), (2 * _Far * _Near) / (_Near - _Far),
                         0, 0, -1, 0
                     }
                };
            }

            static matrix<float, 4, 4> ortho(float left, float right,
                float bottom, float top,
                float zNear, float zFar)
            {
                matrix<float, 4, 4> m = identity();

                m.data[0][0] = 2.0f / (right - left);
                m.data[1][1] = 2.0f / (top - bottom);
                m.data[2][2] = -2.0f / (zFar - zNear);
                m.data[3][3] = 1.0f;

                m.data[0][3] = -(right + left) / (right - left);
                m.data[1][3] = -(top + bottom) / (top - bottom);
                m.data[2][3] = -(zFar + zNear) / (zFar - zNear);

                return m;
            }

            static matrix scale(const vector<T, d2 - 1>& _bury) {
                matrix res;
                for (size_t i = 0; i < d1; ++i) {
                    for (size_t j = 0; j < d2; ++j) {
                        res.data[i][j] = (i == j) ? _bury[i] : 0;
                    }
                }
                res.data[d1 - 1][d2 - 1] = 1.00f;

                return res;
            }
            static matrix inverse(const matrix& a) {
                matrix inv{};
                const float* m = &a.data[0][0];
                float* o = &inv.data[0][0];

                o[0] =   m[5]*m[10]*m[15] - m[5]*m[11]*m[14] - m[9]*m[6]*m[15]
                       + m[9]*m[7]*m[14] + m[13]*m[6]*m[11] - m[13]*m[7]*m[10];

                o[4] =  -m[4]*m[10]*m[15] + m[4]*m[11]*m[14] + m[8]*m[6]*m[15]
                       - m[8]*m[7]*m[14] - m[12]*m[6]*m[11] + m[12]*m[7]*m[10];

                o[8] =   m[4]*m[9]*m[15]  - m[4]*m[11]*m[13] - m[8]*m[5]*m[15]
                       + m[8]*m[7]*m[13] + m[12]*m[5]*m[11] - m[12]*m[7]*m[9];

                o[12] = -m[4]*m[9]*m[14]  + m[4]*m[10]*m[13] + m[8]*m[5]*m[14]
                       - m[8]*m[6]*m[13] - m[12]*m[5]*m[10] + m[12]*m[6]*m[9];

                o[1] =  -m[1]*m[10]*m[15] + m[1]*m[11]*m[14] + m[9]*m[2]*m[15]
                       - m[9]*m[3]*m[14] - m[13]*m[2]*m[11] + m[13]*m[3]*m[10];

                o[5] =   m[0]*m[10]*m[15] - m[0]*m[11]*m[14] - m[8]*m[2]*m[15]
                       + m[8]*m[3]*m[14] + m[12]*m[2]*m[11] - m[12]*m[3]*m[10];

                o[9] =  -m[0]*m[9]*m[15]  + m[0]*m[11]*m[13] + m[8]*m[1]*m[15]
                       - m[8]*m[3]*m[13] - m[12]*m[1]*m[11] + m[12]*m[3]*m[9];

                o[13] =  m[0]*m[9]*m[14]  - m[0]*m[10]*m[13] - m[8]*m[1]*m[14]
                       + m[8]*m[2]*m[13] + m[12]*m[1]*m[10] - m[12]*m[2]*m[9];

                o[2] =   m[1]*m[6]*m[15]  - m[1]*m[7]*m[14]  - m[5]*m[2]*m[15]
                       + m[5]*m[3]*m[14] + m[13]*m[2]*m[7]  - m[13]*m[3]*m[6];

                o[6] =  -m[0]*m[6]*m[15]  + m[0]*m[7]*m[14]  + m[4]*m[2]*m[15]
                       - m[4]*m[3]*m[14] - m[12]*m[2]*m[7]  + m[12]*m[3]*m[6];

                o[10] =  m[0]*m[5]*m[15]  - m[0]*m[7]*m[13]  - m[4]*m[1]*m[15]
                       + m[4]*m[3]*m[13] + m[12]*m[1]*m[7]  - m[12]*m[3]*m[5];

                o[14] = -m[0]*m[5]*m[14]  + m[0]*m[6]*m[13]  + m[4]*m[1]*m[14]
                       - m[4]*m[2]*m[13] - m[12]*m[1]*m[6]  + m[12]*m[2]*m[5];

                o[3] =  -m[1]*m[6]*m[11]  + m[1]*m[7]*m[10]  + m[5]*m[2]*m[11]
                       - m[5]*m[3]*m[10] - m[9]*m[2]*m[7]   + m[9]*m[3]*m[6];

                o[7] =   m[0]*m[6]*m[11]  - m[0]*m[7]*m[10]  - m[4]*m[2]*m[11]
                       + m[4]*m[3]*m[10] + m[8]*m[2]*m[7]   - m[8]*m[3]*m[6];

                o[11] = -m[0]*m[5]*m[11]  + m[0]*m[7]*m[9]   + m[4]*m[1]*m[11]
                       - m[4]*m[3]*m[9]  - m[8]*m[1]*m[7]   + m[8]*m[3]*m[5];

                o[15] =  m[0]*m[5]*m[10]  - m[0]*m[6]*m[9]   - m[4]*m[1]*m[10]
                       + m[4]*m[2]*m[9]  + m[8]*m[1]*m[6]   - m[8]*m[2]*m[5];

                float det =
                    m[0]*o[0] + m[1]*o[4] + m[2]*o[8] + m[3]*o[12];

                if (det == 0.0f) {
                    return matrix{};
                }
            
                float inv_det = 1.0f / det;
                for (int i = 0; i < 16; i++)
                    o[i] *= inv_det;
            
                return inv;
            }
        };



        static matrix identity() {
            matrix res;
            for (size_t i = 0; i < d1; ++i) {
                for (size_t j = 0; j < d2; ++j) {
                    res.data[i][j] = (i == j) ? 1 : 0;
                }
            }
            return res;
        }




        static T determinant(const matrix<T, 2, 2>& _use) {
            return _use[0][0] * _use[1][1] - _use[0][1] * _use[1][0];
        }

        static T determinant(const matrix<T, 3, 3>& _use) {
            return
                _use[0][0] * (_use[1][1] * _use[2][2] - _use[1][2] * _use[2][1]) -
                _use[0][1] * (_use[1][0] * _use[2][2] - _use[1][2] * _use[2][0]) +
                _use[0][2] * (_use[1][0] * _use[2][1] - _use[1][1] * _use[2][0]);
        }



        void operator!=(const matrix& other) {
            return !(other == *this);
        }
        bool operator==(const matrix& other) const {
            bool res = true;
            for (int i = 0; i < d1; ++i) {
                for (int j = 0; j < d2; ++j) {
                    res = data[i][j] == other.data[i][j];
                    if (!res) return res;
                }
            }
            return res;
        }

        std::string to_string() {
            std::string res;
            res += "mat<";
            res += std::to_string(d1);
            res += ",";
            res += std::to_string(d2) + ">";
            res += "{\n";
            for (int i = 0; i < d1; ++i) {
                res += "{";
                for (int j = 0; j < d2; ++j) {
                    res += std::to_string(data[i][j]);
                    res += ",";
                }
                res.erase(res.end() - 1);
                res += "}\n";
            }
            res.erase(res.end() - 1);
            res += "\n}";
            return std::move(res);
        }
    };

    typedef matrix<float, 4, 4> mat4;
    typedef matrix<float, 4, 3> mat4x3;
    typedef matrix<float, 4, 2> mat4x2;
    typedef matrix<float, 4, 1> mat4x1;

    typedef matrix<float, 3, 4> mat3x4;
    typedef matrix<float, 3, 3> mat3;
    typedef matrix<float, 3, 2> mat3x2;
    typedef matrix<float, 3, 1> mat3x1;

    typedef matrix<float, 2, 4> mat2x4;
    typedef matrix<float, 2, 3> mat2x3;
    typedef matrix<float, 2, 2> mat2;
    typedef matrix<float, 2, 1> mat2x1;

    typedef matrix<float, 1, 4> mat1x4;
    typedef matrix<float, 1, 3> mat1x3;
    typedef matrix<float, 1, 2> mat1x2;
    typedef matrix<float, 1, 1> mat1;

    template<typename T, size_t N>
    using matvec = matrix<T,N,1>;

    typedef matvec<float, 2> matvec2;
    typedef matvec<float, 3> matvec3;
    typedef matvec<float, 4> matvec4;


    namespace util{
        
    };

}

#endif