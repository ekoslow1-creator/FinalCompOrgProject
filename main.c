#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int my_scanf(const char *format, ...);
void test_char_multiple();
void test_int_multiple();
void test_string_multiple();
void test_float_multiple();
void test_hex_multiple();

int main(void) {
    // Redirect standard input to my own text file
    freopen("lzbop.txt", "r", stdin);
    test_char_multiple();
    test_int_multiple();
    test_string_multiple();
    test_float_multiple();
    test_hex_multiple();
}

// Core helper functions
int read_int(FILE *stream, int *value);
int read_char(FILE *stream, char *c);
int read_string(FILE *stream, char *str);
int read_double(FILE *stream, double *value);
int read_hex(FILE *stream, unsigned int *value);

// Ancillary helper functions
void skip_whitespace(FILE *stream);
int is_whitespace(int c);
int is_digit(int c);
int is_hex_digit(int c);
int hex_to_int(int c);

int my_scanf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    int count = 0; // Number of successful assignments
    int i = 0; // Index into format string

    while (format[i] != '\0') {
        if (format[i] == '%') {
            i++; // Move past '%'

            switch (format[i]) {
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
                case 'f': {
                    double *ptr = va_arg(args, double*);
                    if (read_double(stdin, ptr)) {
                        count++;
                    } else {
                        va_end(args);
                        return count;
                    }
                    break;
                }
                case 'x': {
                    unsigned int *ptr = va_arg(args, unsigned int*);
                    if (read_hex(stdin, ptr)) {
                        count++;
                    } else {
                        va_end(args);
                        return count;
                    }
                    break;
                }
                case '%': {
                    // Match literal '%'
                    int c = getc(stdin);

                    // Skip leading whitespace (like other format specifiers do)
                    while (is_whitespace(c)) {
                        c = getc(stdin);
                    }

                    // Check if the character is '%'
                    if (c == '%') {
                        // Successfully matched, continue.
                        // Don't increment count - we didn't assign to a variable
                    } else {
                        // Mismatch - matching failed
                        va_end(args);
                        return count;
                    }
                    break;
                }
                default: {
                    // Unknown format specifier - stop processing
                    // This handles cases like %p, %n, %o, etc. that we haven't implemented
                    va_end(args);
                    return count;
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

int read_string(FILE *stream, char *str) {
    int c = getc(stream);
    int index = 0;

    // Step 1: Skip leading whitespace
    while (is_whitespace(c)) {
        c = getc(stream);
    }

    // Step 2: Check for EOF before reading anything
    if (c == EOF) {
        return 0; // Failure - no string to read
    }

    // Step 3: Read characters until whitespace or EOF
    while (c != EOF && !is_whitespace(c)) {
        str[index] = (char)c;
        index++;
        c = getc(stream);
    }

    // Step 4: Null-terminate the string
    str[index] = '\0';

    // Step 5: Put back the whitespace/EOF character
    if (c != EOF) {
        ungetc(c, stream);
    }

    // Step 6: Check if we read at least one character
    if (index == 0) {
        return 0; // Failure - no characters read
    }

    return 1; // Success
}

int read_double(FILE *stream, double *value) {
    int c = getc(stream);
    int sign = 1;
    double result = 0.0;
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

    // Step 3: Read integer part
    while (is_digit(c)) {
        digit_found = 1;
        result = result * 10.0 + (c - '0');
        c = getc(stream);
    }

    // Step 4: Check for decimal point
    if (c == '.') {
        c = getc(stream);
        double fraction = 0.1;

        // Read fractional part
        while (is_digit(c)) {
            digit_found = 1;
            result = result + (c - '0') * fraction;
            fraction = fraction * 0.1;
            c = getc(stream);
        }
    }

    // Step 5: Check if we found at least one digit
    if (!digit_found) {
        return 0; // Failure - no valid float
    }

    // Step 6: Check for scientific notation (e or E)
    if (c == 'e' || c == 'E') {
        c = getc(stream);
        int exp_sign = 1;
        int exponent = 0;
        int exp_digit_found = 0;

        // Check for optional sign in exponent
        if (c == '-') {
            exp_sign = -1;
            c = getc(stream);
        } else if (c == '+') {
            c = getc(stream);
        }

        // Read exponent digits
        while (is_digit(c)) {
            exp_digit_found = 1;
            exponent = exponent * 10 + (c - '0');
            c = getc(stream);
        }

        // If we had 'e' but no valid exponent, that's an error
        if (!exp_digit_found) {
            return 0;
        }

        // Apply exponent: result = result * 10^(exp_sign * exponent)
        exponent = exponent * exp_sign;

        // Calculate 10^exponent
        double multiplier = 1.0;
        if (exponent > 0) {
            for (int i = 0; i < exponent; i++) {
                multiplier *= 10.0;
            }
        } else if (exponent < 0) {
            for (int i = 0; i < -exponent; i++) {
                multiplier /= 10.0;
            }
        }

        result = result * multiplier;
    }

    if (c != EOF) {
        ungetc(c, stream); // We consumed one too many characters
    }

    // Apply sign and store result
    *value = result * sign;

    return 1; // Success
}

int read_hex(FILE *stream, unsigned int *value) {
    int c = getc(stream);
    unsigned int result = 0;
    int digit_found = 0;

    // Step 1: Skip leading whitespace
    while (is_whitespace(c)) {
        c = getc(stream);
    }

    // Step 2: Optional "0x" or "0X" prefix
    if (c == '0') {
        int next = getc(stream);
        if (next == 'x' || next == 'X') {
            // Valid 0x prefix, move to next character
            c = getc(stream);
        } else {
            // Just a '0', not followed by 'x'
            // The '0' is a valid hex digit
            digit_found = 1;
            result = 0;
            c = next; // Process the character after '0'
        }
    }

    // Step 3: Read hex digits
    while (is_hex_digit(c)) {
        digit_found = 1;
        result = result * 16 + hex_to_int(c);
        c = getc(stream);
    }

    // Step 4: Check if we found at least one digit
    if (!digit_found) {
        return 0; // Failure - no valid hex number
    }

    // Step 5: Put back the non-hex character
    if (c != EOF) {
        ungetc(c, stream);
    }

    *value = result;
    return 1; // Success
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

int is_hex_digit(int c) {
    return (c >= '0' && c <= '9') || \
           (c >= 'a' && c <= 'f') || \
           (c >= 'A' && c <= 'F');
}

int hex_to_int(int c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10; // 'a' = 10, 'b' = 11, etc.
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10; // 'A' = 10, 'B' = 11, etc.
    }
    return 0;
}

// BELOW ARE MY TEST FUNCTIONS //

void test_char_multiple() {
    printf("Testing multiple characters\n");
    int test_cases = 62;
    for (int i = 0; i < test_cases; i++) {
        char c1;
        char c2;
        scanf("%c", &c1);
        ungetc(c1, stdin); // reset so that my_scanf() reads same character
        my_scanf("%c", &c2);
        printf("scanf = %c my_scanf = %c\n", c1, c2);
        if (c1 != c2) {
            printf("FAIL: scanf = %c, my_scanf = %c\n", c1, c2);
        }
    }
    printf("\n");
}

void test_int_multiple() {
    printf("Testing multiple integers\n");
    int test_cases = 18;
    for (int i = 0; i < test_cases; i++) {
        // Save file position
        fpos_t pos;
        fgetpos(stdin, &pos);

        // Test scanf
        int d1;
        int ret1 = scanf("%d", &d1);

        // Restore position
        fsetpos(stdin, &pos);

        // Test my_scanf
        int d2;
        int ret2 = my_scanf("%d", &d2);

        // Compare & raise potential error
        printf("scanf = %d my_scanf = %d\n", d1, d2);
        if (d1 == d2 && !is_digit(d1)) {
            printf("OK\n"); // Acknowledge noninteger input
        } else if (ret1 != ret2 || d1 != d2) {
            printf("FAIL (scanf = %d, my_scanf = %d)\n", d1, d2);
        }
    }
    printf("\n");
}

void test_string_multiple() {
    printf("Testing multiple strings\n");
    // This is my third test function, so I expect scanf and my_scanf to read from third line of lzbop.txt
    char *expected[] = {"Let's", "test", "string", "functionality."};
    int num_tests = 4;

    for (int i = 0; i < num_tests; i++) {
        fpos_t pos;
        fgetpos(stdin, &pos);

        char s1[100];
        int ret1 = scanf("%s", s1);

        fsetpos(stdin, &pos);

        char s2[100];
        int ret2 = my_scanf("%s", s2);

        if (ret1 == ret2 && strcmp(s1, s2) == 0 && strcmp(s1, expected[i]) == 0) {
            printf("PASS\n");
        } else {
            printf("FAIL (expected='%s', scanf='%s', my_scanf='%s'\n", expected[i], s1, s2);
        }
    }
    printf("\n");
}

void test_float_multiple() {
    printf("Testing multiple floats\n");

    int num_tests = 5;

    for (int i = 0; i < num_tests; i++) {
        fpos_t pos;
        fgetpos(stdin, &pos);

        double f1;
        int ret1 = scanf("%lf", &f1);

        fsetpos(stdin, &pos);

        double f2;
        int ret2 = my_scanf("%f", &f2);

        if (ret1 == ret2) {
            // Compare floats with small tolerance for rounding errors
            double diff = f1 - f2;
            if (diff < 0) diff = -diff; // Convert difference to absolute value

            if (diff < 0.0001) {
                printf("PASS (%.6f)\n", f1);
            } else {
                printf("FAIL (scanf=%.6f, my_scanf=%.6f, diff=%.6f)\n", f1, f2, diff);
            }
        } else {
            printf("FAIL (scanf returned %d, my_scanf returned %d)\n", ret1, ret2);
        }
    }
    printf("\n");
}

void test_hex_multiple() {
    printf("Testing multiple hex values\n");

    int num_tests = 6;

    for (int i = 0; i < num_tests; i++) {
        fpos_t pos;
        fgetpos(stdin, &pos);

        unsigned int h1;
        int ret1 = scanf("%x", &h1);

        fsetpos(stdin, &pos);

        unsigned int h2;
        int ret2 = my_scanf("%x", &h2);

        if (ret1 == ret2 && h1 == h2) {
            printf("PASS (%u or 0x%x)\n", h1, h1);
        } else if (ret1 == ret2 && ret1 == 0) {
            printf("PASS (both failed to read)\n");
        } else {
            printf("FAIL (scanf=%u/0x%x (ret=%d), my_scanf=%u/0x%x (ret=%d))\n", h1, h1, ret1, h2, h2, ret2);
        }
    }
    printf("\n");
}