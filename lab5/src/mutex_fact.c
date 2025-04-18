#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// проверяет, равны ли две строки
int streq(const char* a, const char* b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return (*a == '\0' && *b == '\0');
}

// глобальные переменные
unsigned long long result = 1;  // результат вычисления факториала
int k;                         // число для вычисления факториала
int mod;                       // модуль
int thread_count;              // количество потоков
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // мьютекс для синхронизации

// структура для передачи данных в поток
typedef struct {
    int thread_id;
} thread_data;

// функция, выполняемая каждым потоком
void* compute_factorial(void* arg) {
    thread_data* data = (thread_data*)arg;
    int tid = data->thread_id;
    
    // вычисляем диапазон чисел для текущего потока
    int start = tid * (k / thread_count) + 1;
    int end = (tid == thread_count - 1) ? k : (tid + 1) * (k / thread_count);
    
    unsigned long long partial_result = 1;
    
    // вычисляем частичный факториал в своем диапазоне
    for (int i = start; i <= end; i++) {
        partial_result = (partial_result * i) % mod;
    }
    
    // блокируем мьютекс для обновления общего результата
    pthread_mutex_lock(&mutex);
    result = (result * partial_result) % mod;
    pthread_mutex_unlock(&mutex);
    
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    // обработка аргументов командной строки
    if (argc != 7) {
        printf("использование: %s -k <число> -pnum <потоки> -mod <модуль>\n", argv[0]);
        return 1;
    }
    
    for (int i = 1; i < argc; i++) {
        if (streq(argv[i], "-k")) {
            k = atoi(argv[++i]);
        } else if (streq(argv[i], "-pnum")) {
            thread_count = atoi(argv[++i]);
        } else if (streq(argv[i], "-mod")) {
            mod = atoi(argv[++i]);
        }
    }
    
    if (k < 0 || thread_count <= 0 || mod <= 0) {
        printf("неверные параметры: k, pnum и mod должны быть положительными числами\n");
        return 1;
    }
    
    // обработка особых случаев
    if (k == 0 || k == 1) {
        printf("%d! mod %d = 1\n", k, mod);
        return 0;
    }
    
    // создаем потоки
    pthread_t threads[thread_count];
    thread_data thread_args[thread_count];
    
    for (int i = 0; i < thread_count; i++) {
        thread_args[i].thread_id = i;
        int rc = pthread_create(&threads[i], NULL, compute_factorial, (void*)&thread_args[i]);
        if (rc) {
            printf("ошибка при создании потока %d\n", i);
            exit(-1);
        }
    }
    
    // ожидаем завершения всех потоков
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // выводим результат
    printf("%d! mod %d = %llu\n", k, mod, result);
    
    // освобождаем ресурсы мьютекса
    pthread_mutex_destroy(&mutex);
    
    return 0;
}