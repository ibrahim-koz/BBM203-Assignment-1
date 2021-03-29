#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <values.h>
#define _GNU_SOURCE

struct Matrix
{
    char *name;
    int **rows;
    int row_size;
    int col_size;
};

struct Row
{
    char **tokens;
    int token_size;
};

void fill_row(struct Row row, struct Matrix *matrix)
{
    (*matrix).row_size++;
    (*matrix).col_size = row.token_size;
    (*matrix).rows = realloc((*matrix).rows, sizeof(int *) * ((*matrix).row_size));
    *((*matrix).rows + (*matrix).row_size - 1) = malloc(sizeof(int) * row.token_size);
    for (int i = 0; i < row.token_size; i++)
        (*matrix).rows[(*matrix).row_size - 1][i] = atoi(row.tokens[i]);
}

struct Row split_line(char *line, char *delimiter)
{
    struct Row row = {malloc(sizeof(char *)), 0};
    char *token;
    /* get the first token */
    token = strtok(line, delimiter);

    /* walk through other tokens and the tokens*/
    while (token != NULL)
    {
        row.token_size++;
        row.tokens = realloc(row.tokens, sizeof(char *) * row.token_size);
        row.tokens[row.token_size - 1] = malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(row.tokens[row.token_size - 1], token);
        token = strtok(NULL, delimiter);
    }
    return row;
}

void free_matrix(struct Matrix *matrix)
{
    for (int i = 0; i < (*matrix).row_size; i++)
        free((*matrix).rows[i]);
    free((*matrix).name);
    free((*matrix).rows);
}
struct Matrix read_file(char *dir_inputs, char *file_name)
{
    //Read the data and parceing it.
    FILE *fp;
    struct Matrix matrix = {malloc(sizeof(char) * (strlen(file_name) - 4 + 1)), malloc(sizeof(int *)), 0, 0};
    char *matrix_name = strdup(file_name);
    matrix_name[strcspn(matrix_name, ".")] = '\0';
    strcpy(matrix.name, matrix_name);
    free(matrix_name);
    char *line = NULL;
    struct Row row;
    size_t len = 0;
    ssize_t read;

    char *path_of_file = malloc(sizeof(char) * (strlen(dir_inputs) + strlen(file_name) + 1));
    snprintf(path_of_file, sizeof(char) * (strlen(dir_inputs) + strlen(file_name) + 1), "%s%s", dir_inputs, file_name);

    fp = fopen(path_of_file, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1)
    {
        line[strcspn(line, "\n")] = '\0';
        row = split_line(line, " ");
        fill_row(row, &matrix);
        for (int i = 0; i < row.token_size; i++)
            free(row.tokens[i]);
        free(row.tokens);
    }
    fclose(fp);
    if (line)
        free(line);
    free(path_of_file);
    return matrix;
}

int find_max(int *matrix, int matrix_size) // tested
{
    int max = INT_MIN;
    for (int i = 0; i < matrix_size; i++)
        if (matrix[i] > max)
            max = matrix[i];
    return max;
}

int find_min(int *matrix, int matrix_size) // tested
{
    int min = INT_MAX;
    for (int i = 0; i < matrix_size; i++)
        if (matrix[i] < min)
            min = matrix[i];
    return min;
}

struct Matrix allocate_matrix(char *name, int row_size, int col_size) // tested
{
    //initilize the matrix
    struct Matrix matrix;

    //allocate memory for attributes
    matrix.name = malloc(sizeof(char) * (strlen(name) + 1));
    matrix.rows = malloc(sizeof(int *) * row_size);
    for (int i = 0; i < row_size; i++)
        matrix.rows[i] = malloc(sizeof(int) * col_size);

    //logins to attribute infos
    strcpy(matrix.name, name);
    matrix.row_size = row_size;
    matrix.col_size = col_size;

    return matrix;
}

struct Matrix create_zeros(char *name, int row_size, int col_size) // tested
{
    struct Matrix matzeros = allocate_matrix(name, row_size, col_size);
    strcpy(matzeros.name, name);
    //fill zeros to the matrix
    for (int i = 0; i < matzeros.row_size; i++)
        for (int j = 0; j < matzeros.col_size; j++)
            matzeros.rows[i][j] = 0;
    return matzeros;
}
struct Matrix vecstack(struct Matrix vector1, struct Matrix vector2, char *direction, char *name)
{
    struct Matrix concatenated;
    if (strcmp(direction, "row") == 0)
    {
        concatenated = allocate_matrix(name, 2, vector1.col_size);
        strcpy(concatenated.name, name);
        for (int i = 0; i < vector1.col_size; i++)
        {
            concatenated.rows[0][i] = vector1.rows[0][i];
            concatenated.rows[1][i] = vector2.rows[0][i];
        }
    }

