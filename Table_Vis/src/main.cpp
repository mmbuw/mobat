#include "../include/Microphone_Object.h"

#include "../include/Structs.h"

#include <iostream>

int main(int argc, char** argv) {

TTT::Microphone_Object test_microphone_object;

sf::Vector2f mic_pos = test_microphone_object.Position();
std::cout << mic_pos.x;

return 0;
}
