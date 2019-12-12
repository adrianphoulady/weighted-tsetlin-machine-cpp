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

    int const literals;
    int const clauses;
    double const p;
    double const gamma;
    int const threshold;
    int const state_bits;
    int const literal_words;
    word const literal_mask;
    array3d<word> state;
    array1d<word> feedback_mask;
    array1d<int> clause;
    array1d<double> weight;

    // increase the states of the automata
    void add(word *feature, word addend) {
        for (int b = 0; addend && b < state_bits; ++b) {
            feature[b] ^= addend;
            addend &= feature[b] ^ addend;
        }
        if (addend)
            for (int b = 0; b < state_bits; ++b)
                feature[b] ^= addend;
    }

    // decrease the states of the automata
    void subtract(word *feature, word subtrahend) {
        for (int b = 0; subtrahend && b < state_bits; ++b) {
            feature[b] ^= subtrahend;
            subtrahend &= ~(feature[b] ^ subtrahend);
        }
        if (subtrahend)
            for (int b = 0; b < state_bits; ++b)
                feature[b] ^= subtrahend;
    }

    // prepare the feedback mask with probability p for reward and penalty in the setter feedback
    void prepare_mask() {
        int flips = binomial(p, literals);
        bool const target = flips <= literals / 2;
        // if flips is more than half, do it the other way; make 0s in an all-1 sequence
        if (!target)
            flips = literals - flips;
        std::fill(&feedback_mask(0), &feedback_mask(literal_words), target - 1);
        while (flips) {
            auto l = fastrandrange(literals), w = l / word_bits, b = l % word_bits;
            // might be one or two machine instructions less if we had done it in two separate loops for target 0 and target 1
            if ((feedback_mask(w) >> b & 1) == target)
                continue;
            feedback_mask(w) ^= (word) 1 << b;
            --flips;
        }
    }

    // setter feedback or feedback type I
    void setter(int c, word const *x) {
        prepare_mask();
        if (clause(c)) {
            weight(c) *= 1 + gamma;
            for (int l = 0; l < literal_words; ++l) {
                add(state(c, l), x[l]);
                subtract(state(c, l), feedback_mask(l) & ~x[l]);
            }
        }
        else
            for (int l = 0; l < literal_words; ++l)
                subtract(state(c, l), feedback_mask(l));
    }

    // clearer feedback or feedback type II
    void clearer(int c, word const *x) {
        if (clause(c)) {
            weight(c) /= 1 + gamma;
            for (int l = 0; l < literal_words; ++l)
                add(state(c, l), ~state(c, l, state_bits - 1) & ~x[l]);
        }
    }

    // value of a clause for an input
    // discard empty clauses instead of having them with value 1 for training == false
    double value(int c, word const *x, bool training = false) {
        word active = 0;
        state(c, literal_words - 1, state_bits - 1) &= literal_mask;
        clause(c) = 1;
        for (int i = 0; i < literal_words; ++i) {
            // bit (state_bits - 1) is the action bit of the automata
            auto s = state(c, i, state_bits - 1);
            if ((s & x[i]) != s)
                return clause(c) = 0;
            active |= s;
        }
        if (training || active)
            return weight(c);
        return clause(c) = 0;
    }

public:

    // constructor
    weightm(int features, int clauses, double p, double gamma, int threshold, int state_bits = 8)
    : literals{2 * features},
    clauses{clauses},
    p{p},
    gamma{gamma},
    threshold{threshold},
    state_bits{state_bits},
    literal_words{(literals - 1) / word_bits + 1},
    literal_mask{~((bool) (literals % word_bits) * ~((word) 0) << literals % word_bits)},
    state{clauses, literal_words, state_bits},
    feedback_mask{literal_words},
    clause{clauses},
    weight{clauses} {
        for (int c = 0; c < clauses; ++c) {
            // even clauses are positive and and odds are negative
            weight(c) = c & 1? -1: +1;
            // initialize all the states to 2^state_bits - 1
            for (int l = 0; l < literal_words; ++l) {
                for (int b = 0; b < state_bits - 1; ++b)
                    state(c, l, b) = ~((word) 0);
                state(c, l, state_bits - 1) = 0;
            }
        }
    }

    // get weighted sum of clauses for an input
    double infer(word const *x, bool training = false) {
        double inference = 0;
        for (int c = 0; c < clauses; ++c)
            inference += value(c, x, training);
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
        put(os, literals);
        put(os, clauses);
        put(os, p);
        put(os, gamma);
        put(os, threshold);
        put(os, state_bits);
        put(os, mcg_state);
        for (int c = 0; c < clauses; ++c)
            for (int l = 0; l < literal_words; ++l)
                for (int b = 0; b < state_bits; ++b)
                    put(os, state(c, l, b));
        for (int c = 0; c < clauses; ++c)
            put(os, weight(c));
    }

    // deserialize; load the machine from an input stream
    explicit weightm(std::istream &is)
    : literals{get<int>(is)},
    clauses{get<int>(is)},
    p{get<double>(is)},
    gamma{get<double>(is)},
    threshold{get<int>(is)},
    state_bits{get<int>(is)},
    literal_words{(literals - 1) / word_bits + 1},
    literal_mask{~((bool) (literals % word_bits) * ~((word) 0) << literals % word_bits)},
    state{clauses, literal_words, state_bits},
    feedback_mask{literal_words},
    clause{clauses},
    weight{clauses} {
        mcg_state = get<uint64_t>(is);
        for (int c = 0; c < clauses; ++c)
            for (int l = 0; l < literal_words; ++l)
                for (int b = 0; b < state_bits; ++b)
                    state(c, l, b) = get<word>(is);
        for (int c = 0; c < clauses; ++c)
            weight(c) = get<double>(is);
    }

};
