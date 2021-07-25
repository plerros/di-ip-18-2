#include <stdio.h>
//to show debug printf's gcc ... -DDEBUG
#define DEBUG
//======================================================================================================================================================
#ifdef DEBUG
void debug_tabulate(int parenthesiscount)
{
	for (int i = 0; i < parenthesiscount; i++)
		printf("\t");
}

#else /* DEBUG */
void debug_tabulate(int parenthesiscount)
{
}

#endif /* DEBUG */

//======================================================================================================================================================
int is_operator(int c)
{
	return(c == '+' || c == '-' || c == '*');
}

int is_number(int c)
{
	return(c >= '0' && c <= '9');
}

int is_sign(int c)
{
	return(c == '+' || c == '-');
}

int is_space(int c)
{
	return(c == ' ' || c == '\t');
}

int is_end(int c)
{
	return(c == '\n' || c == ')');
}

int is_any(int c)
{
	return(is_operator(c) || is_number(c) || is_space(c) || is_end(c) || c == '(');
}

//======================================================================================================================================================
int read(int *error)
{
	int c = getchar();
	if (c == EOF)
		*error = 5;
	else if (!is_any(c))
		*error = 1;
	return c;
}

//======================================================================================================================================================
void handle_error(int *error, int *parenthesiscount) {
	switch (*error)
	{
		case 0:	//error 0 == no error
			break;
		case 1: //error 1 == invalid input
			printf("error: Valid characters are: {+, -, *, 1..9, comma, space, tab, newline}.\n");
			break;
		case 2:	//error 2 == missing operator
			printf("error: One operator between a pair of numbers, or between a number and a parenthesis, is needed.\n");
			break;
		case 3:	//error 3 == missing number
			printf("error: A number or parenthesis_open is expected after an operator or a sign.\n");
			break;
		case 4:	//error 4 == empty read_expression initialization
			printf("error: Empty input ' ' or '( )' isn't a valid expression.\n");
			break;
		case 5: //error 5 == no error just EOF
			printf("\n");
			break;
		case 6:	//error 6 == too many signs
			printf("error: There can be at most one sign per number\n");
			break;
		case 7:	//error :more '(' than ')'
			printf("error: Missing ')'.\n");
			break;
		case 8:	//error :less '(' than ')'
			printf("error: Missing '('.\n");
			break;
	}
}

//======================================================================================================================================================
int spaceskip(int *error)
{
	int current;
	do {
		current = read(error);
		if (*error)
			return 0;
	} while (is_space(current));
	return current;
}

//======================================================================================================================================================
int get_operator(int *error) {
	int current = spaceskip(error);
	if (*error)
		return 0;

	if (!is_operator(current) && !is_end(current))
		*error = 2;
	else if (is_end(current))
		ungetc(current, stdin);
	else if (is_operator(current))
		return current;

	return 0;
}

//======================================================================================================================================================
int read_unumber(int *error)
{
	int current = spaceskip(error);
	if (*error)
		return 0;

	if (!is_number(current)) {
		*error = 2;
		return 0;
	}

	int number = 0;
	while (is_number(current)) {
		number *= 10;
		number += current - '0';
		current = read(error);
		if (*error)
			return 0;
	}
	ungetc(current, stdin);
	return number;
}

int read_snumber(int *error, int *parenthesiscount)
{
	int current = spaceskip(error);
	int sign = 1;
	if (is_sign(current)) {
		if (current == '-')
			sign = -1;
		current = spaceskip(error);
		if (*error)
			return 0;

		if (is_end(current)) {
			ungetc(current, stdin);
			*error = 3;
			return 0;
		}
		else if (is_sign(current)) {
			*error = 6;
			return 0;
		}
	}
	ungetc(current, stdin);
	int number = read_unumber(error);
	if (*error)
		return 0;

	number *= sign;

#ifdef DEBUG
	debug_tabulate(*parenthesiscount);
	printf("number is: %d\n", number);
#endif
	return number;
}

int read_expression(int *error, int *parenthesiscount);
int read_factor(int *error, int *parenthesiscount)
{
	int number;
	int current = spaceskip(error);
	if (current == '(') {
#ifdef DEBUG
	debug_tabulate(*parenthesiscount);
	printf("(\n");
#endif
		++(*parenthesiscount);
		number = read_expression(error, parenthesiscount);
		if (*error)
			return 0;

		current = read(error);
		if (*error)
			return 0;

		if (current == ')') {
			--(*parenthesiscount);
		} else {
			ungetc(current, stdin);
			*error = 7;
			return 0;
		}
#ifdef DEBUG
	debug_tabulate(*parenthesiscount);
	printf(")\n");
#endif
	} else {
		ungetc(current, stdin);
		number = read_snumber(error, parenthesiscount);
	}
	return number;
}

int read_term(int *error, int *parenthesiscount)
{
	int number = read_factor(error, parenthesiscount);
	if (*error)
		return 0;

	int current = spaceskip(error);
	if (*error)
		return 0;

	if (current == '*') {
		number *= read_term(error, parenthesiscount);
		if (*error)
			return 0;
	} else {
		ungetc(current, stdin);
	}
	return number;
}
//======================================================================================================================================================
int read_expression(int *error, int *parenthesiscount)
{
	int prevnumber = 0;
	int operator = '+';
	int number, operatornew, current = spaceskip(error);
	if (*error)
		return 0;
#ifdef DEBUG
													debug_tabulate(*parenthesiscount);  printf("-  -  -  -  -  -  -  -  -  \n");
													debug_tabulate(*parenthesiscount);  printf("initial prevnumber is: %d\n", prevnumber);
													debug_tabulate(*parenthesiscount);  printf("operator is: %c\n", operator);
#endif
	if (is_end(current)) {
		ungetc(current, stdin);
		*error = 4;
		return 0;
	}

	while (!is_end(current)) {
		ungetc(current, stdin);
		number = read_term(error, parenthesiscount);
		if (*error)
			return 0;

		operatornew = get_operator(error);
			if (*error)
				return 0;
#ifdef DEBUG
													debug_tabulate(*parenthesiscount); printf("the operatornew is: %c\n", operatornew);
#endif
		if (operator == '+') {
			prevnumber += number;
		}
		else if (operator == '-') {
			prevnumber -= number;
		}
		operator = operatornew;
#ifdef DEBUG
													debug_tabulate(*parenthesiscount); printf("-----------------------------\n");
													debug_tabulate(*parenthesiscount); printf("prevnumber is: %d\n", prevnumber);
													debug_tabulate(*parenthesiscount); printf("newoperator is: %c\n", operator);
#endif
		current = spaceskip(error);
		if (*error)
			return 0;
	}
	if (is_end(current))
		ungetc(current, stdin);
	if (operatornew != 0)
		*error = 3;
	return prevnumber;
}
//======================================================================================================================================================
int main()
{
	int error = 0;

	for (int counter = 1; error != 5; counter++) {
		error = 0;
		int parenthesiscount = 0;
		int result = read_expression(&error, &parenthesiscount);

		int current = 0;
		if (error) {
			handle_error(&error, &parenthesiscount);
		} else {
			current = read(&error);
			if (current == ')') {
				error = 8;
				handle_error(&error, &parenthesiscount);
			} else {
				printf("Result %d: ", counter);
				printf("%d\n", result);
			}
		}
#ifdef DEBUG
													printf("Remaining stdin:[");
#endif
		while (current != '\n' && error != 5) {
			current = read(&error);
#ifdef DEBUG
												if (current != '\n')
													printf("%c", current);
#endif
		}
#ifdef DEBUG
													printf("]\n");
#endif
	}
	return 0;
}
