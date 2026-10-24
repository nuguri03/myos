#ifndef SHELL_PARSER_H
#define SHELL_PARSER_H

#include "types.h"

ssize_t shell_parse(char *line, char** argv, size_t argv_capacity);

#endif