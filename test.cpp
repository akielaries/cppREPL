#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <fstream>

// Simple tokenizer function
std::vector<std::string> tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// Simple symbol table to store variables
std::map<std::string, std::string> symbolTable;

int main() {
    std::cout << "C++ REPL - Type 'exit' to end\n";

    std::string cumulativeCode;  // To store cumulative code entered so far
    std::vector<std::string> includes;  // To store include statements
    bool mainIncluded = false;  // To track if the main function is already included

    while (true) {
        try {
            std::string input;
            std::cout << "> ";
            std::getline(std::cin, input);

            if (input == "exit") {
                break;
            }

            // Check for include statements
            if (input.find("#include") == 0) {
                includes.push_back(input);
            } else {
                cumulativeCode += input + '\n';  // Add the current line to cumulative code
            }

            // Execute the C++ code using std::system
            std::ofstream codeFile("temp_code.cpp");
            // Add includes to the beginning of the file
            for (const auto& include : includes) {
                codeFile << include << '\n';
            }
            // Add the cumulative code and main function if not included yet
            if (!mainIncluded) {
                codeFile << "int main() {\n" << cumulativeCode << "\nreturn 0;\n}";
                mainIncluded = true;
            } else {
                codeFile << cumulativeCode;
            }
            codeFile.close();

            // Build the command to compile the C++ code
            std::string command = "g++ -xc++ -o temp_executable temp_code.cpp";
            int result = std::system(command.c_str());

            // Check if compilation was successful
            if (result == 0) {
                // Execute the compiled code
                std::system("./temp_executable");
            } else {
                throw std::runtime_error("Compilation error");
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }

    std::cout << "Exiting C++ REPL\n";
    return 0;
}

