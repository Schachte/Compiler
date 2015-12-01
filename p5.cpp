
extern "C" {
 	#include "compiler.h"
}

#define VERBOSE 1

#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;


union statement_types
{
    struct AssignmentStatement* assign_stmt;
    struct PrintStatement* print_stmt;
    struct IfStatement* if_stmt;
    struct GotoStatement* goto_stmt;
};


std::string reserveds[] =
{
    "",
    "VAR",
    "IF",
    "WHILE",
    "SWITCH",
    "CASE",
    "DEFAULT",
    "print",
    "GOTO",
    "FOR",
    "+",
    "-",
    "/",
    "*",
    "=",
    ":",
    ",",
    ";",
    "[",
    "]",
    "(",
    ")",
    "{",
    "}",
    "<>",
    ">",
    "<",
    "ID",
    "NUM",
    "ERROR"
};

int braceCount = 1;
int braceCountNested = 1;

//Global vector list of value node pointers
vector<ValueNode*> value_node_vector_list;

//Vector to repoint all the false branches to appropriate noop locations
vector<StatementNode*> noop_location_vector;

//Linkedlist of statement nodes that will run through the parser
struct StatementNode * statementList = NULL;

//Takes care of very first LBRACE when parsing code body
int beginParsingCode = 0;

//Updates based on most recently visited node type
char * current_type;

ValueNode * current_switch_op_1;

char * currently_adding = "";

vector<char*> if_queue_list;
vector<StatementNode*> if_queue_node_list;

//TESTING FUNCTIONS TO ENSURE VALIDITY INSIDE OF THE PROGRAM
/************************************************************/
StatementNode * gather_statement_data(int counter);
void check_current_token();
void test_linked_list_values();
void test_statement_assigns();
struct ValueNode * create_numerical_value_node(char * integer_input_value);
/************************************************************/


//Function to add value nodes to the statement list
void add_node_to_statement_list(StatementNode * sNode) {

	//Take in the actual pointer to the struct and the enum statement type to insert into the linkedList

	StatementNode * tempIfNode = new StatementNode();

	int found = 0;

	if (statementList == NULL) {

		if (VERBOSE)
			cout << "STATEMENTLIST IS CURRENTLY NULL, ADDING TO BEGINNING OF LIST" << endl;

		statementList = sNode;

	}

	else {

		//Temp node to traverse to the end of the linkedlist
		StatementNode * temp = statementList;

		while (temp->next != NULL) {

			if (temp->type == IF_STMT)
			{

        if (temp->if_stmt->false_branch == NULL && (currently_adding == "SWITCH" || currently_adding == "CASE")) {

          //Assigning what will execute if (TRUE) for a case statement body
          temp->if_stmt->false_branch = temp->next;
          cout << "\n\n\n\nASSIGNED NEXT SWITCH VALUE FOR FALSE BRANCH\n\n" << endl;

          cout << temp << endl;
        }

				else if (temp->if_stmt->true_branch == NULL && currently_adding != "SWITCH") {

					temp->if_stmt->true_branch = temp->next;
				}
			}

			temp = temp->next;
		}

		if (temp->next == NULL){

			if (VERBOSE)
				cout << "Assigning node to the end of the linked list" << endl;

			temp->next = sNode;
			temp = temp->next;
		}

	}
}

//Function to search for a value node and return it to the necessary function
ValueNode * search_for_value(char * findMe) {

	//Loop through all the data in the value node vector
	for (int x = 0; x < value_node_vector_list.size(); x++) {

		if (strcmp(findMe, value_node_vector_list[x]->name)==0) {

			if (VERBOSE)
				cout << "FOUND AND RETURNING" << value_node_vector_list[x]->value << endl;

			return value_node_vector_list[x];
		}
	}

	return NULL;
}

//Function to parse out and return the print statement data
PrintStatement * parse_print_statement() {

	//Memory allocaton
	struct PrintStatement * printNode;
	printNode = new PrintStatement();

	return NULL;
}


