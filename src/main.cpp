/*
	main.hpp

	The MIT License (MIT)

	Copyright (c) 2013 Maxime Alvarez

	Permission is hereby granted, free of charge, to any person obtaining a copy of
	this software and associated documentation files (the "Software"), to deal in
	the Software without restriction, including without limitation the rights to
	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
	the Software, and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
	FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
	COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
	IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
	CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

	This is the interpreter for SmallThink, a toy-programming-exotic-language.
	Look at doc/tutorial.rst for a better description of SmallThink.
*/

// Because we manipulate strings a lot.
#include <string>

// For std::cout, std::cin and std::ifstream.
#include <iostream>
#include <fstream>

// For runtime, used to manipulates variables by their names.
#include <map>
#include <utility>

// To catch exceptions.
#include <stdexcept>

// For flushing std::cin, time(), srand() and rand().
#include <climits>
#include <ctime>
#include <cstdlib>

// For trim and conversions functions : to and from.
#include "string_utils.hpp"

/* Some useful data structures and enums. */
namespace data
{
    /* Opcodes list. */
    std::vector<std::string> opcodes = {"mov", "add", "mul", "cmp_eq", "cmp_gt", "cmp_lt", "neg", "out", "in", "get", "flush", "stop", "label", "jmp", "jnz", "jz", "num", "str", "num_int", "seed_random"};

    /* Returns true if given string is in opcodes list. */
    bool is_opcode(std::string x)
    {
        return find(opcodes.begin(), opcodes.end(), x) != opcodes.end();
    }

    /* Returns true if given string is a decimal number. */
    bool is_number(std::string x)
    {
        // Empty strings are not numbers.
        if(x.size() > 0)
        {
            unsigned int offset(0); // No offset.

            // If starts by a '-'.
            if(x.at(0) == '-')
                offset = 1;

            for(unsigned int i(offset) ; i < x.size() ; ++i)
                if(!isdigit(x.at(i)) && x.at(i) != '.')
                {
                    return false;
                }

            return true;
        }

        return false;
    }

    /* Tokens types, prefixed by TT_ (TOKEN TYPE_). */
    enum token_type
    {
        TT_COMA,
        TT_STRING,
        TT_IDENTIFIER,
        TT_NUMERIC
    };

    /* Represents a token. For easier manipulation. */
    struct token
    {
        token_type type;
        std::string value;
    };

    /*
        Used by parser to know wich token is expected depending of the actual instruction.
        Prefixed by ET_ (EXPECTED TOKEN_).
    */
    enum expected_token
    {
        ET_OPCODE,
        ET_COMA,
        ET_IDENTIFIER,
        ET_IDENTIFIER_OR_NUMERIC,
        ET_IDENTIFIER_OR_NUMERIC_OR_STRING
    };

    /* Used by parser to know if a token type match an expectation. */
    bool match_expectation(expected_token e_type, token g_token)
    {
        token_type t_type = g_token.type;

        switch(e_type)
        {
            case ET_OPCODE:
                return (t_type == TT_IDENTIFIER) && is_opcode(g_token.value);
                break;
            case ET_COMA:
                return t_type == TT_COMA;
                break;
            case ET_IDENTIFIER:
                return t_type == TT_IDENTIFIER;
                break;
            case ET_IDENTIFIER_OR_NUMERIC:
                return (t_type == TT_IDENTIFIER) || (t_type == TT_NUMERIC);
                break;
            case ET_IDENTIFIER_OR_NUMERIC_OR_STRING:
                return (t_type == TT_IDENTIFIER) || (t_type == TT_NUMERIC) || (t_type == TT_STRING);
                break;
            default:
                return false;
                break;
        }
    }

    /* DEBUG ONLY. Return the string representation of the type of the token. */
    std::string print_type(token g_token)
    {
        switch(g_token.type)
        {
            case data::TT_IDENTIFIER:
                return "identifier";
                break;
            case data::TT_COMA:
                return "coma";
                break;
            case data::TT_STRING:
                return "string";
                break;
            case data::TT_NUMERIC:
                return "numeric";
                break;
            default:
                return "";
                break;
        }
    }

    /* DEBUG ONLY. Return the string representation of the expected type. */
    std::string print_type(expected_token t_type)
    {
        switch(t_type)
        {
            case ET_OPCODE:
                return "opcode";
                break;
            case ET_COMA:
                return "coma";
                break;
            case ET_IDENTIFIER:
                return "identifier";
                break;
            case ET_IDENTIFIER_OR_NUMERIC:
                return "identifier_or_numeric";
                break;
            case ET_IDENTIFIER_OR_NUMERIC_OR_STRING:
                return "identifier_or_numeric_or_string";
                break;
            default:
                return "none:" + string_utils::from<expected_token>(t_type);
                break;
        }
    }

} // data namespace.

