
#include <cstdlib>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
using namespace std;

// You will need these forward references.
class Expr;
class Stmt;

// Runtime Global Variables
int pc;  				// program counter
vector<string> lexemes;
vector<string> tokens;
vector<string>::iterator lexitr;
vector<string>::iterator tokitr;
map<string, int> vartable; 	// map of variables and their values
vector<Stmt *> insttable; 		// table of instructions
map<string, string> symboltable; // map of variables to datatype (i.e. sum t_integer)


// Runtime Global Methods
void dump(); 				// prints vartable, instable, symboltable

// You may need a few additional global methods to manipulate the global variables


// Classes Stmt and Expr
// It is assumed some methods in statement and expression objects will change and
// you may need to add a few new ones.


class Expr{ // expressions are evaluated!
public:
	virtual int eval() = 0;
	virtual string toString() = 0;
	virtual ~Expr(){}
};

class ConstExpr : public Expr{
private:
	int value;
public:
	ConstExpr(int val) { value = val; }
	int eval() {
		return value;
	}
	string toString() { return "ConstExpr: " + to_string(value); }
};
// ~ Caden Perez

class IdExpr : public Expr{
private:
	string id;
public:
	IdExpr(string s)
	{
		id = s;
	}
	int eval()
	{
		map<string, int>::iterator it = vartable.find(id); 
		if (it != vartable.end()){
			return vartable[id];
		}

		return 0;
	}
	string toString()
	{
		return id;
	}
};
//Thomas Neumann


// class written by David Rudenya
class InFixExpr : public Expr{
private:
	vector<Expr *> exprs;
	vector<string> ops;  // tokens of operators

	int valueAtIndex(int i)
	// pre:  first parameter refers to an index in the exprs vector
	// post: returns the numerical value inside either the ConstExpr
	//       or IdExpr at the index i in exprs
	{

		ConstExpr* cptr;
		IdExpr* iptr;
		int retVal;

		cptr = dynamic_cast<ConstExpr*>(exprs[i]);
		
		if (cptr != nullptr)
			retVal = cptr->eval();
		else
		{
			iptr = dynamic_cast<IdExpr*>(exprs[i]);
			retVal = iptr->eval();
		}


		return retVal;
	}

public:
	InFixExpr(){}
	~InFixExpr()
	{
		for (int i = 0; i < exprs.size(); i++)
		{
			if (exprs[i] != nullptr)
				delete exprs[i];
		}
	}

	void addExpr(Expr* e)
	// pre: parameter is a pointer to an Expr object or any of its derived
	//      classes
	// post: parameter has been added to the exprs vector
	{
		exprs.push_back(e);
	}	

	void addOp(string op)
	// pre: parameter is a token for one of the operators for the language
	//      entered in as a string
	// post: the operator has been added to the ops vector
	{
		ops.push_back(op);
	}

	int eval()
	{

		// loops through the ops vector
		int j = 0;

		// used for logical operators to compare two values
		int temp = 0;

		int operand = valueAtIndex(0);

		// starts i at 1, because the first value int the exprs vector
		// was already retrieved
		for (int i = 1; i < exprs.size(); i++)
		{
				
			if (ops[j] == "s_plus")
				operand += valueAtIndex(i);
			else if (ops[j] == "s_mult")
				operand *= valueAtIndex(i);
			else if (ops[j] == "s_lt")
			{
				// assign temp to the next operand to compare
				// them	
				temp = valueAtIndex(i);
				
				// the order the user entered would have been
				// operand < temp; set operand to 1 if true, 
				// 0 if not
				if (operand < temp)
					operand = 1;
				else
					operand = 0;
			} else if (ops[j] == "s_le")
			{
				
				temp = valueAtIndex(i);

				if (operand <= temp)
					operand = 1;
				else
					operand = 0;	
			} else if (ops[j] == "s_gt")
			{
				temp = valueAtIndex(i);	

				if (operand > temp)
					operand = 1;
				else
					operand = 0;	

			} else if (ops[j] == "s_ge")
			{
			
				temp = valueAtIndex(i);	

				if (operand >= temp)
					operand = 1;
				else
					operand = 0;	

			} else if (ops[j] == "s_eq")
			{
			
				temp = valueAtIndex(i);	

				if (operand == temp)
					operand = 1;
				else
					operand = 0;	

			} else if (ops[j] == "s_ne")
			{
			
				temp = valueAtIndex(i);
			
				if (operand != temp)
					operand = 1;
				else
					operand = 0;	

			} else if (ops[j] == "s_and")
			{
			
				temp = valueAtIndex(i);	

				if (operand && temp)
					operand = 1;
				else
					operand = 0;	
			} else if (ops[j] == "s_or")
			{
				temp = valueAtIndex(i);

				if (operand || temp)
					operand =  1;
				else
					operand = 0;
			}	

			j++;
		}

		return operand;
	}
	string toString()
	// pre:  none
	// post: the contents of the vectors have been returned in the form
	// 	 of "operand token_of_operator operand" 
	{
		string retVal = "";
		int j = 0;
		for (int i = 0; i < exprs.size(); i++)
		{
			retVal += exprs[i]->toString();
			
			// j loops through a different size array, so it
			// will likely run out of indices before i will, which
			// would disrupt the loop logic; test j here instead
			if (j < ops.size())
			{
				retVal += " ";
				retVal += ops[j];
				retVal += " ";
				j++;
			}
		}

		return retVal;
	}
};

