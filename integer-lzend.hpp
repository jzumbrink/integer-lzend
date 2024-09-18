#ifndef _INTEGER_LZEND_HPP
#define _INTEGER_LZEND_HPP

#include "libsais.h"

#include <cstdint>
#include <iostream>
#include <functional>
#include <vector>
#include <memory>

#include <rmq/rmq.hpp>
#include <ordered/btree/map.hpp>

namespace int_lzend {
    
using Index = int32_t;

struct IntPhrase {
    Index lnk;
    Index len;
    Index ext;
};

std::vector<IntPhrase> parse(Index dsa[], Index const n, bool print_progress = false) {
    if(print_progress) std::cout << "Integer-LZ-End input: n=" << n << std::endl;

    // find minimum of delta suffix array
    Index minimum = 0;
    for (Index i = 0; i < n; i++)
    {
        if (dsa[i] < minimum) {
            minimum = dsa[i];
        }
    }
    Index absmin = abs(minimum);
    

    // reverse delta suffix array
    Index rdsa[n];
    for (Index i = 0; i < n; i++)
    {
        rdsa[n - 1 - i] = dsa[i] + absmin; // TODO evaluate if the best variant is used
        std::cout << rdsa[n-1-i] << std::endl;
    }

    // construct suffix array of reverse delta suffix array
    if(print_progress) std::cout << "\tconstruct suffix array ..." << std::endl;
    auto int_sa = std::make_unique<Index[]>(n);
    std::cout << "hi" << std::endl;
    libsais_int(rdsa, int_sa.get(), n, n + absmin, 0); // TODO
    // Segmentation fault, when negative Integer are present in rdsa??
    // libsais_int only works on non-negative integers
    // Variant 1: convert dsa into an array of non-negative integers, i.e. add the absolute of the lowest negative number to all numbers
    // Variant 2: use unsigned integers, i.e. uint32_t

    // construct PLCP array and the LCP array from it
    if(print_progress) std::cout << "\tconstruct LCP array ..." << std::endl;
    auto isa = std::make_unique<Index[]>(n);
    auto& plcp = isa;
    libsais_plcp_int(rdsa, int_sa.get(), isa.get(), n);

    auto lcp = std::make_unique<Index[]>(n);
    libsais_lcp(plcp.get(), int_sa.get(), lcp.get(), n);

    // construct RMQ data structure
    if(print_progress) std::cout << "\tconstruct RMQ ..." << std::endl;
    rmq::RMQ<Index> rmq(lcp.get(), n);

    // construct permuted inverse suffix array
    if(print_progress) std::cout << "\tconstruct permuted inverse suffix array ..." << std::endl;
    for (Index i = 0; i < n; i++)
    {
        isa[n-int_sa[i]-1] = i;
    }

    // discard suffix array and reverse text
    int_sa.reset();
    // TODO: discard reverse dsa: rdsa = std::string();
    
    // initialize predecessor/successor
    ordered::btree::Map<Index, Index> marked;
    

    // helpers
    struct Candidate { Index lex_pos; Index lnk; Index len; };

    auto lex_smaller_phrase = [&](Index const x){
        auto const r = marked.predecessor(x-1);
        return r.exists
            ? Candidate { r.key, r.value, lcp[rmq(r.key+1, x)] }
            : Candidate { 0, 0, 0 };
    };

    auto lex_greater_phrase = [&](Index const x){
        auto const r = marked.successor(x+1);
        return r.exists
            ? Candidate { r.key, r.value, lcp[rmq(x+1, r.key)] }
            : Candidate { 0, 0, 0 };
    }; 

    // parse
    std::cout << "\tparse ... ";
    std::cout.flush();

    std::vector<IntPhrase> parsing;
    parsing.push_back({0, 1, dsa[0]}); // initial empty phrase
    Index z = 0; // index of latest phrase (number of phrases would be z+1)

    for (Index i = 1; i < n; i++) {
        Index const len1 = parsing[z].len;
        Index const len2 = len1 + (z > 0 ? parsing[z-1].len : 0);

        Index const isa_last = isa[i-1];

        // find source phrase candidates
        Index p1 = -1, p2 = -1;
        auto find_copy_source = [&](std::function<Candidate(Index)> f){
            auto c = f(isa_last);
            if(c.len >= len1) {
                p1 = c.lnk;
                if(i > len1) {
                    if(c.lnk == z-1) c = f(c.lex_pos);
                    if(c.len >= len2) p2 = c.lnk;
                }
            }
        };

        find_copy_source(lex_smaller_phrase);
        if(p1 == -1 || p2 == -1) {
            find_copy_source(lex_greater_phrase);
        }

        // case distinction according to Lemma 2
        if(p2 != -1) {
            // merge last two phrases
            marked.erase(isa[i - 1 - len1]);
            
            parsing.pop_back();
            --z;
            
            parsing.back() = IntPhrase { p2, len2 + 1, dsa[i] };
        } else if(p1 != -1) {
            // extend last phrase
            parsing.back() = IntPhrase { p1, len1 + 1, dsa[i] };
        } else {
            // lazily mark previous phrase
            marked.insert(isa_last, z);

            // begin new phrase
            parsing.push_back(IntPhrase { 0, 1, dsa[i] });
            ++z;
        }
    }
    
    std::cout << "n=" << n << std::endl;

    return parsing;
}

}

#endif