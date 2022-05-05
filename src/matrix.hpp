/// -*- C++ -*- Header compatibility <matrix.hpp>

/// @file matrix
/// @author Tyler Swann (oraqlle@github.com)
/// @brief Two dimensional array
/// @version 0.0.9
/// @date 2022-16-22
/// 
/// @copyright Copyright (c) 2021


#ifndef CORTEX_MATRIX_H
#   define CORTEX_MATRIX_H 1

#include <iterators/normal.hpp>
#include <concepts.hpp>

#if __cplusplus >= 202002L

#include <memory>
#include <utility>
#include <iterator>
#include <initializer_list>
#include <iostream>
#include <cassert>
#include <vector>
#include <execution>

#define lexicographical_compare_bug 1



namespace cortex
{
    /// @brief Matrix - Two Dimensional Array
    /// 
    /// @details Matrix is a two dimensional array that 
    /// stores elements sequentially in memory but is 
    /// viewed as a series of rows and columns. Matrix
    /// supports standard mathematical operations as long 
    /// as the underlying data type supports them. This is 
    /// checked at compile time through the use of concepts.
    /// 
    /// @todo Add support for iterators ✔️
    /// @todo Add support for range based for loops ✔️
    /// @todo Add support for basic arithmatic ✔️
    /// @todo Add support for matrix operations
    /// @todo Add support for operator overloads
    /// @todo Add support for std::allocator
    /// @todo Add support for std::initializer_list ✔️
    /// @todo Add support for std::swap
    /// @todo Add support for swaps of different matrix types
    /// @todo Add support for flatten ✔️
    /// @todo Add support for assign
    /// @todo Add support for clear ✔️
    /// @todo Add support for resize
    /// @todo Add support for reserve ⚙️
    /// @todo Add support for shrink_to_fit
    /// @todo Add invokation operator support ✔️
    /// @todo Comparison operators between matrices ✔️
    /// @todo Comparison operators between scalars ✔️
    /// 
    /// @tparam _Tp 
    template<typename _Tp>
    class matrix
    {
    public:
        using value_type                    = _Tp;
        using size_type                     = std::size_t;
        using difference_type               = std::ptrdiff_t;
        using reference                     = _Tp&;
        using const_reference               = const _Tp&;
        using pointer                       = _Tp*;
        using const_pointer                 = const _Tp*;
        using iterator                      = cortex::normal_iterator<pointer, matrix<value_type>>;
        using const_iterator                = cortex::normal_iterator<const_pointer, matrix<value_type>>;
        using reverse_iterator              = std::reverse_iterator<iterator>;
        using const_reverse_iterator        = std::reverse_iterator<const_iterator>;


    protected: 
            size_type m_rows;
            size_type m_columns;

            size_type m_size;
            size_type m_capacity;

            pointer m_data;

    public:

        /// @brief Default Constructor
        /// 
        /// @details Default constructor for matrix.
        constexpr matrix()
        : m_rows(size_type())
        , m_columns(size_type())
        , m_size(size_type())
        , m_capacity(size_type())
        , m_data(pointer())
        { }


        /// @brief Size Constructor
        /// 
        /// @details Constructs a matrix with dimensions of 
        /// cols x rows. Values are default constructed or 
        /// fill constructed depending on the default constructiblity
        /// qualification.
        /// 
        /// @param cols type: [size_type]
        /// @param rows type: [size_type]
        constexpr matrix(size_type rows, size_type cols)
        : m_rows(rows)
        , m_columns(cols)
        , m_size(rows * cols != 0 ? rows * cols : std::max(rows, cols))
        , m_capacity(m_size)
        , m_data(_M_allocate(m_size))
        { 
            if constexpr (std::is_default_constructible_v<value_type>)
                std::uninitialized_default_construct_n(m_data, m_size);
            else
                std::uninitialized_fill_n(m_data, m_size, value_type());
        }


        /// @brief Size + Value Constructor
        /// 
        /// @details Constructs a matrix with dimensions of
        /// cols x rows. Values are constructed from the a 
        /// const reference to a provided value.
        /// 
        /// @param cols type: [size_type]
        /// @param rows type: [size_type]
        /// @param value type: [value_type] | qualifiers: [const] [ref]
        constexpr matrix(size_type rows, size_type cols, const value_type& value)
        : m_rows(rows)
        , m_columns(cols)
        , m_size(rows * cols != 0 ? rows * cols : std::max(rows, cols))
        , m_capacity(m_size)
        , m_data(_M_allocate(m_size))
        { std::uninitialized_fill_n(m_data, m_size, value); }


        /// @brief Copy Constructor
        /// 
        /// @details Constructs a matrix that is a copy of
        /// another matrix of the same underlying type.
        /// 
        /// @param other type: [matrix] | qualifiers: [const] [ref]
        constexpr matrix(const matrix& other)
        : m_rows(other.m_rows)
        , m_columns(other.m_columns)
        , m_size(other.m_size)
        , m_capacity(other.m_size)
        , m_data(_M_allocate(m_size))
        { std::uninitialized_copy_n(other.m_data, m_size, m_data); }


        /// @brief Move Constructor
        /// 
        /// @details Moves ownership of an existing matrix's
        /// resources to this matrix and leaves the other matrix
        /// in a default constructed state.
        /// 
        /// @param other type: [matrix] | qualifiers: [move]
        constexpr matrix(matrix&& other) noexcept
        : m_rows(other.m_rows)
        , m_columns(other.m_columns)
        , m_size(other.m_size)
        , m_capacity(other.m_capacity)
        , m_data(other.m_data)
        { 
            other.m_data = pointer();
            other.m_columns = size_type();
            other.m_rows = size_type();
            other.m_size = size_type(); 
        }


