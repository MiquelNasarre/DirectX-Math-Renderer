#include "ImageFunctions.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

typedef unsigned int UINT;

Image gradient()
{
	Image image(2560, 1440, Color::Black);

	for (int i = 0; i < 2560; i++) {
		for (int j = 0; j < 1440; j++)
			image(j, i) = Color(unsigned char(256 * (i / 2560.f)), unsigned char(256 * (j / 1440.f)), 0);
	}
	return image;
}

Image EarthDiagram()
{
	UINT width = 1280;
	UINT height = 720;
	UINT lines = 10;

	Image image(width, height + 1, Color::Gray);

	for (UINT i = 0; i < height; i++) {
		for (UINT j = 0; j < width; j++) {
			if (!(i % (height/lines)) || !(j % (width/lines)))
				image(i, j) = Color::Black;
		}
	}
	return image;
}

Image createNightSky(unsigned int width, unsigned int height)
{
	Image image(width, height);
	srand(clock());

	//	Stars

	for (int n = 0; n < 100000; n++) {

		UINT i = rand() % image.height();
		UINT j = rand() % image.width();

		image(i, j) = Color(rand() % 255, rand() % 255, rand() % 255) + Color::Gray - Color(rand() % 255, rand() % 255, rand() % 255);

	}

	for (int n = 0; n < 10000; n++) {

		UINT i = rand() % image.height();
		UINT j = rand() % image.width();

		for (int m = 0; m < 10; m++)
			image(i + (1 - 2 * (rand() % 2)) * (rand() % (rand() % 3 + 1)), j + (1 - 2 * (rand() % 2)) * (rand() % (rand() % 3 + 1)) = Color(rand() % 255, rand() % 255, rand() % 255) + Color::Gray - Color(rand() % 255, rand() % 255, rand() % 255);
	}

	for (int n = 0; n < 500; n++) {

		UINT i = rand() % image.height();
		UINT j = rand() % image.width();

		for (int m = 0; m < 15; m++)
			image(i + (1 - 2 * (rand() % 2)) * (rand() % (rand() % 4 + 1)), j + (1 - 2 * (rand() % 2)) * (rand() % (rand() % 4 + 1)) = Color(rand() % 255, rand() % 255, rand() % 255) + Color::Gray - Color(rand() % 255, rand() % 255, rand() % 255);
	}

	for (int n = 0; n < 250; n++) {

		UINT i = rand() % image.height();
		UINT j = rand() % image.width();

		for (int m = 0; m < 100; m++)
			image(i + (1 - 2 * (rand() % 2)) * (rand() % (rand() % 7 + 1)), j + (1 - 2 * (rand() % 2)) * (rand() % (rand() % 7 + 1)) = Color::White - Color(rand() % 255, rand() % 255, rand() % 255);
	}

	for (int n = 0; n < 150; n++) {

		UINT i = rand() % image.height();
		UINT j = rand() % image.width();

		for (int m = 0; m < 150; m++)
			image(i + (1 - 2 * (rand() % 2)) * (rand() % (rand() % 10 + 1)), j + (1 - 2 * (rand() % 2)) * (rand() % (rand() % 10 + 1)) = Color::White - Color(rand() % 255, rand() % 255, rand() % 255);
	}

	for (UINT i = 0; i < image.height(); i++) {
		for (UINT j = 0; j < image.width(); j++) {

			if (image(i, j) != Color::Transparent)
				image(i, j) = (image(i, j) / (rand() % 10 + 1) + Color::White / (rand() % 20 + 1)) / (rand() % 3 + 2.f);



		}
	}

	//	Galaxies

	Image galaxy("nightsky/MlkyWay.jpg");

	for (UINT j = image.width() * 4 / 8; j < image.width() * 7 / 8; j++) {
		UINT i = UINT(image.height() * (-cosf(2 * 3.14159f * j / image.width())) / 2.f) / 2 + image.height() / 2;

		for (int n = 0; n < (sqrt(j * (11000 - j)) - 5200) / 2; n++) {
			UINT ir = i + (2 * (rand() % 2) - 1) * (rand() % (rand() % 500 + 1));
			UINT jr = j + (2 * (rand() % 2) - 1) * (rand() % (rand() % 500 + 1));
			int ig = int(1.5f * (ir - image.height() / 4.f) * galaxy.height() / image.height());
			int jg = int(1.5f * (jr - image.width() / 4.f) * galaxy.width() / image.width());

			if (ig < 0 || ig >= (int)galaxy.height() || jg < 0 || jg >= (int)galaxy.width()) {
				ig = rand() % galaxy.height();
				jg = rand() % galaxy.width();
			}
			image(ir, jr) = galaxy.pixels()[ig * galaxy.width() + jg] / 3;
		}

	}




	//	Return

	return image;
}

Image createAzimuth(const Image mercator, const unsigned int height, bool KeepAllStars)
{
	Image image(2 * height, height);

	const UINT radius = height / 4;
	const UINT centerx0 = height / 2;
	const UINT centerx1 = 3 * height / 2;
	const UINT centery = height / 2;
	for (UINT i = 0; i < mercator.height; i++) {
		for (UINT j = 0; j < mercator.width; j++) {
			double theta = 2. * 3.1415926535 * double(j) / mercator.width;
			double phi = 3.1415926535 / 2. - 3.1415926535 * double(i) / mercator.height;

			double x, y;
			UINT Fi, Fj;
			if (phi >= 0) {
				x = cos(phi) * cos(theta) * (2 - sin(phi));
				y = cos(phi) * sin(theta) * (2 - sin(phi));
				Fi = centery - UINT(y * radius);
				Fj = centerx0 + UINT(x * radius);
			}
			if (phi < 0) {
				x = cos(phi) * cos(theta) * (2 + sin(phi));
				y = cos(phi) * sin(theta) * (2 + sin(phi));
				Fi = centery - UINT(y * radius);
				Fj = centerx1 + UINT(x * radius);
			}
			if (!KeepAllStars || image.Pixels[_pos(Fi, Fj)] == Color::Transparent || image.Pixels[_pos(Fi, Fj)] == Color::Black)
				image.Pixels[_pos(Fi, Fj)] = mercator.Pixels[i * mercator.width + j];
		}
	}

	//	Blend dark spots

	std::vector<Color> mesh;

	for (int times = 1; times < 30; times++) {
		for (UINT i = 0; i < image.height / 2; i++) {
			for (UINT j = 0; j < image.width; j++) {
				if (image.Pixels[_pos(i, j)] != Color::Transparent)
					continue;

				else if ((j < height && (i - centery) * (i - centery) + (j - centerx0) * (j - centerx0) <= centery * (centery + 120)) || (j >= height && (i - centery) * (i - centery) + (j - centerx1) * (j - centerx1) <= centery * (centery + 120))) {
					mesh.clear();

					if (image.Pixels[_pos(i - 1, j)] != Color::Transparent)
						mesh.push_back(image.Pixels[_pos(i - 1, j)]);
					if (image.Pixels[_pos(i + 1, j)] != Color::Transparent)
						mesh.push_back(image.Pixels[_pos(i + 1, j)]);
					if (image.Pixels[_pos(i, j + 1)] != Color::Transparent)
						mesh.push_back(image.Pixels[_pos(i, j + 1)]);
					if (image.Pixels[_pos(i, j - 1)] != Color::Transparent)
						mesh.push_back(image.Pixels[_pos(i, j - 1)]);

					if (mesh.size()) {
						for (Color& c : mesh)
							image.Pixels[_pos(i, j)] += c / (int)mesh.size();
					}

				}
			}
		}
	}
	image = topDown(image);
	for (int times = 1; times < 30; times++) {
		for (UINT i = 0; i < image.height / 2; i++) {
			for (UINT j = 0; j < image.width; j++) {
				if (image.Pixels[_pos(i, j)] != Color::Transparent)
					continue;

				else if ((j < height && (i - centery) * (i - centery) + (j - centerx0) * (j - centerx0) <= centery * (centery + 120)) || (j >= height && (i - centery) * (i - centery) + (j - centerx1) * (j - centerx1) <= centery * (centery + 120))) {
					mesh.clear();

					if (image.Pixels[_pos(i - 1, j)] != Color::Transparent)
						mesh.push_back(image.Pixels[_pos(i - 1, j)]);
					if (image.Pixels[_pos(i + 1, j)] != Color::Transparent)
						mesh.push_back(image.Pixels[_pos(i + 1, j)]);
					if (image.Pixels[_pos(i, j + 1)] != Color::Transparent)
						mesh.push_back(image.Pixels[_pos(i, j + 1)]);
					if (image.Pixels[_pos(i, j - 1)] != Color::Transparent)
						mesh.push_back(image.Pixels[_pos(i, j - 1)]);

					if (mesh.size()) {
						for (Color& c : mesh)
							image.Pixels[_pos(i, j)] += c / (int)mesh.size();
					}

				}
			}
		}
	}

	return topDown(image);
}

Image chalk(const Image image)
{
	Image whiteBoard(image.width, image.height);

	for (UINT i = 0; i < image.height; i++) {
		for (UINT j = 0; j < image.width; j++) {

			int dR = abs((int)image.Pixels[_pos(i, j - 1)].R - (int)image.Pixels[_pos(i, j + 1)].R);
			int dG = abs((int)image.Pixels[_pos(i, j - 1)].G - (int)image.Pixels[_pos(i, j + 1)].G);
			int dB = abs((int)image.Pixels[_pos(i, j - 1)].B - (int)image.Pixels[_pos(i, j + 1)].B);

			if (dR > 50 || dG > 50 || dB > 50)
				whiteBoard.Pixels[_pos(i, j)] = Color::White;
			else if (dR > 40 || dG > 40 || dB > 40)
				whiteBoard.Pixels[_pos(i, j)] = Color::White / 1.5f;
			else if (dR > 30 || dG > 30 || dB > 30)
				whiteBoard.Pixels[_pos(i, j)] = Color::White / 2.5f;
			else if (dR > 20 || dG > 20 || dB > 20)
				whiteBoard.Pixels[_pos(i, j)] = Color::White / 4.f;
			else if (dR > 10 || dG > 10 || dB > 10)
				whiteBoard.Pixels[_pos(i, j)] = Color::White / 6.f;
		}
	}
	return whiteBoard;
}

Image topDown(const Image image)
{
	Image topdown(image.width, image.height);

	for (UINT i = 0; i < image.height; i++) {
		for (UINT j = 0; j < image.width; j++) {
			topdown.Pixels[_pos(i, j)] = image.Pixels[_pos(image.height - 1 - i, j)];
		}
	}
	return topdown;
}

Image invert(const Image image)
{
	Image inverted(image.width, image.height);

	for (unsigned int i = 0; i < image.height; i++) {
		for (unsigned int j = 0; j < image.width; j++) {
			inverted.Pixels[_pos(i, j)] = -image.Pixels[_pos(i, j)];
		}
	}
	return inverted;
}