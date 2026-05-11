


#include "../header/utils.h"






char* foo(const char *str1, const char *str2, const char *str3) {
    // Calcule la taille totale de la string
    size_t len1 = str1 ? strlen(str1) : 0;
    size_t len2 = str2 ? strlen(str2) : 0;
    size_t len3 = str3 ? strlen(str3) : 0;
    size_t total_len = len1 + len2 + len3 + 1;

    char *result = (char *)malloc(total_len);
    if (!result) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    if (str1) strcpy(result, str1);
    if (str2) strcat(result, str2);
    if (str3) strcat(result, str3);

    return result;
}



void get_current_datetime(char *datetime_str, size_t max_len) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(datetime_str, max_len, "# %a %b %d %H:%M:%S %Y\n", t);
}