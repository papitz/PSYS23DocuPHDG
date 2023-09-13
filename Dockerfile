FROM ubuntu:20.04
# Set environment variables for non-interactive installation
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y \
    cmake \
    g++ \
    git \
    libopencv-dev \
    libomp-dev \
    && apt-get clean
