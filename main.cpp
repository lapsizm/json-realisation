#include <iostream>
#include "json.h"

int main() {
    Document doc = Load(std::cin);
    Print(doc, std::cout);
}
