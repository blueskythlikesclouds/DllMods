#pragma once

#include <fstream>
#include <stack>

class XmlWriter
{
    std::ofstream stream;
    std::stack<std::string> elements;

    void indent();

    static std::string getFixedName(const std::string& name);

public:
    XmlWriter(const std::string& filePath);

    void writeBeginDocument();
    void writeBeginElement(const std::string& name);
    void writeEndElement();
    template<typename T>
    void writeElement(const std::string& name, T value);
    void writeComment(const std::string& comment);

    bool isOpen() const;
};

template <typename T>
void XmlWriter::writeElement(const std::string& name, T value)
{
    indent();

    const std::string fixedName = getFixedName(name);

    stream << "<" << fixedName << ">";
    if (std::is_same<T, bool>::value)
        stream << (value ? "true" : "false");
    else
        stream << value;

    stream << "</" << fixedName << ">" << std::endl;
}
