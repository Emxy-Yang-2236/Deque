#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include <cmath>
#include <iostream>
#include "exceptions.hpp"

#include <cstddef>

namespace sjtu {
    template<class T>
    class double_list {

    public:
        struct node {
            T data;
            node *prev, *next;

            node(const T &x, node *p = nullptr, node *n = nullptr)
                : data(x), prev(p), next(n) {
            }

            node() : prev(nullptr), next(nullptr) {
            }
        };

        node *head, *tail;
        int currentLength = 0;

    public:
        // --------------------------
        /**
         * the follows are constructors and destructors
         * you can also add some if needed.
        */

        static node *make_sentinel() {
            node *p = static_cast<node *>(::operator new(sizeof(node)));
            p->prev = nullptr;
            p->next = nullptr;
            return p;
        }

        static void destroy_sentinel(node *p) {
            ::operator delete(p);
        }

        double_list() {
            head = make_sentinel();
            tail = make_sentinel();
            head->next = tail;
            tail->prev = head;
            currentLength = 0;
        }

        double_list(const double_list<T> &other) {
            head = make_sentinel();
            tail = make_sentinel();
            head->next = tail;
            tail->prev = head;
            currentLength = 0;

            node *p = other.head->next;
            // while (p != other.tail) {
            //     insert_tail(p->data);
            //     p = p->next;
            // }
            for (auto it = other.begin(); it != other.end(); ++it) {
                this->insert_tail(*it); // 这里会调用 CircularArr 的拷贝构造
            }
        }

        double_list &operator=(const double_list<T> &other) {
            if (this == &other) return *this;
            clear();

            node *p = other.head->next;
            while (p != other.tail) {
                insert_tail(p->data);
                p = p->next;
            }
            return *this;
        }

        ~double_list() {
            if (currentLength != 0) {
                node *p = head->next;
                while (p != tail) {
                    node *tmp = p;
                    p = p->next;
                    delete tmp;
                }
            }
            destroy_sentinel(head);
            destroy_sentinel(tail);
        }

        int size() { return currentLength; }

        void clear() {
            if (currentLength != 0) {
                node *p = head->next;
                while (p != tail) {
                    node *tmp = p;
                    p = p->next;
                    delete tmp;
                }
                head->next = tail;
                tail->prev = head;
                currentLength = 0;
            }
        }

        class iterator {
        public:
            node *ptr;
            const double_list *check;
            friend class double_list;
            // --------------------------

            iterator(node *p = nullptr, const double_list *c = nullptr) : ptr(p), check(c) {
            }

            iterator(const iterator &t) {
                ptr = t.ptr;
                check = t.check;
            }

            //iterator(): ptr(nullptr) , check(nullptr){}
            ~iterator() = default;


            iterator &operator++() {
                if (this->ptr == check->tail) {
                    throw sjtu::index_out_of_bound();
                }
                ptr = ptr->next;
                return *this;
            }

            iterator operator++(int) {
                if (this->ptr == check->tail) {
                    throw sjtu::index_out_of_bound();
                }
                iterator tmp = *this;
                ptr = ptr->next;
                return tmp;
            }

            iterator &operator--() {
                if (this->ptr == check->head || this->ptr == check->head->next) {
                    throw sjtu::index_out_of_bound();
                }
                ptr = ptr->prev;
                return *this;
            }

            iterator operator--(int) {
                if (this->ptr == check->head || this->ptr == check->head->next) {
                    throw sjtu::index_out_of_bound();
                }
                iterator tmp = *this;
                ptr = ptr->prev;
                return tmp;
            }

            /**
             * if the iter didn't point to a value
             * throw " invalid"
            */
            T &operator*() const {
                if (!ptr || ptr == check->tail || ptr == check->head) {
                    throw sjtu::invalid_iterator();
                }
                return ptr->data;
            }

            /**
             * other operation
            */
            T *operator->() const {
                if (!ptr) {
                    throw sjtu::invalid_iterator();
                }
                return &ptr->data;
            }