ValueNode * second_operand_for_if_condition() {

	ValueNode * tempSecondOp = new ValueNode();

	t_type = getToken();

	check_current_token();

	if (t_type == NUM) {

		char * current_value = token;

		tempSecondOp = create_numerical_value_node(current_value);

		if (VERBOSE)
			cout << "Second operand assigned as NUM" << endl;

	}

	else if (t_type == ID) {

		char * current_value = token;

		tempSecondOp = search_for_value(current_value);

		if (VERBOSE)
			cout << "Second operand assigned as ID" << endl;
	}

return tempSecondOp;

}


void parse_switch_statement() {

  //Get the operand 1 that you want to use for the switch statement
  t_type = getToken();

  //Build the statement that will contain the operands
  IfStatement * newSwitchStatement = new IfStatement();

  char * current_value;

  /************************
  BUILDING OP_1 FOR SWITCH
  ************************/

  if (current_type == "SWITCH") {

    current_value = strdup(token);

    newSwitchStatement->condition_operand1 = search_for_value(current_value);

    current_switch_op_1 = newSwitchStatement->condition_operand1;

    if (VERBOSE)
      cout << "Assigned the value to operand 1 for the switch as " << newSwitchStatement->condition_operand1->name << endl;


    /************************
    BUILDING OP_2 FOR SWITCH
    ************************/
    while (t_type != CASE) {
      t_type = getToken();
    }

    //Get the actual value that is done for the realational comparisons
    t_type = getToken();
    // t_type = getToken

    braceCount+=1;

  }

  else if (current_type == "CASE") {

    newSwitchStatement->condition_operand1 = current_switch_op_1;

    t_type = getToken();

    cout << "ASSIGNED OP_1 AS PREVIOUS VALUE, CURRENT VALUE IS " << token << endl;

  }

  if (VERBOSE)
    cout << "The type value is now " << token << endl;

  current_value = strdup(token);

  newSwitchStatement->condition_op = CONDITION_NOTEQUAL;
  newSwitchStatement->condition_operand2 = create_numerical_value_node(token);


  /*************************
  ADDING SWITCH NODE TO LIST
  *************************/

  StatementNode * newSwitchNode = new StatementNode();

  newSwitchNode->type = IF_STMT;

  newSwitchNode->if_stmt = newSwitchStatement;

  if_queue_node_list.push_back(newSwitchNode);

  add_node_to_statement_list(newSwitchNode);

  StatementNode * buildNestedBody = gather_statement_data(braceCountNested);

}


//Function to parse and assign proper if statement data
void parse_if_statement() {

	IfStatement * currentIfStmt = new IfStatement();

	if (VERBOSE)
		cout << "Memory allocated for the if statement\n------------------------------------------" << endl;

	t_type = getToken();

	//If the t_type currently exists in the vector node list
	if (t_type == ID) {

		char * current_value = strdup(token);

		currentIfStmt->condition_operand1 = search_for_value(current_value);

		if (VERBOSE)
			cout << "The condition operand has been set as " << currentIfStmt->condition_operand1->name << endl;

		//Consuming the relational operator
		t_type = getToken();

		if (t_type == GREATER) {

			if (VERBOSE)
				cout << "THIS CONDITION IS >" << endl;

			currentIfStmt->condition_op = CONDITION_GREATER;

			if (VERBOSE)
				cout << "OP HAS BEEN SET SUCCESSFULLY!" << endl;

			currentIfStmt->condition_operand2 = second_operand_for_if_condition();

			if (VERBOSE)
				cout << "OP 2 HAS BEEN SET SUCCESSFULLY!" << endl;
		}

		else if (t_type == LESS) {

			if (VERBOSE)
				cout << "THIS CONDITION IS <" << endl;

			currentIfStmt->condition_op = CONDITION_LESS;

			if (VERBOSE)
				cout << "OP HAS BEEN SET SUCCESSFULLY!" << endl;

			currentIfStmt->condition_operand2 = second_operand_for_if_condition();

		}

		else if (t_type == NOTEQUAL) {

			if (VERBOSE)
				cout << "THIS CONDITION IS !=" << endl;

			currentIfStmt->condition_op = CONDITION_NOTEQUAL;

			if (VERBOSE)
				cout << "OP HAS BEEN SET SUCCESSFULLY!" << endl;

			currentIfStmt->condition_operand2 = second_operand_for_if_condition();

		}

		if (current_type == "IF") {

			StatementNode * newIfNode = new StatementNode();

			newIfNode->type = IF_STMT;

			newIfNode->if_stmt = currentIfStmt;

      if_queue_node_list.push_back(newIfNode);

			add_node_to_statement_list(newIfNode);

			StatementNode * buildNestedBody = gather_statement_data(braceCountNested);
		}

		else if (current_type == "WHILE") {

			StatementNode * newWhileNode = new StatementNode();

			newWhileNode->type = IF_STMT;

			newWhileNode->if_stmt = currentIfStmt;

			add_node_to_statement_list(newWhileNode);

			StatementNode * buildNestedBody = gather_statement_data(braceCountNested);

		}

	}
}

