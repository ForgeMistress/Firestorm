
#include <libCore/libCore.h>
#include <libMirror/libMirror.h>
#include <libExistence/libExistence.h>
#include <libHarnessed/libHarnessed.h>

#include <libExistence/ObjectPool.h>
#include <libMirror/Object.h>

using std::cout;
using std::cin;
using std::endl;

class PooledObj
{
public:
	PooledObj() {
		i = 0;
		j = 1;
		k = 2;
	}

	void Delete() {}
	void Recycle() {}
	void Init() {}

	char i;
	char j;
	char k;
};

#define HANDLE_ERROR( RES, TAG ) \
	if(!RES.has_value()) \
		t.Assert(RES.has_value(), "operation tagged '" TAG "'returned an unexpected error : "+RES.error().Message)

int main(int argc, char** argv)
{
	Elf::TestHarness h("libExistence Tests");

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Begin ObjectPool Testing
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	h.It("object pools should instantiate with the correct number of objects", [](Elf::TestCase& t) {
		Elf::ObjectPool<PooledObj, 500> objPool;
		t.Assert(objPool.GetObjectBlockSize() == 1500, "object block size was incorrect");
		t.Assert(objPool.GetFlagBlockSize() == 500, "flag block size was incorrect");
	});

	h.It("object pools should return valid pointers when Acquire is called", [](Elf::TestCase& t) {
		Elf::ObjectPool<PooledObj, 500> objPool;
		Elf::Result<Elf::SharedPtr<PooledObj>,Elf::Error> obj = objPool.Acquire();
		HANDLE_ERROR(obj, "objPool.Acquire");
		t.Assert(obj.value().get() != nullptr, "could not acquire object from pool");
	});

	h.It("objects acquired from the pool should be marked as 'in use' when they're active", [](Elf::TestCase& t) {
		Elf::ObjectPool<PooledObj, 10> objPool;
		{
			Elf::Result<Elf::SharedPtr<PooledObj>,Elf::Error> obj = objPool.Acquire();
			HANDLE_ERROR(obj, "objPool.Acquire");

			Elf::Result<bool, Elf::Error> result = objPool.IsInUse(obj.value().get());
			HANDLE_ERROR(result, "objPool.IsInUse");

			if(result.has_value())
			{
				t.Assert(result.value(), "object is not marked as being in use when it should be");
			}
		}
	});

	h.It("object pools should return invalid pointers when Acquire is called and there are no more objects", 
		[](Elf::TestCase& t) {
		Elf::ObjectPool<PooledObj, 500> objPool;
		Elf::SharedPtr<PooledObj> objs[500];
		for(int i = 0; i < 500; i++)
		{
			Elf::Result<Elf::SharedPtr<PooledObj>, Elf::Error> obj = objPool.Acquire();
			t.Assert(obj.has_value(), "objPool returned an error when it shouldn't have");
			objs[i] = obj.value();
		}
		Elf::Result<Elf::SharedPtr<PooledObj>, Elf::Error> obj = objPool.Acquire();
		t.Assert(!obj.has_value(), "object pool should have returned an error. it didn't.");
	});
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// End ObjectPool Testing
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	uint32_t errorCount = h.Run();
	cout << "Tests Completed with [[" << errorCount << "]] errors." << endl;

	cout << "Press any key to close..." << endl;
	cin.get();

	return errorCount;
}