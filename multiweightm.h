//
//  Created by Adrian Phoulady on 8/29/19.
//  © 2019 Adrian Phoulady
//

#include "weightm.h"

class multiweightm {

    int epoch;
    int const classes;
    array1d<weightm> machine;

public:

    // constructor
    multiweightm(int classes, int features, int clauses, double p, double gamma, int threshold, int state_bits = 8)
    : epoch{0},
    classes{classes},
    machine{classes} {
        while (classes--)
            new (&machine(classes)) weightm(features, clauses, p, gamma, threshold, state_bits);
    };

    // train for a single input
    void train(word *x, int y) {
        int zero = fastrandrange(classes - 1), one = y;
        zero += zero >= one;
        machine(zero).train(x, 0);
        machine(one).train(x, 1);
    };

    // fit on the input dataset
    void fit(array2d<word> &x, array1d<int> &y, int epochs, bool mix = false) {
        array1d<int> idx{x.rows};
        // no need to serialize
        for (int i = 0; i < idx.columns; ++i)
            idx(i) = i;
        while (epochs--) {
            if (mix)
                shuffle(idx);
            for (int i = 0; i < x.rows; ++i)
                train(x(idx(i)), y(idx(i)));
            ++epoch;
        }
    };

    // predict the class of a single input
    int predict(word *input) {
        int mxi = 0;
        double mxv = machine(0).infer(input), v;
        for (int m = 1; m < classes; ++m)
            if (mxv < (v = machine(m).infer(input))) {
                mxv = v;
                mxi = m;
            }
        return mxi;
    };

    // evaluate the machine on a dataset
    double evaluate(array2d<word> &x, array1d<int> &y) {
        int correct = 0;
        for (int i = 0; i < x.rows; ++i)
            correct += predict(x(i)) == y(i);
        return (double) correct / x.rows;
    };

    // get the current epoch number
    int get_epoch() const {
        return epoch;
    }

    ///////////////////////////////////////////////////////////////////////////////
    //// Serialization and deserialization
    ///////////////////////////////////////////////////////////////////////////////

    // save the entire machine
    void serialize(std::ostream &os) {
        put(os, epoch);
        put(os, classes);
        for (int c = 0; c < classes; ++c)
            machine(c).serialize(os);
    }

    // deserialize
    explicit multiweightm(std::istream &is)
    : epoch{get<int>(is)},
    classes{get<int>(is)},
    machine{classes} {
        for (int c = 0; c < classes; ++c)
            new (&machine(c)) weightm(is);
    }

};