class Stmt{ // statements are executed!
private:
	string name;
public:
	Stmt(){}
	Stmt(string n) {name = n;}
	virtual ~Stmt(){}
	virtual string toString() = 0;
	virtual void execute() = 0;
};


// class written by David Rudenya
class AssignStmt : public Stmt{
private:
	string var;
	Expr* p_expr;
public:
	AssignStmt():Stmt("s_assign"){}
	AssignStmt(string var_name):Stmt("s_assign")
	{
		var = var_name;
	}
	~AssignStmt() {
		if (p_expr != nullptr)
			delete p_expr;
	}

	void setExpr(Expr* e)
	// pre: argument is an Expr pointer
	// post: p_expr points to an Expr object (or any of its derived 
	//       classes) 
	// desc: written by David Rudenya
	{
		p_expr = e;
	}

	string toString()
	{
	      	return var + " " + p_expr->toString();	
	}
	void execute()
	{
		// eval() is a pure virtual method; every expr object will
		// have their own version of the method
		vartable[var] = p_expr->eval();	
	}
};

class InputStmt : public Stmt{
private:
	string var;
public:
	InputStmt(string v):Stmt("t_input"){var = v;}
	~InputStmt(){}
	string toString(){return "t_input" +  var;}
	void execute(){
		cout << "enter a value" << endl;
		if (symboltable[var] != "t_string")
			cin >> vartable[var];
		else
		{
			string value;
			cin >> value;
		}
	}
};

class StrOutStmt : public Stmt{
private:
	string value;
public:
	StrOutStmt(string v):Stmt("StrOutStmt"){ value = v;}
	~StrOutStmt() {}
	string toString() {
		// pre: output is built
		// post: string value for the output is returned
		return "output: " + value;
	}
	void execute() {
		// pre: instruction table is executed
		// post: outputs the string value to the console
		cout << value << endl;
	}
};
// ~ Caden Perez


// class written by David Rudenya
class ExprOutStmt : public Stmt{
private:
	Expr* p_expr;
public:
	ExprOutStmt():Stmt("ExprOutStmt"){}
	ExprOutStmt(Expr* e):Stmt("ExprOutStmt"){p_expr = e;}
	~ExprOutStmt()
	{
		if (p_expr != nullptr)
			delete p_expr;
	}

	string toString()
	{
		return p_expr->toString();
	}
	void execute()
	// pre: assumes the p_expr pointer points to a valid expression
	// post: the result of the expression's execute method is printed
	// 	 to console
	{
		cout << p_expr->eval() << endl;
	}
};

class Goto : public Stmt{
private:
	int gotoTarget = -1;;
public:
	Goto():Stmt("goto"){}
	Goto(int gotoTargetIn):Stmt("goto"){
		gotoTarget = gotoTargetIn;
	}
	~Goto(){}
	string toString(){
		return "Goto: inst-" + to_string(gotoTarget);
	}
	void setElseTarget(int else_target_in){
		gotoTarget = else_target_in;
	}
	bool isElseSet(){
		if(gotoTarget != -1){
			return true;
		}
		return false;
	}
	void execute(){
		pc = gotoTarget;
	}
	int getData(){
		return gotoTarget;
	}
};
//Thomas Neumann

class IfStmt : public Stmt{
private:
	Expr* p_expr;
	int elsetarget = -1;
public:
	IfStmt(Expr* p_expr_in):Stmt("t_if"){
		p_expr = p_expr_in;
	}
	IfStmt(Expr* p_expr_in, int else_target_in):Stmt("t_if"){
		p_expr = p_expr_in;
		elsetarget = else_target_in;
	}
	~IfStmt(){
		if(p_expr != nullptr){
			delete p_expr;
		}
	}
	void setElseTarget(int else_target_in){
		elsetarget = else_target_in;
	}
	bool isElseSet(){
		if(elsetarget != -1){
			return true;
		}
		return false;
	}
	string toString(){
		return "t_if" +  p_expr->toString() + ":" + to_string(elsetarget);
	}
	void execute(){
		if(p_expr->eval() == 0){
			pc = elsetarget;
		}
	}
	//thomas Neumann
};

