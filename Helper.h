
#include <sstream>
#include <fstream>
#include <string>
#include <windows.h>
#include <cstdlib> 
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
using namespace std;


struct Btree {
    Btree** child;
    bool leaf;
    pair<int, long>* val;
    int curr;

    Btree(int _t, bool _leaf = true) {
        leaf = _leaf;
        curr = 0;
        val = new pair<int, long>[2 * _t - 1];
        child = new Btree * [2 * _t];
    }

    ~Btree() {
        delete[] val;
        delete[] child;
    }
};

class B_tree {
private:
    Btree* root;
    int num; // Minimum degree

    void insert_to_fill(Btree* N, const pair<int, std::streampos>& val) {
        int i = N->curr - 1;

        if (N->leaf) {
            while (i >= 0 && val.first < N->val[i].first) {
                N->val[i + 1] = N->val[i];
                i--;
            }
            N->val[i + 1] = val;
            N->curr++;
        }
        else {
            while (i >= 0 && val.first < N->val[i].first) {
                i--;
            }
            i++;
            if (N->child[i]->curr == (2 * num - 1)) {
                split_children(N, i);
                if (val.first > N->val[i].first) {
                    i++;
                }
            }
            insert_to_fill(N->child[i], val);
        }
    }
    long retrieveFromFile(Btree* N, int key) {
        if (!N) {
            cout << "Tree is empty." << endl;
            return -1; // Or handle error appropriately
        }
        int i = 0;
        while (i < N->curr && key > N->val[i].first) {
            i++;
        }
        if (i < N->curr&& key == N->val[i].first) {
            return N->val[i].second;
        }
        else if (N->leaf) {
            cout << "Key not found." << endl;
            return -1; // Or handle error appropriately
        }
        else {
            return retrieveFromFile(N->child[i], key);
        }
    }
    void split_children(Btree* par, int ind) {
        Btree* child = par->child[ind];

        Btree* newChild = new Btree(num, child->leaf);
        par->curr++;
        par->child[par->curr] = nullptr;

        for (int i = par->curr - 1; i > ind; i--) {
            par->child[i + 1] = par->child[i];
        }
        par->child[ind + 1] = newChild;

        for (int i = par->curr - 2; i >= ind; i--) {
            par->val[i + 1] = par->val[i];
        }
        par->val[ind] = child->val[num - 1];

        newChild->curr = num - 1;

        for (int i = 0; i < num - 1; i++) {
            newChild->val[i] = child->val[i + num];
        }

        if (!child->leaf) {
            for (int i = 0; i < num; i++) {
                newChild->child[i] = child->child[i + num];
            }
        }

        child->curr = num - 1;
    }

    bool search_value(Btree* N, int key) {
        int i = 0;
        while (i < N->curr && key > N->val[i].first) {
            i++;
        }
        if (i < N->curr && key == N->val[i].first) {
            return true;
        }
        else if (N->leaf) {
            return false;
        }
        else {
            return search_value(N->child[i], key);
        }
    }

    void inOrderTraversal(Btree* N) {
        int i;
        for (i = 0; i < N->curr; i++) {
            if (!N->leaf) {
                inOrderTraversal(N->child[i]);
            }
            cout << " (" << N->val[i].first << ", " << N->val[i].second << ")";
        }
        if (!N->leaf) {
            inOrderTraversal(N->child[i]);
        }
    }

    void serializeNodeToFile(Btree* N, ofstream& name) {
        if (!N) {
            return;
        }
        name << N->leaf << " " << N->curr << " ";
        for (int i = 0; i < N->curr; ++i) {
            name << N->val[i].first << " " << N->val[i].second << " ";
        }
        name << endl;
        if (!N->leaf) {
            for (int i = 0; i <= N->curr; i++) {
                serializeNodeToFile(N->child[i], name);
            }
        }
    }

    Btree* deserializeNodeFromFile(ifstream& name) {
        bool isLeaf;
        int cnt;
        name >> isLeaf >> cnt;

        Btree* tmp = new Btree(num, isLeaf);
        tmp->curr = cnt;
        for (int i = 0; i < cnt; ++i) {
            int key;
            long value;
            name >> key >> value;
            tmp->val[i] = make_pair(key, value);
        }
        if (!isLeaf) {
            for (int i = 0; i <= cnt; i++) {
                tmp->child[i] = deserializeNodeFromFile(name);
            }
        }

        return tmp;
    }

