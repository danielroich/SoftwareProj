#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "LP.h"
#include "gurobi_c.h"
#include "backtrack_core.h"
#include <math.h>

int get_num_of_parameters(Board *board)
{
    int i;
    int j;
    int s;
    int counter = 0;
    int board_size = board->num_of_columns * board->num_of_rows;
    int *possible_values = (int *)calloc(board_size, sizeof(int));
    for (i = 0; i < board_size; i++)
    {
        for (j = 0; j < board_size; j++)
        {
            if (get_value(i, j, board, 0) == BOARD_NULL_VALUE)
            {
                counter += get_possible_values(board, i, j, possible_values);
            }
        }
    }
    return counter;
}

int add_single_value_per_cell_constraints(Board *board, GRBmodel *model, int *ind, double *val, GRBenv *env)
{
    int i;
    int j;
    int k;
    int s;
    char i_char[4096];
    int error = 0;
    char unique_name[2048] = "a";
    int counter = 0;
    int chars_to_copy;
    int board_size = board->num_of_columns * board->num_of_rows;
    int *possible_values = (int *)calloc(board_size, sizeof(int));
    int possible_value_size;
    for (i = 0; i < board_size; i++)
    {
        for (j = 0; j < board_size; j++)
        {
            possible_value_size = get_possible_values(board, i, j, possible_values);

            if (possible_value_size == -1)
                continue;

            for (k = 0; k < possible_value_size; k++)
            {
                ind[k] = counter;
                val[k] = 1;
                counter++;
            }

            /* generate unique name by concatinating i value to the unique name string */
            chars_to_copy = i != 0 ? (int)((ceil(log10(i)) + 1) * sizeof(char)) : 1;
            sprintf(i_char, "%d", chars_to_copy);
            strcat(unique_name, i_char);

            error = GRBaddconstr(model, possible_value_size, ind, val, GRB_EQUAL, 1, unique_name);
            if (error)
            {
                printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
                return -1;
            }

            /* clean the data */
            for (k = 0; k < possible_value_size; k++)
            {
                ind[k] = 0;
                val[k] = 0;
            }
        }
    }
    return 1;
}

int add_rows_constraints(Board *board, GRBmodel *model, int *ind, double *val, GRBenv *env)
{
    int i;
    int j;
    int k;
    int constraint_index_base = 0;
    int value_of_constraint;
    char i_char[4096];
    int val_written = 0;
    int error = 0;
    int board_size = board->num_of_columns * board->num_of_rows;
    int optional_occurences = 0;
    char unique_name[2048] = "b";
    int counter = 0;
    int chars_to_copy;
    int *possible_values = (int *)calloc(board_size, sizeof(int));
    int possible_value_size;
    for (value_of_constraint = 0; value_of_constraint < board_size; value_of_constraint++)
    {
        counter = 0;
        for (i = 0; i < board_size; i++)
        {
            optional_occurences = 0;
            for (j = 0; j < board_size; j++)
            {

                possible_value_size = get_possible_values(board, i, j, possible_values);

                for (k = 0; k < board_size; k++)
                {
                    if (possible_values[k] == 1)
                    {
                        if (k == value_of_constraint)
                        {
                            ind[optional_occurences] = counter;
                            val[optional_occurences] = 1;
                            ++optional_occurences;
                        }
                        ++counter;
                    }
                }
            }

            if (optional_occurences == 0)
                continue;

            /* generate unique name by concatinating i value to the unique name string */
            chars_to_copy = i != 0 ? (int)((ceil(log10(i)) + 1) * sizeof(char)) : 1;
            sprintf(i_char, "%d", chars_to_copy);
            strcat(unique_name, i_char);

            error = GRBaddconstr(model, optional_occurences, ind, val, GRB_EQUAL, 1, unique_name);
            if (error)
            {
                printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
                return -1;
            }

            /* clean the data */
            for (k = 0; k < optional_occurences; k++)
            {
                ind[k] = 0;
                val[k] = 0;
            }
        }
    }
    return 1;
}

int add_column_constraints(Board *board, GRBmodel *model, int *ind, double *val, GRBenv *env)
{
    int i;
    int j;
    int k;
    int value_of_constraint;
    char i_char[4096];
    int board_size = board->num_of_columns * board->num_of_rows;
    int error = 0;
    int optional_occurences = 0;
    char unique_name[2048] = "c";
    int counter = 0;
    int chars_to_copy;
    int column_to_check;
    int *possible_values = (int *)calloc(board_size, sizeof(int));
    int possible_value_size;
    for (value_of_constraint = 0; value_of_constraint < board_size; value_of_constraint++)
    {
        for (column_to_check = 0; column_to_check < board_size; column_to_check++)
        {
            counter = 0;
            optional_occurences = 0;
            for (i = 0; i < board_size; i++)
            {

                for (j = 0; j < board_size; j++)
                {
                    possible_value_size = get_possible_values(board, i, j, possible_values);

                    for (k = 0; k < board_size; k++)
                    {
                        if (possible_values[k] == 1)
                        {
                            if (k == value_of_constraint && j == column_to_check)
                            {
                                ind[optional_occurences] = counter;
                                val[optional_occurences] = 1;
                                ++optional_occurences;
                            }
                            ++counter;
                        }
                    }
                }
            }

            if (optional_occurences == 0)
                continue;

            /* generate unique name by concatinating i value to the unique name string */
            chars_to_copy = i != 0 ? (int)((ceil(log10(i)) + 1) * sizeof(char)) : 1;
            sprintf(i_char, "%d", chars_to_copy);
            strcat(unique_name, i_char);

            error = GRBaddconstr(model, optional_occurences, ind, val, GRB_EQUAL, 1, unique_name);
            if (error)
            {
                printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
                return -1;
            }

            /* clean the data */
            for (k = 0; k < optional_occurences; k++)
            {
                ind[k] = 0;
                val[k] = 0;
            }
        }
    }
    return 1;
}

