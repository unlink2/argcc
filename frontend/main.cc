#include "../src/argcc.h"
#include <iostream>

int main(int argc, char **argv) {
    argcc::Argparse parser("Sample parser");

    // string input
    parser.addArgument("-test", argcc::STRING, 1, "Input a string", "--t");

    // 2 integers
    parser.addArgument("-int", argcc::NUMBER, 2, "Input 2 integers", "--i");

    // unique integer
    parser.addArgument("-unique", argcc::NUMBER, 1, "Input 1 integer", "--ui", true);

    // flag
    parser.addArgument("-flag", argcc::BOOL, 0, "Set or default to false", "--f");

    // required flag
    parser.addArgument("-required", argcc::BOOL, 0, "Set or default to false. Required", "--req", false, true);

    // consume remainder
    parser.addConsumer("consumer", argcc::STRING, "remainder");

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
    } catch (argcc::ArgparseInsufficientArguments &e) {
        std::cerr << "Insufficient arguments!" << std::endl;
    } catch (argcc::ArgparseInvalidArgument &e) {
        // also caused by unique being used twice
        std::cerr << "Invalid argument" << std::endl;
    } catch (argcc::ArgparseMissingArgument &e) {
        // caused by missing required arugment
        std::cerr << "Missing required argument" << std::endl;
    }

    return 0;
}
