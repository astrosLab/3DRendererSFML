#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <string>
#include <map>

float FOV = 30;
float focal;
float cRotation[3] = {0, 0, 0};
sf::Vector3f cPosition = {0, 0, 0};
bool focus = false;

sf::Vector2f translate(sf::Vector3f point) {
	sf::Vector2f npoint;	

	point.x -= cPosition.x;
	point.y -= cPosition.y;
	point.z -= cPosition.z;

	float yawRad = cRotation[2] * (M_PI / 180);
	float cosYaw = cos(yawRad);
	float sinYaw = sin(yawRad);

	float rotatedX = cosYaw * point.x - sinYaw * point.z;
	float rotatedZ = sinYaw * point.x + cosYaw * point.z;

	point.x = rotatedX;
	point.z = rotatedZ;

	float pitchRad = cRotation[0] * (M_PI / 180);
	float cosPitch = cos(pitchRad);
	float sinPitch = sin(pitchRad);

	float newY = cosPitch * point.y - sinPitch * point.z;
	point.z = sinPitch * point.y + cosPitch * point.z;
	point.y = newY;
	
	if (point.z <= 0) {
		point.z = 0.01f;
	}

	npoint.x = (point.x * focal) / (point.z / 100) + (1920.f / 2);
	npoint.y = (point.y * focal) / (point.z / 100) + (1080.f / 2);

	return npoint;
}

void renderPlane(sf::RenderWindow& window, sf::Vector3f points[4]) {
	sf::Vector2f np0 = translate(points[0]);
	sf::Vector2f np1 = translate(points[1]);
	sf::Vector2f np2 = translate(points[2]);	
	sf::Vector2f np3 = translate(points[3]);

	sf::VertexArray lines(sf::LinesStrip, 5);

	lines[0].position = np0;
	lines[1].position = np1;
	lines[2].position = np3;
	lines[3].position = np2;
	lines[4].position = np0;

	window.draw(lines);

}

void renderCube(sf::RenderWindow& window, sf::Vector3f position) {
	sf::Vector3f plane0[4] {
		sf::Vector3f(position.x * 100, position.y * 100, position.z * 100),
		sf::Vector3f(position.x * 100 + 100, position.y * 100, position.z * 100),
		sf::Vector3f(position.x * 100, position.y * 100, position.z * 100 + 100),
		sf::Vector3f(position.x * 100 + 100, position.y * 100, position.z * 100 + 100)
	};

	sf::Vector3f plane1[4] {
		sf::Vector3f(position.x * 100, position.y * 100 + 100, position.z * 100),
		sf::Vector3f(position.x * 100 + 100, position.y * 100 + 100, position.z * 100),
		sf::Vector3f(position.x * 100, position.y * 100 + 100, position.z * 100 + 100),
		sf::Vector3f(position.x * 100 + 100, position.y * 100 + 100, position.z * 100 + 100)
	};

	sf::Vector3f plane2[4] {
		sf::Vector3f(position.x * 100, position.y * 100, position.z * 100),
		sf::Vector3f(position.x * 100 + 100, position.y * 100, position.z * 100),
		sf::Vector3f(position.x * 100, position.y * 100 + 100, position.z * 100),
		sf::Vector3f(position.x * 100 + 100, position.y * 100 + 100, position.z * 100)
	};

	sf::Vector3f plane3[4] {
		sf::Vector3f(position.x * 100, position.y * 100, position.z * 100 + 100),
		sf::Vector3f(position.x * 100 + 100, position.y * 100, position.z * 100 + 100),
		sf::Vector3f(position.x * 100, position.y * 100 + 100, position.z * 100 + 100),
		sf::Vector3f(position.x * 100 + 100, position.y * 100 + 100, position.z * 100 + 100)
	};	

	renderPlane(window, plane0);
	renderPlane(window, plane1);
	renderPlane(window, plane2);
	renderPlane(window, plane3);
}

void makeGrid(sf::RenderWindow& window, int xz) {
	sf::Vector3f cube0 = {0, 4, 0};
	int x, z;

	for (x = 0; x < xz; ++x) {
		for (z = 0; z < xz; ++z) {
			cube0.z++;
			renderCube(window, cube0);
		}	
		cube0.x++;
		cube0.z = 0;
	}
}

