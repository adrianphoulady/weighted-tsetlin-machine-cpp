# The Weighted Tsetlin Machine
This is a C++ implementation of the [Weighted Tsetlin Machine](https://arxiv.org/abs/1911.12607).

## Usage
The following shows a minimal example to fit a Weighted Tsetlin Machine on the `dddd` dataset .

```c++
#include "utils.h"

int main(int argc, char * const argv[]) {
    int clauses = 500, threshold = 20, epochs = 400;
    double p = .075, gamma = .002;
    std::string const experiment = "dddd";

    fit(experiment, clauses, p, gamma, threshold, epochs);

    return 0;
}
```
For the `dddd` dataset, you should make the files `dddd-train.data` and `dddd-test.data` available in the `data/` folder. The `.data` files consist of samples, each at one line, made up of the binary features followed by an integer label, all separated by white spaces.

The function `fit`'s signature is
```c++
fit(experiment, clauses, p, gamma, threshold, epochs, shuffle, write, resume)
```
where `shuffle` makes the training samples shuffle at each epoch, `write` says whether to save the final trained machine to the disk, and `resume` determines if the machine should be loaded from disk and resumed for training. For saving and loading the machine, there should be a folder `results/` present in the working directory.   

Also, there is a helper function `update`, which updates the parameters to `fit` from command line provided options (see the arbitrary machine configuration [here](#options), for example).
```c++
update(argc, argv, clauses, p, threshold, gamma, epochs, shuffle, write, resume)
```
[discarded]: # (
The options are as follows.    
`-c`: number of clauses  
`-p`: feedback probability  
`-t`: threshold  
`-g`: gamma  
`-e`: number of epochs  
`-n`: new random at each run by inputting `0`  
`-s`: shuffle  
`-r`: resume  
`-w`: write)

## Pre-contained Implementations
There are already implementations for MNIST, IMDb, and Connect-4 in the repository.

### Prerequisites
This is how the datasets for the contained implementations are prepared in the `data/` folder.

```sh
$ python3 prepare-mnist-dataset.py
$ python3 prepare-imdb-dataset.py
$ python3 prepare-con4-dataset.py
```

### MNIST
To make an MNIST fitter, `make` for MNIST.

```sh
$ make mnist
g++ -std=c++14 -O3 -Wall -Wextra -o mnist -Dmnist implementations.cpp
```

Thereafter, `./mnist` makes a light implementation of MNIST up and running.

```sh
$ ./mnist
samples=60K - features=784, clauses=500, p=0.0750, gamma=0.0020, threshold=20
epoch 001 of training and testing - 0007s and 0003s -  92.32%  and  93.08%
epoch 002 of training and testing - 0005s and 0003s -  94.68%  and  94.56%
.
.
.
epoch 388 of training and testing - 0003s and 0003s -  99.88%  and  98.31%
epoch 389 of training and testing - 0003s and 0003s -  99.92%  and  98.22%
epoch 390 of training and testing - 0003s and 0003s -  99.88%  and  98.37%
.
.
.
epoch 397 of training and testing - 0003s and 0003s -  99.96%  and  98.04%
epoch 398 of training and testing - 0003s and 0003s -  99.96%  and  98.12%
epoch 399 of training and testing - 0003s and 0003s -  99.92%  and  98.11%
epoch 400 of training and testing - 0003s and 0003s -  99.96%  and  98.13%
total time: 00:53:37
```

<a id='options'></a>
For an arbitrary machine configuration, some command line options are provided. Below, we have a Weighted Tsetlin Machine with 4,000 clauses, feedback probability p of .08, learning rate gamma of .01, and threshold of 100 fitted on MNIST for 3 epochs. We wanted to write the resulting machine on disk.

```sh
$ ./mnist -c 4000 -p .08 -g .01 -t 100 -e 3 -w 1
samples=60K - features=784, clauses=4000, p=0.0800, gamma=0.0100, threshold=100
epoch 001 of training and testing - 0081s and 0038s -  95.36%  and  95.05%
epoch 002 of training and testing - 0061s and 0039s -  96.56%  and  96.08%
epoch 003 of training and testing - 0058s and 0039s -  97.48%  and  96.61%
total time: 00:05:56
```

Now, if we want this saved machine to resume, we pass a `true` value to the command option `r`.

```sh
$ ./mnist -c 4000 -p .08 -g .01 -t 100 -e 5 -r 1 -w 1
Continuing at epoch 4
samples=60K - features=784, clauses=4000, p=0.0800, gamma=0.0100, threshold=100
epoch 004 of training and testing - 0056s and 0039s -  97.68%  and  96.88%
epoch 005 of training and testing - 0055s and 0039s -  97.84%  and  97.05%
total time: 00:03:39
```

For having a classic [Tsetlin Machine](https://arxiv.org/abs/1804.01508), just set the learning rate `gamma` to `0`.

```sh
$ ./mnist -g .0 -e 8
samples=60K - features=784, clauses=500, p=0.0750, gamma=0.0000, threshold=20
epoch 001 of training and testing - 0007s and 0002s -  92.84%  and  93.02%
epoch 002 of training and testing - 0006s and 0003s -  94.92%  and  94.46%
epoch 003 of training and testing - 0005s and 0003s -  95.40%  and  95.23%
epoch 004 of training and testing - 0005s and 0003s -  96.08%  and  95.42%
epoch 005 of training and testing - 0005s and 0003s -  96.28%  and  95.70%
epoch 006 of training and testing - 0005s and 0003s -  96.84%  and  96.14%
epoch 007 of training and testing - 0005s and 0003s -  96.92%  and  95.97%
epoch 008 of training and testing - 0005s and 0003s -  97.20%  and  96.12%
total time: 00:01:25
```

### IMDb
A light implementation of IMDb runs as follows.

```sh
$ make imdb
g++ -std=c++14 -O3 -Wall -Wextra -o imdb -Dimdb implementations.cpp
$ ./imdb
samples=25K - features=5000, clauses=3200, p=0.0120, gamma=0.0006, threshold=12
epoch 001 of training and testing - 0114s and 0036s -  86.35%  and  84.27%
epoch 002 of training and testing - 0101s and 0037s -  88.62%  and  86.59%
.
.
.
epoch 018 of training and testing - 0074s and 0038s -  94.05%  and  89.31%
epoch 019 of training and testing - 0076s and 0039s -  93.63%  and  89.16%
epoch 020 of training and testing - 0075s and 0039s -  93.63%  and  88.58%
.
.
.
epoch 032 of training and testing - 0071s and 0039s -  95.06%  and  89.28%
epoch 033 of training and testing - 0071s and 0040s -  95.55%  and  89.25%
epoch 034 of training and testing - 0070s and 0039s -  95.62%  and  89.23%
epoch 035 of training and testing - 0069s and 0039s -  95.52%  and  89.48%
total time: 01:15:15
```

## Connect-4
An exceptionally light implementation of Connect-4 runs below.

```sh
$ make connect4
g++ -std=c++14 -O3 -Wall -Wextra -o connect4 -Dconnect4 implementations.c++
$ ./connect4
samples=60K - features=84, clauses=200, p=0.0370, gamma=0.0001, threshold=12
epoch 001 of training and testing - 0001s and 0000s -  69.37%  and  70.02%
epoch 002 of training and testing - 0001s and 0000s -  71.62%  and  71.50%
.
.
.
epoch 188 of training and testing - 0000s and 0000s -  82.82%  and  82.01%
epoch 189 of training and testing - 0000s and 0000s -  82.46%  and  82.31%
epoch 190 of training and testing - 0000s and 0000s -  82.46%  and  82.13%
.
.
.
epoch 197 of training and testing - 0000s and 0000s -  82.46%  and  82.24%
epoch 198 of training and testing - 0000s and 0000s -  82.70%  and  82.22%
epoch 199 of training and testing - 0000s and 0000s -  82.11%  and  81.97%
epoch 200 of training and testing - 0000s and 0000s -  83.23%  and  81.98%
total time: 00:03:24
```

## License
© 2019 Adrian Phoulady  
This project is licensed under the MIT License.  
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
