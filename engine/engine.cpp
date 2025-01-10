#include "engine.h"

// Scene Class

Scene::Scene() {
	// Window Setup
	m_window.create(
			VideoMode(800,600),
			"Scene", 
			Style::Close
			);
	std::cout << "Window Created" << std::endl;

	// Getting Font
	if (!m_font.loadFromFile("JetBrainsMono.ttf")) {
		printf("Unable to load font.");
	}
	else {
		m_fpsText.setFont(m_font);
	}
	m_fpsText.setCharacterSize(12);
	m_fpsText.setFillColor(sf::Color::White);

	// Frame Count and FPS
	m_frameCount = m_fps = 0;
	
	// FOV and Focus
	m_fov = 40;
	m_focal = getFocal(m_fov);
	m_focus = false; // If window is focused

	// Speed and Sensitivity
	m_speed = 5;
	m_sensitivity = 130;

	// Key States
	m_keyStates = {
		{sf::Keyboard::W, false},
		{sf::Keyboard::S, false},
		{sf::Keyboard::A, false},
		{sf::Keyboard::D, false},
		{sf::Keyboard::LShift, false},
		{sf::Keyboard::Space, false},
		{sf::Keyboard::Up, false},
		{sf::Keyboard::Down, false},
		{sf::Keyboard::Left, false},
		{sf::Keyboard::Right, false}
	};

	// Camera Position and Rotation
	m_cRotation = {0, 0, 0};
	m_cPosition = {0, 0, 0};
}

Scene::~Scene() {
	while (!m_blocks.empty()) {
		for (const auto& [key, val] : m_blocks) { 
			delete m_blocks[key];
			m_blocks.erase(key);
		}
	}
}

void Scene::update() {
	m_deltaTime = m_deltaClock.restart();
	m_deltaSeconds = m_deltaTime.asSeconds();

	Event event;
	while (m_window.pollEvent(event)) {
		if (event.type == Event::Closed)
			m_window.close();
		else if (event.type == Event::GainedFocus)
			m_focus = true;
		else if (event.type == Event::LostFocus)
			m_focus = false;	

		if (m_focus) {
			for (auto& [key, state] : m_keyStates) {
				if (event.key.code == key) {
					if (event.type == Event::KeyPressed)
						state = true;
					else if (event.type == Event::KeyReleased)
						state = false;
				}
			}
		}
	}

	m_window.clear();

	updateFPS();
	render();
	moveCamera();

	m_window.display();
}

void Scene::moveCamera() {
	float deltaSpeed = m_speed * m_deltaSeconds;
	float deltaSensitivity = m_sensitivity * m_deltaSeconds;

	if (m_keyStates[sf::Keyboard::W]) {
		m_cPosition.x += sin(m_cRotation.z * (M_PI / 180)) * deltaSpeed;
		m_cPosition.z += cos(m_cRotation.z * (M_PI / 180)) * deltaSpeed;
	}
	if (m_keyStates[sf::Keyboard::S]) {
		m_cPosition.x -= sin(m_cRotation.z * (M_PI / 180)) * deltaSpeed;
		m_cPosition.z -= cos(m_cRotation.z * (M_PI / 180)) * deltaSpeed;
	}
	if (m_keyStates[sf::Keyboard::A]) {
		m_cPosition.x += sin((m_cRotation.z - 90) * (M_PI / 180)) * deltaSpeed;
		m_cPosition.z += cos((m_cRotation.z - 90) * (M_PI / 180)) * deltaSpeed;
	}
	if (m_keyStates[sf::Keyboard::D]) {
		m_cPosition.x += sin((m_cRotation.z + 90) * (M_PI / 180)) * deltaSpeed;
		m_cPosition.z += cos((m_cRotation.z + 90) * (M_PI / 180)) * deltaSpeed;
	}
	if (m_keyStates[sf::Keyboard::LShift]) {
		m_cPosition.y += deltaSpeed;
	}
	if (m_keyStates[sf::Keyboard::Space]) {
		m_cPosition.y -= deltaSpeed;
	}
	if (m_keyStates[sf::Keyboard::Up]) {
		if (m_cRotation.x >= -60) {
			m_cRotation.x = std::fmod(m_cRotation.x - deltaSensitivity, 360);
		}
	}	
	if (m_keyStates[sf::Keyboard::Down]) {
		if (m_cRotation.x <= 75) {
			m_cRotation.x = std::fmod(m_cRotation.x + deltaSensitivity, 360);
		}
	}
	if (m_keyStates[sf::Keyboard::Left]) {
		m_cRotation.z = std::fmod(m_cRotation.z - deltaSensitivity, 360);
	}
	if (m_keyStates[sf::Keyboard::Right]) {
		m_cRotation.z = std::fmod(m_cRotation.z + deltaSensitivity, 360);
	}	
}

