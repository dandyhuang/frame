//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 13:29
//  @file:      function_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/base/function.h"
//#include "thirdparty/monitor_sdk_common/base/stdext/shared_ptr.h"
#include "thirdparty/monitor_sdk_common/base/uncopyable.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gmock/gmock.h"
#include "thirdparty/gtest/gtest.h"

using ::testing::Mock;
using ::testing::Return;
using ::testing::StrictMock;

namespace common {

// We need some mocks for test fixture

class NoRef
{
public:
    NoRef() {}

    MOCK_METHOD0(VoidMethod0, void(void));
    MOCK_CONST_METHOD0(VoidConstMethod0, void(void));

    MOCK_METHOD0(IntMethod0, int(void));
    MOCK_CONST_METHOD0(IntConstMethod0, int(void));

private:
    // Particularly important in this test to ensure no copies are made.
    COMMON_DECLARE_UNCOPYABLE(NoRef);
};

class HasRef : public NoRef
{
public:
    HasRef() {}

    MOCK_CONST_METHOD0(AddRef, void(void));
    MOCK_CONST_METHOD0(Release, bool(void));

private:
    // Particularly important in this test to ensure no copies are made.
    COMMON_DECLARE_UNCOPYABLE(HasRef);
};

static const int kParentValue = 1;
static const int kChildValue = 2;

class Parent
{
public:
    virtual ~Parent() {}
    void AddRef(void) const {}
    void Release(void) const {}
    virtual void VirtualSet() { value = kParentValue; }
    void NonVirtualSet() { value = kParentValue; }
    int value;
};

class Child : public Parent
{
public:
    virtual void VirtualSet() { value = kChildValue; }
    void NonVirtualSet() { value = kChildValue; }
};

class NoRefParent
{
public:
    virtual ~NoRefParent() {}
    void AddRef(void) const {}
    virtual void VirtualSet() { value = kParentValue; }
    void NonVirtualSet() { value = kParentValue; }
    int value;
};

class NoRefChild : public NoRefParent
{
    virtual void VirtualSet() { value = kChildValue; }
    void NonVirtualSet() { value = kChildValue; }
};

// Used for probing the number of copies that occur if a type must be coerced
// during argument forwarding in the Run() methods.
struct DerivedCopyCounter
{
    DerivedCopyCounter(int* copies, int* assigns)
        : m_copies(copies), m_assigns(assigns)
    {
    }
    int* m_copies;
    int* m_assigns;
};

// Used for probing the number of copies in an argument.
class CopyCounter
{
public:
    CopyCounter(int* copies, int* assigns)
        : m_copies(copies), m_assigns(assigns)
    {
    }

    CopyCounter(const CopyCounter& other)
        : m_copies(other.m_copies),
          m_assigns(other.m_assigns)
    {
        (*m_copies)++;
    }

    // Probing for copies from coerscion.
    CopyCounter(const DerivedCopyCounter& other) // NOLINT(runtime/explicit)
        : m_copies(other.m_copies),
          m_assigns(other.m_assigns)
    {
        (*m_copies)++;
    }

    const CopyCounter& operator=(const CopyCounter& rhs)
    {
        m_copies = rhs.m_copies;
        m_assigns = rhs.m_assigns;

        if (m_assigns)
        {
            (*m_assigns)++;
        }

        return *this;
    }

    int NumCopies() const
    {
        return *m_copies;
    }

    int NumAssigns() const
    {
        return *m_assigns;
    }

private:
    int* m_copies;
    int* m_assigns;
};

// Some test functions that we can Bind to.
void DoNothing()
{
}

class FunctionTest : public ::testing::Test
{
public:
    FunctionTest() :
        m_function_a(Bind(DoNothing)),
        m_function_b(Bind(DoNothing)),
        m_const_has_ref_ptr(&m_has_ref),
        m_const_no_ref_ptr(&m_no_ref)
    {
        static_func_mock_ptr = &m_static_func_mock;
    }

    virtual ~FunctionTest()
    {
    }

    static void VoidFunc0(void)
    {
        static_func_mock_ptr->VoidMethod0();
    }

