#include <stdio.h>
#include <stdlib.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  // проверка количества аргументов командной строки
  if (argc != 3) {
    printf("Usage: %s seed arraysize\n", argv[0]);
    return 1;
  }

  // преобразование первого аргумента (seed) в целое число
  int seed = atoi(argv[1]);
  // проверка, что seed является положительным числом
  if (seed <= 0) {
    printf("seed is a positive number\n");
    return 1;
  }

  // преобразование второго аргумента (array_size) в целое число
  int array_size = atoi(argv[2]);
  // проверка, что array_size является положительным числом
  if (array_size <= 0) {
    printf("array_size is a positive number\n");
    return 1;
  }

  // выделение памяти
  int *array = malloc(array_size * sizeof(int));
  // заполнение массива случайными числами с использованием seed
  GenerateArray(array, array_size, seed);
  
  // поиск минимального и максимального элементов в массиве
  struct MinMax min_max = GetMinMax(array, 0, array_size);
  
  // освобождение памяти
  free(array);

  // вывод
  printf("min: %d\n", min_max.min);
  printf("max: %d\n", min_max.max);

  return 0;
}