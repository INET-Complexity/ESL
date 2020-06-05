

#ifndef ME_ALLOCATOR_HPP
#define ME_ALLOCATOR_HPP

//#include <memory>
//#include <array>

#include <boost/pool/pool_alloc.hpp>


namespace esl::computation {


    /*
    template<class object_t_, std::size_t increment_ = 1024>
    class memory_pool
    {
    private:
        struct block
        {
            block *next;
        };

        class buffer
        {
        private:
            static constexpr std::size_t size_ = sizeof(object_t_) > sizeof(block) ? sizeof(object_t_) : sizeof(block);

            std::array<uint8_t, size_ * increment_> data;

        public:
            buffer * const next;

            constexpr buffer(buffer *next)
            : data()        // default constructor gives indeterminate values, which is what we want (optimization)
            , next(next)
            {

            }

            object_t_ *operator[](std::size_t index)
            {
                return reinterpret_cast<object_t_ *>(&data[size_ * index]);
            }
        };

        block *free_            = nullptr;
        buffer *first_          = nullptr;
        std::size_t buffered_   = increment_;

    public:
        memory_pool() = default;
        memory_pool(memory_pool &&memory_pool) = delete;
        memory_pool(const memory_pool &memory_pool) = delete;
        memory_pool operator = (memory_pool &&memory_pool) = delete;
        memory_pool operator = (const memory_pool &memory_pool) = delete;

        ~memory_pool()
        {
            while (first_) {
                buffer *buffer_ = first_;
                first_ = buffer_->next;
                delete buffer_;
            }
        }

        object_t_ *allocate()
        {
            if (free_) {
                auto *block = free_;
                free_ = block->next;
                return reinterpret_cast<object_t_ *>(block);
            }

            if (buffered_ >= increment_) {
                first_ = new buffer(first_);
                buffered_ = 0;
            }

            return first_->operator[](buffered_++);
        }

        void deallocate(object_t_ *pointer)
        {
            auto *block_ = reinterpret_cast<struct block *>(pointer);
            block_->next = free_;
            free_ = block_;
        }
    };

    template<class object_t_, std::size_t increment_ = 1024>
    class allocator
    : private memory_pool<object_t_, increment_>
    {
#ifdef _WIN32
        allocator *copyallocator = nullptr;
        std::allocator<object_t_> *rebindallocator = nullptr;
#endif

    public:

        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef object_t_ *pointer;
        typedef const object_t_ *const_pointer;
        typedef object_t_ &reference;
        typedef const object_t_ &const_reference;
        typedef object_t_ value_type;

        template <class operand_t_>
        struct rebind
        {
            typedef allocator<operand_t_, increment_> other;
        };

#ifdef _WIN32
        allocator() = default;

        allocator(allocator &allocator)
        : copyallocator(&allocator)
        {

        }

        template <class operarand_t_>
        allocator(const allocator<operarand_t_, increment_> &other)
        {
            // ????
            (void) other;
            if (!std::is_same<object_t_, operarand_t_>::value) {
                rebindallocator = new std::allocator<object_t_>();
            }
        }

        ~allocator()
        {
            delete rebindallocator;
        }
#endif

        [[nodiscard]] pointer allocate(size_type n, const void *hint = nullptr)
        {
#ifdef _WIN32
            if(copyallocator)
                return copyallocator->allocate(n, hint);

            if(rebindallocator)
                return rebindallocator->allocate(n, hint);
#endif
            if(n != 1){ // can be 0 too!
                return new object_t_[n];
            }

            if(hint){
                // ignore hints for now
            }

            return memory_pool<object_t_, increment_>::allocate();
        }

        void deallocate(pointer p, size_type n)
        {
#ifdef _WIN32
            if (copyallocator) {
                copyallocator->deallocate(p, n);
                return;
            }

            if (rebindallocator) {
                rebindallocator->deallocate(p, n);
                return;
            }
#endif

            memory_pool<object_t_, increment_>::deallocate(p);
        }

        void construct(pointer p, const_reference val)
        {
            new (p) object_t_(val);
        }

        void destroy(pointer p)
        {
            p->~object_t_();
        }
    };
    */
}




#endif //ME_ALLOCATOR_HPP