#include "swap.h"

void Swap(char *left, char *right)
{
	char swapper = *left;
	*left = *right;
	*right = swapper;
}
