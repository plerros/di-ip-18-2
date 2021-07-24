#include <stdio.h>
//to show debug printf's gcc ... -DDEBUG
//======================================================================================================================================================
int operatorchk(int current)
{
	if (current == '+' || current == '-' || current == '*')
		return 1;
	else
		return (0);
}
int numberchk(int current)
{
	if (current >= '0' && current <= '9')
		return 1;
	else
		return (0);
}
int signchk(int current)
{
	if (current == '+' || current == '-')
		return 1;
	else
		return (0);
}
int spacechk(int current)
{
	if (current == ' ' || current == '\t')
		return 1;
	else
		return (0);
}
int endchk(int current)
{
	if (current == '\n'|| current == ')')
		return 1;
	else
		return (0);
}

//======================================================================================================================================================
int input(int *error) {
	int in = getchar();
	if (in == EOF) {
		*error = 5;
		return 0;
	}
	else if (!operatorchk(in) && !numberchk(in) && !spacechk(in) && !endchk(in) && in != '(') {
		*error = 1;
	}
	return(in);
}
//======================================================================================================================================================
int handlerror(int *error, int *parenthesiscount) {
	if (*error) {																	//error 0 == no error
		if (*error == 1) {
			printf("error: Valid characters are: {+,-,*,1..9,(,),space,tab,newline}.\n");
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
		return (current);
	else
		return (0);
}
int signn(int sign, int number)
{
	if (sign)
		return (number * (44 - sign));
	else
		return (number);
}

//======================================================================================================================================================
int spaceskip(int current, int *error)
{
	for (; spacechk(current); current = input(error)) {
		if (*error)
			return (0);
	}
	return (current);
}
//======================================================================================================================================================
int get_operator(int *error) {
	int current = spaceskip(input(error), error);
	if (*error) {
		return (0);
	}
	else if (!operatorchk(current) && !endchk(current)) {
		*error = 2;
		return 0;
	}
	else if (endchk(current)) {
		ungetc(current, stdin);
		return (0);
	}
	else if (operatorchk(current)) {
		return(current);
	}
	return (0);
}
//======================================================================================================================================================
int calcline(int prevnumber, int operator, int *error, int *parenthesiscount);
int get_number(int current, int *error, int *parenthesiscount)
{
	int number;
	if (current == '(') {
#ifdef DEBUG
													for (i=0;i<*parenthesiscount;i++) {printf("\t");}printf("(\n");
#endif
		++(*parenthesiscount);
		number = calcline(0, '+', error, parenthesiscount);
		if (*error)
			return (0);
#ifdef DEBUG
													for (i=0;i<*parenthesiscount;i++) {printf("\t");}printf(")\n");
#endif
	}
	else{
		int sign = signadd(current);
		if (sign) {
			current = spaceskip(input(error), error);
			if (*error) {
				return (0);
			}
			else if (current == '\n' || current == ')') {
				ungetc(current, stdin);
				*error = 3;
				return (0);
			}
			else if (signchk(current)) {
				*error = 6;
				return (0);
			}
		}
		else if (!numberchk(current)) {
			*error = 2;
			return (0);
		}
		for (number = 0;numberchk(current);current = input(error)) {
			if (*error)
				return (0);
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
		return (0);
#ifdef DEBUG
													int i;for (i=0;i<*parenthesiscount;i++) {printf("\t");} printf("-  -  -  -  -  -  -  -  -  \n");for (i=0;i<*parenthesiscount;i++) {printf("\t");} printf("initial prevnumber is: %d\noperator is: %c\n", prevnumber, operator);
#endif
	if (endchk(current)) {
		ungetc(current, stdin);
		*error = 4;
		return 0;
	}

	while (!endchk(current)) {
		number = get_number(current, error, parenthesiscount);
		if (*error)
			return (0);
#ifdef DEBUG
													for (i=0;i<*parenthesiscount;i++) {printf("\t");}printf("number is: %d\n", number);
#endif
		operatornew = get_operator(error);
			if (*error)
				return (0);
#ifdef DEBUG
													for (i=0;i<*parenthesiscount;i++) {printf("\t");}printf("the operatornew is: %c\n", operatornew);
#endif
		if (operatornew == '*' && current != '\n') {
			number = calcline(number,'*',error, parenthesiscount);
			operatornew = get_operator(error);
			if (*error)
				return (0);
#ifdef DEBUG
													for (i=0;i<*parenthesiscount;i++) {printf("\t");}printf("multnumber is: %d\nnewtoperator is: %c\n", number, operatornew);
#endif
		}
		if (operator == '+') {
			prevnumber += number;
		}
		else if (operator == '-') {
			prevnumber -= number;
		}
		else if (operator == '*') {
			if (operatornew) {
				ungetc(operatornew, stdin);
			}
			return (prevnumber * number);
		}
		operator = operatornew;
#ifdef DEBUG
													for (i=0;i<*parenthesiscount;i++) {printf("\t");}printf("-----------------------------\n");for (i=0;i<*parenthesiscount;i++) {printf("\t");}printf("prevnumber is: %d\nnewoperator is: %c\n", prevnumber, operator);
#endif
		current = spaceskip(input(error), error);
		if (*error)
			return (0);
	}
	if (current == ')') {
		--(*parenthesiscount);
	}
	if (current == '\n') {
		ungetc(current, stdin);
	}
	if (operatornew != 0) {
		*error = 3;
	}
	return (prevnumber);
}
//======================================================================================================================================================
int main()
{
	int current = 0, result, counter;
	int error = 0, parenthesiscount = 0;

	for (counter = 1; error != 5; counter++, error = 0, parenthesiscount = 0) {
		current = 0;
		result = calcline(0,'+', &error, &parenthesiscount);
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
		while (current != '\n') {
			current = input(&error);
#ifdef DEBUG
													printf("%c", current);
#endif
		}
#ifdef DEBUG
													printf("\b]\n");
#endif
	}
	return 0;
}
