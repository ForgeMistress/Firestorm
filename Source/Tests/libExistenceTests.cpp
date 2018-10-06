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

#include <iomanip>

using namespace Firestorm;


/*struct TestComponent : public IComponent<uint8_t, uint8_t, uint8_t, uint8_t>
{
};

struct TestComponentMgr : public BasicComponentManager<TestComponent>
{
	TestComponentMgr(size_t count=5)
	{
		Alloc(count);
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




struct TestComponentMgr_Alt : public ComponentManagerBase_MemoryManagement
{
	TestComponentMgr_Alt(size_t count=5)
		: ComponentManagerBase_MemoryManagement({
			MemberData<uint8_t>(),
			MemberData<uint8_t>(),
			MemberData<uint8_t>(),
			MemberData<uint8_t>()
		})
	{
		Alloc(count);
	}
	void SetFlag1(const Entity& ent, uint8_t flag)
	{
		Set(0, ent.Index(), flag);
	}

	uint8_t GetFlag1(const Entity& ent)
	{
		return Get<uint8_t>(0, ent.Index());
	}

	void SetFlag2(const Entity& ent, uint8_t flag)
	{
		Set(1, ent.Index(), flag);
	}

	uint8_t GetFlag2(const Entity& ent)
	{
		return Get<uint8_t>(ent.Index(), 1);
	}
};



struct TestComplexComponent : public IComponent<String, float, double>
{
};

struct TestComplexComponentMgr : public BasicComponentManager<TestComplexComponent>
{
	TestComplexComponentMgr(size_t allocs = 5)
	{
		Alloc(allocs);
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

struct TestComplexComponentMgr_Alt : public ComponentManagerBase_MemoryManagement
{
	TestComplexComponentMgr_Alt(size_t allocs = 5)
		: ComponentManagerBase_MemoryManagement({
			MemberData<String>(),
			MemberData<float>(),
			MemberData<double>()
		})
	{
		Alloc(allocs);
	}

	void SetName(const Entity& entity, const String& name)
	{
		Set<String>(0, entity.Index(), name);
	}

	String GetName(const Entity& entity)
	{
		return Get<String>(0, entity.Index());
	}
};*/

class TupleBasedSimpleComponentMgr : public ComponentDefinition<uint8_t, uint8_t, uint8_t>
{
public:
	TupleBasedSimpleComponentMgr(size_t allocs = 5)
	{
		Alloc(allocs);
	}

	void MakeInstance()
	{
		def_type::New<0, uint8_t>();
		def_type::New<1, uint8_t>();
		def_type::New<2, uint8_t>();
		AddSize();
	}

	void SetFlag1(const Entity& entity, uint8_t value)
	{
		Set<0, uint8_t>(entity.Index(), std::move(value));
		//Set<0, uint8_t>(entity.Index(), value);
	}

	void SetFlag2(const Entity& entity, uint8_t value)
	{
		Set<1, uint8_t>(entity.Index(), std::move(value));
	}

	const uint8_t& GetFlag1(const Entity& entity) const
	{
		return Get<0, uint8_t>(entity.Index());
	}

	const uint8_t& GetFlag2(const Entity& entity) const
	{
		return Get<1, uint8_t>(entity.Index());
	}
};



class TupleBasedComplexComponentMgr : public ComponentDefinition<String, double, float>
{
public:
	TupleBasedComplexComponentMgr(size_t allocs = 5)
	{
		Alloc(allocs);
		
	}

	virtual ~TupleBasedComplexComponentMgr()
	{
	}

	void MakeInstance()
	{
		def_type::New<0, String>();
		def_type::New<1, double>();
		def_type::New<2, float>();
		AddSize();
	}

	void SetName(const Entity& entity, String& value)
	{
		Set<0, String>(entity.Index(), std::move(value));
	}

	const String& GetName(const Entity& entity) const
	{
		return Get<0, String>(entity.Index());
	}

	void SetVelocity(const Entity& entity, double value)
	{
		Set<1, double>(entity.Index(), std::move(value));
	}

