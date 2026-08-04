#pragma once

#include <optional>
#include <string_view>

namespace folio {

inline std::optional<bool> parseClosed(std::string_view status) {
    constexpr std::string_view marker = ",position:";
    constexpr std::string_view prefix = "position:";
    const std::size_t markerPosition = status.find(marker);
    if (markerPosition == std::string_view::npos) {
        if (status.substr(0, prefix.size()) != prefix) {
            return std::nullopt;
        }
    }

    const std::size_t valuePosition = markerPosition == std::string_view::npos
                                              ? prefix.size()
                                              : markerPosition + marker.size();
    if (valuePosition >= status.size()) {
        return std::nullopt;
    }

    const char value = status[valuePosition];
    if (value != '0' && value != '1') {
        return std::nullopt;
    }

    const std::size_t delimiterPosition = valuePosition + 1;
    if (delimiterPosition < status.size() && status[delimiterPosition] != ',' &&
        status[delimiterPosition] != '\n') {
        return std::nullopt;
    }

    return value == '1';
}

}  // namespace folio
