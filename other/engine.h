#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <sstream>

class Renderer {
public:
	Renderer(int width, int height)
		: window(sf::VideoMode(width, height), "3D Renderer"), 
		FOV(60), 
		focal(0),
		width(width),
		height(height),
		frameCount(0),
		fps(0)
	{
		focal = calcFocalLength(FOV);
		cRotation[0] = cRotation[1] = cRotation[2] = 0;
		cPosition[0] = cPosition[1] = cPosition[2] = 0;
	}

	void clear() {
		window.clear();
	}

	float calcFocalLength(float FOVdeg) {
		float FOVrad = FOVdeg * (M_PI / 180.0f);
		return 1.0f / tan(FOVrad / 2.0f);
	}

	void updateFPS() {
		if (clock.getElapsedTime().asSeconds() >= 1.0f) {
			fps = frameCount;
			frameCount = 0;
			clock.restart();
		}	
	}

	void tick() {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();

		frameCount++;
		
		updateFPS();
		
		std::stringstream ss;
		ss << "FPS: " << fps;
		fpsText.setString(ss.str());

		window.draw(fpsText);
		
		window.display();
	}
private:
	sf::RenderWindow window;
	sf::Clock clock;
	int frameCount;
	float fps;
	float FOV;
	float focal;
	float cRotation[3];
	float cPosition[3];
	float width;
	float height;
	sf::Vector3f shapes[];

	sf::Font font;
	if (!font.loadFromFile("jb.ttf")) {
		return -1;
	}
	sf::Text fpsText;
	fpsText.setFont(font);
	fpsText.setCharacterSize(15);
	fpsText.setFillColor(sf::Color::White);

	sf::Vector2f translate(sf::Vector3f point) {
		sf::Vector2f npoint;
		npoint.x = (point.x * focal) / point.z + (width / 2);
		npoint.y = (point.y * focal) / point.z + (height / 2);
		return npoint;
	}

};

#endif

