#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>

#define DOCUMENT_PATH "~/Documents"
#define WCTMB(x, y, z) wcstombs(x, y, z)
#define NOTIFY(x, y) system("notify-send [%s] %s", x, y)
