#include <stdio.h>
//University assignment
/*

GRAMMAR

<expression> ::= <term> | <expression> ’+’ <term> | <expression> ’-’ <term>
<term> ::= <factor> | <factor> ’*’ <term>
<factor> ::= <snumber> | ’(’ <expression> ’)’
<snumber> ::= <unumber> | ’+’ <unumber> | ’-’ <unumber>
<unumber> ::= <digit> | <digit> <unumber>
<digit> ::= ’0’ | ’1’ | ’2’ | ’3’ | ’4’ | ’5’ | ’6’ | ’7’ | ’8’ | ’9’

*/

//Bibliography:Forum lists DIT,recursive descent parser wikipedia

//Prototypes
void initialize_counter();
int parseSet();//Example of parsing
int parseExpression(); //    _________
int parseTerm();// ____  ____
int parseFactor();// __ __ __ __ __
int parseNumber();

//global flags

//x is for current character
//error is for finding the correct result to print, based on the error that has occured
//counter is for printing the grammatical error of more than one sign before a number or a sign before a parentheses
int x,error,counter;

//The concept is to parse mathematical expressions into several fuctions,
//each fuction responsible for a small subproblem,in accordance to the grammar that is given.

//Fuction that handles the end of input
//This fuction also helps adressing error characters that 
//escape the parseFactor.
int parseSet()
{
	int result = parseExpression();//start parsing
	if(x=='\n')
	{
		return result;
	}
	else if(x >='0' && x <='9' && error==0 )	//By tests it was found that if x has ascii value of
	{							// character between 0 and 9 then the expression was missing operator(ex. 21 3+4)
		error=1;
	}
	else if(x == ')' && error==0 )
	{
	//By tests it was found that if  x has ascii value of
	// character ) then there wassn't opening parentheses 
	//ex.2+3)
		error=2;
	}
	else if(error==0||error==5)
	{
	//error character that escapes ex 123+á+3
	//  ||error==5 is fixing 1+(1(1+3)+5 
	//error character will occur before closing parentheses for sure
		error=3;
	}
}


//We know from grammar that expression is
//<term> | <expression> ’+’ <term> | <expression> ’-’ <term>.
//An expression is consisted of products.
//The fuction is processing consecutive terms.For example
//2+3-5 ->operands : term1=2 term2=3|operator:'+'->operands :term1=5 term2=5|operator:'-'-> term1=result: 0
//That is implemented below.
int parseExpression()
{

	initialize_counter();//initialization is for case +2+(2+3) and when we dont want to be printed more than one sign message
	int term1 = parseTerm();//get term 1
	while(x == '+' || x == '-'|| x== ' ')
	{
		if(x == '+')//Case of addition
		{
			x=getchar();//get term 2
			initialize_counter();//initialize counters
			int term2 = parseTerm();//Process new term
			term1 = term1 + term2;//Calculate them
		}
		else if(x == '-')//Case of substraction
		{
			x=getchar();
			initialize_counter();
			int term2 = parseTerm();
			term1 = term1 - term2;
		}
		else if(x == ' ')//Ignoring whitespace characters
		{
			x=getchar();
		}	
		if(error!=0)//Dont read more characters after error has occured
			break;
	}
	return term1;//return the result
}
////We know from grammar that term is
// <factor> | <factor> ’*’ <term>
//A term is consisted of factors.
//The fuction is processing consecutive factors.For example 2*3*4 or 2*3+3*4
int parseTerm()
{
	initialize_counter();
	int fac1 = parseFactor();//get factor 1
	while(x =='*' || x == ' ')
	{
		if(x == '*')//Prepare for multiplication
		{
			x=getchar();//get factor 2
			initialize_counter();
			int fac2 = parseFactor();//Process factor 2
			fac1 = fac1 * fac2;//Do the multiplication
		}
		else if(x == ' ')//Ignoring whitespace characters
		{
			x=getchar();
		} 
		if(error!=0)//Dont read more characters after error has occured
			break;
	}
	return fac1;//Return the result
}

void initialize_counter()//initialize counter.They are explained below.
{
	counter=0;
}

