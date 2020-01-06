#
#  Created by Adrian Phoulady on 12/6/19.
#  (c) 2019 Adrian Phoulady.
#

import csv
import random
import numpy as np


def binarize(sample):
    b = []
    for e in sample:
        if e == 'x':
            b.extend([1, 0])
        elif e == 'o':
            b.extend([0, 1])
        elif e == 'b':
            b.extend([0, 0])
        elif e == 'loss':
            b.append(0)
        elif e == 'draw':
            b.append(1)
        elif e == 'win':
            b.append(2)
    return b


with open('connect-4.data') as f:
    samples = np.asarray([binarize(row) for row in csv.reader(f)])

test_percentage = 10
indices = random.Random(0).sample(range(len(samples)), len(samples) // test_percentage)

np.savetxt('con4-test.data', samples[indices], '%d')
np.savetxt('con4-train.data', np.delete(samples, indices, 0), '%d')
