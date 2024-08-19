FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-20.04

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="3.22.2"

# Optionally install the cmake for vcpkg
COPY ./reinstall-cmake.sh /tmp/

RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

RUN sudo add-apt-repository main
RUN sudo add-apt-repository universe
RUN sudo add-apt-repository multiverse
RUN sudo add-apt-repository restricted
    
# Instalar clang-format
RUN sudo apt-get update && apt-get install -y   

RUN wget https://apt.llvm.org/llvm.sh

RUN chmod +x llvm.sh

RUN sudo apt update

RUN sudo apt install lsb-release wget software-properties-common gnupg

RUN sudo ./llvm.sh 18

RUN sudo ln -s /usr/bin/clangd-18 /usr/bin/clangd

RUN sudo apt install clang-format-18

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Uncomment this section to install additional packages.
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>
