#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#define BUFFER 0x1000

/* Table for perfs */
int bits_un[] = {
  0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

uint64_t n_bytes = 0;
uint64_t n_bits = 0;

uint64_t bits[2];
uint64_t bytes[256];

int get_max_byte(void) {
  int i;
  int max = 0;

  for(i = 0; i <= 0xFF; i++) {
    if(bytes[max] < bytes[i])
      max = i;
  }
  return max;
}

int get_min_byte(void) {
  int i;
  int min = 0;

  for(i = 0; i <= 0xFF; i++) {
    if(bytes[min] > bytes[i])
      min = i;
  }
  return min;
}

void print_report(void) {
  int i = 1;
  int max, min;
  uint64_t differencial;

  max = get_max_byte();
  min = get_min_byte();

  differencial = bytes[max] - bytes[min];

  while(bytes[max] != 0) {
    printf("%02x=%-10" PRIu64 " %-5.2f || ", max, bytes[max], ((double)bytes[max]/n_bytes)*100.0);

    if(i % 4 == 0)
       printf("\n");

     i++;
     bytes[max] = 0;
     max = get_max_byte();
  }

  printf("\n");
  printf("Differencial : %" PRIu64 "\n", differencial);

  if(n_bits) {
    printf("\n\n");
    printf("Bit 0 = %12" PRIu64 " %.2f\n", bits[0], ((double)bits[0]/n_bits)*100.0);
    printf("Bit 1 = %12" PRIu64 " %.2f\n", bits[1], ((double)bits[1]/n_bits)*100.0);
    if(bits[0] > bits[1]) {
      differencial = bits[0] - bits[1];
    } else {
      differencial = bits[1] - bits[0];
    }
    printf("Differencial : %" PRIu64 "\n", differencial);
    printf("\n");
  }

  printf("Total bytes processed : %" PRIu64 "\n", n_bytes);
}

void process_buffer(uint8_t *buffer, size_t size) {
  size_t i;

  n_bytes += size;
  n_bits += size*8;

  for(i = 0; i < size; i++) {
    bytes[buffer[i]]++;
    bits[1] += bits_un[buffer[i]];
    bits[0] += 8 - bits_un[buffer[i]];
  }
}

int main(void) {
  uint8_t buffer[BUFFER];
  size_t len;

  len = BUFFER;

  while(len == BUFFER) {
    len = fread(buffer, 1, BUFFER, stdin);
    process_buffer(buffer, len);
  }

  print_report();

  return EXIT_SUCCESS;
}
