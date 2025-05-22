#pragma once
#include <functional>
#include <vector>
#include "errors.hpp"

template<typename T>
class NAryTree {
public:
    struct Node {
        T value; 
        std::vector<Node*> children;
        Node(const T& v, std::size_t n) : value(v), children(n, nullptr) {}
        ~Node() { for (Node* c : children) delete c; }
    };

    explicit NAryTree(std::size_t n) : max_children_(n) {
        if (!n) {
            throw MyException(ErrorType::NegativeSize, 2);
        }
    }
    ~NAryTree() { delete root_; }

    
    void insert(const std::vector<std::size_t>& path, const T& v) {
        if (path.empty()) {
            if (root_) {
                throw MyException(ErrorType::InvalidArg, 6);
            }
            root_ = new Node(v, max_children_);
            height_ = std::max<std::size_t>(height_, path.size() + 1);
            return;
        }
        if (!root_) throw MyException(ErrorType::OutOfRange, 8);

        Node* cur = root_;
        for (std::size_t i = 0; i + 1 < path.size(); i++) {
            std::size_t idx = path[i];
            if (idx >= max_children_) {
                throw MyException(ErrorType::OutOfRange, 3);
            }
            if (!cur->children[idx]){
                cur->children[idx] = new Node(T{}, max_children_);
            }
            cur = cur->children[idx];
        }
        std::size_t last = path.back();
        if (last >= max_children_) {
            throw MyException(ErrorType::OutOfRange, 3);
        }
        if (cur->children[last]) {
            throw MyException(ErrorType::InvalidArg, 7);
        }
        cur->children[last] = new Node(v, max_children_);
        height_ = std::max<std::size_t>(height_, path.size() + 1);
    }

    Node* find(const std::vector<std::size_t>& path) const {
        Node* cur = root_;
        for (std::size_t idx : path) {
            if (!cur || idx >= max_children_) {
                return nullptr;
            }
            cur = cur->children[idx];
        }
        return cur;
    }

    void erase(const std::vector<std::size_t>& path) {
        if (!root_) {
            throw MyException(ErrorType::InvalidArg, 5);
        }
        if (path.empty()) {
            delete root_; 
            root_ = nullptr; 
            return;
        }

        Node* cur = root_;
        for (std::size_t i = 0; i + 1 < path.size(); ++i) {
            std::size_t idx = path[i];
            if (!cur || idx >= max_children_) {
                throw MyException(ErrorType::OutOfRange, 8);
            }
            cur = cur->children[idx];
        }
        std::size_t last = path.back();
        if (!cur || last >= max_children_) {
            throw MyException(ErrorType::OutOfRange, 8);
        }
        delete cur->children[last];
        cur->children[last] = nullptr;
        recalcHeight();
    }


    std::size_t firstChild(Node* n) const {
        for (std::size_t i = 0; i < max_children_; ++i) {
            if (n->children[i]) {
                return i;
            }
        }
        return max_children_;
    }

    void smartErase(const std::vector<std::size_t>& path)
    {
        if (!root_) {
            throw MyException(ErrorType::InvalidArg, 5);
        }

        Node* cur = root_;
        Node* parent = nullptr;
        std::size_t idxInParent = 0;

        for (std::size_t step = 0; step < path.size(); ++step) {
            std::size_t idx = path[step];
            if (!cur || idx >= max_children_) {
                throw MyException(ErrorType::OutOfRange, 8);
            }
            parent = cur;
            idxInParent = idx;
            cur = cur->children[idx];
        }
        if (!cur) {
            throw MyException(ErrorType::InvalidArg, 5);
        }
        while (true) {
            std::size_t k = firstChild(cur);
            if (k == max_children_) break;
            cur->value = cur->children[k]->value;

            parent = cur;
            idxInParent = k;
            cur = cur->children[k];
        }

        if (parent) parent->children[idxInParent] = nullptr;
        else root_ = nullptr;
        delete cur;

        recalcHeight();
    }
    
    template<typename F>
    NAryTree map(F f) const {
        NAryTree r(max_children_);
        if (!root_) {
            return r;
        }
        r.root_ = new Node(f(root_->value), max_children_);
        std::function<void(Node*,Node*)> dfs = [&](Node* s, Node* d){
            for (std::size_t i=0;i<max_children_;++i)
                if (s->children[i]) {
                    d->children[i] = new Node(f(s->children[i]->value), max_children_);
                    dfs(s->children[i], d->children[i]);
                }
        };
        dfs(root_, r.root_); return r;
    }

    template<typename F, typename Acc>
    Acc reduce(F f, Acc init) const {
        preorder(root_, [&](Node* n){ init=f(init,n->value);} );
        return init;
    }

    bool equalsSubtree(const Node* a, const Node* b) const {
        if (!b) return true;
        if (!a) return false;

        if (a->value!=b->value) {
            return false;
        }
        for (std::size_t i=0;i<max_children_;++i)
            if(!equalsSubtree(a->children[i], b->children[i]) && b->children[i]) {
                return false;
            }
        return true;
    }

    bool containsSubtree(const Node* p) const {
        bool ok=false;
        preorder(root_,[&](Node* n){ if(!ok && equalsSubtree(n,p)) { ok=true; } });
        return ok;
    }

    Node* root() const { return root_; }
    std::size_t degree() const { return max_children_; }
    std::size_t height() const { return height_; }

private:
    Node* root_ = nullptr;
    std::size_t max_children_;
    std::size_t height_ = 0;

    static void preorder(Node* n, const std::function<void(Node*)>& f) {
        if (!n) return;
        f(n);
        for (Node* c : n->children) {
            preorder(c, f);
        }
    }

    std::size_t depth(Node* n) const {
        if (!n) return 0;
        std::size_t h = 1;
        for (Node* c : n->children)
            h = std::max(h, 1 + depth(c));
        return h;
    }

    void recalcHeight() {
        height_ = depth(root_);
    }
};