        /// @brief Initialiser List Constructor
        ///
        /// @details Uses std::initializer_list to create a matrix 
        /// from an initializer list of initializer lists. Elements 
        /// ownership is moved to the matrix's memory.
        constexpr matrix(std::initializer_list<std::initializer_list<value_type>> list)
        : m_rows(list.size())
        , m_columns(list.begin()->size())
        , m_size(m_rows * m_columns != 0 ? m_rows * m_columns : std::max(m_rows, m_columns))
        , m_capacity(m_size)
        , m_data(_M_allocate(m_size))
        {
            using init_iter = typename decltype(list)::iterator;
            auto offset { 0 };
            for (init_iter row { list.begin() }; row not_eq list.end(); ++row)
            {
                if (row->size() not_eq this->m_columns)
                    throw std::invalid_argument("Columns must be all the same size");
                std::uninitialized_move_n(row->begin(), this->m_columns, m_data + offset);
                offset += 2;
            }
        }


        /// @brief Copy Assignment
        /// 
        /// @details Copies the contents of another matrix into
        /// this matrix and returns///this. If self assignment occurs
        /// then///this is returned immediately.
        /// 
        /// @param other type: [matrix] | qualifiers: [const] [ref]
        /// @return constexpr matrix& 
        constexpr matrix& operator=(const matrix& other)
        {
            if (*this != other)
            {
                m_rows = other.m_rows;
                m_columns = other.m_columns;
                m_size = other.m_size;
                m_capacity = other.m_size;
                m_data = _M_allocate(m_size);
                std::uninitialized_copy_n(other.m_data, m_size, m_data);
            }
            return *this;
        }


        /// @brief Move Assignment
        /// 
        /// @details Moves the ownership of other's resources to
        /// this matrix and leaves the other matrix in a default
        /// constructed state. Returns///this. If self assignment 
        /// occurs then///this is returned immediately.
        /// 
        /// @param other type: [matrix] | qualifiers: [move]
        /// @return constexpr matrix&
        constexpr matrix& operator=(matrix&& other) noexcept
        {
            if (*this != other)
            {
                m_rows = other.m_rows;
                m_columns = other.m_columns;
                m_size = other.m_size;
                m_capacity = other.m_capacity;
                m_data = other.m_data;

                other.m_data = pointer();
                other.m_rows = size_type();
                other.m_columns = size_type();
                other.m_size = size_type();
            }
            return *this;
        }


        /// @brief Initialiser List Assignment
        ///
        /// @details Uses std::initializer_list to create a matrix 
        /// from an initializer list of initializer lists. Elements 
        /// ownership is moved to the matrix's memory.
        constexpr matrix& operator= (std::initializer_list<std::initializer_list<value_type>> list)
        {   
            m_rows = list.size();
            m_columns = list.begin()->size();
            m_size = (m_rows * m_columns != 0 ? m_rows * m_columns : std::max(m_rows, m_columns));
            m_capacity = m_size;
            m_data = _M_allocate(m_size);

            using init_iter = typename decltype(list)::iterator;
            auto offset { 0 };
            for (init_iter row { list.begin() }; row not_eq list.end(); ++row)
            {
                if (row->size() not_eq this->m_columns)
                    throw std::invalid_argument("Columns must be all the same size");
                std::uninitialized_move_n(row->begin(), this->m_columns, m_data + offset);
                offset += 2;
            }

            return *this;
        }

        
        /// @brief Destructor
        /// 
        /// @details Releases the resources of this matrix
        /// and leaves the matrix in an uninitialized state. 
    #if __cplusplus >= 202202L
        constexpr ~matrix()
    #else
        ~matrix()
    #endif
        {
            if (m_data)
            {
                std::destroy_n(m_data, m_size);
                _M_deallocate(m_data, m_size);
            }

            m_rows = size_type();
            m_columns = size_type();
            m_size = size_type();
            m_capacity = size_type();
        }


        /// @brief Reserves a new memory block for the matrix
        /// 
        /// @details Reserves a new matrix of dimensions new_columns
        /// by new_rows. 
        /// If the new dimensions are smaller than the
        /// current dimensions, no change is made to the matrix's 
        /// capacity but the matrix's dimensions are set to the new 
        /// dimensional values. This effects the access bounds of the 
        /// matrix. 
        /// If the new dimensions are larger than the current dimensions,
        /// then the matrix's capacity is increased to the new capacity 
        /// and the matrix bounds are adjusted accordingly.
        /// A call to matrix::reserve does not preserve the position of the
        /// elements stored within and merely copies the elements to the new 
        /// allocated buffer. 
        /// This means all elements shift to the front of the buffer and the 
        /// new available elements are at the back of the buffer. 
        /// 
        /// matrix::reserve makes no garuntee that the
        /// the order is preserved of elements in the matrix after
        /// it has been called.
        /// 
        /// @todo Ensure that the element's order is preserved. 
        /// 
        /// @param new_columns type: [size_type] 
        /// @param new_rows type: [size_type]
        constexpr void reserve(size_type new_rows, size_type new_columns)
        {
            auto new_capacity = new_rows * new_columns != 0 ? new_rows * new_columns : std::max(new_rows, new_columns);

            if (new_capacity > m_capacity)
            {
                auto new_data = _M_allocate(new_capacity);

                if constexpr (std::is_move_constructible_v<value_type>)                    
                    std::uninitialized_move_n(m_data, m_size, new_data);
                else
                    std::uninitialized_copy_n(m_data, m_size, new_data);

                std::destroy_n(m_data, m_capacity);
                _M_deallocate(m_data, m_capacity);


                m_data = new_data;
                m_capacity = new_capacity;
                m_rows = new_rows;
                m_columns = new_columns;
            }
        }


