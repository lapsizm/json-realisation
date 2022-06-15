#ifndef INCLUDE_JSON_H_
#define INCLUDE_JSON_H_

#include <ostream>
#include <istream>
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <functional>
#include <exception>

class Node;

using Dict = std::map<std::string, Node>;

using Array = std::vector<Node>;

using namespace std::string_literals;

class ParsingError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class Node final : private std::variant<std::nullptr_t, bool, int, double, std::string, Array, Dict> {
public:

    using variant::variant;

    using Value = variant;

    Node(Node::Value &value) {
        this->swap(value);
    }

    bool IsInt() const {
        return std::holds_alternative<int>(*this);
    }

    int AsInt() const {
        if (!IsInt()) {
            throw std::logic_error("Not an int");
        }
        return std::get<int>(*this);
    }

    bool IsPureDouble() const {
        return std::holds_alternative<double>(*this);
    };

    bool IsDouble() const {
        return IsPureDouble() || IsInt();
    }

    double AsDouble() const {
        using namespace std::literals;
        if (!IsDouble()) {
            throw std::logic_error("Not as double");
        }
        return IsPureDouble() ? std::get<double>(*this) : AsInt();
    }

    bool IsBool() const {
        return std::holds_alternative<bool>(*this);
    }

    bool AsBool() const {
        if (!IsBool()) {
            throw std::logic_error("Not a bool");
        }
        return std::get<bool>(*this);
    }

    bool IsNull() const {
        return std::holds_alternative<nullptr_t>(*this);
    }

    bool IsArray() const {
        return std::holds_alternative<Array>(*this);
    }

    const Array & AsArray() const {
        using namespace std::literals;
        if (!IsArray()) {
            throw std::logic_error("Not an array");
        }
        return std::get<Array>(*this);
    }

    Array &GiveArray() {
        using namespace std::literals;
        if (!IsArray()) {
            throw std::logic_error("Not an array");
        }
        return std::get<Array>(*this);
    }

    bool IsString() const {
        return std::holds_alternative<std::string>(*this);
    }

    const std::string &AsString() {
        using namespace std::literals;
        if (!IsString()) {
            throw std::logic_error("Not a string");
        }
        return std::get<std::string>(*this);
    }

    std::string &GiveString() {
        using namespace std::literals;
        if (!IsString()) {
            throw std::logic_error("Not an array");
        }
        return std::get<std::string>(*this);
    }

    bool IsDict() const {
        return std::holds_alternative<Dict>(*this);
    }

    const Dict & AsDict() const {
        using namespace std::literals;
        if (!IsDict()) {
            throw std::logic_error("Not a dict");
        }
        return std::get<Dict>(*this);
    }

    Dict &GiveDict() {
        using namespace std::literals;
        if (!IsDict()) {
            throw std::logic_error("Not an array");
        }
        return std::get<Dict>(*this);
    }

    const Value &GetValue() const {
        return *this;
    }
};

Node LoadNode(std::istream &input);

