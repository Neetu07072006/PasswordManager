#include "VaultStorage.h"
#include "../security/KeyDerivation.h"
#include "../security/CryptoManager.h"
#include <fstream>
#include <sstream>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <cstdio>

namespace{
void writeUint32(std::ofstream& out,uint32_t value){out.write(reinterpret_cast<const char*>(&value),sizeof(value));}
void writeUint64(std::ofstream& out,uint64_t value){out.write(reinterpret_cast<const char*>(&value),sizeof(value));}

uint32_t readUint32(std::ifstream& in){
    uint32_t value=0;
    in.read(reinterpret_cast<char*>(&value),sizeof(value));
    if(!in)throw std::runtime_error("Invalid vault header.");
    return value;
}

uint64_t readUint64(std::ifstream& in){
    uint64_t value=0;
    in.read(reinterpret_cast<char*>(&value),sizeof(value));
    if(!in)throw std::runtime_error("Invalid vault header.");
    return value;
}

void writeString(std::ostringstream& out,const std::string& value){
    uint64_t size=value.size();
    out.write(reinterpret_cast<const char*>(&size),sizeof(size));
    out.write(value.data(),static_cast<std::streamsize>(size));
}

std::string readString(std::istringstream& in){
    uint64_t size=0;
    in.read(reinterpret_cast<char*>(&size),sizeof(size));
    if(!in||size>10000000)throw std::runtime_error("Invalid vault data.");
    std::string value(static_cast<size_t>(size),'\0');
    if(size>0)
        in.read(value.data(),static_cast<std::streamsize>(size));
    if(!in)throw std::runtime_error("Invalid vault data.");
    return value;
}
}

VaultStorage::VaultStorage(const std::string& filename):filename(filename){}

std::string VaultStorage::serialize(const std::vector<PasswordEntry>& entries) const{
    std::ostringstream out(std::ios::binary);
    uint64_t count=entries.size();
    out.write(reinterpret_cast<const char*>(&count),sizeof(count));
    for(const auto& entry:entries){
        int32_t id=entry.getId();
        out.write(reinterpret_cast<const char*>(&id),sizeof(id));
        writeString(out,entry.getWebsite());
        writeString(out,entry.getUsername());
        writeString(out,entry.getPassword());
        writeString(out,entry.getCategory());
        writeString(out,entry.getNotes());
    }
    return out.str();
}

std::vector<PasswordEntry> VaultStorage::deserialize(const std::string& data) const{
    std::istringstream in(data,std::ios::binary);
    uint64_t count=0;
    in.read(reinterpret_cast<char*>(&count),sizeof(count));
    if(!in||count>100000)throw std::runtime_error("Invalid entry count.");

    std::vector<PasswordEntry> entries;
    entries.reserve(static_cast<size_t>(count));

    for(uint64_t i=0;i<count;i++){
        int32_t id=0;
        in.read(reinterpret_cast<char*>(&id),sizeof(id));
        if(!in)throw std::runtime_error("Invalid entry data.");
        entries.emplace_back(
            id,
            readString(in),
            readString(in),
            readString(in),
            readString(in),
            readString(in)
        );
    }

    if(in.peek()!=std::char_traits<char>::eof())
        throw std::runtime_error("Unexpected data after vault entries.");

    return entries;
}

bool VaultStorage::save(const std::vector<PasswordEntry>& entries,const std::string& masterPassword) const{
    const std::string tempFile=filename+".tmp";
    const std::string backupFile=filename+".bak";

    try{
        std::vector<unsigned char> salt=KeyDerivation::generateSalt(16);
        std::vector<unsigned char> key=KeyDerivation::deriveKey(masterPassword,salt);
        EncryptedData encrypted=CryptoManager::encrypt(serialize(entries),key);

        std::ofstream out(tempFile,std::ios::binary|std::ios::trunc);
        if(!out)return false;

        const char magic[]="PMV2";
        out.write(magic,4);
        writeUint32(out,static_cast<uint32_t>(salt.size()));
        writeUint32(out,static_cast<uint32_t>(encrypted.nonce.size()));
        writeUint32(out,static_cast<uint32_t>(encrypted.tag.size()));
        writeUint64(out,static_cast<uint64_t>(encrypted.ciphertext.size()));

        out.write(reinterpret_cast<const char*>(salt.data()),static_cast<std::streamsize>(salt.size()));
        out.write(reinterpret_cast<const char*>(encrypted.nonce.data()),static_cast<std::streamsize>(encrypted.nonce.size()));
        out.write(reinterpret_cast<const char*>(encrypted.tag.data()),static_cast<std::streamsize>(encrypted.tag.size()));
        out.write(reinterpret_cast<const char*>(encrypted.ciphertext.data()),static_cast<std::streamsize>(encrypted.ciphertext.size()));

        out.flush();

        if(!out){
            out.close();
            std::remove(tempFile.c_str());
            return false;
        }

        out.close();

        std::ifstream verify(tempFile,std::ios::binary|std::ios::ate);
        if(!verify||verify.tellg()<=0){
            verify.close();
            std::remove(tempFile.c_str());
            return false;
        }
        verify.close();

        std::remove(backupFile.c_str());

        if(std::rename(filename.c_str(),backupFile.c_str())!=0){
            if(std::ifstream(filename).good()){
                std::remove(tempFile.c_str());
                return false;
            }
        }

        if(std::rename(tempFile.c_str(),filename.c_str())!=0){
            std::rename(backupFile.c_str(),filename.c_str());
            std::remove(tempFile.c_str());
            return false;
        }

        return true;
    }catch(...){
        std::remove(tempFile.c_str());
        return false;
    }
}

