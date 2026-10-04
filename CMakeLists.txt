cmake_minimum_required(VERSION 3.16)

project(PasswordManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64")
set(OPENSSL_INCLUDE_DIR "C:/Program Files/OpenSSL-Win64/include")

find_package(OpenSSL REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(PasswordManager
    src/main.cpp
    src/core/PasswordEntry.cpp
    src/core/PasswordManager.cpp
    src/security/PasswordGenerator.cpp
    src/security/PasswordStrength.cpp
    src/security/KeyDerivation.cpp
    src/security/CryptoManager.cpp
    src/security/MasterPassword.cpp
    src/storage/VaultStorage.cpp
    src/gui/LoginWindow.cpp
    src/gui/MainWindow.cpp
    src/gui/EntryDialog.cpp
    src/gui/PasswordViewDialog.cpp
)

target_include_directories(PasswordManager
    PRIVATE
    "${OPENSSL_INCLUDE_DIR}"
)

target_link_libraries(PasswordManager
    PRIVATE
    OpenSSL::Crypto
    Qt6::Widgets
)