    else if (strcmp(direction, "column") == 0)
    {
        concatenated = allocate_matrix(name, vector1.col_size, 2);
        strcpy(concatenated.name, name);
        for (int i = 0; i < vector1.col_size; i++)
        {
            concatenated.rows[i][0] = vector1.rows[0][i];
            concatenated.rows[i][1] = vector2.rows[0][i];
        }
    }
    return concatenated;
}

struct Matrix rmatstack(struct Matrix matrix1, struct Matrix matrix2)
{
    struct Matrix concatenated;
    concatenated = allocate_matrix(matrix1.name, matrix1.row_size, matrix1.col_size + matrix2.col_size);
    strcpy(concatenated.name, matrix1.name);
    for (int i = 0; i < matrix1.row_size; i++)
    {
        for (int j = 0; j < matrix1.col_size; j++)
            concatenated.rows[i][j] = matrix1.rows[i][j];
        for (int j = 0; j < matrix2.col_size; j++)
            concatenated.rows[i][matrix1.col_size + j] = matrix2.rows[i][j];
    }

    free_matrix(&matrix1);
    return concatenated;
}
struct Matrix dmatstack(struct Matrix matrix1, struct Matrix matrix2)
{
    struct Matrix concatenated;
    concatenated = allocate_matrix(matrix1.name, matrix1.row_size + matrix2.row_size, matrix1.col_size);
    strcpy(concatenated.name, matrix1.name);
    for (int j = 0; j < matrix1.col_size; j++)
    {
        for (int i = 0; i < matrix1.row_size; i++)
            concatenated.rows[i][j] = matrix1.rows[i][j];
        for (int i = 0; i < matrix2.row_size; i++)
            concatenated.rows[matrix1.row_size + i][j] = matrix2.rows[i][j];
    }
    free_matrix(&matrix1);
    return concatenated;
}

struct Matrix rmvstack(struct Matrix matrix, struct Matrix vector)
{
    struct Matrix concatenated;
    concatenated = allocate_matrix(matrix.name, matrix.row_size, matrix.col_size + 1);
    strcpy(concatenated.name, matrix.name);
    for (int i = 0; i < matrix.row_size; i++)
    {
        for (int j = 0; j < matrix.col_size; j++)
            concatenated.rows[i][j] = matrix.rows[i][j];
        concatenated.rows[i][matrix.col_size] = vector.rows[0][i];
    }
    free_matrix(&matrix);
    return concatenated;
}
struct Matrix dmvstack(struct Matrix matrix, struct Matrix vector)
{
    struct Matrix concatenated;
    concatenated = allocate_matrix(matrix.name, matrix.row_size + 1, matrix.col_size);
    strcpy(concatenated.name, matrix.name);
    for (int j = 0; j < matrix.col_size; j++)
    {
        for (int i = 0; i < matrix.row_size; i++)
            concatenated.rows[i][j] = matrix.rows[i][j];
        concatenated.rows[matrix.row_size][j] = vector.rows[0][j];
    }
    free_matrix(&matrix);
    return concatenated;
}

struct Matrix pad_matrix(struct Matrix matrix, int x, int y, char *mode) // tested
{
    struct Matrix padded_matrix = allocate_matrix(matrix.name, matrix.row_size + x, matrix.col_size + y);

    int row_values[matrix.row_size];

    int col_values[matrix.col_size];

    int extremum;

    for (int i = 0; i < matrix.row_size; i++)
        for (int j = 0; j < matrix.col_size; j++)
            padded_matrix.rows[i][j] = matrix.rows[i][j];

    //padding by max

