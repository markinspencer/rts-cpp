#include "debug.h"

std::ofstream out("debug.txt");

DEBUG debug;

DEBUG::DEBUG()
{
}

DEBUG::~DEBUG()
{
    if (out.good())
        out.close();
}

void DEBUG::Print(char c[])
{
    messages.push_back(c);
    out << c << std::endl;

    if (messages.size() > MAX_MESSAGES)
        messages.erase(messages.begin());
}
std::ofstream &DEBUG::operator<<(char c[])
{
    out << c;
    return out;
}
std::ofstream &DEBUG::operator<<(int i)
{
    out << i;
    return out;
}
std::ofstream &DEBUG::operator<<(float f)
{
    out << f;
    return out;
}
std::ofstream &DEBUG::operator<<(bool b)
{
    if (b)
        out << "True";
    else
        out << "False";
    return out;
}
std::ofstream &DEBUG::operator<<(D3DXVECTOR3 v)
{
    out << "x: " << v.x << ", y: " << v.y << ", z: " << v.z;
    return out;
}
void DEBUG::Endl(int nr)
{
    for (int i = 0; i < nr; i++)
        out << std::endl;
}
const std::vector<std::string> &DEBUG::GetMessages() const
{
    return messages;
}

void DEBUG::ClearScreenMessages()
{
    messages.clear();
}