            bool operator==(const iterator &rhs) const {
                return this->ptr == rhs.ptr;
            }

            bool operator!=(const iterator &rhs) const {
                return this->ptr != rhs.ptr;
            }
        };

        /**
         * return an iterator to the beginning
         */
        iterator begin() const {
            return iterator(head->next, this);
        }

        /**
         * return an iterator to the ending
         * in fact, it returns the iterator point to nothing,
         * just after the last element.
         */
        iterator end() const {
            return iterator(tail, this);
        }

        iterator back() const {
            return iterator(tail->prev, this);
        }


        iterator erase(iterator pos) {
            node *p = pos.ptr;
            iterator ret(p->next, this);

            p->prev->next = p->next;
            p->next->prev = p->prev;

            delete p;
            --currentLength;
            return ret;
        }

        void move_to_head(node *p) {
            p->prev->next = p->next;
            p->next->prev = p->prev;
            p->next = head->next;
            p->prev = head;
            head->next->prev = p;
            head->next = p;
        }

        void move_to_tail(node *p) {
            p->prev->next = p->next;
            p->next->prev = p->prev;

            p->prev = tail->prev;
            p->next = tail;
            tail->prev->next = p;
            tail->prev = p;
        }


        void insert_head(const T &val) {
            node *beg = head->next;
            node *new_node = new node(val, head, beg);

            head->next = new_node;
            beg->prev = new_node;

            ++currentLength;
        }

        void insert_tail(const T &val) {
            node *p = tail->prev;
            node *new_node = new node(val, p, tail);

            tail->prev = new_node;
            p->next = new_node;

            ++currentLength;
        }

        typename double_list<T>::iterator insert_after(const iterator &pos, const T &val) {
            auto p = pos.ptr;
            if (pos == end()) {
                throw sjtu::index_out_of_bound();
            }
            auto new_next = p->next;
            auto new_node = new node(val, p, new_next);

            new_next->prev = new_node;
            p->next = new_node;

            ++currentLength;
            return iterator(new_node,this);
        }

        void delete_head() {
            node *p = head->next;

            head->next = p->next;
            p->next->prev = head;

            delete p;
            --currentLength;
        }

        void delete_tail() {
            node *p = tail->prev;

            tail->prev = p->prev;
            p->prev->next = tail;

            delete p;
            --currentLength;
        }

        bool empty() const {
            return currentLength == 0;
        }
    };

    template<class T>
    class deque {
        struct CircularArr {
            T *buffer;
            int front_idx;
            int tail_idx;
            int count = 0;
            int capacity;

            CircularArr(int init_size = 8) : capacity(init_size), front_idx(0), tail_idx(0), count(0) {
                if (capacity <= 0) capacity = 8;
                buffer = static_cast<T *>(::operator new(sizeof(T) * capacity));
            }

            ~CircularArr() {
                for (int i = 0; i < count; ++i) {
                    int idx = (front_idx + i) % capacity;
                    buffer[idx].~T();
                }
                ::operator delete(buffer);
            }

            bool empty() const {
                return count == 0;
            }

            int size() const {
                return count;
            }



            void resize(int new_capacity) {
                T *new_buffer = static_cast<T *>(::operator new(sizeof(T) * new_capacity));

                for (int i = 0; i < count; ++i) {
                    int old_idx = (front_idx + i) % capacity;
                    new(new_buffer + i) T(std::move(buffer[old_idx]));
                    buffer[old_idx].~T();
                }

                ::operator delete(buffer);
                buffer = new_buffer;
                front_idx = 0;
                tail_idx = count;
                capacity = new_capacity;
            }

            void push_back(const T &x) {
                if (count == capacity) resize(capacity * 2);
                new(buffer + tail_idx) T(x);
                tail_idx = (tail_idx + 1) % capacity;
                ++count;
            }

            void push_front(const T &x) {
                if (count == capacity) resize(capacity * 2);
                front_idx = (front_idx - 1 + capacity) % capacity;
                new(buffer + front_idx) T(x);
                ++count;
            }

