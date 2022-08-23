load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "binary_tree",
    srcs = [
        "binary_tree.hpp",
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "binary_tree_test",
    srcs = [
        "binary_tree_test.cpp",
    ],
    deps = [
        ":binary_tree",
        "//external:gtest_main",
    ],
)
