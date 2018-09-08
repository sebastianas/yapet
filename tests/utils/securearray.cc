#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "securearray.hh"

class SecureArrayTest : public CppUnit::TestFixture {
   public:
    static CppUnit::TestSuite *suite() {
        CppUnit::TestSuite *suiteOfTests =
            new CppUnit::TestSuite("SecureArrayTest");

        suiteOfTests->addTest(new CppUnit::TestCaller<SecureArrayTest>(
            "should not allow initialization with negative size",
            &SecureArrayTest::testNegativeSize));

        suiteOfTests->addTest(new CppUnit::TestCaller<SecureArrayTest>(
            "test empty array", &SecureArrayTest::testEmptyArray));
        suiteOfTests->addTest(new CppUnit::TestCaller<SecureArrayTest>(
            "should set empty array upon destruction",
            &SecureArrayTest::testZeroOutBuffer));
        suiteOfTests->addTest(new CppUnit::TestCaller<SecureArrayTest>(
            "copy constructor", &SecureArrayTest::testCopy));
        suiteOfTests->addTest(new CppUnit::TestCaller<SecureArrayTest>(
            "assignment", &SecureArrayTest::testAssignment));
        suiteOfTests->addTest(new CppUnit::TestCaller<SecureArrayTest>(
            "move", &SecureArrayTest::testMove));
        suiteOfTests->addTest(new CppUnit::TestCaller<SecureArrayTest>(
            "move assignment", &SecureArrayTest::testMove));
        suiteOfTests->addTest(new CppUnit::TestCaller<SecureArrayTest>(
            "equality", &SecureArrayTest::testEquality));
        suiteOfTests->addTest(new CppUnit::TestCaller<SecureArrayTest>(
            "should properly implement index operator",
            &SecureArrayTest::testIndexOperator));
        suiteOfTests->addTest(new CppUnit::TestCaller<SecureArrayTest>(
            "should properly handle assignment of empty array",
            &SecureArrayTest::testEmptyAssignment));
        suiteOfTests->addTest(new CppUnit::TestCaller<SecureArrayTest>(
            "should properly handle copy of empty array",
            &SecureArrayTest::testEmptyCopy));
        suiteOfTests->addTest(new CppUnit::TestCaller<SecureArrayTest>(
            "should properly handle move of empty array",
            &SecureArrayTest::testEmptyMove));
        suiteOfTests->addTest(new CppUnit::TestCaller<SecureArrayTest>(
            "should properly add two secure arrays",
            &SecureArrayTest::testAdd));
        suiteOfTests->addTest(new CppUnit::TestCaller<SecureArrayTest>(
            "test toSecureArray", &SecureArrayTest::testToSecureArray));
        suiteOfTests->addTest(new CppUnit::TestCaller<SecureArrayTest>(
            "test toSecureArray with empty string",
            &SecureArrayTest::testToSecureArrayEmpty));
        suiteOfTests->addTest(new CppUnit::TestCaller<SecureArrayTest>(
            "test SecureArray copy operator",
            &SecureArrayTest::testCopyOperator));

        return suiteOfTests;
    }

    void testNegativeSize() {
        CPPUNIT_ASSERT_THROW(yapet::SecureArray{-1}, std::invalid_argument);
    }

    void testEmptyArray() {
        yapet::SecureArray *ptr{new yapet::SecureArray()};

        CPPUNIT_ASSERT_EQUAL(0u, ptr->size());
        CPPUNIT_ASSERT_EQUAL((std::uint8_t *)nullptr, ptr->operator*());

        delete ptr;

        ptr = new yapet::SecureArray(0);

        CPPUNIT_ASSERT_EQUAL(0u, ptr->size());
        CPPUNIT_ASSERT_EQUAL((std::uint8_t *)nullptr, ptr->operator*());

        delete ptr;
    }

    void testZeroOutBuffer() {
        auto subject = new yapet::SecureArray{3};

        for (auto i = 0; i < 3; i++) {
            subject->operator*()[i] = 42;
        }

        std::uint8_t *array = **subject;
        delete subject;
        for (auto i = 0; i < 3; i++) {
            CPPUNIT_ASSERT_EQUAL((std::uint8_t)0, array[i]);
        }
    }

