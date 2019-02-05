#include <chrono>
#include <iostream>
#include <fstream>
#include <forward_list>
#include <random>
#include <vector>

template <class T>
struct node {
    T val;
    int y;
    int iL, iR, subCnt;
    node(T val, int y, int iL = -1, int iR = -1) {
        this->val = val;
        this->y = y;
        this->iL = iL;
        this->iR = iR;
        this->subCnt = 1;
    }
};
template <class T>
struct implicitTreap {
    std::vector<node<T>> nodes;
    std::forward_list<int> reservedNodes;
    int rootNode = -1;

    void update(int V) {
        nodes[V].subCnt = size(nodes[V].iL) + size(nodes[V].iR) + 1;
    }
    int newNode(T val, int y) {
        int newIdx;
        if (!reservedNodes.empty()) {
            newIdx = reservedNodes.front();
            reservedNodes.pop_front();
            nodes[newIdx].iL = nodes[newIdx].iR = -1;
            nodes[newIdx].y = y;
            nodes[newIdx].val = val;
        } else {
            node<T> tmp(val, y);
            nodes.push_back(tmp);
            newIdx = nodes.size() - 1;
        }
        return newIdx;
    }
    int merge(int L, int R) {
        if (L == -1)
            return R;
        if (R == -1)
            return L;
        if (nodes[L].y > nodes[R].y) {
            nodes[L].iR = merge(nodes[L].iR, R);
            update(L);
            return L;
        } else {
            nodes[R].iL = merge(L, nodes[R].iL);
            update(R);
            return R;
        }
    }
    void split(int V, int pos, int& L, int& R) {
        if (V == -1) {
            L = -1;
            R = -1;
            return;
        }
        int newV = -1;
        int leftSize = size(nodes[V].iL) + 1;
        if (leftSize <= pos) {
            if (nodes[V].iR == -1) {
                R = -1;
            } else {
                split(nodes[V].iR, pos - leftSize, newV, R);
            }
            nodes[V].iR = newV;
            L = V;
            update(L);
        } else {
            if (nodes[V].iL == -1) {
                L = -1;
            } else {
                split(nodes[V].iL, pos, L, newV);
            }
            nodes[V].iL = newV;
            R = V;
            update(R);
        }
    }
    int insert(T val, int pos, int y) {
        if (!size()) {
            rootNode = newNode(val, y);
        } else {
            int L, M, R;
            split(rootNode, pos, L, R);
            M = merge(L, newNode(val, y));
            rootNode = merge(M, R);
        }
        return rootNode;
    }
    bool erase(int pos) {
        if (!size()) {
            return false;
        }
        int L, M, R;
        split(rootNode, pos, L, R);
        split(R, 1, M, R);
        rootNode = merge(L, R);
        if (M != -1) {
            reservedNodes.push_front(M);
            return true;
        }
        return false;
    }
    int size(int nIdx) {
        return (nIdx != -1 ? nodes[nIdx].subCnt : 0);
    }
    int size() {
        return size(rootNode);
    }
    int nodeIdx(int pos) {
        if (pos >= size() || pos < 0) {
            printf("[nodeIdx] out of range (size = %d, pos = %d)\n", size(), pos);
            exit(0);
        }
        int V = rootNode;
        while (V != -1) {
            int cntL = size(nodes[V].iL);
            int cntR = size(nodes[V].iR);
            if (cntL > pos) {
                V = nodes[V].iL;
            } else if (cntL < pos) {
                pos = pos - (cntL + 1);
                V = nodes[V].iR;
            } else {
                return V;
            }
        }
        printf("[nodeIdx] strange error\n");
        exit(0);
    }
    T& operator[](int pos) {
        return nodes[nodeIdx(pos)].val;
    }
    void viewImplicitTreap() {
        int nIdx = 0;
        if (size())
            viewImplicitTreapDfs(rootNode, nIdx);
        else
            printf("[viewImplicitTreap] empty implicitTreap\n");
    }
    void viewImplicitTreapDfs(int V, int& nIdx) {
        if (nodes[V].iL != -1) {
            viewImplicitTreapDfs(nodes[V].iL, nIdx);
        }
        printf("[viewImplicitTreap], at[%d] = {val = %d, y = %d, subCnt = %d}\n",
               nIdx, nodes[V].val, nodes[V].y, nodes[V].subCnt);
        ++nIdx;
        if (nodes[V].iR != -1) {
            viewImplicitTreapDfs(nodes[V].iR, nIdx);
        }
    }
};

using namespace std;

implicitTreap<long long int> implTreap;
long long int ans = 0;
int n, p, k, a;
int e, v;
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

long long int sq(long long int a) { return a * a; }
int randInt(int L, int R) {
    return uniform_int_distribution<int>(L, R)(rng);
}
int main(int argc, char** argv) {
    freopen("river.in", "r", stdin);
    freopen("river.out", "w", stdout);
    cin >> n >> p;
    for(int i = 0; i < n; ++i) {
        cin >> a;
        implTreap.insert(a, i, randInt(0, INT_MAX));
        ans += (a * a);
    }
    cout << ans << '\n';
    cin >> k;
    for(int i = 0; i < k; ++i) {
        cin >> e >> v;
        --v;
        long long int len = implTreap[v];
        ans -= sq(len);
        if (e == 1) {
            if (v == 0) {
                long long int& lnk = implTreap[1];
                ans -= sq(lnk);
                lnk += len;
                ans += sq(lnk);
            } else if (v == implTreap.size() - 1) {
                long long int& lnk = implTreap[v - 1];
                ans -= sq(lnk);
                lnk += len;
                ans += sq(lnk);
            } else {
                long long int& lnk1 = implTreap[v - 1];
                long long int& lnk2 = implTreap[v + 1];
                ans -= sq(lnk1);
                ans -= sq(lnk2);
                lnk1 += len / 2;
                lnk2 += (len + 1) / 2;
                ans += sq(lnk1);
                ans += sq(lnk2);
            }
        } else {
            ans += sq((len + 1) / 2);
            ans += sq(len / 2);
            implTreap.insert((len + 1) / 2, v + 1, randInt(0, INT_MAX));
            implTreap.insert(len / 2, v + 1, randInt(0, INT_MAX));
        }
        implTreap.erase(v);
        cout << ans << '\n';
    }
    return 0;
}
