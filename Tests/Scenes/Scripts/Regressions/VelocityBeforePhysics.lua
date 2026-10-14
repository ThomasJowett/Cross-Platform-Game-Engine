-- OnCreate runs before the physics world exists, so the body has no Box2D body yet.
-- GetLinearVelocity/GetAngularVelocity dereference it without a null check.

function OnCreate()
	local body = CurrentEntity:GetRigidBody2DComponent()
	Log.Info("VelocityBeforePhysics: calling GetLinearVelocity() in OnCreate")
	local velocity = body:GetLinearVelocity()
	Log.Info("VelocityBeforePhysics: calling GetAngularVelocity() in OnCreate")
	local spin = body:GetAngularVelocity()
	Log.Info("VelocityBeforePhysics: calling GetTransform() in OnCreate")
	local position, angle = body:GetTransform()

	Test.Expect(velocity.x == 0 and velocity.y == 0 and spin == 0, "velocities read as zero before physics started")
	Test.Pass("VelocityBeforePhysics: read the body before physics started")
end
