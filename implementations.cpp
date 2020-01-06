//
//  Created by Adrian Phoulady on 12/7/19.
//  © 2019 Adrian Phoulady
//

#include "utils.h"

int main(int argc, char * const argv[]) {
#if defined imdb
    int clauses = 3200, threshold = 12, epochs = 35;
    double p = .012, gamma = .0006;
    std::string const experiment = "imdb";
#elif defined connect4
    int clauses = 200, threshold = 12, epochs = 200;
    double p = .037, gamma = .0001;
    std::string const experiment = "con4";
#else // default: mnist
    int clauses = 500, threshold = 25, epochs = 400;
    double p = .085, gamma = .0025;
    std::string const experiment = "mnist";
#endif

    bool shuffle = false, resume = false, write = false;
    update(argc, argv, clauses, p, threshold, gamma, epochs, shuffle, write, resume);
    fit(experiment, clauses, p, gamma, threshold, epochs, shuffle, write, resume);

    return 0;
}