    void testCopy() {
        yapet::SecureArray a = yapet::SecureArray(1);
        **a = 42;

        yapet::SecureArray b = yapet::SecureArray{a};

        CPPUNIT_ASSERT_EQUAL((std::uint8_t)42, **a);
        CPPUNIT_ASSERT_EQUAL(**a, **b);

        CPPUNIT_ASSERT(*a != *b);
    }

    void testAssignment() {
        yapet::SecureArray a = yapet::SecureArray{1};
        **a = 43;

        yapet::SecureArray b = yapet::SecureArray{1};
        **b = 42;

        a = b;

        CPPUNIT_ASSERT_EQUAL((std::uint8_t)42, **a);
        CPPUNIT_ASSERT(*a != *b);
    }

    void testMove() {
        yapet::SecureArray a = yapet::SecureArray{1};
        **a = 42;

        yapet::SecureArray b{std::move(a)};

        CPPUNIT_ASSERT(nullptr == *a);
        CPPUNIT_ASSERT_EQUAL((std::uint8_t)42, **b);

        CPPUNIT_ASSERT(a.size() == 0);
        CPPUNIT_ASSERT(b.size() == 1);
    }

    void testMoveAssignment() {
        yapet::SecureArray a = yapet::SecureArray{1};
        **a = 42;
        std::uint8_t *ptrAArray = *a;

        yapet::SecureArray b = yapet::SecureArray{1};
        **b = 43;
        std::uint8_t *ptrBArray = *b;

        b = std::move(a);

        CPPUNIT_ASSERT(nullptr == *a);
        CPPUNIT_ASSERT(a.size() == 0);
        CPPUNIT_ASSERT(b.size() == 1);

        CPPUNIT_ASSERT_EQUAL((std::uint8_t)42, **b);
        CPPUNIT_ASSERT_EQUAL(ptrAArray, ptrBArray);
        CPPUNIT_ASSERT_EQUAL((std::uint8_t)0, *ptrAArray);
    }

    void testEquality() {
        yapet::SecureArray a = yapet::SecureArray{1};
        **a = 42;
        CPPUNIT_ASSERT(a == a);

        yapet::SecureArray b = yapet::SecureArray{1};
        **b = 42;

        CPPUNIT_ASSERT(a == b);
        CPPUNIT_ASSERT(!(a != b));

        b = yapet::SecureArray{1};
        **b = 43;

        CPPUNIT_ASSERT(!(a == b));
        CPPUNIT_ASSERT(a != b);

        yapet::SecureArray empty;
        CPPUNIT_ASSERT(empty == empty);
        CPPUNIT_ASSERT(!(a == empty));
        CPPUNIT_ASSERT(a != empty);

        yapet::SecureArray empty2;
        CPPUNIT_ASSERT(empty == empty2);
    }

    void testIndexOperator() {
        yapet::SecureArray secureArray(3);

        CPPUNIT_ASSERT_THROW(secureArray[-1], std::out_of_range);
        CPPUNIT_ASSERT_THROW(secureArray[4], std::out_of_range);

        for (auto i = 0; i < 3; i++) {
            (*secureArray)[i] = i;
            CPPUNIT_ASSERT_EQUAL((std::uint8_t)i, secureArray[i]);
        }
    }

    void testEmptyAssignment() {
        yapet::SecureArray empty;
        yapet::SecureArray nonEmpty{1};
        nonEmpty = empty;
        CPPUNIT_ASSERT_EQUAL(0u, nonEmpty.size());
        CPPUNIT_ASSERT_EQUAL((std::uint8_t *)nullptr, *nonEmpty);

        yapet::SecureArray nonEmpty2{2};
        (*nonEmpty2)[0] = 'A';
        (*nonEmpty2)[1] = 'B';
        empty = nonEmpty2;
        CPPUNIT_ASSERT_EQUAL(2u, empty.size());
        CPPUNIT_ASSERT((*empty)[0] == (std::uint8_t)'A');
        CPPUNIT_ASSERT((*empty)[1] == (std::uint8_t)'B');
    }

