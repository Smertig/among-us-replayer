#include "replay.hpp"

#include <fmt/format.h>

#include <set>
#include <cassert>

struct binary_deserializer {
    std::istream& is;

    void read(std::time_t tp) {
        static_assert(sizeof(tp) == 8);
        read(reinterpret_cast<std::uint64_t&>(tp));
    }

    void read(std::string& value) {
        std::uint32_t size;
        read(size);

        value.resize(size);
        read_raw(value.data(), value.size());
    }

    template <class T>
    void read(T& value, std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>>* = nullptr) {
        read_raw(&value, sizeof(value));
    }

    template <class T, std::size_t N>
    void read(std::array<T, N>& arr) {
        for (std::size_t i = 0; i < N; i++) {
            read(arr[i]);
        }
    }

    template <class E>
    void read(E& value, std::enable_if_t<std::is_enum_v<E>>* = nullptr) {
        read(reinterpret_cast<std::underlying_type_t<E>&>(value));
    }

    template <class T>
    auto read(T& value) -> decltype(value.read_from(*this)) {
        value.read_from(*this);
    }

    void read_raw(void* result, std::size_t size) {
        is.read(reinterpret_cast<char*>(result), size);
    }

    template <class... Ts, class = std::enable_if_t<(sizeof...(Ts) > 1)>>
    void read(Ts& ...values) {
        (read(values), ...);
    }
};

void replay::parse(std::istream& is) {
    constexpr std::array<char, 4> SIGNATURE{ 'A', 'U', 'R', 'P' };

    binary_deserializer deser{ is };

    deser.read(m_signature);

    if (m_signature != SIGNATURE) {
        auto to_view = [](const auto& arr) { return std::string_view(arr.data(), arr.size()); };
        throw std::runtime_error(fmt::format("unknown signature: {}, expected {}", to_view(m_signature), to_view(SIGNATURE)));
    }

    deser.read(m_version, m_timestamp);
    if (m_version != 2 && m_version != 3) {
        throw std::runtime_error(fmt::format("unknown version: {}", m_version));
    }

    if (m_version >= 3) {
        deser.read(m_mod_version, m_compatible_game_version, m_map_id);
    }
    else {
        m_mod_version = "0.3.0";
        m_compatible_game_version = "2020.9.22";
        m_map_id = 0;
    }

    std::uint32_t player_num;
    deser.read(player_num);
    for (std::uint32_t i = 0; i < player_num; i++) {
        player_info info;
        deser.read(info);

        m_players[info.id] = info;
    }

    try {
        while (true) {
            enum class entry_type : std::uint8_t {
                frame_data = 0
            };

            entry_type type;
            std::int32_t dt;
            if (m_version >= 3) {
                deser.read(type, dt);
            }
            else {
                type = entry_type::frame_data;
                // dt will be parsed later
            }

            switch (type) {
                case entry_type::frame_data: {
                    bool need_refresh;
                    if (m_version >= 3) {
                        deser.read(need_refresh);
                    }
                    else {
                        need_refresh = true;
                        deser.read(dt);
                    }

                    std::uint8_t player_num = [this, &deser]() -> std::uint8_t {
                        if (m_version >= 3) {
                            std::uint8_t value;
                            deser.read(value);
                            return value;
                        }
                        else {
                            std::uint32_t value;
                            deser.read(value);
                            return static_cast<std::uint8_t>(value);
                        }
                    }();

                    std::set<std::uint8_t> updated_players;
                    for (std::uint32_t i = 0; i < player_num; i++) {
                        std::uint8_t id;
                        std::array<float, 2> position, velocity;
                        bool is_dead, is_disconnected;

                        deser.read(id, position, velocity, is_dead, is_disconnected);

                        if (m_version >= 3) {
                            std::uint8_t task_count;
                            deser.read(task_count);
                            for (std::uint8_t i = 0; i < task_count; i++) {
                                bool is_complete;
                                std::uint8_t type_id;
                                std::uint32_t id;
                                deser.read(is_complete, type_id, id);
                                // TODO: save parsed tasks
                            }
                        }

                        m_players.at(id).timeline.emplace_back(player_info::player_state{
                            /* dt */ dt,
                            /* position */ position,
                            /* velocity */ velocity,
                            /* is_dead */ is_dead
                        });

                        updated_players.insert(id);
                    }

                    if (!need_refresh) {
                        for (auto& [id, player] : m_players) {
                            if (updated_players.count(id) > 0) {
                                continue;
                            }

                            assert(!player.timeline.empty());
                            auto copy = player.timeline.back();
                            copy.dt = dt;
                            player.timeline.push_back(std::move(copy));
                        }
                    }
                    break;
                }
            }
        }
    }
    catch (std::istream::failure&) {
        // EOF
    }
}

int replay::get_map_id() const {
    return m_map_id;
}

const std::map<std::uint8_t, replay::player_info>& replay::get_players() const {
    return m_players;
}

bool replay::is_meeting(int time) const {
    auto almost_same = [](float a, float b) {
        return std::abs(a - b) < 0.001;
    };

    for (const auto& [_, info] : m_players) {
        const auto cur_state = info.get_interpolated(time);
        const auto state_after_5s = info.get_interpolated(time + 5000);

        if (!almost_same(cur_state.position[0], state_after_5s.position[0]) ||
            !almost_same(cur_state.position[1], state_after_5s.position[1])) {
            return false;
        }
    }

    return true;
}

void replay::player_info::read_from(binary_deserializer& deser) {
    deser.read(id, name, color, hat, pet, skin, impostor);
}

replay::player_info::player_state replay::player_info::get_interpolated(std::int32_t time) const {
    auto it = std::lower_bound(timeline.begin(), timeline.end(), time, [](const auto& info, int dt) {
        return info.dt < dt;
    });

    if (it == timeline.begin()) {
        return timeline.front();
    }

    if (it == timeline.end()) {
        return timeline.back();
    }

    const auto& prev_state = *std::prev(it);
    const auto& next_state = *it;

    float alpha = 1.0f * (time - prev_state.dt) / (next_state.dt - prev_state.dt);

    auto interpolate = [alpha](auto from, auto to) {
        static_assert(std::is_same_v<decltype(from), decltype(to)>);
        return from + static_cast<decltype(from)>(alpha * (to - from));
    };

    return player_state{
        /* dt */ time,
        /* position */ {
            interpolate(prev_state.position[0], next_state.position[0]),
            interpolate(prev_state.position[1], next_state.position[1])
        },
        /* velocity */ {
            interpolate(prev_state.velocity[0], next_state.velocity[0]),
            interpolate(prev_state.velocity[1], next_state.velocity[1])
        },
        /* is_dead */ next_state.is_dead
    };
}

int replay::get_duration() const {
    int duration = 0;
    for (const auto& [id, player] : m_players) {
        duration = std::max(duration, player.timeline.back().dt);
    }
    return duration;
}
