#include <bits/stdc++.h>

using namespace std;

string kek[] = {"aa", "a", "z", "zsdf"};

string gen(int h) {
    if (h == 0) {
        if (rand() % 2 == 0){
            if (rand() % 2 == 0) {
                return kek[rand() % 4];
            } else {
                return "(-" + kek[rand() % 4] + ")";
            }
        }
        if (rand() % 2 == 0) {
            return "(-" + to_string(rand() % 30) + ")";
        }
        string ans = to_string(rand() % 30);
        if (rand() % 2 == 0) ans = "(" + ans + ")";
        return ans;
    } else {
        int vm = (h - 1) >> 1;

        // functions
        if (rand() % 2 == 0){
            if (rand() % 4 == 0) {
                return "sum(" + gen(vm) + ", " + gen(h - vm - 1) + ")";
            } else if (rand() % 3 == 0) {
                return "inc(" + gen(h - 1) + ")";
            } else if (rand() % 2 == 0) {
                return "dec(" + gen(h - 1) + ")";
            }
        }

        string now = "+";
        if (rand() % 3 == 0) now = "*";
        else if (rand() % 2 == 0) now = "-";

        string ans;
        ans += gen(vm);
        ans += now;
        ans += gen(h - 1 - vm);
        if (rand() % 4 == 0) ans = "(" + ans + ")";
        return ans;
    }
}

int main(int argc, char* argv[]) {
    srand( atoi(argv[1]) );

    int n = rand() % 10 + 1;

    cout << ".vars\n";
    cout << "aa=1 a=234 z=-1 zsdf=0\n";
    cout << ".expression\n";
    cout << gen(n);
}
