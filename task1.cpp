#include <bits/stdc++.h>
#define ll long long
#define db long double
#define x first
#define y second
#define mp make_pair
#define pb push_back
#define all(a) a.begin(), a.end()

using namespace std;

extern "C" typedef struct {
    const char* name;
    void* pointer;
} symbol_t;

string convert_without_spaces(const char* expression)
{
    string res = "";
    for (int i = 0; i < strlen(expression); ++i) {
        char ch = expression[i];
        if (ch == ' ')
            continue;
        res += expression[i];
    }
    return res;
}

namespace Instructions
{
uint32_t initialization = 0xE52DE004; // push {lr}
uint32_t push_r4_r5_r6 = 0xE92D0070;  // push {r4, r5, r6}
vector<uint32_t> remove_from_stack = {0xE49D0004,
                                      0xE49D1004,
                                      0xE49D2004,
                                      0xE49D3004}; // pop {r0} , pop {r1}, ...
uint32_t load = 0xE5906000;                        // ldr r6, [r0]
uint32_t mvr0_t0_r6 = 0xE1A06000;                  // mov r6, r0
uint32_t mvr5_zero = 0xE3A05000;                   // mov r5, #0
uint32_t movw = 0xE3000000;                        // movw r0, smth
uint32_t movt = 0xE3400000;                        // movt r0, smth
uint32_t mvr0_to_r5 = 0xE1A05000;                  // mov r5, r0
uint32_t call = 0xE12FFF36;                        // blx r6
uint32_t push = 0xE52D0004;                        // push {r0}
uint32_t normal_add = 0xE0800001;                  // add r0, r1
uint32_t normal_mult = 0xE0000190;                 // mul r0, r1
uint32_t normal_sub = 0xE0400001;                  // sub r0, r1
uint32_t push_r6 = 0xE52D6004;                     // push {r6}
uint32_t pop_registers = 0xE8BD0070;               // push {r4, r5, r6}
uint32_t pop_lr = 0xE49DE004;                      // pop {lr}
uint32_t bx_lr = 0xE12FFF1E;                       // bx lr
uint32_t mvr0_zero = 0xE3A00000;                   // mov r0, #0
};                                                 // namespace Instructions

void init(vector<uint32_t>& instructions)
{
    instructions.push_back(Instructions::initialization);
    instructions.push_back(Instructions::push_r4_r5_r6);
}

void finish(uint32_t* res, vector<uint32_t>& answer)
{
    for (int i = 0; i < answer.size(); ++i) {
        *res = answer[i];
        res++;
    }
}

bool is_service_symbol(char ch)
{
    if (ch == '(' || ch == ')' || ch == '+' || ch == '*' || ch == '-' ||
        ch == ',')
        return true;
    return false;
}

bool is_digit(char ch)
{
    if (ch >= '0' && ch <= '9')
        return true;
    return false;
}

bool same_type(char& first, char& second)
{
    if (is_service_symbol(second))
        return false;
    return (is_digit(first) == is_digit(second));
}

string char_to_string(char ch)
{
    string res = "";
    res += ch;
    return res;
}

string parse_token(string& expr, int& start)
{
    if (is_service_symbol(expr[start])) {
        start++;
        return char_to_string(expr[start - 1]);
    }
    int finish = start;
    while (finish < expr.size() && same_type(expr[start], expr[finish])) {
        finish++;
    }
    string res = "";
    for (int i = start; i < finish; ++i) {
        res += expr[i];
    }

    start = finish;
    return res;
}

int last_open_bracket(vector<string>& current_stack)
{
    int candidate = current_stack.size() - 1;
    while (current_stack[candidate] != "(")
        candidate--;
    return candidate;
}

int to_int(string number)
{
    int res = 0;
    for (int i = 0; i < number.size(); ++i) {
        res *= 10;
        res += (number[i] - '0');
    }
    return res;
}

void add_mov(uint32_t code, uint32_t bits, vector<uint32_t>& answer)
{
    uint32_t small = bits & ((1 << 12) - 1);
    uint32_t lead = (bits ^ small) >> 12;
    code ^= small;
    code ^= (lead << 16);
    answer.push_back(code);
}

void load_number(uint32_t x, vector<uint32_t>& answer)
{
    uint32_t small_bits = x & ((1 << 16) - 1);
    uint32_t lead_bits = (x ^ small_bits) >> 16;
    add_mov(Instructions::movw, small_bits, answer);
    add_mov(Instructions::movt, lead_bits, answer);
}

void execute(
    string function,
    int args,
    map<string, void*>& pointers,
    vector<uint32_t>& answer)
{
    int index = (int)pointers[function];
    load_number(index, answer);
    answer.push_back(Instructions::mvr0_t0_r6);

    for (int i = args - 1; i >= 0; i--) {
        answer.push_back(Instructions::remove_from_stack[i]);
    }

    answer.push_back(Instructions::call);
    answer.push_back(Instructions::push);
}

void push_to_stack(uint32_t x, vector<uint32_t>& answer)
{
    load_number(x, answer);
    answer.push_back(Instructions::push);
}

bool action_string(string x)
{
    return (x == "*");
}

void perform_operation(
    string operation,
    vector<uint32_t>& answer,
    bool need_last_removal,
    bool need_return)
{

    answer.push_back(Instructions::remove_from_stack[1]);
    if (need_last_removal)
        answer.push_back(Instructions::remove_from_stack[0]);

    if (operation == "+") {
        answer.push_back(Instructions::normal_add);
    } else if (operation == "-") {
        answer.push_back(Instructions::normal_sub);
    } else
        answer.push_back(Instructions::normal_mult);
    if (need_return)
        answer.push_back(Instructions::push);
}

