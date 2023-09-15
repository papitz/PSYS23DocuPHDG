FROM alpine:latest

# Set environment variables for non-interactive installation
ENV TZ=UTC

# Install necessary packages
RUN apk update && \
    apk upgrade && \
    apk add --no-cache \
    cmake \
    make \
    g++ \
    git \
    opencv-dev \
    libgomp \
    tzdata && \
    cp /usr/share/zoneinfo/$TZ /etc/localtime && \
    echo $TZ > /etc/timezone && \
    apk del tzdata && \
    rm -rf /var/cache/apk/*