class WhileStmt : public Stmt{
private:
	Expr* p_expr;
	int else_target = -1;
public:
	WhileStmt():Stmt("t_while") {}
	WhileStmt(Expr* inexpr):Stmt("t_while") {
		p_expr = inexpr;
	}
	~WhileStmt() {
		if (p_expr != nullptr)
			delete p_expr;
	}
	string toString() {
		// pre: WhileStmt object is built
		// post: returns string representation of the while loop
		return "t_while " + p_expr->toString() + ": " + to_string(else_target);
	}
	void setElse(int e) {
		else_target = e;
	}
	bool isElseSet() {
		return (else_target != -1);
	}
	void execute() {
		// pre: instruction table is executed
		// post: if p_expr returns a 0, then pc jumps
		//       to the end of the while loop
		if (p_expr->eval() == 0) {
			pc = else_target;
		}
	}
};
// ~ Caden Perez

class Compiler{
private:
	void buildIf()
	{
		tokitr++; lexitr++; // t_if
		tokitr++; lexitr++; // s_lparen
		Expr* temp_expr = buildExpr();

		IfStmt* temp_if = new IfStmt(temp_expr);
		insttable.push_back(temp_if);
		tokitr++; lexitr++;
	}
	void buildWhile() {
		// pre: compile method encountered a t_while lexeme
		// post: a full WhileStmt object has been instantiated, set
		//	     with jump instructions, and pushed to the instruction table
		// ~ Caden Perez

		// move past while and (
		tokitr++; lexitr++;
		tokitr++; lexitr++;

		// build the loop's expression, then push to the instruction table
		Expr* temp_expr = buildExpr();
		WhileStmt* instmt = new WhileStmt(temp_expr);
		insttable.push_back(instmt);
	}
	// ~ Caden Perez

	void buildStmt() {}

	void buildAssign()
	// pre:  assumes the compile method ran into a t_id value 
	// post: an AssignStmt object has been built and pushed into the instruction table
	// 	 tokitr/lexitr have been incremented to the value immediately after 
	// 	 the semicolon that concludes an assignment statement
	// desc: written by David Rudenya
	{

		// lexitr would point to the variable name (on the left side of the
		// equal sign)  corresponding to the t_id that tokitr would point to
		map<string, string>::iterator it = symboltable.find(*lexitr);

		// the variable is not in the symbol table
		if (it == symboltable.end()) 
		{
			cout << "use of undeclared variable " << *lexitr << endl;
			// set pc to a value that will terminate the while loop
			// in the compile method
			pc = -1;

		  // the user is attempting to assign something to a string;
		  // the only values that can be assigned are an integer literal
		  // or a variable containing an int; either way, this is a mismatch
		  // of types
		} else if (symboltable[*lexitr] == "t_string")
		{
			pc = -1;
			cout << "assignment of non-string value to string variable " << *lexitr << endl;
		} else
		{
			AssignStmt* temp = new AssignStmt(*lexitr);
			

			tokitr++; lexitr++;
			tokitr++; lexitr++;
			// increment past the t_id and s_assign; tokitr/lexitr now point
			// to the first token/lexeme pair after the equal sign

			temp->setExpr(buildExpr());
			insttable.push_back(temp);

		}

	}
	void buildInput(){
		// move past input and (
		tokitr++; lexitr++;
		tokitr++; lexitr++;
		insttable.push_back(new InputStmt(*lexitr));
		// move past id and )
		tokitr++; lexitr++;
		tokitr++; lexitr++;
	}
	
	
	void buildOutput() 
	{
		// pre:  compile method encountered a t_output lexeme
		// post: an expression or string output statement is created
		//       and pushed to the instruction table
		// desc: written originally by Caden Perez, reviewed/updated
		//       by the whole group
		
		
		tokitr++; lexitr++; // move past output and (
		tokitr++; lexitr++;
		
		if(*tokitr == "t_str")
		{
			StrOutStmt* str_out = new StrOutStmt(*lexitr);
			insttable.push_back(str_out);	
		} else
		{
			ExprOutStmt* expr_temp = new ExprOutStmt(buildExpr());
			insttable.push_back(expr_temp);
		}	

	}
	
	
	
	// use one of the following buildExpr methods
	void buildExpr(Expr*);
	
