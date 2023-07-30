#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spell.h"
//John Mounce CSE3318-002
/*  Suggestions
- When you work with 2D arrays, be careful. Either manage the memory yourself, or
work with local 2D arrays. Note C99 allows parameters as array sizes as long as
they are declared before the array in the parameter list. See:
https://www.geeksforgeeks.org/pass-2d-array-parameter-c/

Worst case time complexity to compute the edit distance from T test words
 to D dictionary words where all words have length MAX_LEN:
Student answer:  Theta(T*D*MAX_LEN^2)

*/

/* Write helper functions here */

/*
Parameters:
  - first_string - pointer to the first string (displayed vertical in the table)
  - second_string - pointer to the second string (displayed horizontal in the table)
  - print_table - If 1, the table with the calculations for the edit distance will be printed.
                  If 0 (or any value other than 1)it will not print the table
Return:  the value of the edit distance
*/

// function to print the "---" divider based on the string length
void print_divider(int str_length)
{
    printf("\n");
    for (int i = 0; i <= str_length + 1; i++)
    {
        printf("----");
    }
    printf("\n");
}

// function to calculate minimum edit distance
int find_min(int a, int b, int c)
{
    int min = a;
    if (b < min)
    {
        min = b;
    }
    if (c < min)
    {
        min = c;
    }
    return min;
}

// function to print ED table
void print_ED_table(char *first_string, char *second_string, int **arr, int x, int y)
{
    printf("\n");
    printf("%3c|", ' ');
    printf("%3c|", ' ');
    for (int i = 0; i < y; i++)
    {
        printf("%3c|", second_string[i]);
    }
    print_divider(y);

    for (int i = 0; i <= x; i++)
    {
        if (i == 0)
        {
            printf("%3c|", ' ');
        }
        else
        {
            printf("%3c|", first_string[i - 1]);
        }
        for (int j = 0; j <= y; j++)
        {
            printf("%3d|", arr[i][j]);
        }
        print_divider(y);
    }
    printf("\n");
}

// function to calculate the ED of two strings
int edit_distance(char *first_string, char *second_string, int print_table)
{
    // save string lengths for less typing later on
    int x = strlen(first_string);
    int y = strlen(second_string);

    // allocate memory for 2D array
    int **arr = (int **)calloc((x + 1), sizeof(int *));
    for (int i = 0; i <= x; i++)
    {
        arr[i] = (int *)calloc((y + 1), sizeof(int));
    }

    // calculate the edit distance for each array slot
    // starting at 1 because i have already loaded the default ED's in previous 2 loops
    for (int i = 0; i <= x; i++)
    {
        for (int j = 0; j <= y; j++)
        {
            if (i == 0)
            {
                arr[i][j] = j;
            }
            else if (j == 0)
            {
                arr[i][j] = i;
            }
            else if (first_string[i - 1] == second_string[j - 1])
            {
                // if chars are equal, the diagonal ED will not get +1
                arr[i][j] = find_min((arr[i - 1][j]) + 1, (arr[i][j - 1]) + 1, arr[i - 1][j - 1]);
            }
            else
            {
                // if chars are not equal, the diagonal ED will get +1
                arr[i][j] = find_min((arr[i - 1][j]) + 1, (arr[i][j - 1]) + 1, (arr[i - 1][j - 1]) + 1);
            }
        }
    }

    // if print enabled on function call in spell_checker.c, call print_ED_table function
    if (print_table == 1)
    {
        print_ED_table(first_string, second_string, arr, x, y);
    }

    // save final index(final ED), before freeing ED_arr
    int final_ED = arr[x][y];

    // free memory allocated
    for (int i = 0; i <= x; i++)
    {
        free(arr[i]);
    }
    free(arr);

    return final_ED; // return the edit distance, wich is in the last cell
}

/*
Parameters:
  - testname - string containing the name of the file with the paragraph to spell check, includes file extenssion e.g. "text1.txt"
  - dictname - name of file with dictionary words. Includes file extenssion, e.g. "dsmall.txt"
Behavior: If any of the files cannot be open displays a message and returns. (Does not exit the program.)
*/

