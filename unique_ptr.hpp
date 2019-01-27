#ifndef UNIQUE_PTR_H
#define UNIQUE_PTR_H

#include <utility>
#include <memory>
#include <variant>

namespace my {

    template<typename T>
    class unique_ptr {

    private:

        struct holder_base;

        typedef T *pointer;
        typedef std::default_delete<T> d_delete;
        typedef holder_base *c_delete;

        pointer _ptr;
        std::variant<c_delete, d_delete> _deleter;

        class holder_base {
        public:
            holder_base() = default;

            virtual ~holder_base() = default;

            virtual void operator()(pointer) = 0;
        };

        template<typename Deleter>
        class deleter_holder : public holder_base {
        public:
            explicit deleter_holder(Deleter &&deleter) : holder_base(), _deleter(std::forward<Deleter>(deleter)) {}

            void operator()(pointer ptr) override {
                _deleter(ptr);
            }

        private:
            Deleter _deleter;
        };

        void do_delete(pointer ptr) {
            if (std::holds_alternative<d_delete>(_deleter)) {
                std::get<d_delete>(_deleter)(ptr);
            } else {
                (*std::get<c_delete>(_deleter))(ptr);
            }
        }

    public:

        unique_ptr() noexcept : _ptr(nullptr), _deleter(std::default_delete<T>{}) {}

        ~unique_ptr() {
            if (_ptr != nullptr) {
                do_delete(_ptr);
            }
            if (std::holds_alternative<c_delete>(_deleter)) {
                delete std::get<c_delete>(_deleter);
            }
            _deleter = nullptr;
        }

        unique_ptr(pointer ptr) : _ptr(ptr), _deleter(std::default_delete<T>{}) {}

        template<typename Deleter>
        unique_ptr(pointer ptr, Deleter &&deleter) : _ptr(ptr), _deleter(
                new deleter_holder<Deleter>(std::forward<Deleter>(deleter))) {}

        unique_ptr(unique_ptr const &) = delete;

        unique_ptr(unique_ptr &&u_ptr) : _ptr(u_ptr._ptr), _deleter(u_ptr._deleter) {
            u_ptr._ptr = nullptr;
            u_ptr._deleter = nullptr;
        }

        unique_ptr &operator=(unique_ptr &&u_ptr) {
            ~unique_ptr();
            unique_ptr(u_ptr).swap(*this);
            return *this;
        }

        template<typename U>
        unique_ptr(unique_ptr<U> &&u_ptr) : _ptr(u_ptr._ptr), _deleter(u_ptr._deleter) {
            u_ptr._ptr = nullptr;
            u_ptr._deleter = nullptr;
        }

        template<typename U>
        unique_ptr &operator=(unique_ptr<U> &&u_ptr) {
            ~unique_ptr();
            _ptr = u_ptr._ptr;
            _deleter = u_ptr._deleter;
            u_ptr._ptr = nullptr;
            u_ptr._deleter = nullptr;
        }

        void swap(unique_ptr &u_ptr) noexcept {
            std::swap(_ptr, u_ptr._ptr);
            std::swap(_deleter, u_ptr._deleter);
        }

        pointer release() noexcept {
            pointer ret = _ptr;
            _ptr = nullptr;
            return ret;
        }

        void reset(pointer ptr = nullptr) noexcept {
            do_delete(_ptr);
            _ptr = ptr;
        }

        template<typename U>
        void reset(U *ptr) noexcept {
            do_delete(_ptr);
            _ptr = ptr;
        }

        pointer get() const noexcept {
            return _ptr;
        }

        explicit operator bool() const noexcept {
            return _ptr;
        }

        T &operator*() const {
            return *_ptr;
        }

        pointer operator->() const noexcept {
            return _ptr;
        }

    };

    template<typename T, typename ...Args>
    unique_ptr<T> make_unique(Args &&...args) {
        return unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

}

#endif // UNIQUE_PTR_H