//Function to create a value node off of an integer
struct ValueNode * create_numerical_value_node(char * integer_input_value) {

	//Allocate memory for the value node
	ValueNode * newValueNode;
	newValueNode = new ValueNode;

	newValueNode->name = integer_input_value;
	newValueNode->value = atoi(integer_input_value);

	return newValueNode;
}


//Function to return the op type of an assign statement if necessary
char * check_assignment_op_type() {

	//Now we need to check if there are any more operands involved
	t_type = getToken();

	//Checking if another operand is involved in the computation of the assignment statement
	if (t_type == PLUS 	||
		t_type == MINUS ||
		t_type == DIV 	||
		t_type == MULT)
		{
			if (t_type == PLUS)
				return "OP_PLUS";
			else if (t_type == MINUS)
				return "OP_MINUS";
			else if (t_type == DIV)
				return "OP_DIV";
			else if (t_type == MULT)
				return "OP_MULT";
		}

	return "NULL";
}

//Simple function to return ENUM text equivalent from an input string
AssignmentOpType get_enum_from_string(char * string_enum) {


	if (string_enum == "OP_PLUS")
		return OP_PLUS;
	else if (string_enum == "OP_MINUS")
		return OP_MINUS;
	else if (string_enum == "OP_DIV")
		return OP_DIV;
	else if (string_enum == "OP_MULT")
		return OP_MULT;
}

AssignmentStatement * parse_assign_statement() {

	//Allocate memory for the assignment statement
	struct AssignmentStatement * assignStmt;
	assignStmt = new AssignmentStatement;

	char * current_value = strdup(token);

	//Currently at the value on the LHS
	assignStmt->left_hand_side = search_for_value(current_value);

	if (VERBOSE)
		printf("LHS has been assigned as %s\n", current_value);

	check_current_token();

	//Consume the equality symbol
	t_type = getToken();

	//Traverse to the first (or only) operand
	t_type = getToken();

	check_current_token();

	//Check if the first value is a NUM
	if (t_type == NUM) {

		if (VERBOSE)
			cout << "THIS VALUE BEING ASSIGNED IS CURRENTLY A NUM" << endl;

		char * current_value = strdup(token);

		assignStmt->operand1 = create_numerical_value_node(current_value);

		if (VERBOSE)
			cout << "THE OPERAND1 HAS BEEN SET AS A NUMBER" << endl;
		}

	//Else if the operand is a variable, we need to obtain the value node
	else if (t_type == ID) {

		if (VERBOSE)
			cout << "THIS VALUE BEING ASSIGNED IS CURRENTLY AN ID" << endl;

		char * current_value = strdup(token);

		assignStmt->operand1 = search_for_value(current_value);
	}

	char * potential_op_type = check_assignment_op_type();

	//An operand is involved in the assignment statement
	if (potential_op_type != "NULL")
	{
		//Get the op_type
		AssignmentOpType op_type = get_enum_from_string(potential_op_type);
		assignStmt->op = op_type;

		if (VERBOSE)
			printf("OP ASSIGNED (%d)\n", op_type);

		t_type = getToken();

		//Assign the second operand if it's a NUM
		if (t_type == NUM) {

			if (VERBOSE)
				cout << "THIS VALUE BEING ASSIGNED IS CURRENTLY A NUM" << endl;

			char * current_value = strdup(token);

			assignStmt->operand2 = create_numerical_value_node(current_value);
		}

		if (t_type == ID) {

			if (VERBOSE)
				cout << "THIS VALUE BEING ASSIGNED IS CURRENTLY AN ID" << endl;

			char * current_value = strdup(token);

			assignStmt->operand2 = search_for_value(current_value);

			t_type = getToken();
		}
	}

	else {

		assignStmt->op = OP_NOOP;

		//Search through vector and update the value pointer val
		for (int x = 0; x < value_node_vector_list.size(); x++)
		{
			if (strcmp(value_node_vector_list[x]->name, assignStmt->left_hand_side->name) == 0) {

				value_node_vector_list[x]->value = assignStmt->operand1->value;

				if (VERBOSE)
					printf("Updated %s to %d\n", value_node_vector_list[x]->name, value_node_vector_list[x]->value);
			}
		}
	}

	ungetToken();
	check_current_token();

	return assignStmt;
}

