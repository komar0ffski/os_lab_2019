#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                    {"array_size", required_argument, 0, 0},
                                    {"pnum", required_argument, 0, 0},
                                    {"by_files", no_argument, 0, 'f'},
                                    {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // проверка, что seed положительное число
            if (seed <= 0) {
              printf("seed must be a positive number\n");
              return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            // проверка, что array_size положительное число
            if (array_size <= 0) {
              printf("array_size must be a positive number\n");
              return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            // проверка, что pnum положительное число
            if (pnum <= 0) {
              printf("pnum must be a positive number\n");
              return 1;
            }
            break;
          case 3:
            with_files = true;
            break;

          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
          argv[0]);
    return 1;
  }

  // выделение памяти под массив
  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  
  // создание pipe для обмена данными между процессами
  int pipes[2];
  if (!with_files && pipe(pipes) < 0) {
    printf("Pipe creation failed!\n");
    return 1;
  }

  int active_child_processes = 0;
  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  // вычисление размера части массива для каждого процесса
  int chunk_size = array_size / pnum;
  int remainder = array_size % pnum;

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // успешное создание процесса
      active_child_processes += 1;
      if (child_pid == 0) {
        // код, выполняемый в дочернем процессе
        int start = i * chunk_size;
        int end = start + chunk_size;
        // последний процесс обрабатывает остаток
        if (i == pnum - 1) {
          end += remainder;
        }

        // поиск минимума и максимума в своей части массива
        struct MinMax local_min_max = GetMinMax(array, start, end);

        if (with_files) {
          // запись результатов в файл
          char filename[20];
          sprintf(filename, "minmax_%d.txt", i);
          FILE *fp = fopen(filename, "w");
          if (fp == NULL) {
            printf("Failed to create file %s\n", filename);
            exit(1);
          }
          fprintf(fp, "%d %d", local_min_max.min, local_min_max.max);
          fclose(fp);
        } else {
          // отправка результатов через pipe
          write(pipes[1], &local_min_max, sizeof(struct MinMax));
        }
        exit(0);
      }
    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  // ожидание завершения всех дочерних процессов
  while (active_child_processes > 0) {
    wait(NULL);
    active_child_processes -= 1;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  // сбор результатов от всех процессов
  for (int i = 0; i < pnum; i++) {
    struct MinMax local_min_max;

    if (with_files) {
      // чтение результатов из файла
      char filename[20];
      sprintf(filename, "minmax_%d.txt", i);
      FILE *fp = fopen(filename, "r");
      if (fp == NULL) {
        printf("Failed to open file %s\n", filename);
        continue;
      }
      fscanf(fp, "%d %d", &local_min_max.min, &local_min_max.max);
      fclose(fp);
      remove(filename); // удаление временного файла
    } else {
      // чтение результатов из pipe
      read(pipes[0], &local_min_max, sizeof(struct MinMax));
    }

    // обновление общих минимума и максимума
    if (local_min_max.min < min_max.min) min_max.min = local_min_max.min;
    if (local_min_max.max > min_max.max) min_max.max = local_min_max.max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  // вычисление времени выполнения
  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  // освобождение ресурсов
  free(array);
  if (!with_files) {
    close(pipes[0]);
    close(pipes[1]);
  }

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}