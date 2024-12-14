#include "libsais/libsais.h"

#include <algorithm>
#include <cassert>

#include <fstream>
#include <memory>
#include <vector>

#include "integer-lzend.hpp"
#include "time.hpp"

#define PRINT_DETAIL true

int32_t max_phrase_length(std::vector<lzend::IntPhrase> const &phrases) {
    int32_t result = -1;
    for (lzend::IntPhrase p : phrases) {
        if (p.len > result) result = p.len;
    }
    return result;
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

    // construct differential suffix array (introduces true repetitions)
    auto dsa = std::make_unique<int32_t[]>(n);
    if (n > 0) dsa[0] = sa[0];
    
    for (int i = 1; i < n; i++) {
        dsa[i] = sa[i] - sa[i - 1];
    }

    // parse
    auto const t0 = timestamp();
    std::vector<lzend::IntPhrase> const phrases = lzend::parse(dsa.get(), n, PRINT_DETAIL);
    auto const dt = timestamp() - t0;
    std::cout << "-> z=" << phrases.size() << " (" << dt << " ms)" << std::endl;
    std::cout << "-> max_phrase_length=" << max_phrase_length(phrases) << std::endl;
    return 0;
}
