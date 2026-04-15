//
// Created by cakilgan on 4/5/26.
//

#ifndef OMNIX_LOGGER_H
#define OMNIX_LOGGER_H
#include <omnix/core/hashmap.h>
#include <omnix/core/log.h>
#include <omnix/platform/date.h>

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace engine {
namespace logger {

template <typename T> struct log_kv;

struct log_field_key {
    ox::u64 hash;
    const char *name;

    template <typename T> constexpr auto operator=(T &&value) const {
        return log_kv<std::decay_t<T>>{*this, std::forward<T>(value)};
    }
};

constexpr log_field_key message_key = {ox::hash<ox::cstr>{}("msg"), "msg"};

// ─────────────────────────────────────────
// KV PAIR
// ─────────────────────────────────────────
template <typename T> struct log_kv {
    log_field_key key;
    T value;
};

template <typename T> struct is_log_kv : std::false_type {};

template <typename T> struct is_log_kv<log_kv<T>> : std::true_type {};

struct log_field {
    log_field_key key;
    union {
        ox::i64 i;
        ox::f64 f;
        ox::cstr s;
    };
    ox::u8 type;
    ox::u8 _placeholder_[7];
};

enum field_type : ox::u8 { FT_INT, FT_FLOAT, FT_STR };

struct log_event {
    ox::time timestamp{};
    enum struct level {
        info,
        debug,
        warning,
        error,
        fatal,
        trace,
        notice,
        lifecycle,
    } level{};
    ox::u32 field_count{};

    log_field fields[8]{};
};

template <ox::u32 Capacity = 1024> struct log_queue {

    void push(const log_event &ev) {
        std::lock_guard lock(mutex_);

        buffer_[head_] = ev;
        head_ = (head_ + 1) % Capacity;

        if (count_ < Capacity) {
            ++count_;
        } else {
            tail_ = (tail_ + 1) % Capacity;
        }

        cv_.notify_one();
    }

    bool pop(log_event &out) {
        std::unique_lock lock(mutex_);

        cv_.wait(lock, [this] { return count_ > 0 || stopped_; });

        if (count_ == 0)
            return false;

        out = buffer_[tail_];
        tail_ = (tail_ + 1) % Capacity;
        --count_;
        return true;
    }

    bool try_pop(log_event &out) {
        std::lock_guard lock(mutex_);
        if (count_ == 0)
            return false;

        out = buffer_[tail_];
        tail_ = (tail_ + 1) % Capacity;
        --count_;
        return true;
    }

    void stop() {
        std::lock_guard lock(mutex_);
        stopped_ = true;
        cv_.notify_all();
    }

    ox::u32 size() const {
        std::lock_guard lock(mutex_);
        return count_;
    }

    bool empty() const {
        std::lock_guard lock(mutex_);
        return count_ == 0;
    }

  private:
    log_event buffer_[Capacity]{};
    ox::u32 head_ = 0;
    ox::u32 tail_ = 0;
    ox::u32 count_ = 0;
    bool stopped_ = false;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
};

extern log_queue<> *queue;

inline ox::cstr level_to_str(enum log_event::level lvl) {
    switch (lvl) {
    case log_event::level::info:
        return "info";
    case log_event::level::debug:
        return "debug";
    case log_event::level::warning:
        return "warning";
    case log_event::level::error:
        return "error";
    case log_event::level::fatal:
        return "fatal";
    case log_event::level::trace:
        return "trace";
    case log_event::level::notice:
        return "notice";
    case log_event::level::lifecycle:
        return "lifecycle";
    default:
        return "";
    }
    OX_UNREACHABLE();
}

inline void fill_fields(log_event &, int) {}

template <typename T, typename... Rest>
void fill_fields(log_event &ev, int idx, log_kv<T> kv, Rest... rest) {
    if (idx >= 8)
        return;

    auto &f = ev.fields[idx];
    f.key.hash = kv.key.hash;
    f.key.name = kv.key.name;

    if constexpr (std::is_integral_v<T>) {
        f.type = FT_INT;
        f.i = kv.value;
    } else if constexpr (std::is_floating_point_v<T>) {
        f.type = FT_FLOAT;
        f.f = kv.value;
    } else {
        f.type = FT_STR;
        f.s = kv.value;
    }

    fill_fields(ev, idx + 1, rest...);
}

// ─────────────────────────────────────────
// ANSI COLOR HELPERS
// ─────────────────────────────────────────
namespace ansi {
OX_CAUTO reset = "\x1b[0m";
OX_CAUTO bold = "\x1b[1m";
OX_CAUTO dim = "\x1b[2m";

// Standard Foreground
OX_CAUTO black = "\x1b[30m";
OX_CAUTO red = "\x1b[31m";
OX_CAUTO green = "\x1b[32m";
OX_CAUTO yellow = "\x1b[33m";
OX_CAUTO blue = "\x1b[34m";
OX_CAUTO magenta = "\x1b[35m";
OX_CAUTO cyan = "\x1b[36m";
OX_CAUTO white = "\x1b[37m";

// Bright Foreground
OX_CAUTO bright_black = "\x1b[90m";
OX_CAUTO bright_red = "\x1b[91m";
OX_CAUTO bright_green = "\x1b[92m";
OX_CAUTO bright_yellow = "\x1b[93m";
OX_CAUTO bright_blue = "\x1b[94m";
OX_CAUTO bright_magenta = "\x1b[95m";
OX_CAUTO bright_cyan = "\x1b[96m";
OX_CAUTO bright_white = "\x1b[97m";

// Bright Background  ← bunlar terminal'de gerçekten canlı görünür
OX_CAUTO bg_bright_red = "\x1b[101m";
OX_CAUTO bg_bright_green = "\x1b[102m";
OX_CAUTO bg_bright_yellow = "\x1b[103m";
OX_CAUTO bg_bright_blue = "\x1b[104m";
OX_CAUTO bg_bright_magenta = "\x1b[105m";
OX_CAUTO bg_bright_cyan = "\x1b[106m";
OX_CAUTO bg_bright_white = "\x1b[107m";
} // namespace ansi

struct level_style {
    ox::cstr badge_fg;  // badge yazı rengi
    ox::cstr badge_bg;  // badge arka plan
    ox::cstr label;     // sabit 11 karakter: " LABEL     "
    ox::cstr msg_color; // mesaj yazı rengi
};

inline level_style get_level_style(enum log_event::level lvl) {
    switch (lvl) {
    case log_event::level::info:
        return {ansi::black, ansi::bg_bright_green, " INFO      ",
                ansi::bright_green};
    case log_event::level::debug:
        return {ansi::black, ansi::bg_bright_cyan, " DEBUG     ",
                ansi::bright_cyan};
    case log_event::level::warning:
        return {ansi::black, ansi::bg_bright_yellow, " WARN      ",
                ansi::bright_yellow};
    case log_event::level::error:
        return {ansi::bright_red, ansi::bg_bright_white, " ERROR     ",
                ansi::bright_red};
    case log_event::level::fatal:
        return {ansi::bright_white, ansi::bg_bright_red, " FATAL     ",
                ansi::bright_red};
    case log_event::level::trace:
        return {ansi::black, ansi::bg_bright_white, " TRACE     ", ansi::white};
    case log_event::level::notice:
        return {ansi::bright_white, ansi::bg_bright_blue, " NOTICE    ",
                ansi::bright_blue};
    case log_event::level::lifecycle:
        return {ansi::black, ansi::bg_bright_magenta, " LIFECYCLE ",
                ansi::bright_magenta};
    default:
        return {ansi::white, ansi::bg_bright_white, " ???       ", ansi::reset};
    }
    OX_UNREACHABLE();
}

// placeholder

inline void process(const log_event &ev) {
    const auto style = get_level_style(ev.level);

    const auto dt = ox::date(ev.timestamp);
    printf("[%2i.%2i.%2i..%3ius] ", dt.value.hour, dt.value.minute,
           dt.detail.second, dt.detail.microsecond);

    printf("%s%s%s%s ", ansi::bold, style.badge_bg, style.badge_fg,
           style.label);
    printf("%s", ansi::reset);

    bool plain_message =
        ev.field_count > 0 && ev.fields[0].key.hash == message_key.hash;

    if (plain_message) {
        printf("%s%s%s", ansi::bold, style.msg_color, ev.fields[0].s);

        if (ev.field_count > 1) {
            printf("%s {\n", ansi::reset);
        } else {
            printf("%s\n", ansi::reset);
        }
    }

    for (ox::u32 i = plain_message ? 1u : 0u; i < ev.field_count; ++i) {
        const auto &f = ev.fields[i];

        printf("  %s%s%s%s = %s", ansi::dim, style.msg_color, f.key.name,
               ansi::reset, ansi::reset);

        switch (f.type) {
        case FT_INT:
            printf("%s%lld%s", ansi::cyan, static_cast<long long>(f.i),
                   ansi::reset);
            break;
        case FT_FLOAT:
            printf("%s%f%s", ansi::yellow, f.f, ansi::reset);
            break;
        case FT_STR:
            printf("%s\"%s\"%s", ansi::green, f.s, ansi::reset);
            break;
        }

        printf("\n");
    }

    if (plain_message && ev.field_count > 1) {
        printf("%s}%s\n", ansi::dim, ansi::reset);
    } else if (!plain_message && ev.field_count > 0) {
        printf("\n");
    }
}

template <enum log_event::level Level, typename... Args>
void write(const char *msg_or_fmt, Args &&...args) {
    log_event ev{};
    ev.timestamp = ox::now<ox::clocks::wall>();
    ev.level = Level;
    ev.fields[0].key = message_key;
    ev.fields[0].type = FT_STR;
    ev.field_count = 1;

    if constexpr (sizeof...(Args) == 0) {
        // ── plain message, zero alloc ──────────────────
        ev.fields[0].s = msg_or_fmt;

    } else if constexpr ((is_log_kv<std::decay_t<Args>>::value && ...)) {
        // ── structured KV ─────────────────────────────
        ev.fields[0].s = msg_or_fmt;
        fill_fields(ev, 1, std::forward<Args>(args)...);
        ev.field_count = 1 + sizeof...(Args);

    } else {
        // ── printf-style format ────────────────────────
        static thread_local char buf[512];
        snprintf(buf, sizeof(buf), msg_or_fmt, std::forward<Args>(args)...);
        ev.fields[0].s = buf;
    }

    queue->push(ev);
}
} // namespace logger

namespace literals {
constexpr engine::logger::log_field_key operator""_k(const char *str, size_t) {
    return {ox::hash<ox::cstr>{}(str), str};
}
} // namespace literals
} // namespace engine
#endif // OMNIX_LOGGER_H