        /// @brief Clears the matrix elements
        /// 
        /// @details The elements of the matrix are destroyed and
        /// the memory is left in an uninitialized state. 
        /// Capacity of the matrix is left unchanged. 
        /// The dimensions of the matrix and overall size is set 
        /// to zero.
        constexpr void clear() noexcept
        {
            if (m_size)
            {
                std::destroy_n(m_data, m_size);
            
                m_size = size_type();
                m_rows = size_type();
                m_columns = size_type();
            }         
        }


        /// @brief Swaps two matrices of the same type.
        /// 
        /// @details Swaps the contents of two matrices of 
        /// which they are the same type. The swap is performed
        /// by moving ownership of the matrices resources. 
        /// 
        /// @param other type: [matrix] | qualifiers: [ref]
        void swap(matrix& other) noexcept
        {
            matrix tmp = std::move(other);
            other = std::move(*this);
           *this = std::move(tmp);
        }
        

        /// @brief Returns the overall size of the matrix.
        /// 
        /// @return constexpr size_type
        constexpr size_type size() const noexcept
        { return m_size; }


        /// @brief Returns the number of the rows of the matrix.
        /// 
        /// @return constexpr size_type
        constexpr size_type row_size() const noexcept
        { return m_rows; } 


        /// @brief Returns the the number of columns in the matrix.
        /// 
        /// @return constexpr size_type
        constexpr size_type column_size() const noexcept
        { return m_columns; }


        /// @brief Returns the maximum number of elements that
        /// can be stored in the matrix.
        /// 
        /// @return constexpr size_type 
        constexpr size_type max_size() const noexcept
        { return m_size; }


        /// @brief Returns the overall capacity of the matrix.
        /// 
        /// @return constexpr size_type
        constexpr size_type capacity() const noexcept
        { return m_capacity; }


        /// @brief Returns a structured binding of the matrix's 
        /// dimensions.
        /// 
        /// @return constexpr auto : (structured binding)
        constexpr auto dimensions() const noexcept
        { return std::tie(m_columns, m_rows); }


        /// @brief Checks whether the matrix is square.
        /// 
        /// @details If the number of rows and columns are equal,
        /// the matrix is square.
        /// 
        /// @return true 
        /// @return false
        constexpr bool is_square() const noexcept
        { return m_rows == m_columns; }


        /// @brief Checks if the matrix is empty.
        /// 
        /// @return true 
        /// @return false
        constexpr bool empty() const noexcept
        { return m_size == 0; }


        /// @brief Returns the underlying data pointer.
        /// 
        /// @return pointer 
        pointer data() noexcept
        { return _M_data_ptr(m_data); }


        /// @brief Returns the underlying data pointer.
        /// 
        /// @return const_pointer
        const_pointer data() const noexcept
        { return _M_data_ptr(m_data); }


        /// @brief Subscript Operator.
        /// 
        /// @details Returns a reference to the element that 
        /// is at the position indicated by the pointer 
        /// m_data + step. Offers linear access to the matrix's
        /// elements.
        /// 
        /// @param step type: [size_type]
        /// @return constexpr reference
        constexpr reference operator[](size_type step) 
        { return *(m_data + step); }


        /// @brief Two Dimensional Element Access (Point Access).
        /// 
        /// @details Returns a reference to the element that
        /// is at the point position (column, row) of the 
        /// matrix. 
        /// 
        /// @exception std::out_of_range
        /// 
        /// @param column type: [size_type]
        /// @param row type: [size_type]
        /// @return constexpr reference
        constexpr reference at(size_type row, size_type column)
        { 
            _M_range_check(row, column);
            return *(m_data + (m_columns * row) + column);
        }


        /// @brief Two Dimensional Element Access (Point Access).
        /// 
        /// @details Returns a reference to the element that
        /// is at the point position (column, row) of the 
        /// matrix.  
        /// 
        /// @exception std::out_of_range
        /// 
        /// @param column type: [size_type]
        /// @param row type: [size_type]
        /// @return constexpr const_reference 
        constexpr const_reference at(size_type row, size_type column) const
        { 
            _M_range_check(row, column);
            return *(m_data + (m_columns * row) + column); 
        }


        /// @brief Point Access Operator.
        /// 
        /// @details Provides point access to matrix's elements.
        /// Overloads the invokation operator. Utilises the at() method. 
        /// 
        /// @exception std::out_of_range
        /// 
        /// @param column type: [size_type]
        /// @param row type: [size_type]
        /// @return constexpr reference
        constexpr reference operator() (size_type row, size_type column) 
        { return this->at(row, column); }


        /// @brief Point Access Operator.
        /// 
        /// @details Provides point access to matrix's elements.
        /// Overloads the invokation operator. Utilises the at() method. 
        /// 
        /// @exception std::out_of_range 
        /// 
        /// @param column type: [size_type]
        /// @param row type: [size_type]
        /// @return constexpr const_reference 
        constexpr const_reference operator() (size_type row, size_type column) const
        { return this->at(row, column); }


