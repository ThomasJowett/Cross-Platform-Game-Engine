-- Ray casts and point queries must return the right colliders, points and normals. Near Block spans x -0.5 to 0.5
-- and Far Block spans 3.5 to 4.5, both at y 0; High Block spans -0.5 to 0.5 at y 5. All are static, with no gravity.
-- Physics only exists once the scene is running, so the checks run in the first update.

local Epsilon = 0.01

local function Near(actual, expected, message)
	Test.Expect(math.abs(actual - expected) <= Epsilon, message .. " (expected " .. expected .. ", got " .. actual .. ")")
end

function OnUpdate(deltaTime)
	-- The first collider hit is the nearest one, from either side
	local hit = CurrentScene:RayCast2D(Vec2.new(-3, 0), Vec2.new(8, 0))
	Test.Expect(hit.Hit, "a ray along the row hits something")
	Test.Equal(hit.Entity:GetName(), "Near Block", "the first hit going right is the nearest block")
	Near(hit.Point.x, -0.5, "the hit is on the near block's left face")
	Near(hit.Point.y, 0, "the hit height is the ray's height")
	Near(hit.Normal.x, -1, "the left face normal points left")
	Near(hit.Normal.y, 0, "the left face normal has no vertical part")

	local reverse = CurrentScene:RayCast2D(Vec2.new(8, 0), Vec2.new(-3, 0))
	Test.Equal(reverse.Entity:GetName(), "Far Block", "the first hit going left is the other block")
	Near(reverse.Point.x, 4.5, "the hit is on the far block's right face")
	Near(reverse.Normal.x, 1, "the right face normal points right")

	-- A ray from above hits the top face
	local down = CurrentScene:RayCast2D(Vec2.new(4, 3), Vec2.new(4, -3))
	Test.Equal(down.Entity:GetName(), "Far Block", "a ray from above hits the block below")
	Near(down.Point.y, 0.5, "the hit is on the top face")
	Near(down.Normal.y, 1, "the top face normal points up")

	-- A ray that stops short, or misses, hits nothing
	Test.Expect(not CurrentScene:RayCast2D(Vec2.new(-3, 0), Vec2.new(-1, 0)).Hit, "a ray that ends before the block misses")
	Test.Expect(not CurrentScene:RayCast2D(Vec2.new(-3, 2), Vec2.new(8, 2)).Hit, "a ray passing above the blocks misses")

	-- Every collider on the line, nearest first
	local hits = CurrentScene:MultiRayCast2D(Vec2.new(-3, 0), Vec2.new(8, 0))
	Test.Equal(#hits, 2, "the multi ray hits both blocks")
	if #hits == 2 then
		Test.Equal(hits[1].Entity:GetName(), "Near Block", "the multi ray lists the near block first")
		Test.Equal(hits[2].Entity:GetName(), "Far Block", "the multi ray lists the far block second")
	end
	Test.Equal(#CurrentScene:MultiRayCast2D(Vec2.new(-3, 2), Vec2.new(8, 2)), 0, "the multi ray over the blocks hits nothing")

	-- Point queries are exact: inside a collider, just outside it, and empty space
	local inside = CurrentScene:QueryPoint(Vec2.new(0.4, 0.4))
	Test.Equal(#inside, 1, "a point inside the near block finds one entity")
	if #inside == 1 then
		Test.Equal(inside[1]:GetName(), "Near Block", "the point inside is in the near block")
	end
	Test.Equal(#CurrentScene:QueryPoint(Vec2.new(0.6, 0)), 0, "a point just outside the near block finds nothing")
	Test.Equal(#CurrentScene:QueryPoint(Vec2.new(2, 0)), 0, "a point between the blocks finds nothing")
	local high = CurrentScene:QueryPoint(Vec2.new(0, 5))
	Test.Equal(#high, 1, "a point in the high block finds one entity")
	if #high == 1 then
		Test.Equal(high[1]:GetName(), "High Block", "the point is in the high block")
	end

	Test.Pass("RayCastQueries: checked rays and point queries")
end