            void pop_front() {
                buffer[front_idx].~T();
                front_idx = (front_idx + 1) % capacity;
                --count;
                if (count > 0 && count <= capacity / 4 && capacity / 2 >= 8) {
                    resize(capacity / 2);
                }
            }

            void pop_back() {
                tail_idx = (tail_idx - 1 + capacity) % capacity;
                buffer[tail_idx].~T();
                --count;
                if (count > 0 && count <= capacity / 4 && capacity / 2 >= 8) {
                    resize(capacity / 2);
                }
            }

            T &operator[](int idx) {
                if (idx < 0 || idx >= count) {
                    //TODO error throw in test one
                    throw sjtu::index_out_of_bound();
                }
                return buffer[(front_idx + idx) % capacity];
            }

            const T &operator[](int idx) const {
                if (idx < 0 || idx >= count) {
                    throw sjtu::index_out_of_bound();
                }
                return buffer[(front_idx + idx) % capacity];
            }

            void insert(int idx, const T &x) {
                if (idx < 0 || idx > count) {
                    throw sjtu::index_out_of_bound();
                }
                if (idx == 0) {
                    push_front(x);
                    return;
                }
                if (idx == count) {
                    push_back(x);
                    return;
                }
                if (count == capacity) resize(capacity * 2);

                //中间插入时，就近找头/尾进行移位，复杂度为最差（不引发扩容的情况下） O（d/2） d ~ sqrt(n)
                if (idx < count / 2) {
                    int old_front = front_idx;
                    front_idx = (front_idx - 1 + capacity) % capacity;
                    new(buffer + front_idx) T(std::move(buffer[old_front]));
                    for (int i = 1; i < idx; ++i) {
                        int dst = (front_idx + i) % capacity;
                        int src = (old_front + i) % capacity;
                        buffer[dst] = std::move(buffer[src]);
                    }
                } else {
                    int old_tail = tail_idx;
                    tail_idx = (tail_idx + 1) % capacity;
                    int new_last = (old_tail) % capacity;
                    int prev_last = (old_tail - 1 + capacity) % capacity;
                    new(buffer + new_last) T(std::move(buffer[prev_last]));
                    for (int i = count - 2; i >= idx; --i) {
                        int dst = (front_idx + i + 1) % capacity;
                        int src = (front_idx + i) % capacity;
                        buffer[dst] = std::move(buffer[src]);
                    }
                }
                int real_idx = (front_idx + idx) % capacity;
                buffer[real_idx] = x;
                ++count;
            }

            void erase(int idx) {
                if (idx < 0 || idx >= count) {
                    throw sjtu::index_out_of_bound();
                }
                if (idx == 0) {
                    pop_front();
                    return;
                }
                if (idx == count - 1) {
                    pop_back();
                    return;
                }

                if (idx < count / 2) {
                    for (int i = idx; i > 0; --i) {
                        int dst = (front_idx + i) % capacity;
                        int src = (front_idx + i - 1) % capacity;
                        buffer[dst] = std::move(buffer[src]);
                    }
                    buffer[front_idx].~T();
                    front_idx = (front_idx + 1) % capacity;
                } else {
                    for (int i = idx; i < count - 1; ++i) {
                        int dst = (front_idx + i) % capacity;
                        int src = (front_idx + i + 1) % capacity;
                        buffer[dst] = std::move(buffer[src]);
                    }
                    tail_idx = (tail_idx - 1 + capacity) % capacity;
                    buffer[tail_idx].~T();
                }
                --count;
                if (count > 0 && count <= capacity / 4 && capacity / 2 >= 8) {
                    resize(capacity / 2);
                }
            }

            CircularArr(const CircularArr &other)
                : front_idx(0), tail_idx(other.count),
                  count(other.count), capacity(other.capacity) {
                buffer = static_cast<T *>(::operator new(sizeof(T) * capacity));
                for (int i = 0; i < count; ++i) {
                    int src = (other.front_idx + i) % other.capacity;
                    new(buffer + i) T(other.buffer[src]); // placement new
                }
            }

