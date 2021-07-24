#include <stdio.h>
//to show debug printf's gcc ... -DDEBUG
//#define DEBUG
//======================================================================================================================================================
int operatorchk(int current)
{
	return(current == '+' || current == '-' || current == '*');
}
int numberchk(int current)
{
	return(current >= '0' && current <= '9');
}
int signchk(int current)
{
	return(current == '+' || current == '-');
}
int spacechk(int current)
{
	return(current == ' ' || current == '\t');
}
int endchk(int current)
{
	return(current == '\n'|| current == ')');
}

//======================================================================================================================================================
int input(int *error) {
	int in = getchar();
	if (in == EOF)
		*error = 5;
	else if (!operatorchk(in) && !numberchk(in) && !spacechk(in) && !endchk(in) && in != '(')
		*error = 1;
	return(in);
}
//======================================================================================================================================================
int handlerror(int *error, int *parenthesiscount) {
	if (*error) {																	//error 0 == no error
		if (*error == 1) {
			printf("error: Valid characters are: {+, -, *, 1..9, comma, space, tab, newline}.\n");
		}					//error 1 == invalid input
		else if (*error == 2) {
			printf("error: One operator between a pair of numbers, or between a number and  parenthesis, is needed.\n");
		}	//error 2 == missing operator
		else if (*error == 3) {
			printf("error: A number or parenthesis open is expected after an operator or a sign.\n");
		}			//error 3 == missing number
		else if (*error == 4) {
			printf("error: Empty input ' ' or '( )' isn't a valid mathematical expression.\n");
		}					//error 4 == empty calcline initialization
		else if (*error == 5) {
			return 0;
		}													//error 5 == no error just EOF
		else if (*error == 6) {printf("error: There can be at most one sign per number\n");
		}							//error 6 == too many signs
		return 0;
	} else {
		if (*parenthesiscount > 0) {
			printf("error: Missing ')'.\n");
		}									//error :more '(' than ')'
		else if (*parenthesiscount < 0) {
			printf("error: Missing '('.\n");
		}									//error :less '(' than ')'
		return 0;
	}
}
//======================================================================================================================================================
int signadd(int current)
{
	if (signchk(current))
		return current;
	else
		return 0;
}
int signn(int sign, int number)
{
	if (sign)
		return (number * (44 - sign));
	else
		return number;
}

//======================================================================================================================================================
int spaceskip(int current, int *error)
{
	for (; spacechk(current); current = input(error)) {
		if (*error)
			return 0;
	}
	return current;
}
//======================================================================================================================================================
int get_operator(int *error) {
	int current = spaceskip(input(error), error);
	if (*error)
		return 0;

	if (!operatorchk(current) && !endchk(current))
		*error = 2;
	else if (endchk(current))
		ungetc(current, stdin);
	else if (operatorchk(current))
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
	}
	else{
		int sign = signadd(current);
		if (sign) {
			current = spaceskip(input(error), error);
			if (*error) {
				return 0;
			}
			else if (endchk(current)) {
				ungetc(current, stdin);
				*error = 3;
				return 0;
			}
			else if (signchk(current)) {
				*error = 6;
				return 0;
			}
		}
		else if (!numberchk(current)) {
			*error = 2;
			return 0;
		}
		for (number = 0;numberchk(current);current = input(error)) {
			if (*error)
				return 0;
			number *= 10;
			number += current - '0';
		}
		number = signn(sign, number);
		ungetc(current, stdin);
	}
	return number;
}
//======================================================================================================================================================
int calcline(int prevnumber, int operator, int *error, int *parenthesiscount)
{
	int number, operatornew, current = spaceskip(input(error), error);
	if (*error)
		return 0;
#ifdef DEBUG
													for (int i = 0;i<*parenthesiscount;i++) {printf("\t");} printf("-  -  -  -  -  -  -  -  -  \n");for (int i = 0;i<*parenthesiscount;i++) {printf("\t");} printf("initial prevnumber is: %d\noperator is: %c\n", prevnumber, operator);
#endif
	if (endchk(current)) {
		ungetc(current, stdin);
		*error = 4;
		return 0;
	}

	while (!endchk(current)) {
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
		current = spaceskip(input(error), error);
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
		if (error || parenthesiscount) {
			handlerror(&error, &parenthesiscount);
		}
		else{
			printf("%d\n", result);
		}
#ifdef DEBUG
													printf("Remaining stdin:[%c", current);
#endif
		int current = 0;
		while (current != '\n') {
			current = input(&error);
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
