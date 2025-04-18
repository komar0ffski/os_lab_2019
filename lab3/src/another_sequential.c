#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void run_child(char *argv[]) {
    // аргументы для запуска дочерней программы
    char *args[] = {
        "./sequential_min_max",  // имя программы
        argv[1],                // первый аргумент (seed)
        argv[2],                // второй аргумент (array_size)
        NULL                    // обязательное завершение массива
    };

    // выводим информацию о запуске
    printf("запускаем дочернюю программу: ");
    for (int i = 0; args[i] != NULL; i++) {
        printf("%s ", args[i]);
    }
    printf("\n");

    // заменяем текущий процесс новой программой
    execv("./sequential_min_max", args);
    
    // если сюда попали, значит execv не сработал
    perror("ошибка при запуске программы");
    exit(1);
}

void wait_for_child(pid_t child_pid) {
    int status;
    printf("ожидаем завершения дочернего процесса (PID: %d)\n", child_pid);
    
    // ждем завершения конкретного процесса
    waitpid(child_pid, &status, 0);
    
    // проверяем как завершился процесс
    if (WIFEXITED(status)) {
        printf("дочерний процесс завершился с кодом: %d\n", WEXITSTATUS(status));
    } else {
        printf("дочерний процесс завершился аномально\n");
    }
}

int main(int argc, char *argv[]) {
    // проверяем количество аргументов
    if (argc != 3) {
        fprintf(stderr, "использование: %s <seed> <array_size>\n", argv[0]);
        return 1;
    }

    // создаем новый процесс
    pid_t child_pid = fork();
    
    if (child_pid < 0) {
        perror("ошибка при создании процесса");
        return 1;
    }

    if (child_pid == 0) {
        // код для дочернего процесса
        run_child(argv);
    } else {
        // код для родительского процесса
        wait_for_child(child_pid);
    }

    return 0;
}