	const double& GetVelocity(const Entity& entity) const
	{
		return Get<1, double>(entity.Index());
	}
};




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// naieve OOP implementations
struct NaieveSimpleComponent
{
	uint8_t flag1;
	uint8_t flag2;
	uint8_t flag3;
};
struct NaieveSimpleComponentManager
{
	NaieveSimpleComponentManager(size_t allocations = 5)
	{
		components.reserve(allocations);
	}
	~NaieveSimpleComponentManager()
	{
		for(size_t i=0; i<components.size(); ++i)
		{
			delete components[i];
		}
	}

	void MakeInstance()
	{
		components.push_back(new NaieveSimpleComponent());
	}

	void SetFlag1(const Entity& e, const uint8_t& value)
	{
		components[e.Index()]->flag1 = value;
	}

	const uint8_t& GetFlag1(const Entity& e) const
	{
		return components[e.Index()]->flag1;
	}

	void SetFlag2(const Entity& e, const uint8_t& value)
	{
		components[e.Index()]->flag2 = value;
	}

	const uint8_t& GetFlag2(const Entity& e) const
	{
		return components[e.Index()]->flag2;
	}

	Vector<NaieveSimpleComponent*> components;
};


struct NaieveComplexComponent
{
	String name;
	double velocity;
	float forTheLulz;
};
struct NaieveComplexComponentManager
{
	NaieveComplexComponentManager(size_t allocations = 5)
	{
		components.reserve(allocations);
	}
	~NaieveComplexComponentManager()
	{
		for(size_t i=0; i<components.size(); ++i)
		{
			delete components[i];
		}
	}
	void MakeInstance()
	{
		components.push_back(new NaieveComplexComponent());
	}

	void SetName(const Entity& e, const String& value)
	{
		components[e.Index()]->name = value;
	}

	const String& GetName(const Entity& e) const
	{
		return components[e.Index()]->name;
	}

	void SetVelocity(const Entity& e, const double& value)
	{
		components[e.Index()]->velocity = value;
	}

	const double& GetVelocity(const Entity& e) const
	{
		return components[e.Index()]->velocity;
	}

	Vector<NaieveComplexComponent*> components;
};


static size_t numRuns = 5;
static size_t numObjects = 500000;

#define NUM_OBJECTS 500000
#define NUM_RUNS 50