Node LoadString(std::istream &input){
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true){
        if(it == end){
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if(ch == '"'){
            ++it;
            break;
        }
        else if(ch == '\\'){
            ++it;
            if(it == end){
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *it;
            switch (escaped_char) {
                case 'n':
                    s.push_back('\n');
                    break;
                case 't':
                    s.push_back('\t');
                    break;
                case 'r':
                    s.push_back('\r');
                    break;
                case '"':
                    s.push_back('"');
                    break;
                case '\\':
                    s.push_back('\\');
                    break;
                default:
                    throw ParsingError("Unrecognized escape sequence \""s + escaped_char + "\""s);
            }
        } else if (ch == '\n' || ch == '\r') throw ParsingError("Unexpected end of line"s);
        else s.push_back(ch);
        ++it;
    }
    return Node(std::move(s));
}

std::string LoadLiteral(std::istream &input) {
    std::string s;
    while (std::isalpha(input.peek())) {
        s.push_back(static_cast<char>(input.get()));
    }
    return s;
}

Node LoadArray(std::istream &input){
    std::vector<Node> result;
    for(char c; input >> c && c != ']';){
        if(c != ','){
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }
    if(!input){
        throw ParsingError("Array parsing error");
    }
    return Node(std::move(result));
}

Node LoadDict(std::istream &input){
    Dict dict;
    for(char c; input >> c && c != '}';){
        if(c == '"'){
            std::string key = LoadString(input).AsString();
            if(dict.find(key) == dict.end()){
                if(input >> c && c == ':') {
                    dict.emplace(std::move(key), LoadNode(input));
                } else throw ParsingError(": was expected but '"s + c + "' has been found");/// ??????????????
            } else {
                throw ParsingError("Duplicate key error "s + key + " have been found");
            }
        }
        else if(c != ','){
            throw ParsingError(R"(',' is expected but ')"s + c + R"(' was found)");

        }
    }
    if(!input){
        throw ParsingError("Dict parsing error");
    }
    return Node(std::move(dict));
}

Node LoadBool(std::istream &input){
    const auto s = LoadLiteral(input);
    if(s == "true"s){
        return Node{true};
    } else if (s == "false"s){
        return Node{false};
    } else throw ParsingError("Failed to parse '"s + s + "' as bool");

}

Node LoadNull(std::istream &input){
    if(auto l = LoadLiteral(input); l == "null"s){
        return Node{nullptr};
    } else {
        throw ParsingError("Failed to parse '"s + l + "' as null"s);
    }
}

Node LoadNumber(std::istream &input){
    std::string parsed_num;
    auto read_char = [&parsed_num, &input] {
        parsed_num += (static_cast<char>(input.get()));
        if(!input) throw ParsingError("Failed to read number");
    };
    auto read_digit = [&input, read_char]{
        if(!std::isdigit(static_cast<unsigned char>(input.peek()))) throw ParsingError("failed to read number from stream");
        while(std::isdigit(input.peek())){
            read_char();
        }
    };
    if(input.peek() == '-'){
        read_char();
    }
    if(input.peek() == '0'){
        read_char();
    } else read_digit();
    bool is_int = true;
    if(input.peek() == '.'){
        read_char();
        read_digit();
        is_int = false;
    }
    if(int ch = input.peek(); ch == 'e' || ch == 'E'){
        read_char();
        if(int ch_ = input.peek(); ch_ == '+' || ch_ == '-'){
            read_char();
        }
        read_digit();
        is_int = false;
    }

    if(is_int){
        return std::stoi(parsed_num);
    }else {
        return std::stod(parsed_num);
    }
}

Node LoadNode(std::istream &input){
    char c;
    if(!input) {
        throw ParsingError("Error unexpected EOF");
    }
    input >> c;
    switch (c){
        case '[':
            return LoadArray(input);
        case '{':
            return LoadDict(input);
        case '"':
            return LoadString(input);
        case 't':
            [[fallthrough]];
        case 'f':
            input.putback(c);
            return LoadBool(input);
        case 'n':
            input.putback(c);
            return LoadNull(input);
        default:
            input.putback(c);
            return LoadNumber(input);
    }
}

    struct PrintContext {
        std::ostream &out;

        int indent_step = 4;
        int indent = 0;

        void PrintIdent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        PrintContext Indented() const {
            return {out, indent_step, indent};
        }
    };


    class Document {
    public:
        explicit Document(Node root) : root_(root) {}
        const Node &GetRoot() const {
            return root_;
        }

    private:
        Node root_;


    };

    Document Load(std::istream &input){
        return Document(LoadNode(input));
    }

template<typename Value>
void PrintValue(const Value &value, const PrintContext &ctx) {
    ctx.out << value;
}

void PrintString(const std::string &value, std::ostream &out) {
    using namespace std;
    out.put('"');
    for (const char c: value) {
        switch (c) {
            case '\r' :
                out << "\\r"s;
                break;
            case '\n' :
                out << "\\n"s;
                break;
            case '"' :
                [[fallthrough]];
            case '\\':
                out.put('\\');
                [[fallthrough]];
            default:
                out << c;
                break;
        }
    }
    out.put('"');
}

void PrintNode(const Node &node, const PrintContext &ctx);

template<>
void PrintValue<std::string>(const std::string &value, const PrintContext &ctx) {
    PrintString(value, ctx.out);
}

template<>
void PrintValue<std::nullptr_t>([[
maybe_unused]]const std::nullptr_t &value, const PrintContext &ctx) {
    ctx.out << "null"s;
}

template<>
void PrintValue<bool>(const bool &value, const PrintContext &ctx) {
    ctx.out << (value ? "true"s : "false"s);
}

template<>
void PrintValue<Array>(const Array &nodes, const PrintContext &ctx) {
    std::ostream &out = ctx.out;
    out << "[\n"s;
    bool first = true;
    auto inner_ctx = ctx.Indented();
    for (const Node &node: nodes) {
        if (first) {
            first = false;
        } else {
            out << ",\n"s;
        }
        inner_ctx.PrintIdent();
        PrintNode(node, inner_ctx);
    }
    out.put('\n');
    ctx.PrintIdent();
    out.put(']');
}

template<>
void PrintValue<Dict>(const Dict &nodes, const PrintContext &ctx) {
    std::ostream &out = ctx.out;
    out << "{\n"s;
    bool first = true;
    auto inner_ctx = ctx.Indented();
    for (const auto &[key, node]: nodes) {
        if (first) {
            first = false;
        } else out << ",\n"s;
        inner_ctx.PrintIdent();
        PrintString(key, ctx.out);
        out << ": "s;
        PrintNode(node, inner_ctx);
    }
    ctx.PrintIdent();
    out << "\n}"s;
}

void PrintNode(const Node &node, const PrintContext &ctx) {
    std::visit([&ctx](const auto &value) {
                   PrintValue(value, ctx);
               }, node.GetValue()
    );
}

void Print(const Document &doc, std::ostream &output) {
    PrintNode(doc.GetRoot(), PrintContext {output});
}


#endif // INCLUDE_JSON_H_
