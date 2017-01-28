#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define BUF_SZ // buffer size
#define NOT_COMMENT !is_scomment && !is_mcomment
#define NOT_QUOTE !is_squote && !is_dquote

// prints an error message and exits the program
void syntax_error_exit();
// returns a right brace's corresponding left brace
// returns a null byte if the character is not a right brace
char match_left(char);
void remove_comments(char* string);
void remove_strings(char* code);
// this function removes everything except
// brackets, parentheses and curly braces
void strip_code(char* code);
int brace_syntax_check(char* string);

int main(int argc, char** argv) {
	int j;
	char c;
	char* output;
	FILE* file;
	
	// proccessing the command-line arguments
	if (argc < 2) {
		printf("Error: no file specified!\n");
		return -1;
	}
	if (argc > 2) {
		printf("Error: too many arguments!\n");
		return -1;
	}

	// opening the file
	if ((file = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "Error: file couldn't be opened!\n");
		return -1;
	}
	
	// this section of code just gets the length of the file and
	// copies the contents of the file into an array
	j = 0;
	while (fgetc(file) != EOF) j++;
	rewind(file);
	output = (char*)malloc(++j*sizeof(char));
	j = 0;
	while((c = fgetc(file)) != EOF) output[j++] = c;
	output[j] = '\0';
	fclose(file);
	
	strip_code(output);
	if (brace_syntax_check(output))
		printf("No syntax errors detected!\n");
	else
		printf("Some syntax errors were detected!\n");

	free(output);
	return 0;
}

void syntax_error_exit() {
	fprintf(stderr, "Syntax incorrect!!!\n");
	exit(-1);
}

int brace_syntax_check(char* string) {
	int i,j, k;	
	int len;
	int* right_braces;

	len = strlen(string);
	j = 0;
	right_braces = (int*)malloc(len*sizeof(int));
	
	for (i = 0; i < len; i++)
		if (string[i] == '{' || string[i] == '[' || string[i] == '(')
			right_braces[j++] = i;
	j--;
	for (k = j; k >= 0; k--)
		for (i = right_braces[k]+1; i <  len; i++)
			if (string[i] == '}' || string[i] == ']'
					|| string[i] == ')') {
				if (match_left(string[right_braces[k]]) == string[i]) {
					string[i] = 'x';
					break;
				}
				else {
					free(right_braces);
					return 0;
				}
			}
	free(right_braces);
	return 1;
}

char match_left(char a) {
	if (a == '{')
		return '}';
	else if (a == '(')
		return ')';
	else if (a == '[')
		return ']';
	else
		return '\0';
}

// the src should be a null-terminated string
void remove_comments(char* string) {
	int i, j;
	char* tmp;
	char prev_char;
	int len, is_scomment, is_mcomment;

	prev_char = '\0';
	is_scomment = is_mcomment = 0;
	len = strlen(string);

	// j will be used to inded through tmp
	j = 0;
	tmp = (char*)malloc((len+1)*sizeof(char));
	memset(tmp, 0, (len+1)*sizeof(char));

	for (i = 0; i < len; i++) {
		if (string[i] == '/' && prev_char == '/' && NOT_COMMENT) {
			is_scomment = 1;
			tmp[--j] = '\0';
		}
		else if (string[i] == '\n' && is_scomment) {
			tmp[j++] = '\n';
			is_scomment = 0;
		}
		else if (string[i] == '/' && prev_char == '*')
			is_scomment = is_mcomment = 0;
		else if (string[i] == '*' && prev_char == '/') {
			is_mcomment = 1;
			tmp[--j] = '\0';
		}
		else if (!is_scomment && !is_mcomment)
			tmp[j++] = string[i];
		
		prev_char = string[i];
	}

	tmp[j++] = '\0';
	strncpy(string, tmp, j);
	free(tmp);
}
			
void remove_strings(char* code) {
	int i, j;
	char* tmp;
	char prev_char;
	int len, is_dquote, is_squote;
	
	is_dquote = is_squote = prev_char = 0;
	len = strlen(code);

	// j will be used to index through tmp
	j = 0;
	tmp = (char*)malloc((len+1)*sizeof(char));
	memset(tmp, 0, (len+1)*sizeof(char));
	
	for (i = 0; i < len; i++) {
		// I used the actual ascii codes here because it wasn't
		// working with escaped characters for some reason
		//
		// this section removes everything in double quotes
		if (code[i] == 34 && prev_char != 92 && !is_squote)
			is_dquote = !is_dquote;
		// this removes everything in single quotes
		else if (code[i] == 39 && prev_char != 92 
				&& is_squote && !is_dquote)
			is_squote = 0;
		else if (code[i] == 39 && prev_char != 92 
				&& !is_squote && !is_dquote)
			is_squote = 1;
		else if (NOT_QUOTE)
			tmp[j++] = code[i];
		
		prev_char = code[i];
	}
	tmp[j++] = '\0';

	strncpy(code, tmp, j);
	free(tmp);
}

void strip_code(char* code) {
	int i, j, len;
	char* tmp;

	remove_comments(code);
	remove_strings(code);
	
	len = strlen(code);
	
	j = 0;
	tmp = (char*)malloc((len+1)*sizeof(char));
	memset(tmp, 0, (len+1)*sizeof(char));
	
	for (i = 0; i < len; i++)
		if (code[i] == '{' || code[i] == '}'
				|| code[i] == '(' || code[i] == ')'
				|| code[i] == '[' || code[i] == ']')
			tmp[j++] = code[i];

	tmp[j++] = '\0';
	strncpy(code, tmp, j);
	free(tmp);
}

			









