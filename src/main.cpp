//
// Created by mafn on 11/9/17.
//

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

#include "Interpreter.hpp"

struct CLI_Args
{
    static constexpr size_t TARGET_FILE_NAME_INDEX = 0;
    std::string target_filename = "";

    struct
    {
        bool debug = false;
    } flags;
};

std::vector<std::string> get_args(int argc, char** argv)
{
    size_t n_args = size_t(argc - 1);
    std::vector<std::string> args;
    args.reserve(n_args);

    for (size_t i = 0; i < n_args; i++) {
        args.push_back(argv[i + 1]); // We start with index one since we don't care about the first arg (invoking command)
    }

    return args;
}

CLI_Args parse_args(int argc, char** argv)
{
    CLI_Args parsed_args {};

    if (argc < 2) {
        // Nothing to do
        return parsed_args;
    }

    std::vector<std::string> args = get_args(argc, argv);

    // Scan for flags and remove them. This is done by using remove_if with a side effect
    std::remove_if(args.begin(), args.end(),
        [&parsed_args] (const std::string& arg) {
             if (arg == "--debug") {
                 parsed_args.flags.debug = true;
                 return true;
             }

            return false;
        });

    parsed_args.target_filename = args[CLI_Args::TARGET_FILE_NAME_INDEX];

    return parsed_args;
}

int main(int argc, char** argv)
{
    const CLI_Args args = parse_args(argc, argv);

    std::cout << "HONK Interpreter";
    if (args.flags.debug) {
        std::cout << " - Debug mode";
    }
    std::cout << "\n";

    Honk::Interpreter interpreter;

    if (args.target_filename != "") {
        std::cout << "Source file = " << args.target_filename << "\n";
        interpreter.run_from_file(args.target_filename);
    } else {
        std::cout << "REPL\n";
        interpreter.run_repl();
    }

    return 0;
}