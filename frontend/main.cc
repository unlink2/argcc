#include "../src/argcc.h"
#include <iostream>

int main(int argc, char **argv) {
    liblc::Argparse parser("Sample parser");

    // string input
    parser.addArgument("-test", liblc::STRING, 1, "Input a string", "--t");

    // 2 integers
    parser.addArgument("-int", liblc::NUMBER, 2, "Input 2 integers", "--i");

    // unique integer
    parser.addArgument("-unique", liblc::NUMBER, 1, "Input 1 integer", "--ui", true);

    // flag
    parser.addArgument("-flag", liblc::BOOLEAN, 0, "Set or default to false", "--f");

    // required flag
    parser.addArgument("-required", liblc::BOOLEAN, 0, "Set or default to false. Required", "--req", false, true);

    // consume remainder
    parser.addConsumer("consumer", liblc::STRING, "remainder");

    try {
        auto parsed = parser.parse(argc, argv);

        // access arguments
        if (parsed.containsAny("-test")) {
            auto testValue = parsed.toString("-test");
            std::cout << "TestValue: " << testValue << std::endl;
        }
        if (parsed.containsAny("-int")) {
            auto intValue1 = parsed.toNumber("-int");
            auto intValue2 = parsed.toNumber("-int", 1);
            auto intOutOfBounds = parsed.toNumber("-int", 2, 64); // default value
            std::cout << "-int size: " << parsed.getSize("-int") << " Number1: " << intValue1 << " Number2: " << intValue2
                << " Out of bounds value: " << intOutOfBounds << std::endl;
        }
    } catch (liblc::ArgparseInsufficientArguments &e) {
        std::cerr << "Insufficient arguments!" << std::endl;
    } catch (liblc::ArgparseInvalidArgument &e) {
        // also caused by unique being used twice
        std::cerr << "Invalid argument" << std::endl;
    } catch (liblc::ArgparseMissingArgument &e) {
        // caused by missing required arugment
        std::cerr << "Missing required argument" << std::endl;
    }

    return 0;
}
