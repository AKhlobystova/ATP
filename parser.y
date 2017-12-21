%defines 
/*Объявления*/

%{
/*Объявления C++*/
  //#define YYSTYPE string
  //#pragma warning (disable: 4996)
 
//  #include "parser.hpp"
  #include <iostream>
  #include <cstring>
  #include <cmath>
  #include <fstream>
  #include <list>
  #include <string>

  using namespace std;
  
  extern int yylex();
  extern ifstream *fin;
  extern void yyerror(const char* msg);

enum FORMULA_TYPE {
  FT_CNF,
  FT_FOF
};

enum OPERATION_TYPE {
 OP_ATOM,
 OP_NOT,
 OP_OR,
 OP_AND,
 OP_IMPL,
 OP_EQ
};

  class PropTree
  {
    public:
    PropTree *left, *right;
	string pred;
	list<string*> *args;
	OPERATION_TYPE operation;
    PropTree(OPERATION_TYPE op, PropTree *l=nullptr, PropTree *r=nullptr) :
        operation(op), left(l), right(r) {}
    PropTree(string pr, list<string*> *a) :
        operation(OP_ATOM), pred(pr), args(a), left(nullptr), right(nullptr) {}
    ~PropTree()
	{ 
	  delete left;
	  delete right;
	}
  };
  extern list<PropTree*> *ResultTree;
%}

%union
{
    PropTree *prop;
	list<PropTree*> *proplist;
	FORMULA_TYPE ftype;
	string *fname;
	string *fstatus;
	list<string*> *termlist;
	string *predsymbol;
	PropTree *pred;
	string *constsymbol;
	string *var;
};

%type<proplist> program
%type<prop> expression /* cnf(agatha,axiom,( lives(agatha) )).*/
%token<ftype> formula_type /*cnf*/ 
%token<fname> name /*agatha, different_hates*/
%token<fstatus> formula_status /*axiom,negated_conjecture*/
%type<prop> proposition/*lives(agatha)*/
%type<termlist> arguments/**/
%type<pred> predicate/*lives, killed, hates, richer*/
%token<predsymbol> predicate_symbol /*lives, killed, hates, richer*/
%token<constsymbol> constant /*agatha, butler, charles*/
%token<var> variable /*X1,X2,...  Y1,Y2,...*/ 

/*начало синтаксического дерева*/
%start program

/*Далее следуют правила грамматики и действия*/
%%
program: expression { $$ = new list<PropTree*>(1, $1); ResultTree=$$;}
/* program: expression { $$->push_back($1);} */
    | program expression {$1->push_back($2);}
	; 

expression: formula_type'('name','formula_status','proposition')''.'{$$=$7;}
	;

proposition: predicate
    | predicate '|' proposition  { $$ = new PropTree(OP_OR, $1, $3);} 
    /*| '~' proposition  { $$ = new PropTree(OP_NOT, $2, nullptr);}*/
 	| predicate '&' proposition  { $$ = new PropTree(OP_AND, $1, $3);}  
 	| predicate '=''>' proposition  { $$ = new PropTree(OP_IMPL, $1, $4);}  
 	| predicate '<''=''>' proposition  { $$ = new PropTree(OP_EQ, $1, $5);} 
   ;

predicate: predicate_symbol { $$ = new PropTree(*$1, nullptr);}
	 | predicate_symbol '(' arguments ')' { $$ = new PropTree(*$1, $3);}
     | '~' predicate_symbol { $$ = new PropTree(OP_NOT, new PropTree(*$2, nullptr), nullptr);}
	 ;

/*arguments: constant {$$ = new list<string>(1,$1);} */
/*     | variable {$$ = new list<string>(1,$1);}*/
arguments: constant {$$ = new list<string*>(1,$1);}
     | variable {$$ = new list<string*>(1,$1);}
     | constant ',' arguments {$3->push_back($1);}
	 | variable ',' arguments {$3->push_back($1);}
	 ;
%%

/*Дополнительный код на C*/
void yyerror(char const* msg)
{
   std::cout<<"Syntax Error: " << msg <<std::endl;
}

int yylex(void)
{
	static int f = 0;
	static char str[200]={};
	static char str2[200]={};
 	static int fl=0;
	static char * pch = nullptr, *tok = nullptr;
 	if (fl==0)
 	{
 	 fl++;
	 pch = nullptr, 
	 tok = nullptr;
      f=0;
 	}
	if (tok)
	{
	  if (strchr(" (),&~|<=>.", *tok))
	  {
	    char c = *tok;
		tok ++;
		  //cout << str << "________"<< c << endl;
		  if (c=='|') f--;
		  //cout<<f<<endl;
	   if (c=='.')
		{
		   fl=0;
		}
		return c;
	  }
	  if (str2[tok+strlen(tok)-str] != 0)
	  {
	    char c = str2[tok+strlen(tok)-str];
		tok += strlen(tok)+1;
		//cout << c << " " << *tok << endl;
		if (c=='|' || c==')') f--;
		return c;
	  }
	  tok = strtok_s (NULL," (),&|~<=>.",&pch);
	}
	if (!tok)
	{
	  if (!fin->eof())
	    fin->getline(str, 200);
      else return 0;
	  cout<<str<<endl;
	  memcpy(str2, str, 200);
	  f = 0;
  	  tok = strtok_s (str," (),&|~<=>.",&pch);
	}
  //  cout << tok << endl;
          if (strcmp(tok,"cnf")==0)
		    {
			  f++;
			  yylval.ftype=FT_CNF;
              return formula_type;
		    }
		   else if (strcmp(tok,"axiom")==0)
		    {
			  f++;
			  yylval.fstatus=new string("axiom");
              return formula_status;
		    }
			 else if (strcmp(tok,"negated_conjecture")==0)
		    {
			  f++;
			  yylval.fstatus=new string("negated_conjecture");
              return formula_status;
		    }
			else
			{
		  switch (f)
		       {
		         case 1:
		            {
					  yylval.fname=new string(tok);
                      return name;
		            }
		         case 2:
		            {
			            f++;
						 // cout<<*tok<<endl;
						 //if (strcmp(tok+1,"|")==0) f--;
						 //cout<<f<<endl;
						yylval.predsymbol=new string(tok);
                        return predicate_symbol;
		            }
				case 3:
		            {
					  if (isupper(pch[0]))
					   {
					    yylval.var=new string(tok);
                        return variable;
					   }
					  else 
					  {
					    yylval.constsymbol=new string(tok);
					    return constant;
					  }
		            }
		          default: 
		            {
		              cout<<"ERROR"<<endl;
		              exit(0);
				    }
			    }
			  }
	  /* pch = strtok_s (NULL," (),&|~<=>.");*/ 
}