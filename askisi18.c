#include <stdio.h>
//to show debug printf's gcc ... -DDEBUG
//#define DEBUG
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
			if (*parenthesiscount > 0)
				printf("error: Missing ')'.\n");	//error :more '(' than ')'
			else if (*parenthesiscount < 0)
				printf("error: Missing '('.\n");	//error :less '(' than ')'
		case 1: //error 1 == invalid input
			printf("error: Valid characters are: {+, -, *, 1..9, comma, space, tab, newline}.\n");
			break;
		case 2:	//error 2 == missing operator
			printf("error: One operator between a pair of numbers, or between a number and a parenthesis, is needed.\n");
			break;
		case 3:	//error 3 == missing number
			printf("error: A number or parenthesis_open is expected after an operator or a sign.\n");
			break;
		case 4:	//error 4 == empty calcline initialization
			printf("error: Empty input ' ' or '( )' isn't a valid expression.\n");
			break;
		case 5: //error 5 == no error just EOF
			printf("\n");
			break;
		case 6:	//error 6 == too many signs
			printf("error: There can be at most one sign per number\n");
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
		return(current);

	return 0;
}
//======================================================================================================================================================
int calcline(int prevnumber, int operator, int *error, int *parenthesiscount);
int get_number(int current, int *error, int *parenthesiscount)
{
	int number;
	if (current == '(') {
#ifdef DEBUG
													for (int i = 0;i<*parenthesiscount;i++) {printf("\t");}printf("(\n");
#endif
		++(*parenthesiscount);
		number = calcline(0, '+', error, parenthesiscount);
		if (*error)
			return 0;
#ifdef DEBUG
													for (int i = 0;i<*parenthesiscount;i++) {printf("\t");}printf(")\n");
#endif
	} else {
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
		else if (!is_number(current)) {
			*error = 2;
			return 0;
		}
		for (number = 0;is_number(current);current = read(error)) {
			if (*error)
				return 0;
			number *= 10;
			number += current - '0';
		}
		number *= sign;
		ungetc(current, stdin);
	}
	return number;
}
//======================================================================================================================================================
int calcline(int prevnumber, int operator, int *error, int *parenthesiscount)
{
	int number, operatornew, current = spaceskip(error);
	if (*error)
		return 0;
#ifdef DEBUG
													for (int i = 0;i<*parenthesiscount;i++) {printf("\t");} printf("-  -  -  -  -  -  -  -  -  \n");for (int i = 0;i<*parenthesiscount;i++) {printf("\t");} printf("initial prevnumber is: %d\noperator is: %c\n", prevnumber, operator);
#endif
	if (is_end(current)) {
		ungetc(current, stdin);
		*error = 4;
		return 0;
	}

	while (!is_end(current)) {
		number = get_number(current, error, parenthesiscount);
		if (*error)
			return 0;
#ifdef DEBUG
													for (int i = 0;i<*parenthesiscount;i++) {printf("\t");}printf("number is: %d\n", number);
#endif
		operatornew = get_operator(error);
			if (*error)
				return 0;
#ifdef DEBUG
													for (int i = 0;i<*parenthesiscount;i++) {printf("\t");}printf("the operatornew is: %c\n", operatornew);
#endif
		if (operatornew == '*' && current != '\n') {
			number = calcline(number, '*', error, parenthesiscount);
			operatornew = get_operator(error);
			if (*error)
				return 0;
#ifdef DEBUG
													for (int i = 0;i<*parenthesiscount;i++) {printf("\t");}printf("multnumber is: %d\nnewtoperator is: %c\n", number, operatornew);
#endif
		}
		if (operator == '+') {
			prevnumber += number;
		}
		else if (operator == '-') {
			prevnumber -= number;
		}
		else if (operator == '*') {
			if (operatornew)
				ungetc(operatornew, stdin);
			return (prevnumber * number);
		}
		operator = operatornew;
#ifdef DEBUG
													for (int i = 0;i<*parenthesiscount;i++) {printf("\t");}printf("-----------------------------\n");for (int i = 0;i<*parenthesiscount;i++) {printf("\t");}printf("prevnumber is: %d\nnewoperator is: %c\n", prevnumber, operator);
#endif
		current = spaceskip(error);
		if (*error)
			return 0;
	}
	if (current == ')')
		--(*parenthesiscount);
	if (current == '\n')
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
		int result = calcline(0, '+', &error, &parenthesiscount);
		printf("Result %d: ", counter);
		if (error || parenthesiscount)
			handle_error(&error, &parenthesiscount);
		else
			printf("%d\n", result);
#ifdef DEBUG
													printf("Remaining stdin:[%c", current);
#endif
		int current = 0;
		while (current != '\n' && error != 5) {
			current = read(&error);
#ifdef DEBUG
													printf("%c", current);
#endif
		}
#ifdef DEBUG
													printf("]\n");
#endif
	}
	return 0;
}
