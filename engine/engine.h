#ifndef ENGINE_H
#define ENGINE_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>
#include <cmath>
#include <sstream>
#include <string>

using namespace sf;

class Block {
public:
	Block() : 
		m_pos(0, 0, 0),
		m_dim(1, 1, 1), 
		m_color(Color::White) {}

	Block(
		const Vector3f pos,
		const Vector3f dim,
		const Color color
	) : m_pos(pos), m_dim(dim), m_color(color) {}
	
	const Vector3f& getPosition() const { return m_pos; }
	const Vector3f& getDimensions() const { return m_dim; }
	const Color& getColor() const { return m_color; }
	
	void move(const Vector3f offset);
	void scale(const Vector3f amount);
	void setColor(const Color color);
private:
	Vector3f m_pos; // 3D Position
	Vector3f m_dim; // Block Dimensions
	Color m_color;
};

class Scene {
public:
	Scene();
	~Scene();

	void update();
	void render();
	void moveCamera();
	void renderBlock(const Block& block);
	void renderFace(Vector3f face[4], Color& color);
	float getFocal(int fovdeg);
	void updateFPS();
	sf::Vector2f translate(Vector3f point);
	bool isWindowOpen() const { return m_window.isOpen(); }
	float getDeltaSeconds() const { return m_deltaSeconds; }
	Block* getBlock(std::string name) { return m_blocks[name]; }

	void addBlock(
			std::string name,
			Block* pBlock
			);
private:
	RenderWindow m_window;
	//std::map<std::string, Block> m_blocks;

	Clock m_fpsClock;
	int m_frameCount;
	float m_fps;
	Font m_font;
	Text m_fpsText;

	Clock m_deltaClock;
	Time m_deltaTime;
	float m_deltaSeconds;

	// Camera Variables
	float m_fov;
	bool m_focus; // If window is focused
	float m_speed;
	float m_sensitivity;
	float m_focal;

	// Key state map, tracks if keys are pressed
	std::map<Keyboard::Key, bool> m_keyStates;

	// Camera Position and Rotation
	Vector3f m_cRotation;
	Vector3f m_cPosition;

	// Blocks
	std::map<std::string, Block*> m_blocks; 
};

#endif // ENGINE_H
	   