float calcFocalLength(float FOV_deg) {
	float FOV_rad = FOV_deg * (M_PI / 180.0f);
	return 1.0 / tan(FOV_rad / 2.0f);
}

int main() {
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "3D Renderer", sf::Style::None);
	//window.setFramerateLimit(60);
	sf::Clock clock;
	sf::Clock deltaClock;
	int frameCount = 0;
	float fps = 0;
	float speed = 400;
	float sensitivity = 130;
	std::map<sf::Keyboard::Key, bool> keyStates = {
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

	sf::Font font;
	if (!font.loadFromFile("jb.ttf")) {
		return -1;
	}
	sf::Text fpsText;
	fpsText.setFont(font);
	fpsText.setCharacterSize(12);
	fpsText.setFillColor(sf::Color::White);

	focal = calcFocalLength(FOV);

	while (window.isOpen()) {
		sf::Time deltaTime = deltaClock.restart();
		float deltaSeconds = deltaTime.asSeconds();

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::GainedFocus)
				focus = true;
			else if (event.type == sf::Event::LostFocus)
				focus = false;

			if (focus) {
				for (auto& keyState : keyStates) {
					if (event.key.code == keyState.first) {
						if (event.type == sf::Event::KeyPressed)
							keyState.second = true;
						else if (event.type == sf::Event::KeyReleased)
							keyState.second = false;

					}
				}
			}
		}

		if (focus) {
			float deltaSpeed = speed * deltaSeconds;
			float deltaSensitivity = sensitivity * deltaSeconds;

			if (keyStates[sf::Keyboard::W]) {
				cPosition.x += sin(cRotation[2] * (M_PI / 180)) * deltaSpeed;
				cPosition.z += cos(cRotation[2] * (M_PI / 180)) * deltaSpeed;
			}
			if (keyStates[sf::Keyboard::S]) {
				cPosition.x -= sin(cRotation[2] * (M_PI / 180)) * deltaSpeed;
				cPosition.z -= cos(cRotation[2] * (M_PI / 180)) * deltaSpeed;
			}
			if (keyStates[sf::Keyboard::A]) {
				cPosition.x += sin((cRotation[2] - 90) * (M_PI / 180)) * deltaSpeed;
				cPosition.z += cos((cRotation[2] - 90) * (M_PI / 180)) * deltaSpeed;
			}
			if (keyStates[sf::Keyboard::D]) {
				cPosition.x += sin((cRotation[2] + 90) * (M_PI / 180)) * deltaSpeed;
				cPosition.z += cos((cRotation[2] + 90) * (M_PI / 180)) * deltaSpeed;
			}
			if (keyStates[sf::Keyboard::LShift]) {
				cPosition.y += deltaSpeed;
			}
			if (keyStates[sf::Keyboard::Space]) {
				cPosition.y -= deltaSpeed;
			}
			if (keyStates[sf::Keyboard::Up]) {
				if (cRotation[0] >= -60) {
					cRotation[0] = std::fmod(cRotation[0] - deltaSensitivity, 360);
				}
			}	
			if (keyStates[sf::Keyboard::Down]) {
				if (cRotation[0] <= 75) {
					cRotation[0] = std::fmod(cRotation[0] + deltaSensitivity, 360);
				}
			}
			if (keyStates[sf::Keyboard::Left]) {
				cRotation[2] = std::fmod(cRotation[2] - deltaSensitivity, 360);
			}
			if (keyStates[sf::Keyboard::Right]) {
				cRotation[2] = std::fmod(cRotation[2] + deltaSensitivity, 360);
			}
		}

		//std::cout << cRotation[0] << std::endl;

		window.clear();

		makeGrid(window, 1);

		frameCount++;

		if (clock.getElapsedTime().asSeconds() >= 1.0f) {
			fps = frameCount;
			frameCount = 0;
			clock.restart();
		}

		std::stringstream ss;
		ss << "FPS: " << fps;
		fpsText.setString(ss.str());

		window.draw(fpsText);

		window.display();
	}

	return 0;
}

