#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>
#include <time.h>
#include <sys/time.h>

struct SumArgs {
  int *array;
  int begin;
  int end;
};

// вычисляет сумму элементов массива
int Sum(const struct SumArgs *args) {
  int sum = 0;
  for (int i = args->begin; i < args->end; i++) {
    sum += args->array[i];
  }
  return sum;
}

// функция, выполняемая в потоке
void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char *argv[]) {
  uint32_t threads_num = 0;    // количество потоков
  uint32_t array_size = 0;     // размер массива
  uint32_t seed = 0;           // зерно для генератора чисел

  // разбор аргументов командной строки
  while (1) {
    static struct option long_options[] = {
      {"threads_num", required_argument, 0, 't'},
      {"array_size", required_argument, 0, 'a'},
      {"seed", required_argument, 0, 's'},
      {0, 0, 0, 0}
    };

    int option_index = 0;
    int c = getopt_long(argc, argv, "t:a:s:", long_options, &option_index);
    if (c == -1)
      break;

    switch (c) {
      case 't':
        threads_num = atoi(optarg);
        break;
      case 'a':
        array_size = atoi(optarg);
        break;
      case 's':
        seed = atoi(optarg);
        break;
      default:
        printf("использование: %s --threads_num <число> --array_size <размер> --seed <число>\n", argv[0]);
        return 1;
    }
  }

  // проверка введенных параметров
  if (threads_num == 0 || array_size == 0 || seed == 0) {
    printf("использование: %s --threads_num <число> --array_size <размер> --seed <число>\n", argv[0]);
    return 1;
  }

  // создание и заполнение массива
  int *array = malloc(sizeof(int) * array_size);
  if (array == NULL) {
    printf("ошибка: не удалось выделить память для массива\n");
    return 1;
  }
  
  srand(seed);
  for (uint32_t i = 0; i < array_size; i++) {
    array[i] = rand() % 100; // числа от 0 до 99
  }

  // подготовка данных для потоков
  pthread_t *threads = malloc(sizeof(pthread_t) * threads_num);
  struct SumArgs *args = malloc(sizeof(struct SumArgs) * threads_num);

  if (threads == NULL || args == NULL) {
    printf("ошибка: не удалось выделить память для потоков или аргументов\n");
    free(array);
    return 1;
  }

  struct timeval start_time, end_time;
  gettimeofday(&start_time, NULL); // начинаем замер времени

  // распределение работы между потоками
  uint32_t chunk_size = array_size / threads_num;
  for (uint32_t i = 0; i < threads_num; i++) {
    args[i].array = array;
    args[i].begin = i * chunk_size;
    if (i == threads_num - 1) {
      args[i].end = array_size;  // последний поток берет остаток
    } else {
      args[i].end = (i + 1) * chunk_size;
    }
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
      printf("ошибка: не удалось создать поток!\n");
      free(array);
      free(threads);
      free(args);
      return 1;
    }
  }

  // сбор результатов
  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    void *sum_ptr;
    pthread_join(threads[i], &sum_ptr);
    int sum = (int)(size_t)sum_ptr;
    total_sum += sum;
  }

  gettimeofday(&end_time, NULL); // заканчиваем замер времени

  // вычисление времени выполнения в миллисекундах
  double elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (end_time.tv_usec - start_time.tv_usec) / 1000.0;

  // освобождение ресурсов
  free(array);
  free(threads);
  free(args);

  printf("общая сумма: %d\n", total_sum);
  printf("время вычисления: %.2f мс\n", elapsed_time);
  
  return 0;
}