/* Some useful runtime structures and enums. */
namespace runtime
{
    /* Used to indicate the opcode of an instruction. */
    enum opcode
    {
        MOV,
        ADD,
        MUL,
        CMP_EQ,
        CMP_GT,
        CMP_LT,
        NEG,
        OUT,
        IN,
        GET,
        STOP,
        FLUSH,
        LABEL,
        JMP,
        JNZ,
        JZ,
        NUM,
        STR,
        NUM_INT,
        SEED_RANDOM,
        NONE
    };

    /* Represents an instruction for easier manipulation. */
    struct instruction
    {
        opcode op;
        data::token* f_arg;
        data::token* s_arg;
    };

    /* DEBUG ONLY. Returns the string representation of the opcode. */
    std::string print_opcode(opcode op)
    {
        switch(op)
        {
            case MOV:
                return "mov";
                break;
            case ADD:
                return "add";
                break;
            case MUL:
                return "mul";
                break;
            case CMP_EQ:
                return "cmp_eq";
                break;
            case CMP_GT:
                return "cmp_gt";
                break;
            case CMP_LT:
                return "cmp_lt";
                break;
            case NEG:
                return "neg";
                break;
            case OUT:
                return "out";
                break;
            case IN:
                return "in";
                break;
            case GET:
                return "get";
                break;
            case FLUSH:
                return "flush";
                break;
            case JMP:
                return "jmp";
                break;
            case JNZ:
                return "jnz";
                break;
            case JZ:
                return "jz";
                break;
            case NUM:
                return "num";
                break;
            case STR:
                return "str";
                break;
            case NUM_INT:
                return "num_int";
                break;
            case LABEL:
                return "label";
                break;
            case STOP:
                return "stop";
                break;
            case SEED_RANDOM:
                return "seed_random";
                break;
            default:
                return "";
                break;
        }
    }
    /* Return the opcode CODE depending of the opcode STRING representation. */
    opcode get_opcode(std::string x)
    {
        x = string_utils::lowercase(x);

        if(x == "mov")
            return MOV;
        else if(x == "add")
            return ADD;
        else if(x == "mul")
            return MUL;
        else if(x == "cmp_eq")
            return CMP_EQ;
        else if(x == "cmp_gt")
            return CMP_GT;
        else if(x == "cmp_lt")
            return CMP_LT;
        else if(x == "neg")
            return NEG;
        else if(x == "out")
            return OUT;
        else if(x == "in")
            return IN;
        else if(x == "get")
            return GET;
        else if(x == "flush")
            return FLUSH;
        else if(x == "stop")
            return STOP;
        else if(x == "label")
            return LABEL;
        else if(x == "jmp")
            return JMP;
        else if(x == "jz")
            return JZ;
        else if(x == "jnz")
            return JNZ;
        else if(x == "num")
            return NUM;
        else if(x == "str")
            return STR;
        else if(x == "num_int")
            return NUM_INT;
        else if(x == "seed_random")
            return SEED_RANDOM;

        return NONE;
    }

    /* Return the number of arguments needed to complete an instruction (depending of the opcode). */
    int get_number_of_args_needed(opcode g_opcode)
    {
        switch(g_opcode)
        {
            case MOV:
            case ADD:
            case MUL:
            case CMP_EQ:
            case CMP_GT:
            case CMP_LT:
                return 2;
            case NEG:
            case OUT:
            case IN:
            case GET:
            case JMP:
            case JNZ:
            case JZ:
            case NUM:
            case STR:
            case NUM_INT:
            case LABEL:
                return 1;
            // case STOP:
            // case FLUSH:
            // case SEED_RANDOM:
            default:
                return 0;
                break;
        }
    }

    /* Returns the token type of the argument needed (depending of the opcode and the argument number). */
    data::expected_token get_expected_argument_type(opcode op, int argument_number)
    {
        switch(op)
        {
            case MOV:
            case ADD:
            case MUL:
            case CMP_EQ:
            case CMP_GT:
            case CMP_LT:
                if(argument_number == 1)
                    return data::ET_IDENTIFIER;

                return data::ET_IDENTIFIER_OR_NUMERIC_OR_STRING;
                break;
            case OUT:
                return data::ET_IDENTIFIER_OR_NUMERIC_OR_STRING;
                break;
            case NEG:
            case IN:
            case GET:
            case JMP:
            case JNZ:
            case JZ:
            case NUM:
            case STR:
            case NUM_INT:
            case LABEL:
                return data::ET_IDENTIFIER;
                break;
            // case STOP:
            // case FLUSH:
            // case SEED_RANDOM:
            default:
                return data::ET_OPCODE;
                break;
        }
    }

