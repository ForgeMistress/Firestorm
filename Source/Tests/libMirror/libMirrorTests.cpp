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
#include <libHarnessed/libHarnessed.h>

#include <libMirror/libMirror.h>
#include <libMirror/Object.h>
#include <libMirror/SerialTraits.h>

using namespace Elf;

class TestBasicObject : public Mirror::Object
{
	MIRROR_DECLARE(TestBasicObject, Mirror::Object);
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
	MIRROR_DECLARE(TestObjectWithFields, Mirror::Object);
public:
	TestObjectWithFields() {}
	explicit TestObjectWithFields(const std::string& str) : testString(str) {}
	virtual ~TestObjectWithFields() 
	{
		std::cout << "TestObjectWithFields::~TestObjectWithFields" << std::endl;
	}

	std::string testString;
private:
};

RTTR_REGISTRATION
{
	Mirror::Registration::class_<TestBasicObject>("TestBasicObject");

	Mirror::Registration::class_<TestObjectWithFields>("TestObjectWithFields")
		.property("testString", &TestObjectWithFields::testString)
	;
}

/*template<class T1, class T2>
void TestTypeUtils(T1 obj1, T2 obj2, TestCase& t)
{
	Mirror::Type t1 = Mirror::TypeUtils::get(obj1);
	Mirror::Type t2 = Mirror::TypeUtils::get(obj2);

	t.Assert(t1 == t2, "The types do not match.");
}*/

int main()
{
	Elf::TestHarness h("Test libMirror");

	h.It("should allow instantiation of TestBasicObject", [](Elf::TestCase& t) {
		try
		{
			std::shared_ptr<TestBasicObject> test(new TestBasicObject);
		}
		catch(...)
		{
			t.Assert(false, "Allocation error or some other kind of error detected.");
		}
	});

	h.It("TestBasicObject::MyType() should return the same type as instance->GetType()", [](Elf::TestCase& t) {
		Elf::Mirror::Type tboStaticType = TestBasicObject::MyType();

		std::shared_ptr<TestBasicObject> tbo(new TestBasicObject);
		t.Assert(tbo->GetType() == tboStaticType, "The two types were not the same.");
	});

	h.It("Mirror::TypeUtils::get should return the same type no matter what qualifiers are passed in", [](Elf::TestCase& t) {
		Elf::SharedPtr<Mirror::Object> obj = std::make_shared<Mirror::Object>();
		t.Assert(obj.get(), "No object was constructed.");

		/*TestTypeUtils<const Mirror::Object*,            Mirror::Object*>       (obj.get(), obj.get(), t);
		TestTypeUtils<Mirror::Object*,                  const Mirror::Object*> (obj.get(), obj.get(), t);
		TestTypeUtils<shared_ptr<Mirror::Object>,       Mirror::Object*>       (obj,       obj.get(), t);
		TestTypeUtils<const shared_ptr<Mirror::Object>, const Mirror::Object*> (obj,       obj.get(), t);*/
	});

	h.It("the properties in an object instance should be retrievable when passing in a shared_ptr", [](Elf::TestCase& t) {
		using namespace rttr;

		std::string testStringValue("testing value!");
		std::shared_ptr<Elf::Mirror::Object> test = std::make_shared<TestObjectWithFields>(testStringValue);
		// test->testString = testStringValue;

		instance objInstance(test);
		Elf::Mirror::Type testType = test->GetType();

		property testStringProperty = testType.get_property("testString");
		variant extractedValueVariant = testStringProperty.get_value(objInstance);

		t.Assert(extractedValueVariant.get_value<std::string>() == testStringValue, "testStringValue != extractedValueVariant");
	});

	uint32_t results = h.Run();

	std::cout << "Completed with " << results << " errors." << std::endl << std::endl;
	std::cout << "Press 'Enter' to close..." << std::endl;
	std::cin.get();

    return results;
}