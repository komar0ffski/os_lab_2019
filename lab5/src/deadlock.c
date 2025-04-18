#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// мьютексы для демонстрации взаимной блокировки
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;

// функция для первого потока
void* thread1_func(void* arg) {
    // захватываем первый мьютекс
    pthread_mutex_lock(&mutex1);
    printf("поток 1: захватил mutex1\n");

    // задержка для создания условий deadlock
    sleep(1);

    // попытка захватить второй мьютекс (приведет к deadlock)
    pthread_mutex_lock(&mutex2);
    printf("поток 1: захватил mutex2\n");

    // освобождаем мьютексы (эта часть не выполнится при deadlock)
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
    
    return NULL;
}

// функция для второго потока
void* thread2_func(void* arg) {
    // захватываем второй мьютекс
    pthread_mutex_lock(&mutex2);
    printf("поток 2: захватил mutex2\n");

    // задержка для создания условий deadlock
    sleep(1);

    // попытка захватить первый мьютекс (приведет к deadlock)
    pthread_mutex_lock(&mutex1);
    printf("поток 2: захватил mutex1\n");

    // освобождаем мьютексы (эта часть не выполнится при deadlock)
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
    
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    // инициализация мьютексов
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);

    // создание потоков
    pthread_create(&thread1, NULL, thread1_func, NULL);
    pthread_create(&thread2, NULL, thread2_func, NULL);

    // ожидание завершения потоков
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // уничтожение мьютексов
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);

    return 0;
}