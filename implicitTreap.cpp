#include <chrono>
#include <forward_list>
#include <random>
#include <vector>

#define PROTECTION 1

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
private:
    std::vector<node<T>> nodes;
    std::forward_list<int> reservedNodes;
    int rootNode = -1;

    void update(int V) {
        if (V == -1)
            return;
        int L = nodes[V].iL;
        int R = nodes[V].iR;
        nodes[V].subCnt = size(L) + size(R) + 1;
        nodes[V].sum = nodes[V].val + nodes[V].add * nodes[V].subCnt;
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
        nodes[V].sum += nodes[V].subCnt * addVal;
    }
    void pushAdd(int V) {
        nodes[V].val += nodes[V].add;
        updateAdd(nodes[V].iL, nodes[V].add);
        updateAdd(nodes[V].iR, nodes[V].add);
        nodes[V].add = T(0);
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
            pushAdd(L);
            nodes[L].iR = merge(nodes[L].iR, R);
            update(L);
            return L;
        } else {
            pushAdd(R);
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
        pushAdd(V);
        int newV = -1;
        int L = nodes[V].iL;
        int R = nodes[V].iR;
        int leftSize = size(L) + 1;

        if (leftSize <= pos) {
            if (R == -1) {
                outR = -1;
            } else {
                split(R, pos - leftSize, newV, outR);
            }
            nodes[V].iR = newV;
            outL = V;
            update(V);
        } else {
            if (L == -1) {
                outL = -1;
            } else {
                split(L, pos, outL, newV);
            }
            nodes[V].iL = newV;
            outR = V;
            update(V);
        }
    }
    int size(int nIdx) {
        return (nIdx != -1 ? nodes[nIdx].subCnt : 0);
    }
    T val(int nIdx) {
        return (nIdx != -1 ? (nodes[nIdx].val + nodes[nIdx].add) : T(0));
    }
    int nodeIdx(int pos) {
#ifdef PROTECTION
        if (pos >= size() || pos < 0) {
            printf("[nodeIdx] out of range (size = %d, pos = %d)\n", size(), pos);
            exit(0);
        }
#endif
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
        printf("[nodeIdx] node at pos %d was not found!\n", pos);
        exit(0);
    }
    void debugImplicitTreapDfs(int V, int& idx) {
        if (nodes[V].iL != -1) {
            debugImplicitTreapDfs(nodes[V].iL, idx);
        }
        printf("[debug], at[%d] = {val = %d, add = %d, y = %d, p = %d, subCnt = %d, sum = %d}\n",
               idx, nodes[V].val, nodes[V].add, nodes[V].y, nodes[V].iP, nodes[V].subCnt, nodes[V].sum);
        ++idx;
        if (nodes[V].iR != -1) {
            debugImplicitTreapDfs(nodes[V].iR, idx);
        }
    }
public:
    void insert(T val, int pos, int y) {
        if (!size()) {
            rootNode = newNode(val, y);
        } else {
            int L, R;
            split(rootNode, pos, L, R);
            rootNode = merge(merge(L, newNode(val, y)), R);
        }
    }
    void push_back(T val, int y) {
        if (!size()) {
            rootNode = newNode(val, y);
        } else {
            rootNode = merge(rootNode, newNode(val, y));
        }
    }
    bool erase(int pos) {
#ifdef PROTECTION
        if (pos >= size() || pos < 0) {
            printf("[erase] out of range (size = %d, pos = %d)\n", size(), pos);
            exit(0);
        }
#endif
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
    int size() {
        return size(rootNode);
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

        updateAdd(M, addVal);
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
    void move(int posL, int posR, int offset) {
        if (!offset)
            return;
#ifdef PROTECTION
        if (posR >= size() || posL < 0 || posR + offset >= size() || posL + offset < 0) {
            printf("[move] out of range (size = %d, posL = %d, posR = %d, offset = %d)\n", size(), posL, posR, offset);
            exit(0);
        }
#endif
        int L, M1, M2, R;
        split(rootNode, posL, L, R);
        split(R, posR - posL + 1, M1, R);
        if (offset > 0) {
            split(R, offset, M2, R);
            rootNode = merge(merge(L, M2), merge(M1, R));
        } else {
            split(L, size(L) + offset, L, M2);
            rootNode = merge(merge(L, M1), merge(M2, R));
        }
    }
    void move(int pos, int offset) {
        move(pos, pos, offset);
    }
    void shift(int offset) {
        offset = offset % size();
        if (!offset)
            return;
        int L, R;
        if (offset > 0)
            split(rootNode, size() - offset, L, R);
        else
            split(rootNode, -offset, L, R);
        rootNode = merge(R, L);
    }
    void debugImplicitTreap(int V = -2) {
        int idx = 0;
        if (V == -2)
            V = rootNode;
        if (size(V))
            debugImplicitTreapDfs(V, idx);
        else
            printf("[debug] empty implicitTreap\n");
    }
};
