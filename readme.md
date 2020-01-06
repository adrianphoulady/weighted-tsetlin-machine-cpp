# The Weighted Tsetlin Machine in C++
This is a C++ implementation of the [Weighted Tsetlin Machine](https://arxiv.org/abs/1911.12607).

## Contents

- [Usage](#usage)
- [Pre-contained Implementations](#pre-contained-implementations)
  - [Prerequisites](#prerequisites)
  - [MNIST](#mnist)
    - [How to Fit a Machine](#how-to-fit-a-machine)
    - [Arbitrary Machine Configuration](#arbitrary-machine-configuration)
    - [Fitting a Classic Tsetlin Machine](#fitting-a-classic-tsetlin-machine)
    - [Best Tested Hyper-parameters](#best-tested-hyper-parameters)
    - [Best Tested Classic Tsetlin machine Hyper-parameters](#best-tested-classic-tsetlin-machine-hyper-parameters)
  - [IMDb](#imdb)
    - [How to Fit a Machine](#how-to-fit-a-machine)
    - [Best Tested Hyper-parameters](#best-tested-hyper-parameters)
    - [Best Tested Classic Tsetlin machine Hyper-parameters](#best-tested-classic-tsetlin-machine-hyper-parameters)
  - [Connect-4](#connect-4)
    - [How to Fit a Machine](#how-to-fit-a-machine)
    - [Best Tested Hyper-parameters](#best-tested-hyper-parameters)
    - [Best Tested Classic Tsetlin machine Hyper-parameters](#best-tested-classic-tsetlin-machine-hyper-parameters)
- [License](#license)

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

Also, there is a helper function `update`, which updates the parameters to `fit` from command line provided options (see [arbitrary machine configuration](#arbitrary-machine-configuration), for example).
```c++
update(argc, argv, clauses, p, threshold, gamma, epochs, shuffle, write, resume)
```
The options are as follows.

`-c clauses`: number of clauses  
`-p probability`: feedback probability  
`-t threshold`: threshold  
`-g gamma`: learning rate gamma  
`-e epochs`: number of epochs  
`-n seed`: new random at each run by inputting `0`, or otherwise, randoms with the initial seed value of `seed`   
`-s ifshuffle`: if shuffle the training set at each epoch  
`-r ifresume`: if resume the machine  
`-w ifwrite`: if write the trained machine

## Pre-contained Implementations
There are already implementations for MNIST, IMDb, and Connect-4 in the repository.

### Prerequisites
This is how the datasets for the contained implementations are prepared in the `data/` folder.

```sh
$ python3 prepare-mnist-dataset.py
$ python3 prepare-imdb-dataset.py
$ python3 prepare-con4-dataset.py
```
The data files are also contained in zip format in the `data/` folder.
### MNIST
#### How to Fit a Machine
To make an MNIST fitter, `make` for MNIST.

```sh
$ make mnist
g++ -std=c++11 -O3 -Wall -Wextra -o mnist -Dmnist implementations.cpp
```

Thereafter, `./mnist` makes a light implementation of MNIST up and running.

```sh
$ ./mnist
samples=60K, features=784, classes=10 - clauses=500, p=0.0850, gamma=0.00250, threshold=25
epoch 001 of training and testing - 0007s and 0003s -  93.24%  and  93.43%
epoch 002 of training and testing - 0006s and 0003s -  95.16%  and  94.67%
epoch 003 of training and testing - 0005s and 0003s -  95.64%  and  95.17%
.
.
.
epoch 387 of training and testing - 0003s and 0003s -  99.88%  and  98.12%
epoch 388 of training and testing - 0003s and 0003s -  99.92%  and  98.36%
epoch 389 of training and testing - 0003s and 0003s -  99.96%  and  98.18%
.
.
.
epoch 398 of training and testing - 0003s and 0003s - 100.00%  and  98.09%
epoch 399 of training and testing - 0003s and 0003s -  99.96%  and  98.09%
epoch 400 of training and testing - 0003s and 0003s -  99.96%  and  98.13%
total time: 00:53:44
```

#### Arbitrary Machine Configuration
For an arbitrary machine configuration, some command line options are provided. Below, we have a Weighted Tsetlin Machine with 4,000 clauses, feedback probability p of .08, learning rate gamma of .01, and threshold of 100 fitted on MNIST for 3 epochs. We wanted to write the resulting machine on disk.

```sh
$ ./mnist -c 4000 -p .08 -g .01 -t 100 -e 3 -w 1
samples=60K, features=784, classes=10 - clauses=4000, p=0.0800, gamma=0.01000, threshold=100
epoch 001 of training and testing - 0071s and 0038s -  95.36%  and  95.05%
epoch 002 of training and testing - 0051s and 0039s -  96.56%  and  96.08%
epoch 003 of training and testing - 0048s and 0039s -  97.48%  and  96.61%
total time: 00:05:25
```

Now, if we want this saved machine to resume, we pass a `true` value through the command option `r`.

```sh
$ ./mnist -c 4000 -p .08 -g .01 -t 100 -e 5 -r 1 -w 1
Continuing at epoch 4
samples=60K, features=784, classes=10 - clauses=4000, p=0.0800, gamma=0.01000, threshold=100
epoch 004 of training and testing - 0046s and 0039s -  97.68%  and  96.88%
epoch 005 of training and testing - 0045s and 0039s -  97.84%  and  97.05%
total time: 00:03:39
```
#### Fitting a Classic Tsetlin Machine
For having a classic [Tsetlin Machine](https://arxiv.org/abs/1804.01508), just set the learning rate `gamma` to `0`.

```sh
$ ./mnist -g .0 -e 8
samples=60K, features=784, classes=10 - clauses=500, p=0.0850, gamma=0.00000, threshold=25
epoch 001 of training and testing - 0006s and 0002s -  93.32%  and  93.30%
epoch 002 of training and testing - 0006s and 0003s -  94.36%  and  94.66%
epoch 003 of training and testing - 0005s and 0003s -  95.84%  and  95.07%
epoch 004 of training and testing - 0005s and 0003s -  95.88%  and  95.45%
epoch 005 of training and testing - 0005s and 0003s -  95.88%  and  95.79%
epoch 006 of training and testing - 0005s and 0003s -  96.80%  and  96.15%
epoch 007 of training and testing - 0005s and 0003s -  96.60%  and  96.00%
epoch 008 of training and testing - 0005s and 0003s -  96.88%  and  96.21%
total time: 00:01:23
```
#### Best Tested Hyper-parameters
The best tested MNIST hyper-parameter set is as follows.

`clauses = 4000`  
`p = .085`  
`gamma = .012`  
`threshold = 90`  
`epochs = 200`

```sh
$ ./mnist -c 4000 -p .085 -g .012 -t 90 -e 200
samples=60K, features=784, classes=10 - clauses=4000, p=0.0850, gamma=0.01200, threshold=90
epoch 001 of training and testing - 0072s and 0038s -  94.64%  and  94.67%
epoch 002 of training and testing - 0059s and 0038s -  96.96%  and  96.10%
epoch 003 of training and testing - 0057s and 0038s -  97.64%  and  96.60%
.
.
.
epoch 182 of training and testing - 0047s and 0040s - 100.00%  and  98.54%
epoch 183 of training and testing - 0047s and 0040s - 100.00%  and  98.63%
epoch 184 of training and testing - 0047s and 0040s - 100.00%  and  98.52%
.
.
.
epoch 199 of training and testing - 0047s and 0040s - 100.00%  and  98.54%
epoch 200 of training and testing - 0047s and 0040s - 100.00%  and  98.51%
total time: 05:30:56
```
As it is shown, it reaches the peak accuracy of 98.63%.

#### Best Tested Classic Tsetlin machine Hyper-parameters
The best tested MNIST hyper-parameter set of a classic Tsetlin machine is as follows.
    
`clauses = 4000`  
`p = .1`  
`threshold = 100`  
`epochs = 200`

```sh
$ ./mnist -c 4000 -p .1 -g .0 -t 100 -e 200
samples=60K - features=784, clauses=4000, p=0.1000, gamma=0.0000, threshold=100
epoch 001 of training and testing - 0080s and 0041s -  94.76%  and  94.62%
epoch 002 of training and testing - 0067s and 0041s -  96.44%  and  95.79%
epoch 003 of training and testing - 0064s and 0041s -  97.04%  and  96.20%
.
.
.
epoch 195 of training and testing - 0057s and 0046s -  99.72%  and  98.23%
epoch 196 of training and testing - 0057s and 0046s -  99.68%  and  98.24%
epoch 197 of training and testing - 0058s and 0049s -  99.68%  and  98.23%
epoch 198 of training and testing - 0061s and 0049s -  99.68%  and  98.16%
epoch 199 of training and testing - 0062s and 0049s -  99.68%  and  98.15%
epoch 200 of training and testing - 0061s and 0049s -  99.64%  and  98.16%
total time: 06:36:41
```
As it is shown, it reaches the peak accuracy of 98.24%.


### IMDb
#### How to Fit a Machine
A light implementation of IMDb runs as follows.

```sh
$ make imdb
g++ -std=c++11 -O3 -Wall -Wextra -o imdb -Dimdb implementations.cpp
$ ./imdb
samples=25K, features=5000, classes=2 - clauses=3200, p=0.0120, gamma=0.00060, threshold=12
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
#### Best Tested Hyper-parameters
The best tested IMDb hyper-parameter set is as follows.

`clauses = 25000`  
`p = .008`  
`gamma = .006`  
`threshold = 60`  
`epochs = 25`

```sh
$ ./imdb -c 25000 -p .008 -g .006 -t 60 -e 25
samples=25K, features=5000, classes=2 - clauses=25000, p=0.0080, gamma=0.0060, threshold=60
epoch 001 of training and testing - 0724s and 0264s -  89.68%  and  87.22%
epoch 002 of training and testing - 0623s and 0272s -  91.79%  and  88.50%
epoch 003 of training and testing - 0576s and 0277s -  92.93%  and  89.12%
.
.
.
epoch 020 of training and testing - 0399s and 0286s -  98.29%  and  90.02%
epoch 021 of training and testing - 0397s and 0285s -  98.75%  and  90.37%
epoch 022 of training and testing - 0392s and 0284s -  98.86%  and  90.37%
.
.
.
epoch 024 of training and testing - 0384s and 0283s -  98.99%  and  90.30%
epoch 025 of training and testing - 0380s and 0284s -  99.01%  and  90.19%
total time: 05:46:49
```
As it is shown, it reaches the peak accuracy of 90.37%.

#### Best Tested Classic Tsetlin machine Hyper-parameters
The best tested IMDb hyper-parameter set of a classic Tsetlin machine is as follows.
  
`clauses = 25000`  
`p = .02`  
`threshold = 150`  
`epochs = 25`

```sh
$ ./imdb -c 25000 -p .02 -g .0 -t 150 -e 25
samples=25K, features=5000, classes=2 - clauses=25000, p=0.0200, gamma=0.0000, threshold=150
epoch 001 of training and testing - 0823s and 0269s -  88.45%  and  87.68%
epoch 002 of training and testing - 0709s and 0277s -  90.19%  and  88.60%
epoch 003 of training and testing - 0635s and 0280s -  91.26%  and  89.05%
.
.
.
epoch 021 of training and testing - 0473s and 0284s -  95.44%  and  89.86%
epoch 022 of training and testing - 0499s and 0283s -  95.47%  and  89.81%
epoch 023 of training and testing - 0433s and 0280s -  95.74%  and  89.77%
epoch 024 of training and testing - 0424s and 0283s -  95.76%  and  89.73%
epoch 025 of training and testing - 0468s and 0287s -  96.08%  and  89.84%
total time: 06:34:31
```

As it is shown, it reaches the peak accuracy of 89.86%.

### Connect-4
#### How to Fit a Machine
An exceptionally light implementation of Connect-4 runs below.

```sh
$ make connect4
g++ -std=c++11 -O3 -Wall -Wextra -o connect4 -Dconnect4 implementations.c++
$ ./connect4
samples=60K, features=84, classes=3 - clauses=200, p=0.0370, gamma=0.00010, threshold=12
epoch 001 of training and testing - 0001s and 0000s -  68.48%  and  69.43%
epoch 002 of training and testing - 0001s and 0000s -  70.08%  and  71.44%
.
.
.
epoch 132 of training and testing - 0000s and 0000s -  81.28%  and  80.64%
epoch 133 of training and testing - 0000s and 0000s -  82.11%  and  81.10%
epoch 134 of training and testing - 0000s and 0000s -  81.99%  and  80.34%
.
.
.
epoch 197 of training and testing - 0000s and 0000s -  80.27%  and  80.07%
epoch 198 of training and testing - 0000s and 0000s -  80.21%  and  79.93%
epoch 199 of training and testing - 0000s and 0000s -  81.04%  and  80.22%
epoch 200 of training and testing - 0000s and 0000s -  81.34%  and  80.13%
total time: 00:03:29
```

#### Best Tested Hyper-parameters
The best tested Connect-4 hyper-parameter set is as follows.
  
`clauses = 25000`  
`p = .0065`  
`gamma = .0007`  
`threshold = 100`  
`epochs = 1000`

```sh
$ ./connect4 -c 25000 -p .0065 -g .0007 -t 100 -e 1000
samples=60K, features=84, classes=3 - clauses=25000, p=0.0065, gamma=0.00070, threshold=100
epoch 001 of training and testing - 0121s and 0008s -  70.85%  and  70.69%
epoch 002 of training and testing - 0114s and 0007s -  72.33%  and  72.18%
epoch 003 of training and testing - 0112s and 0007s -  74.11%  and  73.13%
.
.
.
epoch 939 of training and testing - 0050s and 0007s -  99.94%  and  87.83%
epoch 940 of training and testing - 0051s and 0007s -  99.88%  and  87.73%
epoch 941 of training and testing - 0051s and 0007s -  99.82%  and  87.91%
.
.
.
epoch 999 of training and testing - 0051s and 0007s -  99.41%  and  87.34%
epoch 1000 of training and testing - 0051s and 0007s -  99.64%  and  87.24%
total time: 21:05:14
```
As it is shown, it reaches the peak accuracy of 87.91%.
#### Best Tested Classic Tsetlin machine Hyper-parameters
The best tested Connect-4 hyper-parameter set of a classic Tsetlin machine is as follows.
    
`clauses = 25000`  
`p = .02`  
`threshold = 150`  
`epochs = 25`

```sh
$ ./connect4 -c 25000 -p .033 -g .0 -t 100 -e 1000
samples=60K, features=84, classes=3 - clauses=25000, p=0.0330, gamma=0.00000, threshold=100
epoch 001 of training and testing - 0124s and 0008s -  73.58%  and  73.52%
epoch 002 of training and testing - 0121s and 0008s -  75.47%  and  74.77%
epoch 003 of training and testing - 0119s and 0007s -  76.36%  and  75.56%
.
.
.
epoch 970 of training and testing - 0082s and 0007s -  85.07%  and  82.62%
epoch 971 of training and testing - 0083s and 0007s -  84.95%  and  82.72%
epoch 972 of training and testing - 0082s and 0007s -  85.43%  and  82.93%
.
.
.
epoch 999 of training and testing - 0084s and 0007s -  85.07%  and  82.55%
epoch 1000 of training and testing - 0083s and 0007s -  85.31%  and  82.64%
total time: 29:20:46
```

As it is shown, it reaches the peak accuracy of 82.93%.  

## License
© 2020 Adrian Phoulady  
This project is licensed under the MIT License.  
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