void spell_check(char *testname, char *dictname)
{
    // file open and save the number of strings for both files
    int test_num;
    FILE *fp_test;
    fp_test = fopen(testname, "r");

    int dict_num;
    FILE *fp_dict;
    fp_dict = fopen(dictname, "r");

    char test_buffer[128];
    char dict_buffer[128];

    // check fp's
    if ((fp_test == NULL) || (fp_dict == NULL))
    {
        printf("could not open one or both of the files: %s, %s \n\n", dictname, testname);
        return;
    }

    // saving the amount of elements in each file passed and cutting off new line
    fscanf(fp_test, "%d", &test_num);
    fgets(test_buffer, sizeof(test_buffer), fp_test);
    fscanf(fp_dict, "%d", &dict_num);
    fgets(dict_buffer, sizeof(dict_buffer), fp_dict);

    // declare string arrays to hold all items
    char **test_arr = (char **)malloc(test_num * sizeof(char *));
    char **dict_arr = (char **)malloc(dict_num * sizeof(char *));
    int **ED_arr = (int **)calloc((test_num), sizeof(int *));
    
    for (int i = 0; i < test_num; i++)
    {
        ED_arr[i] = (int *)calloc(dict_num, sizeof(int));
    }

    // loading test elements into test_arr
    for (int i = 0; i < test_num; i++)
    {
        if (fgets(test_buffer, sizeof(test_buffer), fp_test) != NULL)
        {
            test_buffer[strcspn(test_buffer, "\n")] = '\0';
            test_arr[i] = (char *)malloc((strlen(test_buffer)) * sizeof(char));
            strcpy(test_arr[i], test_buffer);
        }
        else
        {
            // free memory if unable to read and exit
            printf("unable to read string %d from the file\n", i);
            for (int i = 0; i < test_num; i++)
            {
                free(test_arr[i]);
            }
            free(test_arr);
            fclose(fp_test);
            return;
        }
    }

    // load dictonary elements into dict_arr
    for (int i = 0; i < dict_num; i++)
    {
        if (fgets(dict_buffer, sizeof(dict_buffer), fp_dict) != NULL)
        {
            dict_buffer[strcspn(dict_buffer, "\n")] = '\0';
            dict_arr[i] = (char *)malloc((strlen(dict_buffer) + 1) * sizeof(char));
            strcpy(dict_arr[i], dict_buffer);
        }
        else
        {
            // free allocated mem to dict_arr if cannot read string from file
            printf("unable to read string %d from the file\n", i);
            for (int i = 0; i < dict_num; i++)
            {
                free(dict_arr[i]);
            }
            free(dict_arr);
            fclose(fp_dict);
            return;
        }
    }

    // for testing printing elemts in test_arr and dict_arr to make sure they were loaded from files correctly
    /*for (int i = 0; i < test_num; i++)
    {
        printf("|%s|\n", test_arr[i]);
    }
    printf("====================\n");
    for (int i = 0; i < dict_num; i++)
    {
        printf("|%s|\n", dict_arr[i]);
    }*/

    // put ED's into ED_arr
    for (int i = 0; i < test_num; i++)
    {
        for (int j = 0; j < dict_num; j++)
        {
            if (strlen(test_arr[i]) < strlen(dict_arr[j]))
            {
                ED_arr[i][j] = edit_distance(test_arr[i], dict_arr[j], 0);
            }
            else
            {
                ED_arr[i][j] = edit_distance(dict_arr[j], test_arr[i], 0);
            }
        }
    }

    // beginning of the while loop for menu
    printf("\nloaded dictonary from %s has size: %d\n\n", dictname, dict_num);
    int loop_counter = 0;
    char change_string[128];
    int user_input;
    while (loop_counter < test_num)
    {
        // print out current string from test file and default choices
        printf("---> |%s|\n", test_arr[loop_counter]);
        printf("-1 - Type correction\n");
        printf(" 0 - Leave word as is (do not fix spelling)\n");

        // finding the min ED for the current row/string in the ED_arr
        int min = 399999999;
        for (int j = 0; j < dict_num; j++)
        {
            if (min > ED_arr[loop_counter][j])
            {
                min = ED_arr[loop_counter][j];
            }
        }

        // printing the min distance and minimum ED strings from dictonary
        printf("     Minimum distance: %2d\n", min);
        printf("     Words that give minimum distance:\n");
        int temp_counter = 1;
        for (int j = 0; j < dict_num; j++)
        {
            if (min == ED_arr[loop_counter][j])
            {
                printf("%2d - %s\n", temp_counter, dict_arr[j]);
                temp_counter++;
            }
        }

        // get user input based on menu
        printf("pick your selection: ");
        scanf("%d", &user_input);

        if (user_input == -1)
        {
            printf("enter correction: ");
            scanf("%s", change_string);
            test_arr[loop_counter] = change_string;
            printf("the corrected word is: %s\n\n", change_string);
        }
        else if (user_input == 0)
        {
            printf("the corrected word is: %s\n\n", test_arr[loop_counter]);
        }
        else if (user_input < -1 || user_input > temp_counter)
        {
            printf("Invalid choice, your original word will be kept.\n");
            printf("The corrected word is: %s\n\n", test_arr[loop_counter]);
        }
        else
        {
            temp_counter = 1;
            for (int j = 0; j < dict_num; j++)
            {
                if (min == ED_arr[loop_counter][j])
                {
                    if (temp_counter == user_input)
                    {
                        printf("your corrected word is: %s\n\n", dict_arr[j]);
                        break;
                    }
                    temp_counter++;
                }
            }
        }
        loop_counter++;
    }

    // free all allocated memory for test_arr
    for (int i = 0; i < test_num; i++)
    {
        free(test_arr[i]);
    }
    free(test_arr);

    // free all allocated memory for dict_arr
    for (int i = 0; i < dict_num; i++)
    {
        free(dict_arr[i]);
    }
    free(dict_arr);

    // free all allocated memory for ED_arr
    for (int i = 0; i < test_num; i++)
    {
        free(ED_arr[i]);
    }
    free(ED_arr);

    // close files used
    fclose(fp_dict);
    fclose(fp_test);
}