//Function to return a linkedlist of Statement Nodes
StatementNode * gather_statement_data(int braceCountNum) {

	if (beginParsingCode == 0) {

		//This is an LBRACE, we need to continue parsing until we hit an RBRACE
		t_type = getToken();
		beginParsingCode = 1;
	}

	while (braceCount != 0) {

		//Traverse through the characters within the statements
		if (beginParsingCode == 0)
			t_type = getToken();

		check_current_token();

		//Check to see if it is an assignment statement
		t_type = getToken();

		check_current_token();

		if (t_type == EQUAL){

			AssignmentStatement * assignStatement;

			if (VERBOSE)
				printf("This is an assignment statement\n");

			//Unget token so you can store the LHS inside of a value node
			ungetToken();

			assignStatement = parse_assign_statement();

			if (VERBOSE)
				printf("THE ASSSIGNMENT STATEMENT HAS BEEN PARSED ACCORDINGLY\n");

			//We now need to add this to the linkedlist
			StatementNode * tempStatementNode;
			tempStatementNode = new StatementNode();

			tempStatementNode->type = ASSIGN_STMT;
			tempStatementNode->assign_stmt = assignStatement;

			add_node_to_statement_list(tempStatementNode);

			if (VERBOSE)
				cout << "**********************\nASSIGN ADDED\n**********************" << endl;

		}

		else if (t_type == PRINT) {

			ungetToken();

			if (VERBOSE)
				printf("The current token type is a print statement\n");

			//Allocating memory for the print statement node
			PrintStatement * pStatement;
			pStatement = new PrintStatement;

			if (VERBOSE)
				printf("The memory for the print statement node has been allocated successfully\n");

			//Consume the print keyword
			t_type = getToken();

			//Gather the ID that we are printing out to the console
			t_type = getToken();

			//Debugging check
			check_current_token();

			char * current_value = strdup(token);

			ValueNode * printNode = search_for_value(current_value);

			pStatement->id = printNode;

			StatementNode * tempStatementNode;
			tempStatementNode = new StatementNode();

			tempStatementNode->type = PRINT_STMT;
			tempStatementNode->print_stmt = pStatement;

			if (VERBOSE) {
				cout << "**********************\nPRINT ADDED\n**********************" << endl;
			}

			add_node_to_statement_list(tempStatementNode);

			t_type = getToken();

			check_current_token();

		}

		else if (t_type == IF){

      if_queue_list.push_back("IF");
      // printf("ADDDDDDED\n\n\n\n\n");
      currently_adding = "IF";

			current_type = "IF";

			braceCount += 1;

			parse_if_statement();

		}

		else if (t_type == WHILE) {


			current_type = "WHILE";

			braceCount += 1;

			parse_if_statement();

		}

    else if (t_type == SWITCH) {

      if_queue_list.push_back("SWITCH");
      // printf("ADDDDDDED\n\n\n\n\n");

      braceCount += 1;

      currently_adding = "SWITCH";

      current_type = "SWITCH";

      cout << "\n\nCURRENTLY ENTERING SWITCH STATEMENT\n\n" << endl;

      parse_switch_statement();


    }

    else if (t_type == CASE) {

      currently_adding = "SWITCH";

      if_queue_list.push_back("SWITCH");
      // printf("ADDDDDDED\n\n\n\n\n");

      braceCount += 1;

      current_type = "CASE";

      cout << "\n\nCURRENTLY ENTERING CASE STATEMENT\n\n" << endl;

      parse_switch_statement();

    }

		else if (t_type == RBRACE) {

			braceCount -= 1;

			if (braceCount > 0 && current_type == "IF") {

				StatementNode * newNoOPNode = new StatementNode();

				newNoOPNode->type = NOOP_STMT;

				add_node_to_statement_list(newNoOPNode);
			}

			else if (braceCount > 0 && current_type == "WHILE") {

				StatementNode * newGoToNode = new StatementNode();

				newGoToNode->type = GOTO_STMT;

				struct GotoStatement * GTN = new GotoStatement();

				newGoToNode->goto_stmt = GTN;

				add_node_to_statement_list(newGoToNode);

				StatementNode * newNoOPNode = new StatementNode();

				newNoOPNode->type = NOOP_STMT;

				add_node_to_statement_list(newNoOPNode);

			}

      else if (braceCount > 0 && (current_type == "CASE" || current_type == "SWITCH")) {

          currently_adding = "SWITCH";

          StatementNode * newNoOPNode = new StatementNode();

  				newNoOPNode->type = NOOP_STMT;

  				add_node_to_statement_list(newNoOPNode);
      }
		}
	}

	return statementList;
}

