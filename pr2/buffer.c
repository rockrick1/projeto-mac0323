#include <ctype.h>
#include <stdio.h>
#include "buffer.h"


/*
  Create and return a new buffer that holds member elements of size
  member_size.
*/
Buffer *buffer_create(size_t member_size) {
    Buffer *B = (Buffer*) malloc(sizeof(Buffer));
    B->member_size = member_size;
    B->buffer_size = 64;
    B->data = (char*) malloc(B->buffer_size * B->member_size);
    buffer_reset(B);
    return B;
}

/*
  Destroy a buffer.
*/
void buffer_destroy(Buffer *B) {
    free(B->data);
    free(B);
}

/*
  Reset buffer, eliminating contents.
*/
void buffer_reset(Buffer *B) {
    char *data = (char*)B->data;
    for (int i = 0; i < B->buffer_size; i++) {
        data[i] = '\0';
    }
}

/*
  Return a valid pointer to the first free position of the
  buffer. This means that, if the space allocated is not enough, then
  the buffer size is increased and the contents are copied.
*/
void *buffer_push_back(Buffer *B) {
    void *pos;
    char *data = (char*)B->data;
    int i, size = B->buffer_size;
    // procura a primeira pos livre
    for (i = 0; i < size; i++) {
        if (data[i] == '\0')
            break;
    }
    // excederá o limite, realoca o buffer
    if (i == size) {
        B->buffer_size *= 2;
        B->data = (char*)realloc(B->data, B->buffer_size*B->member_size);
        data = (char*)B->data;
        for (int j = size; j < B->buffer_size; j++)
            data[j] = '\0';
        pos = &data[size];
        return pos;
    }
    pos = &data[i];
    return pos;
}

/*
  Read a line (i.e., reads up to a newline '\n' character or the
  end-of-file) from the input file and places it into the given
  buffer, including the newline character if it is present. The buffer
  is resetted before the line is read.
  Returns the number of characters read; in particular, returns ZERO
  if end-of-file is reached before any characters are read.
*/
int read_line(FILE *input, Buffer *B) {
    int n = 0;
    buffer_reset(B);

    char c = getc(input);
    buffer_push_char(B, c);
    n++;
    while (c != '\n' && c != EOF) {
        c = getc(input);
        if (c != EOF) {
            buffer_push_char(B, c);
            n++;
        }
    }
    if (n == 1 && c == EOF) return 0;
    return n;
}
