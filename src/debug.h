#ifndef cj_debug
#define cj_debug

#include <d3dx9.h>
#include <fstream>
#include <vector>
#include <string>

class DEBUG
{
public:
    DEBUG();
    ~DEBUG();
    void Print(char c[]);
    const std::vector<std::string> &GetMessages() const;
    void ClearMessages();
    std::ofstream &operator<<(char c[]);
    std::ofstream &operator<<(int i);
    std::ofstream &operator<<(float f);
    std::ofstream &operator<<(bool b);
    std::ofstream &operator<<(D3DXVECTOR3 v);
    void Endl(int nr);

private:
    std::vector<std::string> messages;
    const size_t MAX_MESSAGES = 6;
};

extern DEBUG debug;

#endif