            CircularArr &operator=(const CircularArr &other) {
                if (this == &other) return *this;
                for (int i = 0; i < count; ++i) {
                    buffer[(front_idx + i) % capacity].~T();
                }
                ::operator delete(buffer);
                capacity = other.capacity;
                count = other.count;
                front_idx = 0;
                tail_idx = count;
                buffer = static_cast<T *>(::operator new(sizeof(T) * capacity));
                for (int i = 0; i < count; ++i) {
                    int src = (other.front_idx + i) % other.capacity;
                    new(buffer + i) T(other.buffer[src]);
                }
                return *this;
            }

            CircularArr(CircularArr &&other) noexcept
                : buffer(other.buffer), front_idx(other.front_idx),
                  tail_idx(other.tail_idx), count(other.count),
                  capacity(other.capacity) {
                other.buffer = nullptr;
                other.count = 0;
                other.capacity = 0;
                other.front_idx = 0;
                other.tail_idx = 0;
            }

            CircularArr &operator=(CircularArr &&other) noexcept {
                if (this == &other) return *this;
                for (int i = 0; i < count; ++i) {
                    buffer[(front_idx + i) % capacity].~T();
                }
                ::operator delete(buffer);
                buffer = other.buffer;
                front_idx = other.front_idx;
                tail_idx = other.tail_idx;
                count = other.count;
                capacity = other.capacity;
                other.buffer = nullptr;
                other.count = 0;
                other.capacity = 0;
                return *this;
            }
        };

        friend class double_list<CircularArr>::iterator;
        double_list<CircularArr> list;
        int total_size = 0;
        int version = 0;

        int get_ideal_cap() {
            int cap = std::sqrt(total_size) + 1;
            return cap < 8 ? 8 : cap;
        }

        int base_line = 8;

    public:
        class const_iterator;

        class iterator {
            typename double_list<CircularArr>::iterator db_lst_it_;
            int current_idx_;
            int current_version_ = 0; //辅助失效检查，若内外版本号不同则说明迭代器已经失效
            const deque *tar_lst_;
            friend class deque;

            iterator(typename double_list<CircularArr>::iterator it, int idx, int version,
                     const deque *tar_lst) : db_lst_it_(it), current_idx_(idx), current_version_(version),
                                             tar_lst_(tar_lst) {
            }

        public:
            iterator() : current_idx_(0), tar_lst_(nullptr) {
            };

            //失效检查
            void check_valid() const{
                if (tar_lst_ == nullptr || current_version_ != tar_lst_->version) {
                    throw sjtu::invalid_iterator();
                }
            }

            [[nodiscard]] int global_index() const {
                int result = 0;
                auto it = tar_lst_->list.begin();

                while (it != db_lst_it_) {
                    result += (*it).size();
                    ++it;
                }

                result += current_idx_;
                return result;
            }

            iterator operator+(const int &n) const {
                check_valid();
                if (n < 0) return operator-(-n);
                if (n == 0) return *this;

                int idx = current_idx_ + n;
                auto cur_it = db_lst_it_;

                // 只有当 cur_it 不是末尾，且 idx 超过了当前块的索引范围时才跳转
                while (cur_it != tar_lst_->list.end() && idx >= cur_it->size()) {
                    idx -= cur_it->size();
                    ++cur_it;
                    // 如果跳到了哨兵，必须保证 idx 刚好为 0（即指向 end()）
                    if (cur_it == tar_lst_->list.end()) {
                        if (idx == 0) break;
                        throw sjtu::index_out_of_bound();
                    }
                }
                return iterator(cur_it, idx, current_version_, tar_lst_);
            }

