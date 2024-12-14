#include "libsais.h"

#include <algorithm>
#include <cassert>
#include <chrono>

#include <fstream>
#include <memory>

#include "integer-lzend.hpp"

bool PRINT_DETAIL = true;
bool USE_FILE = true;

uintmax_t timestamp() {
    return std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
}

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cerr << "usage: " << argv[0] << " [FILE]" << std::endl;
        return -1;
    }
    
    // load input file
    std::string s;
    {
        std::ifstream ifs(argv[1]);
        s = std::string(std::istreambuf_iterator<char>(ifs), {});
    }

    int32_t n = s.length();

    // construct suffix array of input
    auto sa = std::make_unique<int32_t[]>(n);
    libsais((uint8_t const*) s.data(), sa.get(), n, 0, nullptr);
    
    // parse
    auto const t0 = timestamp();
    auto const z = lzend::parse(sa.get(), n, true).size();
    auto const dt = timestamp() - t0;
    std::cout << "-> z=" << z << " (" << dt << " ms)" << std::endl;
    return 0;
}
