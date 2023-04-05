# File: devcontainer.Dockerfile

FROM mcr.microsoft.com/devcontainers/base:ubuntu

RUN apt-get update \
    && apt-get -qqy install \
        build-essential \
        curl \
        git \
        pipx \
        python-is-python3 \
        python3 \
        software-properties-common

    # bc \
    # sudo \
    # build-essential \
    # ca-certificates \
    # clang \
    # curl \
    # gcc \
    # git \
    # python3 \
    # python3-dev \
    # python3-distutils \
    # python3-pip \
    # python3-setuptools \
    # srecord \
    # udev \
    # xz-utils \

# The correct version of uncrustify is available via the Pybricks PPA
RUN apt-add-repository ppa:pybricks/ppa \
    && apt-get update \
    && apt-get -qqy install \
        uncrustify \
    && apt-get autoremove -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

# Install GNU ARM Embedded Toolchain v10-2020-q4
# Reference: https://lindevs.com/install-arm-gnu-toolchain-on-ubuntu
RUN ARM_TOOLCHAIN_VERSION=$(curl -s https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads | grep -Po '<h4>Version \K.+(?=</h4>)') \
    && curl -Lo gcc-arm-none-eabi.tar.xz "https://developer.arm.com/-/media/Files/downloads/gnu/${ARM_TOOLCHAIN_VERSION}/binrel/arm-gnu-toolchain-${ARM_TOOLCHAIN_VERSION}-x86_64-arm-none-eabi.tar.xz" \
    && sudo mkdir /opt/gcc-arm-none-eabi \
    && tar xf gcc-arm-none-eabi.tar.xz --strip-components=1 -C /opt/gcc-arm-none-eabi \
    && echo 'export PATH=$PATH:/opt/gcc-arm-none-eabi/bin' | sudo tee -a /etc/profile.d/gcc-arm-none-eabi.sh

# EOF
