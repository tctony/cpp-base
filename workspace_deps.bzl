""""""
base_workspace_deps = {
    "bazel_skylib": {
        "urls": [
            "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.0.2/bazel-skylib-1.0.2.tar.gz",
            "https://github.com/bazelbuild/bazel-skylib/releases/download/1.0.2/bazel-skylib-1.0.2.tar.gz",
        ],
        "sha256": "97e70364e9249702246c0e9444bccdc4b847bed1eb03c5a3ece4f83dfe6abc44",
    },
    "rules_foreign_cc": {
        "strip_prefix": "rules_foreign_cc-74b146dc87d37baa1919da1e8f7b8aafbd32acd9",
        # 2020-05-08
        "url": "https://github.com/bazelbuild/rules_foreign_cc/archive/74b146dc87d37baa1919da1e8f7b8aafbd32acd9.zip",
        "sha256": "2de65ab702ebd0094da3885aae2a6a370df5edb4c9d0186096de79dffb356dbc",
    },
}
