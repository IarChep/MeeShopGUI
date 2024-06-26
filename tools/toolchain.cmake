# Имя целевой операционной системы
set(CMAKE_SYSTEM_NAME Linux)

# Компиляторы для C и C++
set(CMAKE_C_COMPILER /opt/harmattan-gcc-11/bin/arm-none-linux-gnueabi-gcc)
set(CMAKE_CXX_COMPILER /opt/harmattan-gcc-11/bin/arm-none-linux-gnueabi-g++)

# Местоположение целевой среды
set(CMAKE_FIND_ROOT_PATH /home/user/QtSDK/Madde/sysroots/harmattan_sysroot_10.2011.34-1_slim/)

# Поиск программ в среде хоста
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Поиск библиотек и заголовков в целевой среде
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

