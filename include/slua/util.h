//
// Created by erik on 20-12-19.
//

#ifndef SIMPLELUA_UTIL_H
#define SIMPLELUA_UTIL_H

template<int... Is>
struct seq {
};

template<int N, int... Is>
struct gen_seq : gen_seq<N - 1, N - 1, Is...> {
};

template<int... Is>
struct gen_seq<0, Is...> : seq<Is...> {
};

#endif //SIMPLELUA_UTIL_H
