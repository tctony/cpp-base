load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "rules_foreign_cc",
    strip_prefix = "rules_foreign_cc-master",
    url = "https://github.com/bazelbuild/rules_foreign_cc/archive/master.zip",
)

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