        /// @brief Returns a reference to the front element 
        /// of the matrix. 
        /// 
        /// @return constexpr reference 
        constexpr reference front() noexcept
        { return *m_data; }


        /// @brief Returns a reference to the front element 
        /// of the matrix.
        /// 
        /// @return constexpr const_reference
        constexpr const_reference front() const noexcept
        { return *m_data; }


        /// @brief Returns a reference to the back element 
        /// of the matrix.
        /// 
        /// @return constexpr reference
        constexpr reference back() noexcept
        { return *(m_data + m_size - 1); }


        /// @brief Returns a reference to the back element 
        /// of the matrix.
        /// 
        /// @return constexpr const_reference
        constexpr const_reference back() const noexcept
        { return *(m_data + m_size - 1); }


        /// @brief Flattens the matrix into a std::vector.
        /// 
        /// @details Creates a vector of the matrix's elements
        /// in row major order.
        /// 
        /// @return constexpr std::vector<value_type>
        constexpr std::vector<value_type> flatten() noexcept
        {
            std::vector<value_type> vec(m_size);
            std::copy_n(m_data, m_size, vec.begin());
            return vec;
        }


        /// @brief Iterator to the beginning of the matrix. 
        /// 
        /// @return constexpr iterator
        constexpr iterator begin() noexcept
        { return iterator(m_data); }


        /// @brief Constant iterator to the beginning of the matrix.
        /// 
        /// @return constexpr const_iterator
        constexpr const_iterator begin() const noexcept
        { return const_iterator(m_data); }


        /// @brief Constant iterator to the beginning of the matrix.
        /// 
        /// @return constexpr const_iterator
        constexpr const_iterator cbegin() const noexcept
        { return const_iterator(m_data); }


        /// @brief Reverse iterator to the end of the matrix.
        /// 
        /// @return constexpr reverse_iterator
        constexpr reverse_iterator rbegin() noexcept
        { return reverse_iterator(end()); }


        /// @brief Constant reverse iterator to the end of the matrix.
        /// 
        /// @return constexpr const_reverse_iterator 
        constexpr const_reverse_iterator rbegin() const noexcept
        { return const_reverse_iterator(end()); }


        /// @brief Constant reverse iterator to the end of the matrix.
        /// 
        /// @return constexpr const_reverse_iterator
        constexpr const_reverse_iterator crbegin() const noexcept
        { return const_reverse_iterator(end()); }


        /// @brief Iterator to the end of the matrix.
        /// 
        /// @return constexpr iterator 
        constexpr iterator end() noexcept
        { return iterator(m_data + m_size); }

        
        /// @brief Constant iterator to the end of the matrix.
        /// 
        /// @return constexpr const_iterator
        constexpr const_iterator end() const noexcept
        { return const_iterator(m_data + m_size); }


        /// @brief Constant iterator to the end of the matrix.
        /// 
        /// @return constexpr const_iterator
        constexpr const_iterator cend() const noexcept
        { return const_iterator(m_data + m_size); }


        /// @brief Reverse iterator to the beginning of the matrix.
        /// 
        /// @return constexpr reverse_iterator 
        constexpr reverse_iterator rend() noexcept
        { return reverse_iterator(begin()); }


        /// @brief Constant reverse iterator to the beginning of the matrix.
        /// 
        /// @return constexpr const_reverse_iterator 
        constexpr const_reverse_iterator rend() const noexcept
        { return const_reverse_iterator(begin()); }


        /// @brief Constant reverse iterator to the beginning of the matrix.
        /// 
        /// @return constexpr const_reverse_iterator
        constexpr const_reverse_iterator crend() const noexcept
        { return const_reverse_iterator(begin()); }


        /// @brief Adds the elements of one matrix by another.
        ///
        /// @details Performs an element-wise addition 
        /// between matrices. Matrices are `Addable` iff the 
        /// type of this matrix's elements satisfies the 
        /// concept of `Addable` and satisfies the concept
        /// `AddableWith` with type of the elemnts of the 
        /// passed matrix. A new matrix is created and returned. 
        ///
        /// @requires The type of this matrix's elements are `Addable` 
        /// @requires The type of this matrix's elements and the type
        /// of the passed matrix's element types satisfy `AddableWith`.
        /// 
        /// @tparam _ElemT 
        /// @param other type: matrix<_ElemT> | qualifiers: [const] [ref] 
        /// @return constexpr auto : A matrix whose element's type 
        /// is the sum of the two input matrices element types. 
        template<Addable _ElemT>
            requires AddableWith<value_type, _ElemT>
        constexpr auto add(const matrix<_ElemT>& other)
            -> matrix<decltype(std::declval<value_type>() + std::declval<_ElemT>())>
        {
            if (this->dimensions() != other.dimensions())
                throw std::invalid_argument("In matrix::add - dimensions do not match");

            matrix<decltype(std::declval<value_type>() + std::declval<_ElemT>())> result(this->row_size(), this->column_size());

            std::transform(this->begin(), this->end(), other.begin(), result.begin(), std::plus{});

            return result;
        }


