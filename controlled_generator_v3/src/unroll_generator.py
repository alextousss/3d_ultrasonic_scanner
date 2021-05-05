import sys

for i in range(int(sys.argv[1])):
    for j in range(int(sys.argv[2])):
        print("digitalWriteFast({j}, bool(ptr[{i}]));".format(j=j, i=i))
print("ptr += " + sys.argv[1] + ";")
