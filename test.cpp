#include <iostream>
#include <memory>
#include <stdexcept>
#include <stack>

template<class K, class V>
class SearchingTree {
private:
    struct Node {
        K key;
        V value;
        std::unique_ptr<Node> left, right;

        Node(const K& k, const V& v) : key(k), value(v), left(nullptr), right(nullptr) {}
    };

    std::unique_ptr<Node> root;

    Node* insert(Node* node, const K& key, const V& value) {
        if (!node) {
            return new Node(key, value);
        }
        if (key < node->key) {
            node->left.reset(insert(node->left.release(), key, value));
        } else if (key > node->key) {
            node->right.reset(insert(node->right.release(), key, value));
        }

        return node;
    }

    Node* findMin(Node* node) const {
        while (node && node->left) {
            node = node->left.get();
        }
        return node;
    }

    Node* findMax(Node* node) const {
        while (node && node->right) {
            node = node->right.get();
        }
        return node;
    }

    Node* erase(Node* node, const K& key) {
        if (!node) return nullptr;

        if (key < node->key) {
            node->left.reset(erase(node->left.release(), key));
        } else if (key > node->key) {
            node->right.reset(erase(node->right.release(), key));
        } else {
            if (!node->left) {
                return node->right.release();
            } else if (!node->right) {
                return node->left.release();
            }

            Node* minNode = findMin(node->right.get());
            node->key = minNode->key;
            node->value = minNode->value;
            node->right.reset(erase(node->right.release(), minNode->key));
        }
        return node;
    }

    Node* find(Node* node, const K& key) const {
        if (!node || node->key == key) {
            return node;
        }
        if (key < node->key) {
            return find(node->left.get(), key);
        } else {
            return find(node->right.get(), key);
        }
    }

public:
    SearchingTree() : root(nullptr) {}

    void insert(const K& key, const V& value) {
        root.reset(insert(root.release(), key, value));
    }

    void erase(const K& key) {
        root.reset(erase(root.release(), key));
    }

    const V* find(const K& key) const {
        Node* node = find(root.get(), key);
        if (node) {
            return &(node->value);
        }
        return nullptr;
    }

    const K& findMinKey() const {
        if (!root) throw std::runtime_error("The tree is empty");
        Node* node = findMin(root.get());
        return node->key;
    }

    const K& findMaxKey() const {
        if (!root) throw std::runtime_error("The tree is empty");
        Node* node = findMax(root.get());
        return node->key;
    }

    class Iterator {
    private:
        std::stack<Node*> stack;
        K lower_bound;
        K upper_bound;

        void pushLeft(Node* node) {
            while (node) {
                if (node->key >= lower_bound) {
                    stack.push(node);
                }
                node = node->left.get();
            }
        }

    public:
        Iterator(Node* root, const K& a, const K& b) : lower_bound(a), upper_bound(b) {
            pushLeft(root);
        }

        bool hasNext() const {
            return !stack.empty();
        }

        std::pair<const K&, const V&> next() {
            Node* node = stack.top();
            stack.pop();
            if (node->key < upper_bound) {
                pushLeft(node->right.get());
            }
            return {node->key, node->value};
        }
    };

    Iterator range(const K& a, const K& b) const {
        return Iterator(root.get(), a, b);
    }
};

int main() {
    SearchingTree<int, std::string> tree;
    tree.insert(10, "ten");
    tree.insert(20, "twenty");
    tree.insert(5, "five");
    tree.insert(15, "fifteen");

    std::cout << "Min key: " << tree.findMinKey() << std::endl;
    std::cout << "Max key: " << tree.findMaxKey() << std::endl;

    SearchingTree<int, std::string>::Iterator it = tree.range(5, 20);
    while (it.hasNext()) {
        auto[key, value] = it.next();
        std::cout << "Key in range: " << key << ", Value: " << value << std::endl;
    }

    return 0;
}
