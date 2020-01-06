#
#  Created by Adrian Phoulady on 12/6/19.
#  (c) 2019 Adrian Phoulady.
#

import numpy as np
import mnist


def savexy(x, y, threshold, filename):
    xy = np.concatenate((x.reshape(-1, 28 * 28) > int(threshold * 255), y.reshape(-1, 1)), 1)
    np.savetxt(filename, xy, '%d')


t = .3
savexy(mnist.train_images(), mnist.train_labels(), t, 'mnist-train.data')
savexy(mnist.test_images(), mnist.test_labels(), t, 'mnist-test.data')