    void delete_value(Btree* N, int key) {
        int i = 0;
        while (i < N->curr && key > N->val[i].first) {
            i++;
        }

        if (i < N->curr && key == N->val[i].first) {
            // Found the pair, now perform deletion
            if (N->leaf) {
                // Shift elements to fill the gap
                for (int j = i; j < N->curr - 1; ++j) {
                    N->val[j] = N->val[j + 1];
                }
                N->curr--;
            }
            else {
                // Replace with predecessor and recursively delete
                pair<int, long> predecessor = get_predecessor(N, i);
                N->val[i] = predecessor;
                delete_value(N->child[i], predecessor.first);
            }
        }
        else {
            // Key not found in this node, go to appropriate child
            if (N->leaf) {
                cout << "Key not found." << endl;
                return;
            }
            bool flag = (i == N->curr); // Indicates whether to go to last child or not
            if (N->child[i]->curr < num) {
                fill(N, i);
            }
            if (flag && i > N->curr) {
                delete_value(N->child[i - 1], key);
            }
            else {
                delete_value(N->child[i], key);
            }
        }
    }

    pair<int, long> get_predecessor(Btree* N, int ind) {
        Btree* current = N->child[ind];
        while (!current->leaf) {
            current = current->child[current->curr];
        }
        return current->val[current->curr - 1];
    }

    void fill(Btree* N, int ind) {
        if (ind != 0 && N->child[ind - 1]->curr >= num) {
            Btree* child = N->child[ind];
            Btree* sibling = N->child[ind - 1];
            for (int i = child->curr - 1; i >= 0; --i) {
                child->val[i + 1] = child->val[i];
            }
            if (!child->leaf) {
                for (int i = child->curr; i >= 0; --i) {
                    child->child[i + 1] = child->child[i];
                }
            }
            child->val[0] = N->val[ind - 1];
            if (!N->leaf) {
                child->child[0] = sibling->child[sibling->curr];
            }
            N->val[ind - 1] = sibling->val[sibling->curr - 1];
            child->curr++;
            sibling->curr--;
        }
        else if (ind != N->curr && N->child[ind + 1]->curr >= num) {
            Btree* child = N->child[ind];
            Btree* sibling = N->child[ind + 1];
            child->val[(child->curr)] = N->val[ind];
            if (!child->leaf) {
                child->child[(child->curr) + 1] = sibling->child[0];
            }
            N->val[ind] = sibling->val[0];
            for (int i = 1; i < sibling->curr; ++i) {
                sibling->val[i - 1] = sibling->val[i];
            }
            if (!sibling->leaf) {
                for (int i = 1; i <= sibling->curr; ++i) {
                    sibling->child[i - 1] = sibling->child[i];
                }
            }
            child->curr++;
            sibling->curr--;
        }
        else {
            if (ind != N->curr) {
                merge(N, ind);
            }
            else {
                merge(N, ind - 1);
            }
        }
    }

    void merge(Btree* N, int ind) {
        Btree* child = N->child[ind];
        Btree* sibling = N->child[ind + 1];
        child->val[num - 1] = N->val[ind];
        for (int i = 0; i < sibling->curr; ++i) {
            child->val[i + num] = sibling->val[i];
        }
        if (!child->leaf) {
            for (int i = 0; i <= sibling->curr; ++i) {
                child->child[i + num] = sibling->child[i];
            }
        }
        for (int i = ind + 1; i < N->curr; ++i) {
            N->val[i - 1] = N->val[i];
        }
        for (int i = ind + 2; i <= N->curr; ++i) {
            N->child[i - 1] = N->child[i];
        }
        child->curr += sibling->curr + 1;
        N->curr--;
        delete (sibling);
        if (N->curr == 0) {
            if (N == root) {
                root = child;
            }
            delete (N);
        }
    }

public:
    B_tree(int _n) {
        root = nullptr;
        num = _n;
    }