//Implementation of factor
// <snumber> | ’(’ <expression> ’)’
//There we process the factor and do error checking 
int parseFactor()
{
	
	while(x == ' ')//Ignoring whitespace characters
	{
		x = getchar();
	}
	if(x >= '0' && x <='9')
	//If we find digit we calculate the number
	//that represents the factor
	{
		return parseNumber();
	}
	else if(x == '(')
	//Case of factor is a parethesized expression
	//There the recursion is implemented.For example 2*(3+4)
	//Pro1=2 Pro2 (parethesized) 7 but parseSum has called parseSum to calculate the expression
	{
		
		if(counter>0 && error==0)//Error handling when before parentheses there is a sign
		{
		
			error=4;
			while(x!='\n')
			//Ignoring everything after the error because we don't care
			//about that and find the new line character that represents the end of the expression
			{
				x=getchar();
			}
		}
		else
		{
			x=getchar(); //consume (
			int expression = parseExpression();//Recursion
		
			if(x == ')')
			{
				x=getchar(); //consume )
				return expression;
			}
			else
			{
			//Error handling when there is not closing parentheses
				if(error==0)
				{
					error=5;
				}
			}
		}
	}
	//(Counted 2 number of - and 0 of +)
	else if(x == '-' && counter < 2  && error==0)
	//Handle negative numbers
	//Simultaneously,we prevent unacceptable number of signs example +++,-- 
	{
		counter++;
		
		if(counter==2 )//Prevent more than one sign situations
		{
			error=6;//Flag to print the desired message (more than one sign)
		}
		if(error!=6)//If we have only one sign
		{
			x=getchar();//get number
			return (parseFactor()*(-1));//return the negative number.Recursive call:parse factor calls parse factor
		}
	}
	else if(x == '+' && counter < 2 && error==0)
	//Same applies to + sign
	{
		counter++;
		
		if(counter==2)
		{
			error=6;
		}
		if(error!=6)
		{
			x=getchar();
			return parseFactor();
		}	
	
	}
	else if(x=='\n' && error==0)
	//If  parsefactor finds change of line then 
	//we know we have not correct end of input like 23+7-, 23 *
	{
		error=7;
	}
	else if(error==0||error==5)
	{
		error=3;//Handle cases when there is not patricular pattern of mistake like 125+a+1 situation or a123  or a++
	//	||error==5 is fixing 1+(1(1+3)+5 where 
	//the first grammatical error is the absence of operator before parentheses
	}
	
}

int parseNumber()
//Creating  the number from a series of characters
//The fuction can handle multi digit numbers
{
	int number=0;
	
	while(x >= '0' && x <= '9')
	{
		number = number * 10 + x - '0';
		x=getchar();
	}
	
	return number;
}

int main()
{
	int i=1;//counter of results
	while(x!=EOF)//End of the procedure if we find end of file
	{
		error=0;//initialize error indicator
		x=getchar();//get first character
		int result=parseSet();//start process
		if(error==0)//error=0 code for normal result.Process found no errors 
		{
			printf("Result %d : %d \n",i,result);
			i++;
		}	
		else if(error==1)//error=1 code for no operator
		{
			printf("Result %d : No operator  \n",i);
			i++;
		}
		else if(error==2)//error=2 code for missing opening parentheses
		{
			printf("Result %d : Missing opening parentheses '(' \n",i);
			i++;
		}
		else if(error==3)//error=3,error=8  code for unexpected character
		{	
			if(x!='\n')
			{
				printf("Result %d : Unexpected character  %c \n",i,x);
				i++;
			}
			else // case 2+()
			{
				printf("Result %d : Unexpected character \n",i);
				i++;
			}
		}
		else if(error==4)//error=4 code for sign before a paretheses
		{
			printf("Result %d : A sign before a parentheses \n",i);
			i++;
		}
		else if(error==5)//error=5 code for missing closing parentheses
		{
			printf("Result %d : Missing closing parentheses ')' \n",i);
			i++;
		}
		else if(error==6)//error=6 code for more than one sign before a number
		{
			printf("Result %d : More than one sign \n",i);
			i++;
		}
		else //error=7 code for unexpected end of input
		{
			printf("Result %d : Unexpected end of input  \n",i);
			i++;
		}
		
		while(x!='\n')//Ignore other characters after we find the error and print the right message
		{
			x=getchar();
		}
	}
}