RefPtr<TestHarness> libExistencePrepareHarness(int ac, char** av)
{
	static EntityMgr eMgr;

	FIRE_LOG_DEBUG("+++++ allocating entities");
	static Vector<Entity> ents;
	ents.reserve(NUM_OBJECTS);
	std::unordered_set<uint32_t> createdIndices;
	for(size_t i = 0; i < NUM_OBJECTS; ++i)
	{
		Entity e = eMgr.Create();
		FIRE_ASSERT(createdIndices.find(e.Index()) == createdIndices.end());
		createdIndices.insert(e.Index());
		ents.push_back(e);
	}

	RefPtr<TestHarness> h(new TestHarness("libExistence"));

	h->Profile(Format("Tuple/Vector Simple ComponentManager [%d Items]",NUM_OBJECTS), NUM_RUNS, [](Benchmark& bm) {

		Benchmark::SnapshotHandle* ssh = nullptr;
		{
		
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
			ssh = bm.StartSegment("manager constructor");
			TupleBasedSimpleComponentMgr testMgr(NUM_OBJECTS);
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("component additions");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr.MakeInstance();
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("setting flags 1");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr.SetFlag1(ents[i], 101);
			}
			bm.StopSegment(ssh);

			ssh = bm.StartSegment("setting flags 2");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr.SetFlag2(ents[i], 101);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("getting flags 1");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				FIRE_ASSERT(testMgr.GetFlag1(ents[i]) == 101);
			}
			bm.StopSegment(ssh);

			ssh = bm.StartSegment("getting flags 2");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				FIRE_ASSERT(testMgr.GetFlag2(ents[i]) == 101);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("destruction");
		}
		bm.StopSegment(ssh);
	});

	h->Profile(Format("Naieve Simple ComponentManager [%d Items]",NUM_OBJECTS), NUM_RUNS, [](Benchmark& bm) {

		Benchmark::SnapshotHandle* ssh = nullptr;
		{

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("manager constructor");
			NaieveSimpleComponentManager testMgr(NUM_OBJECTS);
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("component additions");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr.MakeInstance();
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("setting flags 1");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr.SetFlag1(ents[i], 101);
			}
			bm.StopSegment(ssh);

			ssh = bm.StartSegment("setting flags 2");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr.SetFlag2(ents[i], 101);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("getting flags 1");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				FIRE_ASSERT(testMgr.GetFlag1(ents[i]) == 101);
			}
			bm.StopSegment(ssh);

			ssh = bm.StartSegment("getting flags 2");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				FIRE_ASSERT(testMgr.GetFlag2(ents[i]) == 101);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("destruction");
		}
		bm.StopSegment(ssh);
	});


	h->Profile(Format("Tuple/Vector Complex ComponentManager [%d Items]",NUM_OBJECTS), NUM_RUNS, [](Benchmark& bm) {

		Benchmark::SnapshotHandle* ssh = nullptr;
		{

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("manager constructor");
			TupleBasedComplexComponentMgr testMgr(NUM_OBJECTS);
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("component additions");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr.MakeInstance();
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("setting names");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr.SetName(ents[i], String("Slim Shady"));
			}
			bm.StopSegment(ssh);

			ssh = bm.StartSegment("setting velocities");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr.SetVelocity(ents[i], 9001.0);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("getting names");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				FIRE_ASSERT(testMgr.GetName(ents[i]) == "Slim Shady");
			}
			bm.StopSegment(ssh);

			ssh = bm.StartSegment("getting velocities");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				FIRE_ASSERT(testMgr.GetVelocity(ents[i]) == 9001.0);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("destruction");
		}
		bm.StopSegment(ssh);
	});

	h->Profile(Format("Naieve Complex ComponentManager [%d Items]",NUM_OBJECTS), NUM_RUNS, [](Benchmark& bm) {

		Benchmark::SnapshotHandle* ssh = nullptr;
		{

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("manager constructor");
			NaieveComplexComponentManager testMgr(NUM_OBJECTS);
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("component additions");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr.MakeInstance();
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("setting names");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr.SetName(ents[i], "Slim Shady");
			}
			bm.StopSegment(ssh);

			ssh = bm.StartSegment("setting velocities");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr.SetVelocity(ents[i], 9001.0);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("getting names");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				FIRE_ASSERT(testMgr.GetName(ents[i]) == "Slim Shady");
			}
			bm.StopSegment(ssh);

			ssh = bm.StartSegment("getting velocities");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				FIRE_ASSERT(testMgr.GetVelocity(ents[i]) == 9001.0);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("destruction");
		}
		bm.StopSegment(ssh);
	});

	return h;
}

	/*h->It("benchmark[OOP Complex]", [](TestCase&) {
		Vector<double> construction;
		Vector<double> constructors;
		Vector<double> flagSets;
		Vector<double> flagGets;
		Vector<double> destructions;
		for(size_t i=0;i<numRuns;++i)
		{
			//FIRE_LOG_DEBUG("+++++ Run %d +++++", i);
			auto begin = high_resolution_clock::now();
			{
				TupleBasedComponentMgr testMgr(benchmarkAllocations);
				log_duration("manager construction", begin, construction);
				begin = high_resolution_clock::now();

				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d allocations performed...", i);
					testMgr.MakeInstance();
				}

				log_duration("component additions", begin, constructors);

				//FIRE_LOG_DEBUG("+++++ beginning %d flag sets", benchmarkAllocations);
				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					testMgr.SetName(ents[i], "Slim Shady");
				}
				log_duration("component name sets", begin, flagSets);

				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					String value(testMgr.GetName(ents[i]));
				}
				log_duration("component name gets", begin, flagGets);
				begin = high_resolution_clock::now();
			}
			log_duration("manager destruction", begin, destructions);
		}
		FIRE_LOG_DEBUG("+++++ Average over %d Runs +++++", numRuns);
		double constructiont = 0, compCons = 0,flagt = 0, flaggt=0, destructiont = 0;
		for(size_t i = 0; i < numRuns; ++i)
		{
			constructiont += construction[i];
			compCons += constructors[i];
			flagt += flagSets[i];
			flaggt += flagGets[i];
			destructiont += destructions[i];
		}
		FIRE_LOG_DEBUG("%f%f+++++ constructions     = %d", std::fixed, std::setprecision(15), constructiont / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ comp constructors = %d", std::fixed, std::setprecision(15), compCons / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ name sets         = %d", std::fixed, std::setprecision(15), flagt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ name gets         = %d", std::fixed, std::setprecision(15), flaggt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ destructions      = %d", std::fixed, std::setprecision(15), destructiont / numRuns);
	});
}*/

	/*h->It("Entities should be instantiable and contain proper ids", [](Firestorm::TestCase& t) {
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

	using namespace std::chrono;
	static size_t benchmarkAllocations = 100000;

	static auto log_duration = [](const String& tag, const time_point<high_resolution_clock>& begin, Vector<double>& times)
	{
		auto dur = duration<double, std::nano>(high_resolution_clock::now() - begin);
		auto cnt = duration_cast<nanoseconds>(dur).count();
		
		double c = (double)cnt / 1000000000.0;
		//FIRE_LOG_DEBUG("+++++%f[%s] -> Time = %d seconds", std::setprecision(10), tag, c);
		times.push_back(c);
	};

	static EntityMgr eMgr;

	FIRE_LOG_DEBUG("+++++ allocating entities");
	static Vector<Entity> ents;
	ents.reserve(benchmarkAllocations);
	std::unordered_set<uint32_t> createdIndices;
	for(size_t i = 0; i < benchmarkAllocations; ++i)
	{
		Entity e = eMgr.Create();
		FIRE_ASSERT(createdIndices.find(e.Index()) == createdIndices.end());
		createdIndices.insert(e.Index());
		ents.push_back(e);
	}

	static size_t numRuns = 50;

	h->It("benchmark[Single Buffer Allocator]", [](TestCase&) {
		Vector<double> construction;
		Vector<double> constructors;
		Vector<double> flagSets;
		Vector<double> flagGets;
		Vector<double> destructions;
		for(size_t i=0;i<numRuns;++i)
		{
			//FIRE_LOG_DEBUG("+++++ Run %d +++++", i);
			auto begin = high_resolution_clock::now();
			{
				TestComponentMgr testMgr(benchmarkAllocations);
				log_duration("manager construction", begin, construction);
				begin = high_resolution_clock::now();

				// not needed in the alternate version
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d allocations performed...", i);
					testMgr.AddNewInstance();
				}
				log_duration("component additions", begin, constructors);

				//FIRE_LOG_DEBUG("+++++ beginning %d flag sets", benchmarkAllocations);
				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					testMgr.SetFlag1(ents[i], 101);
				}
				log_duration("component flag sets", begin, flagSets);

				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					uint8_t value = testMgr.GetFlag1(ents[i]);
				}
				log_duration("component flag gets", begin, flagGets);

				begin = high_resolution_clock::now();
			}
			log_duration("manager destruction", begin, destructions);
		}

		FIRE_LOG_DEBUG("+++++ Average over %d Runs +++++", numRuns);
		double constructiont = 0, compCons = 0,flagt = 0, flaggt=0, destructiont = 0;
		for(size_t i = 0; i < numRuns; ++i)
		{
			constructiont += construction[i];
			compCons += constructors[i];
			flagt += flagSets[i];
			flaggt += flagGets[i];
			destructiont += destructions[i];
		}
		FIRE_LOG_DEBUG("%f%f+++++ constructions     = %d", std::fixed, std::setprecision(15), constructiont / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ comp constructors = %d", std::fixed, std::setprecision(15), compCons / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ flag sets         = %d", std::fixed, std::setprecision(15), flagt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ flag gets         = %d", std::fixed, std::setprecision(15), flaggt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ destructions      = %d", std::fixed, std::setprecision(15), destructiont / numRuns);
	});


	h->It("benchmark[Multi Buffer Allocator]", [](TestCase&) {
		Vector<double> construction;
		Vector<double> constructors;
		Vector<double> flagSets;
		Vector<double> flagGets;
		Vector<double> destructions;
		for(size_t i=0;i<numRuns;++i)
		{
			//FIRE_LOG_DEBUG("+++++ Run %d +++++", i);
			auto begin = high_resolution_clock::now();
			{
				TestComponentMgr_Alt testMgr(benchmarkAllocations);
				log_duration("manager construction", begin, construction);
				begin = high_resolution_clock::now();

				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d allocations performed...", i);
					testMgr.MakeInstance();
				}

				log_duration("component additions", begin, constructors);

				//FIRE_LOG_DEBUG("+++++ beginning %d flag sets", benchmarkAllocations);
				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					testMgr.SetFlag1(ents[i], 101);
				}
				log_duration("component flag sets", begin, flagSets);

				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					uint8_t value = testMgr.GetFlag1(ents[i]);
				}
				log_duration("component flag gets", begin, flagGets);
				begin = high_resolution_clock::now();
			}
			log_duration("manager destruction", begin, destructions);
		}
		FIRE_LOG_DEBUG("+++++ Average over %d Runs +++++", numRuns);
		double constructiont = 0, compCons = 0,flagt = 0, flaggt=0, destructiont = 0;
		for(size_t i = 0; i < numRuns; ++i)
		{
			constructiont += construction[i];
			compCons += constructors[i];
			flagt += flagSets[i];
			flaggt += flagGets[i];
			destructiont += destructions[i];
		}
		FIRE_LOG_DEBUG("%f%f+++++ constructions     = %d", std::fixed, std::setprecision(15), constructiont / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ comp constructors = %d", std::fixed, std::setprecision(15), compCons / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ flag sets         = %d", std::fixed, std::setprecision(15), flagt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ flag gets         = %d", std::fixed, std::setprecision(15), flaggt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ destructions      = %d", std::fixed, std::setprecision(15), destructiont / numRuns);
	});






	h->It("benchmark[Single Buffer Allocator Complex]", [](TestCase&) {
		Vector<double> construction;
		Vector<double> constructors;
		Vector<double> flagSets;
		Vector<double> flagGets;
		Vector<double> destructions;
		for(size_t i=0;i<numRuns;++i)
		{
			//FIRE_LOG_DEBUG("+++++ Run %d +++++", i);
			auto begin = high_resolution_clock::now();
			{
				TestComplexComponentMgr testMgr(benchmarkAllocations);
				log_duration("manager construction", begin, construction);
				begin = high_resolution_clock::now();

				// not needed in the alternate version
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d allocations performed...", i);
					testMgr.AddNewInstance();
				}

				log_duration("component additions", begin, constructors);

				//FIRE_LOG_DEBUG("+++++ beginning %d flag sets", benchmarkAllocations);
				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					testMgr.SetName(ents[i], "Slim Shady");
				}
				log_duration("component name sets", begin, flagSets);

				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					String value(testMgr.GetName(ents[i]));
				}
				log_duration("component name gets", begin, flagGets);
				begin = high_resolution_clock::now();
			}
			log_duration("manager destruction", begin, destructions);
		}
		FIRE_LOG_DEBUG("+++++ Average over %d Runs +++++", numRuns);
		double constructiont = 0, compCons = 0,flagt = 0, flaggt=0, destructiont = 0;
		for(size_t i = 0; i < numRuns; ++i)
		{
			constructiont += construction[i];
			compCons += constructors[i];
			flagt += flagSets[i];
			flaggt += flagGets[i];
			destructiont += destructions[i];
		}
		FIRE_LOG_DEBUG("%f%f+++++ constructions     = %d", std::fixed, std::setprecision(15), constructiont / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ comp constructors = %d", std::fixed, std::setprecision(15), compCons / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ name sets         = %d", std::fixed, std::setprecision(15), flagt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ name gets         = %d", std::fixed, std::setprecision(15), flaggt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ destructions      = %d", std::fixed, std::setprecision(15), destructiont / numRuns);
	});


	h->It("benchmark[Multi Buffer Allocator Complex]", [](TestCase&) {
		Vector<double> construction;
		Vector<double> constructors;
		Vector<double> flagSets;
		Vector<double> flagGets;
		Vector<double> destructions;
		for(size_t i=0;i<numRuns;++i)
		{
			//FIRE_LOG_DEBUG("+++++ Run %d +++++", i);
			auto begin = high_resolution_clock::now();
			{
				TestComplexComponentMgr_Alt testMgr(benchmarkAllocations);
				log_duration("manager construction", begin, construction);
				begin = high_resolution_clock::now();

				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d allocations performed...", i);
					testMgr.MakeInstance();
				}

				log_duration("component additions", begin, constructors);

				//FIRE_LOG_DEBUG("+++++ beginning %d flag sets", benchmarkAllocations);
				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					testMgr.SetName(ents[i], "Slim Shady");
				}
				log_duration("component name sets", begin, flagSets);

				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					String value(testMgr.GetName(ents[i]));
				}
				log_duration("component name gets", begin, flagGets);
				begin = high_resolution_clock::now();
			}
			log_duration("manager destruction", begin, destructions);
		}
		FIRE_LOG_DEBUG("+++++ Average over %d Runs +++++", numRuns);
		double constructiont = 0, compCons = 0,flagt = 0, flaggt=0, destructiont = 0;
		for(size_t i = 0; i < numRuns; ++i)
		{
			constructiont += construction[i];
			compCons += constructors[i];
			flagt += flagSets[i];
			flaggt += flagGets[i];
			destructiont += destructions[i];
		}
		FIRE_LOG_DEBUG("%f%f+++++ constructions     = %d", std::fixed, std::setprecision(15), constructiont / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ comp constructors = %d", std::fixed, std::setprecision(15), compCons / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ name sets         = %d", std::fixed, std::setprecision(15), flagt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ name gets         = %d", std::fixed, std::setprecision(15), flaggt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ destructions      = %d", std::fixed, std::setprecision(15), destructiont / numRuns);
	});

	struct ComplexComponent
	{
		String name;
		float value1;
		double value2;
	};
	struct ComponentManager
	{

		ComponentManager(size_t allocations = 5)
		{
			components.reserve(allocations);
		}
		void MakeInstance()
		{
			components.push_back(ComplexComponent());
		}

		void SetName(const Entity& e, const String& name)
		{
			components[e.Index()].name = name;
		}

		const String& GetName(const Entity& e) const
		{
			return components[e.Index()].name;
		}

		Vector<ComplexComponent> components;
	};

	h->It("benchmark[OOP Complex]", [](TestCase&) {
		Vector<double> construction;
		Vector<double> constructors;
		Vector<double> flagSets;
		Vector<double> flagGets;
		Vector<double> destructions;
		for(size_t i=0;i<numRuns;++i)
		{
			//FIRE_LOG_DEBUG("+++++ Run %d +++++", i);
			auto begin = high_resolution_clock::now();
			{
				ComponentManager testMgr(benchmarkAllocations);
				log_duration("manager construction", begin, construction);
				begin = high_resolution_clock::now();

				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d allocations performed...", i);
					testMgr.MakeInstance();
				}

				log_duration("component additions", begin, constructors);

				//FIRE_LOG_DEBUG("+++++ beginning %d flag sets", benchmarkAllocations);
				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					testMgr.SetName(ents[i], "Slim Shady");
				}
				log_duration("component name sets", begin, flagSets);

				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					String value(testMgr.GetName(ents[i]));
				}
				log_duration("component name gets", begin, flagGets);
				begin = high_resolution_clock::now();
			}
			log_duration("manager destruction", begin, destructions);
		}
		FIRE_LOG_DEBUG("+++++ Average over %d Runs +++++", numRuns);
		double constructiont = 0, compCons = 0,flagt = 0, flaggt=0, destructiont = 0;
		for(size_t i = 0; i < numRuns; ++i)
		{
			constructiont += construction[i];
			compCons += constructors[i];
			flagt += flagSets[i];
			flaggt += flagGets[i];
			destructiont += destructions[i];
		}
		FIRE_LOG_DEBUG("%f%f+++++ constructions     = %d", std::fixed, std::setprecision(15), constructiont / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ comp constructors = %d", std::fixed, std::setprecision(15), compCons / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ name sets         = %d", std::fixed, std::setprecision(15), flagt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ name gets         = %d", std::fixed, std::setprecision(15), flaggt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ destructions      = %d", std::fixed, std::setprecision(15), destructiont / numRuns);
	});

	return h;
}*/