//Function to parse out the variables used inside of the block of code
vector<ValueNode*> parse_variable_names() {

	//Parse until we hit the end of the declaration section at the top of the code block
	while (t_type != SEMICOLON) {

		//Allocate memory inside of the program
		struct ValueNode * temp_token_value;
		temp_token_value = new ValueNode();

		//Get the token from the input fed into the program
		t_type = getToken();

		//String dupe to avoid addressing issues of the token
		char * tempTokenStore = strdup(token);

		//Set the name param in the struct
		temp_token_value->name = tempTokenStore;

		//Pre-initialize all values to 0
		temp_token_value->value = 0;

		//Add the value to the vector list containing all of the initial value nodes
		value_node_vector_list.push_back(temp_token_value);

		//Increment the position of the current token to the next token
		t_type = getToken();
	}

	t_type = getToken();

	check_current_token();

	return value_node_vector_list;
}

//Main function to parse out all the data and assign value nodes
struct StatementNode * parse_generate_intermediate_representation() {

	//Begin the parsing for the print statement in the program
	struct StatementNode * program;

	//Get a vector of value node pointers
	vector<ValueNode*> variables_used = parse_variable_names();

	program = gather_statement_data(braceCount);

	StatementNode * traversal = statementList;

	//Search through vector and update the value pointer val
	for (int x = 0; x < value_node_vector_list.size(); x++)
	{
		value_node_vector_list[x]->value = 0;
	}

  /******************
      DEBUGGING
  ******************/

  printf("\n\n\n\n\n----%d\n", if_queue_list.size());

  for (int i = 0; i < if_queue_list.size(); i++)
  {
    printf("%s ", if_queue_list[i]);
  }


  /******************
     END  DEBUGGING
  ******************/

  // vector<char *>clone_if_queue_list = if_queue_list;

  int queue_counter = 0;

	vector<StatementNode*> if_queue;

  printf("\n\n\n\n\n\n");

