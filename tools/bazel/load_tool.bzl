"""some load tools"""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def load_deps_if_needed(*deps_list):
    """load http archive deps if needed

    Args:
        *deps_list: list of deps
            deps is dict of http_archives
            deps = {
                "rules_python": {
                   "url": "https://github.com/bazelbuild/rules_python/releases/download/0.0.2/rules_python-0.0.2.tar.gz",
                   "strip_prefix": "rules_python-0.0.2",
                   "sha256": "b5668cde8bb6e3515057ef465a35ad712214962f0b3a314e551204266c7be90c",
                },
            }
    """
    for deps in deps_list:
        for name in deps:
            maybe(http_archive, name, **deps[name])