            iterator operator-(const int &n) const {
                check_valid();
                if (n < 0) {
                    return operator+(-n);
                }

                int idx = current_idx_ - n;
                auto cur_it = db_lst_it_;
                bool warning_flag = false;
                if (cur_it == tar_lst_->list.begin()) {
                    warning_flag = true;
                }
                while (idx < 0) {
                    if (warning_flag && idx < 0) {
                        throw sjtu::index_out_of_bound();
                    }
                    --cur_it;
                    idx += cur_it->size();
                    if (cur_it == tar_lst_->list.begin()) {
                        warning_flag = true;
                    }
                }
                return iterator(cur_it, idx,current_version_, tar_lst_);
            }

            int operator-(const iterator &rhs) const {
                check_valid();
                if (this->tar_lst_ != rhs.tar_lst_) {
                    throw sjtu::invalid_iterator();
                }
                return this->global_index() - rhs.global_index();
            }

            iterator &operator+=(const int &n) {
                check_valid();
                *this = *this + n;
                return *this;
            }


            iterator &operator-=(const int &n) {
                check_valid();
                *this = *this - n;
                return *this;
            }

            iterator operator++(int) {
                check_valid();
                iterator tmp = *this;
                *this = *this + 1;
                return tmp;
            }

            iterator &operator++() {
                check_valid();
                *this = *this + 1;
                return *this;
            }

            iterator operator--(int) {
                check_valid();
                iterator tmp = *this;
                *this = *this - 1;
                return tmp;
            }

            iterator &operator--() {
                check_valid();
                *this = *this - 1;
                return *this;
            }

            T &operator*() const {
                check_valid();
                return (*db_lst_it_)[current_idx_];
            }

            T *operator->() const noexcept {
                check_valid();
                return &((*db_lst_it_)[current_idx_]);
            }

            bool operator==(const iterator &rhs) const {
                check_valid();
                rhs.check_valid();
                return db_lst_it_ == rhs.db_lst_it_ && current_idx_ == rhs.current_idx_;
            }

            bool operator==(const const_iterator &rhs) const {
                check_valid();
                rhs.check_valid();
                return db_lst_it_ == rhs.db_lst_it_ && current_idx_ == rhs.current_idx_;
            }

            bool operator!=(const iterator &rhs) const {
                check_valid();
                rhs.check_valid();
                return db_lst_it_ != rhs.db_lst_it_ || current_idx_ != rhs.current_idx_;
            }

            bool operator!=(const const_iterator &rhs) const {
                check_valid();
                rhs.check_valid();
                return db_lst_it_ != rhs.db_lst_it_ || current_idx_ != rhs.current_idx_;
            }
        };

        class const_iterator {
        private:
            typename double_list<CircularArr>::iterator db_lst_it_;
            int current_idx_;
            int current_version_ = 0;
            const deque *tar_lst_;

            friend class deque;
            friend class iterator;

            const_iterator(typename double_list<CircularArr>::iterator it, int idx, int version,
                           const deque *tar_lst) : db_lst_it_(it), current_idx_(idx), current_version_(version),
                                                   tar_lst_(tar_lst) {
            }

        public:
            const_iterator() : current_idx_(0), tar_lst_(nullptr) {
            }

            // 允许 iterator 隐式转换为 const_iterator
            const_iterator(const iterator &other) : db_lst_it_(other.db_lst_it_), current_idx_(other.current_idx_),
                                                    current_version_(other.current_version_), tar_lst_(other.tar_lst_) {
            }

            void check_valid() const {
                if (tar_lst_ == nullptr || current_version_ != tar_lst_->version) {
                    throw sjtu::invalid_iterator();
                }
            }

            [[nodiscard]] int global_index() const {
                int result = 0;
                auto it = tar_lst_->list.begin();
                while (it != db_lst_it_) {
                    result += (*it).size();
                    ++it;
                }
                result += current_idx_;
                return result;
            }

            // 算术运算符 完全同 iterator