int add_square_constraints(Board *board, GRBmodel *model, int *ind, double *val, GRBenv *env)
{
    int i;
    int j;
    int k;
    int square_index_to_check;
    int value_of_constraint;
    int possible_value_size;
    char i_char[4096];
    int board_size = board->num_of_columns * board->num_of_rows;
    int error = 0;
    int optional_occurences = 0;
    char unique_name[2048] = "d";
    int counter = 0;
    int chars_to_copy;
    int value_to_check;
    int top_left_corner_row;
    int top_left_corner_col;
    int square_index;
    int **square_num_matrix = (int **)malloc(sizeof(int *) * board->num_of_columns);
    int *possible_values = (int *)calloc(board_size, sizeof(int));

    for (i = 0; i < board->num_of_columns; i++)
    {
        square_num_matrix[i] = (int *)malloc(sizeof(int) * board->num_of_rows);
        for (j = 0; j < board->num_of_rows; j++)
        {
            square_num_matrix[i][j] = counter;
            ++counter;
        }
    }

    for (value_of_constraint = 0; value_of_constraint < board_size; value_of_constraint++)
    {
        for (square_index_to_check = 0; square_index_to_check < board_size; square_index_to_check++)
        {
            counter = 0;
            optional_occurences = 0;
            for (i = 0; i < board_size; i++)
            {

                for (j = 0; j < board_size; j++)
                {
                    possible_value_size = get_possible_values(board, i, j, possible_values);

                    if (possible_value_size == -1)
                        continue;

                    top_left_corner_row = floor(i / board->num_of_rows) * board->num_of_rows;
                    top_left_corner_col = floor(j / board->num_of_columns) * board->num_of_columns;
                    square_index = square_num_matrix[(top_left_corner_row / board->num_of_columns)][(top_left_corner_col / board->num_of_rows)];

                    for (k = 0; k < board_size; k++)
                    {
                        if (possible_values[k] == 1)
                        {
                            if (k == value_of_constraint && square_index == square_index_to_check)
                            {
                                ind[optional_occurences] = counter;
                                val[optional_occurences] = 1;
                                ++optional_occurences;
                            }
                            ++counter;
                        }
                    }
                }
            }

            if (optional_occurences == 0)
                continue;

            /* generate unique name by concatinating i value to the unique name string */
            chars_to_copy = i != 0 ? (int)((ceil(log10(i)) + 1) * sizeof(char)) : 1;
            sprintf(i_char, "%d", chars_to_copy);
            strcat(unique_name, i_char);

            error = GRBaddconstr(model, optional_occurences, ind, val, GRB_EQUAL, 1, unique_name);
            if (error)
            {
                printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
                return -1;
            }

            /* clean the data */
            for (k = 0; k < optional_occurences; k++)
            {
                ind[k] = 0;
                val[k] = 0;
            }
        }
    }
    return 1;
}