    static int IntFunc0(void) { return static_func_mock_ptr->IntMethod0(); }

protected:
    Function<void(void)> m_function_a;
    const Function<void(void)> m_function_b;  // Ensure APIs work with const.
    Function<void(void)> m_null_function;

    StrictMock<NoRef> m_no_ref;
    StrictMock<HasRef> m_has_ref;
    const HasRef* m_const_has_ref_ptr;
    const NoRef* m_const_no_ref_ptr;
    StrictMock<NoRef> m_static_func_mock;

    // Used by the static functions to perform expectations.
    static StrictMock<NoRef>* static_func_mock_ptr;

private:
    COMMON_DECLARE_UNCOPYABLE(FunctionTest);
};

StrictMock<NoRef>* FunctionTest::static_func_mock_ptr;

// Ensure we can create unbound functions. We need this to be able to store
// them in class members that can be initialized later.
TEST_F(FunctionTest, DefaultConstruction)
{
    Function<void ()> f0;
    Function<void (int)> f1;
    Function<void (int, int)> f2;
    Function<void (int, int, int)> f3;
    Function<void (int, int, int, int)> f4;
    Function<void (int, int, int, int, int)> f5;
    Function<void (int, int, int, int, int, int)> f6;
    Function<void (int, int, int, int, int, int, int)> f7;
    Function<void (int, int, int, int, int, int, int, int)> f8;
    EXPECT_TRUE(f0 == NULL);
    EXPECT_TRUE(f1 == NULL);
    EXPECT_TRUE(f2 == NULL);
    EXPECT_TRUE(f3 == NULL);
    EXPECT_TRUE(f4 == NULL);
    EXPECT_TRUE(f5 == NULL);
    EXPECT_TRUE(f6 == NULL);
    EXPECT_TRUE(f7 == NULL);
    EXPECT_TRUE(f8 == NULL);
    EXPECT_TRUE(!f0);
    EXPECT_TRUE(!f1);
    EXPECT_TRUE(!f2);
    EXPECT_TRUE(!f3);
    EXPECT_TRUE(!f4);
    EXPECT_TRUE(!f5);
    EXPECT_TRUE(!f6);
    EXPECT_TRUE(!f7);
    EXPECT_TRUE(!f8);
}

TEST_F(FunctionTest, NullConstruction)
{
    Function<void ()> f0(NULL);
    Function<void (int)> f1(NULL);
    Function<void (int, int)> f2(NULL);
    Function<void (int, int, int)> f3(NULL);
    Function<void (int, int, int, int)> f4(NULL);
    Function<void (int, int, int, int, int)> f5(NULL);
    Function<void (int, int, int, int, int, int)> f6(NULL);
    Function<void (int, int, int, int, int, int, int)> f7(NULL);
    Function<void (int, int, int, int, int, int, int, int)> f8(NULL);
    EXPECT_TRUE(f0 == NULL);
    EXPECT_TRUE(f1 == NULL);
    EXPECT_TRUE(f2 == NULL);
    EXPECT_TRUE(f3 == NULL);
    EXPECT_TRUE(f4 == NULL);
    EXPECT_TRUE(f5 == NULL);
    EXPECT_TRUE(f6 == NULL);
    EXPECT_TRUE(f7 == NULL);
    EXPECT_TRUE(f8 == NULL);
    EXPECT_TRUE(!f0);
    EXPECT_TRUE(!f1);
    EXPECT_TRUE(!f2);
    EXPECT_TRUE(!f3);
    EXPECT_TRUE(!f4);
    EXPECT_TRUE(!f5);
    EXPECT_TRUE(!f6);
    EXPECT_TRUE(!f7);
    EXPECT_TRUE(!f8);
}

TEST_F(FunctionTest, IsNull)
{
    EXPECT_TRUE(!m_null_function);
    EXPECT_FALSE(!m_function_a);
    EXPECT_FALSE(!m_function_b);
}

TEST_F(FunctionTest, CopyAndAssign)
{
    m_function_a = m_null_function;
    m_null_function = m_function_a;
    EXPECT_TRUE(m_function_a == m_null_function);

    m_function_a = m_function_b;
    EXPECT_TRUE(m_function_a == m_function_b);
    EXPECT_TRUE(m_function_a != m_null_function);
}

TEST_F(FunctionTest, Equal)
{
    EXPECT_TRUE(m_function_a == m_function_a);
    EXPECT_FALSE(m_function_a == m_function_b);
    EXPECT_FALSE(m_function_b == m_function_a);

    // We should compare based on internal state, not object.
    Function<void (void)> function_c(Bind(DoNothing));
    Function<void (void)> function_a2 = m_function_a;
    EXPECT_TRUE(m_function_a == function_a2);
    EXPECT_FALSE(m_function_a == function_c);

    // Empty is always equal to empty.
    Function<void(void)> empty2;
    EXPECT_TRUE(m_null_function == empty2);
}

TEST_F(FunctionTest, Clear)
{
    // Resetting should bring us back to empty.
    m_function_a.Clear();
    EXPECT_TRUE(m_function_a == NULL);
}

TEST_F(FunctionTest, AssignNull)
{
    m_function_a = NULL;
    EXPECT_TRUE(m_function_a == NULL);
}

TEST_F(FunctionTest, Swap)
{
    Function<void ()> a = m_function_a;
    Function<void ()> b = m_function_b;

    EXPECT_TRUE(a == m_function_a);
    EXPECT_TRUE(a == m_function_a);
    EXPECT_TRUE(b == m_function_b);

    std::swap(a, b);
    EXPECT_TRUE(a == m_function_b);
    EXPECT_TRUE(b == m_function_a);
}

int Sum(int a, int b, int c, int d, int e, int f)
{
    return a + b + c + d + e + f;
}

TEST_F(FunctionTest, ArityTest)
{
    Function<int (void)> c0 = Bind(&Sum, 32, 16, 8, 4, 2, 1);
    EXPECT_EQ(63, c0());
    Function<int (int)> c1 = Bind(&Sum, 32, 16, 8, 4, 2);
    EXPECT_EQ(75, c1(13));
    Function<int (int, int)> c2 = Bind(&Sum, 32, 16, 8, 4);
    EXPECT_EQ(85, c2(13, 12));
    Function<int (int, int, int)> c3 = Bind(&Sum, 32, 16, 8);
    EXPECT_EQ(92, c3(13, 12, 11));
    Function<int (int, int, int, int)> c4 = Bind(&Sum, 32, 16);
    EXPECT_EQ(94, c4(13, 12, 11, 10));
    Function<int (int, int, int, int, int)> c5 = Bind(&Sum, 32);
    EXPECT_EQ(87, c5(13, 12, 11, 10, 9));
    Function<int (int, int, int, int, int, int)> c6 = Bind(&Sum);
    EXPECT_EQ(69, c6(13, 12, 11, 10, 9, 14));
}

int Add(int x, int y)
{
    return x + y;
}

TEST_F(FunctionTest, BindFunction)
{
    Function<int (int x, int y)> add = Bind(Add);
    EXPECT_EQ(3, add(1, 2));

    Function<int (int x)> inc = Bind(add, 1);
    EXPECT_EQ(2, inc(1));

    Function<int ()> two = Bind(inc, 1);
    EXPECT_EQ(2, two());
}

// Function type support.
//   - Normal function.
//   - Method bound to non-const object.
//   - Const method bound to non-const object.
//   - Const method bound to const object.
//   - Derived classes can be used with pointers to non-virtual base functions.
//   - Derived classes can be used with pointers to virtual base functions (and
//     preserve virtual dispatch).
TEST_F(FunctionTest, FunctionTypeSupport)
{
    EXPECT_CALL(m_static_func_mock, VoidMethod0());
    EXPECT_CALL(m_has_ref, AddRef()).Times(3);
    EXPECT_CALL(m_has_ref, Release()).Times(3);
    EXPECT_CALL(m_has_ref, VoidMethod0());
    EXPECT_CALL(m_has_ref, VoidConstMethod0()).Times(2);
    Function<void ()> normal_cb = Bind(&VoidFunc0);
    Function<void ()> method_cb = Bind(&HasRef::VoidMethod0, &m_has_ref);
    Function<void ()> const_method_nonconst_obj_cb =
        Bind(&HasRef::VoidConstMethod0, &m_has_ref);
    Function<void ()> const_method_const_obj_cb = Bind(&HasRef::VoidConstMethod0,
                                        m_const_has_ref_ptr);
    normal_cb();
    method_cb();
    const_method_nonconst_obj_cb();
    const_method_const_obj_cb();
    Child child;
    child.value = 0;
    Function<void ()> virtual_set_cb = Bind(&Parent::VirtualSet, &child);
    virtual_set_cb();
    EXPECT_EQ(kChildValue, child.value);
    child.value = 0;
    Function<void ()> non_virtual_set_cb = Bind(&Parent::NonVirtualSet, &child);
    non_virtual_set_cb();
    EXPECT_EQ(kParentValue, child.value);
}

// Return value support.
//   - Function with return value.
//   - Method with return value.
//   - Const method with return value.
TEST_F(FunctionTest, ReturnValues)
{
    EXPECT_CALL(m_static_func_mock, IntMethod0()).WillOnce(Return(1337));
    EXPECT_CALL(m_has_ref, AddRef()).Times(3);
    EXPECT_CALL(m_has_ref, Release()).Times(3);
    EXPECT_CALL(m_has_ref, IntMethod0()).WillOnce(Return(31337));
    EXPECT_CALL(m_has_ref, IntConstMethod0())
    .WillOnce(Return(41337))
    .WillOnce(Return(51337));
    Function<int (void)> normal_cb = Bind(&IntFunc0);
    Function<int (void)> method_cb = Bind(&HasRef::IntMethod0, &m_has_ref);
    Function<int (void)> const_method_nonconst_obj_cb =
        Bind(&HasRef::IntConstMethod0, &m_has_ref);
    Function<int (void)> const_method_const_obj_cb =
        Bind(&HasRef::IntConstMethod0, m_const_has_ref_ptr);
    EXPECT_EQ(1337, normal_cb());
    EXPECT_EQ(31337, method_cb());
    EXPECT_EQ(41337, const_method_nonconst_obj_cb());
    EXPECT_EQ(51337, const_method_const_obj_cb());
}

// Argument binding tests.
//   - Argument binding to primitive.
//   - Argument binding to primitive pointer.
//   - Argument binding to a literal integer.
//   - Argument binding to a literal string.
//   - Argument binding with template function.
//   - Argument binding to an object.
//   - Argument gets type converted.
//   - Pointer argument gets converted.
//   - Const Reference forces conversion.

int Identity(int n)
{
    return n;
}

const char* CStringIdentity(const char* s)
{
    return s;
}

template <typename T>
T PolymorphicIdentity(T t)
{
    return t;
}

int UnwrapNoRefParent(NoRefParent p)
{
    return p.value;
}

int UnwrapNoRefParentPtr(NoRefParent* p)
{
    return p->value;
}

int UnwrapNoRefParentConstRef(const NoRefParent& p)
{
    return p.value;
}

TEST_F(FunctionTest, ArgumentBinding)
{
    int n = 2;
    Function<int (void)> bind_primitive_cb = Bind(&Identity, n);
    EXPECT_EQ(n, bind_primitive_cb());
    Function<int* (void)> bind_primitive_pointer_cb =
        Bind(&PolymorphicIdentity<int*>, &n);
    EXPECT_EQ(&n, bind_primitive_pointer_cb());
    Function<int (void)> bind_int_literal_cb = Bind(&Identity, 3);
    EXPECT_EQ(3, bind_int_literal_cb());
    Function<const char*(void)> bind_string_literal_cb =
        Bind(&CStringIdentity, "hi");
    EXPECT_STREQ("hi", bind_string_literal_cb());
    Function<int (void)> bind_template_function_cb =
        Bind(&PolymorphicIdentity<int>, 4);
    EXPECT_EQ(4, bind_template_function_cb());
    NoRefParent p;
    p.value = 5;
    Function<int (void)> bind_object_cb = Bind(&UnwrapNoRefParent, p);
    EXPECT_EQ(5, bind_object_cb());
    NoRefChild c;
    c.value = 6;
    Function<int (void)> bind_promotes_cb = Bind(&UnwrapNoRefParent, c);
    EXPECT_EQ(6, bind_promotes_cb());
    c.value = 7;
//    Function<int(void)> bind_pointer_promotes_cb =
//        Bind(&UnwrapNoRefParentPtr, &c);
//    EXPECT_EQ(7, bind_pointer_promotes_cb());
    c.value = 8;
    Function<int (void)> bind_const_reference_promotes_cb =
        Bind(&UnwrapNoRefParentConstRef, c);
    EXPECT_EQ(8, bind_const_reference_promotes_cb());
}

// Unbound argument type support tests.
//   - Unbound value.
//   - Unbound pointer.
//   - Unbound reference.
//   - Unbound const reference.
//   - Unbound unsized array.
//   - Unbound sized array.
//   - Unbound array-of-arrays.
template <typename T>
void VoidPolymorphic1(T t)
{

}

TEST_F(FunctionTest, UnboundArgumentTypeSupport)
{
    Function<void (int)> unbound_value_cb = Bind(&VoidPolymorphic1<int>);
    Function<void (int*)> unbound_pointer_cb = Bind(&VoidPolymorphic1<int*>);
    Function<void (int&)> unbound_ref_cb = Bind(&VoidPolymorphic1<int&>);
    Function<void (const int&)> unbound_const_ref_cb =
        Bind(&VoidPolymorphic1<const int&>);
    //Function<void (int[])> unbound_unsized_array_cb =
    //    Bind(&VoidPolymorphic1<int[]>);
    //Function<void (int[2])> unbound_sized_array_cb =
    //    Bind(&VoidPolymorphic1<int[2]>);
    //Function<void (int[][2])> unbound_array_of_arrays_cb =
    //    Bind(&VoidPolymorphic1<int[][2]>);
}

// Function with unbound reference parameter.
//   - Original paraemter is modified by function.
void RefArgSet(int& n)
{
    n = 2;
}

TEST_F(FunctionTest, UnboundReferenceSupport)
{
    int n = 0;
    Function<void (int&)> unbound_ref_cb = Bind(&RefArgSet);
    unbound_ref_cb(n);
    EXPECT_EQ(2, n);
}

// Functions that take reference parameters.
//  - Forced reference parameter type still stores a copy.
//  - Forced const reference parameter type still stores a copy.
TEST_F(FunctionTest, ReferenceArgumentBinding)
{
    int n = 1;
    int& ref_n = n;
    const int& const_ref_n = n;
    Function<int (void)> ref_copies_cb = Bind(&Identity, ref_n);
    EXPECT_EQ(n, ref_copies_cb());
    n++;
    EXPECT_EQ(n - 1, ref_copies_cb());
    Function<int (void)> const_ref_copies_cb = Bind(&Identity, const_ref_n);
    EXPECT_EQ(n, const_ref_copies_cb());
    n++;
    EXPECT_EQ(n - 1, const_ref_copies_cb());
}

// Check that we can pass in arrays and have them be stored as a pointer.
//  - Array of values stores a pointer.
//  - Array of const values stores a pointer.

int ArrayGet(const int array[], int n)
{
    return array[n];
}

TEST_F(FunctionTest, ArrayArgumentBinding)
{
    int array[4] = {1, 1, 1, 1};
    const int (*const_array_ptr)[4] = &array;
    Function<int (void)> array_cb = Bind(&ArrayGet, array, 1);
    EXPECT_EQ(1, array_cb());
    Function<int (void)> const_array_cb = Bind(&ArrayGet, *const_array_ptr, 1);
    EXPECT_EQ(1, const_array_cb());
    array[1] = 3;
    EXPECT_EQ(3, array_cb());
    EXPECT_EQ(3, const_array_cb());
}

// Verify SupportsAddRefAndRelease correctly introspects the class type for
// AddRef() and Release().
TEST_F(FunctionTest, SupportsAddRefAndRelease)
{
    EXPECT_TRUE(common::internal::SupportsAddRefAndRelease<HasRef>::Value);
    EXPECT_FALSE(common::internal::SupportsAddRefAndRelease<NoRef>::Value);
    // StrictMock<T> is a derived class of T.  So, we use StrictMock<HasRef> and
    // StrictMock<NoRef> to test that SupportsAddRefAndRelease works over
    // inheritance.
    EXPECT_TRUE(common::internal::SupportsAddRefAndRelease<StrictMock<HasRef> >::Value);
    EXPECT_FALSE(common::internal::SupportsAddRefAndRelease<StrictMock<NoRef> >::Value);
}

// Unretained() wrapper support.
//   - Method bound to Unretained() non-object.
//   - Const method bound to Unretained() non-const object.
//   - Const method bound to Unretained() const object.
TEST_F(FunctionTest, Unretained)
{
    EXPECT_CALL(m_no_ref, VoidMethod0());
    EXPECT_CALL(m_no_ref, VoidConstMethod0()).Times(2);
    Function<void (void)> method_cb =
        Bind(&NoRef::VoidMethod0, Unretained(&m_no_ref));
    method_cb();
    Function<void (void)> const_method_cb =
        Bind(&NoRef::VoidConstMethod0, Unretained(&m_no_ref));
    const_method_cb();
    Function<void (void)> const_method_const_ptr_cb =
        Bind(&NoRef::VoidConstMethod0, Unretained(m_const_no_ref_ptr));
    const_method_const_ptr_cb();
}

// ConstRef() wrapper support.
//   - Binding w/o ConstRef takes a copy.
//   - Binding a ConstRef takes a reference.
//   - Binding ConstRef to a function ConstRef does not copy on invoke.
int GetCopies(const CopyCounter& counter)
{
    return counter.NumCopies();
}

TEST_F(FunctionTest, ConstRef)
{
    int n = 1;
    Function<int (void)> copy_cb = Bind(&Identity, n);
    Function<int (void)> const_ref_cb = Bind(&Identity, ConstRef(n));
    EXPECT_EQ(n, copy_cb());
    EXPECT_EQ(n, const_ref_cb());
    n++;
    EXPECT_EQ(n - 1, copy_cb());
    EXPECT_EQ(n, const_ref_cb());
    int copies = 0;
    int assigns = 0;
    CopyCounter counter(&copies, &assigns);
    Function<int (void)> all_const_ref_cb =
        Bind(&GetCopies, ConstRef(counter));
    EXPECT_EQ(0, all_const_ref_cb());
    EXPECT_EQ(0, copies);
    EXPECT_EQ(0, assigns);
}

// Argument Copy-constructor usage for non-reference parameters.
//   - Bound arguments are only copied once.
//   - Forwarded arguments are only copied once.
//   - Forwarded arguments with coerscions are only copied twice (once for the
//     coerscion, and one for the final dispatch).
TEST_F(FunctionTest, ArgumentCopies)
{
    int copies = 0;
    int assigns = 0;
    CopyCounter counter(&copies, &assigns);
    Function<void (void)> copy_cb =
        Bind(&VoidPolymorphic1<CopyCounter>, counter);
    EXPECT_GE(1, copies);
    EXPECT_EQ(0, assigns);

    copies = 0;
    assigns = 0;
    Function<void (CopyCounter)> forward_cb =
        Bind(&VoidPolymorphic1<CopyCounter>);
    forward_cb(counter);
    EXPECT_GE(1, copies);
    EXPECT_EQ(0, assigns);

    copies = 0;
    assigns = 0;
    DerivedCopyCounter dervied(&copies, &assigns);
    Function<void (CopyCounter)> coerce_cb =
        Bind(&VoidPolymorphic1<CopyCounter>);
    coerce_cb(dervied);
    EXPECT_GE(2, copies);
    EXPECT_EQ(0, assigns);
}

// No-compile tests. These should not compile. If they do, we are allowing
// error-prone, or incorrect behavior in the function system.  Uncomment the
// tests to check.
// TODO(phongchen): Is there any better way to do this kind of test?

// Only useful in no-compile tests.
int UnwrapParent(Parent& p)
{
    return p.value;
}

TEST_F(FunctionTest, NoCompile)
{
#if 0
    // - Method bound to const-object.
    //
    // Only const methods should be allowed to work with const objects.

    Function<void(void)> method_to_const_cb =
        Bind(&HasRef::VoidMethod0, m_const_has_ref_ptr);
    method_to_const_cb();
    // - Method bound to non-refcounted object.
    // - Const Method bound to non-refcounted object.
    //
    // We require refcounts unless you have Unretained().
#endif

#if 0
    Function<void(void)> no_ref_cb =
        Bind(&NoRef::VoidMethod0, &m_no_ref);
    no_ref_cb();
    Function<void(void)> no_ref_const_cb =
        Bind(&NoRef::VoidConstMethod0, &m_no_ref);
    no_ref_const_cb();
#endif

#if 0
    // - Unretained() used with a refcounted object.
    //
    // If the object supports refcounts, unretaining it in the function is a
    // memory management contract break.
    Function<void(void)> unretained_cb =
        Bind(&HasRef::VoidConstMethod0, Unretained(&m_has_ref));
    unretained_cb();
#endif

#if 0
    // - Const argument used with non-const pointer parameter of same type.
    // - Const argument used with non-const pointer parameter of super type.
    //
    // This is just a const-correctness check.
    const Parent* const_parent_ptr;
    const Child* const_child_ptr;
    Function<Parent*(void)> pointer_same_cb =
        Bind(&PolymorphicIdentity<Parent*>, const_parent_ptr);
    pointer_same_cb();
    Function<Parent*(void)> pointer_super_cb =
        Bind(&PolymorphicIdentity<Parent*>, const_child_ptr);
    pointer_super_cb();
#endif

#if 0
    // - Construction of Function<A> from Function<B> if A is supertype of B.
    //   Specific example: Function<void(void)> a; Function<int(void)> b; a = b;
    //
    // While this is technically safe, most people aren't used to it when coding
    // C++ so if this is happening, it is almost certainly an error.
    Function<int (void)> cb_a0 = Bind(&Identity, 1);
    Function<void(void)> cb_b0 = cb_a0;
#endif

#if 0
    // - Assignment of Function<A> from Function<B> if A is supertype of B.
    // See explanation above.
    //
    Function<int (void)> cb_a1 = Bind(&Identity, 1);
    Function<void(void)> cb_b1;
    cb_a1 = cb_b1;
#endif

    // - Functions with reference parameters, unsupported.
    //
    // First, non-const reference parameters are disallowed by the our style
    // guide.
    // Seconds, since we are doing argument forwarding it becomes very tricky
    // to avoid copies, maintain const correctness, and not accidentally have
    // the function be modifying a temporary, or a copy.
#if 0
    Parent p;
    Function<int (void)> ref_cb = Bind(&UnwrapParent, p);
    ref_cb();
#endif

#if 0
    // - A method should not be bindable with an array of objects.
    //
    // This is likely not wanted behavior. We specifically check for it though
    // because it is possible, depending on how you implement prebinding, to
    // implicitly convert an array type to a pointer type.
    HasRef p[10];
    Function<void(void)> method_bound_to_array_cb =
        Bind(&HasRef::VoidConstMethod0, p);
    method_bound_to_array_cb();
#endif

#if 0
    // - Refcounted types should not be bound as a raw pointer.
    HasRef for_raw_ptr;
    Function<void(void)> ref_count_as_raw_ptr =
        Bind(&VoidPolymorphic1<HasRef*>, &for_raw_ptr);
#endif
}

#if defined(_WIN32)
int __fastcall FastCallFunc(int n)
{
    return n;
}

int __stdcall StdCallFunc(int n)
{
    return n;
}

// Windows specific calling convention support.
//   - Can bind a __fastcall function.
//   - Can bind a __stdcall function.
TEST_F(FunctionTest, WindowsCallingConventions)
{
    Function<int (void)> fastcall_cb = Bind(&FastCallFunc, 1);
    EXPECT_EQ(1, fastcall_cb());
    Function<int (void)> stdcall_cb = Bind(&StdCallFunc, 2);
    EXPECT_EQ(2, stdcall_cb());
}
#endif

//class Shared : public stdext::enable_shared_from_this<Shared>
//{
//public:
//    int F() { return 42; }
//};
//
//TEST(Function, SharedPtr)
//{
//    stdext::shared_ptr<Shared> p(new Shared());
//    Function<int ()> f = Bind(&Shared::F, p);
//    EXPECT_EQ(42, f());
//}

} // end of namespace common
