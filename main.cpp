#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <fstream>

struct Token {
    std::string type;  // "KEYWORD", "SHAPE", "IDENTIFIER", "EQUAL", "NUMBER", "COLOR"
    std::string value;
};

// Tokenizer
std::vector<Token> tokenize(const std::string& line) {
    std::vector<Token> tokens;
    std::istringstream stream(line);
    std::string word;

    while (stream >> word) {
        if (word == "DRAW") tokens.push_back({"KEYWORD", word});
        else if (word == "RECTANGLE" || word == "CIRCLE") tokens.push_back({"SHAPE", word});
        else if (word.find('=') != std::string::npos) {
            std::string key = word.substr(0, word.find('='));
            std::string val = word.substr(word.find('=') + 1);
            tokens.push_back({"IDENTIFIER", key});
            tokens.push_back({"VALUE", val});
        }
    }

    return tokens;
}

// SVG Generator
std::string generateSVG(const std::string& shape, const std::map<std::string, std::string>& params) {
    std::ostringstream svg;

    if (shape == "RECTANGLE") {
        svg << "<rect x=\"" << params.at("x")
            << "\" y=\"" << params.at("y")
            << "\" width=\"" << params.at("width")
            << "\" height=\"" << params.at("height")
            << "\" fill=\"" << params.at("color") << "\" />\n";
    } else if (shape == "CIRCLE") {
        svg << "<circle cx=\"" << params.at("x")
            << "\" cy=\"" << params.at("y")
            << "\" r=\"" << params.at("radius")
            << "\" fill=\"" << params.at("color") << "\" />\n";
    }

    return svg.str();
}

// Parser Class
class Parser {
private:
    std::vector<Token> tokens;
    int current;

    Token advance() {
        return tokens[current++];
    }

    Token peek() {
        return tokens[current];
    }

    bool match(const std::string& expectedType) {
        return current < tokens.size() && tokens[current].type == expectedType;
    }

public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

    std::pair<std::string, std::map<std::string, std::string>> parseCommand() {
        advance(); // consume "DRAW"

        std::string shape = advance().value;
        std::map<std::string, std::string> params;

        while (current < tokens.size()) {
            std::string key = advance().value;
            std::string val = advance().value;
            params[key] = val;
        }

        return {shape, params};
    }
};

// Main Block
int main() {
    std::ifstream inputFile("input.txt");
    std::ofstream svgFile("output.svg");

    if (!inputFile || !svgFile) {
        std::cerr << "Error opening files.\n";
        return 1;
    }

    std::string line;
    std::ostringstream svgContent;
    svgContent << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"800\" height=\"600\">\n";

    while (getline(inputFile, line)) {
        if (line.empty()) continue;
        auto tokens = tokenize(line);
        Parser parser(tokens);
        auto [shape, params] = parser.parseCommand();
        
        // Print parse tree to console
        std::cout << "<command>\n";
        std::cout << "├── shape: " << shape << "\n";
        for (auto& [key, val] : params) {
            std::cout << "├── " << key << ": " << val << "\n";
        }
        std::cout << "\n";
    
        svgContent << generateSVG(shape, params);
    }
    

    svgContent << "</svg>";
    svgFile << svgContent.str();

    std::cout << "SVG generated in output.svg ✅\n";
    return 0;
}
