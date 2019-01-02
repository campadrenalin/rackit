import sys

print '''
#include <cstdio>

int main(int argc, char *argv[]) {
'''

for line in sys.stdin:
    print '    printf("'+line.strip()+'\\n");'

print '''
    return 0;
}
'''