    /* Represents a dynamic variable type. */
    enum dynamic_variable_type
    {
        DVT_NUMERIC,
        DVT_STRING
    };

    /* Used by the runtime to represent a dynamically typed variable. */
    struct dynamic_variable
    {
        dynamic_variable_type type;
        std::string value;
    };

} // runtime namespace.

/*
    Very basic lexer.

    FIXME: The for loop used to read the line is dirty.
*/
std::vector<data::token> lex(std::ifstream &inputfile)
{
    std::vector<data::token> tokens;
    std::string line("");

    /* We read the file line by line. */
    while(std::getline(inputfile, line))
    {
        // First we trim the line.
        line = string_utils::trim(line);

        // We check if the line is not a comment or not empty.
        if(line.size() < 1 || line.at(0) == ';')
            continue;

        // We are starting the reading.
        data::token current_token; // The current token.
        std::string word(""); // The current word.
        bool is_string(false), is_escaped(false); // To indicate if we are in a string and if we are

        for(unsigned int i(0) ; i < line.size() ; ++i) // FIXME: this loop is dirty, need to find a more elegant way to do it.
        {
            char cchar(line.at(i));

            // If we met a coma and we are not in a string, it's considered as a token.
            if(cchar == ',' && !is_string)
            {
                // We initialize the previous token, if there was.
                if(word != "")
                {
                    if(data::is_number(word))
                        current_token.type = data::TT_NUMERIC;
                    else
                        current_token.type = data::TT_IDENTIFIER;

                    current_token.value = word;

                    // We clear the current word.
                    word = "";

                    // Then we push our token in the token list.
                    tokens.push_back(current_token);
                }

                // And then we initializa the coma token.
                current_token.type = data::TT_COMA;
                current_token.value = ",";

                // Then we push our token in the token list.
                tokens.push_back(current_token);
            }
            // If we are in a string and an un-escaped slash appears, we know the next char will be escaped.
            else if(cchar == '\\' && is_string && !is_escaped)
            {
                is_escaped = true;
            }
            // If we are not in a string and a double-quote appears, we know that we are starting a string.
            else if(cchar == '"' && !is_string)
            {
                is_string = true;
            }
            // If we are in a string and an un-escaped double-quote appears, we know that we have finished the string.
            else if(cchar == '"' && is_string && !is_escaped)
            {
                // End of string.
                is_string = false;

                // We initialize the token.
                current_token.type = data::TT_STRING;
                current_token.value = word;

                // We clear the current word.
                word = "";

                // Then we push our token in the token list.
                tokens.push_back(current_token);
            }
            // If we are not in a string and a space appears, it's the end of the token.
            else if(isspace(cchar) && !is_string)
            {
                if(word != "")
                {
                    // We initialize the token.
                    if(data::is_number(word))
                        current_token.type = data::TT_NUMERIC;
                    else
                        current_token.type = data::TT_IDENTIFIER;

                    current_token.value = word;

                    // We clear the current word.
                    word = "";

                    // Then we push our token in the token list.
                    tokens.push_back(current_token);
                }
            }
            else
            {
                // We put the char into the current word.
                word += cchar;
                is_escaped = false;
            }
        }

        // If word is not empty.
        if(word != "")
        {
            // We initialize the last token.
            if(word == ",")
                current_token.type = data::TT_COMA;
            else if(data::is_number(word))
                current_token.type = data::TT_NUMERIC;
            else
                current_token.type = data::TT_IDENTIFIER;

            current_token.value = word;

            // Then we push it in the token list.
            tokens.push_back(current_token);
        }
    }

    return tokens;
}

