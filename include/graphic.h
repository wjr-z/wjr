#ifndef __WJR_GRAPHIC_H
#define __WJR_GRAPHIC_H

#include <list>
#include <vector>
#include "mallocator.h"

namespace wjr {

    template<typename T /*node info*/, typename U /*edge info*/,typename Al = mallocator<int>>
    class wnode {
    private:
        class wedge;
        class simple_ptr;
        using edge_al = typename Al::template rebind<wedge>::other;
        using uint8_t_al = typename Al::template rebind<uint8_t>::other;
        using simple_ptr_al = typename Al::template rebind<simple_ptr>::other;
        using node_al = typename Al::template rebind<T>::other;
        class wedge {
        private:
            using edge_list = std::list<wedge, edge_al>;
            using iterator = typename edge_list::iterator;
        public:
            template<typename...Args>
            wedge(uint8_t* _Count, wnode* _Nxt, Args&&...args)
                : _Count(_Count), _Nxt(_Nxt), _Info(std::forward<Args>(args)...) {

            }

            wedge(const wedge&) = delete;

            wedge(wedge&& other)
                : _Count(other._Count), _Nxt(other._Nxt),
                _Info(std::move(other._Info)) {
                other._Count = nullptr;
            }

            wedge& operator=(const wedge&) = delete;

            ~wedge() {
                if (_Count != nullptr) {
                    switch (*_Count) {
                    case 2:
                        --(*_Count);
                        break;
                    default:
                        assert(false);
                        break;
                    }
                }
            }

            U& info() {
                return _Info;
            }

            const U& info()const {
                return _Info;
            }

            wnode* nxt() {
                return _Nxt;
            }

            const wnode* nxt()const {
                return _Nxt;
            }
        private:
            uint8_t* _Count;
            wnode* _Nxt;
            U _Info;
        };

    public:
        using node = wnode<T, U>;
        using edge = wedge;
    private:
        using edge_list = std::list<edge, edge_al>;
    public:
        using iterator = typename edge_list::iterator;
        using const_iterator = typename edge_list::const_iterator;
    private:
        struct simple_ptr {
            uint8_t* _Count;
            wnode* _Nxt;
            iterator _Iter;
            simple_ptr(uint8_t* _Count, wnode* _Nxt, iterator _Iter)
                : _Count(_Count), _Nxt(_Nxt), _Iter(_Iter) {

            }

            simple_ptr(const simple_ptr&) = delete;

            simple_ptr(simple_ptr&& other)
                : _Count(other._Count), _Nxt(other._Nxt),
                _Iter(std::move(other._Iter)) {
                other._Count = nullptr;
            }

            simple_ptr& operator=(const simple_ptr&) = delete;

            ~simple_ptr() {
                if (_Count != nullptr) {
                    switch (*_Count) {
                    case 2:
                        _Nxt->_Edge.erase(_Iter);
                        // fallthrough
                    case 1:
                        uint8_t_al().deallocate(_Count,1);
                        break;
                    default:
                        assert(false);
                        break;
                    }
                }
            }
        };

    public:

        template<typename...Args>
        wnode(Args&&...args)
            : _Info(node_al().allocate(1)) {
            new (_Info) T(std::forward<Args>(args)...);
        }

        wnode(const wnode&) = delete;

        wnode(wnode&& other)
            : _Info(other._Info), _Edge(std::move(other._Edge)),
            _Cleaner(std::move(other._Cleaner)) {
            other._Info = nullptr;
        }

        wnode& operator=(const wnode&) = delete;

        ~wnode() {
            if (_Info != nullptr) {
                _Info->~T();
                node_al().deallocate(_Info, 1);
                _Edge.clear();
                _Cleaner.clear();
            }
        }

        iterator begin() {
            return _Edge.begin();
        }

        const_iterator begin()const {
            return _Edge.begin();
        }

        const_iterator cbegin()const {
            return _Edge.cbegin();
        }

        iterator end() {
            return _Edge.end();
        }

        const_iterator end()const {
            return _Edge.end();
        }

        const_iterator cend()const {
            return _Edge.cend();
        }

        T& info() {
            return *_Info;
        }

        const T& info()const {
            return *_Info;
        }

        template<typename...Args>
        void link(wnode* _Nxt, Args&&...args) {
            auto ptr = uint8_t_al().allocate(1);
            new (ptr) (uint8_t)(2);
            _Edge.emplace_back(ptr, _Nxt, std::forward<Args>(args)...);
            auto iter = --_Edge.end();
            _Nxt->_Cleaner.emplace_back(ptr, this, iter);
        }

    private:
        T* _Info;
        edge_list _Edge;
        std::vector<simple_ptr, simple_ptr_al> _Cleaner;
    };
}

#endif