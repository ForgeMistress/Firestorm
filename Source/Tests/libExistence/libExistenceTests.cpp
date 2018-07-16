
#include <libCore/libCore.h>
#include <libMirror/libMirror.h>
#include <libExistence/libExistence.h>
#include <libHarnessed/libHarnessed.h>

#include <libExistence/ObjectPool.h>
#include <libExistence/Engine.h>
#include <libExistence/System.h>
#include <libExistence/Component.h>
#include <libExistence/Entity.h>

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





class TestComponent_1 : public Elf::Component
{
	ELF_MIRROR_DECLARE(TestComponent_1, Elf::Component);
public:

};


class TestSystem_1 : public Elf::System
{
	ELF_MIRROR_DECLARE(TestSystem_1, Elf::System);
public:
private:
	virtual void OnModified(double deltaT);
	virtual void OnEntityAdded(const Elf::WeakPtr<Elf::Entity>& entity);
	virtual void OnEntityRemoved(const Elf::WeakPtr<Elf::Entity>& entity);
	virtual void OnBeforeAddToEngine();
	virtual void OnAddToEngine();
	virtual void OnRemoveFromEngine();
	virtual void OnUpdate(double deltaT, const Elf::Vector< Elf::WeakPtr<Elf::Entity> >& entities);
	virtual bool OnEntityFilter(const Elf::WeakPtr<Elf::Entity>& entity) const;
};

void TestSystem_1::OnModified(double deltaT)
{
}

void TestSystem_1::OnEntityAdded(const Elf::WeakPtr<Elf::Entity>& entity)
{
}

void TestSystem_1::OnEntityRemoved(const Elf::WeakPtr<Elf::Entity>& entity)
{
}

void TestSystem_1::OnBeforeAddToEngine()
{
}

void TestSystem_1::OnAddToEngine()
{
}

void TestSystem_1::OnRemoveFromEngine()
{
}

void TestSystem_1::OnUpdate(double deltaT, const Elf::Vector< Elf::WeakPtr<Elf::Entity> >& entities)
{
}

bool TestSystem_1::OnEntityFilter(const Elf::WeakPtr<Elf::Entity>& entity) const
{
	return entity->Inspect<TestComponent_1>() != nullptr;
}

ELF_MIRROR_REGISTRATION
{
	ELF_MIRROR_DEFINE(TestSystem_1)
	;

	ELF_MIRROR_DEFINE(TestComponent_1)
	;
}



int main(int argc, char** argv)
{
	Elf::TestHarness h("libExistence Tests");

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Begin ObjectPool Testing
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	h.It("object pools should instantiate with the correct number of objects", [](Elf::TestCase& t) {
		Elf::ObjectPool<PooledObj> objPool(500);
		t.Assert(objPool.GetObjectBlockSize() == 1500, "object block size was incorrect");
		t.Assert(objPool.GetFlagBlockSize() == 500, "flag block size was incorrect");
	});

	h.It("object pools should return valid pointers when Acquire is called", [](Elf::TestCase& t) {
		Elf::ObjectPool<PooledObj> objPool(500);
		Elf::Result<Elf::SharedPtr<PooledObj>,Elf::Error> obj = objPool.Acquire();
		HANDLE_ERROR(obj, "objPool.Acquire");
		t.Assert(obj.value().get() != nullptr, "could not acquire object from pool");
	});

	h.It("objects acquired from the pool should be marked as 'in use' when they're active", [](Elf::TestCase& t) {
		Elf::ObjectPool<PooledObj> objPool(10);
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
		Elf::ObjectPool<PooledObj> objPool(500);
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


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Begin ECS Testing
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	h.It("[ECS] ", [](Elf::TestCase& t) {
	});
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// End ECS Testing
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	uint32_t errorCount = h.Run();
	cout << "Tests Completed with [[" << errorCount << "]] errors." << endl;

	cout << "Press any key to close..." << endl;
	cin.get();

	return errorCount;
}