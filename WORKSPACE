workspace(name = "bazel-grpc-proto-cpp-example")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "c5ad277fc21d4899f0e23f6f0337d8a2190d3c66c57ca868378be7c7bfa59fec",
    strip_prefix = "grpc-1.64.1",
    urls = ["https://github.com/grpc/grpc/archive/v1.64.1.tar.gz"],
)

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")

grpc_extra_deps()

http_archive(
    name = "com_github_nelhage_rules_boost",
    sha256 = "6c15542d0d3b6f47cd4d2a1632a3054a0502b5102e471b4453354312cb474407",
    strip_prefix = "rules_boost-5d04542e52164931841d06d5a6b3fd2f43c436d0",
    url = "https://github.com/nelhage/rules_boost/archive/5d04542e52164931841d06d5a6b3fd2f43c436d0.tar.gz",
)

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")

boost_deps()
