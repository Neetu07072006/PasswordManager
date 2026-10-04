# Password Manager

A secure C++ desktop password manager with a Qt graphical interface, encrypted local storage, password generation, password strength analysis, automatic locking and clipboard protection.

## Features

- Master password protected vault
- AES-256-GCM authenticated encryption
- PBKDF2-SHA256 key derivation
- 600,000 PBKDF2 iterations
- Random cryptographic salt
- Random encryption nonce
- Tamper detection
- Encrypted backup vault
- Automatic vault recovery
- Secure password generator
- Password strength analyzer
- Add, edit and delete password entries
- Search by website, username and category
- Password visibility control
- Clipboard password copying
- Automatic clipboard clearing
- Automatic inactivity lock
- First-run vault creation
- Corrupted vault validation
- Qt graphical interface
- C++ data structures and algorithms

## Technologies

- C++17
- Qt 6
- OpenSSL
- CMake
- STL

## Security Architecture

The master password is never directly used as the AES encryption key.

The process is:

Master Password
        |
        v
PBKDF2-SHA256
        |
        v
256-bit Encryption Key
        |
        v
AES-256-GCM
        |
        v
Encrypted Vault

Each vault save generates a new random salt and encryption nonce.

AES-GCM also provides authentication, allowing the application to detect modified or corrupted encrypted data.

## Data Structure

The application uses:

- `std::vector` for password entries
- `std::unordered_map` for ID-based indexing
- Binary serialization for vault storage
- Qt widgets for the graphical interface

## Project Structure

```text
PasswordManager/
├── CMakeLists.txt
├── README.md
├── .gitignore
└── src/
    ├── main.cpp
    ├── core/
    │   ├── PasswordEntry.h
    │   ├── PasswordEntry.cpp
    │   ├── PasswordManager.h
    │   └── PasswordManager.cpp
    ├── security/
    │   ├── PasswordGenerator.h
    │   ├── PasswordGenerator.cpp
    │   ├── PasswordStrength.h
    │   ├── PasswordStrength.cpp
    │   ├── KeyDerivation.h
    │   ├── KeyDerivation.cpp
    │   ├── CryptoManager.h
    │   ├── CryptoManager.cpp
    │   ├── MasterPassword.h
    │   └── MasterPassword.cpp
    ├── storage/
    │   ├── VaultStorage.h
    │   └── VaultStorage.cpp
    └── gui/
        ├── LoginWindow.h
        ├── LoginWindow.cpp
        ├── MainWindow.h
        ├── MainWindow.cpp
        ├── EntryDialog.h
        ├── EntryDialog.cpp
        ├── PasswordViewDialog.h
        └── PasswordViewDialog.cpp