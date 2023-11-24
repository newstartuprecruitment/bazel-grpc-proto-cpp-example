FROM ubuntu:23.04 as build
ARG TARGETARCH
ENV PATH=$PATH:/usr/bin:/usr/local/bin

WORKDIR /installs
RUN apt-get update && apt-get -qq install -y default-jre default-jdk
RUN apt-get update && apt-get -qq install -y                                   \
    vim                                                                        \
    sudo                                                                       \
    wget                                                                       \
    curl                                                                       \
    git                                                                        \
    python3                                                                    \
    python-is-python3                                                          \
    pip                                                                        \
    gnupg                                                                      \
    software-properties-common

# Install gcc
ENV GCC_VERSION=13
RUN add-apt-repository ppa:ubuntu-toolchain-r/test                          && \
    apt-get -qq update                                                      && \
    apt-get -qq install -y gcc-${GCC_VERSION} g++-${GCC_VERSION}            && \
    apt-get -qq install -y ca-certificates libgnutls30                      && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${GCC_VERSION} 90  \
                        --slave   /usr/bin/g++ g++ /usr/bin/g++-${GCC_VERSION} && \
    update-alternatives --set gcc /usr/bin/gcc-${GCC_VERSION}

# Install bazel
RUN wget https://github.com/bazelbuild/bazelisk/releases/download/v1.19.0/bazelisk-linux-${TARGETARCH}
RUN cp bazelisk-linux-${TARGETARCH} /usr/local/bin/bazel
RUN chmod a+x /usr/local/bin/bazel
ENV EXTRA_BAZEL_ARGS="--tool_java_runtime_version=local_jdk"
RUN wget https://github.com/bazelbuild/buildtools/releases/download/v6.4.0/buildifier-linux-${TARGETARCH}
RUN cp buildifier-linux-${TARGETARCH} /usr/local/bin/buildifier
RUN chmod a+x /usr/local/bin/buildifier
RUN wget https://github.com/bazelbuild/buildtools/releases/download/v6.4.0/buildozer-linux-${TARGETARCH}
RUN cp buildozer-linux-${TARGETARCH} /usr/local/bin/buildozer
RUN chmod a+x /usr/local/bin/