void remove_pluses_minuses(
    vector<string>& current_stack,
    map<string, void*>& pointers,
    vector<uint32_t>& answer,
    bool end)
{
    string finish;
    if (!end) {
        finish = current_stack.back();
        current_stack.pop_back();
    }
    int index = current_stack.size() - 2;
    while (index >= 0 &&
           (current_stack[index] == "-" || current_stack[index] == "+"))
        index -= 2;
    answer.push_back(Instructions::mvr0_zero);
    while (index + 2 < current_stack.size()) {
        current_stack.pop_back();
        perform_operation(current_stack.back(), answer, false, false);
        current_stack.pop_back();
    }
    perform_operation("+", answer, false, true);
    if (!end) {
        current_stack.push_back(finish);
    }
}

void process_token(
    vector<string>& current_stack,
    map<string, void*>& pointers,
    vector<uint32_t>& answer,
    bool next_open,
    bool first_launch)
{

    if (!current_stack.size())
        return;
    string token = current_stack.back();
    char first_symbol = token[0];
    if (is_service_symbol(first_symbol)) {
        if (first_symbol == ',' || first_symbol == ')') {
            remove_pluses_minuses(current_stack, pointers, answer, false);
        }
        if (first_symbol != ')') {
            return;
        }
        int index = last_open_bracket(current_stack);
        current_stack.pop_back();
        int args = 1;
        for (int i = index + 1; i < current_stack.size(); ++i) {
            if (current_stack[i] == ",")
                args++;
        }
        if (index > 0 && !is_service_symbol(current_stack[index - 1][0])) {
            while (current_stack.size() != index)
                current_stack.pop_back();
            string name = current_stack.back();
            current_stack.pop_back();
            execute(name, args, pointers, answer);
            current_stack.push_back("0");
        } else {
            while (index + 1 < current_stack.size()) {
                swap(current_stack[index], current_stack[index + 1]);
                index++;
            }
            current_stack.pop_back();
        }
        process_token(current_stack, pointers, answer, next_open, false);
        return;
    }

    else if (is_digit(first_symbol)) {

        if (first_launch)
            push_to_stack(to_int(token), answer);
        if (current_stack.size() <= 1) {
            return;
        }
        if (action_string(current_stack[current_stack.size() - 2])) {
            current_stack.pop_back();
            perform_operation(current_stack.back(), answer, true, true);
            current_stack.pop_back();
            current_stack.pop_back();
            current_stack.push_back("0");
            process_token(current_stack, pointers, answer, next_open, false);
        } else
            return;
    }

    else {
        if (next_open) {
            return;
        }

        int ptr = (int)pointers[token];
        load_number(ptr, answer);
        answer.push_back(Instructions::load);
        answer.push_back(Instructions::push_r6);
        current_stack.pop_back();
        current_stack.push_back("0");
        process_token(current_stack, pointers, answer, next_open, false);
    }
}

/*
void print(vector<uint32_t> &answer){
    for (int i=0; i < answer.size(); ++i) printf("%x ", answer[i]);
    cout << endl;
}
*/

void parse(string& expr, map<string, void*>& pointers, vector<uint32_t>& answer)
{

    vector<string> current_stack;

    int current_pointer = 0;

    while (current_pointer < expr.size()) {
        string token = parse_token(expr, current_pointer);
        current_stack.push_back(token);
        bool next_open = false;
        if (current_pointer < expr.size() && expr[current_pointer] == '(')
            next_open = true;
        process_token(current_stack, pointers, answer, next_open, true);
    }

    remove_pluses_minuses(current_stack, pointers, answer, true);
}

string remove_unary_operators(string& current_string)
{
    string res = "";
    int current_pointer = 0;
    while (current_pointer < current_string.size()) {
        if (current_string[current_pointer] != '-' || current_pointer == 0) {
            res += current_string[current_pointer++];
        } else {
            char prev = current_string[current_pointer - 1];
            if (!is_service_symbol(prev) || prev == ')') {
                res += current_string[current_pointer++];
            } else {
                res += "(0-";
                current_pointer++;
                while (current_pointer < current_string.size() &&
                       is_digit(current_string[current_pointer])) {
                    res += current_string[current_pointer++];
                }
                res += ')';
            }
        }
    }
    return res;
}

void final_steps(vector<uint32_t>& answer)
{
    answer.push_back(Instructions::remove_from_stack[0]);
    answer.push_back(Instructions::pop_registers);
    answer.push_back(Instructions::pop_lr);
    answer.push_back(Instructions::bx_lr);
}

extern "C" void jit_compile_expression_to_arm(
    const char* expression,
    const symbol_t* externs,
    void* out_buffer)
{

    string correct_string = convert_without_spaces(expression);
    correct_string = remove_unary_operators(correct_string);

    map<string, void*> pointers;

    int current_pointer = 0;
    while (true) {
        if (externs[current_pointer].name == 0)
            break;
        symbol_t cur_object = externs[current_pointer];
        string name = convert_without_spaces(externs[current_pointer].name);
        pointers[name] = externs[current_pointer].pointer;
        current_pointer++;
    }

    uint32_t* res = (uint32_t*)out_buffer;

    vector<uint32_t> answer;

    init(answer);

    parse(correct_string, pointers, answer);
    final_steps(answer);

    finish(res, answer);
}