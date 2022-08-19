load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "linked_list",
    srcs = [
        "linked_list.cpp",
    ],
    hdrs = [
        "linked_list.hpp",
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "linked_list_test",
    srcs = [
        "linked_list_test.cpp",
    ],
    deps = [
        ":linked_list",
        "//external:gtest_main",
    ],
)
