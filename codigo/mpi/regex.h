#ifndef REGEX_H
#define REGEX_H
#include <regex.h>

#define SIZE_MSG 125

int er_error(int i, regex_t reg);
void regex_match_replace(char *regex, char* info, int card_number);
#endif
