#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "../include/lexer.hpp"

static std::string ReadFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: could not open file '" << path << "'\n";
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Error: invalid arguments provided\n"
            << "Usage: " << argv[0] << " <file.jwl>\n";
        return EXIT_FAILURE;
    }

    std::string path = argv[1];
    std::string source = ReadFile(path);
    if (source.empty()) return EXIT_FAILURE;

    Jewel::Lexer lexer(path, source);
    std::vector<Jewel::Token> tokens = lexer.Tokenize();
    if (!tokens.empty() && tokens.back().Type == Jewel::ILLEGAL)
        return EXIT_FAILURE;

    for (const Jewel::Token& token : tokens)
        std::cout << "Token { Type: " << token.Type  << ", " <<
            "Lexeme: " << token.Lexeme << ", Line: " <<
            token.Line << ", Column: " << token.Column << " }\n";

    return EXIT_SUCCESS;
}