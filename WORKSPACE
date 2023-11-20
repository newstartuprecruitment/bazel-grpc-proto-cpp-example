workspace(name = "bazel-grpc-proto-cpp-example")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "ea281bb3489520ad4fb96ae84b45ed194a1f0b944d3e74f925f5e019d31ecd0f",
    strip_prefix = "grpc-1.59.3",
    urls = ["https://github.com/grpc/grpc/archive/v1.59.3.tar.gz"],
)

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")

grpc_extra_deps()
