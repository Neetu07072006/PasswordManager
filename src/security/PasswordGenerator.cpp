#include "PasswordGenerator.h"
#include <openssl/rand.h>
#include <stdexcept>
#include <string>

std::string PasswordGenerator::generate(int length,bool uppercase,bool lowercase,bool numbers,bool symbols){
    if(length<=0)
        throw std::invalid_argument("Password length must be positive.");
    std::string characters;
    if(uppercase)
        characters+="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if(lowercase)
        characters+="abcdefghijklmnopqrstuvwxyz";
    if(numbers)
        characters+="0123456789";
    if(symbols)
        characters+="!@#$%^&*()-_=+[]{};:,.<>?";
    if(characters.empty())
        throw std::invalid_argument("At least one character set is required.");
    std::string password;
    password.reserve(length);
    unsigned char randomByte=0;
    const unsigned int limit=256-(256%characters.size());
    for(int i=0;i<length;i++){
        do{
            if(RAND_bytes(&randomByte,1)!=1)
                throw std::runtime_error("Cryptographically secure random generation failed.");
        }while(randomByte>=limit);
        password+=characters[randomByte%characters.size()];
    }
    return password;
}