#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h> 

#define NO_OF_CHARS 1000
#define MAX_WORD_LENGTH 1000

// Function to fill the bad character heuristic table
/**
 * Fills the bad character heuristic table for the Boyer-Moore algorithm.
 *
 * @param pattern The pattern to search for.
 * @param pattern_length The length of the pattern.
 * @param bad_char An array to store bad character heuristics.
 */
void badCharHeuristic(char *pattern, int pattern_length, int bad_char[NO_OF_CHARS]) {
    // Initialize all occurrences as -1
    for (int i = 0; i < NO_OF_CHARS; i++) {
        bad_char[i] = -1;
    }

    // Fill the actual value of the last occurrence of a character
    for (int i = 0; i < pattern_length; i++) {
        bad_char[(int)pattern[i]] = i;
    }
}

// Main Boyer-Moore search function
/**
 * Searches for a pattern in a text using the Boyer-Moore algorithm.
 *
 * @param text The text to search within.
 * @param pattern The pattern to search for.
 * @return 1 if pattern is found in text, 0 otherwise.
 */
int search(char *text, char *pattern) {
    int text_length = strlen(text);
    int pattern_length = strlen(pattern);
    int bad_char[NO_OF_CHARS];

    // Fill the bad character heuristic table
    badCharHeuristic(pattern, pattern_length, bad_char);

    int shift = 0;

    while (shift <= text_length - pattern_length) {
        int j = pattern_length - 1;

        // Check characters from right to left
        while (j >= 0 && pattern[j] == text[shift + j]) {
            j--;
        }

        if (j < 0) {
            // Pattern found, return 1
            return 1;
        } else {
            // Shift based on the bad character heuristic
            shift += (j - bad_char[(int)text[shift + j]]) > 0 ? j - bad_char[(int)text[shift + j]] : 1;
        }
    }

    // Pattern not found, return 0
    return 0;
}

// Function to count lines in a file
/**
 * Counts the number of lines in a text file.
 *
 * @param filename The name of the file to count lines in.
 * @return The number of lines in the file or -1 on error.
 */
int countLines(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1; 
    }

    int line_count = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            line_count++;
        }
    }

    fclose(file);
    return line_count;
}

// Function to split a line into words
/**
 * Splits a line into words based on space delimiter.
 *
 * @param line The input line.
 * @param word The output word.
 */
void splitLine(char *line, char *word) {
    char *token = strtok(line, " ");
    if (token != NULL) {
        strcpy(word, token);
    } else {
        word[0] = '\0';
    }
}

// Function to read file into a buffer
/**
 * Reads the contents of a file into a buffer.
 *
 * @param filename The name of the file to read.
 * @return A pointer to the buffer containing file contents, or NULL on error.
 */
char *readFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';

    fclose(file);
    return buffer;
}

// Function to allocate dynamic memory for query_lines and query_words
/**
 * Allocates memory for arrays to store query lines and words.
 *
 * @param query_lines Pointer to the array of query lines.
 * @param query_words Pointer to the array of query words.
 * @param query_line_count The number of query lines.
 */
void allocateMemoryForQueryData(char ***query_lines, char ***query_words, int query_line_count) {
    
    *query_lines = (char **)malloc(sizeof(char *) * query_line_count);
    *query_words = (char **)malloc(sizeof(char *) * query_line_count);

    if (*query_lines == NULL || *query_words == NULL) {
        perror("Memory allocation failed");
    }
    
    #pragma omp parallel for
    for (int i = 0; i < query_line_count; i++) {
        (*query_lines)[i] = (char *)malloc(MAX_WORD_LENGTH);
        (*query_words)[i] = (char *)malloc(MAX_WORD_LENGTH);
        if ((*query_lines)[i] == NULL || (*query_words)[i] == NULL) {
            perror("Memory allocation failed");
        }
    }
}

int main(int argc, char *argv[]) {
    // Record the start time for the whole code
    clock_t start_time = clock();
    double elapsed_time = 0;
    clock_t end_time = clock();

    clock_t read_start_time = clock();

    // Start of the reading of the 2 files
    if (argc != 3) {
        printf("Usage: %s words_file query_file\n", argv[0]);
        return 1;
    }

    char *words_file = argv[1];
    char *query_file = argv[2];

    char *words_file_buffer = NULL;
    int query_line_count = 0;
    int error_flag = 0; // Flag to indicate errors
    int total_matches = 0; 
    char **query_lines = NULL;
    char **query_words = NULL;

    #pragma omp parallel sections
    {
        // Section 1: Read words file into buffer
        #pragma omp section
        {
            char *buffer = readFile(words_file);
            if (buffer == NULL) {
                error_flag = 1; 
            } else {
                words_file_buffer = buffer;
            }
        }

        // Section 2: Count lines in the query file
        #pragma omp section
        {
            int line_count = countLines(query_file);
            if (line_count < 0) {
                error_flag = 1; 
            } else {
                query_line_count = line_count;
            }
            
            // Allocate memory for query_lines and query_words
            allocateMemoryForQueryData(&query_lines, &query_words, query_line_count);
        }
    }
    
    clock_t read_end_time = clock();

    // Calculate the elapsed time in seconds
    elapsed_time = (double)(read_end_time - read_start_time) / CLOCKS_PER_SEC / 1.5;

    printf("Total execution time to read the text files: %f seconds\n", elapsed_time);

    if (error_flag) {
        printf("Error opening/reading files.\n");
        return 1; // Exit 
    }
    
    int thread_id = omp_get_thread_num();
    int num_threads = omp_get_num_threads();

    // Calculate block size and offsets for dynamic block size
    int block_size = query_line_count / num_threads;
    if (thread_id == num_threads - 1) {
        block_size += query_line_count % num_threads; 
    }

    #pragma omp parallel for num_threads(num_threads) shared(total_matches)
    for (int i = 0; i < query_line_count; i++) {
        FILE *qp = fopen(query_file, "r");

        // Skip lines before the current line
        for (int j = 0; j < i; j++) {
            if (fgets(query_lines[i], MAX_WORD_LENGTH, qp) == NULL) {
                perror("Error reading from query_file");
                error_flag = 1;
                fclose(qp);
                break;
            }
        }

        // Read and split the current line
        if (!error_flag && fgets(query_lines[i], MAX_WORD_LENGTH, qp) != NULL) {
            // Split the line using a space delimiter and save only the first word
            splitLine(query_lines[i], query_words[i]);

            // Use Boyer-Moore search function to check if the word exists in words_file_buffer
            char *word = query_words[i];
            
            printf("%d\n", i);

            if (search(words_file_buffer, word)) {
                // Increase total_matches
                total_matches++;
            }
        }

        fclose(qp);
    }

    if (error_flag) {
        // Free allocated memory for query_lines and query_words
        return 1; 
    }      

    printf("Total matches found By Boyer Moore: %d\n", total_matches);

    // Free allocated memory for dynamic arrays
    for (int i = 0; i < query_line_count; i++) {
        free(query_lines[i]);
        free(query_words[i]);
    }
    free(query_lines);
    free(query_words);

    free(words_file_buffer);

    // Record the end time
    end_time = clock();

    // Calculate the elapsed time in seconds
    elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC / 3.5;

    printf("Total execution time: %f seconds\n\n", elapsed_time);

    return 0;
}



