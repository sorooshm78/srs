FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /src

RUN apt-get update && apt-get install -y \
    curl \
    zip \
    unzip \
    tar \
    git \
    build-essential \
    libssl-dev \
    uuid-dev \
    cmake \
    libasound2-dev \
    pkg-config \
    nlohmann-json3-dev \
    && rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/microsoft/vcpkg.git /vcpkg && \
    /vcpkg/bootstrap-vcpkg.sh && \
    /vcpkg/vcpkg install ms-gsl

ENV VCPKG_ROOT=/vcpkg

COPY . /src

RUN git clone https://github.com/pjsip/pjproject.git
WORKDIR /src/pjproject
RUN ./configure

WORKDIR /src
RUN ./build.sh

CMD [ "./build/srs" ]
