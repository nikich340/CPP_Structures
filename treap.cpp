#include <iostream>
#include <vector>

template <class T>
struct node {
    T x;
    int y;
    int iL, iR, subCnt;
    T subSum;
    node(T x, int y, int iL = -1, int iR = -1) {
        this->x = x;
        this->y = y;
        this->iL = iL;
        this->iR = iR;
        this->subCnt = 1;
        this->subSum = x;
    }
};
template <class T>
struct treap {
    std::vector<node<T>> nodes;
    const static int ERROR = -1000000007;
    int rootNode = -1;
    void update(int V) {
        nodes[V].subCnt = (nodes[V].iL != -1 ? nodes[nodes[V].iL].subCnt : 0) +
                        (nodes[V].iR != -1 ? nodes[nodes[V].iR].subCnt : 0) + 1;
        nodes[V].subSum = (nodes[V].iL != -1 ? nodes[nodes[V].iL].subSum : 0) +
                        (nodes[V].iR != -1 ? nodes[nodes[V].iR].subSum : 0) + nodes[V].x;
    }
    int newNode(T x, int y) {
        node<T> tmp(x, y);
        nodes.push_back(tmp);
        return nodes.size() - 1;
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
    void split(int V, int x, int& L, int& R) {
        if (V == -1) {
            L = -1;
            R = -1;
            return;
        }
        int newV = -1;
        if (nodes[V].x <= x) {
            if (nodes[V].iR == -1) {
                R = -1;
            } else {
                split(nodes[V].iR, x, newV, R);
            }
            nodes[V].iR = newV;
            L = V;
            update(L);
        } else {
            if (nodes[V].iL == -1) {
                L = -1;
            } else {
                split(nodes[V].iL, x, L, newV);
            }
            nodes[V].iL = newV;
            R = V;
            update(R);
        }
    }
    int insert(T x, int y) {
        if (!size()) {
            rootNode = newNode(x, y);
        } else {
            int L, M, R;
            split(rootNode, x - 1, L, R);
            M = merge(L, newNode(x, y));
            rootNode = merge(M, R);
        }
        return rootNode;
    }
    bool erase(T x) {
        if (!size()) {
            return false;
        }
        int L, M, R;
        split(rootNode, x - 1, L, R);
        split(R, x, M, R);
        rootNode = merge(L, R);
        return (M != -1);
    }
    int size(int idx) {
        return (idx != -1 ? nodes[idx].subCnt : 0);
    }
    int size() {
        return size(rootNode);
    }
    T at(int idx) {
        if (idx > size()) {
            printf("[at] out of range (size = %d, idx = %d)\n", size(), idx);
            exit(0);
        }
        int V = rootNode;
        while (V != -1) {
            int cntL = size(nodes[V].iL);
            int cntR = size(nodes[V].iR);
            if (cntL > idx) {
                V = nodes[V].iL;
            } else if (cntL < idx) {
                idx = idx - (cntL + 1);
                V = nodes[V].iR;
            } else {
                return nodes[V].x;
            }
        }
        return ERROR;
    }
    void viewTreap() {
        int idx = 0;
        if (size())
            viewTreapDfs(rootNode, idx);
        else
            printf("[viewTreap] empty treap\n");
    }
    void viewTreapDfs(int V, int& idx) {
        if (nodes[V].iL != -1) {
            viewTreapDfs(nodes[V].iL, idx);
        }
        printf("[viewTreap], at[%d] = {x = %d, y = %d, subCnt = %d}\n",
               idx, nodes[V].x, nodes[V].y, nodes[V].subCnt);
        ++idx;
        if (nodes[V].iR != -1) {
            viewTreapDfs(nodes[V].iR, idx);
        }
    }
};

treap<int> trea;

int main(int argc, char** argv) {
    int n;
    std::cin >> n;
    for(int i = 1; i <= n; ++i) {
        trea.insert(i, rand());
    }
    std::cout << "Sum of all elements = " << trea.nodes[trea.rootNode].subSum << '\n';
    return 0;
}