            const_iterator operator+(const int &n) const {
                check_valid();
                if (n < 0) return operator-(-n);
                if (n == 0) return *this;

                int idx = current_idx_ + n;
                auto cur_it = db_lst_it_;

                // 只有当 cur_it 不是末尾，且 idx 超过了当前块的索引范围时才跳转
                while (cur_it != tar_lst_->list.end() && idx >= cur_it->size()) {
                    idx -= cur_it->size();
                    ++cur_it;
                    // 如果跳到了哨兵，必须保证 idx 刚好为 0（即指向 end()）
                    if (cur_it == tar_lst_->list.end()) {
                        if (idx == 0) break;
                        throw sjtu::index_out_of_bound();
                    }
                }
                return const_iterator(cur_it, idx, current_version_, tar_lst_);
            }

            const_iterator operator-(const int &n) const {
                check_valid();
                if (n < 0) return operator+(-n);

                int idx = current_idx_ - n;
                auto cur_it = db_lst_it_;
                while (idx < 0) {
                    if (cur_it == tar_lst_->list.begin()) throw sjtu::index_out_of_bound();
                    --cur_it;
                    idx += cur_it->size();
                }
                return const_iterator(cur_it, idx, current_version_, tar_lst_);
            }

            int operator-(const const_iterator &rhs) const {
                check_valid();
                rhs.check_valid();
                if (this->tar_lst_ != rhs.tar_lst_) throw sjtu::invalid_iterator();
                return this->global_index() - rhs.global_index();
            }

            const_iterator &operator+=(const int &n) {
                *this = *this + n;
                return *this;
            }

            const_iterator &operator-=(const int &n) {
                *this = *this - n;
                return *this;
            }

            const_iterator operator++(int) {
                const_iterator tmp = *this;
                *this = *this + 1;
                return tmp;
            }

            const_iterator &operator++() {
                *this = *this + 1;
                return *this;
            }

            const_iterator operator--(int) {
                const_iterator tmp = *this;
                *this = *this - 1;
                return tmp;
            }

            const_iterator &operator--() {
                *this = *this - 1;
                return *this;
            }


            const T &operator*() const {
                check_valid();
                if (db_lst_it_ == tar_lst_->list.end()) throw sjtu::invalid_iterator();
                return (*db_lst_it_)[current_idx_];
            }

            const T *operator->() const noexcept {
                check_valid();
                return &((*db_lst_it_)[current_idx_]);
            }


            bool operator==(const const_iterator &rhs) const {
                check_valid();
                rhs.check_valid();
                return db_lst_it_ == rhs.db_lst_it_ && current_idx_ == rhs.current_idx_;
            }

            bool operator!=(const const_iterator &rhs) const {
                return !(*this == rhs);
            }
        };

        //将下标转化迭代器

        iterator idx_to_it(const size_t &pos) const {
            if (pos > total_size) throw sjtu::index_out_of_bound(); // 报错 11
            if (pos == total_size) return end(); // 快捷返回 end()

            auto cur_lst_it = list.begin();
            size_t current_pos = pos;

            while (cur_lst_it != list.end()) {
                if (current_pos < (size_t)cur_lst_it->size()) {
                    return iterator(cur_lst_it, (int)current_pos, this->version, this);
                }
                current_pos -= cur_lst_it->size();
                ++cur_lst_it;
            }
            return end();
        }

        deque(int init_size = 8) {
            list.insert_tail(CircularArr(init_size));
        }

        deque(const deque &other) : list(other.list), total_size(other.total_size), base_line(other.base_line), version(other.version) {}

        ~deque() {
            //list.clear();
            total_size = 0;
        }

        deque &operator=(const deque &other) {
            if (this == &other) return *this;

            deque temp(other);

            std::swap(list, temp.list);
            std::swap(total_size, temp.total_size);
            std::swap(base_line, temp.base_line);

            version++;

            return *this;
        }

        T &at(const size_t &pos) {
            return *idx_to_it(pos);
        }

        const T &at(const size_t &pos) const {
            return *idx_to_it(pos);
        }

        T &operator[](const size_t &pos) {
            return *idx_to_it(pos);
        }

        const T &operator[](const size_t &pos) const {
            return *idx_to_it(pos);
        }

