#pragma once

size_t
line_number(const char *f, size_t len);
size_t
count_comas(const char *line);
void
get_nth_coma(const char *line, size_t n, char **dest);
int
dostuff(const char *path,
        double **train_inputs,
        char **train_outputs,
        size_t *inputNb,
        size_t *trainingSetsNb);
void
print_char_array(char *a, size_t lines, size_t columns);
void
print_double_array(double *a, size_t lines, size_t columns);
int
main();