#pragma once

#include <istream>
#include <ctime>
#include <map>
#include <vector>
#include <array>

struct binary_deserializer;

class replay {
    struct player_info {
        std::uint8_t id;
        std::string name;
        std::uint8_t color;
        std::uint32_t hat;
        std::uint32_t pet;
        std::uint32_t skin;
        bool impostor;

        struct player_state {
            std::int32_t dt;
            std::array<float, 2> position;
            std::array<float, 2> velocity;
            bool is_dead;
        };

        std::vector<player_state> timeline;

        void read_from(binary_deserializer& deser);

        player_state get_interpolated(std::int32_t time) const;
    };

    std::array<char, 4> m_signature;
    std::uint32_t m_version;
    std::time_t m_timestamp;
    std::string m_mod_version;
    std::string m_compatible_game_version;
    std::int32_t m_map_id;
    std::map<std::uint8_t, player_info> m_players;

public:
    explicit replay() = default;

    void parse(std::istream& is);

    int get_map_id() const;

    const std::map<std::uint8_t, player_info>& get_players() const;

    bool is_meeting(int time) const;

    int get_duration() const;
};
