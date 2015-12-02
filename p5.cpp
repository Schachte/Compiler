
extern "C" {
 	#include "compiler.h"
}

// Gcc -c compiler.c; g++ p5.cpp compiler.o; ./a.out < tests/test_control_whilewhile_if1.txt


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

ValueNode * currentSwitchOperand1;

vector<char *> if_statement_type_tracker_list;

char * currently_adding = "";

//TESTING FUNCTIONS TO ENSURE VALIDITY INSIDE OF THE PROGRAM
/************************************************************/
StatementNode * gather_statement_data(int counter);
void check_current_token();
void test_linked_list_values();
void test_statement_assigns();
ValueNode * second_operand_for_if_condition();
struct ValueNode * create_numerical_value_node(char * integer_input_value);
/************************************************************/
test
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

				if (temp->if_stmt->true_branch == NULL && strcmp(currently_adding, "SWITCH") != 0) {

          // printf("added\n");
					temp->if_stmt->true_branch = temp->next;

				}

        if (temp->if_stmt->false_branch == NULL && strcmp(currently_adding, "SWITCH") == 0) {
          // printf("added\n");

          temp->if_stmt->false_branch = temp->next;
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

//Function designed to understand the structure of a switch statement and parse the nodes accordingly
void parse_switch_statement() {

  if (VERBOSE)
    printf("Inside of switch/case and the current token type is as follows\n");

    //Get the operand_1 for the switch statement
    t_type = getToken();

    /***************************************************
    * Build the if node for the switch statement
    ****************************************************/

    //Parent if node that will hold the switch statement node
    IfStatement * switch_statement_node = new IfStatement();

    //If the t_type currently exists in the vector node list
  	if (t_type == ID) {

  		char * current_value = strdup(token);

      //Store the parent switch op1 when encountering more case statements
      currentSwitchOperand1 = search_for_value(current_value);

  		switch_statement_node->condition_operand1 = search_for_value(current_value);

      switch_statement_node->condition_op = CONDITION_NOTEQUAL;

      if (VERBOSE) {
        printf("CONDITION NOT EQUAL HAS BEEN SET FOR THE CASE STATEMENT\n\n\n");
      }

      braceCount+=1;

      //Traverse until you hit the num inside the first case statement
      while (t_type != NUM) {
        t_type = getToken();
        check_current_token();

        if (t_type == NUM) {
          break;
        }
      }
    }

      // if (VERBOSE)
        // printf("Case number achieved for operand2 %s\n\n", token);

        //If the global_op_1 is not null, then we can assign the statement node op1
          // printf("Brace count increase %d\n\n", braceCount);


        if (currentSwitchOperand1) {

          // cout << "GLOBAL OP 1 NOT NULL, BUILDING STATEMENT NODE" << endl;

          switch_statement_node->condition_operand1 = currentSwitchOperand1;

          switch_statement_node->condition_op = CONDITION_NOTEQUAL;

        }

      //Set the operand2 for the switch statement
      char * current_token_value = strdup(token);

      //Create the value node for the current token

      ValueNode * operand_2_value_node = new ValueNode();

      operand_2_value_node->name = current_token_value;
      operand_2_value_node->value = atoi(current_token_value);

      if (VERBOSE)
        printf("The token has been copied successfully\n\n");

      switch_statement_node->condition_operand2 = operand_2_value_node;

      if (VERBOSE)
        printf("The token has been stored successfully\n\n");


      StatementNode * newSwitchNode = new StatementNode();

      newSwitchNode->type = IF_STMT;

      newSwitchNode->if_stmt = switch_statement_node;

      currently_adding = "SWITCH";

      // cout << "\n\n\nADDING SWITCH NOW\n\n\n" << endl;

      add_node_to_statement_list(newSwitchNode);

      // cout << "\n\n\nADDING SWITCH NOW\n\n\n" << endl;

      StatementNode * buildNestedBody = gather_statement_data(braceCountNested);
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

      currently_adding = "IF";

			add_node_to_statement_list(newIfNode);

			StatementNode * buildNestedBody = gather_statement_data(braceCountNested);
		}

		else if (current_type == "WHILE") {

			StatementNode * newWhileNode = new StatementNode();

			newWhileNode->type = IF_STMT;

			newWhileNode->if_stmt = currentIfStmt;

      currently_adding = "IF";

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

		//Search through vector and update the value pointer value
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

		// check_current_token();

		//Check to see if it is an assignment statement
		t_type = getToken();

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

      // printf("right before adding\n");

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

      if (t_type == RBRACE && braceCount == 1) {

        ungetToken();
      }

		}

		else if (t_type == IF){

      if_statement_type_tracker_list.push_back("IF");

			current_type = "IF";

			braceCount += 1;

			parse_if_statement();

		}

		else if (t_type == WHILE) {

      if_statement_type_tracker_list.push_back("IF");

			current_type = "WHILE";

			braceCount += 1;

			parse_if_statement();

		}

    else if (t_type == SWITCH || t_type == CASE) {

      if_statement_type_tracker_list.push_back("SWITCH");

      currently_adding = "SWITCH";

      if (VERBOSE)
        cout << "************************************************************\nSWITCH STATEMENT ENTERED SUCCESSFULLY\n************************************************************" << endl;

      current_type = "SWITCH";

      braceCount += 1;

      // printf("BRACE COUNT INSIDE OF THE SWITCH STATEMENT INCREMENT - %d\n\n", braceCount);

      parse_switch_statement();

    }

		else if (t_type == RBRACE) {


			braceCount -= 1;

      // printf("BRACE COUNT DECREMENT- %d\n\n", braceCount);

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

      else if (braceCount > 0 && current_type == "SWITCH") {

        //Create a new statement node for the no-op
        StatementNode * newNoOPNode = new StatementNode();
        newNoOPNode->type = NOOP_STMT;
				add_node_to_statement_list(newNoOPNode);

        if (VERBOSE)
          printf("\n\nThe no-op has been added at the end of the switch statement\n\n");

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

  //Run through and begin parsing the program code
	program = gather_statement_data(braceCount);

  //Create a traversal node to sift through all the stored memory nodes
	StatementNode * traversal = statementList;

	//Search through vector and update the value pointer val (making sure all values are zeroed for execution to do the math properly)
	for (int x = 0; x < value_node_vector_list.size(); x++)
		value_node_vector_list[x]->value = 0;

	vector<StatementNode*> if_queue;

  vector<char*> statement_tracker_copy = if_statement_type_tracker_list;

  // cout << "TEST\n--------" << endl;
  // for (int x = 0; x < if_statement_type_tracker_list.size(); x++)
  // {
  //   // cout << if_queue[x]->if_stmt->condition_operand2->name << endl;
  //   cout << if_statement_type_tracker_list[x] << endl;
  // }
  //
  // cout << "\n\n--------" << endl;

  int position_tracker = 0;
  int global_tracker = 0;

    traversal = statementList;

  while (traversal != NULL) {

    if (traversal->type == IF_STMT) {

      //Add if statement to queue to fix no-ops
			if_queue.push_back(traversal);
      global_tracker+=1;
		}

		if (traversal->type == NOOP_STMT && if_queue.size() > 0 && traversal->next && statement_tracker_copy[global_tracker-1] == "IF") {
      //Assign relevant no-op statement

      global_tracker-=1;

      if_queue.back()->if_stmt->false_branch = traversal->next;

      if_queue.pop_back();

      statement_tracker_copy.erase(statement_tracker_copy.begin() + global_tracker);

      position_tracker+=1;

		}

		else if (traversal->type == NOOP_STMT && traversal->next == NULL && statement_tracker_copy[global_tracker-1] == "IF") {
      //If the no-op exists at the end of the list

      global_tracker-=1;

      // printf("assigned IF\n");
			StatementNode * sNode = new StatementNode();
			sNode->type = NOOP_STMT;
      // if_queue[0]->if_stmt->false_branch = sNode;
      // if_queue.erase(if_queue.begin());
      // if_statement_type_tracker_list.erase(if_statement_type_tracker_list.begin());
      position_tracker+=1;

			if_queue.back()->if_stmt->false_branch = sNode;
      if_queue.pop_back();
      statement_tracker_copy.erase(statement_tracker_copy.begin() + global_tracker);
      // if_statement_type_tracker_list.erase(if_statement_type_tracker_list.begin());
		}

    else if (traversal->type == NOOP_STMT && if_queue.size() > 0 && traversal->next && statement_tracker_copy[global_tracker-1] == "SWITCH") {
      //Assign relevant no-op statement

      global_tracker-=1;

      // printf("assigned switch %d\n", position_tracker);
			if_queue.back()->if_stmt->true_branch = traversal->next;
      if_queue.pop_back();
      statement_tracker_copy.erase(statement_tracker_copy.begin() + global_tracker);
      // if_statement_type_tracker_list.erase(if_statement_type_tracker_list.begin());
      position_tracker+=1;

		}

    else if (traversal->type == NOOP_STMT && traversal->next == NULL && traversal->next && statement_tracker_copy[global_tracker-1] == "SWITCH") {
      //If the no-op exists at the end of the list

      global_tracker-=1;

      // printf("assigned switch\n");
			StatementNode * sNode = new StatementNode();
			sNode->type = NOOP_STMT;
			if_queue.back()->if_stmt->true_branch = sNode;
      if_queue.pop_back();
      statement_tracker_copy.erase(statement_tracker_copy.begin() + global_tracker);
      // if_statement_type_tracker_list.erase(if_statement_type_tracker_list.begin());
      position_tracker+=1;
		}

    //Increment traversal pointer through the node graph
		if (traversal->next)
			traversal = traversal->next;
		else
			break;
	}

    traversal = statementList;

    int counter_track = 0;
  	while (traversal!=NULL)
  	{

  		if (traversal->type == IF_STMT) {
        //Add if statement to queue to fix no-ops
  			if (traversal->if_stmt->false_branch == NULL) {

          // cout << counter_track << endl;
          break;

        }
        counter_track+=1;

  		}

      //Increment traversal pointer through the node graph
      if (traversal->next)
        traversal = traversal->next;
      else
        break;

    }


  // Reset the traversal pointer
	traversal = statementList;

  //Dynamic vector to store while no-op queues
	vector<StatementNode*> while_queue;

	while (traversal!=NULL)
	{
		if (traversal->type == IF_STMT) {
      //Queue up current while node
			while_queue.push_back(traversal);
      // printf("added\n");
		}

		else if (traversal->type == GOTO_STMT && while_queue.size() > 0) {
      //Assign no-op and pop the queue
			traversal->goto_stmt->target = while_queue.back();
			while_queue.pop_back();
		}

    //Increment pointer location for traversing through the graph of nodes
		if (traversal->next)
			traversal = traversal->next;
		else
			break;
	}

  //Test view outut of the current assignments
	test_statement_assigns();


  printf("\n\n\n\n\n\n");

  	traversal = statementList;

    int temp_count = 0;

    while (traversal!=NULL)
  	{
      temp_count+=1;
      if (traversal->type == IF_STMT) {

        if (traversal->if_stmt->false_branch == NULL)
        {
          printf("%d\n", temp_count);

          break;
        }
      }

      if (traversal->next!=NULL)
      {
        traversal=traversal->next;
      }
      else {

        break;
      }
    }


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