    void testEmptyCopy() {
        yapet::SecureArray empty;
        yapet::SecureArray empty2{empty};
        CPPUNIT_ASSERT_EQUAL(0u, empty2.size());
        CPPUNIT_ASSERT_EQUAL((std::uint8_t *)nullptr, *empty2);

        yapet::SecureArray nonEmpty{2};
        (*nonEmpty)[0] = 'A';
        (*nonEmpty)[1] = 'B';
        yapet::SecureArray empty3{nonEmpty};
        CPPUNIT_ASSERT_EQUAL(2u, empty3.size());
        CPPUNIT_ASSERT((*empty3)[0] == (std::uint8_t)'A');
        CPPUNIT_ASSERT((*empty3)[1] == (std::uint8_t)'B');
    }

    void testEmptyMove() {
        yapet::SecureArray empty;
        yapet::SecureArray empty2{std::move(empty)};
        CPPUNIT_ASSERT_EQUAL(0u, empty2.size());
        CPPUNIT_ASSERT_EQUAL((std::uint8_t *)nullptr, *empty2);

        yapet::SecureArray nonEmpty{2};
        (*nonEmpty)[0] = 'A';
        (*nonEmpty)[1] = 'B';
        yapet::SecureArray empty3{std::move(nonEmpty)};
        CPPUNIT_ASSERT_EQUAL(2u, empty3.size());
        CPPUNIT_ASSERT((*empty3)[0] == (std::uint8_t)'A');
        CPPUNIT_ASSERT((*empty3)[1] == (std::uint8_t)'B');
    }

    void testAdd() {
        yapet::SecureArray a{1};
        **a = 'A';

        yapet::SecureArray b{1};
        **b = 'B';

        yapet::SecureArray result = a + b;
        CPPUNIT_ASSERT(result.size() == 2);
        CPPUNIT_ASSERT(**result == 'A');
        CPPUNIT_ASSERT((*result)[1] == 'B');
    }

    void testToSecureArray() {
        yapet::SecureArray a = yapet::toSecureArray("ABCDE");
        yapet::SecureArray b = yapet::toSecureArray(std::string{"ABCDE"});

        CPPUNIT_ASSERT(a == b);
    }

    void testToSecureArrayEmpty() {
        yapet::SecureArray a = yapet::toSecureArray("");
        CPPUNIT_ASSERT(a == yapet::SecureArray{});

        yapet::SecureArray b = yapet::toSecureArray(std::string{""});
        CPPUNIT_ASSERT(b == yapet::SecureArray{});
    }

    void testCopyOperator() {
        yapet::SecureArray empty{};
        yapet::SecureArray a{1};
        **a = 'A';

        empty << a;
        CPPUNIT_ASSERT(empty.size() == 0);
        CPPUNIT_ASSERT(*empty == nullptr);
        CPPUNIT_ASSERT(a.size() == 1);
        CPPUNIT_ASSERT(**a == 'A');

        a << empty;
        CPPUNIT_ASSERT(a.size() == 1);
        CPPUNIT_ASSERT(**a == 'A');
        CPPUNIT_ASSERT(empty.size() == 0);
        CPPUNIT_ASSERT(*empty == nullptr);

        yapet::SecureArray bc{2};
        (*bc)[0] = 'B';
        (*bc)[1] = 'C';

        a << bc;
        CPPUNIT_ASSERT(a.size() == 1);
        CPPUNIT_ASSERT(**a == 'B');
        CPPUNIT_ASSERT(bc.size() == 2);
        CPPUNIT_ASSERT((*bc)[0] == 'B' && (*bc)[1] == 'C');
        CPPUNIT_ASSERT(*a != *bc);

        yapet::SecureArray d{1};
        **d = 'D';

        bc << d;
        CPPUNIT_ASSERT(bc.size() == 1);
        CPPUNIT_ASSERT(**bc == 'D');
        CPPUNIT_ASSERT(d.size() == 1);
        CPPUNIT_ASSERT(**d == 'D');
        CPPUNIT_ASSERT(*bc != *d);
    }
};

int main() {
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(SecureArrayTest::suite());
    return runner.run() ? 0 : 1;
}
