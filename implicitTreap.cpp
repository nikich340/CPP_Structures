#include <chrono>
#include <forward_list>
#include <random>
#include <vector>

template <class T>
struct node {
    int y, iL, iR, iP, subCnt;
    T val, add, sum;
    node(T val, int y, int iL = -1, int iR = -1) {
        this->val = val;
        this->y = y;
        this->iL = iL;
        this->iR = iR;
        this->iP = -1;
        this->subCnt = 1;
        this->sum = val;
        this->add = T(0);
    }
};
template <class T>
struct implicitTreap {
    std::vector<node<T>> nodes;
    std::forward_list<int> reservedNodes;
    int rootNode = -1;

    void update(int V) {
        if (V == -1)
            return;
        int L = nodes[V].iL;
        int R = nodes[V].iR;
        nodes[V].subCnt = size(L) + size(R) + 1;
        nodes[V].sum = nodes[V].val;
        if (L != -1) {
            nodes[V].sum += nodes[L].sum;
            nodes[L].iP = V;
        }
        if (R != -1) {
            nodes[V].sum += nodes[R].sum;
            nodes[R].iP = V;
        }
    }
    void updateAdd(int V, T addVal) {
        if (V == -1)
            return;
        nodes[V].add += addVal;
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
            nodes[L].val += nodes[L].add;
            updateAdd(nodes[L].iL, nodes[L].add);
            updateAdd(nodes[L].iR, nodes[L].add);
            nodes[L].add = T(0);

            nodes[L].iR = merge(nodes[L].iR, R);
            update(L);
            return L;
        } else {
            nodes[R].val += nodes[R].add;
            updateAdd(nodes[R].iL, nodes[R].add);
            updateAdd(nodes[R].iR, nodes[R].add);
            nodes[R].add = T(0);

            nodes[R].iL = merge(L, nodes[R].iL);
            update(R);
            return R;
        }
    }
    void split(int V, int pos, int& outL, int& outR) {
        if (V == -1) {
            outL = -1;
            outR = -1;
            return;
        }
        int newV = -1;
        int L = nodes[V].iL;
        int R = nodes[V].iR;
        int leftSize = size(L) + 1;

        if (leftSize <= pos) {
            if (R == -1) {
                outR = -1;
            } else {
                nodes[V].val += nodes[V].add;
                updateAdd(L, nodes[V].add);
                updateAdd(R, nodes[V].add);
                nodes[V].add = T(0);
                split(R, pos - leftSize, newV, outR);
            }
            nodes[V].iR = newV;
            outL = V;
            update(V);
        } else {
            if (L == -1) {
                outL = -1;
            } else {
                nodes[V].val += nodes[V].add;
                updateAdd(L, nodes[V].add);
                updateAdd(R, nodes[V].add);
                nodes[V].add = T(0);
                split(L, pos, outL, newV);
            }
            nodes[V].iL = newV;
            outR = V;
            update(V);
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
    T val(int nIdx) {
        return (nIdx != -1 ? nodes[nIdx].val + nodes[nIdx].subCnt * nodes[nIdx].add : T(0));
    }
    T valueAt(int pos) {
        return val(nodeIdx(pos));
    }
    T sumAt(int posL, int posR) {
        if (posL > posR) {
            printf("[sumAt] error (L > R)\n");
            exit(0);
        }
        int L, M, R;
        split(rootNode, posL, L, R);
        split(R, posR - posL + 1, M, R);

        T ret = nodes[M].sum;
        rootNode = merge(merge(L, M), R);
        return ret;
    }
    void addValue(int pos, T addVal) {
        int V = nodeIdx(pos);
        nodes[V].val += addVal;
        while (V != -1) {
            update(V);
            V = nodes[V].iP;
        }
    }
    void addValue(int posL, int posR, T addVal) {
        if (posL > posR) {
            printf("[sumAt] error (L > R)\n");
            exit(0);
        }
        int L, M, R;
        split(rootNode, posL, L, R);
        split(R, posR - posL + 1, M, R);

        nodes[M].add += addVal;
        rootNode = merge(merge(L, M), R);
    }
    void setValue(int pos, T newVal) {
        int V = nodeIdx(pos);
        nodes[V].val = newVal;
        while (V != -1) {
            update(V);
            V = nodes[V].iP;
        }
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
    void viewImplicitTreap(int V = -2) {
        int idx = 0;
        if (V == -2)
            V = rootNode;
        if (size(V))
            viewImplicitTreapDfs(V, idx);
        else
            printf("[view] empty implicitTreap\n");
    }
    void viewImplicitTreapDfs(int V, int& idx) {
        if (nodes[V].iL != -1) {
            viewImplicitTreapDfs(nodes[V].iL, idx);
        }
        printf("[view], at[%d] = {val = %lld, y = %d, p = %d, subCnt = %d, sum = %lld}\n",
               idx, nodes[V].val, nodes[V].y, nodes[V].iP, nodes[V].subCnt, nodes[V].sum);
        ++idx;
        if (nodes[V].iR != -1) {
            viewImplicitTreapDfs(nodes[V].iR, idx);
        }
    }
};
