load("//tools/bazel:load_tool.bzl", "load_deps_if_needed")
load("//:workspace_deps.bzl", "base_workspace_deps")

load_deps_if_needed(base_workspace_deps)

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

# for openssl
load("@rules_foreign_cc//:workspace_definitions.bzl", "rules_foreign_cc_dependencies")

rules_foreign_cc_dependencies()

local_repository(
    name = "gtest",  # 1.10
    path = "../lib/googletest",
)

local_repository(
    name = "abseil",  # 20200225.1
    path = "../lib/abseil-cpp",
)

local_repository(
    name = "asio",  # 1.16
    path = "../lib/asio",
)

local_repository(
    name = "openssl",  # 1.1.1
    path = "../lib/openssl",
)

local_repository(
    name = "ThreadPool",
    path = "../lib/ThreadPool",
)

load("//tools/bazel_compile_commands:deps.bzl", "bazel_compile_commands_deps")

load_deps_if_needed(bazel_compile_commands_deps)