    void insert(const pair<int, long>& val) {
        if (root == nullptr) {
            root = new Btree(num, true);
            root->val[0] = val;
            root->curr = 1;
        }
        else {
            if (root->curr == (2 * num - 1)) {
                Btree* newRoot = new Btree(num, false);
                newRoot->child[0] = root;
                split_children(newRoot, 0);
                root = newRoot;
            }
            insert_to_fill(root, val);
        }
    }

    bool search(const int& key) {
        return search_value(root, key);
    }

    void remove(int key) {
        if (!root) {
            cout << "Tree is empty." << endl;
            return;
        }
        delete_value(root, key);
        if (root->curr == 0) {
            Btree* tmp = root;
            if (root->leaf) {
                root = nullptr;
            }
            else {
                root = root->child[0];
            }
            delete tmp;
        }
    }
    int getDegree() {
        return num;
    }
    long Return_position(int key) {
        return retrieveFromFile(root, key);
    }
    void serializeToFile(const string& name) {
        ofstream wrtr(name, ios::binary);
        if (!wrtr.is_open()) {
            cout << "Error: Could not open file for writing." << endl;
            return;
        }
        serializeNodeToFile(root, wrtr);
        wrtr.close();
    }

    void deserializeFromFile(const string& name) {
        ifstream Rdr(name, ios::binary);
        if (!Rdr.is_open()) {
            cout << "Error: Could not open file for reading." << endl;
            return;
        }
        root = deserializeNodeFromFile(Rdr);
        Rdr.close();
    }

    void print() {
        if (root != nullptr) {
            inOrderTraversal(root);
        }
        cout << endl;
    }

    void delete_tree(Btree* N) {
        if (N) {
            if (!N->leaf) {
                for (int i = 0; i <= N->curr; ++i) {
                    delete_tree(N->child[i]);
                }
            }
            delete N;
        }
    }

    ~B_tree() {
        delete_tree(root);
    }
};

template<class K, class T>
struct Node {
    K key;
    T data;
    Node* next;
    Node(const K& k, const T& d) {
        key = k;
        data = d;
        next = nullptr;
    }
};

template<class K, class T>
class my_hashmap {
private:
    Node<K, T>** map;
    int sz, cap;

    int my_hash(int val) {
        return val % cap;
    }

    int my_hash(float val) {
        int intVal = *reinterpret_cast<int*>(&val);
        return my_hash(intVal);
    }

    /*unsigned int my_hash(const std::string& val) {
        unsigned int hashValue = 5381;

        for (const char& c : val) {
            hashValue = ((hashValue << 5) + hashValue) ^ c;
        }

        return hashValue % cap;
    }*/

    size_t my_hash(const string& val) {
        hash< string> hasher;
        return hasher(val) % cap;
    }

    int my_hash(char val) {
        return static_cast<int>(val) % cap;
    }

    int find_index(K val) {
        int ind = my_hash(val), cnt = 1;
        while (map[ind] != nullptr && map[ind]->key != val) {
            ind = _Collide(ind, cnt++);
        }
        return ind;
    }

    /* int _Collide(int i, int j) {
         return (i + j) % cap;
     }*/

    int _Collide(int i, int j) {
        return (i + j * j) % cap;
    }


public:

    my_hashmap() {
        sz = 0;
        cap = 5000;
        map = new Node<K, T>* [cap]();
    }

    int size() {
        return sz;
    }

    void re_Hash() {
        int prev_cap = cap;
        cap = cap * 2;

        Node<K, T>** helper = new Node<K, T>* [cap]();

        for (int i = 0; i < prev_cap; i++) {

            Node<K, T>* curr = map[i
            ];

            while (curr != nullptr) {
                int ind = my_hash(curr->key), cnt = 1;

                while (helper[ind] != nullptr) {
                    ind = _Collide(ind, cnt++);
                }
                helper[ind] = new Node<K, T>(curr->key, curr->data);

                Node<K, T>* temp = curr;
                curr = curr->next;
                delete temp;
            }
        }
        delete[] map;
        map = helper;
    }

    void clear() {
        for (int i = 0; i < cap; i++) {
            Node<K, T>* tmp = map[i];
            while (tmp != nullptr) {
                Node<K, T>* temp = tmp;
                tmp = tmp->next;
                delete temp;
            }
            map[i] = nullptr;
        }
        sz = 0;
    }