int run_LP(Board *board, double *sol, int num_of_params, int params_mode)
{
    int i;
    int board_size = board->num_of_columns * board->num_of_rows;
    GRBenv *env = NULL;
    GRBmodel *model = NULL;
    int error = 0;
    int *ind = (int *)malloc(board_size * sizeof(int));
    double *val = (double *)malloc(board_size * sizeof(double));
    double *obj = (double *)malloc(num_of_params * sizeof(double));
    char *vtype = (char *)malloc(num_of_params * sizeof(char));
    int optimstatus;
    double objval;

    /* Create environment - log file is mip1.log */
    error = GRBloadenv(&env, "mip1.log");
    if (error)
    {
        printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }

    error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
    if (error)
    {
        printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }

    /* Create an empty model named "mip1" */
    error = GRBnewmodel(env, &model, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
    if (error)
    {
        printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }

    /* Add variables */
    for (i = 0; i < num_of_params; i++)
    {
        /* variable types - for x,y,z (cells 0,1,2 in "vtype") */
        /* other options: GRB_INTEGER, GRB_CONTINUOUS */
        if (params_mode == 1)
            vtype[i] = GRB_BINARY;
        else
            vtype[i] = GRB_CONTINUOUS;

        /* coefficients - for x,y,z (cells 0,1,2 in "obj")
        maximize x+y+z+k.. */
        obj[i] = 1;
    }

    /* add variables to model */
    error = GRBaddvars(model, num_of_params, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);
    if (error)
    {
        printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }

    /* Change objective sense to maximization */
    error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
    if (error)
    {
        printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }

    /* update the model - to integrate new variables */

    error = GRBupdatemodel(model);
    if (error)
    {
        printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }

    add_single_value_per_cell_constraints(board, model, ind, val, env);

    add_rows_constraints(board, model, ind, val, env);

    add_column_constraints(board, model, ind, val, env);

    add_square_constraints(board, model, ind, val, env);

    /* Optimize model - need to call this before calculation */
    error = GRBoptimize(model);
    if (error)
    {
        printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }

    /* Write model to 'mip1.lp' - this is not necessary but very helpful */
    error = GRBwrite(model, "mip1.lp");
    if (error)
    {
        printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }

    /* Get solution information */

    error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
    if (error)
    {
        printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }

    if (optimstatus == GRB_OPTIMAL)
    {

        /* print results */
        printf("\nOptimization complete\n");

        error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
        if (error)
        {
            printf("ERROR %d GRBgettdblattr(): %s\n", error, GRBgeterrormsg(env));
            return -1;
        }

        /* get the solution - the assignment to each variable */
        error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, num_of_params, sol);
        if (error)
        {
            printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
            return -1;
        }
    }

    /* no solution found */
    else if (optimstatus == GRB_INF_OR_UNBD)
    {
        printf("Model is infeasible or unbounded\n");
    }
    /* error or calculation stopped */
    else
    {
        printf("Optimization was stopped early\n");
    }

    /* IMPORTANT !!! - Free model and environment */
    GRBfreemodel(model);
    GRBfreeenv(env);

    return optimstatus == GRB_OPTIMAL ? 1 : 0;
}

int get_result_by_index(Board *board, double *sol, int row, int column)
{
    int i;
    int j;
    int k;
    int s;
    int temp_counter = 0;
    int counter = 0;
    int chosen_posbbile_values_index = 0;
    int board_size = board->num_of_columns * board->num_of_rows;
    int *possible_values = (int *)calloc(board_size, sizeof(int));
    for (i = 0; i < board_size; i++)
    {
        for (j = 0; j < board_size; j++)
        {
            if (get_value(i, j, board, 0) == BOARD_NULL_VALUE)
            {
                temp_counter = counter;
                counter += get_possible_values(board, i, j, possible_values);
                for (k = temp_counter; k < counter; k++)
                {
                    if (sol[k] > 0)
                    {

                        chosen_posbbile_values_index = k - temp_counter;
                        for (s = 0; s < board_size; s++)
                        {
                            if (possible_values[s] == 1)
                            {
                                if (chosen_posbbile_values_index == 0)
                                {
                                    if (i == row && j == column)
                                    {
                                        return s + 1;
                                    }
                                }
                                --chosen_posbbile_values_index;
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}

void fill_and_print_results(Board *board, double *sol, float threshold)
{
    int i;
    int j;
    int k;
    int s;
    int temp_counter = 0;
    int counter = 0;
    int chosen_posbbile_values_index = 0;
    int board_size = board->num_of_columns * board->num_of_rows;
    int *possible_values = (int *)calloc(board_size, sizeof(int));
    for (i = 0; i < board_size; i++)
    {
        for (j = 0; j < board_size; j++)
        {
            if (get_value(i, j, board, 0) == BOARD_NULL_VALUE)
            {
                temp_counter = counter;
                counter += get_possible_values(board, i, j, possible_values);
                for (k = temp_counter; k < counter; k++)
                {
                    if (sol[k] > threshold)
                    {
                        chosen_posbbile_values_index = k - temp_counter;
                        for (s = 0; s < board_size; s++)
                        {
                            if (possible_values[s] == 1)
                            {
                                if (chosen_posbbile_values_index == 0)
                                {
                                    printf("for index: %d,%d the value %d has prob of: %f\n", i, j, s + 1, sol[k]);
                                    set_value(i, j, s + 1, board, 0);
                                }
                                --chosen_posbbile_values_index;
                            }
                        }
                    }
                }
            }
        }
    }
}

int hint_ILP(Board *board, int row, int column)
{
    int status;
    int num_of_params = get_num_of_parameters(board);
    double *sol = (double *)malloc(num_of_params * sizeof(double));
    status = run_LP(board, sol, num_of_params, 1);

    if (status != 1)
    {
        return BOARD_NULL_VALUE;
    }

    return get_result_by_index(board, sol, row, column);
}

int guess_LP(Board *board, int row, int column, float threshold)
{
    int status;
    int i;
    int num_of_params = get_num_of_parameters(board);
    double *sol = (double *)malloc(num_of_params * sizeof(double));
    status = run_LP(board, sol, num_of_params, 0);

    if (status != 1)
    {
        return BOARD_NULL_VALUE;
    }

    fill_and_print_results(board, sol, threshold);

    return get_result_by_index(board, sol, row, column);
}

int validate_ILP(Board *board)
{
    int num_of_params = get_num_of_parameters(board);
    double *sol = (double *)malloc(num_of_params * sizeof(double));
    return run_LP(board, sol, num_of_params, 1);
}