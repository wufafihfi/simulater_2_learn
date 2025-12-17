#ifndef BZD_SFML_MAIN
#define BZD_SFML_MAIN

#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <memory>

#include <box2d/box2d.h>

#include "appInit.h"
#include "PhysicsWorld.h"

namespace bzd_SFML_main {

	void Bzd_SFML_Ready();
	void Bzd_SFML_Update();
	void Bzd_SFML_draw();

	void UIready();
	void Bzd_SFML_Event(std::optional<sf::Event> _event);
	void UIuodata();

	void Bzd_End();
}

#endif#pragma once
