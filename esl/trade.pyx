def dot(a, b):
    out = 0
    for i in range(len(a)):
        out += a[i] * b[i]
    return out