    if (strcmp("maximum", mode) == 0)
    {
        int current_max = INT_MIN;

        // row padding
        for (int i = 0; i < matrix.row_size; i++)
        {
            for (int j = 0; j < matrix.col_size; j++)
                if (matrix.rows[i][j] > current_max)
                    current_max = matrix.rows[i][j];
            row_values[i] = current_max;
            current_max = INT_MIN;
        }

        // column padding
        for (int j = 0; j < matrix.col_size; j++)
        {
            for (int i = 0; i < matrix.row_size; i++)
                if (matrix.rows[i][j] > current_max)
                    current_max = matrix.rows[i][j];
            col_values[j] = current_max;
            current_max = INT_MIN;
        }

        int row_max = find_max(row_values, matrix.row_size);
        int col_max = find_max(col_values, matrix.col_size);

        if (row_max > col_max)
            extremum = row_max;
        else
            extremum = col_max;
    }

    //padding by min
    else if (strcmp("minimum", mode) == 0)
    {
        int current_min = INT_MAX;
        // row padding
        for (int i = 0; i < matrix.row_size; i++)
        {
            for (int j = 0; j < matrix.col_size; j++)
                if (matrix.rows[i][j] < current_min)
                    current_min = matrix.rows[i][j];
            row_values[i] = current_min;
            current_min = INT_MAX;
        }

        // column padding
        for (int j = 0; j < matrix.col_size; j++)
        {
            for (int i = 0; i < matrix.row_size; i++)
                if (matrix.rows[i][j] < current_min)
                    current_min = matrix.rows[i][j];
            col_values[j] = current_min;
            current_min = INT_MAX;
        }

        int row_min = find_min(row_values, matrix.row_size);
        int col_min = find_min(col_values, matrix.col_size);

        if (row_min < col_min)
            extremum = row_min;
        else
            extremum = col_min;
    }

    // padding towards to down of the matrix
    for (int i = matrix.row_size; i < matrix.row_size + x; i++)
        for (int j = 0; j < matrix.col_size; j++)
            padded_matrix.rows[i][j] = col_values[j];

    // padding towards to right of the matrix
    for (int j = matrix.col_size; j < matrix.col_size + y; j++)
        for (int i = 0; i < matrix.row_size; i++)
            padded_matrix.rows[i][j] = row_values[i];

    // filling rest of the matrix
    for (int i = matrix.row_size; i < matrix.row_size + x; i++)
        for (int j = matrix.col_size; j < matrix.col_size + y; j++)
            padded_matrix.rows[i][j] = extremum;
    free_matrix(&matrix);
    return padded_matrix;
}

struct Matrix pad_val(struct Matrix matrix, int x, int y, int value) //tested
{
    struct Matrix padded_matrix = allocate_matrix(matrix.name, matrix.row_size + x, matrix.col_size + y);

    // filling the matrix
    for (int i = 0; i < matrix.row_size; i++)
        for (int j = 0; j < matrix.col_size; j++)
            padded_matrix.rows[i][j] = matrix.rows[i][j];

    // padding towards to down of the matrix
    for (int i = matrix.row_size; i < matrix.row_size + x; i++)
        for (int j = 0; j < matrix.col_size; j++)
            padded_matrix.rows[i][j] = value;

    // padding towards to right of the matrix
    for (int j = matrix.col_size; j < matrix.col_size + y; j++)
        for (int i = 0; i < matrix.row_size; i++)
            padded_matrix.rows[i][j] = value;

    // filling rest of the matrix
    for (int i = matrix.row_size; i < matrix.row_size + x; i++)
        for (int j = matrix.col_size; j < matrix.col_size + y; j++)
            padded_matrix.rows[i][j] = value;
    free_matrix(&matrix);
    return padded_matrix;
}

struct Matrix slice_row(struct Matrix matrix, int index, int start, int stop, char *name) // tested
{
    struct Matrix sliced_matrix = allocate_matrix(name, 1, stop - start);
    for (int i = start; i < stop; i++)
        sliced_matrix.rows[0][i - start] = matrix.rows[index][i];
    return sliced_matrix;
}

struct Matrix slice_col(struct Matrix matrix, int index, int start, int stop, char *name) // tested
{
    struct Matrix sliced_matrix = allocate_matrix(name, 1, stop - start);
    for (int i = start; i < stop; i++)
        sliced_matrix.rows[0][i - start] = matrix.rows[i][index];
    return sliced_matrix;
}

struct Matrix slice_matrix(struct Matrix matrix, int col_start, int col_stop, int row_start, int row_stop, char *name) // tested
{
    struct Matrix sliced_matrix = allocate_matrix(name, row_stop - row_start, col_stop - col_start);
    for (int i = row_start; i < row_stop; i++)
        for (int j = col_start; j < col_stop; j++)
            sliced_matrix.rows[i - row_start][j - col_start] = matrix.rows[i][j];
    return sliced_matrix;
}

