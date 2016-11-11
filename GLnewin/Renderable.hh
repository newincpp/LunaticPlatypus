#pragma once

template <typename RenderingPolicy>
class Renderable : private RenderingPolicy {
public:
	using RenderingPolicy::init;
	using RenderingPolicy::update;
	using RenderingPolicy::destroy;
};
