FROM ubuntu:24.04 AS build

ARG TARGETARCH

ENV PATH=$PATH:/usr/bin:/usr/local/bin

RUN apt-get update && apt-get -qq install -y                                   \
    wget software-properties-common g++

# Install bazel
RUN wget https://github.com/bazelbuild/bazelisk/releases/download/v1.26.0/bazelisk-linux-${TARGETARCH}
RUN cp bazelisk-linux-${TARGETARCH} /usr/local/bin/bazel
RUN chmod a+x /usr/local/bin/bazel
ENV EXTRA_BAZEL_ARGS="--tool_java_runtime_version=local_jdk"