	while (traversal!=NULL)
	{

		if (traversal->type == IF_STMT) {

			if_queue.push_back(traversal);
      queue_counter+=1;
		}

    //Handle setting the false branch for an if statement
		else if (traversal->type == NOOP_STMT && if_queue.size() > 0 && traversal->next && if_queue_list[queue_counter-1] == "IF") {

      cout << "IF setting false branch" << endl;

      queue_counter--;
			if_queue_node_list[queue_counter]->if_stmt->false_branch = traversal->next;
      if_queue_list.erase(if_queue_list.begin() + queue_counter);
      if_queue_node_list.erase(if_queue_node_list.begin() + queue_counter);

		}

    //Handle setting the false branch for an if statement if next is null
		else if (traversal->type == NOOP_STMT && traversal->next == NULL && if_queue_list[queue_counter-1] == "IF") {

      cout << "IF" << endl;

      queue_counter--;
			StatementNode * sNode = new StatementNode();
			sNode->type = NOOP_STMT;
			if_queue_node_list[queue_counter]->if_stmt->false_branch = sNode;
      if_queue_list.erase(if_queue_list.begin() + queue_counter);
      if_queue_node_list.erase(if_queue_node_list.begin() + queue_counter);

		}

    //Handle setting the false branch for a switch statement
		else if (traversal->type == NOOP_STMT && if_queue.size() > 0 && traversal->next && if_queue_list[queue_counter-1] == "SWITCH") {

      cout << "SWITCH true branch set" << endl;

      queue_counter--;
      if_queue_node_list[queue_counter]->if_stmt->true_branch = traversal->next;

      if (!if_queue_node_list[queue_counter]->if_stmt->false_branch)
      {
        cout << "TRUE THE FALSE BRANCH FOR SWITCH IS NULL " << endl;
      }

      if_queue_list.erase(if_queue_list.begin() + queue_counter);
      if_queue_node_list.erase(if_queue_node_list.begin() + queue_counter);

		}

    //Handle setting the false branch for a switch statement if next is null
		else if (traversal->type == NOOP_STMT && traversal->next == NULL && if_queue_list[queue_counter-1] == "SWITCH") {

      cout << "SWITCH true branch set" << endl;

      queue_counter--;
			StatementNode * sNode = new StatementNode();
			sNode->type = NOOP_STMT;
			if_queue_node_list[queue_counter]->if_stmt->true_branch = sNode;

      if (!if_queue_node_list[queue_counter]->if_stmt->false_branch)
      {
        cout << "TRUE THE FALSE BRANCH FOR SWITCH IS NULL " << endl;
      }

      if_queue_list.erase(if_queue_list.begin() + queue_counter);
      if_queue_node_list.erase(if_queue_node_list.begin() + queue_counter);

		}

		if (traversal->next)
			traversal = traversal->next;
		else
			break;
	}

  printf("\n\n\n\n\n\n");

	// traversal = statementList;
  //
  //
	// vector<StatementNode*> while_queue;
  //
	// while (traversal!=NULL)
	// {
  //
	// 	if (traversal->type == IF_STMT) {
  //
	// 		// cout << "THIS IS THE WHILE , queieing" << endl;
  //
	// 		while_queue.push_back(traversal);
	// 	}
  //
	// 	else if (traversal->type == GOTO_STMT && while_queue.size() > 0) {
  //
	// 		traversal->goto_stmt->target = while_queue.back();
	// 		while_queue.pop_back();
  //
	// 		// cout << "LINKING GO TO TO WHILE STATEMENT" << endl;
	// 	}
  //
	// 	if (traversal->next)
	// 		traversal = traversal->next;
	// 	else
	// 		break;
	// }

  // printf("\n\n\n\n\n\n");
  //
  // 	traversal = statementList;
  //
  //   int temp_count = 0;
  //
  //   while (traversal!=NULL)
  // 	{
  //
  //     if (traversal->type == NOOP_STMT) {
  //       temp_count+=1;
  //
  //       if (traversal->if_stmt->false_branch == NULL)
  //       {
  //         printf("%s\n", clone_if_queue_list[temp_count]);
  //         break;
  //       }
  //     }
  //
  //     if (traversal->next!=NULL)
  //     {
  //       traversal=traversal->next;
  //     }
  //     else {
  //
  //       break;
  //     }
    // }


	test_statement_assigns();



	return program;
}


/****************
TESTING FUNCTIONS
*****************/

//Print all the values that are currently stored inside of the linkedlist
void test_linked_list_values() {

	if (VERBOSE) {
		printf("************************************\nTEST FUNCTION FOR LINKEDLIST\n***********************************\n");

		//Loop through the names and values of the value node vector list
		for (int i = 0; i < value_node_vector_list.size(); i++)
		{
			printf("The name is: %s\n", value_node_vector_list[i]->name);
			printf("The value is: %d\n\n", value_node_vector_list[i]->value);
		}
	}
}

//Call the name of the current t_type at any given time
void check_current_token() {

	if (VERBOSE)
		printf("The current token_type is %s\n", reserveds[t_type].c_str());

}

void test_statement_assigns() {


	StatementNode * traversal = statementList;

	if (VERBOSE) {

		std::string type_array[] =
		{"NOOP_STMT",
	    "PRINT_STMT",
	    "ASSIGN_STMT",
	    "IF_STMT",
	    "GOTO_STMT"};


		while (traversal!=NULL)
		{
			cout << type_array[traversal->type] << endl;

			if (traversal->next)
				traversal = traversal->next;
			else
				break;
		}
	}
}
