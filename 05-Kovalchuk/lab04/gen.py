#!/usr/bin/env python

import random

if __name__ == '__main__':
    size = 2000
    mtx = [
        [random.random() * 100 for _ in range(size)]
        for _ in range(size)
    ]
    vec = [random.random() * 100 for _ in range(size)]


    with open('matrix.in', 'w') as f:
        f.write(str(size) + '\n')
        f.write('\n'.join(' '.join(map(str, row)) for row in mtx))
        f.write('\n')

    with open('b.in', 'w') as f:
        f.write(str(size) + ' ')
        f.write(' '.join(map(str, vec)))
        f.write('\n')