        /// @brief Substracts the elements of one matrix from another.
        ///
        /// @details Performs an element-wise subtraction 
        /// between matrices. Matrices are `Subtractable` 
        /// iff the type of this matrix's elements satisfies 
        /// the concept of `Subtractable` and satisfies the 
        /// concept `SubtractableWith` with type of the elemnts 
        /// of the passed matrix. A new matrix is created and 
        /// returned. 
        ///
        /// @requires The type of this matrix's elements are `Subtractable` 
        /// @requires The type of this matrix's elements and the type
        /// of the passed matrix's element types satisfy `SubtractableWith`.
        /// 
        /// @tparam _ElemT 
        /// @param other type: matrix<_ElemT> | qualifiers: [const] [ref] 
        /// @return constexpr auto : A matrix whose element type 
        /// is the difference of the two input matrices element types.
        template<Subtractable _ElemT>
            requires SubtractableWith<value_type, _ElemT>
        constexpr auto sub(const matrix<_ElemT>& other)
            -> matrix<decltype(std::declval<value_type>() - std::declval<_ElemT>())>
        {
            if (this->dimensions() != other.dimensions())
                throw std::invalid_argument("In matrix::sub - dimensions do not match");

            matrix<decltype(std::declval<value_type>() - std::declval<_ElemT>())> result(this->row_size(), this->column_size());

            std::transform(this->begin(), this->end(), other.begin(), result.begin(), std::minus{});

            return result;
        }


        /// @brief Multiplies the elements of one matrix by another..
        ///
        /// @details Performs an element-wise multiplication 
        /// between matrices. Matrices are `Multiplicible` 
        /// iff the type of this matrix's elements satisfies 
        /// the concept of `Multiplicable` and satisfies the 
        /// concept `MultiplicableWith` with type of the elemnts 
        /// of the passed matrix. A new matrix is created and 
        /// returned. 
        ///
        /// @requires The type of this matrix's elements are `Multiplicable` 
        /// @requires The type of this matrix's elements and the type
        /// of the passed matrix's element types satisfy `MultiplicableWith`.
        /// 
        /// @tparam _ElemT 
        /// @param other type: matrix<_ElemT> | qualifiers: [const] [ref] 
        /// @return constexpr auto : A matrix whose element type 
        /// is the product of the two input matrices element types.
        template<Multiplicable _ElemT>
            requires MultiplicableWith<value_type, _ElemT>
        constexpr auto mul(const matrix<_ElemT>& other)
            -> matrix<decltype(std::declval<value_type>() * std::declval<_ElemT>())>
        {
            if (this->dimensions() != other.dimensions())
                throw std::invalid_argument("In matrix::mult - dimensions do not match");

            matrix<decltype(std::declval<value_type>() * std::declval<_ElemT>())> result(this->row_size(), this->column_size());

            std::transform(this->begin(), this->end(), other.begin(), result.begin(), std::multiplies{});

            return result;
        }


        /// @brief Scalar Matrix Multiplication.
        ///
        /// @details Performs an element-wise multiplication of the matrix
        /// by a 'scalar' value. 
        ///
        /// @requires The type of this matrix's elements are `MultiplicableWith`
        /// the type denoted _ScalarT. 
        /// @requires The type denoted _ScalarT be `Multiplicable`.
        ///
        /// 
        /// @tparam _ScalarT 
        /// @param scalar type: _ScalarT | qualifiers: [const] [ref]
        /// @return matrix<decltype(std::declval<value_type>() * std::declval<_ScalarT>())> 
        template<Multiplicable _ScalarT>
            requires MultiplicableWith<value_type, _ScalarT>
        constexpr auto mul(const _ScalarT& scalar)
            -> matrix<decltype(std::declval<value_type>() * std::declval<_ScalarT>())>
        {
            if (this->empty())
                throw std::invalid_argument("In matrix::mul - scalar multiplication on empty matrix");

            matrix<decltype(std::declval<value_type>() * std::declval<_ScalarT>())> result(this->row_size(), this->column_size());

            std::transform(this->begin(), this->end(), result.begin(), [&](auto& elem) { return elem * scalar; });

            return result;
        }


        /// @brief Divides the elements of one matrix by another.
        ///
        /// @details Performs an element-wise division 
        /// between matrices. Matrices are `Divisible` 
        /// iff the type of this matrix's elements satisfies 
        /// the concept of `Divisible` and satisfies the 
        /// concept `MultiplicibleWith` with type of the elemnts 
        /// of the passed matrix. A new matrix is created and 
        /// returned. 
        ///
        /// @requires The type of this matrix's elements are `Divisible` 
        /// @requires The type of this matrix's elements and the type
        /// of the passed matrix's element types satisfy `DivisibleWith`.
        ///
        /// @note When dividing two matrices, if both matrices elements 
        /// are integrals, the division is performed as integer division.
        /// due to C++ rounding rules.
        /// 
        /// @tparam _ElemT 
        /// @param other type: matrix<_ElemT> | qualifiers: [const] [ref] 
        /// @return constexpr auto : A matrix whose element type 
        /// is the quotient of the two input matrices element types.
        template<Divisible _ElemT>
            requires DivisibleWith<value_type, _ElemT>
        constexpr auto div(const matrix<_ElemT>& other)
            -> matrix<decltype(std::declval<value_type>() / std::declval<_ElemT>())>
        {
            if (this->dimensions() != other.dimensions())
                throw std::invalid_argument("In matrix::div - dimensions do not match");

            matrix<decltype(std::declval<value_type>() / std::declval<_ElemT>())> result(this->row_size(), this->column_size());

            std::transform(this->begin(), this->end(), other.begin(), result.begin(), std::divides{});

            return result;
        }


