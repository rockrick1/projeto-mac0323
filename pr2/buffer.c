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
    // B->buffer_size = ???
    B->data = (char*) malloc(B->buffer_size * (B->member_size * sizeof(char)));
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
    B->data = NULL; // ?????
}

/*
  Return a valid pointer to the first free position of the
  buffer. This means that, if the space allocated is not enough, then
  the buffer size is increased and the contents are copied.
*/
void *buffer_push_back(Buffer *B) {
    int i, size = B->member_size * B->buffer_size;
    for (i = 0; i < size; i++)
        if (B->data[i] == NULL)
            break;
    // excederá o limite, realoca o buffer
    if (i == size) {
        B->data = realloc(B->data, 2*sizeof(B->data)); // might work idk
        // idk either, a função ser void nao ajuda tambem
        return (char*) B->data[size];
    }
    return (char*) B->data[i];
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
    char c = getc(input);
    while (c != '\n') {
        buffer_push_char(B, c);
        c = getc(input);
    }
}
