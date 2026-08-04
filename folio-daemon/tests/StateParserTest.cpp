#include "../StateParser.h"

#include <cstdlib>
#include <iostream>
#include <optional>
#include <string_view>

namespace {

void expect(std::string_view input, std::optional<bool> expected) {
    const std::optional<bool> actual = folio::parseClosed(input);
    if (actual != expected) {
        std::cerr << "parseClosed failed for: " << input << '\n';
        std::exit(EXIT_FAILURE);
    }
}

}  // namespace

int main() {
    expect("CHECK,offset:0,step:50,far_th:150,far_noise_th:125,position:0,err_state:0\n", false);
    expect("CHECK,offset:-200,step:50,far_th:-50,far_noise_th:-75,position:1,err_state:0\n", true);
    expect("position:0", false);
    expect("position:1", true);

    expect("", std::nullopt);
    expect("position:", std::nullopt);
    expect("position:2", std::nullopt);
    expect("position:10", std::nullopt);
    expect("position:0x", std::nullopt);
    expect("last_position:1", std::nullopt);

    return EXIT_SUCCESS;
}