        /// @brief Scalar Matrix Division
        ///
        /// @details Performs an element-wise division of the matrix
        /// by a 'scalar' value. 
        ///
        /// @requires The type of this matrix's elements are `DivisibleWith`
        /// the type denoted _ScalarT. 
        /// @requires The type denoted _ScalarT be `Divisible`.
        ///
        /// @note When dividing two matrices, if both matrices elements 
        /// are integrals, the division is performed as integer division.
        /// due to C++ rounding rules.
        /// 
        /// @tparam _ScalarT 
        /// @param scalar type: _ScalarT | qualifiers: [const] [ref]
        /// @return matrix<decltype(std::declval<value_type>() / std::declval<_ScalarT>())> 
        template<Divisible _ScalarT>
            requires DivisibleWith<value_type, _ScalarT>
        constexpr auto div(const _ScalarT& scalar)
            -> matrix<decltype(std::declval<value_type>() / std::declval<_ScalarT>())>
        {
            if (this->empty())
                throw std::invalid_argument("In matrix::div - scalar division on empty matrix");
            if (scalar == 0)
                throw std::invalid_argument("In matrix::div - scalar is zero");

            matrix<decltype(std::declval<value_type>() / std::declval<_ScalarT>())> result(this->row_size(), this->column_size());

            std::transform(this->begin(), this->end(), result.begin(), [&](auto& elem) { return elem / scalar; });

            return result;
        }


    private:

        /// @brief Allocates Matrix Recources
        /// 
        /// @details Allocates the memory for the matrix 
        /// using ::operator new.
        /// 
        /// @param __n type: [size_type]
        /// @return constexpr pointer 
        constexpr pointer _M_allocate(size_type __n)
        { return __n != 0 ? static_cast<pointer>(::operator new(__n * sizeof(value_type))) : pointer(); }


        /// @brief Dellocates Matrix Recources
        /// 
        /// @details Dellocates the memory for the matrix 
        /// using ::operator delete.
        /// 
        /// @param __p type: [pointer]
        /// @param __n type: [size_type] | attr: [[maybe_unused]]
        constexpr void _M_deallocate(pointer __p, [[maybe_unused]] size_type __n)
        { 
            ::operator delete(__p
#                       if __cpp_sized_deallocation
			                , __n * sizeof(_Tp)
#                       endif
                            ); 
        }


        /// @brief Checks index's are in the bounds of the matrix
        /// 
        /// @details Checks if __column and __row are withing 
        /// the matrix's bounds. 
        /// 
        /// @exception std::out_of_range
        /// 
        /// @param __column type: [size_type]
        /// @param __row type: [size_type]
        constexpr void _M_range_check(size_type __row, size_type __column) const
        {
            if (__row >= this->row_size() or __column >= this->column_size())
                throw std::out_of_range("matrix::_M_range_check - index out of range.");
        }


        /// @brief Returns the pointer passed to it.
        /// 
        /// @tparam _Up 
        /// @param __ptr type: [_Up*]
        /// @return _Up* 
        template<typename _Up>
        _Up* _M_data_ptr(_Up* __ptr) const noexcept
        { return __ptr; }

#if __cplusplus >= 201103L

        /// @brief Returns the pointer passed to it.
        /// 
        /// @details If the value given is not a builtin 
        /// pointer type, a pointer is created from the 
        /// underlying element type.
        /// 
        /// @tparam _Ptr
        /// @param __ptr type: [_Ptr]
        /// @return typename std::pointer_traits<_Ptr>::element_type*
        template<typename _Ptr>
        typename std::pointer_traits<_Ptr>::element_type* 
        _M_data_ptr(_Ptr __ptr) const
        { return this->empty() ? nullptr : std::to_address(*__ptr); }
#else

        /// @brief Returns the given pointer
        /// 
        /// @details Returns the pointer given to it is 
        /// a builtin pointer type.
        /// 
        /// @tparam _Up 
        /// @param __ptr type: [_Up*] 
        /// @return _Up*
        template<typename _Up>
        _Up* _M_data_ptr(_Up* __ptr) noexcept
        { return __ptr; }
#endif // __cplusplus >= 201103L

    };

    /// @brief Compares two matrices for equality.
    ///
    /// @details Uses std::equal to compare the matrices.
    /// Takes at least O(n) where n = columns x rows = lhs.end() - lhs.begin()
    ///
    /// @requires Matrix elements support equality comparison
    /// that converts to a bool
    ///
    /// @exception Operation is has no exception iff the comparison
    /// between matrix elements is noexcept and std::equal is
    /// noexcept across the range
    ///
    /// @tparam _ElemL
    /// @tparam _ElemR
    /// @rparam lhsE type: [_ElemL]
    /// @rparam rhsE type: [_ElemR]
    /// @param lhs type: [matrix<_ElemL>] | qualifiers: [const] [ref]
    /// @param rhs type: [matrix<_ElemR>] | qualifiers: [const] [ref]
    /// @return true
    /// @return false
    template<typename _ElemL, typename _ElemR>
#if __cpluscplus >= 202002L 
        requires requires (_ElemL lhsE, _ElemR rhsE)
        { { lhsE == rhsE } -> std::convertible_to<bool>; }
    constexpr inline bool
    operator== (const matrix<_ElemL>& lhs, const matrix<_ElemR>& rhs)
        noexcept ( 
               noexcept (std::declval<_ElemL>() == std::declval<_ElemR>()) 
            && noexcept (std::equal(lhs.begin(), lhs.end(), rhs.begin()))
        )
#else
    inline bool
    operator== (const matrix<_ElemL>& lhs, const matrix<_ElemR>& rhs)
#endif 
    { 
        if (lhs.dimensions() not_eq rhs.dimensions())
            return false;
        return std::equal(lhs.begin(), lhs.end(), rhs.begin()); 
    }

/// Current bug with GCC-11.1 with lexicographical_compare_three_way
/// causes cortex::matrix to not compile. Issue: PR
/// Current bug with Clang++-12 with lexicographical_compare_three_way
/// cause cortex::matrix to be compared on matrix size over value precidence
#if __cpp_lib_three_way_comparison && !(lexicographical_compare_bug)

