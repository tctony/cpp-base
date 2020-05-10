load("//tools/bazel:load_tool.bzl", "load_http_archive_deps_if_needed")

load_http_archive_deps_if_needed({
    "rules_foreign_cc": {
        "strip_prefix": "rules_foreign_cc-master",
        "url": "https://github.com/bazelbuild/rules_foreign_cc/archive/master.zip",
    },
})

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

load_http_archive_deps_if_needed(bazel_compile_commands_deps)