/* Very basic parser. */
std::vector<runtime::instruction> parse(std::vector<data::token> tokens)
{
    std::vector<runtime::instruction> instructions;
    runtime::instruction current_instruction{runtime::NONE, 0, 0};
    data::expected_token expected_token_type(data::ET_OPCODE); // At the beginning we expect an opcode.

    enum {IE_OPCODE, IE_FARG, IE_COMA, IE_SARG} instruction_element = IE_OPCODE; // Used to know wich element we need to complete the instruction.
    bool instruction_complete = false; // Used to know if current instruction is complete.

    /*
        We parse all the tokens.

        An instruction is either :
            opcode identifier|numeric|string coma identifier|numeric|string
            opcode identifier|numeric|string
            opcode
    */
    for(unsigned int i(0) ; i < tokens.size() ; ++i)
    {
        // If the token is not expected.
        if(!match_expectation(expected_token_type, tokens.at(i)))
        {
            std::cout << "[" << i << "] Unexpected token: (" << data::print_type(tokens.at(i)) << ") \"" << tokens.at(i).value << "\"." << std::endl;
            std::cout << "[" << i << "] Expecting: (" << data::print_type(expected_token_type) << ")." << std::endl;
            exit(1);
        }
        else
        {
            // We check wich element we need to complete the instruction.
            switch(instruction_element)
            {
                case IE_OPCODE:
                    current_instruction.op = runtime::get_opcode(tokens.at(i).value);

                    // Depending of the opcode we need (or don't need) an argument.
                    if(runtime::get_number_of_args_needed(current_instruction.op) > 0)
                    {
                        instruction_element = IE_FARG;
                        expected_token_type = runtime::get_expected_argument_type(current_instruction.op, 1);
                    }
                    else // We have a full instruction.
                    {
                        instruction_complete = true;
                        expected_token_type = data::ET_OPCODE;
                    }

                    break;
                case IE_FARG:
                    current_instruction.f_arg = new data::token(tokens.at(i));

                    // Depending of the opcode we need (or don't need) another argument.
                    if(runtime::get_number_of_args_needed(current_instruction.op) > 1)
                    {
                        instruction_element = IE_COMA;
                        expected_token_type = data::ET_COMA;
                    }
                    else // We have a full instruction.
                    {
                        instruction_complete = true;
                        expected_token_type = data::ET_OPCODE;
                    }
                    break;
                case IE_COMA:
                    // After the first argument, we have the coma. Then we have the second argument.
                    instruction_element = IE_SARG;
                    expected_token_type = runtime::get_expected_argument_type(current_instruction.op, 2);
                    break;
                case IE_SARG:
                    current_instruction.s_arg = new data::token(tokens.at(i));

                    // We have a full instruction.
                    instruction_complete = true;
                    expected_token_type = data::ET_OPCODE;
                    break;
                default:
                    std::cout << "Unknown parser error. Sorry." << std::endl;
                    exit(2);
                    break;
            }
        }

        // We check if the instrucion is complete.
        if(instruction_complete)
        {
            // We push the instruction and reset it.
            instructions.push_back(current_instruction);
            instruction_complete = false; // Reset the instruction complete flag.
            instruction_element = IE_OPCODE; // Reset the next element flag.
            current_instruction = {runtime::NONE, 0, 0}; // This is a null instruction.
        }
    }

    return instructions;
}

