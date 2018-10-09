
#include <libCore/libCore.h>
#include <libCore/IRefCounted.h>
#include <libCore/RefPtr.h>
#include <libCore/WeakPtr.h>
#include <libCore/Assert.h>
#include <libHarnessed/libHarnessed.h>
#include <functional>

#include <libMath/Vector.h>
#include <libCore/SOA.h>

using std::cout;
using std::cin;
using std::endl;

using namespace Firestorm;


class TupleBasedSimpleComponentMgr
{
public:
	SOA<uint8_t, uint8_t, uint8_t> _s;
	TupleBasedSimpleComponentMgr(size_t allocs = 5)
	{
		_s.Reserve(allocs);
	}

	void MakeInstance()
	{
		_s.PushBack(0, 0, 0);
	}

	void SetFlag1(const std::size_t& entity, uint8_t value)
	{
		_s[0_soa][entity] = value;
	}

	void SetFlag2(const std::size_t& entity, uint8_t value)
	{
		_s[1_soa][entity] = value;
	}

	const uint8_t& GetFlag1(const std::size_t& entity) const
	{
		return _s[0_soa][entity];
	}

	const uint8_t& GetFlag2(const std::size_t& entity) const
	{
		return _s[1_soa][entity];
	}
};



class TupleBasedComplexComponentMgr
{
public:
	SOA<String, Vector3, float> _s;
	TupleBasedComplexComponentMgr(size_t allocs = 5)
	{
		_s.Reserve(allocs);
	}

	~TupleBasedComplexComponentMgr()
	{
	}

	void MakeInstance()
	{
		_s.PushBack("", Vector3{ 0.0f,0.0f,0.0f }, 9001.0f);
	}
	void SetName(const std::size_t& entity, const char* value)
	{
		_s[0_soa][entity] = value;
	}

	const String& GetName(const std::size_t& entity) const
	{
		return _s[0_soa][entity];
	}

	void SetVelocity(const std::size_t& entity, double value)
	{
		_s[1_soa][entity] = value;
	}

	const Vector3& GetVelocity(const std::size_t& entity) const
	{
		return _s[1_soa][entity];
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
	}

	void MakeInstance()
	{
		components.push_back(NaieveSimpleComponent{ 0,0,0 });
	}

	void SetFlag1(const std::size_t& e, const uint8_t& value)
	{
		components[e].flag1 = value;
	}

	const uint8_t& GetFlag1(const std::size_t& e) const
	{
		return components[e].flag1;
	}

	void SetFlag2(const std::size_t& e, const uint8_t& value)
	{
		components[e].flag2 = value;
	}

	const uint8_t& GetFlag2(const std::size_t& e) const
	{
		return components[e].flag2;
	}

	Vector<NaieveSimpleComponent> components;
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
	}

	void MakeInstance(size_t numItems = 1)
	{
		components.push_back(NaieveComplexComponent{ "",0.0,0.0f });
	}

	void SetName(const std::size_t& e, const String& value)
	{
		components[e].name = value;
	}

	const String& GetName(const std::size_t& e) const
	{
		return components[e].name;
	}

	void SetVelocity(const std::size_t& e, const double& value)
	{
		components[e].velocity = value;
	}

	const double& GetVelocity(const std::size_t& e) const
	{
		return components[e].velocity;
	}

	Vector<NaieveComplexComponent> components;
};

#define NUM_OBJECTS 5000
#define NUM_RUNS 100


