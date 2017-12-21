#include"stdafx.h"
//#include"parser.hpp"
#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include <iterator>
#include <functional>
#include <set>
using namespace std;
string unify(list<string> &t1, list<string> &t2) //мб в будущем будет использоваться
{
	string result;
	if (t1.size()==1 && t2.size() == 1)
	{
		if (t1.front() == t2.front()) return"#";
		else
		{//все переменные начинаются с большой буквы, константы с маленькой
			if (isupper((t1.front())[0])) return(t1.front() + "#"+t2.front()); //если t1 переменная
			if (isupper((t2.front())[0])) return(t2.front() + "#" + t1.front()); //если t2 переменная
			else return("fail");
		}
	}
	list<string> f1{ t1.front() };
	list<string> f2{ t2.front() };
	t1.pop_front();
	t2.pop_front();
	string z1,z2;
	z1 = unify(f1,f2);
	if (z1 == "fail") return z1;	
	result = result+ " " + z1;
	z2 = unify(t1, t2);
	if (z2 == "fail") return"fail";
	return result + " " +z2;//в итоге строка из требуемых подстановок, хотя не очень удобно
}
ifstream *fin;
int yyparse(void);
class PropTree
{
 public:
	PropTree *left, *right;
	string pred;
	list<string*> *args;
	OPERATION_TYPE operation;
	PropTree(OPERATION_TYPE op, PropTree *l = nullptr, PropTree *r = nullptr) :
		operation(op), left(l), right(r) {}
	PropTree(string pr, list<string*> *a) :
		operation(OP_ATOM), pred(pr), args(a), left(nullptr), right(nullptr) {}
	~PropTree()
	{
		delete left;
		delete right;
	}
};
list<string> NList(PropTree* r)
{
	list<string> argum;
	list<string*> *arg = r->args;
	int i = 0;
	int k = (*arg).size();
	while (true)
	{
		cout << i << endl;
		if (i == k) break;
		argum.push_back(*(*arg).front());
		(*arg).pop_front();
		i++;
	}
	return argum;
};
list<PropTree*> *ResultTree;
list<PropTree*> NormalTree;
void print(PropTree* r, int offset = 0)
{
	if (r == nullptr) return;
	print(r->right, offset + 3);
	for (int i = 0; i<offset; i++)
		cout << " ";
	cout << r->operation << "  " << r->pred << endl;
	print(r->left, offset + 3);
}
void printNormalTree(int offset = 0)
{
	list <PropTree*>::iterator it;
	int i = 1;
	for (it = NormalTree.begin(); it != NormalTree.end(); it++)
	{
		cout << "_____________________________________________________________" << endl;
		cout << i <<": " << endl;
		i++;
		PropTree* r;
		r = *it;
		if (r == nullptr) return;
		print(r, offset + 3);
	}
	cout << "____________________________________________________________________________________" << endl;
}
PropTree* check_left(PropTree* r, bool not, string pred_symb)
{
  if (not) //проверяем, что это искомый предикатный символ с отрицанием
  {
	  if (r != nullptr && r->left != nullptr && r->left->left!= nullptr && (r->left)->operation == OP_NOT && ((r->left)->left)->pred == pred_symb)
	  {
		  return (r->left)->left;
	  }
	  else
		  if (r != nullptr && r->left != nullptr && r->operation == OP_NOT && (r->left)->pred == pred_symb)
		  {
			  return r->left;
		  }
  }
  else
  {
	  if (r != nullptr && r->operation!= OP_NOT && r->left != nullptr && (r->left)->operation == OP_ATOM && (r->left)->pred == pred_symb)
	  {
		  return (r->left);
	  }
	  else
		 if (r != nullptr && r->left == nullptr && r->operation == OP_ATOM && r->pred == pred_symb)
		  {
			  return r;
		  }
  }
  return nullptr;
}
PropTree* check_right(PropTree* r, bool not, string pred_symb)
{
	if (r!=nullptr && r->operation != OP_NOT && r->operation != OP_ATOM)
	{
		if (r->left!=nullptr && check_left(r->left, not, pred_symb) != nullptr)
		{
			return check_left(r->left, not, pred_symb);
		}
		if (r->right != nullptr && check_right(r->right, not, pred_symb) != nullptr)
		{
			return check_right(r->right, not, pred_symb);
		}
	}
	if (not) //проверяем, что это искомый предикатный символ с отрицанием
	{
		if (r!=nullptr && r->right!=nullptr && (r->right)->operation == OP_NOT && ((r->right)->left)->pred == pred_symb)
		{
			return (r->right)->left;
		}
	}
	else
	{
		if (r != nullptr && r->operation == OP_ATOM && r->pred == pred_symb)
		{
			return r;
		}
		if (r != nullptr && r->left != nullptr && (r->left)->operation == OP_ATOM && (r->left)->pred == pred_symb)
		{
			return (r->left);
		}
		if (r != nullptr && r->right != nullptr && (r->left)->operation == OP_ATOM && r->right->pred == pred_symb)
		{
			return r->right;
		}
	}
	return nullptr;
}
PropTree* Find(bool not, string pred_symb)
{
	list <PropTree*>::iterator it;
	set<int> static V;
	it = NormalTree.begin();
	int k = 1;
	set<int>::iterator j;
	int i = 1;
	while(true)
	{
		i++;
		++it;
		PropTree* r;
		r = *it;
		//not == false означает, что ищем формулу без отрицания
		k = 1;
		for (j = V.begin(); j != V.end(); j++)
		{
			//cout << *j << "    "<< i <<endl;
			if (*j == i ) k = 0;
		}
		if (k != 0)
		{
			if (r->operation == OP_ATOM && r->pred == pred_symb && !not)
			{
				cout << i << ":       " << pred_symb << endl;
				//return r;
				V.insert(i);
				return *it;
			}
			else
			{
				if (check_left(r, not, pred_symb) != nullptr)
				{
					cout << i << ":       " << pred_symb << endl;
					//return check_left(r, not, pred_symb);
					V.insert(i);
					return *it;
				}
				else
				{
					if (check_right(r, not, pred_symb) != nullptr)
					{
						cout << i << ":       " << pred_symb << endl;
						//return check_right(r, not, pred_symb);
						V.insert(i);
						return *it;
					}
				}
			}
		}
		if (i == NormalTree.size())
		{
			cout << "The empty clause cannot be derived" << endl;
			exit(0);
		}
	}
}
void Delete_pred_symbol(PropTree* &r)
{
	if (r!=nullptr && r->left != nullptr)
	{
		if (r->right == nullptr)
		{
			//delete r->left;
			r = nullptr;
		}
		else
		{
			r = r->right;
		}
	}
	else r = nullptr;
}
void Add_pred_symbols(PropTree* &m, PropTree* t)
{
	PropTree* r = m;
	//cout << "___________________-" << endl;
	//print(r);
	//cout << "___________________-" << endl;
	//print(t);
	//cout << "___________________-" << endl;

	while(true)
	{
		if (r==nullptr || r->right == nullptr || r->right->operation == OP_ATOM || r->right->operation == OP_NOT)
		{
			break;
		}
		else
		{
			r = r->right;
		}
	}
		PropTree* g;
		if (r != nullptr &&  r->right != nullptr)
		g = (r->right);
		else g = r;
		string s;
		if (g->operation == OP_ATOM)
		{
			s = g->pred;
			if (r->right != nullptr)
			r->right = new PropTree(OP_OR, new PropTree(s, nullptr), t);
			else m = new PropTree(OP_OR, new PropTree(s, nullptr), t);
		}
		else if (g->operation == OP_NOT)
		{
			s = g->left->pred;
			if (r->right != nullptr)
			r->right = new PropTree(OP_OR, new PropTree(OP_NOT, new PropTree(s, nullptr), nullptr), t);
			else m = new PropTree(OP_OR, new PropTree(OP_NOT, new PropTree(s, nullptr), nullptr), t);
		}
}
void Remove_pred_symbols(PropTree* &m,bool not, string s)
{

	PropTree* r = m;
	PropTree* p = r;
	if (r!=nullptr && ((r->operation == OP_ATOM && r->pred==s) || (r->operation == OP_NOT && r->left->pred == s)))
	{
		if (r->operation == OP_ATOM)
		m->pred="#";
		else m->left->pred = "#";
	}
	while (true)
	{
		if (r == nullptr) break;
		if (check_right(r, not, s) == nullptr && check_left(r, not, s) == nullptr)
		{
			//cout << "1  " << r ->operation <<endl;
			break;
		}
		if (r != nullptr && r->right != nullptr)
		{
			//cout << "2  " << r->operation << endl;
			if ((r->right->operation == OP_ATOM && r->right->pred == s) || 
				(r->right->operation == OP_NOT && r->right->left->pred == s))
			{
				 //cout << "3  " << r->operation << endl;
				if (p->right!=nullptr && p->right == r)
				{
					p->right = r->left;
					r = p;
				}
				else
				{
					if (r->left != nullptr)
					{
						PropTree* g;
						//cout << "*******" << endl;
						//print(m);
						g = m->left;
						//delete m;
						m = nullptr;
						m = g;
						//print(m);
						break;
					}
				}
			}
			if (r != nullptr && r->right != nullptr && (r->right->operation != OP_ATOM || r->right->operation != OP_NOT))
			{
				if (check_left(r, not, s)!=nullptr)
				{
					//cout << "4  " << r->operation << endl;
					delete r->left;
					r->left = nullptr;
					p->right = r->right;
				}
			}
		}
		if (r->right != nullptr) {
			p = r;
			r = r->right;
		}
		if (r == nullptr) break;
	}
}
void Resolution()
{
	PropTree* r=NormalTree.front();
	bool not;
	 while(true)
	 {
		 string pred_s;
		 if (r->left == nullptr && r == nullptr)
		 {
			 cout << ">>>>>>Congratulations!<<<<<<<<" << endl;
			 break;
		 }
		 if (r->left != nullptr && r->left->operation == OP_NOT)
		 {
			 not = false;
			 pred_s=r->left->left->pred;
		 }
		 else
		 {
			 if (r->operation == OP_NOT)
			 {
				 not = false;
				 pred_s = r->left->pred;
			 }
			 else
			 {
				 if (r->left != nullptr)
				 {
					 not = true;
					 pred_s = r->left->pred;
				 }
				 else
				 {
					 not = false;
					 pred_s = r->pred;
				 }
			 }
		 }
		 //cout << "******" << endl;
		 PropTree* k = Find(not, pred_s);
		 cout << "**************************" << endl;
		 print(r);
		 cout << "^^^^^^^^^^^^^^^^^^^^^^" << endl;
		 print(k);
		 cout << "**************************" << endl;
		 Add_pred_symbols(r,k);
		 // cout << "////////////////" << endl;
		 //print(r);
		 // cout << "////////////" << endl;
		 Delete_pred_symbol(r);
		 // print(r);
		 //if (!not) cout << pred_s << "((((((((((((" << endl;
		 cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
		 Remove_pred_symbols(r, not, pred_s);
		 if (r != nullptr && ((r->operation == OP_ATOM && r->pred == "#") || (r->operation == OP_NOT && r->left->pred == "#")))
		 {
			 cout << ">>>>>>Congratulations!<<<<<<<<" << endl;
			 break;
		 }
	 	 print(r);
		 cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	 }
}
int main()
{
	string path;
	cout << "Please enter path to your file." << endl;
	cin >> path;
	fin = new ifstream(path);
	string st;
	cout << yyparse() << endl;
	int k = (*ResultTree).size();
	int i = 0;
	while (true)
	{
		if (i==k) break;
		NormalTree.push_back((*ResultTree).front());
		(*ResultTree).pop_front();
		i++;
	}
	cout <<"Number of sentences:  "<< NormalTree.size() << endl;
	cout << OP_ATOM << ":  predicate_symbol    " << OP_NOT << ": ~   " <<
		OP_OR << ": |   " <<
		OP_AND << ": &   " <<
		OP_IMPL << ": =>   " <<
		OP_EQ << ": <=>   " << endl; 
	//list<string> t1 = {"X","agata"};
	//list<string> t2 = {"butler","Y"};
	//cout << unify(t1, t2) << endl;
	printNormalTree();
	
	//Delete_pred_symbol(NormalTree.front());
	//print(NormalTree.front());
	//Find(false,"dated");
	//print(check_right(NormalTree.front(),false, "dated"));

	Resolution();
	return EXIT_SUCCESS;
}
