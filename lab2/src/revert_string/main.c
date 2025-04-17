#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "revert_string.h"

int main(int argc, char *argv[]) // argc - количество аргументов argv - массив строк-аргументов 
{
	if (argc != 2) // проверка количества аргументов, если не 2 выводим сообщение об ошибке
	{
		printf("Usage: %s string_to_revert\n", argv[0]);
		return -1;
	}

	// выделяем память для хранения перевернутой строки
    // strlen(argv[1]) - получаем длину входной строки
    // +1 - дополнительный байт для терминатора '\0'
    // sizeof(char) - размер одного символа
	char *reverted_str = malloc(sizeof(char) * (strlen(argv[1]) + 1));
	strcpy(reverted_str, argv[1]); // копируем входную строку в выделенную память

	RevertString(reverted_str); // вызываем переворачивающую функцию

	printf("Reverted: %s\n", reverted_str); // вывод
	free(reverted_str); // освобождаем память
	return 0;
}

