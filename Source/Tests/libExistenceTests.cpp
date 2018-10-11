#include <libCore/libCore.h>
#include <libCore/RefPtr.h>
#include <libHarnessed/libHarnessed.h>

#include <libExistence/libExistence.h>
#include <libExistence/Universe.h>
#include <libExistence/System.h>
#include <libExistence/Entity.h>
#include <libExistence/ComponentDefinition.h>

#include <libMath/Vector.h>
#include <libMath/Quaternion.h>
#include <libMath/Matrix.h>

#include <libCore/Logger.h>

#include <iomanip>

using namespace Firestorm;

class PosRotComponent : public Component<Vector3, Quaternion>
{
public:
	FIRE_SOA_INDEX(ENTITY, 0);
	FIRE_SOA_INDEX(POSITION, 1);

	PosRotComponent(EntityMgr& eMgr)
	: Base(eMgr, DestructionHandler::kImmediate)
	{
	}

	const Vector3& GetPosition(Instance i) const
	{
		return _this[POSITION][i];
	}

	void SetPosition(Instance i, const Vector3& value)
	{
		_this[POSITION][i] = value;
	}
};

RefPtr<TestHarness> libExistencePrepareHarness(int ac, char** av)
{
	RefPtr<TestHarness> h(new TestHarness("libExistence"));


	h->It("entity managers should register and unregister destructor callbacks as the definitions fall out of scope", 
		[&](TestCase& t) {
			UUIDMgr uuidMgr;
			EntityMgr eMgr(uuidMgr);
			{
				PosRotComponent componentMgr(eMgr);
				t.Assert(eMgr.GetNumRegisteredDestructors() == 1, "the destructor callback was not registered");
			}
			t.Assert(eMgr.GetNumRegisteredDestructors() == 0, "the destructor callback was not unregistered");
		});

	h->It("component definitions should be able to map entities to components", [&](TestCase& t) {
		UUIDMgr uuidMgr;
		EntityMgr eMgr(uuidMgr);
		PosRotComponent componentMgr(eMgr);

		Entity e = eMgr.SpawnEntity();
		IComponent::Instance eye = componentMgr.Assign(e);
		t.Assert(eye != FIRE_INVALID_COMPONENT, "received an invalid component for some reason");

		IComponent::Instance i = componentMgr.Lookup(e);
		IComponent::Instance ii = componentMgr.Lookup(e);
		t.Assert(eye == i, "the component manager did not properly map the Entity to the component");
	});

	h->It("component definitions should never double-map entities",[&](TestCase& t) {
		UUIDMgr uuidMgr;
		EntityMgr eMgr(uuidMgr);
		PosRotComponent componentMgr(eMgr);
		size_t count = 10;

		Vector<Entity> entities;
		entities.reserve(count);
		Vector<IComponent::Instance> components;
		components.reserve(count);

		for(size_t i=0; i<count; ++i)
		{
			entities.push_back(eMgr.SpawnEntity());
			IComponent::Instance ii = componentMgr.Assign(entities.back());

			// if we're double mapping, the call to Lookup would return a duplicate component index.
			components.push_back(componentMgr.Lookup(entities.back()));
		}

		for(size_t i=0; i<count; ++i)
		{
			IComponent::Instance ii = components[i];
			for(size_t j=0; j<count; j++)
			{
				if(i == j) continue;
				IComponent::Instance ji = components[j];
				t.Assert(ii != ji, "a component was double-mapped");
			}
		}
	});


	h->It("component definitions should re-map to their entities once one has been despawned",[&](TestCase& t) {
		UUIDMgr uuidMgr;
		EntityMgr eMgr(uuidMgr);
		PosRotComponent componentMgr(eMgr);
		size_t count = 10;

		Vector<Entity> entities;
		entities.reserve(count);
		Vector<IComponent::Instance> components;
		components.reserve(count);

		for(size_t i=0; i<count; ++i)
		{
			entities.push_back(eMgr.SpawnEntity());
			IComponent::Instance ii = componentMgr.Assign(entities.back());
			components.push_back(ii);
		}

		IComponent::Instance i_last = componentMgr.Lookup(entities.back());
		componentMgr.SetPosition(i_last, { 9001.0f,9001.0f,9001.0f });
		{
			const Vector3& pos_test = componentMgr.GetPosition(i_last);
			t.Assert(pos_test.x == 9001.0f, "the component did not have the right values");
			t.Assert(pos_test.y == 9001.0f, "the component did not have the right values");
			t.Assert(pos_test.z == 9001.0f, "the component did not have the right values");
		}

		// erase a random entity in the middle of the buffer
		auto ent = entities.begin() + 4;
		eMgr.DespawnEntity(*ent);
		entities.erase(ent);

		Entity testEnt = entities.back();
		IComponent::Instance i_test = componentMgr.Lookup(testEnt);
		const Vector3& pos_test = componentMgr.GetPosition(i_test);
		t.Assert(pos_test.x == 9001.0f, "the component did not have the right values");
		t.Assert(pos_test.y == 9001.0f, "the component did not have the right values");
		t.Assert(pos_test.z == 9001.0f, "the component did not have the right values");
	});


	h->It("see above, only we're removing what would be the last component",[&](TestCase& t) {
		UUIDMgr uuidMgr;
		EntityMgr eMgr(uuidMgr);
		PosRotComponent componentMgr(eMgr);
		size_t count = 10;

		Vector<Entity> entities;
		entities.reserve(count);
		Vector<IComponent::Instance> components;
		components.reserve(count);

		for(size_t i=0; i<count; ++i)
		{
			entities.push_back(eMgr.SpawnEntity());
			components.push_back(componentMgr.Assign(entities.back()));
		}

		IComponent::Instance i_last = componentMgr.Lookup(entities.back());
		componentMgr.SetPosition(i_last, { 9001.0f,9001.0f,9001.0f });
		{
			const Vector3& pos_test = componentMgr.GetPosition(i_last);
			t.Assert(pos_test.x == 9001.0f, "the component did not have the right values");
			t.Assert(pos_test.y == 9001.0f, "the component did not have the right values");
			t.Assert(pos_test.z == 9001.0f, "the component did not have the right values");
		}

		// erase a random entity in the middle of the buffer
		Entity ent = entities[entities.size()-1];
		eMgr.DespawnEntity(ent);
		entities.pop_back();

		Entity testEnt = entities.back();
		IComponent::Instance i_test = componentMgr.Lookup(testEnt);
		t.Assert(i_test != i_last, "the last component didn't destroy itself properly");
	});

	return h;
}