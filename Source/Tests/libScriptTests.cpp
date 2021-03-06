////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  /!\ libMirror Tests /!\
//
//  These are the test cases for libMirror, the reflection utility library.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libCore/libCore.h>
#include <libCore/RefPtr.h>
#include <libHarnessed/libHarnessed.h>

#include <libMirror/libMirror.h>
#include <libMirror/Object.h>

using namespace Firestorm;

class TestBasicObject : public Mirror::Object
{
	FIRE_MIRROR_DECLARE(TestBasicObject, Mirror::Object);
public:
	TestBasicObject() {}
	virtual ~TestBasicObject()
	{
		std::cout << "TestBasicObject::~TestBasicObject" << std::endl;
	}
private:
};

class TestObjectWithFields : public Mirror::Object
{
	FIRE_MIRROR_DECLARE(TestObjectWithFields, Mirror::Object);
public:
	TestObjectWithFields() {}
	virtual ~TestObjectWithFields() 
	{
		std::cout << "TestObjectWithFields::~TestObjectWithFields" << std::endl;
	}

	string testString;
private:
};

RefPtr<TestHarness> libScriptPrepareHarness(int ac, char** av)
{
	RefPtr<TestHarness> h(new TestHarness("Test libMirror"));

	h->It("should allow instantiation of TestBasicObject", [](TestCase& t) {
		try
		{
			std::shared_ptr<TestBasicObject> test(new TestBasicObject);
		}
		catch(...)
		{
			t.Assert(false, "Allocation error or some other kind of error detected.");
		}
	});

	// h->It("TestBasicObject::MyType() should return the same type as instance->GetType()", [](TestCase& t) {
	// 	Mirror::Type tboStaticType = TestBasicObject::MyType();
	// 
	// 	RefPtr<TestBasicObject> tbo(new TestBasicObject);
	// 	t.Assert(tbo->GetType() == tboStaticType, "The two types were not the same.");
	// });

	return h;
}