	Expr* buildExpr()
	// pre:  tokitr/lexitr point to either a t_int/int or t_id/ID pair
	// post: moves the tokitr/lexitr through the file and builds either
	//       an IdExpr, a ConstExpr, or an InFixExpr and returns the
	//       object it built; increments tokitr/lexitr to the term
	//       immediately following either the semicolon or parenthesis that
	//       concluded the loop
	// desc: written by David Rudenya
	{

		// at this point, tokitr/lexitr points to either an ID value 
		// or an int

		Expr* retVal;
		ConstExpr* const_temp;
		IdExpr* id_temp;

		tokitr++;

		// the ID/int was alone, so this is not an InFixExpr 
		if (*tokitr == "s_semi" || *tokitr == "s_rparen")
		{
			// decrement the tokitr so that we can consider 
			// the value it pointed to before testing for a 
			// semicolon or right parenthesis
			tokitr--;
			
			if (*tokitr == "t_int")
				retVal = new ConstExpr(stoi(*lexitr));
			else if (*tokitr == "t_id")
				retVal = new IdExpr(*lexitr);
		} else
		{
			// decrement the tokitr so that we can consider 
			// the value it pointed to 
			tokitr--;

			InFixExpr* in_temp = new InFixExpr(); 
			map<string, string>::iterator it;
			
			// loop until encountering one of the two symbols
			// that can end either an assignment statement
			// or any statement that uses (EXPR) in the grammar
			while (*tokitr != "s_rparen" && *tokitr != "s_semi")
			{
				if (*tokitr == "t_int")
				{
					const_temp = new ConstExpr(stoi(*lexitr));
					in_temp->addExpr(const_temp);
				} else if (*tokitr == "t_id")
				{
					it = symboltable.find(*lexitr); 
					if (it != symboltable.end())
					{
						id_temp = new IdExpr(*lexitr);
						in_temp->addExpr(id_temp);
					} else
					{
						cout << "undeclared variable " << *lexitr << endl;
						pc = -1;
					}	
				} else
					in_temp->addOp(*tokitr);

				tokitr++; lexitr++;
			}

			retVal = in_temp;

		}

		// increment tokitr/lexitr past either the semicolon or
		// right parenthesis that ended the loop
		tokitr++; lexitr++;
		return retVal; 
	}
	// headers for populate methods may not change

	void buildElseIf(){
		//finds the last ifstmt
		int i = insttable.size();
		IfStmt* temp_if = nullptr;
		while(temp_if == nullptr && i >= 0){
			i--;
			temp_if = dynamic_cast<IfStmt*>(insttable[i]);
			if(temp_if != nullptr){
				if(temp_if->isElseSet()){
					temp_if = nullptr;
				}
			}
		}

		insttable.push_back(new Goto());

		//if i is greater or equal to 0 then that means it found a Goto or IfStmt
		if(i >= 0){
			//insttable.size() is going to be the next instruction in the line
			IfStmt* temp_ptr = dynamic_cast<IfStmt*>(insttable[i]);
			temp_ptr->setElseTarget(insttable.size() - 1);
		}
		tokitr++; lexitr++;
	}
	void buildEndIf(){
		//finds the last goto or ifstmt
		int i = insttable.size();
		IfStmt* temp_if = nullptr;
		Goto* temp_goto = nullptr;
		while(temp_if == nullptr && temp_goto == nullptr && i >= 0){
			i--;
			temp_if = dynamic_cast<IfStmt*>(insttable[i]);
			if(temp_if != nullptr){
				if(temp_if->isElseSet()){
					temp_if = nullptr;
				}
			}
			temp_goto = dynamic_cast<Goto*>(insttable[i]);
			if(temp_goto != nullptr){
				if(temp_goto->isElseSet()){
					temp_goto = nullptr;
				}
			}
		}
		//if it is an Ifstmt then it was an if...endIf no else
		if(temp_if != nullptr && i >= 0){
			temp_if->setElseTarget(insttable.size() - 1);

		//if it is a goto then it was an if...else...endIf
		}else if(temp_goto != nullptr && i >= 0){
			temp_goto->setElseTarget(insttable.size() - 1);
		}

		tokitr++; lexitr++;
	}
	void buildEndLoop(){
		// pre: a WhileStmt was built and this method was called
		// post: determines the location of the while statement's
		//       else part and create a jump location.

		//finds the last ifstmt
		int i = insttable.size();
		WhileStmt* temp_while = nullptr;
		while (temp_while == nullptr && i >= 0) {
			i--;
			temp_while = dynamic_cast<WhileStmt*>(insttable[i]);
			// following if block will check for nested loops.
			if (temp_while != nullptr){
			 	if (temp_while->isElseSet()){
			 		temp_while = nullptr;
				}
			}
		}

		// for loop in run will increment after, so we need to go back one
		insttable.push_back(new Goto(i - 1));

		// if i is greater or equal to 0 then that means it found a WhileStmt
		if (i >= 0){
			// insttable.size() is going to be the next instruction in the line
			WhileStmt* temp_ptr = dynamic_cast<WhileStmt*>(insttable[i]);
			temp_ptr->setElse(insttable.size() - 1);
		}
		tokitr++; lexitr++;	
	}
	