/* Very basic runtime. */
int run(std::vector<runtime::instruction> instructions)
{
    /*
        cip : current instruction pointer
            -> index of the current instruction in the instructions vector.
    */
    std::map<std::string, unsigned int> labels; // The map of the labels.

    /* Remember the position of each label. */
    for(unsigned int cip(0) ; cip < instructions.size() ; ++cip)
    {
        if(instructions.at(cip).op == runtime::LABEL)
            labels[instructions.at(cip).f_arg->value] = cip;
    }

    // For rand().
    srand(time(NULL));

    // Prepare memory.
    std::map<std::string, runtime::dynamic_variable> memory;
    memory["cmp_register"] = runtime::dynamic_variable{runtime::DVT_NUMERIC, "0"};
    memory["random_max"] = runtime::dynamic_variable{runtime::DVT_NUMERIC, string_utils::from<int>(10000)};
    memory["random_int"] = runtime::dynamic_variable{runtime::DVT_NUMERIC, string_utils::from<int>(rand() % 1000)};
    memory["random_num"] = runtime::dynamic_variable{runtime::DVT_NUMERIC, string_utils::from<double>(rand() % 10000)};


    // Let's go ! \o/
    for(unsigned int cip(0) ; cip < instructions.size() ; ++cip)
    {
        switch(instructions.at(cip).op)
        {
            case runtime::MOV:
                // We need a new local context to prevent compiler errors about new_variable.
                {
                    // We create a new variable.
                    runtime::dynamic_variable new_variable;

                    // We detect the variable type.
                    if(instructions.at(cip).s_arg->type == data::TT_NUMERIC)
                    {
                        new_variable.type = runtime::DVT_NUMERIC;
                        new_variable.value = instructions.at(cip).s_arg->value;
                    }
                    else if(instructions.at(cip).s_arg->type == data::TT_STRING)
                    {
                        new_variable.type = runtime::DVT_STRING;
                        new_variable.value = instructions.at(cip).s_arg->value;
                    }
                    else if(instructions.at(cip).s_arg->type == data::TT_IDENTIFIER)
                    {
                        try
                        {
                            new_variable = memory.at(instructions.at(cip).s_arg->value);
                        }
                        catch(const std::out_of_range& e)
                        {
                            // Error.
                            std::cerr << std::endl << "[MOV-VAR-VAR][ERROR] Unknown variable : " << instructions.at(cip).s_arg->value << std::endl;
                            return 3;
                        }
                    }

                    // We add it to the memory (or overwrite the old one with the same name).
                    memory[instructions.at(cip).f_arg->value] = new_variable;
                }
                break;
            case runtime::ADD:
                {
                    // Add a variable and a value or two variable.
                    // First arg must be a variable (used to store the result).

                    // We check if the second arg is a variable. If yes we use its value.
                    if(instructions.at(cip).s_arg->type == data::TT_IDENTIFIER)
                    {
                        try
                        {
                            /**
                                4 cases :
                                    str + str   -> normal
                                    num + num   -> normal
                                    str + num   -> num converted to str
                                    num + str   -> num converted to str
                            **/
                            runtime::dynamic_variable first_variable(memory.at(instructions.at(cip).f_arg->value)), second_variable(memory.at(instructions.at(cip).s_arg->value));

                            if(first_variable.type == runtime::DVT_STRING) // No conversions needed.
                            {
                                first_variable.value += second_variable.value;
                            }
                            else
                            {
                                // num + num
                                if(second_variable.type == runtime::DVT_NUMERIC)
                                {
                                    // We convert to double to add, then convert it back to std::string.
                                    first_variable.value = string_utils::from<double>(string_utils::to<double>(first_variable.value) + string_utils::to<double>(second_variable.value));
                                }
                                // num + str
                                else
                                {
                                    // We need to change the type.
                                    first_variable.type = runtime::DVT_STRING;
                                    first_variable.value = first_variable.value + second_variable.value;
                                }
                            }

                            // Finally we store the result in the first variable.
                            memory.at(instructions.at(cip).f_arg->value) = first_variable;
                        }
                        catch(const std::out_of_range& e)
                        {
                            // Error.
                            std::cerr << std::endl << "[ADD-VAR-VAR][ERROR] Unknown variable : " << instructions.at(cip).f_arg->value << std::endl;
                            return 3;
                        }
                    }
                    else
                    {
                        try
                        {
                            runtime::dynamic_variable first_variable(memory.at(instructions.at(cip).f_arg->value));
                            runtime::dynamic_variable second_variable;

                            if(instructions.at(cip).s_arg->type == data::TT_NUMERIC)
                                second_variable = {runtime::DVT_NUMERIC, instructions.at(cip).s_arg->value};
                            else
                                second_variable = {runtime::DVT_STRING, instructions.at(cip).s_arg->value};

                            // str + str
                            // str + num
                            if(first_variable.type == runtime::DVT_STRING) // No conversions needed.
                            {
                                first_variable.value += second_variable.value;
                            }
                            else
                            {
                                // num + num
                                if(second_variable.type == runtime::DVT_NUMERIC)
                                {
                                    // We convert to double to add, then convert it back to std::string.
                                    first_variable.value = string_utils::from<double>(string_utils::to<double>(first_variable.value) + string_utils::to<double>(second_variable.value));
                                }
                                // num + str
                                else
                                {
                                    // We need to change the type.
                                    first_variable.type = runtime::DVT_STRING;
                                    first_variable.value = first_variable.value + second_variable.value;
                                }
                            }

                            // Finally we store the result in the first variable.
                            memory.at(instructions.at(cip).f_arg->value) = first_variable;
                        }
                        catch(const std::out_of_range& e)
                        {
                            // Error.
                            std::cerr << std::endl << "[ADD-VAR-VAL][ERROR] Unknown variable : " << instructions.at(cip).f_arg->value << std::endl;
                            return 3;
                        }
                    }
                }
                break;
            case runtime::MUL:
                {
                    // Mul a variable and a value or two variable.
                    // First arg must be a variable (used to store the result).

                    // We check if the second arg is a variable. If yes we use its value.
                    if(instructions.at(cip).s_arg->type == data::TT_IDENTIFIER)
                    {
                        try
                        {
                            /**
                                2 cases :
                                    num * num   -> normal
                                    str * num   -> str repeated num times.
                            **/
                            runtime::dynamic_variable first_variable(memory.at(instructions.at(cip).f_arg->value)), second_variable(memory.at(instructions.at(cip).s_arg->value));

                            // num * num
                            if(first_variable.type == runtime::DVT_NUMERIC) // No conversions needed.
                            {
                                first_variable.value =  string_utils::from<double>(string_utils::to<double>(first_variable.value) * string_utils::to<double>(second_variable.value));
                            }
                            // str * num
                            else
                            {
                                unsigned int times = string_utils::to<unsigned int>(second_variable.value);

                                for(unsigned int i(0) ; i < times ; ++i)
                                    first_variable.value += first_variable.value;
                            }

                            // Finally we store the result in the first variable.
                            memory.at(instructions.at(cip).f_arg->value) = first_variable;
                        }
                        catch(const std::out_of_range& e)
                        {
                            // Error.
                            std::cerr << std::endl << "[MUL-VAR-VAR][ERROR] Unknown variable : " << instructions.at(cip).f_arg->value << std::endl;
                            return 3;
                        }
                    }
                    else
                    {
                        try
                        {
                            /**
                                2 cases :
                                    num * num   -> normal
                                    str * num   -> str repeated num times.
                            **/
                            runtime::dynamic_variable first_variable(memory.at(instructions.at(cip).f_arg->value));
                            runtime::dynamic_variable second_variable = {runtime::DVT_NUMERIC, instructions.at(cip).s_arg->value};

                            // num * num
                            if(first_variable.type == runtime::DVT_NUMERIC) // No conversions needed.
                            {
                                first_variable.value =  string_utils::from<double>(string_utils::to<double>(first_variable.value) * string_utils::to<double>(second_variable.value));
                            }
                            // str * num
                            else
                            {
                                unsigned int times = string_utils::to<unsigned int>(second_variable.value);

                                for(unsigned int i(0) ; i < times ; ++i)
                                    first_variable.value += first_variable.value;
                            }

                        }
                        catch(const std::out_of_range& e)
                        {
                            // Error.
                            std::cerr << std::endl << "[MUL-VAR-VAL][ERROR] Unknown variable : " << instructions.at(cip).f_arg->value << std::endl;
                            return 3;
                        }
                    }
                }
                break;
            case runtime::CMP_EQ:
                {
                    // Compare a variable and a value or two variable.
                    // Check if equals.
                    // THE TYPE IS NOT USED IN COMPARISONS.
                    // First arg must be a variable.
                    // Result is stored into special variable "cmp_register".

                    // We check if the second arg is a variable. If yes we use its value.
                    if(instructions.at(cip).s_arg->type == data::TT_IDENTIFIER)
                    {
                        try
                        {
                            if(memory.at(instructions.at(cip).f_arg->value).value == memory.at(instructions.at(cip).s_arg->value).value)
                                memory.at("cmp_register").value = "1";
                            else
                                memory.at("cmp_register").value = "0";
                        }
                        catch(const std::out_of_range& e)
                        {
                            // Error.
                            std::cerr << std::endl << "[CMP_EQ-VAR-VAR][ERROR] Unknown variable : " << instructions.at(cip).f_arg->value << std::endl;
                            return 3;
                        }
                    }
                    else // If the second argument is not a variable.
                    {
                        try
                        {
                            if(memory.at(instructions.at(cip).f_arg->value).value == instructions.at(cip).s_arg->value)
                                memory.at("cmp_register").value = "1";
                            else
                                memory.at("cmp_register").value = "0";
                        }
                        catch(const std::out_of_range& e)
                        {
                            // Error.
                            std::cerr << std::endl << "[CMP_EQ-VAR-VAL][ERROR] Unknown variable : " << instructions.at(cip).f_arg->value << std::endl;
                            return 3;
                        }
                    }
                }
                break;
            case runtime::CMP_GT:
                {
                    // Compare a variable and a value or two variable.
                    // Check if first > second.
                    // THE TYPE IS NOT USED IN COMPARISONS.
                    // First arg must be a variable.
                    // Result is stored into special variable "cmp_register".

                    // We check if the second arg is a variable. If yes we use its value.
                    if(instructions.at(cip).s_arg->type == data::TT_IDENTIFIER)
                    {
                        try
                        {
                            if(memory.at(instructions.at(cip).f_arg->value).value > memory.at(instructions.at(cip).s_arg->value).value)
                                memory.at("cmp_register").value = "1";
                            else
                                memory.at("cmp_register").value = "0";
                        }
                        catch(const std::out_of_range& e)
                        {
                            // Error.
                            std::cerr << std::endl << "[CMP_GT-VAR-VAR][ERROR] Unknown variable : " << instructions.at(cip).f_arg->value << std::endl;
                            return 3;
                        }
                    }
                    else // If the second argument is not a variable.
                    {
                        try
                        {
                            if(memory.at(instructions.at(cip).f_arg->value).value > instructions.at(cip).s_arg->value)
                                memory.at("cmp_register").value = "1";
                            else
                                memory.at("cmp_register").value = "0";
                        }
                        catch(const std::out_of_range& e)
                        {
                            // Error.
                            std::cerr << std::endl << "[CMP_GT-VAR-VAL][ERROR] Unknown variable : " << instructions.at(cip).f_arg->value << std::endl;
                            return 3;
                        }
                    }
                }
                break;
            case runtime::CMP_LT:
                {
                    // Compare a variable and a value or two variable.
                    // Check if first < second.
                    // THE TYPE IS NOT USED IN COMPARISONS.
                    // First arg must be a variable.
                    // Result is stored into special variable "cmp_register".

                    // We check if the second arg is a variable. If yes we use its value.
                    if(instructions.at(cip).s_arg->type == data::TT_IDENTIFIER)
                    {
                        try
                        {
                            if(memory.at(instructions.at(cip).f_arg->value).value < memory.at(instructions.at(cip).s_arg->value).value)
                                memory.at("cmp_register").value = "1";
                            else
                                memory.at("cmp_register").value = "0";
                        }
                        catch(const std::out_of_range& e)
                        {
                            // Error.
                            std::cerr << std::endl << "[CMP_LT-VAR-VAR][ERROR] Unknown variable : " << instructions.at(cip).f_arg->value << std::endl;
                            return 3;
                        }
                    }
                    else // If the second argument is not a variable.
                    {
                        try
                        {
                            if(memory.at(instructions.at(cip).f_arg->value).value < instructions.at(cip).s_arg->value)
                                memory.at("cmp_register").value = "1";
                            else
                                memory.at("cmp_register").value = "0";
                        }
                        catch(const std::out_of_range& e)
                        {
                            // Error.
                            std::cerr << std::endl << "[CMP_LT-VAR-VAL][ERROR] Unknown variable : " << instructions.at(cip).f_arg->value << std::endl;
                            return 3;
                        }
                    }
                }
                break;
            case runtime::NEG:
                {
                    // Negate a variable.
                    if(instructions.at(cip).f_arg->type == data::TT_IDENTIFIER)
                    {
                        try
                        {
                            // Try to negate the variable.
                            std::string value = memory.at(instructions.at(cip).f_arg->value).value;

                            double converted_value = string_utils::to<double>(value);
                            converted_value *= (-1);
                            value = string_utils::from<double>(converted_value);

                            memory.at(instructions.at(cip).f_arg->value).value = value;
                        }
                        catch(const std::out_of_range& e)
                        {
                            // Error.
                            std::cerr << std::endl << "[NEG-VAR][ERROR] Unknown variable : " << instructions.at(cip).f_arg->value << std::endl;
                            return 3;
                        }
                    }
                    else
                    {
                        // We are lazy, if not a variable, we do not do anything.
                        // Remember that this is not an error. You are allowed to negate a numeric value.
                    }
                }
                break;
            case runtime::OUT:
                // Prints the given argument.
                // If argument is a variable we have to print it.
                if(instructions.at(cip).f_arg->type == data::TT_IDENTIFIER)
                {
                    // We test for special identifier endline, wich correspond to std::endl;
                    if(instructions.at(cip).f_arg->value == "endline")
                    {
                        std::cout << std::endl;
                    }
                    else
                    {
                        try
                        {
                            // Try to print the variable.
                            std::cout << memory.at(instructions.at(cip).f_arg->value).value;
                        }
                        catch(const std::out_of_range& e)
                        {
                            // Error.
                            std::cerr << std::endl << "[OUT-VAR][ERROR] Unknown variable : " << instructions.at(cip).f_arg->value << std::endl;
                            return 3;
                        }
                    }
                }
                else
                {
                    // Else we just print the value.
                    std::cout << instructions.at(cip).f_arg->value;
                }
                break;
            case runtime::IN:
                // Gets input from user (one word).
                try
                {
                    // Try to get input in the variable.
                    memory.at(instructions.at(cip).f_arg->value).type = runtime::DVT_STRING;
                    std::cin >> memory.at(instructions.at(cip).f_arg->value).value;
                }
                catch(const std::out_of_range& e)
                {
                    // Error.
                    std::cerr << std::endl << "[IN-VAR][ERROR] Unknown variable : " << instructions.at(cip).f_arg->value << std::endl;
                    return 3;
                }
                break;
            case runtime::GET:
                // Gets input from user (one character).
                // We test for special identifier endline, wich correspond to std::endl;
                try
                {
                    // Try to get input in the variable.
                    memory.at(instructions.at(cip).f_arg->value).type = runtime::DVT_STRING;

                    // Gets only one char.
                    char input = std::cin.get();

                    // Store input in variable.
                    memory.at(instructions.at(cip).f_arg->value).value = "" + input; // Add char* and char to get a std::string...
                }
                catch(const std::out_of_range& e)
                {
                    // Error.
                    std::cerr << std::endl << "[IN-VAR][ERROR] Unknown variable : " << instructions.at(cip).f_arg->value << std::endl;
                    return 3;
                }
                break;
            case runtime::FLUSH:
                std::cin.clear();
                std::cin.ignore(INT_MAX, '\n');
                break;
            case runtime::STOP:
                // Stop the runtime.
                return 0;
                break;
            case runtime::LABEL:
                // Nothing to do.
                break;
            case runtime::JMP:
                try
                {
                    // Jump to the indicated label.
                    cip = labels[instructions.at(cip).f_arg->value];
                }
                catch(const std::out_of_range& e)
                {
                    // Error.
                    std::cerr << std::endl << "[JMP-LABEL][ERROR] Unknown label : " << instructions.at(cip).f_arg->value << std::endl;
                    return 3;
                }
                break;
            case runtime::JNZ:
                try
                {
                    // Jump to the indicated label if special variable "cmp_register" is different than 0.
                    if(string_utils::to<int>(memory.at("cmp_register").value) != 0)
                        cip = labels[instructions.at(cip).f_arg->value];
                }
                catch(const std::out_of_range& e)
                {
                    // Error.
                    std::cerr << std::endl << "[JNZ-LABEL][ERROR] Unknown label : " << instructions.at(cip).f_arg->value << std::endl;
                    return 3;
                }
                break;
            case runtime::JZ:
                try
                {
                    // Jump to the indicated label if special variable "cmp_register" equals 0.
                    if(string_utils::to<int>(memory.at("cmp_register").value) == 0)
                        cip = labels[instructions.at(cip).f_arg->value];
                }
                catch(const std::out_of_range& e)
                {
                    // Error.
                    std::cerr << std::endl << "[JZ-LABEL][ERROR] Unknown label : " << instructions.at(cip).f_arg->value << std::endl;
                    return 3;
                }
                break;
            case runtime::NUM:
                try
                {
                    // Try to convert variable.
                    memory.at(instructions.at(cip).f_arg->value).type = runtime::DVT_NUMERIC;

                    // Now we convert to double and convert it back to string to have a valid numeric variable.
                    memory.at(instructions.at(cip).f_arg->value).value = string_utils::from<double>(string_utils::to<double>(memory.at(instructions.at(cip).f_arg->value).value));
                }
                catch(const std::out_of_range& e)
                {
                    // Error.
                    std::cerr << std::endl << "[NUM-VAR][ERROR] Unknown variable : " << instructions.at(cip).f_arg->value << std::endl;
                    return 3;
                }
                break;
            case runtime::STR:
                try
                {
                    // Try to convert variable.
                    memory.at(instructions.at(cip).f_arg->value).type = runtime::DVT_STRING;
                }
                catch(const std::out_of_range& e)
                {
                    // Error.
                    std::cerr << std::endl << "[STR-VAR][ERROR] Unknown variable : " << instructions.at(cip).f_arg->value << std::endl;
                    return 3;
                }
                break;
            case runtime::NUM_INT:
                try
                {
                    // Try to convert variable.
                    memory.at(instructions.at(cip).f_arg->value).type = runtime::DVT_NUMERIC;

                    // Now we convert to double and convert it back to string to have a valid numeric variable.
                    memory.at(instructions.at(cip).f_arg->value).value = string_utils::from<int>(string_utils::to<int>(memory.at(instructions.at(cip).f_arg->value).value));
                }
                catch(const std::out_of_range& e)
                {
                    // Error.
                    std::cerr << std::endl << "[NUM_INT-VAR][ERROR] Unknown variable : " << instructions.at(cip).f_arg->value << std::endl;
                    return 3;
                }
                break;
            case runtime::SEED_RANDOM:
                {
                    int random_max = string_utils::to<int>(memory.at("random_max").value);

                    memory.at("random_int").value = string_utils::from<int>(rand() % random_max);
                    memory.at("random_num").value = string_utils::from<double>(rand() % random_max);
                }
                break;
            default:
                break;
        }
    }

    return 0;
}

