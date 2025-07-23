load("@grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "coroutine",
    srcs = ["coroutine.cc"],
    deps = [
        "@boost//:coroutine",
    ],
)

proto_library(
    name = "hello_proto",
    srcs = ["hello.proto"],
)

cc_proto_library(
    name = "cc_hello_proto",
    deps = [":hello_proto"],
)

cc_grpc_library(
    name = "cc_grpc_hello_proto",
    srcs = [":hello_proto"],
    grpc_only = True,
    deps = [":cc_hello_proto"],
)

cc_binary(
    name = "hello",
    srcs = ["hello.cc"],
    deps = [
    ],
)

cc_binary(
    name = "hello_client",
    srcs = ["hello_client.cc"],
    deps = [
        ":cc_grpc_hello_proto",
        "@grpc//:grpc++",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/flags:parse",
    ],
)

cc_binary(
    name = "hello_server",
    srcs = ["hello_server.cc"],
    defines = ["BAZEL_BUILD"],
    deps = [
        ":cc_grpc_hello_proto",
        "@grpc//:grpc++",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/strings:str_format",
    ],
)

cc_binary(
    name = "hello_server_async",
    srcs = ["hello_server_async.cc"],
    defines = ["BAZEL_BUILD"],
    deps = [
        ":cc_grpc_hello_proto",
        "@grpc//:grpc++",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/strings:str_format",
    ],
)

cc_binary(
    name = "hello_server_async2",
    srcs = ["hello_server_async2.cc"],
    defines = ["BAZEL_BUILD"],
    deps = [
        ":cc_grpc_hello_proto",
        "@grpc//:grpc++",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/strings:str_format",
    ],
)

