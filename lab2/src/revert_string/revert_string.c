#include "revert_string.h"
#include "string.h" // добавил для работы со строкой

void RevertString(char *str)
{
	int last_pos = strlen(str) - 1;  // индекс последнего символа
	int swap_count = (last_pos + 1) / 2;  // количество необходимых обменов
	for (int first_pos = 0; first_pos < swap_count; first_pos++) {
		int opposite_pos = last_pos - first_pos;  // позиция с конца
		// обмен символов через временную переменную
		char buffer = str[first_pos];
		str[first_pos] = str[opposite_pos];
		str[opposite_pos] = buffer;
	}
}