    /// @brief Spaceship Operator for matrices.
    ///
    /// @details Uses std::lexicographical_compare_three_way to 
    /// compare the matrices and generates the !=, <, >, <=, >=
    /// operators.
    /// 
    /// @tparam _ElemL 
    /// @tparam _ElemR 
    /// @param lhs type: [matrix<_ElemL>] | qualifiers: [const] [ref]
    /// @param lhs type: [matrix<_ElemL>] | qualifiers: [const] [ref]
    /// @return constexpr inline auto
    template<typename _ElemL, typename _ElemR>
    constexpr inline auto
    operator<=> (const matrix<_ElemL>& lhs, const matrix<_ElemR>& rhs)
    { 
        return std::lexicographical_compare_three_way(lhs.rend(), lhs.rbegin()
                                                , rhs.rend(), rhs.rbegin()
                                                , std::compare_three_way{}); 
    }

#else // !C++20

    /// @brief Compares two matrices for inequality.
    ///
    /// @details Inverts the result of a equality comparison
    /// between two matrices.
    /// 
    /// @tparam _ElemL 
    /// @tparam _ElemR 
    /// @param lhs type: [matrix<_ElemL>] | qualifiers: [const] [ref]
    /// @param rhs type: [matrix<_ElemR>] | qualifiers: [const] [ref]
    /// @return true 
    /// @return false 
    template<typename _ElemL, typename _ElemR>
    inline bool
    operator!= (const matrix<_ElemL>& lhs, const matrix<_ElemR>& rhs)
    { return !(lhs == rhs); }


    /// @brief Compares if a matrix is lexicographically 
    /// less than another. 
    /// 
    /// @tparam _ElemL 
    /// @tparam _ElemR 
    /// @param lhs type: [matrix<_ElemL>] | qualifiers: [const] [ref]
    /// @param rhs type: [matrix<_ElemR>] | qualifiers: [const] [ref]
    /// @return true 
    /// @return false 
    template<typename _ElemL, typename _ElemR>
    inline bool
    operator< (const matrix<_ElemL>& lhs, const matrix<_ElemR>& rhs)
    { 
        return std::lexicographical_compare(lhs.begin(), lhs.end()
                                        , rhs.begin(), rhs.end()); 
    }


    /// @brief Compares if a matrix is lexicographically
    /// greater than another.
    ///
    /// @details Uses less than comparison and swaps the
    /// order of the arguments. 
    /// 
    /// @tparam _ElemL 
    /// @tparam _ElemR 
    /// @param lhs type: [matrix<_ElemL>] | qualifiers: [const] [ref]
    /// @param rhs type: [matrix<_ElemR>] | qualifiers: [const] [ref]
    /// @return true 
    /// @return false 
    template<typename _ElemL, typename _ElemR>
    inline bool
    operator> (const matrix<_ElemL>& lhs, const matrix<_ElemR>& rhs)
    { return rhs < lhs; }


    /// @brief Compares if a matrix is lexicographically
    /// less than or equal to another. 
    ///
    /// @details Uses less than comparison and swaps the
    /// order of the arguments. If the rhs matrix is less
    /// than the lhs matrix, then the lhs matrix cannot
    /// be less then or equal to the rhs matrix.  
    /// 
    /// @tparam _ElemL 
    /// @tparam _ElemR 
    /// @param lhs type: [matrix<_ElemL>] | qualifiers: [const] [ref]
    /// @param rhs type: [matrix<_ElemR>] | qualifiers: [const] [ref]
    /// @return true 
    /// @return false 
    template<typename _ElemL, typename _ElemR>
    inline bool
    operator<= (const matrix<_ElemL>& lhs, const matrix<_ElemR>& rhs)
    { return !(rhs < lhs); }


    /// @brief Compares if a matrix is lexicographically
    /// greater than or equal to another.
    ///
    /// @details Inverts the result of a less than comparison
    /// between the two matrices. 
    /// 
    /// @tparam _ElemL 
    /// @tparam _ElemR 
    /// @param lhs type: [matrix<_ElemL>] | qualifiers: [const] [ref]
    /// @param rhs type: [matrix<_ElemR>] | qualifiers: [const] [ref]
    /// @return true 
    /// @return false 
    template<typename _ElemL, typename _ElemR>
    inline bool
    operator>= (const matrix<_ElemL>& lhs, const matrix<_ElemR>& rhs)
    { return !(lhs < rhs); }

#endif // three way compare


    template<typename _ElemT>
#if __cpluscplus >= 202002L 
        requires requires (_ElemT lhsE, _ElemT rhsE)
        { { lhsE == rhsE } -> std::convertible_to<bool>; }
    constexpr inline auto
    operator== (const matrix<_ElemT>& __mtx, const _ElemT& __elem)
        noexcept ( noexcept ( std::declval<_ElemT>() == std::declval<_ElemT>() ) )
        -> matrix<bool>
#else
    inline auto
    operator== (const matrix<_ElemT>& mtx, const _ElemT& elem)
        -> matrix<bool>
#endif 
    { 
        matrix<bool> result(mtx.row_size(), mtx.column_size(), false);
        std::transform(mtx.cbegin(), mtx.cend(), result.begin(), 
                        [elem](const _ElemT& mtxE) { return mtxE == elem; });
        return result;
    }



