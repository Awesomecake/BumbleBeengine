#pragma once

// Jolt Physics Library (https://github.com/jrouwe/JoltPhysics)
// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

// The Jolt headers don't include Jolt.h. Always include Jolt.h before including any other Jolt header.
// You can use Jolt.h in your precompiled header to speed up compilation.
#include <Jolt/Jolt.h>

// Jolt includes
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhase.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Geometry/OrientedBox.h>

// STL includes
#include <iostream>
#include <cstdarg>
#include <thread>
#include <unordered_map>

#include "AudioManager.h"

// Disable common warnings triggered by Jolt, you can use JPH_SUPPRESS_WARNING_PUSH / JPH_SUPPRESS_WARNING_POP to store and restore the warning state
JPH_SUPPRESS_WARNINGS;

// All Jolt symbols are in the JPH namespace
using namespace JPH;

// If you want your code to compile using single or double precision write 0.0_r to get a Real value that compiles to double or float depending if JPH_DOUBLE_PRECISION is set or not.
using namespace JPH::literals;

// We're also using STL classes in this example
using namespace std;

#pragma region JoltPhysicsStuff
// Callback for traces, connect this to your own trace function if you have one
static void TraceImpl(const char* inFMT, ...)
{
	// Format the message
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);

	// Print to the TTY
	cout << buffer << endl;
}

#ifdef JPH_ENABLE_ASSERTS

// Callback for asserts, connect this to your own assert handler if you have one
static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, uint inLine)
{
	// Print to the TTY
	cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr ? inMessage : "") << endl;

	// Breakpoint
	return true;
};

#endif // JPH_ENABLE_ASSERTS

// Layer that objects can be in, determines which other objects it can collide with
// Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies, but you can have more
// layers if you want. E.g. you could have a layer for high detail collision (which is not used by the physics simulation
// but only if you do collision testing).
namespace Layers
{
	static constexpr ObjectLayer NON_MOVING = 0;
	static constexpr ObjectLayer MOVING = 1;
	static constexpr ObjectLayer NUM_LAYERS = 2;
};

/// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter
{
public:
	virtual bool					ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override
	{
		switch (inObject1)
		{
		case Layers::NON_MOVING:
			return inObject2 == Layers::MOVING; // Non moving only collides with moving
		case Layers::MOVING:
			return true; // Moving collides with everything
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};

// Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
// a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
// You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
// many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
// your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
namespace BroadPhaseLayers
{
	static constexpr BroadPhaseLayer NON_MOVING(0);
	static constexpr BroadPhaseLayer MOVING(1);
	static constexpr uint NUM_LAYERS(2);
};

// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface
{
public:
	BPLayerInterfaceImpl()
	{
		// Create a mapping table from object to broad phase layer
		mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
	}

	virtual uint					GetNumBroadPhaseLayers() const override
	{
		return BroadPhaseLayers::NUM_LAYERS;
	}

	virtual BroadPhaseLayer			GetBroadPhaseLayer(ObjectLayer inLayer) const override
	{
		JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
		return mObjectToBroadPhase[inLayer];
	}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
	{
		switch ((BroadPhaseLayer::Type)inLayer)
		{
		case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
		default:													JPH_ASSERT(false); return "INVALID";
		}
	}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
	BroadPhaseLayer					mObjectToBroadPhase[Layers::NUM_LAYERS];
};

/// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter
{
public:
	virtual bool				ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override
	{
		switch (inLayer1)
		{
		case Layers::NON_MOVING:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case Layers::MOVING:
			return true;
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};
#pragma endregion

// An example contact listener
class MyContactListener : public ContactListener
{
	AudioManager* audioManager;

public:
	function<void(AudioManager* audioManager)> collisionDelegate;

	// See: ContactListener
	virtual ValidateResult	OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult) override
	{
		//cout << "Contact validate callback" << endl;

		// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
		return ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	virtual void OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override
	{
		collisionDelegate(audioManager);
		//cout << "A contact was added" << endl;
	}

	virtual void OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override
	{
		//cout << "A contact was persisted" << endl;
	}

	virtual void OnContactRemoved(const SubShapeIDPair& inSubShapePair) override
	{
		//cout << "A contact was removed" << endl;
	}
};

// An example activation listener
class MyBodyActivationListener : public BodyActivationListener
{
public:
	virtual void		OnBodyActivated(const BodyID& inBodyID, uint64 inBodyUserData) override
	{
		cout << "A body got activated" << endl;
	}

	virtual void		OnBodyDeactivated(const BodyID& inBodyID, uint64 inBodyUserData) override
	{
		cout << "A body went to sleep" << endl;
	}
};


class PhysicsManager
{
public:
	PhysicsManager();
	~PhysicsManager();

	void JoltPhysicsFrame();
	void DeInitPhysics();
	BodyID CreateSphereBody(RVec3 position, float size, EMotionType motionType, JPH::EAllowedDOFs degreesOfFreedom = JPH::EAllowedDOFs::All);
	BodyID CreateCubeBody(RVec3 position, Vec3 size, EMotionType motionType, JPH::EAllowedDOFs degreesOfFreedom = JPH::EAllowedDOFs::All);

	AllHitCollisionCollector<RayCastBodyCollector>  JoltRayCast(Vec3::ArgType origin, Vec3Arg direction, float length);
	AllHitCollisionCollector<CollideShapeBodyCollector> JoltShapeCast(Vec3Arg min, Vec3Arg max, Vec3Arg direction);

	BodyInterface* body_interface;
	MyContactListener contact_listener;

private:
	//-------------JoltPhysics-------------------

	// This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get an error.
	// Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
	const uint cMaxBodies = 1024;

	// This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the default settings.
	const uint cNumBodyMutexes = 0;

	// This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
	// body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this buffer
	// too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is slightly less efficient.
	// Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
	const uint cMaxBodyPairs = 1024;

	// This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are detected than this
	// number then these contacts will be ignored and bodies will start interpenetrating / fall through the world.
	// Note: This value is low because this is a simple test. For a real project use something in the order of 10240.
	const uint cMaxContactConstraints = 1024;

	// Create mapping table from object layer to broadphase layer
	// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
	// Also have a look at BroadPhaseLayerInterfaceTable or BroadPhaseLayerInterfaceMask for a simpler interface.
	BPLayerInterfaceImpl broad_phase_layer_interface;

	// Create class that filters object vs broadphase layers
	// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
	// Also have a look at ObjectVsBroadPhaseLayerFilterTable or ObjectVsBroadPhaseLayerFilterMask for a simpler interface.
	ObjectVsBroadPhaseLayerFilterImpl object_vs_broadphase_layer_filter;

	// Create class that filters object vs object layers
	// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
	// Also have a look at ObjectLayerPairFilterTable or ObjectLayerPairFilterMask for a simpler interface.
	ObjectLayerPairFilterImpl object_vs_object_layer_filter;

	PhysicsSystem physics_system;

	vector<BodyID> bodies;
	Body* floor;

	TempAllocatorImpl* temp_allocator;
	JobSystemThreadPool* job_system;

	// We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
	const float cDeltaTime = 1.0f / 60.0f;
};