#ifndef MYSET_H
#define MYSET_H

#include <iterator>

template <typename T>
class Set {
public:
    struct node {
       // T value;
        node *l, *r, *par;
        node() : l(nullptr), r(nullptr), par(nullptr) {}
        node(node* l, node* r, node* par) : l(l), r(r), par(par) {}
    };

    struct extended_node : public node {
        T value;
        extended_node() = delete;
        extended_node(T const& v, node* pv = nullptr, node* lv = nullptr, node* rv = nullptr) :
            value(v), node(lv, rv, pv) {}
    };

    Set() noexcept : root(nullptr) {}
    ~Set() noexcept {
        dealloc(root);
    }
    Set(Set const& o) {
        root = new node();
        alloc(root, o.root);
    }
    Set& operator = (Set const& o) {
        Set tmp(o);
        std::swap(*this, tmp);
        return *this;
    }

    template <typename U>

    //ITERATOR IMPL

  //  class Iterator : std::iterator<std::bidirectional_iterator_tag, U, std::ptrdiff_t, U*, U&> {
    class Iterator {

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = U;
        using pointer = U*;
        using reference = U&;
        using iterator_category = std::bidirectional_iterator_tag;
        friend class Set;

        template <typename V>
        Iterator(const Iterator<V>& o, typename std::enable_if<std::is_same<U, const V>::value>::type* = nullptr) : ptr(o.ptr) {}

        Iterator& operator++() {
            if (!ptr) return *this;
            if (ptr->r) {
                ptr = ptr->r;
                while (ptr->l) ptr = ptr->l;
                return *this;
            }
            if (ptr->par && ptr->par->l == ptr) {
                ptr = ptr->par;
                return *this;
            }
            do {
               if (ptr->par) ptr = ptr->par;
            } while (ptr->par && ptr->par->r == ptr);
            if (ptr->par) ptr = ptr->par;
            return *this;
        }
        const Iterator operator++(int) {
            Iterator res(ptr);
            ++(*this);
            return res;
        }
        Iterator& operator--() {
            if (!ptr) return *this;
            if (ptr->l) {
                ptr = ptr->l;
                while (ptr->r) ptr = ptr->r;
                return *this;
            }
            if (ptr->par && ptr->par->r == ptr) {
                ptr = ptr->par;
                return *this;
            }
            do {
               if (ptr->par) ptr = ptr->par;
            } while (ptr->par && ptr->par->l == ptr);
            if (ptr->par) ptr = ptr->par;
            return *this;
        }
        const Iterator operator--(int) {
            Iterator res(ptr);
            --(*this);
            return res;
        }

        U& operator *() const {

           return static_cast<extended_node*>(ptr)->value;
        }

        friend bool operator ==(Iterator const& a, Iterator const& b) {
            return a.ptr == b.ptr;
        }

        friend bool operator !=(Iterator const& a, Iterator const& b) {
            return a.ptr != b.ptr;
        }

    private:
        explicit Iterator(Set<T>::node* p) : ptr(p) {}
        node* ptr;
    };

    using iterator = Iterator<T>;
    using const_iterator = Iterator <const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() {
        if (!root) return iterator(root);
        node* v = root;
        while (v->l) v = v->l;
        return iterator(v);
    }
    const_iterator begin() const {
        if (!root) return iterator(root);
        node* v = root;
        while (v->l) v = v->l;
        return const_iterator(v);
    }
    iterator end() {
        return iterator(root);
    }
    const_iterator end() const{
        return const_iterator(root);
    }
    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }
    reverse_iterator rend() {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }
    //ENDS HERE

    std::pair <iterator, bool> insert(T const& x, node* v = nullptr, bool init = false) {
        if (!root) {
            root = new node();
            root->l = new extended_node(x, root);
            return std::make_pair(iterator(root->l), true);
        }
        if (!init) v = root->l;
        const T val = static_cast<extended_node*>(v)->value;
        if (!v->l && x < val) {
            v->l = new extended_node(x, v);
            return std::make_pair(iterator(v->l), true);
        }
        if (!v->r && val < x) {
            v->r = new extended_node(x, v);
            return std::make_pair(iterator(v->r), true);
        }
        if (x < val) return insert(x, v->l, true);
        if (val < x) return insert(x, v->r, true);
        return std::make_pair(iterator(v), false);
    }

    const_iterator find(T const& x, node* v = nullptr, bool init = false) const noexcept {
        if (!root) return end();
        if (!init) v = root->l;
        if (!v) return end();
        T const& val = static_cast<extended_node*>(v)->value;
        if (x < val) return find(x, v->l, true);
        if (val < x) return find(x, v->r, true);
        return const_iterator(v);
    }

    const_iterator upper_bound(T const& x) const noexcept {
        if (!root) return end();
       node* v = root->l;
       node* last = nullptr;
       while(v) {
           if (x < static_cast<extended_node*>(v)->value) {
                last = v;
                v = v->l;
           } else v = v->r;
       }
       return (last && x < static_cast<extended_node*>(last)->value) ? const_iterator(last) : end();
    }
    const_iterator lower_bound(T const& x) const noexcept {
        if (find(x) != end()) return find(x); else return upper_bound(x);
    }

    const_iterator erase(const_iterator place) noexcept {
        if (place == end()) return end();
        node* v = place.ptr;
        T x = static_cast<extended_node*>(v)->value;
        if (!v->l && !v->r) {
           if (v->par->l == v) v->par->l = nullptr; else v->par->r = nullptr;
        } else if (!v->l || !v->r) {
            if (!v->l) {
                if (v->par->l == v) {
                    v->par->l = v->r;
                } else {
                    v->par->r = v->r;
                }
                v->r->par = v->par;
            } else {
                if (v->par->l == v) {
                    v->par->l = v->l;
                } else {
                    v->par->r = v->l;
                }
                v->l->par = v->par;
            }
        } else {
            node* next = upper_bound(static_cast<extended_node*>(v)->value).ptr;
            if (next->par->l == next) {
                next->par->l = next->r;
                if (next->r) next->r->par = next->par;
            } else {
                next->par->r = next->r;
                if (next->r) next->r->par = next->par;
            }
            if (v->l) {
                next->l = v->l;
                v->l->par = next;
            }
            if (v->r) {
               next->r = v->r;
               v->r->par = next;
            }
            next->par = v->par;
            if (v->par->l == v) v->par->l = next; else v->par->r = next;
        }
        delete v;
        return upper_bound(x);
    }

    bool empty() const noexcept {
       return (root == nullptr);
    }
    void clear() noexcept {
        dealloc(root);
        root = nullptr;
    }
    friend void swap(Set<T> & a, Set<T> & b) noexcept {
        std::swap(a.root, b.root);
    }
private:
    node* root;
    void alloc (node *&cur, const node *o) {
        if (!o) return;
        if (o->l) cur->l = new extended_node(static_cast<extended_node*>(o->l)->value, cur);
        if (o->r) cur->r = new extended_node(static_cast<extended_node*>(o->r)->value, cur);
    }
    void dealloc(node *&cur) {
        if (!cur) return;
        if (cur->r) dealloc(cur->r);
        if (cur->l) dealloc(cur->l);
        delete cur;
    }
};


#endif
