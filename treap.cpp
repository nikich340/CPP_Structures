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
struct treap {
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
    void split(int V, T val, bool eq, int& L, int& R) {
        if (V == -1) {
            L = -1;
            R = -1;
            return;
        }
        int newV = -1;
        if ((eq && nodes[V].val <= val) || (!eq && nodes[V].val < val)) {
            if (nodes[V].iR == -1) {
                R = -1;
            } else {
                split(nodes[V].iR, val, true, newV, R);
            }
            nodes[V].iR = newV;
            L = V;
            update(L);
        } else {
            if (nodes[V].iL == -1) {
                L = -1;
            } else {
                split(nodes[V].iL, val, true, L, newV);
            }
            nodes[V].iL = newV;
            R = V;
            update(R);
        }
    }
    int insert(T val, int y) {
        if (!size()) {
            rootNode = newNode(val, y);
        } else {
            int L, M, R;
            split(rootNode, val, false, L, R);
            M = merge(L, newNode(val, y));
            rootNode = merge(M, R);
        }
        return rootNode;
    }
    bool erase(T val) {
        if (!size()) {
            return false;
        }
        int L, M, R;
        split(rootNode, val - 1, L, R);
        split(R, val, M, R);
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
    T at(int pos) {
        return nodes[nodeIdx(pos)].val;
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
    void viewTreap() {
        int nIdx = 0;
        if (size())
            viewTreapDfs(rootNode, nIdx);
        else
            printf("[viewTreap] empty treap\n");
    }
    void viewTreapDfs(int V, int& nIdx) {
        if (nodes[V].iL != -1) {
            viewTreapDfs(nodes[V].iL, nIdx);
        }
        printf("[viewTreap], at[%d] = {val = %d, y = %d, subCnt = %d}\n",
               nIdx, nodes[V].val, nodes[V].y, nodes[V].subCnt);
        ++nIdx;
        if (nodes[V].iR != -1) {
            viewTreapDfs(nodes[V].iR, nIdx);
        }
    }
};

std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
int randInt(int L, int R) {
    return std::uniform_int_distribution<int>(L, R)(rng);
}

treap<int> tr;

int main(int argc, char** argv) {
    int n;
    std::cin >> n;
    for (int i = 0; i < n; ++i) {
        tr.insert(i, randInt(0, INT_MAX));
    }
    for (int i = 0; i < n; ++i) {
        std::cout << tr.at(i) << '\n';
    }
    return 0;
}
