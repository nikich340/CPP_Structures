#include "implicitTreap.cpp"
#include <iostream>

#define ll long long
#define ld long double
#define X first
#define Y second
#define upn(x, init, n) for (int x = init; x <= n; ++x)
#define upiter(x, container) for (auto x = container.begin(); x != container.end(); ++x)
#define dn(x, init, n) for(int x = init; x >= n; --x)
#define diter(x, container) for (auto x = container.rbegin(); x != container.rend(); ++x)
#define pb push_back
#define pii pair<int, int>
#define el '\n'
#define sfio() freopen("input.txt", "r", stdin); freopen("output.txt", "w", stdout);
#define PI acos(-1.0)
#define eps 0.0000001
#define mod 1000000007

using namespace std;
implicitTreap<long long int> implTreap;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

int randInt(int L, int R) {
    return uniform_int_distribution<int>(L, R)(rng);
}
int main(int argc, char** argv) {
    int n;
    cin >> n;

    return 0;
}
