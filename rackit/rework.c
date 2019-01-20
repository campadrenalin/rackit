static const int BUFFER_SIZE = 2048;
typedef double Sample;
typedef Sample Buffer[2048];

void Buffer_fill(Buffer *buf, double value) {
    for (int i=0; i<BUFFER_SIZE; i++)
        (*buf)[i] = value;
}
