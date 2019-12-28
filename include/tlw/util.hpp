//
// Created by erik on 20-12-19.
//

#ifndef TLW_UTIL_H
#define TLW_UTIL_H

template<int... Is>
struct seq {
};

template<int N, int... Is>
struct gen_seq : gen_seq<N - 1, N - 1, Is...> {
};

template<int... Is>
struct gen_seq<0, Is...> : seq<Is...> {
};

#endif //TLW_UTIL_H
