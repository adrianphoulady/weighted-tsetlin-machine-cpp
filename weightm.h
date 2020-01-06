//
//  Created by Adrian Phoulady on 8/29/19.
//  © 2019 Adrian Phoulady
//

#include <istream>
#include "fastrand.h"

typedef uint32_t word;
/*inline*/ static int constexpr word_bits = sizeof(word) << 3;

template<typename T>
void put(std::ostream &os, T v) {
    os.write((char *) &v, sizeof(v));
}

template<typename T>
T get(std::istream &is) {
    T v;
    is.read((char *) &v, sizeof(v));
    return v;
}

class weightm {
    int const features;     // number of features
    int const clauses;      // number of clauses
    double const p;         // setter feedback probability
    double const gamma;     // weight learning rate
    int const threshold;    // weighted sum threshold for learning toward
    int const states;       // number of bits of states
    int const literals;     // number of words containing all literals
    word const actmask;     // mask for zeroing the remaining action bits in literal words
    array3d<word> state;    // state of all clauses in [clause, literals, bit] order
    array1d<word> lmask;    // feedback mask for reward and penalty in setter feedback
    array1d<int> clause;    // value of clauses
    array1d<double> weight; // weight associated to each clause

    // increase the states of the automata
    void add(word *state_word, word addend) {
        for (int b = 0; addend && b < states; ++b) {
            state_word[b] ^= addend;
            addend &= state_word[b] ^ addend;
        }
        if (addend)
            for (int b = 0; b < states; ++b)
                state_word[b] ^= addend;
    }

    // decrease the states of the automata
    void subtract(word *state_word, word subtrahend) {
        for (int b = 0; subtrahend && b < states; ++b) {
            state_word[b] ^= subtrahend;
            subtrahend &= ~(state_word[b] ^ subtrahend);
        }
        if (subtrahend)
            for (int b = 0; b < states; ++b)
                state_word[b] ^= subtrahend;
    }

    // prepare the feedback mask with probability p for reward and penalty in the setter feedback
    void literal_mask() {
        int n = features << 1, flips = binomial(p, n);
        bool const target = flips <= features;
        // if flips are more than half, do it the other way; make 0s in an all-1 sequence
        if (!target)
            flips = n - flips;
        std::fill(&lmask(0), &lmask(literals), target - 1);
        while (flips) {
            auto l = fastrandrange(n), w = l / word_bits, b = l % word_bits;
            if ((lmask(w) >> b & 1) == target)
                continue;
            lmask(w) ^= (word) 1 << b;
            --flips;
        }
    }

    // setter feedback or feedback type I
    void setter(int c, word const *x) {
        literal_mask();
        if (clause(c)) {
            weight(c) *= 1 + gamma;
            for (int l = 0; l < literals; ++l) {
                add(state(c, l), x[l]);
                subtract(state(c, l), lmask(l) & ~x[l]);
            }
        }
        else
            for (int l = 0; l < literals; ++l)
                subtract(state(c, l), lmask(l));
    }

    // clearer feedback or feedback type II
    void clearer(int c, word const *x) {
        if (clause(c)) {
            weight(c) /= 1 + gamma;
            for (int l = 0; l < literals; ++l)
                add(state(c, l), ~state(c, l, states - 1) & ~x[l]);
        }
    }

    // value of a clause for an input
    // discard empty clauses instead of having them with value 1 for training == false
    int value(int c, word const *x, bool training = false) {
        word active = 0;
        state(c, literals, -1) &= actmask; // == state(c, literals - 1, states - 1)
        for (int l = 0; l < literals; ++l) {
            auto s = state(c, l, states - 1); // bit (states - 1) is the action bit of the automata
            if ((s & x[l]) != s)
                return clause(c) = 0;
            active |= s;
        }
        return clause(c) = training || active;
    }


public:

    // constructor
    weightm(int features, int clauses, double p, double gamma, int threshold, int states = 8)
    : features{features},
      clauses{clauses},
      p{p},
      gamma{gamma},
      threshold{threshold},
      states{states},
      literals{(2 * features - 1) / word_bits + 1},
      actmask{~((bool) (2 * features % word_bits) * ~((word) 0) << 2 * features % word_bits)},
      state{clauses, literals, states},
      lmask{literals},
      clause{clauses},
      weight{clauses} {
        for (int c = 0; c < clauses; ++c) {
            // even clauses are positive and and odds are negative
            weight(c) = c & 1? -1: +1;
            // initialize all the states to 2^states - 1
            for (int l = 0; l < literals; ++l) {
                for (int b = 0; b < states - 1; ++b)
                    state(c, l, b) = ~((word) 0);
                state(c, l, states - 1) = 0;
            }
        }
    }

    // get weighted sum of clauses for an input
    double infer(word const *x, bool training = false) {
        double inference = 0;
        for (int c = 0; c < clauses; ++c)
            if (value(c, x, training))
                inference += weight(c);
        return inference;
    }

    // train the machine for a single input
    void train(word const *x, int y) {
        double const diversion = .5 + (.5 - y) * infer(x, true) / threshold;
        for (int c = 0; c < clauses; ++c)
            if (fastrandom() < diversion)
                y != (c & 1)? setter(c, x): clearer(c, x);
    }

    // fit the machine on a dataset for a number of epochs
    void fit(array2d<word> &x, array1d<int> &y, int epochs) {
        while (epochs--)
            for (int i = 0; i < x.rows; ++i)
                train(x(i), y(i));
    }

    // predict the 0-1 output for the input
    int predict(word *input) {
        return infer(input) >= 0;
    }

    // evaluate the accuracy of the machine
    double evaluate(array2d<word> &x, array1d<int> &y)  {
        int correct = 0;
        for (int i = 0; i < x.rows; ++i)
            correct += predict(x(i)) == y(i);
        return (double) correct / x.rows;
    }

    ///////////////////////////////////////////////////////////////////////////////
    //// Serialization and deserialization
    ///////////////////////////////////////////////////////////////////////////////

    // save the entire machine
    void serialize(std::ostream &os) {
        put(os, features);
        put(os, clauses);
        put(os, p);
        put(os, gamma);
        put(os, threshold);
        put(os, states);
        put(os, mcg_state);
        for (int c = 0; c < clauses; ++c)
            for (int l = 0; l < literals; ++l)
                for (int b = 0; b < states; ++b)
                    put(os, state(c, l, b));
        for (int c = 0; c < clauses; ++c)
            put(os, weight(c));
    }

    // deserialize; load the machine from an input stream
    explicit weightm(std::istream &is)
    : features{get<int>(is)},
      clauses{get<int>(is)},
      p{get<double>(is)},
      gamma{get<double>(is)},
      threshold{get<int>(is)},
      states{get<int>(is)},
      literals{(2 * features - 1) / word_bits + 1},
      actmask{~((bool) (2 * features % word_bits) * ~((word) 0) << 2 * features % word_bits)},
      state{clauses, literals, states},
      lmask{literals},
      clause{clauses},
      weight{clauses} {
        mcg_state = get<uint64_t>(is);
        for (int c = 0; c < clauses; ++c)
            for (int l = 0; l < literals; ++l)
                for (int b = 0; b < states; ++b)
                    state(c, l, b) = get<word>(is);
        for (int c = 0; c < clauses; ++c)
            weight(c) = get<double>(is);
    }

};
