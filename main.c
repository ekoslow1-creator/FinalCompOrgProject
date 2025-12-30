#include <stdarg.h>
#include <stdio.h>

int my_scanf(const char *format, ...);

int main(void) {
    char elza;
    int bop;
    char hip;
    my_scanf("%c %d %c", &elza, &bop, &hip);
    printf("%c %d %c", elza, bop, hip);
}

void skip_whitespace(FILE *stream);
int read_int(FILE *stream, int *value);
int read_uint(FILE *stream, unsigned int *value);
int read_char(FILE *stream, char *c);
int read_string(FILE *stream, char *str);
int is_whitespace(int c);
int is_digit(int c);

int my_scanf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    int count = 0; // Number of successful assignments
    int i = 0; // Index into format string

    while (format[i] != '\0') {
        if (format[i] == '%') {
            i++; // Move past '%'

            switch (format[i]) {
                case 'd': {
                    int *ptr = va_arg(args, int*);
                    if (read_int(stdin, ptr)) {
                        count++;
                    } else {
                        va_end(args);
                        return count;
                    }
                    break;
                }
                case 'c': {
                    char *ptr = va_arg(args, char*);
                    if (read_char(stdin, ptr)) {
                        count++;
                    } else {
                        va_end(args);
                        return count;
                    }
                    break;
                }
                case 's': {
                    char *ptr = va_arg(args, char*);
                    if (read_string(stdin, ptr)) {
                        count++;
                    } else {
                        va_end(args);
                        return count;
                    }
                    break;
                }
                case '%':
                    // Match literal '%'
                    // Implementation needed
                    break;
                default: {
                    break;
                }
            }
            i++;
        } else if (is_whitespace(format[i])) {
            skip_whitespace(stdin);
            i++;
        } else {
            // Match literal character
            int c = getchar();
            if (c != format[i]) {
                // Mismatch - return early
                va_end(args);
                return count;
            }
            i++;
        }
    }

    va_end(args);
    return count;
}

int read_char(FILE *stream, char *c) {
    const int ch = getc(stream);
    if (ch == EOF) {
        return 0; // Failure - no character available
    }
    *c = (char)ch; // Store the characters at the pointer location
    return 1; // Success
}

// MUST CREATE ALL FUNCTIONS BELOW

int read_int(FILE *stream, int *value) {
    int c = getc(stream);
    int sign = 1;
    int result = 0;
    int digit_found = 0;

    // Step 1: Skip leading whitespace
    while (is_whitespace(c)) {
        c = getc(stream);
    }

    // Step 2: Check for optional sign
    if (c == '-') {
        sign = -1;
        c = getc(stream);
    } else if (c == '+') {
        c = getc(stream);
    }

    // Step 3: Read digits
    while (is_digit(c)) {
        digit_found = 1;
        result = result * 10 + (c - '0'); // Build the number
        c = getc(stream);
    }

    // Step 4: Check if we found at least one digit
    if (!digit_found) {
        return 0; // Failure - no valid integer inputted
    }

    // Apply positive or negative sign and store result
    *value = result * sign;

    ungetc(c, stream); // We consumed one too many characters
    return 1; // Success
}

int read_string(FILE *stream, char *s) {
    return 1;
}

int is_whitespace(int c) {
    return (c == ' ' || c == '\n' || c == '\t' \
        || c == '\r' || c == '\f' || c == '\v');
}

void skip_whitespace(FILE *stream) {
    int c = getc(stream);
    while (is_whitespace(c)) { // If c is still whitespace
        c = getc(stream);  // Keep reading
    }
    ungetc(c, stream); // Put back the first non whitespace character
}

int is_digit(int c) {
    return (c >= '0' && c <= '9');
}
