#ifndef KRAKENTRAP_COLOR_H
#define KRAKENTRAP_COLOR_H

#define KT_RESET   "\033[0m"
#define KT_BOLD    "\033[1m"

#define KT_RED     "\033[31m"
#define KT_GREEN   "\033[32m"
#define KT_YELLOW  "\033[33m"
#define KT_BLUE    "\033[34m"

#define KT_ERR     "[" KT_RED "-" KT_RESET "] "
#define KT_GOOD    "[" KT_GREEN "+" KT_RESET "] "
#define KT_INFO    "[" KT_BLUE "*" KT_RESET "] "
#define KT_WARN    "[" KT_YELLOW "!" KT_RESET "] "

#endif