    T& operator[](K val) {
        int ind = find_index(val);

        if (map[ind] == nullptr) {
            map[ind] = new Node<K, T>(val, T());
            sz++;

            if (static_cast<double>(sz) / cap > 0.75) {
                re_Hash();
            }
        }
        return map[ind]->data;
    }

    const T& operator[](K val) const {
        int ind = find_index(val);

        if (map[ind] == nullptr) {
            static T _def;
            return _def;
        }
        return map[ind]->data;
    }

    T* find(K key) {
        int ind = find_index(key);

        if (map[ind] == nullptr) {
            return nullptr;
        }
        return &map[ind]->data;
    }

    void insert(K key, T data) {
        (*this)[key] = data;
    }

    bool erase(K Key) {
        int ind = find_index(Key);

        if (map[ind] == nullptr) {
            return false;
        }

        Node<K, T>* temp = map[ind];
        map[ind] = map[ind]->next;
        delete temp;
        sz--;
        return true;
    }

    my_hashmap& operator=(const my_hashmap& temp) {
        if (this == &temp) {
            return *this;
        }
        for (int i = 0; i < cap; i++) {
            Node<K, T>* tmp = map[i];
            while (tmp != nullptr) {
                Node<K, T>* temp = tmp;
                tmp = tmp->next;
                delete temp;
            }
            map[i] = nullptr;
        }
        delete[] map;
        sz = temp.sz;
        cap = temp.cap;

        map = new Node<K, T>* [cap]();

        for (int i = 0; i < cap; i++) {
            Node<K, T>* tmp = temp.map[i];
            Node<K, T>*& curr = map[i];
            Node<K, T>* prev = nullptr;

            while (tmp != nullptr) {
                curr = new Node<K, T>(tmp->key, tmp->data);

                if (prev != nullptr) {
                    prev->next = curr;
                }

                prev = curr;
                tmp = tmp->next;
            }
        }

        return *this;
    }

    class iterator {
    private:
        Node<K, T>** map;
        int cap;
        int curr;
        Node<K, T>* N;

    public:
        iterator(Node<K, T>** m, int c, int b, Node<K, T>* n) : map(m), cap(c), curr(b), N(n) {}

        bool operator!=(const iterator& other) const {
            return curr != other.curr || (N != nullptr && other.N != nullptr && N->key != other.N->key);
        }

        iterator& operator++() {
            if (N != nullptr && N->next != nullptr) {
                N = N->next;
            }
            else {
                do {
                    ++curr;
                } while (curr < cap && map[curr] == nullptr);

                if (curr < cap) {
                    N = map[curr];
                }
                else {
                    N = nullptr;
                }
            }

            return *this;
        }

        Node<K, T>* operator->() const {
            return N;
        }

        Node<K, T>& operator*() const {
            return *N;
        }
    };

    iterator begin() const {
        int st = 0;
        while (st < cap && map[st] == nullptr) {
            ++st;
        }

        if (st < cap) {
            return iterator(map, cap, st, map[st]);
        }
        else {
            return end();
        }
    }

    iterator end() const {
        return iterator(map, cap, cap, nullptr);
    }

    void print() {
        int i = 0;
        while (i < cap) {
            Node<K, T>* tmp = map[i];
            while (tmp != nullptr) {
                cout << "Key = " << tmp->key << " .... Data = " << tmp->data << endl;
                tmp = tmp->next;
            }
            i++;
        }
    }

    typename my_hashmap<K, T>::iterator find_vals(const K& key) {
        int ind = find_index(key);

        if (map[ind] == nullptr) {
            return end();
        }


        Node<K, T>* current = map[ind];
        while (current != nullptr && current->key != key) {
            current = current->next;
        }

        if (current != nullptr) {
            return iterator(map, cap, ind, current);
        }
        else {
            return end();
        }
    }
    ~my_hashmap() {
        int i = 0;
        while (i < cap) {
            Node<K, T>* tmp = map[i];

            while (tmp != nullptr) {
                Node<K, T>* temp = tmp;
                tmp = tmp->next;
                delete temp;
            }
            i++;
        }
        delete[] map;
    }
};