        const T &front() const {
            if (total_size == 0) throw sjtu::container_is_empty();
            auto it = list.begin();
            while (it != list.end()) {
                if (!it->empty()) {
                    return (*it)[0];
                }
                ++it;
            }
        }

        const T &back() const {
            auto it = list.back();
            do {
                if (!it->empty()) {
                    return (*it)[it->size() - 1];
                }
                --it;
            } while (it != list.begin());
            throw sjtu::container_is_empty();
        }


        iterator begin() {
            return iterator(list.begin(), 0,this->version, this);
        }

        const_iterator cbegin() const {
            return const_iterator(list.begin(), 0,this->version, this);
        }


        iterator end() {
            if (total_size == 0) return iterator(list.begin(),0,this->version,this);
            return iterator(list.end(), 0,this->version, this);
        }

        const_iterator cend() const {
            if (total_size == 0) return const_iterator(list.begin(),0,this->version,this);
            return const_iterator(list.end(), 0,this->version, this);
        }

        iterator begin() const {
            return iterator(list.begin(), 0,this->version, this);
        }

        iterator end() const {
            if (total_size == 0) return iterator(list.begin(),0,this->version,this);
            return iterator(list.end(), 0,this->version, this);
        }


        [[nodiscard]] bool empty() const {
            return total_size == 0;
        }

        [[nodiscard]] size_t size() const {
            return total_size;
        }

        void clear() {
            list.clear();
            total_size = 0;
            version++;
            list.insert_tail(CircularArr(8));
        }

        //以下为维护块长的函数
        //too big
        typename double_list<CircularArr>::iterator split(typename double_list<CircularArr>::iterator block_it) {
            CircularArr &block = *block_it;
            int B = get_ideal_cap();

            if (block.size() <= 2 * B) return block_it;

            int keep_size = block.size() / 2;
            int move_size = block.size() - keep_size;

            CircularArr new_block(move_size + 4);
            //将原block的后一半移进新的block
            for (int i = keep_size; i < block.size(); ++i) {
                new_block.push_back(block[i]);
            }
            for (int i = 0; i < move_size; ++i) {
                block.pop_back();
            }

            auto new_block_it = list.insert_after(block_it, std::move(new_block));

            ++version;

            return new_block_it;
        }

        //too small
        typename double_list<CircularArr>::iterator maintain(typename double_list<CircularArr>::iterator block_it) {
            CircularArr &cur_block = *block_it;
            int B = get_ideal_cap();

            if (cur_block.size() >= B / 2 || list.size() <= 1) return block_it;

            auto node = block_it.ptr;
            auto right_node = node->next, left_node = node->prev;

            //先借右边
            if (right_node != list.tail) {
                CircularArr &right = right_node->data;
                int total = cur_block.size() + right.size();

                if (total <= 2 * B) {
                    for (int i = 0; i < right.size(); ++i) {
                        cur_block.push_back(right[i]);
                    }

                    typename double_list<CircularArr>::iterator right_it(right_node, &list);
                    list.erase(right_it);
                    ++version;
                    return block_it;
                } else {
                    int tar_size = total / 2;
                    int need_size = tar_size - cur_block.size();

                    for (int i = 0; i < need_size; ++i) {
                        cur_block.push_back(right[0]);
                        right.pop_front();
                    }
                    ++version;
                    return block_it;
                }
            } else {
                if (left_node != list.head) {
                    CircularArr &left = left_node->data;
                    int total = cur_block.size() + left.size();

                    if (total <= 2 * B) {
                        for (int i = left.size() - 1; i >= 0; --i) {
                            cur_block.push_front(left[i]);
                        }

                        typename double_list<CircularArr>::iterator left_it(left_node, &list);
                        list.erase(left_it);
                        ++version;
                        return block_it;
                    } else {
                        int tar_size = total / 2;
                        int need_size = tar_size - cur_block.size();

                        for (int i = 0; i < need_size; ++i) {
                            cur_block.push_front(left[left.size() - 1]);
                            left.pop_back();
                        }
                        ++version;
                        return block_it;
                    }
                }
            }
            //++version;
        }

