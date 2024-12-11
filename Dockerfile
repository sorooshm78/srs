FROM ubuntu:22.04


ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /src

RUN apt-get update && apt-get install -y \
    git \
    build-essential \
    libssl-dev \
    uuid-dev \
    cmake \
    libasound2-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*


# RUN git clone --single-branch https://github.com/sorooshm78/srs.git .
COPY . /src

WORKDIR /src/pjproject
RUN ./configure

WORKDIR /src
RUN ./build.sh

CMD [ "./build/srs" ]