    template<typename _ElemT>
#if __cpluscplus >= 202002L 
        requires requires (_ElemT lhsE, _ElemT rhsE)
        { { lhsE != rhsE } -> std::convertible_to<bool>; }
    constexpr inline auto
    operator!= (const matrix<_ElemT>& __mtx, const _ElemT& __elem)
        noexcept ( noexcept ( std::declval<_ElemT>() != std::declval<_ElemT>() ) )
        -> matrix<bool>
#else
    inline auto
    operator!= (const matrix<_ElemT>& mtx, const _ElemT& elem)
        -> matrix<bool>
#endif 
    { 
        matrix<bool> result(mtx.row_size(), mtx.column_size(), false);
        std::transform(mtx.cbegin(), mtx.cend(), result.begin(), 
                        [elem](const _ElemT& mtxE) { return mtxE != elem; });
        return result;
    }



    template<typename _ElemT>
#if __cpluscplus >= 202002L 
        requires requires (_ElemT lhsE, _ElemT rhsE)
        { { lhsE < rhsE } -> std::convertible_to<bool>; }
    constexpr inline auto
    operator< (const matrix<_ElemT>& __mtx, const _ElemT& __elem)
        noexcept ( noexcept ( std::declval<_ElemT>() < std::declval<_ElemT>() ) )
        -> matrix<bool>
#else
    inline auto
    operator< (const matrix<_ElemT>& mtx, const _ElemT& elem)
        -> matrix<bool>
#endif 
    { 
        matrix<bool> result(mtx.row_size(), mtx.column_size(), false);
        std::transform(mtx.cbegin(), mtx.cend(), result.begin(), 
                        [elem](const _ElemT& mtxE) { return mtxE < elem; });
        return result;
    }


    template<typename _ElemT>
#if __cpluscplus >= 202002L 
        requires requires (_ElemT lhsE, _ElemT rhsE)
        { { lhsE > rhsE } -> std::convertible_to<bool>; }
    constexpr inline auto
    operator> (const matrix<_ElemT>& __mtx, const _ElemT& __elem)
        noexcept ( noexcept ( std::declval<_ElemT>() > std::declval<_ElemT>() ) )
        -> matrix<bool>
#else
    inline auto
    operator> (const matrix<_ElemT>& mtx, const _ElemT& elem)
        -> matrix<bool>
#endif 
    { 
        matrix<bool> result(mtx.row_size(), mtx.column_size(), false);
        std::transform(mtx.cbegin(), mtx.cend(), result.begin(), 
                        [elem](const _ElemT& mtxE) { return mtxE > elem; });
        return result;
    }


    template<typename _ElemT>
#if __cpluscplus >= 202002L 
        requires requires (_ElemT lhsE, _ElemT rhsE)
        { { lhsE <= rhsE } -> std::convertible_to<bool>; }
    constexpr inline auto
    operator<= (const matrix<_ElemT>& __mtx, const _ElemT& __elem)
        noexcept ( noexcept ( std::declval<_ElemT>() > std::declval<_ElemT>() ) )
        -> matrix<bool>
#else
    inline auto
    operator<= (const matrix<_ElemT>& mtx, const _ElemT& elem)
        -> matrix<bool>
#endif 
    { 
        matrix<bool> result(mtx.row_size(), mtx.column_size(), false);
        std::transform(mtx.cbegin(), mtx.cend(), result.begin(), 
                        [elem](const _ElemT& mtxE) { return mtxE <= elem; });
        return result;
    }


    template<typename _ElemT>
#if __cpluscplus >= 202002L 
        requires requires (_ElemT lhsE, _ElemT rhsE)
        { { lhsE >= rhsE } -> std::convertible_to<bool>; }
    constexpr inline auto
    operator>= (const matrix<_ElemT>& __mtx, const _ElemT& __elem)
        noexcept ( noexcept ( std::declval<_ElemT>() > std::declval<_ElemT>() ) )
        -> matrix<bool>
#else
    inline auto
    operator>= (const matrix<_ElemT>& mtx, const _ElemT& elem)
        -> matrix<bool>
#endif 
    { 
        matrix<bool> result(mtx.row_size(), mtx.column_size(), false);
        std::transform(mtx.cbegin(), mtx.cend(), result.begin(), 
                        [elem](const _ElemT& mtxE) { return mtxE >= elem; });
        return result;
    }

} // namespace cortex

namespace std
{   
    /// @brief Uses std::swaps to swap the contents of two matrices. 
    ///
    /// @details Swaps the contents of two matrices if they are of 
    /// the same type.
    ///
    /// @exception std::swap is noexcept if __x.swap(__y) is noexcept.
    /// 
    /// @tparam _Tp 
    /// @param __x type: [cortex::matrix<_Tp>] | qualifiers: [const] [ref]
    /// @param __y type: [cortex::matrix<_Tp>] | qualifiers: [const] [ref]
    /// @return inline void
    template<typename _Tp>
    inline void swap(cortex::matrix<_Tp>& __x, cortex::matrix<_Tp>& __y)
        noexcept( noexcept(__x.swap(__y)) )
    { __x.swap(__y); }
}

#endif // __cplusplus >= 201703L

#endif // CORTEX_MATRIX_H