FROM ubuntu:24.10

WORKDIR /project

RUN apt-get update

# Install required tools
RUN apt-get install -y build-essential \ 
    cmake \ 
    swig 

# Install additional tools
RUN apt-get install -y tmux \
    vim 