bool VaultStorage::load(std::vector<PasswordEntry>& entries,const std::string& masterPassword) const{
    auto loadFile=[&](const std::string& path,std::vector<PasswordEntry>& result)->bool{
        try{
            std::ifstream in(path,std::ios::binary|std::ios::ate);
            if(!in)return false;

            std::streamsize fileSize=in.tellg();
            if(fileSize<24||fileSize>100000100)
                return false;

            in.seekg(0,std::ios::beg);

            char magic[4];
            in.read(magic,4);
            if(!in)return false;

            std::string version(magic,4);

            if(version=="PMV1"){
                uint32_t saltSize=readUint32(in);
                uint32_t nonceSize=readUint32(in);
                uint32_t tagSize=readUint32(in);
                uint64_t ciphertextSize=readUint64(in);

                if(saltSize<16||saltSize>64||nonceSize!=12||tagSize!=16||ciphertextSize>100000000)
                    return false;

                uint64_t expectedSize=4+4+4+4+8+saltSize+nonceSize+tagSize+ciphertextSize;
                if(expectedSize!=static_cast<uint64_t>(fileSize))
                    return false;

                std::vector<unsigned char> salt(saltSize);
                std::vector<unsigned char> nonce(nonceSize);
                std::vector<unsigned char> tag(tagSize);
                std::vector<unsigned char> ciphertext(static_cast<size_t>(ciphertextSize));

                in.read(reinterpret_cast<char*>(salt.data()),static_cast<std::streamsize>(salt.size()));
                in.read(reinterpret_cast<char*>(nonce.data()),static_cast<std::streamsize>(nonce.size()));
                in.read(reinterpret_cast<char*>(tag.data()),static_cast<std::streamsize>(tag.size()));
                in.read(reinterpret_cast<char*>(ciphertext.data()),static_cast<std::streamsize>(ciphertext.size()));

                if(!in)return false;

                std::vector<unsigned char> key=KeyDerivation::deriveKey(masterPassword,salt);
                EncryptedData encrypted{nonce,ciphertext,tag};
                std::string plaintext=CryptoManager::decrypt(encrypted,key);

                std::istringstream oldData(plaintext);
                std::string line;
                std::vector<PasswordEntry> loaded;

                while(std::getline(oldData,line)){
                    if(line.empty())
                        continue;

                    std::stringstream row(line);
                    std::string id;
                    std::string website;
                    std::string username;
                    std::string password;
                    std::string category;
                    std::string notes;

                    if(!(std::getline(row,id,'|')&&
                         std::getline(row,website,'|')&&
                         std::getline(row,username,'|')&&
                         std::getline(row,password,'|')&&
                         std::getline(row,category,'|')&&
                         std::getline(row,notes,'|')))
                        return false;

                    try{
                        loaded.emplace_back(
                            std::stoi(id),
                            website,
                            username,
                            password,
                            category,
                            notes
                        );
                    }catch(...){
                        return false;
                    }
                }

                result=std::move(loaded);
                return true;
            }

            if(version!="PMV2")
                return false;

            uint32_t saltSize=readUint32(in);
            uint32_t nonceSize=readUint32(in);
            uint32_t tagSize=readUint32(in);
            uint64_t ciphertextSize=readUint64(in);

            if(saltSize<16||saltSize>64||nonceSize!=12||tagSize!=16||ciphertextSize>100000000)
                return false;

            uint64_t expectedSize=4+4+4+4+8+saltSize+nonceSize+tagSize+ciphertextSize;
            if(expectedSize!=static_cast<uint64_t>(fileSize))
                return false;

            std::vector<unsigned char> salt(saltSize);
            std::vector<unsigned char> nonce(nonceSize);
            std::vector<unsigned char> tag(tagSize);
            std::vector<unsigned char> ciphertext(static_cast<size_t>(ciphertextSize));

            in.read(reinterpret_cast<char*>(salt.data()),static_cast<std::streamsize>(salt.size()));
            in.read(reinterpret_cast<char*>(nonce.data()),static_cast<std::streamsize>(nonce.size()));
            in.read(reinterpret_cast<char*>(tag.data()),static_cast<std::streamsize>(tag.size()));
            in.read(reinterpret_cast<char*>(ciphertext.data()),static_cast<std::streamsize>(ciphertext.size()));

            if(!in)return false;

            std::vector<unsigned char> key=KeyDerivation::deriveKey(masterPassword,salt);
            EncryptedData encrypted{nonce,ciphertext,tag};
            std::string plaintext=CryptoManager::decrypt(encrypted,key);

            result=deserialize(plaintext);
            return true;
        }catch(...){
            return false;
        }
    };

    std::vector<PasswordEntry> loaded;

    if(loadFile(filename,loaded)){
        entries=std::move(loaded);
        return true;
    }

    loaded.clear();

    const std::string backupFile=filename+".bak";

    if(loadFile(backupFile,loaded)){
        entries=std::move(loaded);
        return true;
    }

    return false;
}

bool VaultStorage::exists() const{
    std::ifstream file(filename,std::ios::binary);
    return file.good();
}

std::string VaultStorage::getFilename() const{
    return filename;
}