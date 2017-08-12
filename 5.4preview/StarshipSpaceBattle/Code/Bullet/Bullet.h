#pragma once

////////////////////////////////////////////////////////
// Physicalized bullet shot from weaponry, expires on collision with another object
////////////////////////////////////////////////////////
class CBulletComponent final : public IEntityComponent
{
public:
	virtual ~CBulletComponent() {}

	//static void ReflectType(Schematyc::CTypeDesc<CBulletComponent>& desc) 
	//{
	//	desc.SetGUID("{774B000F-05D1-427F-A1D9-E1F8E5CFA853}"_cry_guid); 
	//}

	// IEntityComponent
	virtual void Initialize() override
	{
		// Set the model
		const int geometrySlot = 1;
		m_pEntity->LoadGeometry(geometrySlot, "Assets\\Objects\\Bullet\\Bullet.cgf");
		m_pEntity->SetName("Bullet");

		// Load the custom bullet material.
		// This material has the 'mat_bullet' surface type applied, which is set up to play sounds on collision with 'mat_default' objects in Libs/MaterialEffects
		auto *pBulletMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial("Assets\\Materials\\Bullet");
		m_pEntity->SetMaterial(pBulletMaterial);

		// Now create the physical representation of the entity
		//SEntityPhysicalizeParams physParams;
		//physParams.type = PE_PARTICLE;
		//physParams.mass = 1000;
		////physParams.nFlagsOR = ent_all; //| geom_colltype_ray;

		//pe_params_particle particle;
		//particle.mass = 1000.0f;
		//particle.flags = particle_traceable | particle_no_roll | pef_log_collisions;
		//
		//particle.size = 1.0f;
		//particle.thickness = 1.0f;
		//particle.gravity = Vec3(0, 0, 0);
		//particle.kAirResistance = 0.0f;
		//particle.surface_idx = 0;
		//particle.velocity = 30.0f;

		//// heading is the movedir for patricle = it's get from entity's spawned world 
		//particle.heading = GetEntity()->GetWorldRotation().GetColumn1();
		//physParams.pParticle = &particle;

		//GetEntity()->Physicalize(physParams);

		SEntityPhysicalizeParams physParams;
		physParams.type = PE_RIGID;
		physParams.mass = 200;
		physParams.nSlot = geometrySlot;
		physParams.nFlagsOR = pef_ignore_areas | pef_log_collisions | pef_monitor_collisions;

		GetEntity()->Physicalize(physParams);

		pe_simulation_params simParams;
		simParams.mass = physParams.mass;	// if this mass value not preseted it fly up into the space
		simParams.gravity = Vec3(0, 0, 0);
		//simParams.collTypes = ent_all;
		m_pEntity->GetPhysics()->SetParams(&simParams, 0);



		// Make sure that bullets are always rendered regardless of distance
		// Ratio is 0 - 255, 255 being 100% visibility
		GetEntity()->SetViewDistRatio(255);

		// Apply an impulse so that the bullet flies forward
		if (auto *pPhysics = GetEntity()->GetPhysics())
		{
			pe_action_impulse impulseAction;

			const float initialVelocity = 8000.0f;

			// Set the actual impulse, in this cause the value of the initial velocity CVar in bullet's forward direction
			impulseAction.impulse = GetEntity()->GetWorldRotation().GetColumn1() * initialVelocity;

			// Send to the physical entity
			pPhysics->Action(&impulseAction);
		}
	}

	virtual uint64 GetEventMask() const override { return BIT64(ENTITY_EVENT_COLLISION) | BIT64(ENTITY_EVENT_UPDATE); }
	virtual void ProcessEvent(SEntityEvent& event) override;
	// ~IEntityComponent
protected:
	void SpawnHitEffect(EventPhysCollision* col);

	float lifeTime = 0;
};