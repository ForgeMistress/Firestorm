#include <libCore/libCore.h>
#include <libCore/RefPtr.h>
#include <libCore/WeakPtr.h>
#include <libHarnessed/libHarnessed.h>

#include <libExistence/libExistence.h>
#include <libExistence/Engine.h>
#include <libExistence/System.h>
#include <libExistence/Component.h>
#include <libExistence/Entity.h>

#include <libCore/Logger.h>

using namespace Firestorm;


struct TestComponent : public IComponent<uint8_t, uint8_t, uint8_t, uint8_t>
{
};

struct TestComponentMgr : public BasicComponentManager<TestComponent>
{
	TestComponentMgr()
	{
		Alloc(5);
	}
	void SetFlag1(const Entity& ent, uint8_t flag)
	{
		SetData<uint8_t>(ent.Index(), 0, flag);
	}

	uint8_t GetFlag1(const Entity& ent)
	{
		return GetData<uint8_t>(ent.Index(), 0);
	}
};

struct TestComponentView
{
	uint8_t v1;
	uint8_t v2;
	uint8_t v3;
	uint8_t v4;
};

struct TestComplexComponent : public IComponent<String, float, double>
{
};

struct TestComplexComponentView
{
	String name;
	float value1;
	float value2;
};

struct TestComplexComponentMgr : public BasicComponentManager<TestComplexComponent>
{
	TestComplexComponentMgr()
	{
		Alloc(5);
	}

	void SetName(const Entity& entity, const String& name)
	{
		SetData(entity.Index(), 0, name);
	}

	String GetName(const Entity& entity)
	{
		return GetData<String>(entity.Index(), 0);
	}
};

RefPtr<TestHarness> libExistencePrepareHarness(int ac, char** av)
{
	RefPtr<TestHarness> h(new TestHarness("libExistence"));

	h->It("Entities should be instantiable and contain proper ids", [](Firestorm::TestCase& t) {
		EntityMgr eMgr;

		Entity e = eMgr.Create();
		t.Assert(e.Index() == 0, "index was incorrect");
		t.Assert(e.Generation() == 0, "generation was incorrect");
	});

	h->It("components should report back their proper size", [](Firestorm::TestCase& t) {

		static size_t expectedSize = 4;

		size_t componentSize = TestComponent::SizeOf();

		FIRE_LOG_DEBUG("TestComponent (IComponent<uint8_t, uint8_t, uint8_t, uint8_t>) reported: %d", componentSize);

		t.Assert(expectedSize == componentSize, "the component reported back the incorrect size");
	});

	h->It("component field offsets should be proper", [](TestCase& t) {
		auto& memberDetailList = TestComponent::MyMembers();

		const size_t expectedMemberSize = sizeof(uint8_t);

		size_t expectedOffset = 0;
		for(size_t i=0; i<memberDetailList.size(); ++i)
		{
			auto& memberInfo = memberDetailList[i];
			FIRE_LOG_DEBUG("[%d] size = %d, offset = %d", i, memberInfo.Size, memberInfo.OffsetTo);
			t.Assert(memberInfo.Size == expectedMemberSize, 
				Format("size of member [%d] was wrong. expected %d. got %d", i, expectedMemberSize, memberInfo.Size));
			t.Assert(memberInfo.OffsetTo == expectedOffset,
				Format("offset of member [%d] was wrong. expected %d. got %d.", i, expectedOffset, memberInfo.OffsetTo));
			expectedOffset += memberInfo.Size;
		}
	});

	h->It("complex components should have the expected offsets as well", [](TestCase& t) {
		auto& memberDetailList = TestComplexComponent::MyMembers();
		size_t expectedOffset = 0;
		for(size_t i=0; i<memberDetailList.size(); ++i)
		{
			auto& memberInfo = memberDetailList[i];
			FIRE_LOG_DEBUG("[%d] size = %d, offset = %d", i, memberInfo.Size, memberInfo.OffsetTo);
			t.Assert(memberInfo.OffsetTo == expectedOffset,
				Format("offset of member [%d] was wrong. expected %d. got %d.", i, expectedOffset, memberInfo.OffsetTo));
			expectedOffset += memberInfo.Size;
		}
	});

	h->It("the component manager should allocate space properly", [](TestCase& t) {
		TestComponentMgr test;
		t.Assert(test.GetCapacity() == 5,
			Format(
			"test mgr capacity was an unexpected value. expected %d. got %d.",
			5,
			test.GetCapacity()));
	});

	h->It("components managers should manage their data buffers properly", [](TestCase& t) {

		// ensure that the capacity of the data buffer is enlarged when it is expected to be.
		// data buffer should expand when Count == Capacity and someone attempts to add a new instance.
		// in this instance, the capacity of the internal data buffer should grow by a factor of 2.
		{
			TestComponentMgr testMgr;
			EntityMgr eMgr;

			Entity e = eMgr.Create();
			size_t capacityBefore = testMgr.GetCapacity();

			Vector<Component> handles(testMgr.AddNewInstances(5));

			t.Assert(capacityBefore == testMgr.GetCapacity(),
				Format("the testMgr's capacity was modified when it shouldn't have been. before = %d. after = %d",
				capacityBefore, testMgr.GetCapacity()));

			handles.push_back(testMgr.AddNewInstance());
			t.Assert(testMgr.GetCapacity() == (capacityBefore * 2),
				Format("the testMgr's capacity was not modified properly. expected %d. got %d.",
				(capacityBefore * 2), testMgr.GetCapacity()));
		}


		{

		}
	});

	h->It("component managers should be able to access the fields of components properly", [](TestCase& t) {
		// test of a simple 4 byte component.
		{
			TestComponentMgr testMgr;
			EntityMgr eMgr;

			Entity e = eMgr.Create();
			Component c = testMgr.AddNewInstance();
			testMgr.SetFlag1(e, 20);
			uint8_t flag1 = testMgr.GetFlag1(e);
			t.Assert(flag1 == 20, "the flag value was not set properly");
		}

		// test of a more complicated component
		{
			TestComplexComponentMgr testMgr;
			EntityMgr eMgr;
			Entity e = eMgr.Create();
			Component c = testMgr.AddNewInstance();

			String testName = "Test Name";

			testMgr.SetName(e, testName);
			String retrieveTestName = testMgr.GetName(e);
			FIRE_LOG_DEBUG("retrieved the test name '%s'", retrieveTestName);
			t.Assert(testName == retrieveTestName,
				Format("the retrieved test name was wrong. expected %s. got %s", testName, retrieveTestName));
		}
	});


	return h;
}