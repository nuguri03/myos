#ifndef SHELL_BUILTIN_H
#define SHELL_BUILTIN_H

#include "types.h"

// 아는 명령어를 처리했으면 true, 모르는 명령어면 false
bool shell_execute_builtin(size_t argc, char** argv);

#endif