/* Coordinate lexer, parser and runtime. */
int load_from_file(std::string filename, bool time_measurement = false)
{
    std::ifstream inputfile(filename.c_str());
    std::vector<runtime::instruction> instructions = parse(lex(inputfile));


    if(time_measurement)
    {
        clock_t start_time = clock();
        int result = run(instructions);
        clock_t end_time = clock();

        std::cout << "----------------------------------" << std::endl;
        std::cout << "[TIME] " << (end_time - start_time) << " clicks (" << (float)(end_time - start_time)/CLOCKS_PER_SEC << " seconds.)" << std::endl;

        return result;
    }

    return run(instructions);
}

/* Main function. */
int main(int argc, char* argv[])
{
    /* Arguments check. */
    if(argc > 1 && argv[1])
    {
        if(argc > 2 && std::string(argv[2]) == std::string("-time"))
            /* Launch the interpreter with time measurement. */
            return load_from_file(std::string(argv[1]), true);

        /* Launch the interpreter. */
        return load_from_file(std::string(argv[1]));
    }

    /* Ask for filename (only read from files is supported for the moment). */
    std::string filename("");

    std::cout << "No filename given." << std::endl;
    std::cout << "Enter filename (may contains spaces) : ";
    std::getline(std::cin, filename);

    /* Launch the interpreter. */
    return load_from_file(filename);

	return 0;
}
