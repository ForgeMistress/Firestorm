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

#include <libIO/libIO.h>
#include <libIO/IDocument.h>
#include <libIO/TypeTraits.h>

#define FAIL_END(T, FUNC_CALL) \
{\
	Elf::Result<void, Elf::Error> err = document -> FUNC_CALL ; \
	T.Assert(err.has_value(), "error calling function "#FUNC_CALL": "+err.error().Message); \
}


class TestBasicObject : public Elf::Mirror::Object
{
	ELF_MIRROR_DECLARE(TestBasicObject, Elf::Mirror::Object);
public:
	TestBasicObject() {}
	virtual ~TestBasicObject()
	{
		std::cout << "TestBasicObject::~TestBasicObject" << std::endl;
	}
private:
};

RTTR_REGISTRATION
{
	Elf::Mirror::Registration::class_<TestBasicObject>("TestBasicObject");
}

int main()
{
	Elf::TestHarness h("Test libIOTests");
	//TestBasicObject object;
	Elf::Mirror::Object obj;

	h.It("documents should be properly constructed when the Document::Make* functions are called", [](Elf::TestCase& t) {
		Elf::SharedPtr<Elf::IDocument> document = Elf::Document::MakeBlank("Elf::Document::JSON");
		t.Assert(document != nullptr, "No JSON document was created.");

		Elf::Vector<char> data = {'{', '}'};
		Elf::SharedPtr<Elf::IDocument> document = Elf::Document::MakeFromData("Elf::Document::JSON", data);
		t.Assert(document != nullptr, "No JSON document was created.");
	});

	h.It("documents should write out to a string as expected", [](Elf::TestCase& t) {
		/*Json::Value value;
		Elf::IDocument* document = new Elf::JSONDocument(value);
		document->WriteSubsection("subsection");
		Elf::Vector<char> data;
		document->WriteToData(data);
		Elf::String str(data.begin(), data.end());
		Elf::String valid("{\"subsection\":{}}\n");
		t.Assert(str == valid, "document output was not what was expected: '" + valid + "' got : '"+str+"'");
		delete document;*/
	});


	h.It("complex documents should write out to a string as expected", [](Elf::TestCase& t) {

		/*Json::Value value;
		value[Elf::TraitsDocs::DATA_KEY] = Json::Value(Json::objectValue);

		Elf::IDocument* document = new Elf::JSONDocument(value);
		document->FindSubsection(Elf::TraitsDocs::DATA_KEY);
		document->EnterSubsection();

		rttr::type ty = rttr::type::get_by_name("Elf::Mirror::Object");
		std::cout << std::endl << "++++++" << ty.get_name().to_string() << std::endl;
		std::cout.flush();
		t.Assert(ty == Elf::Mirror::Object::MyType(), "the types do not match");

		FAIL_END(t, WriteSubsection("object_1"));
		FAIL_END(t, EnterSubsection());

			FAIL_END(t, WriteString(Elf::TraitsDocs::OBJECT_TYPE_KEY, Elf::Mirror::Object::MyType().get_name().to_string()));
			FAIL_END(t, WriteSubsection(Elf::TraitsDocs::OBJECT_DATA_KEY));
			FAIL_END(t, EnterSubsection());

			FAIL_END(t, WriteString("firstPart", "Hello there!"));
			FAIL_END(t, WriteString("secondPart", "Lovely weather, right?"));

			FAIL_END(t, LeaveSubsection());
		FAIL_END(t, LeaveSubsection());

		std::cout << std::endl << document->ToString() << std::endl;
		delete document;*/
	});

	uint32_t results = h.Run();

	std::cout << "Completed with " << results << " errors." << std::endl << std::endl;
	std::cout << "Press 'Enter' to close..." << std::endl;
	std::cin.get();

    return results;
}