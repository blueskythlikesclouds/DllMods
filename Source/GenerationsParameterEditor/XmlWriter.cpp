#include "XmlWriter.h"

void XmlWriter::indent()
{
    for (size_t i = 0; i < elements.size(); i++)
        stream << "  ";
}

std::string XmlWriter::getFixedName(const std::string& name)
{
    std::string fixedName;
    fixedName.reserve(name.size());

    for (auto& character : name)
        fixedName.push_back(character != ' ' ? character : '_');

    return fixedName;
}

XmlWriter::XmlWriter(const std::string& filePath) : stream(filePath, std::ofstream::out)
{
}

void XmlWriter::writeBeginDocument()
{
    stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
}

void XmlWriter::writeBeginElement(const std::string& name)
{
    const std::string fixedName = getFixedName(name);

    indent();
    elements.push(fixedName);

    stream << "<" << fixedName << ">" << std::endl;
}

void XmlWriter::writeEndElement()
{
    const std::string name = elements.top();
    elements.pop();

    indent();
    stream << "</" << name << ">" << std::endl;
}

void XmlWriter::writeComment(const std::string& comment)
{
    indent();
    stream << "<!-- " << comment << " -->" << std::endl;
}

bool XmlWriter::isOpen() const
{
    return stream.is_open();
}
