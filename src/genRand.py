import sys
import numpy as np

LEN_VEC = 50


def genRand(length):
    rand = np.random.randn(length, LEN_VEC)
    return rand

if __name__ == '__main__':
    args = sys.argv[1:]
    if not args:
        print "./cmd.py length tofile"
        sys.exit(-1)
    length, tofile = args
    length = int(length)

    rand = genRand(length)
    with open(tofile, 'w') as f:
        lines = []
        for i in xrange(length):
            arr = rand[i]
            line = " ".join([str(i) for i in arr])
            lines.append(line)
        f.write("\n".join(lines))

