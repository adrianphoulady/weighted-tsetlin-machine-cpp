//
//  Created by Adrian Phoulady on 12/7/19.
//  © 2019 Adrian Phoulady
//

#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include "multiweightm.h"

// get the next option from command line arguments
char getopt(int argc, char * const argv[], char const *optstr, char *&optarg) {
    static int optind = 1;
    if (optind >= argc || *argv[optind] != '-')
        return -1;
    auto opt = *(argv[optind++] + 1);
    auto p = strchr(optstr, opt);
    if (!p)
        return '?';
    if (p[1] == ':') {
        if (optind >= argc)
            return ':';
        optarg = argv[optind++];
    }
    return opt;
}

// read hyper-parameters from command line arguments
void update(int argc, char * const argv[], int &clauses, double &p, int &threshold, double &gamma, int &epochs, bool &shuffle, bool &write, bool &resume) {
    int opt;
    static char *optarg = nullptr;
    while ((opt = getopt(argc, argv, "c:p:t:g:e:n:s:r:w:h", optarg)) != -1)
        switch (opt) {
            case 'h':
                printf("-c clauses\n-p p\n-t threshold\n-g gamma\n-e epochs\n-n new rand\n-s shuffle\n-r resume\n-w write\n");
                break;
            case 'c':
                clauses = atoi(optarg);
                break;
            case 'p':
                p = atof(optarg);
                break;
            case 't':
                threshold = atoi(optarg);
                break;
            case 'g':
                gamma = atof(optarg);
                break;
            case 'e':
                epochs = atoi(optarg);
                break;
            case 'n':
                fastsrand(strcmp(optarg, "0")? atoi(optarg): time(nullptr));
                break;
            case 's':
                shuffle = strcmp(optarg, "0") && strcasecmp(optarg, "false");
                break;
            case 'r':
                resume =  strcmp(optarg, "0") && strcasecmp(optarg, "false");
                break;
            case 'w':
                write =  strcmp(optarg, "0") && strcasecmp(optarg, "false");
        }
}

// load the file into arrays x and y, and get the number of features
int load_file(std::string const &fname, array2d<word> *&x, array1d<int> *&y) {
    std::ifstream fin(fname);
    std::vector<std::vector<int>> data;
    if (!fin) {
        printf("File %s is missing!\n", fname.c_str());
        exit(1);
    }
    for (std::string line; std::getline(fin, line); ) {
        std::istringstream iss{line};
        data.emplace_back(std::vector<int>{});
        std::copy(std::istream_iterator<int>(iss), std::istream_iterator<int>(), std::back_inserter(data.back()));
    }
    fin.close();

    int features = data[0].size() - 1, samples = data.size(), literal_words = (2 * features + word_bits - 1) / word_bits;
    x = new array2d<word>{samples, literal_words};
    y = new array1d<int>{samples};
    std::fill((*x)(0), (*x)(samples), 0);
    for (int s = 0; s < samples; ++s) {
        if (data[s].size() != features + 1u) {
            printf("Inconsistent sample at line %d of %s!", s + 1, fname.c_str());
            exit(2);
        }
        for (int f = 0; f < features; ++f) {
            int l = f + !data[s][f] * features;
            (*x)(s, l / word_bits) |= (word) 1 << l % word_bits;
        }
        (*y)(s) = data[s].back();
    }

    return features;
}

// determine features and classes, and load test and train data
void load_data(std::string const &experiment, int &features, int &classes, array2d<word> *&x_train, array1d<int> *&y_train, array2d<word> *&x_test, array1d<int> *&y_test) {
    load_file("data/" + experiment + "-train.data", x_train, y_train);
    features = load_file("data/" + experiment + "-test.data", x_test, y_test);
    classes = std::max(*std::max_element(&(*y_train)(0), &(*y_train)(y_train->columns)), *std::max_element(&(*y_test)(0), &(*y_test)(y_test->columns))) + 1;
}

// sample train data for faster evaluations in training, assuming size < x.rows
void sample_data(array2d<word> *x, array1d<int> *y, array2d<word> *&xs, array1d<int> *&ys, int size) {
    array1d<int> idx{x->rows};
    for (int i = 0; i < idx.columns; ++i)
        idx(i) = i;
    xs = new array2d<word>{size, x->columns};
    ys = new array1d<int>{size};
    for (int i = 0; i < xs->rows; ++i) {
        std::swap(idx(i), idx(i + fastrandrange(x->rows - i)));
        std::copy((*x)(idx(i)), (*x)(idx(i) + 1), (*xs)(i));
        (*ys)(i) = (*y)(idx(i));
    }
}

// fit a machine on the dataset for the given hyper-parameters.
void fit(std::string const &experiment, int clauses, double p, double gamma, int threshold, int epochs, bool shuffle = false, bool write = false, bool resume = false) {
    clock_t tc0 = clock();

    int features, classes;
    array2d<word> *x_train, *x_test, *x_tray;
    array1d<int> *y_train, *y_test, *y_tray;
    load_data(experiment, features, classes, x_train, y_train, x_test, y_test);
    sample_data(x_train, y_train, x_tray, y_tray, x_test->rows / 4);

    char mname[100];
    sprintf(mname, "results/%s-c%04d-p%04d-g%04d-t%04d.machine", experiment.c_str(), clauses / 10, (int) round(p * 10000 + .0001), (int) round(gamma * 10000 + .0001), threshold);

    multiweightm *wtm = nullptr;
    if (resume) { // deserializing machine
        std::ifstream min(mname, std::ifstream::binary);
        if (min) {
            wtm = new multiweightm(min);
            printf("Continuing at epoch %d\n", wtm->get_epoch() + 1);
            min.close();
        }
    }
    if (!wtm)
        wtm = new multiweightm(classes, features, clauses, p, gamma, threshold);

    printf("samples=%dK - features=%d, clauses=%d, p=%.4f, gamma=%.4f, threshold=%d\n", x_train->rows / 1000, features, clauses, p, gamma, threshold);

    while (wtm->get_epoch() < epochs) {
        clock_t c0 = clock();
        wtm->fit(*x_train, *y_train, 1, shuffle);
        clock_t c1 = clock();
        double e1 = wtm->evaluate(*x_test, *y_test);
        clock_t c2 = clock();
        double e2 = wtm->evaluate(*x_tray, *y_tray);

        printf("epoch %03d of training and testing -", wtm->get_epoch());
        printf(" %04lus and %04lus -", (c1 - c0) / CLOCKS_PER_SEC, (c2 - c1) / CLOCKS_PER_SEC);
        printf(" %6.2f%%  and %6.2f%%\n", 100 * e2, 100 * e1);
    }

    if (write) {
        // serializing machine
        std::ofstream mout(mname, std::ofstream::binary);
        wtm->serialize(mout);
        mout.close();
    }

    int ss = (clock() - tc0) / CLOCKS_PER_SEC, mm = ss / 60, hh = mm / 60;
    printf("total time: %02d:%02d:%02d\n", hh, mm % 60, ss % 60);
}