void Scene::render() {
	if (!m_blocks.empty()) {
		for (const auto& [key, val] : m_blocks) {
			renderBlock(*val);
		}
	}
}

void Scene::renderBlock(const Block& block) {
	Vector3f pos = block.getPosition();
	Vector3f dim = block.getDimensions();
	Color blockColor = block.getColor();

	Vector3f verts[8] {
		{pos.x, pos.y, pos.z},
		{pos.x + dim.x, pos.y, pos.z},
		{pos.x, pos.y + dim.y, pos.z},
		{pos.x + dim.x, pos.y + dim.y, pos.z},
		{pos.x, pos.y, pos.z + dim.z},
		{pos.x + dim.x, pos.y, pos.z + dim.z},
		{pos.x, pos.y + dim.y, pos.z + dim.z},
		{pos.x + dim.x, pos.y + dim.y, pos.z + dim.z},
	};

	Vector3f faces[6][4] = {
		// Front Face
		{verts[0], verts[1], verts[2], verts[3]},
		// Back Face
		{verts[4], verts[5], verts[6], verts[7]},
		// Top Face
		{verts[0], verts[1], verts[4], verts[5]},
		// Bottom Face
		{verts[3], verts[2], verts[7], verts[6]},
		// Right Face
		{verts[0], verts[2], verts[4], verts[6]},
		// Left Face
		{verts[1], verts[3], verts[5], verts[7]}
	};

	for (int i = 0; i < 6; ++i) {
		renderFace(faces[i], blockColor);
	}
}

void Scene::renderFace(Vector3f face[4], Color& color) {
	VertexArray lines(LineStrip, 5);

	lines[0].position = translate(face[0]);
	lines[1].position = translate(face[1]);
	lines[2].position = translate(face[3]);
	lines[3].position = translate(face[2]);
	lines[4].position = translate(face[0]);

	for (int i = 0; i < 5; i++)
		lines[i].color = color;

	m_window.draw(lines);
}

float Scene::getFocal(int fovdeg) {
	float FOVrad = fovdeg * (M_PI / 180.0f); // Degrees to Radians
	return 1.0 / tan(FOVrad / 2.0f); // Calculate the Focal Length
}

void Scene::updateFPS() {
	m_frameCount++;

	if (m_fpsClock.getElapsedTime().asSeconds() >= 1.0f) {
		m_fps = m_frameCount;
		m_frameCount = 0;
		m_fpsClock.restart();
	}

	std::stringstream ss;
	ss << "FPS: " << m_fps;
	m_fpsText.setString(ss.str());

	m_window.draw(m_fpsText);
}

Vector2f Scene::translate(Vector3f point) {
	Vector2f newPoint;
	
	point.x -= m_cPosition.x;
	point.y -= m_cPosition.y;
	point.z -= m_cPosition.z;
	
	float yawRad = m_cRotation.z * (M_PI / 180);
	float cosYaw = cos(yawRad);
	float sinYaw = sin(yawRad);

	float rotatedX = cosYaw * point.x - sinYaw * point.z;
	float rotatedZ = sinYaw * point.x + cosYaw * point.z;

	point.x = rotatedX;
	point.z = rotatedZ;

	float pitchRad = m_cRotation.x * (M_PI / 180);
	float cosPitch = cos(pitchRad);
	float sinPitch = sin(pitchRad);

	float rotatedY = cosPitch * point.y - sinPitch * point.z;
	point.z = sinPitch * point.y + cosPitch * point.z;
	point.y = rotatedY;

	if (point.z <= 0) {
		point.z = 0.01f;
	}

	Vector2f windowSize = (Vector2f)m_window.getSize();
	newPoint.x = (point.x * m_focal) / (point.z / 100) + (windowSize.x / 2);
	newPoint.y = (point.y * m_focal) / (point.z / 100) + (windowSize.y / 2);

	return newPoint;
}

void Scene::addBlock(
		std::string name, 
		Block* pBlock) {
	m_blocks[name] = pBlock;
}

// Block Class

void Block::move(const Vector3f offset) {
	m_pos += offset;
}

void Block::scale(const Vector3f amount) {
	m_dim += amount;
}

void Block::setColor(const Color newColor) {
	m_color = newColor;
}

