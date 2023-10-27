# Parallelized-Boyer-Moore-Search

#Introduction

Selected Pattern Matching Algorithm: Boyer-Moore String Matching Algorithm

 The Boyer-Moore string matching algorithm is a widely-used and efficient method for finding
 occurrences of a pattern (a smaller string) within a larger text string. It is known for its simplicity and
 speed according to GeeksforGeeks. (2015), making it a suitable choice for various applications,
 including text search and data processing.
 Description:
 The core idea behind Boyer-Moore is to skip as many characters as possible while searching for a
 pattern match. It accomplishes this by:
 Bad Character Heuristic:
 The bad character heuristic is a straightforward concept. When there's a mismatch between the current
 character in the text and the corresponding character in the pattern, we consider it a "bad character."
 Weneed to adjust the pattern's position until one of the following happens, this is even shown in
 Horspool, R. N. (1980):

 1) The mismatch becomes a match.
 In this case, we find the last occurrence of the mismatched character in the pattern. If the
 mismatched character exists in the pattern, we shift the pattern to align it with the mismatched
 character in the text.
 Figure 1: Mismatch becoming a match

 2) The pattern moves past the mismatched character.
 Here, we look for the last occurrence of the mismatching character in the pattern. If the
 character doesn't exist in the pattern, we simply shift the pattern past the mismatched
 character in the text.

 Figure 2: Pattern move past the mismatch character
 Justification of Selection:
 The choice of the Boyer-Moore algorithm for string matching is justified for several reasons:
 Efficiency: Boyer-Moore is known for its efficiency in practice. It significantly reduces the number of
 character comparisons during the search, making it one of the fastest string matching algorithms.
 Simplicity: Unlike some complex algorithms, Boyer-Moore is conceptually straightforward to
 understand and implement. It doesn't require intricate data structures or advanced mathematical
 models.
 Parallelism: it is inherently suitabile for parallelization and optimization. Its simplicity and structured
 approach to pattern matching make it easier to break down into parallel tasks and optimize for
 performance. This algorithm's well-defined steps and character-based heuristic allow for efficient parallel
 processing, which can significantly speed up the pattern matching process across large datasets.

 Test Data Sets Used
 The query text file is in the format [word 1] OR [word 0],depending on if the word existed in the word list or not and each line of the file consisted
 of 1 word,same goes to the word.txt file have one word per line and all the words are unique to
 replicate the test for a scenario that is not best case for the boyer moore algorithm,names of the test files
 were:
 [word_small.txt,query_small.txt]:size of 10000 and 20000 respectively
