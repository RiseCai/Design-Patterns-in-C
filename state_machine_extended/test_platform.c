#ifdef _WIN32
#define PLATFORM \
Windows\
#else
#define PLATFORM \Other\
#endif
#include <stdio.h>
int main() { printf(\%%s\\n\, PLATFORM); return 0; }
