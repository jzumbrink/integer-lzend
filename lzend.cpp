/**
 * lzend.cpp
 * part of pdinklag/lzend
 * 
 * MIT License
 * 
 * Copyright (c) Patrick Dinklage
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "libsais.h"
#include "delta_sa.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>

#include <fstream>
#include <memory>

#include "lzend.hpp"

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
    /*{
        std::ifstream ifs(argv[1]);
        s = std::string(std::istreambuf_iterator<char>(ifs), {});
    }*/

    //s = "ababbabb"; // Seg fault
    s = "dnkldflgfdfskmldfskdfsdfmkdflkdflskdfslk"; // No SegFault

    int32_t n = s.length();

    int32_t delta = 4;
    int32_t sample_size = (n-1) / delta;

    // construct suffix array of input
    auto sa = std::make_unique<int32_t[]>(n);
    auto dsa = std::make_unique<int32_t[]>(n);
    auto dsa_sample = std::make_unique<int32_t[]>(sample_size);
    libsais((uint8_t const*) s.data(), sa.get(), n, 0, nullptr);

    for (int32_t i = 0; i < n; i++)
    {
        if (i == 0) {
            dsa[i] = sa[i];
        } else {
            dsa[i] = sa[i] - sa[i-1];
        }
        //std::cout << i << "-th SA entry: " << sa[i] << std::endl;
        if (PRINT_DETAIL)
            std::cout << i << "-th DSA entry: " << dsa[i] << std::endl;
    }

    // construct sample
    for (int32_t i = 0; i < sample_size; i++)
    {
        dsa_sample[i] = sa[(i+1) * delta];
        if (PRINT_DETAIL)
            std::cout << i << "-th DSA sample entry: " << dsa_sample[i] << std::endl;
    }
    
    // test efficient access on delta suffix array
    for (int32_t i = 0; i < n; i++)
    {
        if (PRINT_DETAIL) {
            std::cout << i << "-th SA entry (calculated via DSA): " << sa_value(dsa.get(), dsa_sample.get(), delta, i)
                << " (naive: " << sa_value_naive(dsa.get(), i)
                << ", SA: " << sa[i] << ")" << std::endl;
        }
    }
    
    // parse
    auto const t0 = timestamp();
    auto const z = lzend::parse(s, true).size();
    auto const dt = timestamp() - t0;
    std::cout << "-> z=" << z << " (" << dt << " ms)" << std::endl;
    return 0;
}
