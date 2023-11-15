#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <fstream>
#include <getopt.h>
#include <iomanip>
#include <cstdlib> // for std::exit
#include <termios.h>
#include <unistd.h>
#include <ncurses.h>
#include <ctime>
#include <cstdlib>
#include <sys/utsname.h>

#define VERSION "0.1.0+git" // VERSION

// Simple tokenizer function
std::vector<std::string> tokenize(const std::string &input) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    while (iss >> std::quoted(token)) {  // Handle quoted strings properly

        tokens.push_back(token);
    }
    return tokens;
}

// Simple symbol table to store variables
std::map<std::string, std::string> symbol_table;

// Function to handle user input and update state
void handle_input(std::string &cumulative_stdout, std::vector<std::string> &includes,
                     std::string &main_function, bool &inside_main) {
    std::string input;
    std::cout << ">>> ";
    std::getline(std::cin, input);

    if (input == "exit") {
        std::cout << "Exiting...\n";
        exit(EXIT_SUCCESS);
    }

    // Check for include statements
    if (input.find("#include") == 0) {
        includes.push_back(input);
    } else {
        // If not an include statement, add to the main function
        if (!inside_main) {
            main_function += "\n";
            inside_main = true;
        }
        cumulative_stdout += input + '\n';
    }
}

// Function to write code to a temporary file
void file_write(const std::vector<std::string> &includes, const std::string &main_function,
                     const std::string &cumulative_stdout) {
    std::ofstream temp_code_file("temp_code.cpp");
    // Add includes to the beginning of the file
    for (const auto &include : includes) {
        temp_code_file << include << '\n';
    }
    // Add the cumulative code and main function
    temp_code_file << main_function << cumulative_stdout;
    if (cumulative_stdout.find("{") != std::string::npos) {
        temp_code_file << "}\n";
    }
    temp_code_file << "\nreturn 0;\n}";
    temp_code_file.close();
}

// Function to compile and execute the C++ code
void execute() {
    // Build the command to compile the C++ code
    std::string command = "g++ -xc++ -o temp_executable temp_code.cpp";
    // Execute the C++ code using std::system
    int result = std::system(command.c_str());

    // Check if compilation was successful
    if (result == 0) {
        // Execute the compiled code
        std::system("./temp_executable");
    } else {
        throw std::runtime_error("Compilation error");
    }
}

// REPL function
void runREPL(const char *compiler_path) {
    std::cout << "cppREPL v" << VERSION << " - Type 'exit' to end session\n\n";

    // Display system information
    struct utsname sys_info;
    if (uname(&sys_info) == 0) {
        std::cout << "(" << sys_info.sysname << " " << sys_info.release << ", " << sys_info.machine << ") [" << sys_info.version << "] on " << sys_info.nodename << "\n";
    } else {
        std::cerr << "Error getting system information\n";
    }

    // Display compiler information
    if (compiler_path) {
        std::cout << "Custom compiler: " << compiler_path << "\n";
    } else {
        std::cout << "Default compiler: ";
#ifdef __GNUC__
        std::cout << "GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__;
#else
        std::cout << "Unknown compiler";
#endif
        std::cout << "\n";
    }

    // Display C++ standard version
    std::cout << "C++ standard version: " << __cplusplus << "\n";

//    std::cout << "cppREPL v" << VERSION << " [" << "]" << std::endl; 
//    std::cout << "Type 'exit' to end session\n\n";

    std::string cumulative_stdout; // To store cumulative code entered so far
    std::vector<std::string> includes; // To store include statements
    std::string main_function = "int main() {\n";
    bool inside_main = false;

    while (true) {
        try {
            handle_input(cumulative_stdout, includes, main_function, inside_main);
            file_write(includes, main_function, cumulative_stdout);
            execute();
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }

    std::cout << "Exiting C++ REPL\n";
}

void usage(const char *bin) {
    std::cout << "Usage: " << bin << "[-h] [-v] [-d] FILE [-c] PATH\n";
    std::cout << "  - FILE file name\n"
                 "  - PATH file path to compiler\n";
    std::cout << "\n";
}

void help() {
    std::cout << "\nAvailable options:\n"
            "   -h help\n"
            "   -v version\n"
            "   -d [FILE] dump to file\n"
            "   -c [PATH] compiler\n"
            "\n";
}

int main(int argc, char *argv[]) {
    int option;
    int d_flag = 1, c_flag = 1;
    // TODO: probably want to clean this up
    char filename[20];
    char compiler[120];

    while ((option = getopt(argc, argv, "hvd:c:")) != -1) {
        switch(option) {
            // help
            case 'h':
                help();
                exit(EXIT_SUCCESS);
            // version
            case 'v':
                std::cout << VERSION << std::endl;
                exit(EXIT_SUCCESS);
                break;

            // dump to file
            case 'd':
                d_flag = 0;
                // output file name
                snprintf(filename, sizeof(filename), "%s", optarg);

                std::cout << "Dumping to file: " << filename << "\n";
                break;

            // compiler
            case 'c':
                c_flag = 0;
                // get compiler filepath/name
                snprintf(compiler, sizeof(compiler), "%s", optarg);

                std::cout << "Using compiler: " << compiler << "\n";
                break;

            case '?':
                usage(argv[0]);
                exit(EXIT_FAILURE);
                break;
            
            default:
                usage(argv[0]);
                exit(EXIT_FAILURE);

        }
    }

    runREPL(c_flag ? nullptr : compiler);

    return 0;
}