struct Matrix add_matrix(struct Matrix matrix1, struct Matrix matrix2) // tested
{
    struct Matrix product_matrix = allocate_matrix(matrix1.name, matrix1.row_size, matrix1.col_size);
    strcpy(product_matrix.name, matrix1.name);
    for (int i = 0; i < matrix1.row_size; i++)
        for (int j = 0; j < matrix1.col_size; j++)
            product_matrix.rows[i][j] = matrix1.rows[i][j] + matrix2.rows[i][j];
    free_matrix(&matrix1);
    return product_matrix;
}

struct Matrix subtract_matrix(struct Matrix matrix1, struct Matrix matrix2) // tested
{
    struct Matrix product_matrix = allocate_matrix(matrix1.name, matrix1.row_size, matrix1.col_size);
    strcpy(product_matrix.name, matrix1.name);
    for (int i = 0; i < matrix1.row_size; i++)
        for (int j = 0; j < matrix1.col_size; j++)
            product_matrix.rows[i][j] = matrix1.rows[i][j] - matrix2.rows[i][j];
    free_matrix(&matrix1);
    return product_matrix;
}

struct Matrix multiply_matrix(struct Matrix matrix1, struct Matrix matrix2) // tested
{
    struct Matrix product_matrix = allocate_matrix(matrix1.name, matrix1.row_size, matrix1.col_size);
    strcpy(product_matrix.name, matrix1.name);
    for (int i = 0; i < matrix1.row_size; i++)
        for (int j = 0; j < matrix1.col_size; j++)
            product_matrix.rows[i][j] = matrix1.rows[i][j] * matrix2.rows[i][j];
    free_matrix(&matrix1);
    return product_matrix;
}

void command_reader(struct Row **commands, int *commands_size, char *file_name)
{
    //Read the data and parceing it.
    struct Row row;
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(file_name, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1)
    {
        if (strcmp(line, "\r\n") == 0 || strcmp(line, "") == 0)
            continue;
        (*commands_size)++;
        *commands = realloc(*commands, sizeof(struct Row) * *commands_size);
        line[strcspn(line, "\r\n")] = '\0';
        row = split_line(line, " ");
        (*commands)[*(commands_size)-1] = row;
    }
    fclose(fp);
    if (line)
        free(line);
}

void fprint_matrix(struct Matrix matrix, FILE *fp)
{
    for (int i = 0; i < matrix.row_size; i++)
    {
        for (int j = 0; j < matrix.col_size; j++)
            fprintf(fp, "%d ", matrix.rows[i][j]);
        fprintf(fp, "\n");
    }
}

int find_matrix_by_name(char *matrix_name, struct Matrix *matrices, int matrices_size)
{
    int index = -1;
    for (int i = 0; i < matrices_size; i++)
    {
        if (strcmp(matrices[i].name, matrix_name) == 0)
        {
            index = i;
            break;
        }
    }
    return index;
}