RefPtr<TestHarness> libCorePrepareHarness(int argc, char** argv)
{
	FIRE_LOG_DEBUG("+++++ allocating entities");
	static Vector<std::size_t> ents;
	ents.reserve(NUM_OBJECTS);
	for(size_t i = 0; i < NUM_OBJECTS; ++i)
	{
		ents.push_back(i);
	}

	FIRE_LOG_DEBUG("Sizeof String: %d", sizeof(String));

	RefPtr<TestHarness> h(new TestHarness("libCore Tests"));



	h->It("SOA containers should report back the correct traits <uint8 * 4>", [](TestCase& t) {
		using SOAType = SOA<uint8_t, uint8_t, uint8_t, uint8_t>;
		SOAType container;
		t.AssertIsTrue(SOAType::IsTrivialDest, "the soa container should report back as trivially destructible");
		t.AssertIsTrue(SOAType::IsTrivialCons, "the soa container should report back as trivially constructible");
		t.Assert(SOAType::Sizeof == 4, "the soa container reported back the wrong size");
	});

	h->It("SOA containers should allocate their memory without issue", [](TestCase& t) {
		using SOAType = SOA<uint8_t, uint8_t, uint8_t, uint8_t>;
		SOAType container;
		container.Reserve(5);
		t.Assert(container.Capacity() == 5,
			Format("the container reported the wrong capacity. expected %d, got %d", 5, container.Capacity()));
		t.Assert(container.Size() == 0,
			Format("the container reported the wrong size. expected %d, got %d", 0, container.Size()));
		t.Assert(container.Bytes() == 5 * SOAType::Sizeof,
			Format("the container reported the wrong buffer size. expected %d, got %d", 5 * SOAType::Sizeof, container.Bytes()));
	});

	h->It("a container that has items pushed back should increase their size exponentially.", [](TestCase& t) {
		using SOAType = SOA<uint8_t, uint8_t, uint8_t, uint8_t>;
		SOAType container;
		t.Assert(container.Capacity() == 0,
			Format("[1]the container reported the wrong capacity. expected %d, got %d", 0, container.Capacity()));
		t.Assert(container.Size() == 0,
			Format("[1]the container reported the wrong size. expected %d, got %d", 0, container.Size()));
		t.Assert(container.Bytes() == 0,
			Format("[1]the container reported the wrong buffer size. expected %d, got %d", 0, container.Bytes()));

		container.PushBack(0, 0, 0, 0);

		t.Assert(container.Capacity() == 1,
			Format("[2]the container reported the wrong capacity. expected %d, got %d", 1, container.Capacity()));
		t.Assert(container.Size() == 1,
			Format("[2]the container reported the wrong size. expected %d, got %d", 1, container.Size()));
		t.Assert(container.Bytes() == 4,
			Format("[2]the container reported the wrong buffer size. expected %d, got %d", 4, container.Bytes()));

		container.PushBack(0, 0, 0, 0);
		t.Assert(container.Capacity() == 2,
			Format("[3]the container reported the wrong capacity. expected %d, got %d", 2, container.Capacity()));
		t.Assert(container.Size() == 2,
			Format("[3]the container reported the wrong size. expected %d, got %d", 2, container.Size()));
		t.Assert(container.Bytes() == 8,
			Format("[3]the container reported the wrong buffer size. expected %d, got %d", 8, container.Bytes()));

		container.PushBack(0, 0, 0, 0);
		t.Assert(container.Capacity() == 4,
			Format("[4]the container reported the wrong capacity. expected %d, got %d", 4, container.Capacity()));
		t.Assert(container.Size() == 3,
			Format("[4]the container reported the wrong size. expected %d, got %d", 3, container.Size()));
		t.Assert(container.Bytes() == 16,
			Format("[4]the container reported the wrong buffer size. expected %d, got %d", 16, container.Bytes()));
	});

	h->Profile(Format("Tuple Based SOA (Simple <uint8_t,uint8_t,uint8_t>)[%d Items]",NUM_OBJECTS), NUM_RUNS, [](Benchmark& bm) {

		Benchmark::SnapshotHandle* ssh = nullptr;
		{

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("manager constructor");
			TupleBasedSimpleComponentMgr* testMgr = new TupleBasedSimpleComponentMgr(NUM_OBJECTS);
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("component additions");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr->MakeInstance();
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("setting flags 1");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr->SetFlag1(ents[i], 101);
			}
			bm.StopSegment(ssh);

			ssh = bm.StartSegment("setting flags 2");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr->SetFlag2(ents[i], 101);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("getting flags 1");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				FIRE_ASSERT(testMgr->GetFlag1(ents[i]) == 101);
			}
			bm.StopSegment(ssh);

			ssh = bm.StartSegment("getting flags 2");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				FIRE_ASSERT(testMgr->GetFlag2(ents[i]) == 101);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("destruction");
			delete testMgr;
		}
		bm.StopSegment(ssh);
	});

	h->Profile(Format("Tuple Based SOA (Complex <string,double,float>)[%d Items]",NUM_OBJECTS), NUM_RUNS, [](Benchmark& bm) {

		Benchmark::SnapshotHandle* ssh = nullptr;
		{

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("manager constructor");
			TupleBasedComplexComponentMgr* testMgr = new TupleBasedComplexComponentMgr(NUM_OBJECTS);
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("component additions");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr->MakeInstance();
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("setting names");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr->SetName(ents[i], "Slim Shady");
			}
			bm.StopSegment(ssh);

			ssh = bm.StartSegment("setting velocities");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr->SetVelocity(ents[i], 9001.0);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("getting names");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				FIRE_ASSERT(testMgr->GetName(ents[i]) == "Slim Shady");
			}
			bm.StopSegment(ssh);

			ssh = bm.StartSegment("getting velocities");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				FIRE_ASSERT(testMgr->GetVelocity(ents[i]).x == 9001.0f);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("destruction");
			delete testMgr;
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