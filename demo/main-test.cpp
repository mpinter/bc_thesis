#include "lvis.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <cstdlib>

std::ifstream input;
LightPath* current;

static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

void parseVertex(std::string s) {
	std::string first,second,third;
	unsigned int firstComma,secondComma,thirdComma;
	firstComma = s.find(",");
	first.assign(s,5,firstComma-5);
	secondComma = s.find(",",firstComma+1);
	second.assign(s,firstComma+2,secondComma-firstComma-2);
	thirdComma = s.find(",",secondComma+1);
	third.assign(s,secondComma+2,thirdComma-secondComma-3);
	current->pushVertex(new PathVertex(atof(first.c_str()),atof(second.c_str()),atof(third.c_str())));
}

void parseNext() {
	current->clear();
	std::string s;
	while ((std::getline(input,s))&&(s.find("Path[")==std::string::npos)) {
		s=trim(s);
		if ((s.at(0)=='p')&&(s.at(2)=='=')) {
			parseVertex(s);
		} 
	}
}

int main() {
	input.open("mlt_trimmed.txt"); //alternative - input.open("mlt_output.txt"); 
	if (!input.good()) std::cout << "Invalid pathtracer input file!\n";
	current = new LightPath();
	Lvis* elvis=new Lvis("./models/sponza/sponza.obj","./models/sponza/");
	//pretend we're actually generating paths - at least start from a random pre-generated
	int r=rand()%100000+1; //first parse stops at the occurence of the first path
	for (int i=0;i<r;i++) parseNext();
	elvis->run();
	elvis->setupVirtualCamera(glm::vec3(14.4499f,6.89415f,-1.18547f),glm::vec3(13.4563f,6.78438f,-1.15861f),glm::vec3(-0.109708, 0.993957, 0.00374357));
	elvis->setupLight(glm::vec3(0.0f,17.0f,0.0f));
	while (elvis->isRunning()) {
		current=new LightPath();
		/*the commented part won't work with current setup, but is nice to have it
		if (elvis->flagSelected) {
			delete current;
			current=elvis->getSelected();
		}*/
		parseNext();
		if (input.eof()) {
			std::cout << "Reached the end of input file, the main thread will now wait for visualisation to finish.";
			break;
		}
		if (!input.good()) {
			std::cout << "An error occured while reading the file, the main thread will now wait for visualisation to finish.";
			break;
		}
		current->finalize();
		if (elvis->flagSingle) {
			elvis->pushSinglePath(current);
			if (elvis->flagPause) elvis->wait();
			continue;
		}
		if (elvis->flagPause) {
			elvis->flagPause=false;
			elvis->wait();
		}
		elvis->pushPath(current);
	}
	elvis->join();
	input.close();
}