void exec_commands(struct Row *commands, int commands_size, struct Matrix **matrices, int *matrices_size, char *name_of_output_file, char *dir_inputs)
{
    FILE *fp;
    fp = fopen(name_of_output_file, "w");
    for (int i = 0; i < commands_size; i++)
    {
        if (strcmp(commands[i].tokens[0], "veczeros") == 0) // done
        {
            struct Matrix veczeros = create_zeros(commands[i].tokens[1], 1, atoi(commands[i].tokens[2]));
            (*matrices_size)++;
            *matrices = realloc(*matrices, sizeof(struct Matrix) * *matrices_size);
            (*matrices)[*(matrices_size)-1] = veczeros;

            fprintf(fp, "%s %s %d\n", "created vector", veczeros.name, veczeros.col_size);
            fprint_matrix(veczeros, fp);
        }

        else if (strcmp(commands[i].tokens[0], "matzeros") == 0) // done
        {
            struct Matrix matzeros = create_zeros(commands[i].tokens[1], atoi(commands[i].tokens[2]), atoi(commands[i].tokens[3]));
            (*matrices_size)++;
            *matrices = realloc(*matrices, sizeof(struct Matrix) * *matrices_size);
            (*matrices)[*(matrices_size)-1] = matzeros;

            fprintf(fp, "%s %s %d %d\n", "created matrix", matzeros.name, matzeros.row_size, matzeros.col_size);
            fprint_matrix(matzeros, fp);
        }

        else if (strcmp(commands[i].tokens[0], "vecread") == 0)
        {
            struct Matrix newvec = read_file(dir_inputs, commands[i].tokens[1]);
            (*matrices_size)++;
            *matrices = realloc(*matrices, sizeof(struct Matrix) * *matrices_size);
            (*matrices)[*(matrices_size)-1] = newvec;

            fprintf(fp, "%s %s %d\n", "read vector", newvec.name, newvec.col_size);
            fprint_matrix(newvec, fp);
        }

        else if (strcmp(commands[i].tokens[0], "matread") == 0)
        {
            struct Matrix newmat = read_file(dir_inputs, commands[i].tokens[1]);
            (*matrices_size)++;
            *matrices = realloc(*matrices, sizeof(struct Matrix) * *matrices_size);
            (*matrices)[*(matrices_size)-1] = newmat;

            fprintf(fp, "%s %s %d %d\n", "read matrix", newmat.name, newmat.row_size, newmat.col_size);
            fprint_matrix(newmat, fp);
        }

        else if (strcmp(commands[i].tokens[0], "vecstack") == 0)
        {
            int index_of_first_vec = find_matrix_by_name(commands[i].tokens[1], *matrices, *matrices_size);
            int index_of_second_vec = find_matrix_by_name(commands[i].tokens[2], *matrices, *matrices_size);

            if (!(index_of_first_vec == -1 || index_of_second_vec == -1))
            {
                struct Matrix first_vec = (*matrices)[index_of_first_vec];
                struct Matrix second_vec = (*matrices)[index_of_second_vec];

                if (first_vec.col_size == second_vec.col_size)
                {
                    struct Matrix concatenated_matrix = vecstack(first_vec, second_vec, commands[i].tokens[3], commands[i].tokens[4]);

                    (*matrices_size)++;
                    *matrices = realloc(*matrices, sizeof(struct Matrix) * *matrices_size);
                    (*matrices)[*(matrices_size)-1] = concatenated_matrix; // name eklenecek.

                    fprintf(fp, "%s %s %d %d\n", "vectors concatenated", concatenated_matrix.name, concatenated_matrix.row_size, concatenated_matrix.col_size);
                    fprint_matrix(concatenated_matrix, fp);
                }
                else
                    fprintf(fp, "error\n");
            }
            else
                fprintf(fp, "error\n");
        }

        else if (strcmp(commands[i].tokens[0], "matstack") == 0)
        {
            int index_of_first_mat = find_matrix_by_name(commands[i].tokens[1], *matrices, *matrices_size);
            int index_of_second_mat = find_matrix_by_name(commands[i].tokens[2], *matrices, *matrices_size);

            if (!(index_of_first_mat == -1 || index_of_second_mat == -1))
            {
                struct Matrix first_mat = (*matrices)[index_of_first_mat];
                struct Matrix second_mat = (*matrices)[index_of_second_mat];
                char direction = commands[i].tokens[3][0];
                if (direction == 'r')
                {
                    if (first_mat.row_size == second_mat.row_size)
                    {
                        struct Matrix concatenated_matrix = rmatstack(first_mat, second_mat);
                        (*matrices)[index_of_first_mat] = concatenated_matrix; // name eklenecek.
                        fprintf(fp, "%s %s %d %d\n", "matrices concatenated", concatenated_matrix.name, concatenated_matrix.row_size, concatenated_matrix.col_size);
                        fprint_matrix(concatenated_matrix, fp);
                    }
                    else
                        fprintf(fp, "error\n");
                }
                else if (direction == 'd')
                {
                    if (first_mat.col_size == second_mat.col_size)
                    {
                        struct Matrix concatenated_matrix = dmatstack(first_mat, second_mat);
                        (*matrices)[index_of_first_mat] = concatenated_matrix; // name eklenecek.
                        fprintf(fp, "%s %s %d %d\n", "matrices concatenated", concatenated_matrix.name, concatenated_matrix.row_size, concatenated_matrix.col_size);
                        fprint_matrix(concatenated_matrix, fp);
                    }
                    else
                        fprintf(fp, "error\n");
                }
            }
            else
                fprintf(fp, "error\n");
        }

        else if (strcmp(commands[i].tokens[0], "mvstack") == 0) //done
        {
            int index_of_mat = find_matrix_by_name(commands[i].tokens[1], *matrices, *matrices_size);
            int index_of_vec = find_matrix_by_name(commands[i].tokens[2], *matrices, *matrices_size);
            if (!(index_of_mat == -1 || index_of_vec == -1))
            {
                struct Matrix mat = (*matrices)[index_of_mat];
                struct Matrix vec = (*matrices)[index_of_vec];

                char direction = commands[i].tokens[3][0];
                if (direction == 'r')
                {
                    if (mat.row_size == vec.col_size)
                    {
                        printf("ibrahim\n");
                        struct Matrix concatenated_matrix = rmvstack(mat, vec);

                        (*matrices)[index_of_mat] = concatenated_matrix;

                        fprintf(fp, "%s %s %d %d\n", "matrix and vector concatenated", concatenated_matrix.name, concatenated_matrix.row_size, concatenated_matrix.col_size);
                        fprint_matrix(concatenated_matrix, fp);
                    }
                    else
                        fprintf(fp, "error\n");
                }
                else if (direction == 'd')
                {
                    if (mat.col_size == vec.col_size)
                    {
                        struct Matrix concatenated_matrix = dmvstack(mat, vec);

                        (*matrices)[index_of_mat] = concatenated_matrix;

                        fprintf(fp, "%s %s %d %d\n", "matrix and vector concatenated", concatenated_matrix.name, concatenated_matrix.row_size, concatenated_matrix.col_size);
                        fprint_matrix(concatenated_matrix, fp);
                    }
                    else
                        fprintf(fp, "error\n");
                }
            }
            else
                fprintf(fp, "error\n");
        }

        else if (strcmp(commands[i].tokens[0], "pad") == 0)
        {
            int index_of_mat = find_matrix_by_name(commands[i].tokens[1], *matrices, *matrices_size);

            if (index_of_mat != -1)
            {
                (*matrices)[index_of_mat] = pad_matrix((*matrices)[index_of_mat], atoi(commands[i].tokens[2]), atoi(commands[i].tokens[3]), commands[i].tokens[4]);
                fprintf(fp, "%s %s %d %d\n", "matrix paded", (*matrices)[index_of_mat].name, (*matrices)[index_of_mat].row_size, (*matrices)[index_of_mat].col_size);
                fprint_matrix((*matrices)[index_of_mat], fp);
            }
            else
                fprintf(fp, "error\n");
        }

        else if (strcmp(commands[i].tokens[0], "padval") == 0)
        {
            int index_of_mat = find_matrix_by_name(commands[i].tokens[1], *matrices, *matrices_size);
            if (index_of_mat != -1)
            {
                (*matrices)[index_of_mat] = pad_val((*matrices)[index_of_mat], atoi(commands[i].tokens[2]), atoi(commands[i].tokens[3]), atoi(commands[i].tokens[4]));
                fprintf(fp, "%s %s %d %d\n", "matrix paded", (*matrices)[index_of_mat].name, (*matrices)[index_of_mat].row_size, (*matrices)[index_of_mat].col_size);
                fprint_matrix((*matrices)[index_of_mat], fp);
            }
            else
                fprintf(fp, "error\n");
        }

        else if (strcmp(commands[i].tokens[0], "vecslice") == 0)
        {
            int index_of_vec = find_matrix_by_name(commands[i].tokens[1], *matrices, *matrices_size);
            if (index_of_vec != -1)
            {
                if (0 <= atoi(commands[i].tokens[2]) && atoi(commands[i].tokens[2]) < (*matrices)[index_of_vec].col_size && 1 <= atoi(commands[i].tokens[3]) && atoi(commands[i].tokens[3]) <= (*matrices)[index_of_vec].col_size)
                {
                    (*matrices_size)++;
                    *matrices = realloc(*matrices, sizeof(struct Matrix) * (*matrices_size));
                    (*matrices)[*(matrices_size)-1] = slice_row((*matrices)[index_of_vec], 0, atoi(commands[i].tokens[2]), atoi(commands[i].tokens[3]), commands[i].tokens[4]);
                    fprintf(fp, "%s %s %d\n", "vector sliced", (*matrices)[*(matrices_size)-1].name, (*matrices)[*(matrices_size)-1].col_size);
                    fprint_matrix((*matrices)[*(matrices_size)-1], fp);
                }
                else
                    fprintf(fp, "error\n");
            }
            else
                fprintf(fp, "error\n");
        }

        else if (strcmp(commands[i].tokens[0], "matslicecol") == 0)
        {
            int index_of_mat = find_matrix_by_name(commands[i].tokens[1], *matrices, *matrices_size);
            if (index_of_mat != -1)
            {
                if (0 <= atoi(commands[i].tokens[3]) && atoi(commands[i].tokens[3]) < (*matrices)[index_of_mat].row_size && 1 <= atoi(commands[i].tokens[4]) && atoi(commands[i].tokens[4]) <= (*matrices)[index_of_mat].row_size && 0 <= atoi(commands[i].tokens[2]) && atoi(commands[i].tokens[2]) < (*matrices)[index_of_mat].col_size)
                {
                    (*matrices_size)++;
                    *matrices = realloc(*matrices, sizeof(struct Matrix) * *matrices_size);
                    (*matrices)[*(matrices_size)-1] = slice_col((*matrices)[index_of_mat], atoi(commands[i].tokens[2]), atoi(commands[i].tokens[3]), atoi(commands[i].tokens[4]), commands[i].tokens[5]);

                    fprintf(fp, "%s %s %d\n", "vector sliced", (*matrices)[*(matrices_size)-1].name, (*matrices)[*(matrices_size)-1].col_size);
                    fprint_matrix((*matrices)[*(matrices_size)-1], fp);
                }
                else
                    fprintf(fp, "error\n");
            }
            else
                fprintf(fp, "error\n");
        }

        else if (strcmp(commands[i].tokens[0], "matslicerow") == 0)
        {
            int index_of_mat = find_matrix_by_name(commands[i].tokens[1], *matrices, *matrices_size);
            if (index_of_mat != -1)
            {
                if (0 <= atoi(commands[i].tokens[3]) && atoi(commands[i].tokens[3]) < (*matrices)[index_of_mat].col_size && 1 <= atoi(commands[i].tokens[4]) && atoi(commands[i].tokens[4]) <= (*matrices)[index_of_mat].col_size && 0 <= atoi(commands[i].tokens[2]) && atoi(commands[i].tokens[2]) < (*matrices)[index_of_mat].row_size)
                {
                    (*matrices_size)++;
                    *matrices = realloc(*matrices, sizeof(struct Matrix) * *matrices_size);
                    (*matrices)[*(matrices_size)-1] = slice_row((*matrices)[index_of_mat], atoi(commands[i].tokens[2]), atoi(commands[i].tokens[3]), atoi(commands[i].tokens[4]), commands[i].tokens[5]);

                    fprintf(fp, "%s %s %d\n", "vector sliced", (*matrices)[*(matrices_size)-1].name, (*matrices)[*(matrices_size)-1].col_size);
                    fprint_matrix((*matrices)[*(matrices_size)-1], fp);
                }
                else
                    fprintf(fp, "error\n");
            }
            else
                fprintf(fp, "error\n");
        }

        else if (strcmp(commands[i].tokens[0], "matslice") == 0)
        {
            int index_of_mat = find_matrix_by_name(commands[i].tokens[1], *matrices, *matrices_size);
            if (index_of_mat != -1)
            {
                if (0 <= atoi(commands[i].tokens[2]) && atoi(commands[i].tokens[2]) < (*matrices)[index_of_mat].col_size && 1 <= atoi(commands[i].tokens[3]) && atoi(commands[i].tokens[3]) <= (*matrices)[index_of_mat].col_size && 0 <= atoi(commands[i].tokens[4]) && atoi(commands[i].tokens[4]) < (*matrices)[index_of_mat].row_size && 1 <= atoi(commands[i].tokens[5]) && atoi(commands[i].tokens[5]) <= (*matrices)[index_of_mat].row_size)
                {
                    (*matrices_size)++;
                    *matrices = realloc(*matrices, sizeof(struct Matrix) * *matrices_size);
                    (*matrices)[*(matrices_size)-1] = slice_matrix((*matrices)[index_of_mat], atoi(commands[i].tokens[2]), atoi(commands[i].tokens[3]), atoi(commands[i].tokens[4]), atoi(commands[i].tokens[5]), commands[i].tokens[6]);

                    fprintf(fp, "%s %s %d %d\n", "matrix sliced", (*matrices)[*(matrices_size)-1].name, (*matrices)[*(matrices_size)-1].row_size, (*matrices)[*(matrices_size)-1].col_size);
                    fprint_matrix((*matrices)[*(matrices_size)-1], fp);
                }
                else
                    fprintf(fp, "error\n");
            }
            else
                fprintf(fp, "error\n");
        }

        else if (strcmp(commands[i].tokens[0], "add") == 0)
        {
            int index_of_first_mat = find_matrix_by_name(commands[i].tokens[1], *matrices, *matrices_size);
            int index_of_second_mat = find_matrix_by_name(commands[i].tokens[2], *matrices, *matrices_size);
            if (index_of_second_mat != 1 && index_of_second_mat != 1)
            {
                struct Matrix first_mat = (*matrices)[index_of_first_mat];
                struct Matrix second_mat = (*matrices)[index_of_second_mat];
                if (first_mat.row_size == second_mat.row_size && first_mat.col_size == second_mat.col_size)
                {
                    (*matrices)[index_of_first_mat] = add_matrix(first_mat, second_mat);
                    fprintf(fp, "%s %s %s\n", "add", (*matrices)[index_of_first_mat].name, second_mat.name);
                    fprint_matrix((*matrices)[index_of_first_mat], fp);
                }
                else
                    fprintf(fp, "error\n");
            }
            else
                fprintf(fp, "error\n");
        }

        else if (strcmp(commands[i].tokens[0], "multiply") == 0)
        {
            int index_of_first_mat = find_matrix_by_name(commands[i].tokens[1], *matrices, *matrices_size);
            int index_of_second_mat = find_matrix_by_name(commands[i].tokens[2], *matrices, *matrices_size);
            if (index_of_second_mat != 1 && index_of_second_mat != 1)
            {
                struct Matrix first_mat = (*matrices)[index_of_first_mat];
                struct Matrix second_mat = (*matrices)[index_of_second_mat];
                if (first_mat.row_size == second_mat.row_size && first_mat.col_size == second_mat.col_size)
                {
                    (*matrices)[index_of_first_mat] = multiply_matrix(first_mat, second_mat);
                    fprintf(fp, "%s %s %s\n", "multiply", (*matrices)[index_of_first_mat].name, second_mat.name);
                    fprint_matrix((*matrices)[index_of_first_mat], fp);
                }
                else
                    fprintf(fp, "error\n");
            }
            else
                fprintf(fp, "error\n");
        }

        else if (strcmp(commands[i].tokens[0], "subtract") == 0)
        {
            int index_of_first_mat = find_matrix_by_name(commands[i].tokens[1], *matrices, *matrices_size);
            int index_of_second_mat = find_matrix_by_name(commands[i].tokens[2], *matrices, *matrices_size);
            if (index_of_second_mat != 1 && index_of_second_mat != 1)
            {
                struct Matrix first_mat = (*matrices)[index_of_first_mat];
                struct Matrix second_mat = (*matrices)[index_of_second_mat];
                if (first_mat.row_size == second_mat.row_size && first_mat.col_size == second_mat.col_size)
                {
                    (*matrices)[index_of_first_mat] = subtract_matrix(first_mat, second_mat);
                    fprintf(fp, "%s %s %s\n", "subtract", (*matrices)[index_of_first_mat].name, second_mat.name);
                    fprint_matrix((*matrices)[index_of_first_mat], fp);
                }
                else
                    fprintf(fp, "error\n");
            }
            else
                fprintf(fp, "error\n");
        }
        else
            fprintf(fp, "error\n");
    }
    fclose(fp);
}

int main(int argc, char *argv[])
{
    struct Matrix *matrices = malloc(sizeof(struct Matrix));
    int matrices_size = 0;

    struct Row *commands = malloc(sizeof(struct Row));
    int commands_size = 0;

    command_reader(&commands, &commands_size, argv[2]);

    exec_commands(commands, commands_size, &matrices, &matrices_size, argv[3], argv[1]);

    //free
    for (int i = 0; i < matrices_size; i++)
        free_matrix(&(matrices[i]));

    free(matrices);

    for (int i = 0; i < commands_size; i++)
    {
        for (int j = 0; j < commands[i].token_size; j++)
            free(commands[i].tokens[j]);
        free(commands[i].tokens);
    }

    free(commands);
    return 0;
}