        void check_and_rebuild() {
            int B = get_ideal_cap();
            if (B < base_line / 4 || B > base_line * 4) {
                for (auto it = list.begin(); it != list.end(); ++it) {
                    auto &block = *it;
                    if (block.size() == 0) {
                        if (list.size() > 1) {
                            it = list.erase(it);
                            --it;
                            continue;
                        }
                    }
                    if (block.size() > 2 * B) {
                        it = split(it);
                    } else if (block.size() < B / 2 && block.size() > 8) {
                        it = maintain(it);
                    }
                    //++it;
                }
                base_line = B;
            }
        }


        iterator insert(iterator pos, const T &value) {
            if (pos.tar_lst_ != this) {
                throw sjtu::invalid_iterator();
            }

            if (pos == end()) {   //insert end 特判
                auto last_block_it = list.back();
                auto& block = *list.back();
                block.push_back(value);
                ++total_size;
                ++version;
                int B = get_ideal_cap();

                if (block.size() > 2 * B) {
                    split(last_block_it);
                }

                check_and_rebuild();
                return idx_to_it(total_size - 1);
            }
            auto block_it = pos.db_lst_it_;
            int idx = pos.current_idx_;
            int glob_idx = pos.global_index();
            auto &block = *block_it;

            block.insert(idx, value);
            ++total_size;
            ++version;
            int B = get_ideal_cap();

            if (block.size() > 2 * B) {
                split(pos.db_lst_it_);
            }

            check_and_rebuild();
            return idx_to_it(glob_idx);
        }


        iterator erase(iterator pos) {
            if (pos.tar_lst_ != this) {
                throw sjtu::invalid_iterator();
            }

            auto glb_idx = pos.global_index();
            auto block_it = pos.db_lst_it_;
            int idx = pos.current_idx_;
            int glob_idx = pos.global_index();
            auto &block = *block_it;

            block.erase(idx);
            --total_size;
            ++version;
            int B = get_ideal_cap();

            if (block_it->size() == 0) {
                if (list.size() > 1) {
                    list.erase(block_it);
                }
            } else if (block_it->size() < B / 2 && block_it->size() > 8) {
                maintain(block_it);
            }

            check_and_rebuild();
            return idx_to_it(glob_idx);
        }

        //对头尾的操作暂定不会使迭代器失效
        void push_back(const T &value) {
            auto it = list.back();
            it->push_back(value);
            ++total_size;
            int B = get_ideal_cap();

            if (it->size() > 2 * B) {
                split(it);
            }

            check_and_rebuild();
        }

        /**
         * remove the last element.
         * throw when the container is empty.
         */
        void pop_back() {
            if (total_size == 0) {
                throw sjtu::container_is_empty();
            }
            auto it = list.back();
            it->pop_back();
            --total_size;
            int B = get_ideal_cap();

            if (it->size() == 0) {
                if (list.size() > 1) {
                    list.erase(it);
                }
            } else if (it->size() < B / 2 && it->size() > 8) {
                maintain(it);
            }
        }

        /**
         * insert an element to the beginning.
         */
        void push_front(const T &value) {
            auto it = list.begin();
            it->push_front(value);
            ++total_size;
            int B = get_ideal_cap();

            if (it->size() > 2 * B) {
                split(it);
            }

            check_and_rebuild();
        }

        /**
         * remove the first element.
         * throw when the container is empty.
         */
        void pop_front() {
            if (total_size == 0) {
                throw sjtu::container_is_empty();
            }
            auto it = list.begin();
            it->pop_front();
            --total_size;
            int B = get_ideal_cap();

            if (it->size() == 0) {
                if (list.size() > 1) {
                    list.erase(it);
                }
            } else if (it->size() < B / 2 && it->size() > 8) {
                maintain(it);
            }

            check_and_rebuild();
        }
    };
} // namespace sjtu

#endif
