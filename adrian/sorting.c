#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define DIRECTORY_SIZE 6
#define SERIAL_LENGTH 8
// Forward declaration
int less(byte val1[SERIAL_LENGTH], byte val2[SERIAL_LENGTH]);
void shift(int index, int size, byte directory[DIRECTORY_SIZE][SERIAL_LENGTH]);

void insertion_sort(int size, byte directory[DIRECTORY_SIZE][SERIAL_LENGTH],
                    byte entry[SERIAL_LENGTH]) {
  for (int i = 0; i <= size; i++) {
    if (i == size) {
      memcpy(directory[i], entry, SERIAL_LENGTH);
    } else if (!less(directory[i], entry)) {
      shift(i, size, directory);
      memcpy(directory[i], entry, SERIAL_LENGTH);
      break;
    }
  }
}

void inline shift(int index, int size,
                  byte directory[DIRECTORY_SIZE][SERIAL_LENGTH]) {
  for (int i = size - 1; i >= index; i--) {
    memcpy(directory[i + 1], directory[i], SERIAL_LENGTH);
  }
}

int inline less(byte val1[SERIAL_LENGTH], byte val2[SERIAL_LENGTH]) {
  for (int i = 0; i < SERIAL_LENGTH; i++) {
    if (val1[i] < val2[i]) return 1;
    if (val2[i] < val1[i]) return 0;
  }
  return 0;
}

int main() {
  srand(time(NULL));
  byte directory[DIRECTORY_SIZE][SERIAL_LENGTH];
  for (int i = 0; i < DIRECTORY_SIZE; i++) {
    byte entry[SERIAL_LENGTH];
    for (int j = 0; j < SERIAL_LENGTH; j++) {
      entry[j] = (byte)rand();
    }
    insertion_sort(i, directory, entry);
  }
  for (int i = 0; i < DIRECTORY_SIZE; i++) {
    for (int j = 0; j < SERIAL_LENGTH; j++) {
      printf("%02x ", directory[i][j] & 0xFF);
    }
    printf("\n");
  }
  return 0;
}
