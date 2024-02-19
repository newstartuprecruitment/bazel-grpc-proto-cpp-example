FROM ubuntu:23.04 as build

ARG TARGETARCH

ENV PATH=$PATH:/usr/bin:/usr/local/bin

RUN apt-get update && apt-get -qq install -y                                   \
    wget software-properties-common

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

COPY . gameService

RUN cd gameService && BAZEL_CXXOPTS="-std=c++20" bazel build -c opt //...     \
    --incompatible_strict_action_env                                          \
    --remote_cache=https://storage.googleapis.com/bazel-cache-location --google_credentials=key.json