	void populateTokenLexemes(istream& infile)
	// pre:  parameter is an open input file object with token lexeme pairs
	// post: tokens and lexemes vectors populated with values
	// desc: written by the whole group 
	{
		string line, tok, lex;
		int pos;
			
		getline(infile, line);
		while(!infile.eof())
		{
			pos = line.find(" ");
			tok = line.substr(0, pos);
			lex = line.substr(pos+1, line.length());
			tokens.push_back(tok);
			lexemes.push_back(lex);
			getline(infile, line);
		}
		
	}
	void populateSymbolTable(istream& infile)
	// pre:  parameter is an open input file object with id type pairs
	// post: symboltable map is populated; variable ID is the key, type is the value 
	// desc: written by the whole group
	{
		string line, var, type;
		int pos;
			
		getline(infile, line);
		while(!infile.eof())
		{
			pos = line.find(" ");
			var = line.substr(0, pos);
			type = line.substr(pos+1, line.length());

			if(symboltable.find(var) == symboltable.end() && var != "\0")
				symboltable[var] = type;

			getline(infile, line);
		}
	}
public:
	Compiler(istream& source, istream& symbols)
	// pre:  first parameter is a valid open input file containing source code,
	//       second paremter is a valid open input file containing id type pairs
	// post: compiler object built and symboltable and token/lexeme vectors populated
	// desc: written by David Rudenya
	{
		populateTokenLexemes(source);
		populateSymbolTable(symbols);
	}
	// The compile method is responsible for getting the instruction
	// table built.  It will call the appropriate build methods.
	bool compile(){
		// pre: assumes the token and lexeme vectors have been populated
		//		from a source code file
		// post: constructs an instruction table for the program, based
		// 		on the token and lexeme vectors, that can be utilized by
		//		the run method

		tokitr = tokens.begin();
		lexitr = lexemes.begin();

		// variable declaration skip
		while (*tokitr != "t_main") {
			tokitr++; lexitr++;
		}
		tokitr++; lexitr++; // move past main
		pc = 0;

		// main build loop checks for specific token values.
		while(tokitr != tokens.end() && pc != -1) {
			if (*tokitr == "t_input")
				buildInput();
			else if (*tokitr == "t_while")
				buildWhile();
			else if (*tokitr == "t_if")
				buildIf();
			else if (*tokitr == "t_id")
				buildAssign();
			else if (*tokitr == "t_output")
				buildOutput();
			else if (*tokitr == "t_else")
				buildElseIf();
			else if (*tokitr == "t_end") {
				tokitr++; lexitr++;
				if (tokitr != tokens.end()) {
					if (*tokitr == "t_if")
						buildEndIf();
					if (*tokitr == "t_loop")
						buildEndLoop();
				}
			}
			else {
				tokitr++;
				lexitr++;
			}
		}
		// if pc is set to -1, an error is found
		if (pc == -1)
			return false;
		return true;
	}

	// The run method will execute the code in the instruction
	// table.
	void run(){
		for(pc = 0; pc < insttable.size() && pc!=-1; pc++){
			insttable[pc]->execute();
		}
	}
};

// prints vartable, instable, symboltable
void dump(){ 
	cout << "vartable" << endl;
	for(map<string, int>::iterator iter=vartable.begin(); iter!=vartable.end(); ++iter){
		cout << iter->first << " => " << iter->second << endl;
	}

	cout << endl << "symboltable" << endl;
	for(map<string, string>::iterator iter=symboltable.begin(); iter!=symboltable.end(); ++iter){
		cout << iter->first << " => " << iter->second << endl;
	}

	cout << endl << "insttable" << endl;
	for(int instCount = 0; instCount < insttable.size(); instCount++){
		cout << instCount << ": " << insttable[instCount]->toString() << endl;
	}
}
//Thomas Neumann

int main(){
	ifstream infile1("source1.txt");
	ifstream infile2("symbol1.txt");
	if (!infile1 || !infile2) exit(-1);
	Compiler c(infile1, infile2);
	if ( c.compile() )
		c.run();
	dump();

	return 0;
}
